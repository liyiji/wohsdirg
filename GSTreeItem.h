/// File Name : GSTreeItem.h

#ifndef GSTREEITEM_H
#define GSTREEITEM_H

#include <QTreeWidgetItem>

#include "GSPublicDefine.h"

class GSTree;
class GS3DObject;

class GSTreeItem : public QTreeWidgetItem
{
public:
    GSTreeItem(GSTree *parent,
               GSObjectType eType);
    GSTreeItem(GSTreeItem* parent,
               GSObjectType eType);
    virtual ~GSTreeItem();

    GSObjectType getType();

    void setName(QString name);
    QString getUniqueName();

    void set3DObject(GS3DObject *p3DObject);
    GS3DObject *get3DObject();
    void clean3DObject();

    void setCheckState(Qt::CheckState state);
    Qt::CheckState previousCheckState();
    Qt::CheckState currentCheckState();
    bool isCheckStateChanged();

    void setItemData(Qt::CheckState state, GS3DObject *p3DObject);

private:
    void initItem(GSObjectType eType);
    QString getName();

private:
    QString m_sUniqueName;
    Qt::CheckState m_state;
    GSObjectType m_eType;

    GS3DObject *m_p3DObject;
};

#endif // GSTREEITEM_H
