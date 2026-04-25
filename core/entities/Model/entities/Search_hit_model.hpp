#ifndef SEARCHHITMODEL_H
#define SEARCHHITMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include "SearchHit.hpp"

class SearchHitModel : public QAbstractListModel {
Q_OBJECT

public:
    explicit SearchHitModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid()) {
            return 0;
        }
        return m_hits.size();
    }

    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_hits.size()) {
            return {};
        }
        const SearchHit &hit = m_hits[index.row()];

        if (role == Qt::DisplayRole) {
            return QString("Filename: '%1', hits: '%2' in path '%3'")
                    .arg(QString::fromStdString(hit.file_name))
                    .arg(hit.total_score)
                    .arg(hit.file_path);
        }
        return {};
    }

    /**
     * Set model hits with data
     * @param hits hits object
     */
    void setHits(const QList<SearchHit> &hits) {
        beginInsertRows(QModelIndex(), hits.size(), hits.size());
        m_hits = hits;
        endInsertRows();
    }

    void clear() {
        setHits(QList<SearchHit>());
    }

private:
    QList<SearchHit> m_hits;
};

#endif