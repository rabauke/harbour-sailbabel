#include "dictionary.hpp"
#include <QStandardPaths>
#include <QFile>
#include <QSet>
#include <QRegularExpression>

dictionary::dictionary(QObject *parent) : QObject(parent) {
}

QString dictionary::purify(const QString &entry) const {
  QString plain;
  plain.reserve(entry.size());
  bool in_word_mode=true;
  QChar waiting_for;
  for (auto l: entry) {
    if (in_word_mode) {
      if (l.isLetter()) {
        plain.append(l.toCaseFolded());
        continue;
      }
      if (l=='-')
        l=' ';
      if (l.isSpace() and (not plain.endsWith(' ')) and (not plain.isEmpty())) {
        plain.append(l);
        continue;
      }
      if (l=='(') {
        waiting_for=')'; in_word_mode=false; continue;
      }
      if (l=='[') {
        waiting_for=']'; in_word_mode=false; continue;
      }
      if (l=='{') {
        waiting_for='}'; in_word_mode=false; continue;
      }
      if (l=='<') {
        waiting_for='>'; in_word_mode=false; continue;
      }
    } else {
      if (l==waiting_for)
        in_word_mode=true;
    }
  }
  if (plain.endsWith(' '))
    plain.chop(1);
  return plain;
}

void dictionary::read(const QString &filename) {
  QThread* thread=new QThread;
  dictionaryloader* worker=new dictionaryloader(*this, filename);
  worker->moveToThread(thread);
  connect(worker, SIGNAL(error(QString)), this, SLOT(error(QString)));
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
  thread->start();
}

void dictionary::read_(const QString &filename) {
  dict_A.clear();
  dict_B.clear();
  map_A.clear();
  map_B.clear();
  emit dictChanged();
  QFile file(filename);
  if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    throw std::runtime_error("cannot read file");
  while (!file.atEnd()) {
    QString line(file.readLine());
    if (line.startsWith('#'))
      continue;
#if QT_VERSION>=0x050400
    auto line_split=line.splitRef('\t');
#else
    auto line_split=line.split('\t');
#endif
    if (line_split.size()<2)
      continue;
#if QT_VERSION>=0x050400
    QString entry_A;
    entry_A+=line_split[0];
    QString entry_B;
    entry_B+=line_split[1];
#else
    QString entry_A(line_split[0]);
    QString entry_B(line_split[1]);
#endif
    if (entry_A.startsWith("to "))
      entry_A.remove(0, 3);
    if (entry_B.startsWith("to "))
      entry_B.remove(0, 3);
    dict_A.push_back(entry_A.toUtf8());
    dict_A.back().squeeze();
    dict_B.push_back(entry_B.toUtf8());
    dict_B.back().squeeze();
    QString entry_plain_A=purify(entry_A);
    QString entry_plain_B=purify(entry_B);
#if QT_VERSION>=0x050400
    for (const auto &v: entry_plain_A.splitRef(' ', QString::SkipEmptyParts)) {
#else
    for (const auto &v: entry_plain_A.split(' ', QString::SkipEmptyParts)) {
#endif
      QByteArray word=v.toUtf8();
      word.squeeze();
      map_A.insert(word, dict_A.size()-1);
    }
#if QT_VERSION>=0x050400
    for (const auto &v: entry_plain_B.splitRef(' ', QString::SkipEmptyParts)) {
#else
    for (const auto &v: entry_plain_B.split(' ', QString::SkipEmptyParts)) {
#endif
      QByteArray word=v.toUtf8();
      word.squeeze();
      map_B.insert(word, dict_B.size()-1);
    }
    if (dict_A.size()%2477==0)
      emit sizeChanged();
  }
  emit sizeChanged();
  dict_A.squeeze();
  dict_B.squeeze();
  map_A.squeeze();
  map_B.squeeze();
  if (dict_A.empty() or dict_B.empty())
    throw std::runtime_error("empty dictionary");
}

int dictionary::size() const {
  int res;
  mutex.lock();
  res=dict_A.size();
  mutex.unlock();
  return res;
}

QVariantList dictionary::translate(const QString &querry,
                                   const QVector<QByteArray> &dict_A,
                                   const QVector<QByteArray> &dict_B,
                                   const QMultiHash<QByteArray, int> &map_A) const {
  // remove non-letters from query and split into single words
  QStringList querry_list=purify(querry).split(' ', QString::SkipEmptyParts);
  // no results if no words in query
  if (querry_list.empty())
    return QVariantList();
  // construct intersection of all matches for each single query word
  QSet<int> results;
  {
    auto i=map_A.find(querry_list[0].toUtf8());
    while (i!=map_A.end() and i.key()==querry_list[0]) {
      results.insert(*i);
      ++i;
    }
  }
  for (int k=1; k<querry_list.size(); ++k) {
    QSet<int> further_results;
    auto i=map_A.find(querry_list[k].toUtf8());
    while (i!=map_A.end() and i.key()==querry_list[k]) {
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
  for (int i=0; i<hits.size(); ++i) {
    QString plain=purify(dict_A[hits[i]]);
    QString prefix=querry_list[0];
    if (plain.startsWith(prefix)) {
      scores[i]+=6;
      if (QString(dict_A[hits[i]]).toCaseFolded().contains(QRegularExpression("^"+prefix+"\\S")))
        scores[i]-=2;
    } else if (plain.contains(prefix))
      scores[i]+=3;
    for (int k=1; k<querry_list.size(); ++k) {
      prefix+=" ";
      prefix+=querry_list[k];
      if (plain.startsWith(prefix))
        scores[i]+=6;
      else if (plain.contains(prefix))
        scores[i]+=3;
    }
    // additional points if there is an exact match
    if (plain==prefix)
      scores[i]+=2;
    // prefer short results
    scores[i]-=plain.count(" ");
  }
  // indirect sort accoring to scores
  QVector<int> indices(results.size());
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(indices.begin(), indices.end(),
            [&](int a, int b) -> bool { return scores[a]>scores[b]; });
  // generate sorted results
  int count=0;
  QVariantList result;
  for (auto i: indices) {
    result.append(QStringList( { dict_A[hits[i]], dict_B[hits[i]] } ));
    ++count;
    if (count==max_num_results)
      break;
  }
  return result;
}

QVariantList dictionary::translateAtoB(const QString &querry) const {
  return translate(querry, dict_A, dict_B, map_A);
}

QVariantList dictionary::translateBtoA(const QString &querry) const {
  return translate(querry, dict_B, dict_A, map_B);
}

void dictionary::threadFinished() {
  emit readingFinished();
}

void dictionary::error(QString) {
  emit readingError();
}


//--------------------------------------------------------------------

dictionaryloader::dictionaryloader(dictionary &dict, const QString &filename) :
  dict(dict),
  filename(filename) {
}

void dictionaryloader::process() {
  try {
    dict.read_(filename);
    emit finished();
  }
  catch (...) {
    emit error("unable to read dictionary");
  }
}
