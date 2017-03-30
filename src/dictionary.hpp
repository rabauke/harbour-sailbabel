#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QByteArray>
#include <QString>
#include <QVector>
#include <QMultiHash>
#include <QVariantList>
#include "sqlquerymodel.h"

class dictionary;
class dictionaryloader;

class dictionary : public QObject {
  Q_OBJECT
  QMultiHash<QByteArray, int> map_A;
  QMultiHash<QByteArray, int> map_B;
  int dicSize;
  int dicProgress;
  int max_num_results=200;
  friend class dictionaryloader;
  QString loadingTitle="default";
  QString loadingSubtitle="default";
  QString coverTitle="default";

public:
  Q_PROPERTY(int size READ size NOTIFY sizeChanged)
  Q_PROPERTY(int progress READ progress NOTIFY sizeChanged)
  Q_PROPERTY(QString loadingTitle READ getTitle NOTIFY sizeChanged)
  Q_PROPERTY(QString loadingSubtitle READ getSubtitle NOTIFY sizeChanged)
  Q_PROPERTY(QString coverTitle READ getCover WRITE setCover NOTIFY sizeChanged)
  explicit dictionary(QObject *parent = 0);
private:
  QString purify(const QString &entry) const;
  void generateQuery(QMultiHash<QByteArray, int> map,QSqlQuery q_ins_word,QSqlQuery q_ins_occ,QString lang,QString langFrom, QString langTo);
  void updateMap(QMultiHash<QByteArray, int> &map,QString entry,int def_id);
public:
  Q_INVOKABLE void read(const QString &filename);
private:
  void read_(const QString &filename);
public:
  int size() const;
  int progress() const;
  QString getTitle() const;
  QString getSubtitle() const;
  QString getCover() const;
  void setCover(QString c);
  virtual ~dictionary() {}
  void openDB(QUrl offlineStoragePath,QString dbname);
signals:
  void sizeChanged();
  void dictChanged();
  void readingFinished();
  void readingError();
  void initDB();
  void initLangs();
public slots:
  void threadFinished();
  void error(QString err);
};

//---------------------------------------------------------------------

class dictionaryloader : public QObject {
  Q_OBJECT
  dictionary &dict;
  QString filename;
public:
  dictionaryloader(dictionary &dict, const QString &filename);
  virtual ~dictionaryloader() { }
public slots:
  void process();
signals:
  void finished();
  void error(QString err);
};

#endif // DICTIONARY_HPP
