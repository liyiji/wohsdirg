/// File Name : GSTree.h

#ifndef GSTREE_H
#define GSTREE_H

#include <QTreeWidget>

#include "GS3DObject.h"
#include "GSPublicDefine.h"

class GSTreeItem;
class GSMainWindow;

struct TreeItemState
{
    GSObjectType    m_eType;
    Qt::CheckState  m_checkState;
    QString         m_sUniqueName;
    GS3DObject      *m_p3DObject;
};

class GSTree : public QTreeWidget
{
    Q_OBJECT

public:
    GSTree(GSMainWindow *pMainWindow, QWidget *parent = 0);
    virtual ~GSTree();

public:
    void refreshTree();
    GSTreeItem *findItem(GSObjectType eType,
                         QString m_sUniqueName);

private:
    void saveItemData();
    void saveSingleItemData(GSTreeItem *pItem);
    void restoreItemData();
    void createTree();
    void buildConnection();
    GSTreeItem *findItemInChildren(GSTreeItem *pParentItem,
                                   GSObjectType eType,
                                   QString m_sUniqueName);
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void slotItemClicked(QTreeWidgetItem *pItem, int column);

    void slotCreateSubGridSlice();

    void slotShowCurrentItem();
    void slotHideCurrentItem();
    void slotRemoveCurrentItem();
    void slotRefresh();

private:
    GSMainWindow *m_pMainWindow;
    QVector<TreeItemState> m_vItemState;
    GSTreeItem *m_pCurrentItem;
};

#endif // GSTREE_H
