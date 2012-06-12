/// File Name : GSView.cpp

#include <QString>

#include <Inventor/SoPickedPoint.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>
#include <Inventor/Qt/SoQtDirectionalLightEditor.h>
#include <Inventor/events/SoLocation2Event.h>
#include <Inventor/events/SoMouseButtonEvent.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoTransparencyType.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>

#include "GSView.h"
#include "GS3DGrid.h"
#include "GSTreeItem.h"
#include "GS3DObject.h"
#include "GSSelection.h"
#include "GSMainWindow.h"
#include "GSPublicDefine.h"
#include "GSPublicFunctions.h"

void MouseMoveEventCB(void *userData, SoEventCallback *eventCB)
{
    GSView *pView = (GSView *) userData;
    SoSeparator *pRoot = pView->getRoot();

    SbVec3f currentPos = pView->findPosition(pRoot, eventCB);
    if (currentPos != g_invalidCoord)
    {
        pView->showCoordInStatusBar(currentPos);
    }
}

void MouseClickEventCB(void *userData, SoEventCallback *eventCB)
{
    /// TODO :
}

GSView::GSView(GSMainWindow* pMainWindow, QWidget *parent) : QWidget(parent)
{
    m_pMainWindow           = 0;
    m_pRootSelection        = 0;
    m_v3DObject.clear();

    m_pRoot                 = 0;
    m_pViewer               = 0;
    m_pCamera3D             = 0;
    m_pMouseEventCallBack   = 0;
    m_pAllTransform         = 0;
    m_pScale3Direction      = 0;

    m_pTopLight             = 0;
    m_pBottomLight          = 0;
    m_pLeftLight            = 0;
    m_pRightLight           = 0;
    m_pFrontLight           = 0;
    m_pEndLight             = 0;
    m_pMainLightEditor      = 0;
    m_pTopLightEditor       = 0;
    m_pBottomLightEditor    = 0;
    m_pLeftLightEditor      = 0;
    m_pRightLightEditor     = 0;
    m_pFrontLightEditor     = 0;
    m_pEndLightEditor       = 0;

    m_pMainWindow = pMainWindow;

    m_pViewer = new SoQtExaminerViewer(this);
    m_pViewer->setAnimationEnabled(false);
    m_pViewer->setDecoration(true);
    m_pViewer->setTransparencyType(SoGLRenderAction::SORTED_OBJECT_BLEND);
    m_pViewer->setBackgroundColor(getSbColorFromQColor(Qt::black));
    QWidget *viewer = m_pViewer->getWidget();
    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->setMargin(0);
    vbox->setSpacing(0);
    vbox->addWidget(viewer);

    m_pCamera3D = new SoPerspectiveCamera;
    m_pCamera3D->position.setValue(1, 0, 0);
    m_pCamera3D->nearDistance.setValue(0.001);
    m_pCamera3D->farDistance.setValue(100000.0);

    m_pRootSelection = new GSSelection;
    m_pRootSelection->ref();
    m_pRootSelection->policy = SoSelection::SINGLE;
    m_pRootSelection->addChild(m_pCamera3D);
    m_pRootSelection->addSelectionCallback(GSSelection::SelectObject, this);
    m_pRootSelection->addDeselectionCallback(GSSelection::DeselectObject, this);

#ifdef CODING
    bool bAddSmallSphereForDebug = false;
    if (bAddSmallSphereForDebug)
    {
        SoSeparator* pSepCenter = new SoSeparator;
        SoSphere* pSphereCenter = new SoSphere;
        pSphereCenter->radius.setValue(100);
        pSepCenter->addChild(pSphereCenter);
        m_pRootSelection->addChild(pSepCenter);

        SoSeparator* pSepX = new SoSeparator;
        SoSphere* pSphereX = new SoSphere;
        pSphereX->radius.setValue(100);
        SoMaterial* pMaterialX = new SoMaterial;
        SoTransform* pTransX = new SoTransform;
        pTransX->translation.setValue(1000, 0, 0);
        pMaterialX->diffuseColor.setValue(1, 0, 0);
        pSepX->addChild(pTransX);
        pSepX->addChild(pMaterialX);
        pSepX->addChild(pSphereX);
        m_pRootSelection->addChild(pSepX);

        SoSeparator* pSepY = new SoSeparator;
        SoSphere* pSphereY = new SoSphere;
        pSphereY->radius.setValue(100);
        SoMaterial* pMaterialY = new SoMaterial;
        SoTransform* pTransY = new SoTransform;
        pTransY->translation.setValue(0, 1000, 0);
        pMaterialY->diffuseColor.setValue(0, 1, 0);
        pSepY->addChild(pTransY);
        pSepY->addChild(pMaterialY);
        pSepY->addChild(pSphereY);
        m_pRootSelection->addChild(pSepY);

        SoSeparator* pSepZ = new SoSeparator;
        SoSphere* pSphereZ = new SoSphere;
        pSphereZ->radius.setValue(100);
        SoMaterial* pMaterialZ = new SoMaterial;
        SoTransform* pTransZ = new SoTransform;
        pTransZ->translation.setValue(0, 0, 1000);
        pMaterialZ->diffuseColor.setValue(0, 0, 1);
        pSepZ->addChild(pTransZ);
        pSepZ->addChild(pMaterialZ);
        pSepZ->addChild(pSphereZ);
        m_pRootSelection->addChild(pSepZ);
    }
#endif

    m_pViewer->setCameraSceneGraph(m_pRootSelection);
    m_pViewer->setSceneGraph(m_pRootSelection);
    m_pViewer->redrawOnSelectionChange(m_pRootSelection);

    m_pMouseEventCallBack = new SoEventCallback;
    /// TODO :
    m_pMouseEventCallBack->addEventCallback(SoLocation2Event::getClassTypeId(), MouseMoveEventCB, this);
    m_pMouseEventCallBack->addEventCallback(SoMouseButtonEvent::getClassTypeId(), MouseClickEventCB, this);
    m_pRootSelection->addChild(m_pMouseEventCallBack);

    m_pRoot = new SoSeparator;
    m_pRoot->ref();
    m_pRoot->addChild(m_pRootSelection);

    m_pAllTransform = new SoTransform;
    m_pRootSelection->addChild(m_pAllTransform);

    SoTransparencyType* pTT = new SoTransparencyType;
    pTT->type.setValue(SoTransparencyType::SORTED_OBJECT_BLEND);
    m_pRootSelection->addChild(pTT);

    m_pMainLightEditor = new SoQtDirectionalLightEditor(this);
    m_pMainLightEditor->setTitle("Head Light Editor");
    SoPath *pPath = new SoPath(m_pViewer->getHeadlight());
    pPath->ref();
    m_pMainLightEditor->attach(pPath);
    pPath->unref();

    m_pTopLight = new SoDirectionalLight;
    m_pTopLight->direction.setValue(0, -1, 0);
    m_pTopLight->color.setValue(1, 1, 1);
    m_pTopLight->on = false;
    m_pTopLightEditor = new SoQtDirectionalLightEditor(this);
    m_pTopLightEditor->setTitle("Top Light Editor");
    pPath = new SoPath(m_pTopLight);
    pPath->ref();
    m_pTopLightEditor->attach(pPath);
    pPath->unref();

    m_pBottomLight = new SoDirectionalLight;
    m_pBottomLight->direction.setValue(0, 1, 0);
    m_pBottomLight->color.setValue(1, 1, 1);
    m_pBottomLight->on = false;
    m_pBottomLightEditor = new SoQtDirectionalLightEditor(this);
    m_pBottomLightEditor->setTitle("Bottom Light Editor");
    pPath = new SoPath(m_pBottomLight);
    pPath->ref();
    m_pBottomLightEditor->attach(pPath);
    pPath->unref();

    m_pLeftLight = new SoDirectionalLight;
    m_pLeftLight->direction.setValue(1, 0, 0);
    m_pLeftLight->color.setValue(1, 1, 1);
    m_pLeftLight->on = false;
    m_pLeftLightEditor = new SoQtDirectionalLightEditor(this);
    m_pLeftLightEditor->setTitle("Left Light Editor");
    pPath = new SoPath(m_pLeftLight);
    pPath->ref();
    m_pLeftLightEditor->attach(pPath);
    pPath->unref();

    m_pRightLight = new SoDirectionalLight;
    m_pRightLight->direction.setValue(-1, 0, 0);
    m_pRightLight->color.setValue(1, 1, 1);
    m_pRightLight->on = false;
    m_pRightLightEditor = new SoQtDirectionalLightEditor(this);
    m_pRightLightEditor->setTitle("Right Light Editor");
    pPath = new SoPath(m_pRightLight);
    pPath->ref();
    m_pRightLightEditor->attach(pPath);
    pPath->unref();

    m_pFrontLight = new SoDirectionalLight;
    m_pFrontLight->direction.setValue(0, 0, 1);
    m_pFrontLight->color.setValue(1, 1, 1);
    m_pFrontLight->on = false;
    m_pFrontLightEditor = new SoQtDirectionalLightEditor(this);
    m_pFrontLightEditor->setTitle("Front Light Editor");
    pPath = new SoPath(m_pFrontLight);
    pPath->ref();
    m_pFrontLightEditor->attach(pPath);
    pPath->unref();

    m_pEndLight = new SoDirectionalLight;
    m_pEndLight->direction.setValue(0, 0, -1);
    m_pEndLight->color.setValue(1, 1, 1);
    m_pEndLight->on = false;
    m_pEndLightEditor = new SoQtDirectionalLightEditor(this);
    m_pEndLightEditor->setTitle("End Light Editor");
    pPath = new SoPath(m_pEndLight);
    pPath->ref();
    m_pEndLightEditor->attach(pPath);
    pPath->unref();

    m_pRootSelection->addChild(m_pTopLight);
    m_pRootSelection->addChild(m_pBottomLight);
    m_pRootSelection->addChild(m_pLeftLight);
    m_pRootSelection->addChild(m_pRightLight);
    m_pRootSelection->addChild(m_pFrontLight);
    m_pRootSelection->addChild(m_pEndLight);

#ifdef UNFINISHED
    {
        m_Compass.createCompass(0);
        m_Compass.createCompass(1, g_sResoucePath + "defaultCompass.iv");

        m_Compass.m_pCompassSwitch->whichChild = 0;
        m_pRootSelection->addChild(m_Compass.m_pCompassSwitch);
        m_pViewer->setSceneGraph(m_pRoot);
        m_pCamera3D->viewAll(m_pRoot, m_pViewer->getViewportRegion());
    }
#endif

    SoBoxHighlightRenderAction *pHighLightAction = new SoBoxHighlightRenderAction;
    pHighLightAction->setLineWidth(1);
    m_pViewer->setGLRenderAction(pHighLightAction);

    m_pScale3Direction = new SoTransform;
    m_pScale3Direction->scaleFactor.setValue(1.0, 1.0, 1.0);
    m_pScale3Direction->ref();
}

