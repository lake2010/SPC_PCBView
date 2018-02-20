#ifndef ELEMENTLISTMODEL_HPP
#define ELEMENTLISTMODEL_HPP

#include <QObject>
#include <QAbstractListModel>

#include "Element.hpp"

class ElementListModel: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString source READ source WRITE setSource)
public:
    ElementListModel(QObject *parent = nullptr);
    virtual ~ElementListModel();

    Q_INVOKABLE void reload();
    Q_INVOKABLE void remove(int index);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    QString source() const;
    void setSource(const QString& filePath);

Q_SIGNALS:

private:
    Element* m_pElement { nullptr };
};

#endif // ELEMENTLISTMODEL_HPP