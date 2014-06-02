//=============================================================================================================
/**
* @file     mainwindow.cpp
* @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
*           Matti Hamalainen <msh@nmr.mgh.harvard.edu>
* @version  1.0
* @date     February, 2013
*
* @section  LICENSE
*
* Copyright (C) 2013, Christoph Dinh and Matti Hamalainen. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that
* the following conditions are met:
*     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
*       following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
*       the following disclaimer in the documentation and/or other materials provided with the distribution.
*     * Neither the name of the Massachusetts General Hospital nor the names of its contributors may be used
*       to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MASSACHUSETTS GENERAL HOSPITAL BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*
* @brief    Contains the implementation of the MainWindow class.
*
*/

//*************************************************************************************************************
//=============================================================================================================
// INCLUDES
//=============================================================================================================

#include <mne_x/Management/pluginmanager.h>
#include <mne_x/Management/pluginscenemanager.h>
#include <mne_x/Management/displaymanager.h>

//GUI
#include "mainwindow.h"
#include "runwidget.h"
#include "startupwidget.h"
#include "plugingui.h"


//*************************************************************************************************************
//=============================================================================================================
// QT INCLUDES
//=============================================================================================================

#include <QtWidgets>
#include <QDebug>
#include <QTimer>
#include <QTime>

#include <iostream>


//*************************************************************************************************************
//=============================================================================================================
// USED NAMESPACES
//=============================================================================================================

using namespace MNEX;


//*************************************************************************************************************
//=============================================================================================================
// CONST
//=============================================================================================================

const char* pluginDir = "/mne_x_plugins";        /**< holds path to plugins.*/


//*************************************************************************************************************
//=============================================================================================================
// DEFINE MEMBER METHODS
//=============================================================================================================

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, m_pStartUpWidget(new StartUpWidget(this))
, m_pRunWidget(NULL)
, m_pDisplayManager(new DisplayManager(this))
, m_bDisplayMax(false)
, m_bIsRunning(false)
, m_pToolBar(NULL)
, m_pDynamicPluginToolBar(NULL)
, m_pDynamicDisplayToolBar(NULL)
, m_pLabelTime(NULL)
, m_pTimer(NULL)
, m_pTime(new QTime(0, 0))
, m_iTimeoutMSec(1000)
, m_pPluginGui(NULL)
, m_pPluginManager(new PluginManager(this))
, m_pPluginSceneManager(new PluginSceneManager(this))
, m_eLogLevelCurrent(_LogLvMax)
{
    qDebug() << "Clinical Sensing and Analysis - Version" << CInfo::AppVersion();

    setCentralWidget(m_pStartUpWidget);

    setWindowTitle(CInfo::AppNameShort());
    setMinimumSize(400, 400);
    resize(1280, 800);

    setUnifiedTitleAndToolBarOnMac(true);

    m_pPluginManager->loadPlugins(qApp->applicationDirPath()+pluginDir);

    createActions();
    createMenus();
    createToolBars();
    createPluginDockWindow();
    createLogDockWindow();

//    //ToDo Debug Startup
//    writeToLog(tr("Test normal message, Max"), _LogKndMessage, _LogLvMax);
//    writeToLog(tr("Test warning message, Normal"), _LogKndWarning, _LogLvNormal);
//    writeToLog(tr("Test error message, Min"), _LogKndError, _LogLvMin);

    initStatusBar();
}


//*************************************************************************************************************

MainWindow::~MainWindow()
{
    clear();
}


//*************************************************************************************************************

void MainWindow::clear()
{
    if(m_bIsRunning)
        this->stopMeasurement();

    //garbage collection
    m_pPluginSceneManager.reset();
}


//*************************************************************************************************************

void MainWindow::closeEvent(QCloseEvent *event)
{
    clear();

    QMainWindow::closeEvent(event);
}



//*************************************************************************************************************
//File QMenu
void MainWindow::newConfiguration()
{
    writeToLog(tr("Invoked <b>File|NewPreferences</b>"), _LogKndMessage, _LogLvMin);
}


//*************************************************************************************************************

void MainWindow::openConfiguration()
{
    writeToLog(tr("Invoked <b>File|OpenPreferences</b>"), _LogKndMessage, _LogLvMin);
}


//*************************************************************************************************************

