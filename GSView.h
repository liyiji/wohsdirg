/// File Name : GSView.h

#ifndef GSVIEW_H
#define GSVIEW_H

#include <QWidget>

#include <Inventor/SbVec.h>

#include "GSPublicDefine.h"

class SoSeparator;
class SoTransform;
class SoSelection;
class SoEventCallback;
class SoQtExaminerViewer;
class SoDirectionalLight;
class SoPerspectiveCamera;
class SoQtDirectionalLightEditor;

class GSTreeItem;
class GS3DObject;
class GSSelection;
class GSMainWindow;

void MouseMoveEventCB(void *userData, SoEventCallback *eventCB);
void MouseClickEventCB(void *userData, SoEventCallback *eventCB);

class GSView : public QWidget
{
    Q_OBJECT

public:
    GSView(GSMainWindow* pMainWindow, QWidget *parent = 0);
    virtual ~GSView();

    void showObject(GSTreeItem *pItem);
    void removeObject(GSTreeItem *pItem);
    void hideObject(GSTreeItem *pItem);
    GS3DObjectParameters get3DParameters();

    void viewAll();
    SoSeparator *getRoot();

    SbVec3f findPosition(SoSeparator *pRoot, SoEventCallback *eventCB);
    void showCoordInStatusBar(SbVec3f vec);

private:
    GSMainWindow        *m_pMainWindow;
    GSSelection         *m_pRootSelection;

    QVector<GS3DObject *> m_v3DObject;

    SoSeparator         *m_pRoot;
    SoQtExaminerViewer  *m_pViewer;
    SoPerspectiveCamera *m_pCamera3D;
    SoEventCallback     *m_pMouseEventCallBack;
    SoTransform         *m_pAllTransform;
    SoTransform         *m_pScale3Direction;

    // 光源及控制
    SoDirectionalLight         *m_pTopLight,   *m_pBottomLight;
    SoDirectionalLight         *m_pLeftLight,  *m_pRightLight;
    SoDirectionalLight         *m_pFrontLight, *m_pEndLight;
    SoQtDirectionalLightEditor *m_pMainLightEditor;
    SoQtDirectionalLightEditor *m_pTopLightEditor,   *m_pBottomLightEditor;
    SoQtDirectionalLightEditor *m_pLeftLightEditor,  *m_pRightLightEditor;
    SoQtDirectionalLightEditor *m_pFrontLightEditor, *m_pEndLightEditor;
};

#endif // GSVIEW_H
