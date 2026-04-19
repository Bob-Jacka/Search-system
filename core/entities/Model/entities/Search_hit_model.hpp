#ifndef SEARCHHITMODEL_H
#define SEARCHHITMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include "SearchHit.hpp"

class SearchHitModel : public QAbstractListModel {
Q_OBJECT

public:
    enum Roles {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        ScoreRole,
        DisplayRole
    };

    explicit SearchHitModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid()) {
            return 0;
        }
        return m_hits.size();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_hits.size()) {
            return QVariant();
        }

        const SearchHit &hit = m_hits[index.row()];

        switch (role) {
            case FileNameRole:
                return QString::fromStdString(hit.file_name);
            case FilePathRole:
                return QString::fromStdString(hit.file_path);
            case ScoreRole:
                return hit.total_score;
            case DisplayRole:
                return QString("%1 [hits: %2]")
                        .arg(QString::fromStdString(hit.file_name))
                        .arg(hit.total_score);
            default:
                return QVariant();
        }
    }

    void setHits(const QList<SearchHit> &hits) {
        beginResetModel();
        m_hits = hits;
        endResetModel();
    }

    void clear() {
        setHits(QList<SearchHit>());
    }

private:
    QList<SearchHit> m_hits;
};

#endif