void MainWindow::saveConfiguration()
{
    writeToLog(tr("Invoked <b>File|SavePreferences</b>"), _LogKndMessage, _LogLvMin);
}


//*************************************************************************************************************
//Help QMenu
void MainWindow::helpContents()
{
    writeToLog(tr("Invoked <b>Help|HelpContents</b>"), _LogKndMessage, _LogLvMin);
}


//*************************************************************************************************************

void MainWindow::about()
{
    writeToLog(tr("Invoked <b>Help|About</b>"), _LogKndMessage, _LogLvMin);
    QMessageBox::about(this, CInfo::AppNameShort()+ ", "+tr("Version ")+CInfo::AppVersion(),
         tr("Copyright (C) 2013 Christoph Dinh, Martin Luessi, Limin Sun, Jens Haueisen, Matti Hamalainen. All rights reserved.\n\n"
            "Redistribution and use in source and binary forms, with or without modification, are permitted provided that"
            " the following conditions are met:\n"
            "\t* Redistributions of source code must retain the above copyright notice, this list of conditions and the"
            " following disclaimer.\n"
            "\t* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and"
            " the following disclaimer in the documentation and/or other materials provided with the distribution.\n"
            "\t* Neither the name of the Massachusetts General Hospital nor the names of its contributors may be used"
            " to endorse or promote products derived from this software without specific prior written permission.\n\n"
            "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED"
            " WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A"
            " PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MASSACHUSETTS GENERAL HOSPITAL BE LIABLE FOR ANY DIRECT,"
            " INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,"
            " PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)"
            " HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING"
            " NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE"
            " POSSIBILITY OF SUCH DAMAGE."));
}


//*************************************************************************************************************

void MainWindow::setMinLogLevel()
{
    writeToLog(tr("minimal log level set"), _LogKndMessage, _LogLvMin);
    m_eLogLevelCurrent = _LogLvMin;
}


//*************************************************************************************************************

void MainWindow::setNormalLogLevel()
{
    writeToLog(tr("normal log level set"), _LogKndMessage, _LogLvMin);
    m_eLogLevelCurrent = _LogLvNormal;
}


//*************************************************************************************************************

void MainWindow::setMaxLogLevel()
{
    writeToLog(tr("maximal log level set"), _LogKndMessage, _LogLvMin);
    m_eLogLevelCurrent = _LogLvMax;
}

//*************************************************************************************************************

