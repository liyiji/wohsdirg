/// File Name : GSTree.cpp

#include <QDir>
#include <QMenu>
#include <QDebug>
#include <QHeaderView>
#include <QContextMenuEvent>

#include "GSTree.h"
#include "GSView.h"
#include "GS3DGrid.h"
#include "GSTreeItem.h"
#include "GSMainWindow.h"
#include "GS3DGridSlice.h"
#include "GSPublicDefine.h"
#include "GSDialogCreateSubGridSlice.h"

GSTree::GSTree(GSMainWindow *pMainWindow, QWidget *parent) :
        QTreeWidget(parent)
{
    header()->setVisible(false);
    m_pMainWindow = pMainWindow;
    m_pCurrentItem = 0;
}

GSTree::~GSTree()
{

}

void GSTree::refreshTree()
{
    saveItemData();
    createTree();
    restoreItemData();
    buildConnection();
}

GSTreeItem *GSTree::findItem(GSObjectType eType, QString m_sUniqueName)
{
    GSTreeItem *pResultItem = 0;
    for (int i = 0; i < topLevelItemCount(); i++)
    {
        GSTreeItem *pItem = dynamic_cast<GSTreeItem *> (topLevelItem(i));
        if (pItem)
        {
            pResultItem = findItemInChildren(pItem, eType, m_sUniqueName);
            if (pResultItem)
            {
                break;
            }
        }
    }

    return pResultItem;
}

void GSTree::saveItemData()
{
    m_vItemState.clear();
    for (int i = 0; i < topLevelItemCount(); i++)
    {
        GSTreeItem *pItem = dynamic_cast<GSTreeItem *> (topLevelItem(i));
        if (pItem)
        {
            saveSingleItemData(pItem);
        }
    }
}

void GSTree::saveSingleItemData(GSTreeItem *pItem)
{
    if (pItem)
    {
        TreeItemState tis;
        tis.m_eType = pItem->getType();
        tis.m_checkState = pItem->currentCheckState();
        tis.m_sUniqueName = pItem->getUniqueName();
        tis.m_p3DObject = pItem->get3DObject();

        m_vItemState.append(tis);
    }
    for (int i = 0; i < pItem->childCount(); i++)
    {
        GSTreeItem *pChildItem = dynamic_cast<GSTreeItem *> (pItem->child(i));
        if (pChildItem)
        {
            saveSingleItemData(pChildItem);
        }
    }
}

void GSTree::restoreItemData()
{
    for (int i = 0; i < m_vItemState.size(); i++)
    {
        TreeItemState tis = m_vItemState.at(i);
        GSTreeItem *pItem = findItem(tis.m_eType, tis.m_sUniqueName);
        if (pItem)
        {
            pItem->setItemData(tis.m_checkState, tis.m_p3DObject);
        }
    }
}

int g_iTemp = 0;

void GSTree::createTree()
{
    disconnect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
               this, SLOT(slotItemClicked(QTreeWidgetItem*, int)));

    clear();

    QDir dir(g_sDataPath);
    QStringList fileList = dir.entryList();
    fileList.removeAll(".");
    fileList.removeAll("..");
    foreach (QString item, fileList)
    {
        if (item.endsWith("~"))
        {
            fileList.removeAll(item);
        }
    }

    for (int i = 0; i < fileList.size(); i++)
    {
        GSTreeItem *pItem = new GSTreeItem(this, GridType);
        pItem->setName(fileList.at(i));
        insertTopLevelItem(i, pItem);
    }

    bool bDebugAddTestItem = false;
    if (bDebugAddTestItem)
    {
        GSTreeItem *pItem = new GSTreeItem(this, GridType);
        pItem->setName("item" + QString::number(g_iTemp++));
        pItem->setName("itemxxx1");
        pItem->setCheckState(Qt::PartiallyChecked);
        insertTopLevelItem(0, pItem);
        pItem = new GSTreeItem(this, GridType);
        pItem->setName("item" + QString::number(g_iTemp++));
        pItem->setCheckState(Qt::PartiallyChecked);
        insertTopLevelItem(0, pItem);
        pItem = new GSTreeItem(this, GridType);
        pItem->setName("item" + QString::number(g_iTemp++));
        pItem->setName("itemxxx3");
        insertTopLevelItem(0, pItem);
        pItem = new GSTreeItem(this, GridType);
        pItem->setName("item" + QString::number(g_iTemp++));
        pItem->setName("itemxxx4");
        insertTopLevelItem(0, pItem);
        pItem = new GSTreeItem(this, GridType);
        pItem->setName("item" + QString::number(g_iTemp++));
        insertTopLevelItem(0, pItem);
    }
}

void GSTree::buildConnection()
{
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(slotItemClicked(QTreeWidgetItem*, int)));
}

