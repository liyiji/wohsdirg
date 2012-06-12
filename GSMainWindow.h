/// File Name : GSMainWindow.h

#ifndef GSMAINWINDOW_H
#define GSMAINWINDOW_H

#include <QString>
#include <QMainWindow>

#include <Inventor/SbVec.h>

class QDockWidget;
class GSTree;
class GSView;

namespace Ui {
    class GSMainWindow;
}

class GSMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    GSMainWindow(QWidget *parent = 0);
    virtual ~GSMainWindow();

public:
    GSTree* getTree();
    GSView* getView();

    QDockWidget* addDockWidget(QString              sName,
                               QWidget             *pWidgetInDock,
                               QAction             *pActionShowWidget,
                               QToolBar            *pToolBarForWidget,
                               QAction             *pActionShowToolBar,
                               Qt::DockWidgetArea   eDockArea);

    void showCoordInStatusBar(SbVec3f coord);
    void showMessageInStatusBar(QString str);

private:
    virtual void createActions();
    virtual void createMenuBar();
    virtual void createToolBar();
    virtual void createStatusBar();
    virtual void createDockWindows();
    virtual void initConnection();

    virtual void closeEvent(QCloseEvent *event);

private slots:
    void slotShowTree(bool);
    void slotShowView(bool);

private:
    Ui::GSMainWindow *ui;

    QDockWidget *m_pDockTree;
    QDockWidget *m_pDockView;
    GSTree* m_pTree;
    GSView* m_pView;
};

#endif // GSMAINWINDOW_H
