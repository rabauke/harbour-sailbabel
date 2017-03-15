#ifndef SQLQUERYMODEL_H
#define SQLQUERYMODEL_H

#pragma once
#include <QSqlQueryModel>

class SqlQueryModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    explicit SqlQueryModel(QObject *parent = 0);

    void setQuery(const QString &query, const QSqlDatabase &db = QSqlDatabase());
    void setQuery(const QSqlQuery &query);
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const {	return m_roleNames;	}
private:
    void generateRoleNames();
    QHash<int, QByteArray> m_roleNames;
    const QString m_query="SELECT * FROM words w INNER JOIN occurrences o ON o.wordId=w.wid INNER JOIN definitions d ON o.defId = d.did";
};

#endif // SQLQUERYMODEL_H
