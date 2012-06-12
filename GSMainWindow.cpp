/// File Name : GSMainWindow.cpp

#include <QDockWidget>
#include <QMessageBox>
#include <QCloseEvent>

#include "GSMainWindow.h"
#include "ui_GSMainWindow.h"
#include "GSTree.h"
#include "GSView.h"
#include "GSPublicFunctions.h"

GSMainWindow::GSMainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::GSMainWindow)
{
    ui->setupUi(this);

    m_pDockTree = 0;
    m_pDockView = 0;
    m_pTree = 0;
    m_pView = 0;

    initOIVParameters(this);

    //    setWindowIcon();
    //    setWindowTitle();
    //    resize();

    createActions();
    createToolBar();
    createStatusBar();
    createDockWindows();
    createMenuBar();
    initConnection();

    m_pView->viewAll();
}

GSMainWindow::~GSMainWindow()
{
    delete ui;
    if (m_pTree)
    {
        delete m_pTree;
        m_pTree = 0;
    }
}

GSTree* GSMainWindow::getTree()
{
    return m_pTree;
}

GSView* GSMainWindow::getView()
{
    return m_pView;
}

QDockWidget* GSMainWindow::addDockWidget(QString              sName,
                                         QWidget             *pWidgetInDock,
                                         QAction             *pActionShowWidget,
                                         QToolBar            *pToolBarForWidget,
                                         QAction             *pActionShowToolBar,
                                         Qt::DockWidgetArea   eDockArea)
{
    /// TODO :
    QDockWidget* pDock = new QDockWidget(this);
    pDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    pDock->setWidget(pWidgetInDock);
    pDock->setWindowTitle(sName);

    if (pWidgetInDock)
    {
        pWidgetInDock->setWindowTitle(sName);
        pWidgetInDock->showMaximized();
    }

    QMainWindow::addDockWidget(eDockArea, pDock);

    if (pActionShowWidget)
    {
        if (sName == "Data Tree")
        {
            connect(pActionShowWidget, SIGNAL(triggered(bool)),
                    this, SLOT(slotShowTree(bool)));
        }
        else if (sName == "3D View")
        {
            connect(pActionShowWidget, SIGNAL(triggered(bool)),
                    this, SLOT(slotShowView(bool)));
        }
        pActionShowWidget->setCheckable(true);
        pActionShowWidget->setChecked(true);
    }

    return pDock;
}

void GSMainWindow::showCoordInStatusBar(SbVec3f coord)
{
    QString strCoord;
    strCoord.append("x: ");
    strCoord.append(QString::number(int(coord[0])));
    strCoord.append(", y: ");
    strCoord.append(QString::number(int(coord[1])));
    strCoord.append(", z: ");
    strCoord.append(QString::number(int(coord[2])));

    showMessageInStatusBar(strCoord);
}

void GSMainWindow::showMessageInStatusBar(QString str)
{
    if (str.isEmpty())
    {
        str = QString("Ready");
    }

    statusBar()->showMessage(str);
}

void GSMainWindow::createActions()
{
    /// TODO :
}

void GSMainWindow::createMenuBar()
{
    /// TODO :
}

void GSMainWindow::createToolBar()
{
    /// TODO :
}

void GSMainWindow::createStatusBar()
{
    showMessageInStatusBar("Ready");
}

void GSMainWindow::createDockWindows()
{
    setDockOptions(QMainWindow::AnimatedDocks);
    setDockNestingEnabled (true);

    m_pTree = new GSTree(this);
    m_pTree->refreshTree();
    m_pTree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pDockTree = addDockWidget(tr("Data Tree"),
                                m_pTree,
                                ui->actionView_Data_Tree,
                                0,
                                0,
                                Qt::LeftDockWidgetArea);
    m_pDockTree->setMinimumWidth(200);
    m_pDockTree->setMaximumWidth(300);
    connect(m_pDockTree, SIGNAL(visibilityChanged(bool)),
            this, SLOT(slotShowTree(bool)));

    m_pView = new GSView(this, this);
    m_pDockView = addDockWidget(tr("3D View"),
                                m_pView,
                                ui->actionView_3D_View,
                                0,
                                0,
                                Qt::RightDockWidgetArea);
    connect(m_pDockView, SIGNAL(visibilityChanged(bool)),
            this, SLOT(slotShowView(bool)));

    setCentralWidget(0);
}

void GSMainWindow::initConnection()
{
    /// TODO :
}

void GSMainWindow::closeEvent(QCloseEvent *event)
{
#ifdef CODING
    event->accept();
#else
    if (QMessageBox::warning(this,
                             "Close Window",
                             "Are you sure to close this window?",
                             QMessageBox::Yes,
                             QMessageBox::No) == QMessageBox::Yes)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
#endif
}

void GSMainWindow::slotShowTree(bool b)
{
    if (m_pDockTree == 0)
    {
        return;
    }

    if (ui->actionView_Data_Tree->isChecked() != b)
    {
        ui->actionView_Data_Tree->setChecked(b);
    }
    if (m_pDockTree->isVisible() != b)
    {
        m_pDockTree->setVisible(b);
    }
}

void GSMainWindow::slotShowView(bool b)
{
    if (m_pDockView == 0)
    {
        return;
    }

    if (ui->actionView_3D_View->isChecked() != b)
    {
        ui->actionView_3D_View->setChecked(b);
    }
    if (m_pDockView->isVisible() != b)
    {
        m_pDockView->setVisible(b);
    }
}