void MainWindow::createActions()
{
    //File QMenu
    m_pActionNewConfig = new QAction(QIcon(":/images/new.png"), tr("&New configuration"), this);
    m_pActionNewConfig->setShortcuts(QKeySequence::New);
    m_pActionNewConfig->setStatusTip(tr("Create a new configuration"));
//    connect(m_pActionNewConfig, SIGNAL(triggered()), this, SLOT(newPreferences()));

    m_pActionOpenConfig = new QAction(tr("&Open configuration..."), this);
    m_pActionOpenConfig->setShortcuts(QKeySequence::Open);
    m_pActionOpenConfig->setStatusTip(tr("Open an existing configuration"));
//    connect(m_pActionOpenConfig, SIGNAL(triggered()), this, SLOT(openPreferences()));

    m_pActionSaveConfig = new QAction(QIcon(":/images/save.png"), tr("&Save configuration..."), this);
    m_pActionSaveConfig->setShortcuts(QKeySequence::Save);
    m_pActionSaveConfig->setStatusTip(tr("Save the current configuration"));
//    connect(m_pActionSaveConfig, SIGNAL(triggered()), this, SLOT(savePreferences()));

    m_pActionExit = new QAction(tr("E&xit"), this);
    m_pActionExit->setShortcuts(QKeySequence::Quit);
    m_pActionExit->setStatusTip(tr("Exit the application"));
    connect(m_pActionExit, SIGNAL(triggered()), this, SLOT(close()));

    //View QMenu
    m_pActionMinLgLv = new QAction(tr("&Minimal"), this);
    m_pActionMinLgLv->setCheckable(true);
    m_pActionMinLgLv->setShortcut(tr("Ctrl+1"));
    m_pActionMinLgLv->setStatusTip(tr("Open an existing file"));
    connect(m_pActionMinLgLv, SIGNAL(triggered()), this, SLOT(setMinLogLevel()));

    m_pActionNormLgLv = new QAction(tr("&Normal"), this);
    m_pActionNormLgLv->setCheckable(true);
    m_pActionNormLgLv->setShortcut(tr("Ctrl+2"));
    m_pActionNormLgLv->setStatusTip(tr("Save the document to disk"));
    connect(m_pActionNormLgLv, SIGNAL(triggered()), this, SLOT(setNormalLogLevel()));

    m_pActionMaxLgLv = new QAction(tr("Maximal"), this);
    m_pActionMaxLgLv->setCheckable(true);
    m_pActionMaxLgLv->setShortcut(tr("Ctrl+3"));
    m_pActionMaxLgLv->setStatusTip(tr("Exit the application"));
    connect(m_pActionMaxLgLv, SIGNAL(triggered()), this, SLOT(setMaxLogLevel()));

    m_pActionGroupLgLv = new QActionGroup(this);
    m_pActionGroupLgLv->addAction(m_pActionMinLgLv);
    m_pActionGroupLgLv->addAction(m_pActionNormLgLv);
    m_pActionGroupLgLv->addAction(m_pActionMaxLgLv);
    if (m_eLogLevelCurrent == _LogLvMin){
        m_pActionMinLgLv->setChecked(true);}
    else if (m_eLogLevelCurrent == _LogLvNormal){
        m_pActionNormLgLv->setChecked(true);}
    else {
        m_pActionMaxLgLv->setChecked(true);}

    //Help QMenu
    m_pActionHelpContents = new QAction(tr("Help &Contents"), this);
    m_pActionHelpContents->setShortcuts(QKeySequence::HelpContents);
    m_pActionHelpContents->setStatusTip(tr("Show the help contents"));
    connect(m_pActionHelpContents, SIGNAL(triggered()), this, SLOT(helpContents()));

    m_pActionAbout = new QAction(tr("&About"), this);
    m_pActionAbout->setStatusTip(tr("Show the application's About box"));
    connect(m_pActionAbout, SIGNAL(triggered()), this, SLOT(about()));

    //QToolbar
    m_pActionRun = new QAction(QIcon(":/images/run.png"), tr("Run (F5)"), this);
    m_pActionRun->setShortcut(tr("F5"));
    m_pActionRun->setStatusTip(tr("Runs (F5) ")+CInfo::AppNameShort());
    connect(m_pActionRun, SIGNAL(triggered()), this, SLOT(startMeasurement()));

    m_pActionStop = new QAction(QIcon(":/images/stop.png"), tr("Stop (F6)"), this);
    m_pActionStop->setShortcut(tr("F6"));
    m_pActionStop->setStatusTip(tr("Stops (F6) ")+CInfo::AppNameShort());
    connect(m_pActionStop, SIGNAL(triggered()), this, SLOT(stopMeasurement()));

    m_pActionZoomStd = new QAction(QIcon(":/images/zoomStd.png"), tr("Standard Zoom (Ctrl+0)"), this);
    m_pActionZoomStd->setShortcut(tr("Ctrl+0"));
    m_pActionZoomStd->setStatusTip(tr("Sets the standard Zoom (Ctrl+0)"));
    connect(m_pActionZoomStd, SIGNAL(triggered()), this, SLOT(zoomStd()));

    m_pActionZoomIn = new QAction(QIcon(":/images/zoomIn.png"), tr("Zoom In ")+QKeySequence(QKeySequence::ZoomIn).toString(), this);
    m_pActionZoomIn->setShortcuts(QKeySequence::ZoomIn);
    m_pActionZoomIn->setStatusTip(tr("Zooms in the magnitude ")+QKeySequence(QKeySequence::ZoomIn).toString());
    connect(m_pActionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));

    m_pActionZoomOut = new QAction(QIcon(":/images/zoomOut.png"), tr("Zoom Out ")+QKeySequence(QKeySequence::ZoomOut).toString(), this);
    m_pActionZoomOut->setShortcuts(QKeySequence::ZoomOut);
    m_pActionZoomOut->setStatusTip(tr("Zooms out the magnitude ")+QKeySequence(QKeySequence::ZoomOut).toString());
    connect(m_pActionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));

    m_pActionDisplayMax = new QAction(QIcon(":/images/displayMax.png"), tr("Maximize current Display (F11)"), this);
    m_pActionDisplayMax->setShortcut(tr("F11"));
    m_pActionDisplayMax->setStatusTip(tr("Maximizes the current display (F11)"));
    connect(m_pActionDisplayMax, SIGNAL(triggered()), this, SLOT(toggleDisplayMax()));
}


