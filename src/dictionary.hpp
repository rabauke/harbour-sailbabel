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
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlQuery>

class dictionary;
class dictionaryloader;

class dictionary : public QSqlQueryModel {
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
  QHash<int,QByteArray> *_roleNames;

public:
  Q_PROPERTY(int size READ size NOTIFY sizeChanged)
  Q_PROPERTY(int progress READ progress NOTIFY sizeChanged)
  Q_PROPERTY(QString loadingTitle READ getTitle NOTIFY sizeChanged)
  Q_PROPERTY(QString loadingSubtitle READ getSubtitle NOTIFY sizeChanged)
  Q_PROPERTY(QString coverTitle READ getCover WRITE setCover NOTIFY sizeChanged)
  explicit dictionary(QSqlQueryModel *parent = 0);
private:
  QString purify(const QString &entry) const;
  void generateQuery(QMultiHash<QByteArray, int> map,QSqlQuery q_ins_word,QSqlQuery q_ins_occ,QString lang,QString langFrom, QString langTo);
  void updateMap(QMultiHash<QByteArray, int> &map,QString entry,int def_id);
  QSqlDatabase db;
  const QString m_query="SELECT definition1,definition2 FROM words w INNER JOIN occurrences o ON o.wordId=w.wid INNER JOIN definitions d ON o.defId = d.did WHERE word=?";
public:
  Q_INVOKABLE void read(const QString &filename);
  Q_INVOKABLE void eraseDB();
  Q_INVOKABLE void initDB();
  Q_INVOKABLE void clear(){this->setQuery("select * from words where id=-1");}
  Q_INVOKABLE void search(const QString &term);
private:
  void read_(const QString &filename);
public:
  int size() const;
  int progress() const;
  QString getTitle() const;
  QString getSubtitle() const;
  QString getCover() const;
  void setCover(QString c);
  virtual ~dictionary() {if (db.open()) db.close();}
  QVariant data(const QModelIndex &index, int role) const {
      if(role < Qt::UserRole) {
         return QSqlQueryModel::data(index, role);
      }
      QSqlRecord r = record(index.row());
      return r.value(QString(_roleNames->value(role))).toString();
   }
  inline QHash<int, QByteArray> roleNames() const { return *_roleNames; }
  void updateModel(QString condition);
signals:
  void sizeChanged();
  void readingFinished();
  void readingError();
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
