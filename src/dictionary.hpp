#ifndef DICTIONARY_HPP
#define DICTIONARY_HPP

#include <QObject>
#include <QFile>
#include <QByteArray>
#include <QString>
#include <QVector>
#include <QRegularExpression>
#include <QTextStream>
#include <QSet>
#include <QMultiHash>
#include <algorithm>

class dictionary : public QObject {
  Q_OBJECT
  QVector<QByteArray> dict_A;
  QVector<QByteArray> dict_B;
  QMultiHash<QByteArray, int> map_A;
  QMultiHash<QByteArray, int> map_B;
  int max_num_results=100;
public:
  Q_PROPERTY(int size READ size NOTIFY sizeChanged)
  explicit dictionary(QObject *parent = 0);
private:
  void purify(const QString &entry, QString &plain) const;
public:
  Q_INVOKABLE void read();
  int size() const;
  Q_INVOKABLE QVariantList translateAtoB(const QString &query) const;
  Q_INVOKABLE QVariantList translateBtoA(const QString &query) const;
  virtual ~dictionary() {}
signals:
  void sizeChanged();
public slots:
};

#endif // DICTIONARY_HPP
