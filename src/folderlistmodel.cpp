#include "folderlistmodel.hpp"
#include <QDirIterator>

FolderListModel::FolderListModel(QObject *parent) :
  QAbstractListModel(parent),
  dir(""),
  fileInfoList(dir.entryInfoList()) {
}

QHash<int, QByteArray> FolderListModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[FileNameRole]="fileName";
  roles[FilePathRole]="filePath";
  roles[FileIsDirRole]="fileIsDir";
  return roles;
}

QVariant FolderListModel::data(const QModelIndex &index, int role) const {
  if (not index.isValid())
    return QVariant();
  if (role==FileNameRole)
    return QVariant(fileInfoList[index.row()].fileName());
  if (role==FilePathRole)
    return QVariant(fileInfoList[index.row()].filePath());
  if (role==FileIsDirRole)
    return QVariant(fileInfoList[index.row()].isDir());
  return QVariant();
}

QString FolderListModel::getFolder() const {
  return dir.absolutePath();
}

void FolderListModel::setFolder(const QString &name) {
  beginResetModel();
  QFileInfo test(name);
  if (test.isDir())
    dir.cd(name);
  else
    dir.cd(dir.homePath());
  fileInfoList=dir.entryInfoList();
  endResetModel();
}
