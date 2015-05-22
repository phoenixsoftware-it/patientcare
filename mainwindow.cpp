/*
 Embedded Widgets Demo
 Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).*
 Contact:  Qt Software Information (qt-info@nokia.com)**
 This file may be used under the terms of the Embedded Widgets Demo License
 Agreement.
*/

#include "mainwindow.h"

#include <QBitmap>
#include <QtSvgToggleSwitch>

#include <QtGui/QMouseEvent>
#include <QMenu>
#include <QApplication>
#include <QtCore/QTimer>
#include <QDesktopWidget>
#include "patientcarecontroller.h"

/*!
    \page PatientCare Patient Care example

    \brief This is an example to show a embedded program with
    new generated widgets.

    The description of the used widgets and skins
    in this example.

    \section General GENERAL
        skin "Beryl"
        - QtSvgToggleSwitch

    \section Medication MEDICATION
        skin "Beryl"
        - QtScrollDial (two times)

    \section Limits LIMITS
        skin "Beryl"
        - QtMultiSlider
        \n The LED's are QLabels.

    \section Heart_Rate HEART RATE
        skin "Beryl"
        - Qt5WayButton (The Button group)
        - QtBasicGraph (The heart plot)

    \name Interaction

    The following interaction is possible.

    - The allowed medcine Dose can be changed with the Sliders from the
    QtMultiSlider in "LIMITS".
    - The medcine dose can updated with QtScrollDial in "MEDICATION".
    - The heart rate display can be changed with the Qt5WayButton in
    "HEART RATE".
    - The actual mode "Night mode" can be changed with the second
    QtSvgToggleSwitch  in "GENERAL"

*/



/*!
    \class MainWindow mainwindow.h
    \brief Class fills graphical user interface with controls.
*/
MainWindow::MainWindow()
    : QWidget(0, Qt::FramelessWindowHint), m_embedded(false)
{
    // create a widget we can access in CSS: QWidget#PatientCare
    QWidget* top = new QWidget(this);

#if defined(Q_WS_HILDON) || defined(Q_WS_S60) || defined(Q_WS_QWS) || defined(Q_OS_WINCE)
    m_embedded = true;
#endif
    if (QApplication::arguments().contains("-embedded"))
        m_embedded = true;
    if (QApplication::arguments().contains("-no-embedded"))
        m_embedded = false;

    ui.setupUi(top);
    connect(ui.switchNightMode, SIGNAL(toggled(bool)), this, SLOT(toggleNightMode(bool)));

    setWindowTitle("PatientCare (Demo)");
    setWindowIcon(QIcon(":/qt-logo32x32"));
    setSkins();

    ui.switchSystemOff->setChecked(true);
    ui.switchNightMode->setChecked(true);
    toggleNightMode(true);

    m_controller = new PatientCareController(&ui, this);

    if (!m_embedded) {
        updateDecoration();
        top->setGeometry(153, 95, 636, 476);
        QtSvgToggleSwitch* offSwitch = new QtSvgToggleSwitch(this);
        offSwitch->setSkin("Beryl");
        offSwitch->setChecked(true);
        offSwitch->setGeometry(795, 568, 50, 50);
    
        connect(offSwitch, SIGNAL(toggled(bool)), top, SLOT(setVisible(bool)));
        connect(offSwitch, SIGNAL(toggled(bool)), this, SLOT(quitDelay()));
    } else {
        QVBoxLayout *lay = new QVBoxLayout(this);
        lay->setContentsMargins(0, 0, 0, 0);
        lay->addWidget(top);

        qApp->setOverrideCursor(Qt::BlankCursor);
        setWindowState(Qt::WindowFullScreen);
    }
}


MainWindow::~MainWindow()
{
    // no need to delete the user interface, Qt does it for us
}

void MainWindow::updateDecoration()
{
    m_device = QPixmap(":/device-640x480.png", "png");
    QBitmap bitmap = m_device.createHeuristicMask();
    setFixedSize(m_device.size());
    setMask(bitmap);
    update();
}

void MainWindow::paintEvent(QPaintEvent* e)
{
    QPainter painter(this);
    painter.drawPixmap(e->rect(), m_device, e->rect());
}

void MainWindow::setSkins()
{
    ui.switchSystemOff->setSkin("Beryl");
    ui.switchNightMode->setSkin("Beryl");

    ui.multiSliderMedicine->setSkin("Beryl");

    ui.heartRateNavigator->setSkin("Beryl");
    ui.multiSliderHeartRate->setSkin("Beryl");
    ui.medicineFlow->setSkin("Beryl");
}

void MainWindow::toggleNightMode(bool on)
{
    if (m_embedded) {
        qApp->setPalette(on ? QColor(116,116,118) : QColor(218,219,220));
    } else {
        if (on) {
            qApp->setStyleSheet(
                "QWidget#PatientCare {background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, "
                "stop:0 rgb(116, 116, 118), stop:1 rgb(59, 57, 60)) }\n"
                "QLabel, QGroupBox { color: rgb(255, 255, 255)}\n"
                "QGroupBox { border: 2px solid rgb(180, 180, 180); border-radius: 7px;}");
        } else {
            qApp->setStyleSheet(
                "QWidget#PatientCare {background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, "
                "stop:0 rgb(218, 219, 220), stop:1 rgb(157, 157, 159)) }\n"
                "QLabel, QGroupBox { color: rgb(0, 0, 0)}\n"
                "QGroupBox { border: 2px solid rgb(150, 150, 150); border-radius: 7px;}");
        }
    }
    update();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (!m_embedded && (event->button() == Qt::LeftButton)) {
        m_dragPosition = event->globalPos();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_embedded && (event->buttons() & Qt::LeftButton)) {
        move(pos() + event->globalPos() - m_dragPosition);
        m_dragPosition = event->globalPos();
        event->accept();
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);
    QAction* action = menu.addAction("Close PatientCare");

    if (menu.exec(event->globalPos()) == action) {
        close();
    }
}

void MainWindow::quitDelay() {
    QTimer::singleShot(3000, QApplication::instance(), SLOT(quit()));
}
