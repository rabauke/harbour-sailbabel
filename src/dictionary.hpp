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

class dictionary;
class dictionaryloader;

class dictionary : public QObject {
  Q_OBJECT
  QVector<QByteArray> dict_A;
  QVector<QByteArray> dict_B;
  QMultiHash<QByteArray, int> map_A;
  QMultiHash<QByteArray, int> map_B;
  int max_num_results=200;
  mutable QMutex mutex;

  friend class dictionaryloader;

public:
  Q_PROPERTY(int size READ size NOTIFY sizeChanged)
  explicit dictionary(QObject *parent = 0);
  Q_PROPERTY(bool dictionaryLoaded READ dictEmpty NOTIFY dictChanged)
private:
  QString purify(const QString &entry) const;
public:
  Q_INVOKABLE void read(const QString &filename);
private:
  void read_(const QString &filename);
  QVariantList translate(const QString &query,
                         const QVector<QByteArray> &dict_A,
                         const QVector<QByteArray> &dict_B,
                         const QMultiHash<QByteArray, int> &map_A) const;
public:
  int size() const;
  bool dictEmpty(){return dict_A.empty() || dict_B.empty();}
  Q_INVOKABLE QVariantList translateAtoB(const QString &query) const;
  Q_INVOKABLE QVariantList translateBtoA(const QString &query) const;
  virtual ~dictionary() {}
signals:
  void sizeChanged();
  void dictChanged();
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
