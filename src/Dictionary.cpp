#include "Dictionary.hpp"
#include <QtGlobal>
#include <QStandardPaths>
#include <QFile>
#include <QSet>
#include <QRegularExpression>
#include <QMutexLocker>
#include <QDebug>


Dictionary::Dictionary(QObject *parent) : QObject{parent} {
}


void Dictionary::readAsync(const QUrl &filename) {
  QThread *thread{new QThread};
  DictionaryLoader *worker{new DictionaryLoader(*this, filename.toLocalFile())};
  worker->moveToThread(thread);
  connect(worker, &DictionaryLoader::error, this, [this]() { emit readingError(); });
  connect(thread, &QThread::started, worker, &DictionaryLoader::process);
  connect(worker, &DictionaryLoader::finished, thread, &QThread::quit);
  connect(worker, &DictionaryLoader::finished, worker, &DictionaryLoader::deleteLater);
  connect(thread, &QThread::finished, thread, &QThread::deleteLater);
  connect(thread, &QThread::finished, this, [this]() { emit readingFinished(); });
  thread->start();
}


QVariantList Dictionary::translateAtoB(const QString &querry) const {
  return translate(querry, m_dict_A, m_dict_B, m_map_A);
}


QVariantList Dictionary::translateBtoA(const QString &querry) const {
  return translate(querry, m_dict_B, m_dict_A, m_map_B);
}


int Dictionary::size() const {
  QMutexLocker lock(&m_mutex);
  return m_dict_A.size();
}


void Dictionary::read_(const QString &filename) {
  {
    QMutexLocker lock{&m_mutex};
    m_dict_A.clear();
    m_dict_B.clear();
    m_map_A.clear();
    m_map_B.clear();
  }
  emit dictChanged();
  QFile file{filename};
  if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    throw std::runtime_error("cannot read file");
  while (!file.atEnd()) {
    const QString line{file.readLine()};
    if (line.startsWith('#'))
      continue;
    auto line_split{line.splitRef('\t')};
    if (line_split.size() < 2)
      continue;
    QString entry_A;
    entry_A += line_split[0];
    QString entry_B;
    entry_B += line_split[1];
    if (entry_A.startsWith("to "))
      entry_A.remove(0, 3);
    if (entry_B.startsWith("to "))
      entry_B.remove(0, 3);
    {
      QMutexLocker lock{&m_mutex};
      m_dict_A.push_back(entry_A.toUtf8());
      m_dict_A.back().squeeze();
      m_dict_B.push_back(entry_B.toUtf8());
      m_dict_B.back().squeeze();
    }
    QString entry_plain_A{purify(entry_A)};
    QString entry_plain_B{purify(entry_B)};
    const auto words_A{entry_plain_A.splitRef(' ', QString::SkipEmptyParts)};
    for (const auto &v : words_A) {
      QByteArray word{v.toUtf8()};
      word.squeeze();
      QMutexLocker lock{&m_mutex};
      m_map_A.insert(word, m_dict_A.size() - 1);
    }
    const auto words_B{entry_plain_B.splitRef(' ', QString::SkipEmptyParts)};
    for (const auto &v : words_B) {
      QByteArray word{v.toUtf8()};
      word.squeeze();
      QMutexLocker lock{&m_mutex};
      m_map_B.insert(word, m_dict_B.size() - 1);
    }
    if (m_dict_A.size() % 2477 == 0)
      emit sizeChanged();
  }
  emit sizeChanged();
  {
    QMutexLocker lock{&m_mutex};
    m_dict_A.squeeze();
    m_dict_B.squeeze();
    m_map_A.squeeze();
    m_map_B.squeeze();
    if (m_dict_A.empty() or m_dict_B.empty())
      throw std::runtime_error("empty dictionary");
  }
}