GSTreeItem *GSTree::findItemInChildren(GSTreeItem *pParentItem,
                                       GSObjectType eType,
                                       QString m_sUniqueName)
{
    GSTreeItem *pResultItem = 0;
    if (pParentItem)
    {
        if (pParentItem->getType() == eType)
        {
            if (pParentItem->getUniqueName() == m_sUniqueName)
            {
                pResultItem = pParentItem;
            }
        }
        else
        {
            for (int i = 0; i < pParentItem->childCount(); i++)
            {
                GSTreeItem *pChildItem = dynamic_cast<GSTreeItem *> (pParentItem->child(i));
                if (pChildItem)
                {
                    pResultItem = findItemInChildren(pChildItem, eType, m_sUniqueName);
                    if (pResultItem)
                    {
                        break;
                    }
                }
            }
        }
    }

    return pResultItem;
}

void GSTree::contextMenuEvent(QContextMenuEvent *event)
{
    GSTreeItem *pItem = dynamic_cast<GSTreeItem *> (itemAt(event->pos()));
    QMenu menu(this);
    if (pItem)
    {
        m_pCurrentItem = pItem;
        if (pItem->getType() == GridType)
        {
            if (pItem->checkState(0) != Qt::Unchecked)
                menu.addAction("&Create Sub Slice", this, SLOT(slotCreateSubGridSlice()));
        }
        if (pItem->currentCheckState() == Qt::Checked)
        {
            menu.addAction("&Hide", this, SLOT(slotHideCurrentItem()));
            menu.addAction("&Remove", this, SLOT(slotRemoveCurrentItem()));
        }
        else if (pItem->currentCheckState() == Qt::Unchecked)
        {
            menu.addAction("&Show", this, SLOT(slotShowCurrentItem()));
        }
        else if (pItem->currentCheckState() == Qt::PartiallyChecked)
        {
            menu.addAction("&Show", this, SLOT(slotShowCurrentItem()));
            menu.addAction("&Remove", this, SLOT(slotRemoveCurrentItem()));
        }
    }
    menu.addAction("&Refresh", this, SLOT(slotRefresh()));

    menu.exec(QCursor::pos());
}

void GSTree::slotItemClicked(QTreeWidgetItem *pItem, int column)
{
    qDebug() << pItem << "slotItemClicked";
    GSTreeItem *pGSItem = dynamic_cast<GSTreeItem *> (pItem);
    if (pGSItem == 0)
    {
        return;
    }

    m_pCurrentItem = pGSItem;
    GSView *pView = m_pMainWindow->getView();
    if (pGSItem->isCheckStateChanged())
    {
        qDebug() << pGSItem << "check state changed";
        pGSItem->setCheckState(pGSItem->currentCheckState());
        if (pGSItem->currentCheckState() == Qt::Checked)
        {
            pView->showObject(pGSItem);
        }
        else
        {
            pView->removeObject(pGSItem);
        }
    }

    if (pGSItem->currentCheckState() == Qt::Checked)
    {
        /// TODO : 在pView中选中
    }
    else
    {
        /// TODO : 在pView中选中NULL
    }
}

void GSTree::slotCreateSubGridSlice()
{
    qDebug() << m_pCurrentItem;
    if (m_pCurrentItem == 0)
    {
        return;
    }
    GS3DGrid *pGrid = dynamic_cast<GS3DGrid *> (m_pCurrentItem->get3DObject());
    if (pGrid == 0)
    {
        return;
    }
    GSDialogCreateSubGridSlice dlg(pGrid);
    if (dlg.exec() == QDialog::Accepted)
    {
        QVector<int> selectedI = dlg.getSelectedI();
        QVector<int> selectedJ = dlg.getSelectedJ();
        QVector<int> selectedK = dlg.getSelectedK();
        for (int i = 0; i < selectedI.size(); i++)
        {
//            GS3DGridSlice *pSlice = new GS3DGridSlice()
            /// TODO :
        }
    }
}

void GSTree::slotShowCurrentItem()
{
    GSTreeItem *pItem = dynamic_cast<GSTreeItem *> (currentItem());
    if (pItem == 0)
    {
        return;
    }

    /// TODO :
    qDebug() << "Show" << pItem->getUniqueName();
    GSView *pView = m_pMainWindow->getView();
    pView->showObject(pItem);
}

void GSTree::slotHideCurrentItem()
{
    GSTreeItem *pItem = dynamic_cast<GSTreeItem *> (currentItem());
    if (pItem == 0)
    {
        return;
    }

    /// TODO :
    qDebug() << "Hide" << pItem->getUniqueName();
    GSView *pView = m_pMainWindow->getView();
    pView->hideObject(pItem);
}

void GSTree::slotRemoveCurrentItem()
{
    GSTreeItem *pItem = dynamic_cast<GSTreeItem *> (currentItem());
    if (pItem == 0)
    {
        return;
    }

    /// TODO :
    qDebug() << "Remove" << pItem->getUniqueName();
    GSView *pView = m_pMainWindow->getView();
    pItem->setCheckState(Qt::Unchecked);
    pView->removeObject(pItem);
}

void GSTree::slotRefresh()
{
    refreshTree();
}