//*************************************************************************************************************

void MainWindow::createMenus()
{
    m_pMenuFile = menuBar()->addMenu(tr("&File"));
    m_pMenuFile->addAction(m_pActionNewConfig);
    m_pMenuFile->addAction(m_pActionOpenConfig);
    m_pMenuFile->addAction(m_pActionSaveConfig);
    m_pMenuFile->addSeparator();
    m_pMenuFile->addAction(m_pActionExit);

    m_pMenuView = menuBar()->addMenu(tr("&View"));
    m_pMenuLgLv = m_pMenuView->addMenu(tr("&Log Level"));
    m_pMenuLgLv->addAction(m_pActionMinLgLv);
    m_pMenuLgLv->addAction(m_pActionNormLgLv);
    m_pMenuLgLv->addAction(m_pActionMaxLgLv);
    m_pMenuView->addSeparator();

    menuBar()->addSeparator();

    m_pMenuHelp = menuBar()->addMenu(tr("&Help"));
    m_pMenuHelp->addAction(m_pActionHelpContents);
    m_pMenuHelp->addSeparator();
    m_pMenuHelp->addAction(m_pActionAbout);

}


//*************************************************************************************************************

void MainWindow::createToolBars()
{
    //Control
    if(!m_pToolBar)
    {
        m_pToolBar = addToolBar(tr("Control"));
        m_pToolBar->addAction(m_pActionRun);
        m_pToolBar->addAction(m_pActionStop);
        m_pActionStop->setEnabled(false);

        m_pToolBar->addSeparator();

        m_pToolBar->addAction(m_pActionZoomStd);
        m_pToolBar->addAction(m_pActionZoomIn);
        m_pToolBar->addAction(m_pActionZoomOut);
        m_pToolBar->addAction(m_pActionDisplayMax);
        m_pActionZoomStd->setEnabled(false);
        m_pActionZoomIn->setEnabled(false);
        m_pActionZoomOut->setEnabled(false);
        m_pActionDisplayMax->setEnabled(false);

        m_pToolBar->addSeparator();

        m_pLabelTime = new QLabel;
        m_pToolBar->addWidget(m_pLabelTime);
        m_pLabelTime->setText(QTime(0, 0).toString());
    }

    //Plugin
    if(m_pDynamicPluginToolBar)
    {
        removeToolBar(m_pDynamicPluginToolBar);
        delete m_pDynamicPluginToolBar;
        m_pDynamicPluginToolBar = NULL;
    }
    if(m_qListDynamicPluginActions.size() > 0 || m_qListDynamicPluginWidgets.size() > 0)
    {
        m_pDynamicPluginToolBar = addToolBar(m_sCurPluginName + tr("Control"));
        for(qint32 i = 0; i < m_qListDynamicPluginActions.size(); ++i)
            m_pDynamicPluginToolBar->addAction(m_qListDynamicPluginActions[i]);
        for(qint32 i = 0; i < m_qListDynamicPluginWidgets.size(); ++i)
            m_pDynamicPluginToolBar->addWidget(m_qListDynamicPluginWidgets[i]);
    }

    //Display
    if(m_pDynamicDisplayToolBar)
    {
        removeToolBar(m_pDynamicDisplayToolBar);
        delete m_pDynamicDisplayToolBar;
        m_pDynamicDisplayToolBar = NULL;
    }
    if(m_qListDynamicDisplayActions.size() > 0 || m_qListDynamicDisplayWidgets.size() > 0)
    {
        m_pDynamicDisplayToolBar = addToolBar(tr("Display"));
        for(qint32 i = 0; i < m_qListDynamicDisplayActions.size(); ++i)
            m_pDynamicDisplayToolBar->addAction(m_qListDynamicDisplayActions[i]);
        for(qint32 i = 0; i < m_qListDynamicDisplayWidgets.size(); ++i)
            m_pDynamicDisplayToolBar->addWidget(m_qListDynamicDisplayWidgets[i]);
    }

}


//*************************************************************************************************************

void MainWindow::initStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}


//*************************************************************************************************************

