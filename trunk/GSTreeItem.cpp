/// File Name : GSTreeItem.cpp

#include "GSTreeItem.h"
#include "GSTree.h"
#include "GS3DObject.h"

GSTreeItem::GSTreeItem(GSTree* parent, GSObjectType eType)
    : QTreeWidgetItem(parent)
{
    initItem(eType);
}

GSTreeItem::GSTreeItem(GSTreeItem* parent, GSObjectType eType)
    : QTreeWidgetItem(parent)
{
    initItem(eType);
}

GSTreeItem::~GSTreeItem()
{

}

GSObjectType GSTreeItem::getType()
{
    return m_eType;
}

void GSTreeItem::setName(QString name)
{
    setText(0, name);
}

QString GSTreeItem::getName()
{
    return text(0);
}

QString GSTreeItem::getUniqueName()
{
    m_sUniqueName = getName();
    GSTreeItem *pParentItem = dynamic_cast<GSTreeItem *> (parent());
    if (pParentItem)
    {
        m_sUniqueName.append(g_sUniqueNameSpliter + pParentItem->getName());
        GSTreeItem *pGrandparentItem = dynamic_cast<GSTreeItem *> (pParentItem->parent());
        if (pGrandparentItem)
        {
            m_sUniqueName.append(g_sUniqueNameSpliter + pGrandparentItem->getName());
        }
    }
    qDebug() << "uniqueName is " << m_sUniqueName;
    return m_sUniqueName;
}

void GSTreeItem::set3DObject(GS3DObject *p3DObject)
{
    m_p3DObject = p3DObject;
}

GS3DObject *GSTreeItem::get3DObject()
{
    return m_p3DObject;
}

void GSTreeItem::clean3DObject()
{
    m_p3DObject = 0;
}

void GSTreeItem::setCheckState(Qt::CheckState state)
{
    m_state = state;
    QTreeWidgetItem::setCheckState(0, m_state);

    // 更新父节点状态
    GSTreeItem* pParentItem = dynamic_cast<GSTreeItem*> (parent());
    if (pParentItem == 0)
    {
        return;
    }
    if (state == Qt::PartiallyChecked)
    {
        pParentItem->setCheckState(Qt::PartiallyChecked);
        return;
    }

    int selCount = 0;
    for (int i = 0; i < pParentItem->childCount(); i++)
    {
        GSTreeItem *pChildItem = dynamic_cast<GSTreeItem*> (pParentItem->child(i));
        if (pChildItem->checkState(0) == Qt::Checked)
            selCount++;
    }
    if (selCount == 0)
    {
        pParentItem->setCheckState(Qt::Unchecked);
    }
    else if (selCount == pParentItem->childCount())
    {
        pParentItem->setCheckState(Qt::Checked);
    }
    else
    {
        pParentItem->setCheckState(Qt::PartiallyChecked);
    }
}

Qt::CheckState GSTreeItem::previousCheckState()
{
    return m_state;
}

Qt::CheckState GSTreeItem::currentCheckState()
{
    return QTreeWidgetItem::checkState(0);
}

bool GSTreeItem::isCheckStateChanged()
{
    return previousCheckState() != currentCheckState();
}

void GSTreeItem::setItemData(Qt::CheckState state, GS3DObject *p3DObject)
{
    m_state = state;
    QTreeWidgetItem::setCheckState(0, m_state);
    m_p3DObject = p3DObject;
}

void GSTreeItem::initItem(GSObjectType eType)
{
    m_sUniqueName.clear();
    m_state = Qt::Unchecked;
    QTreeWidgetItem::setCheckState(0, m_state);
    m_eType = eType;
    m_p3DObject = 0;
}
