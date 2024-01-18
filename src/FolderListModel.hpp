#pragma once

#include <QAbstractListModel>
#include <QDirIterator>
#include <QVector>
#include <QDir>
#include <QFileInfo>


class FolderListModel : public QAbstractListModel {
  Q_OBJECT

public:
  explicit FolderListModel(QObject *parent = nullptr);
  Q_PROPERTY(QString folder READ getFolder WRITE setFolder)

  int rowCount(const QModelIndex &) const override { return m_dir.count(); }
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  QString getFolder() const;
  void setFolder(const QString &name);

private:
  static constexpr int file_name_role = Qt::UserRole;
  static constexpr int file_path_role = Qt::UserRole + 1;
  static constexpr int fileIs_dir_role = Qt::UserRole + 2;

  QDir m_dir;
  QFileInfoList m_file_info_list;
};