GSView::~GSView()
{
    m_pScale3Direction->unref();
    m_pRootSelection->unref();
    m_pRoot->unref();
}

void GSView::showObject(GSTreeItem *pItem)
{
    if (pItem == 0)
    {
        return;
    }

    BUSY_CURSOR;

    GS3DObject *p3DObject = pItem->get3DObject();

    if (p3DObject)
    {
        p3DObject->show();
        pItem->setCheckState(Qt::Checked);
    }
    else
    {
//        QStringList stl = uniqueName.split(g_sUniqueNameSpliter);
//        QString name, parentName, grandparentName;
//        if (stl.size())
//        {
//            name = stl.at(0);
//            if (stl.size() > 1)
//            {
//                parentName = stl.at(1);
//                if (stl.size() > 2)
//                {
//                    grandparentName = stl.at(2);
//                }
//            }
//        }
        GSObjectType eType = pItem->getType();
        QString uniqueName = pItem->getUniqueName();

        switch (eType)
        {
        case GridType:
            {
                /// TODO :
                GS3DGrid *p3DGrid = new GS3DGrid(uniqueName, get3DParameters());
                m_pRootSelection->addChild(p3DGrid->getOIVNode());
                p3DGrid->refreshObject();
                QList<QColor> cm; /// TODO :
                p3DGrid->changeColorMap(cm);

                pItem->set3DObject(p3DGrid);
                pItem->setCheckState(Qt::Checked);
                break;
            }
        case GridGroupType:
            {
                break;
            }
        default:
            {
                break;
            }
        }
    }
    RESTORE_CURSOR;
}

