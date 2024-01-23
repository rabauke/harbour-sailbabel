#pragma once

#include <QtGlobal>
#include <QObject>
#include <QString>
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
#include <QtQmlIntegration>
#endif
#include "Version.h"


class AppModel : public QObject {
  Q_OBJECT

public:
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
  QML_NAMED_ELEMENT(AppModel)
#endif

  explicit AppModel(QObject* parent = nullptr);
  ~AppModel();

  Q_PROPERTY(QString version MEMBER m_version CONSTANT)
  Q_PROPERTY(QString currentDictionary READ get_current_dictionary WRITE set_current_dictionary
                 NOTIFY currentDictionaryChanged)
  Q_PROPERTY(bool autoLoadDictionary READ get_auto_load_dictionary WRITE
                 set_auto_load_dictionary NOTIFY autoLoadDictionaryChanged)

signals:
  void currentDictionaryChanged();
  void autoLoadDictionaryChanged();

private:
  QString get_current_dictionary() const;
  void set_current_dictionary(const QString& current_dictionary);

  bool get_auto_load_dictionary() const;
  void set_auto_load_dictionary(bool auto_load_dictionary);

  QString m_version{QString::fromStdString(project_version)};
  QString m_current_dictionary;
  bool m_auto_load_dictionary{true};
};
