#ifndef FOLDERLISTMODEL_HPP
#define FOLDERLISTMODEL_HPP

#include <QAbstractListModel>
#include <QDirIterator>
#include <QVector>
#include <QDir>
#include <QFileInfo>

class FolderListModel : public QAbstractListModel {
  Q_OBJECT
  QDir dir;
  QFileInfoList fileInfoList;

public:
  Q_PROPERTY(QString folder READ getFolder WRITE setFolder)
  enum FolderListModelRoles { FileNameRole = Qt::UserRole + 1, FilePathRole, FileIsDirRole };
  explicit FolderListModel(QObject *parent = 0);
  virtual int rowCount(const QModelIndex &) const { return dir.count(); }
  virtual QVariant data(const QModelIndex &index, int role) const;
  QHash<int, QByteArray> roleNames() const;

  QString getFolder() const;
  void setFolder(const QString &name);

signals:

public slots:
};

#endif
