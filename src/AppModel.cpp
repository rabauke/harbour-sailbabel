#include "AppModel.hpp"
#include <QCoreApplication>
#include <QSettings>
#include <QStandardPaths>


#ifdef SAILJAIL

namespace {

  QString settings_path() {
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" +
           QCoreApplication::applicationName() + ".conf";
  }

}  // namespace

#endif


AppModel::AppModel(QObject* parent) : QObject{parent} {
#ifdef SAILJAIL
  QSettings settings(settings_path(), QSettings::NativeFormat);
#else
  QSettings settings;
#endif

  const QVariant current_dictionary{settings.value("currentDictionary")};
  if (current_dictionary.canConvert<QString>())
    m_current_dictionary = current_dictionary.toString();

  const QVariant auto_load_dictionary{settings.value("autoLoadDictionary")};
  if (auto_load_dictionary.canConvert<bool>())
    m_auto_load_dictionary = auto_load_dictionary.toBool();
}


AppModel::~AppModel() {
#ifdef SAILJAIL
  QSettings settings(settings_path(), QSettings::NativeFormat);
#else
  QSettings settings;
#endif
  settings.setValue("currentDictionary", m_current_dictionary);
  settings.setValue("autoLoadDictionary", m_auto_load_dictionary);
}


QString AppModel::get_current_dictionary() const {
  return m_current_dictionary;
}


void AppModel::set_current_dictionary(const QString& current_dictionary) {
  if (current_dictionary != m_current_dictionary) {
    m_current_dictionary = current_dictionary;
    emit currentDictionaryChanged();
  }
}


bool AppModel::get_auto_load_dictionary() const {
  return m_auto_load_dictionary;
}


void AppModel::set_auto_load_dictionary(bool auto_load_dictionary) {
  if (auto_load_dictionary != m_auto_load_dictionary) {
    m_auto_load_dictionary = auto_load_dictionary;
    emit autoLoadDictionaryChanged();
  }
}