void MainWindow::createPluginDockWindow()
{
    m_pPluginGuiDockWidget = new QDockWidget(tr("Plugins"), this);
    m_pPluginGuiDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_pPluginGui = new PluginGui(m_pPluginManager, m_pPluginSceneManager);
    m_pPluginGui->setParent(m_pPluginGuiDockWidget);
    m_pPluginGuiDockWidget->setWidget(m_pPluginGui);

    addDockWidget(Qt::LeftDockWidgetArea, m_pPluginGuiDockWidget);

    connect(m_pPluginGui, &PluginGui::selectedPluginChanged,
            this, &MainWindow::updatePluginWidget);

    connect(m_pPluginGui, &PluginGui::selectedConnectionChanged,
            this, &MainWindow::updateConnectionWidget);
}


//*************************************************************************************************************

void MainWindow::createLogDockWindow()
{
    //Log TextBrowser
    m_pDockWidget_Log = new QDockWidget(tr("Log"), this);

    m_pTextBrowser_Log = new QTextBrowser(m_pDockWidget_Log);

    m_pDockWidget_Log->setWidget(m_pTextBrowser_Log);

    m_pDockWidget_Log->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, m_pDockWidget_Log);

    //dock->setVisible(false);

    m_pMenuView->addAction(m_pDockWidget_Log->toggleViewAction());

}


//*************************************************************************************************************
//Plugin stuff
void MainWindow::updatePluginWidget(IPlugin::SPtr pPlugin)
{
    m_qListDynamicPluginActions.clear();
    m_qListDynamicPluginWidgets.clear();
    m_qListDynamicDisplayActions.clear();
    m_qListDynamicDisplayWidgets.clear();

    if(!pPlugin.isNull())
    {
        // Add Dynamic Plugin Actions
        m_qListDynamicPluginActions.append(pPlugin->getPluginActions());
        m_qListDynamicPluginWidgets.append(pPlugin->getPluginWidgets());

        m_sCurPluginName = pPlugin->getName();

        //Garbage collecting
        if(m_pRunWidget)
        {
            delete m_pRunWidget;
            m_pRunWidget = NULL;
        }

        if(pPlugin.isNull())
        {
            QWidget* pWidget = new QWidget;
            setCentralWidget(pWidget);
        }
        else
        {
            if(!m_bIsRunning)
                setCentralWidget(pPlugin->setupWidget());
            else
            {
                m_pRunWidget = new RunWidget( m_pDisplayManager->show(pPlugin->getOutputConnectors(), m_pTime, m_qListDynamicDisplayActions, m_qListDynamicDisplayWidgets));

                m_pRunWidget->show();

                if(m_bDisplayMax)//ToDo send events to main window
                {
                    m_pRunWidget->showFullScreen();
                    connect(m_pRunWidget, &RunWidget::displayClosed, this, &MainWindow::toggleDisplayMax);
                }
                else
                    setCentralWidget(m_pRunWidget);
            }
        }
    }
    else
    {
        QWidget* t_pWidgetEmpty = new QWidget;
        setCentralWidget(t_pWidgetEmpty);
    }

    this->createToolBars();
}


//*************************************************************************************************************

void MainWindow::updateConnectionWidget(PluginConnectorConnection::SPtr pConnection)
{
    QWidget* pWidget = pConnection->setupWidget();
    setCentralWidget(pWidget);
}


//*************************************************************************************************************

void MainWindow::writeToLog(const QString& logMsg, LogKind lgknd, LogLevel lglvl)
{
    if(lglvl<=m_eLogLevelCurrent)
    {
        if(lgknd == _LogKndError)
        {
            m_pTextBrowser_Log->insertHtml("<font color=red><b>Error:</b> "+logMsg+"</font>");

        }
        else if(lgknd == _LogKndWarning)
        {
            m_pTextBrowser_Log->insertHtml("<font color=blue><b>Warning:</b> "+logMsg+"</font>");
        }
        else
        {
            m_pTextBrowser_Log->insertHtml(logMsg);
        }
        m_pTextBrowser_Log->insertPlainText("\n"); // new line
        //scroll down to the newest entry
        QTextCursor c = m_pTextBrowser_Log->textCursor();
        c.movePosition(QTextCursor::End);
        m_pTextBrowser_Log->setTextCursor(c);

        m_pTextBrowser_Log->verticalScrollBar()->setValue(m_pTextBrowser_Log->verticalScrollBar()->maximum());
    }
}


//*************************************************************************************************************

