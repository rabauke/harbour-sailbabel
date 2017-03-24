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
    dicSize=0;
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
    map_A.clear();
    map_B.clear();
  QFile file(filename);
  if (not file.open(QIODevice::ReadOnly | QIODevice::Text))
    throw std::runtime_error("cannot read file");
  QString lang_A="";
  QString lang_B="";
  QSqlDatabase::database().transaction();
  // Improve performance significantly by doing all operations in memory
  QSqlQuery("PRAGMA journal_mode = OFF");
  QSqlQuery("PRAGMA synchronous = OFF");
  QSqlQuery q_del_def,q_del_occ,q_sel_def,q_ins_def,q_sel_word,q_ins_word,q_ins_occ;
  q_del_def.prepare("DELETE FROM definitions WHERE lang1 = ? AND lang2 = ?");
  q_del_occ.prepare("DELETE FROM occurrences WHERE langFrom = ? AND langTo = ?");
  q_sel_def.prepare("SELECT DID FROM definitions WHERE definition1 = ? AND lang1 = ? AND definition2 = ? AND lang2 = ?");
  q_ins_def.prepare("INSERT INTO definitions (definition1, lang1,definition2, lang2) VALUES (?,?,?,?)");
  q_sel_word.prepare("SELECT WID FROM words WHERE word = ? AND lang = ?");
  q_ins_word.prepare("INSERT INTO words (word, lang) VALUES (?, ?)");
  q_ins_occ.prepare("INSERT INTO occurrences (langFrom,langTo,wordId,defId) VALUES (?,?,?,?)");
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
  int cnt=0;
  if(lang_A.isEmpty() && lang_B.isEmpty()){
      qDebug("Warning, unable to determine languages. Are languages described in the first line of the dictionary file?");
  } else {
      dicSize=0;
      // delete all old entries with the same languages
      q_del_def.bindValue(0,lang_A);
      q_del_def.bindValue(1,lang_B);
      q_del_def.exec();
      q_del_occ.bindValue(0,lang_A);
      q_del_occ.bindValue(1,lang_B);
      q_del_occ.exec();
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
          QString entry_plain_A=purify(entry_A);
          QString entry_plain_B=purify(entry_B);
          q_ins_def.bindValue(0,entry_A);
          q_ins_def.bindValue(1,lang_A);
          q_ins_def.bindValue(2,entry_B);
          q_ins_def.bindValue(3,lang_B);
          if(!q_ins_def.exec())
              qDebug()<<"Failure";
          int def_id=q_ins_def.lastInsertId().toInt();
          updateMap(map_A,entry_plain_A,def_id);
          updateMap(map_B,entry_plain_B,def_id);
          if (cnt%100==0){
              dicSize=cnt;
              emit sizeChanged();
          }
      }
      // insert words and associations
      generateQuery(map_A,q_ins_word,q_ins_occ, lang_A, lang_A,lang_B);
      generateQuery(map_B,q_ins_word,q_ins_occ, lang_B, lang_A,lang_B);
      QSqlDatabase::database().commit();
      emit sizeChanged();
      emit initLangs();
  }
}

void dictionary::updateMap(QMultiHash<QByteArray, int> &map,QString entry,int def_id){
#if QT_VERSION>=0x050400
    for (const auto &v: entry.splitRef(' ', QString::SkipEmptyParts)) {
#else
    for (const auto &v: entry.split(' ', QString::SkipEmptyParts)) {
#endif
        QByteArray w=v.toUtf8();
        w.squeeze();
        map.insert(w, def_id);
    }
}

void dictionary::generateQuery(QMultiHash<QByteArray, int> map,QSqlQuery q_ins_word,QSqlQuery q_ins_occ,
                               QString lang,QString langFrom, QString langTo) {
    QString lastKey="";
    QList<int> values;
    int word_id=-1;
    for (QMultiHash<QByteArray, int>::iterator i = map.begin(); i != map.end(); ++i){
        QString k=i.key();
        int v=i.value();
        if(k!=lastKey){ // add a new word
            lastKey=k;
            values.clear();
            // insert word
            q_ins_word.bindValue(0,k);
            q_ins_word.bindValue(1,lang);
            if(!q_ins_word.exec())
                qDebug()<<"Failure";
            word_id=q_ins_word.lastInsertId().toInt();
        }
        if(!values.contains(v)){ // not a duplicate
            values.append(v); //add to the list
            // insert occurrence
            if(v!=-1 and word_id!=-1){
                q_ins_occ.bindValue(0,langFrom);
                q_ins_occ.bindValue(1,langTo);
                q_ins_occ.bindValue(2,word_id);
                q_ins_occ.bindValue(3,v);
                if(!q_ins_occ.exec())
                    qDebug()<<"Failure";
            } else {
                qDebug("Warning: invalid indexes");
            }
        } // if the entry is duplicated, do nothing
    }
}

int dictionary::size() const {
  return dicSize;
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
        emit initLangs();
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
