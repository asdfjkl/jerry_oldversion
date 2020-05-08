#ifndef DATABASE_INDEX_MODEL_H
#define DATABASE_INDEX_MODEL_H

#include <QAbstractTableModel>
#include "database/database.h"

const int COLUMN_COUNT = 6;

class DatabaseIndexModel : public QAbstractTableModel
{
public:
    DatabaseIndexModel(QObject *parent = 0);

    void setDatabase(Database *database);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    Database *database;

};

#endif // DATABASE_INDEX_MODEL_H