QVariantList Dictionary::translate(const QString &querry, const QVector<QByteArray> &dict_A,
                                   const QVector<QByteArray> &dict_B,
                                   const QMultiHash<QByteArray, int> &map_A) const {
  // remove non-letters from query and split into single words
  const QStringList querry_list{purify(querry).split(' ', QString::SkipEmptyParts)};
  // no results if no words in query
  if (querry_list.empty())
    return {};
  // construct intersection of all matches for each single query word
  QSet<int> results;
  {
    auto i{map_A.find(querry_list[0].toUtf8())};
    while (i != map_A.end() and i.key() == querry_list[0]) {
      results.insert(*i);
      ++i;
    }
  }
  for (int k{1}; k < querry_list.size(); ++k) {
    QSet<int> further_results;
    auto i{map_A.find(querry_list[k].toUtf8())};
    while (i != map_A.end() and i.key() == querry_list[k]) {
      further_results.insert(*i);
      ++i;
    }
    results.intersect(further_results);
  }
  // calculate scores for each match
  QVector<int> hits(results.size());
  QVector<int> scores(results.size(), 0);
  std::copy(results.begin(), results.end(), hits.begin());
  // combine query words successfully into a string and check if
  // match contains this string, if yes increase score, in particular,
  // when match starts with this string
  for (int i{0}; i < hits.size(); ++i) {
    QString plain{purify(dict_A[hits[i]])};
    QString prefix{querry_list[0]};
    if (plain.startsWith(prefix)) {
      scores[i] += 6;
      if (QString(dict_A[hits[i]])
              .toCaseFolded()
              .contains(QRegularExpression("^" + prefix + "\\S")))
        scores[i] -= 2;
    } else if (plain.contains(prefix))
      scores[i] += 3;
    for (int k{1}; k < querry_list.size(); ++k) {
      prefix += " ";
      prefix += querry_list[k];
      if (plain.startsWith(prefix))
        scores[i] += 6;
      else if (plain.contains(prefix))
        scores[i] += 3;
    }
    // additional points if there is an exact match
    if (plain == prefix)
      scores[i] += 2;
    // prefer short results
    scores[i] -= plain.count(" ");
  }
  // indirect sort accoring to scores
  QVector<int> indices(results.size());
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(indices.begin(), indices.end(),
            [&](int a, int b) -> bool { return scores[a] > scores[b]; });
  // generate sorted results
  int count{0};
  QVariantList result;
  for (auto i : indices) {
    result.append(QStringList({dict_A[hits[i]], dict_B[hits[i]]}));
    ++count;
    if (count == max_num_results)
      break;
  }
  return result;
}


QString Dictionary::purify(const QString &entry) const {
  QString plain;
  plain.reserve(entry.size());
  bool in_word_mode{true};
  QChar waiting_for;
  for (auto l : entry) {
    if (in_word_mode) {
      if (l.isLetter()) {
        plain.append(l.toCaseFolded());
        continue;
      }
      if (l == '-')
        l = ' ';
      if (l.isSpace() and (not plain.endsWith(' ')) and (not plain.isEmpty())) {
        plain.append(l);
        continue;
      }
      if (l == '(') {
        waiting_for = ')';
        in_word_mode = false;
        continue;
      }
      if (l == '[') {
        waiting_for = ']';
        in_word_mode = false;
        continue;
      }
      if (l == '{') {
        waiting_for = '}';
        in_word_mode = false;
        continue;
      }
      if (l == '<') {
        waiting_for = '>';
        in_word_mode = false;
        continue;
      }
    } else {
      if (l == waiting_for)
        in_word_mode = true;
    }
  }
  if (plain.endsWith(' '))
    plain.chop(1);
  return plain;
}


DictionaryLoader::DictionaryLoader(Dictionary &dict, const QString &filename)
    : m_dict{dict}, m_filename{filename} {
}


void DictionaryLoader::process() {
  try {
    m_dict.read_(m_filename);
    emit finished();
  } catch (...) {
    emit error("unable to read dictionary");
  }
}
