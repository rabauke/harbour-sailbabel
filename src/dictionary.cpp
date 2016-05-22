#include <QApplication>
#include <QStandardPaths>
#include "dictionary.hpp"

dictionary::dictionary(QObject *parent) : QObject(parent) {
}

void dictionary::purify(const QString &entry, QString &plain) const {
  plain=entry;
  plain.clear();
  bool in_word_mode=true;
  char waiting_for;
  for (auto l: entry) {
    if ( in_word_mode and (l=='(' or l=='[' or l=='{' or l=='<')) {
      in_word_mode=false;
      if (l=='(') waiting_for=')';
      else if (l=='[') waiting_for=']';
      else if (l=='{') waiting_for='}';
      else if (l=='<') waiting_for='>';
      continue;
    }
    if ( (not in_word_mode) and l==waiting_for) {
      in_word_mode=true;
      continue;
    }
    if (in_word_mode)
      plain.append(l);
  }
  static const QRegularExpression spaces_re(R"(([\s]+))",
                                            QRegularExpression::UseUnicodePropertiesOption);
  static const QRegularExpression spaces_end_re(R"(([\s]+$))",
                                                QRegularExpression::UseUnicodePropertiesOption);
  plain.replace(spaces_re, " ");
  plain.replace(spaces_end_re, "");
}

void dictionary::read() {
  dict_A.clear();
  dict_B.clear();
  map_A.clear();
  map_B.clear();

  QFile file(QStandardPaths::locate(QStandardPaths::HomeLocation, "/sailbabel/dictionary.txt"));
  if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    throw std::runtime_error("cannot read file");
  QRegularExpression whole_word_re(R"(([\w]+))",
                                   QRegularExpression::UseUnicodePropertiesOption);
  // whole_word_re.optimize();
  QString entry_plain_A;
  QString entry_plain_B;
  QRegularExpressionMatchIterator i;
  while (!file.atEnd()) {
    auto line=file.readLine();
    if (line.startsWith('#'))
      continue;
    auto line_split=line.split('\t');
    if (line_split.size()<3)
      continue;
    QString entry_A(line_split[0]);
    QString entry_B(line_split[1]);
    if (entry_A.startsWith("to "))
      entry_A.remove(0, 3);
    if (entry_B.startsWith("to "))
      entry_B.remove(0, 3);
    purify(entry_A, entry_plain_A);
    purify(entry_B, entry_plain_B);
    if (entry_plain_A.count(" ")>3 or
        entry_plain_B.count(" ")>3)
      continue;
    dict_A.push_back(entry_A.toUtf8());
    dict_A.back().squeeze();
    i=whole_word_re.globalMatch(entry_plain_A);
    while (i.hasNext()) {
      QByteArray word=i.next().captured(0).toUtf8();
      word.squeeze();
      map_A.insert(word, dict_A.size()-1);
      // map_A.insert(i.next().captured(0).toUtf8(), dict_A.size()-1);
    }
    dict_B.push_back(entry_B.toUtf8());
    dict_B.back().squeeze();
    i=whole_word_re.globalMatch(entry_plain_B);
    while (i.hasNext()) {
      QByteArray word=i.next().captured(0).toUtf8();
      word.squeeze();
      map_B.insert(word, dict_B.size()-1);
      // map_B.insert(i.next().captured(0).toUtf8(), dict_B.size()-1);
    }
    if (dict_A.size()%1987==0) {
      emit sizeChanged();
      QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }
//    if (dict_A.size()>50000)
//      break;
  }
  QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
  dict_A.squeeze();
  QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
  dict_B.squeeze();
  QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
  map_A.squeeze();
  QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
  map_B.squeeze();
}

int dictionary::size() const {
  return dict_A.size();
}

QVariantList dictionary::translateAtoB(const QString &querry) const {
  QStringList querry_list=querry.split(' ', QString::SkipEmptyParts);
  if (querry_list.empty())
    return QVariantList();
  QVector<QSet<int> > results(std::min(querry_list.size(), 2));
  for (int k=0; k<1; ++k) {
    auto i=map_A.find(querry_list[k].toUtf8());
    while (i!=map_A.end() and i.key()==querry_list[k]) {
      results[k].insert(*i);
      ++i;
    }
  }
  for (int k=1; k<querry_list.size(); ++k) {
    auto i=map_A.find(querry_list[k].toUtf8());
    results[1].clear();
    while (i!=map_A.end() and i.key()==querry_list[k]) {
      results[1].insert(*i);
      ++i;
    }
    results[0].intersect(results[1]);
  }
  QVector<int> hits(results[0].size());
  QVector<int> scores(results[0].size(), 0);
  std::copy(results[0].begin(), results[0].end(), hits.begin());
  for (int i=0; i<hits.size(); ++i) {
    QString plain;
    purify(dict_A[hits[i]], plain);
    QString prefix=querry_list[0];
    if (plain.startsWith(prefix))
      scores[i]+=10;
    for (int k=1; k<querry_list.size(); ++k) {
      prefix+=" "+querry_list[k];
      if (plain.startsWith(prefix))
        scores[i]+=10;
      if (plain.contains(prefix))
        scores[i]+=5;
    }
    if (plain==prefix)
      scores[i]+=5;
  }
  QVector<int> indices(results[0].size());
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(indices.begin(), indices.end(),
            [&](int a, int b) -> bool { return scores[a]>scores[b]; });
  int count=0;
  QVariantList result;
  for (auto i: indices) {
    result.append(QStringList({dict_A[hits[i]], dict_B[hits[i]]}));
    ++count;
    if (count==max_num_results)
      break;
  }
  return result;
}

QVariantList dictionary::translateBtoA(const QString &querry) const {
  QStringList querry_list=querry.split(' ', QString::SkipEmptyParts);
  if (querry_list.empty())
    return QVariantList();
  QVector<QSet<int> > results(std::min(querry_list.size(), 2));
  for (int k=0; k<1; ++k) {
    auto i=map_B.find(querry_list[k].toUtf8());
    while (i!=map_B.end() and i.key()==querry_list[k]) {
      results[k].insert(*i);
      ++i;
    }
  }
  for (int k=1; k<querry_list.size(); ++k) {
    auto i=map_B.find(querry_list[k].toUtf8());
    results[1].clear();
    while (i!=map_B.end() and i.key()==querry_list[k]) {
      results[1].insert(*i);
      ++i;
    }
    results[0].intersect(results[1]);
  }
  QVector<int> hits(results[0].size());
  QVector<int> scores(results[0].size(), 0);
  std::copy(results[0].begin(), results[0].end(), hits.begin());
  for (int i=0; i<hits.size(); ++i) {
    QString plain;
    purify(dict_B[hits[i]], plain);
    QString prefix=querry_list[0];
    if (plain.startsWith(prefix))
      scores[i]+=10;
    for (int k=1; k<querry_list.size(); ++k) {
      prefix+=" "+querry_list[k];
      if (plain.startsWith(prefix))
        scores[i]+=10;
      if (plain.contains(prefix))
        scores[i]+=5;
    }
    if (plain==prefix)
      scores[i]+=5;
  }
  QVector<int> indices(results[0].size());
  std::iota(indices.begin(), indices.end(), 0);
  std::sort(indices.begin(), indices.end(),
            [&](int a, int b) -> bool { return scores[a]>scores[b]; });
  int count=0;
  QVariantList result;
  for (auto i: indices) {
    result.append(QStringList({dict_B[hits[i]], dict_A[hits[i]]}));
    ++count;
    if (count==max_num_results)
      break;
  }
  return result;
}
