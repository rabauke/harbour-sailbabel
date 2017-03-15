#include "dictionary.hpp"
#include <QStandardPaths>
#include <QFile>
#include <QSet>
#include <QRegularExpression>
#include <QtSql/QSqlQuery>
#include "sqlquerymodel.h"
#include <QQmlEngine>
#include <QUrl>
#include <QDir>
#include <QDebug>

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
  emit dictChanged();
  QFile file(filename);
  if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    throw std::runtime_error("cannot read file");
  lang_A="";
  lang_B="";
  QString line(file.readLine());
  if (line.startsWith('#')){
      /* Checking for type of dictionary
       * assuming the first line of the dictionary contains a string like:
       * langA-langB vocabulary database  compiled by dict.cc
      */
      QRegularExpression re("(?<langA>[A-Z]+)-(?<langB>[A-Z]+)");
      QRegularExpressionMatch langs = re.match(line);
      if(langs.hasMatch()){
//          if(!lang_A.isEmpty() && !lang_B.isEmpty()){
//              qDebug("Warning, overwriting the identities of languages. Are there two comments defining the languages in the dictionary file?");
//          }
          lang_A=langs.captured("langA").toLatin1();
          lang_B=langs.captured("langB").toLatin1();
      }
  }
  if(lang_A.isEmpty() && lang_B.isEmpty()){
      qDebug("Warning, unable to determine languages. Are languages described in the first line of the dictionary file?");
  } else {
      dict_A.clear();
      dict_B.clear();
      map_A.clear();
      map_B.clear();
      QSqlQuery query;
      // delete all old entries with the same languages
      query.exec("DELETE FROM definitions WHERE lang1 = '"+lang_A+"' AND lang2 = '"+lang_B+"'");
      query.exec("DELETE FROM occurrences WHERE langFrom = '"+lang_A+"' AND langTo = '"+lang_B+"'");
      int cnt=0;
      while (!file.atEnd()) {
          cnt++;
          line=file.readLine();
          if (line.startsWith('#'))
              continue;
#if QT_VERSION>=0x050400
          auto line_split=line.splitRef('\t');
#else
          auto line_split=line.split('\t');
#endif
          if (line_split.size()<2)
              continue;
          QString entry_A(line_split[0]);
          QString entry_B(line_split[1]);
          if (entry_A.startsWith("to "))
              entry_A.remove(0, 3);
          if (entry_B.startsWith("to "))
              entry_B.remove(0, 3);
//          qDebug("entryA: "+entry_A.toLatin1());
//          qDebug("entryB: "+entry_B.toLatin1());
//          dict_A.push_back(entry_A.toUtf8());
//          dict_A.back().squeeze();
//          dict_B.push_back(entry_B.toUtf8());
//          dict_B.back().squeeze();
          QString entry_plain_A=purify(entry_A);
          QString entry_plain_B=purify(entry_B);
          QString q=QString("SELECT DID FROM definitions WHERE ")+
                  "definition1 = '"+entry_A.toUtf8()+"' AND lang1 = '"+lang_A+"' AND "+
                  "definition2 = '"+entry_B.toUtf8()+"' AND lang2 = '"+lang_B+"') ";
          query.exec(q);
          int def_id=-1; // assume each word is unique, consider only the first occurrence
          if(query.first()){ //the record exists
              QVariant res=query.value(0);
              def_id=res.toInt();
          } else { //insert a new record
              query.exec("INSERT INTO definitions (definition1, lang1,definition2, lang2) VALUES ('"+entry_A.toUtf8()+"', '"+lang_A+"','"+entry_B.toUtf8()+"', '"+lang_B+"')");
              def_id=query.lastInsertId().toInt();
          }
          if(query.next()){
              qDebug("Warning: duplicate entry for definition "+entry_A.toUtf8()+" and "+entry_B.toUtf8());
          }          
          generateQuery(entry_plain_A,map_A,dict_A,lang_A,lang_A, lang_B,def_id);
          generateQuery(entry_plain_B,map_B,dict_B,lang_B,lang_A, lang_B,def_id);

          QByteArray word_full=entry_A.toUtf8();
          word_full.squeeze();
          QByteArray translation_full=entry_B.toUtf8();
          translation_full.squeeze();
          if (cnt%1000==0)
              emit sizeChanged();          
      }
      emit sizeChanged();
//      dict_A.squeeze();
//      dict_B.squeeze();
//      map_A.squeeze();
//      map_B.squeeze();
//      if (dict_A.empty() || dict_B.empty())
//          throw std::runtime_error("empty dictionary");
  }
}

void dictionary::generateQuery(QString entry,QMultiHash<QByteArray, int> &map,QVector<QByteArray> dict,QString lang,QString langFrom, QString langTo,int def_id) {
#if QT_VERSION>=0x050400
    for (const auto &v: entry.splitRef(' ', QString::SkipEmptyParts)) {
#else
    for (const auto &v: entry.split(' ', QString::SkipEmptyParts)) {
#endif
        QByteArray word=v.toUtf8();
        word.squeeze();
//        map.insert(word, dict.size()-1);
        QString q="SELECT WID FROM words WHERE word = '"+word+"' AND lang = '"+lang+"'";
        QSqlQuery query;
        query.exec(q);
        int word_id=-1; // assume each word is unique, consider only the first occurrence
        if(query.first()){ //the record exists
            QVariant res=query.value(0);
            word_id=res.toInt();
        } else { //insert a new record
            query.exec("INSERT INTO words (word, lang) VALUES ('"+word+"', '"+lang+"')");
            word_id=query.lastInsertId().toInt();
        }
        if(query.next()){
            qDebug("Warning: duplicate entry for word "+word+" and lang "+lang.toLatin1());
        }
        if(def_id!=-1 and word_id!=-1){
            query.exec("INSERT INTO occurrences (langFrom,langTo,wordId,defId) VALUES ('"+langFrom+"','"+langTo+"',"+QString::number(word_id)+","+QString::number(def_id)+")");
        } else {
            qDebug("Warning: invalid indexes");
        }
    }
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

void dictionary::openDB(QUrl offlineStoragePath,QString dbname){
    QDir storageDir(offlineStoragePath.toLocalFile()+"/Databases");
    qDebug() << "Dictionaries found in dir: "<<storageDir.absolutePath();
    qDebug() << storageDir.entryList();
    QString dbPath="";
    for(QString file : storageDir.entryList()){
        if(file.endsWith(".ini"))
        {
            QFile ini(storageDir.absoluteFilePath(file));
            if (not ini.open(QIODevice::ReadOnly | QIODevice::Text))
                continue;
            while(!ini.atEnd())
            {
                QString line = ini.readLine();
                if(line.contains(dbname)) //probably could be better
                {
                    dbPath= storageDir.absoluteFilePath(file).replace(".ini", ".sqlite");
                }
            }

            ini.close();
        }
    }
    qDebug()<<dbPath;
    if(dbPath==""){
        qDebug()<<"Initializing the database";
        emit initDB();
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    if ( db.open ( )) {
        qDebug("connected");
    }
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
