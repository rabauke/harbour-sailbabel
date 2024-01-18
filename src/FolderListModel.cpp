#include "FolderListModel.hpp"
#include <QDirIterator>


FolderListModel::FolderListModel(QObject *parent)
    : QAbstractListModel{parent}, m_dir{""}, m_file_info_list{m_dir.entryInfoList()} {
}


QHash<int, QByteArray> FolderListModel::roleNames() const {
  static const QHash<int, QByteArray> roles{{file_name_role, "fileName"},
                                            {file_path_role, "filePath"},
                                            {fileIs_dir_role, "fileIsDir"}};
  return roles;
}


QVariant FolderListModel::data(const QModelIndex &index, int role) const {
  if (not index.isValid())
    return {};
  if (role == file_name_role)
    return m_file_info_list[index.row()].fileName();
  if (role == file_path_role)
    return m_file_info_list[index.row()].filePath();
  if (role == fileIs_dir_role)
    return m_file_info_list[index.row()].isDir();
  return {};
}

QString FolderListModel::getFolder() const {
  return m_dir.absolutePath();
}


void FolderListModel::setFolder(const QString &name) {
  beginResetModel();
  QFileInfo test{name};
  if (test.isDir())
    m_dir.cd(name);
  else
    m_dir.cd(m_dir.homePath());
  m_file_info_list = m_dir.entryInfoList();
  endResetModel();
}
