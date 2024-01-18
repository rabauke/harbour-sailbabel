#pragma once

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QByteArray>
#include <QString>
#include <QVector>
#include <QMultiHash>
#include <QVariantList>


class Dictionary;
class DictionaryLoader;


class Dictionary : public QObject {
  Q_OBJECT

public:
  explicit Dictionary(QObject *parent = nullptr);
  virtual ~Dictionary() = default;

  Q_PROPERTY(int size READ size NOTIFY sizeChanged)
  Q_PROPERTY(bool dictionaryLoaded READ dictEmpty NOTIFY dictChanged)

  Q_INVOKABLE void read(const QString &filename);
  Q_INVOKABLE QVariantList translateAtoB(const QString &query) const;
  Q_INVOKABLE QVariantList translateBtoA(const QString &query) const;

  int size() const;
  bool dictEmpty() { return m_dict_A.empty() or m_dict_B.empty(); }

signals:
  void sizeChanged();
  void dictChanged();
  void readingFinished();
  void readingError();

  friend class DictionaryLoader;

private:
  void read_(const QString &filename);
  QVariantList translate(const QString &query, const QVector<QByteArray> &dict_A,
                         const QVector<QByteArray> &dict_B,
                         const QMultiHash<QByteArray, int> &map_A) const;
  QString purify(const QString &entry) const;

  QVector<QByteArray> m_dict_A;
  QVector<QByteArray> m_dict_B;
  QMultiHash<QByteArray, int> m_map_A;
  QMultiHash<QByteArray, int> m_map_B;
  static constexpr int max_num_results = 200;
  mutable QMutex m_mutex;
};


class DictionaryLoader : public QObject {
  Q_OBJECT

public:
  DictionaryLoader(Dictionary &dict, const QString &filename);
  virtual ~DictionaryLoader() = default;

signals:
  void finished();
  void error(QString err);

public slots:
  void process();

private:
  Dictionary &m_dict;
  QString m_filename;
};