void GSView::removeObject(GSTreeItem *pItem)
{
    if (pItem == 0)
    {
        return;
    }
    GS3DObject *p3DObject = pItem->get3DObject();
    if (p3DObject == 0)
    {
        return;
    }

    m_pRootSelection->removeChild(p3DObject->getOIVNode());
    delete p3DObject;
    pItem->clean3DObject();
    /// TODO :
}

void GSView::hideObject(GSTreeItem *pItem)
{
    if (pItem == 0)
    {
        return;
    }
    GS3DObject *p3DObject = pItem->get3DObject();
    if (p3DObject == 0)
    {
        return;
    }

    p3DObject->hide();
    pItem->setCheckState(Qt::PartiallyChecked);
    /// TODO :
}

GS3DObjectParameters GSView::get3DParameters()
{
    GS3DObjectParameters para;
    SbVec3f scaleFactor = m_pScale3Direction->scaleFactor.getValue();
    para.xScale = scaleFactor[0];
    para.yScale = scaleFactor[1];
    para.zScale = scaleFactor[2];
    debug(scaleFactor, "Scale Factor is :");
    SbVec3f transform(0, 0, 0); /// TODO :
    para.xTrans = transform[0];
    para.yTrans = transform[1];
    para.zTrans = transform[2];
    debug(transform, "Transform is :");
    return para;
}

void GSView::viewAll()
{
    m_pViewer->viewAll();
}

SoSeparator *GSView::getRoot()
{
    return m_pRoot;
}

SbVec3f GSView::findPosition(SoSeparator *pRoot, SoEventCallback *eventCB)
{
    const SoEvent *event = eventCB->getEvent();
    const SbViewportRegion &myRegion = eventCB->getAction()->getViewportRegion();
    SbVec3f pos = g_invalidCoord;

    {
        SoRayPickAction myPickAction(myRegion);
        myPickAction.setPoint(event->getPosition());
        myPickAction.setRadius(5.0);
        myPickAction.apply(pRoot);
        SoPickedPoint *pPoint = myPickAction.getPickedPoint();

        if (pPoint)
        {
            float x, y, z;
            SbVec3f worldPoint = pPoint->getPoint();
            worldPoint.getValue(x, y, z);

            /// TODO :
            pos = SbVec3f(x, y, z);
        }
    }
    return pos;
}

void GSView::showCoordInStatusBar(SbVec3f vec)
{
    m_pMainWindow->showCoordInStatusBar(vec);
}
