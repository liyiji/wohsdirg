/// File Name : gridShowMain.cpp

#include <QApplication>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

#include "GSMainWindow.h"
#include "GSPublicFunctions.h"

int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(Icons);
    QApplication app(argc, argv);

    bool bCheckOIVLicense = true;
#ifdef CODING
    bCheckOIVLicense = false;
#endif
    if (bCheckOIVLicense)
    {
        if (outputCheckOIVLicenseResult() == false)
        {
            return 0;
        }
    }

    GSMainWindow *a = new GSMainWindow();
    app.setActiveWindow(a);
    a->show();
    app.exec();

    delete a;
    a = 0;
    return 0;
}

//int main(int, char** argv)
//{
//    QWidget* myWindow = SoQt::init(argv[0]);

//    SoQtExaminerViewer* myViewer = new SoQtExaminerViewer(myWindow);

//    SoSeparator* pRootSep = new SoSeparator;

//    createScene(pRootSep);

//    myViewer->setSceneGraph(pRootSep);
//    myViewer->setTitle("(@.@)Title(@.@)");
//    myViewer->show();

//    SoQt::show(myWindow);
//    SoQt::mainLoop();

//    delete myViewer;
//    SoQt::finish();

//    return 0;
//}