void MainWindow::startMeasurement()
{
    writeToLog(tr("Starting real-time measurement..."), _LogKndMessage, _LogLvMin);

    if(!m_pPluginSceneManager->startPlugins())
    {
        QMessageBox::information(0, tr("MNE X - Start"), QString(QObject::tr("Not able to start at least one sensor plugin!")), QMessageBox::Ok);
        return;
    }

    m_pPluginGui->uiSetupRunningState(true);
    uiSetupRunningState(true);
    startTimer(m_iTimeoutMSec);

    updatePluginWidget(m_pPluginGui->getCurrentPlugin());

//    CentralWidgetShowPlugin();


//    //OLD
//    qDebug() << "MainCSART::startMeasurement()";

//    //MeasurementManager::clean();
//    //DisplayManager::clean();

//    if(!PluginManager::startPlugins())
//    {
//        QMessageBox::information(0, QObject::tr("CSA RT - Start plugins"), QString(QObject::tr("No Sensor plugin is active")), QMessageBox::Ok);
//        return;
//    }

//    m_pPluginDockWidget->setTogglingEnabled(false);

//    //OLD
}


//*************************************************************************************************************

void MainWindow::stopMeasurement()
{
    writeToLog(tr("Stopping real-time measurement..."), _LogKndMessage, _LogLvMin);

    m_pPluginSceneManager->stopPlugins();
    m_pDisplayManager->clean();


    m_pPluginGui->uiSetupRunningState(false);
    uiSetupRunningState(false);
    stopTimer();

    updatePluginWidget(m_pPluginGui->getCurrentPlugin());



//    PluginManager::stopPlugins();

//    Connector::disconnectMeasurementWidgets(m_pListCurrentDisplayPlugins);//was before stopPlugins();

//    qDebug() << "set stopped UI";

//    m_pPluginDockWidget->setTogglingEnabled(true);
}


//*************************************************************************************************************

void MainWindow::zoomStd()
{
    if(m_pRunWidget)
    {
        m_pRunWidget->setStandardZoom();
        m_pActionZoomStd->setEnabled(false);
    }
}


//*************************************************************************************************************

void MainWindow::zoomIn()
{
    if(m_pRunWidget)
    {
        m_pRunWidget->zoomVert(2);
        m_pActionZoomStd->setEnabled(true);
    }
}


//*************************************************************************************************************

void MainWindow::zoomOut()
{
    if(m_pRunWidget)
    {
        m_pRunWidget->zoomVert(0.5);
        m_pActionZoomStd->setEnabled(true);
    }

}


//*************************************************************************************************************

void MainWindow::toggleDisplayMax()
{

    m_bDisplayMax = !m_bDisplayMax;

    m_pActionDisplayMax->setEnabled(!m_bDisplayMax);

    updatePluginWidget(m_pPluginGui->getCurrentPlugin());
}


//*************************************************************************************************************

void MainWindow::uiSetupRunningState(bool state)
{
    m_pActionRun->setEnabled(!state);
    m_pActionStop->setEnabled(state);

    if(state)
    {
        m_pActionZoomStd->setEnabled(!state);
        m_pActionZoomIn->setEnabled(state);
        m_pActionZoomOut->setEnabled(state);
        m_pActionDisplayMax->setEnabled(state);
    }
    else
    {
        m_pActionZoomStd->setEnabled(state);
        m_pActionZoomIn->setEnabled(state);
        m_pActionZoomOut->setEnabled(state);
        m_pActionDisplayMax->setEnabled(state);
    }

    m_bIsRunning = state;
}



//*************************************************************************************************************

void MainWindow::startTimer(int msec)
{
    m_pTimer = QSharedPointer<QTimer>(new QTimer(this));
    connect(m_pTimer.data(), SIGNAL(timeout()), this, SLOT(updateTime()));
    m_pTimer->start(msec);
    m_pTime->setHMS(0,0,0);
    QString strTime = m_pTime->toString();
    m_pLabelTime->setText(strTime);
}


//*************************************************************************************************************

void MainWindow::stopTimer()
{
    disconnect(m_pTimer.data(), SIGNAL(timeout()), this, SLOT(updateTime()));
}


//*************************************************************************************************************

void MainWindow::updateTime()
{
    *m_pTime = m_pTime->addMSecs(m_iTimeoutMSec);
    QString strTime = m_pTime->toString();
    m_pLabelTime->setText(strTime);
}
