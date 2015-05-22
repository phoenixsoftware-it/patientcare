/*
 Embedded Widgets Demo
 Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).*
 Contact:  Qt Software Information (qt-info@nokia.com)**
 This file may be used under the terms of the Embedded Widgets Demo License
 Agreement.
*/

#include "patientcarecontroller.h"

#include <QtCore>
#include <QtGui>
#include <QtSvg>
#include "ui_patientcare.h"

#include "datagenerator.h"
#include "lazytimer.h"

/*!

\class PatientCareController patientcarecontroller.h
\brief Private class used from PatientCare example to add interaction
    to the example widget.

    The interaction is described in \a Control.
*/
PatientCareController::PatientCareController(Ui::PatientCare* gui, QObject * parent)
    : QObject(parent)
    , ui(gui)
{
    m_pulseTimer = new LazyTimer(this);

    // set alignments
    ui->layoutMedication->setAlignment(ui->medicineFlow, Qt::AlignHCenter);
//     ui->layoutGraph->setAlignment(ui->heartRateNavigator, Qt::AlignHCenter);
//     ui->layoutLimits->setAlignment(ui->multiSliderMedicine, Qt::AlignHCenter);
    ui->layoutGeneral->setAlignment(ui->switchNightMode, Qt::AlignHCenter);
    ui->layoutGeneral->setAlignment(ui->switchSystemOff, Qt::AlignHCenter);

    ui->topLayout->setStretchFactor(ui->layoutGroupBoxes, 5);
    ui->topLayout->setStretchFactor(ui->gbGraph, 0);

    // other initialization
    ui->multiSliderMedicine->setRange(0, 100);
    ui->multiSliderMedicine->setMaximumRange(70);
    ui->multiSliderMedicine->setMinimumRange(20);
    ui->multiSliderMedicine->setValue(60);

    ui->medicineFlow->setRange(0, 100);
    ui->medicineFlow->setValue(60);

    ui->multiSliderHeartRate->setRange(0, 130);
    ui->multiSliderHeartRate->topSlider()->setRange(0, 130);
    ui->multiSliderHeartRate->bottomSlider()->setRange(0, 130);
    ui->multiSliderHeartRate->setMaximumRange(90);
    ui->multiSliderHeartRate->setMinimumRange(60);
    ui->multiSliderHeartRate->setValue(80);

    updateFlowRange();
    updatePulseRange();

    connect(ui->medicineFlow, SIGNAL(valueChanged(int)), this, SLOT(setMedication(int)));
    connect(ui->multiSliderMedicine, SIGNAL(maximumExceeded(bool)), this, SLOT(maximumDripRateExceeded(bool)));
    connect(ui->multiSliderMedicine, SIGNAL(minimumExceeded(bool)), this, SLOT(minimumDripRateExceeded(bool)));
    connect(ui->multiSliderMedicine, SIGNAL(valueChanged(int)), this, SLOT(updateMedicineFlow(int)));
    connect(ui->multiSliderMedicine->topSlider(), SIGNAL(valueChanged(int)), this, SLOT(updateFlowRange()));
    connect(ui->multiSliderMedicine->bottomSlider(), SIGNAL(valueChanged(int)), this, SLOT(updateFlowRange()));

    connect(ui->multiSliderHeartRate, SIGNAL(maximumExceeded(bool)), this, SLOT(maximumHeartRateExceeded(bool)));
    connect(ui->multiSliderHeartRate, SIGNAL(minimumExceeded(bool)), this, SLOT(minimumHeartRateExceeded(bool)));
    connect(ui->multiSliderHeartRate, SIGNAL(valueChanged(int)), this, SLOT(updateHeartRate(int)));
    connect(ui->multiSliderHeartRate->topSlider(), SIGNAL(valueChanged(int)), this, SLOT(updatePulseRange()));
    connect(ui->multiSliderHeartRate->bottomSlider(), SIGNAL(valueChanged(int)), this, SLOT(updatePulseRange()));

    // make the PlotWidget work
    m_signalGenerator = new SignalGenerator(this);
    connect(m_signalGenerator, SIGNAL(newPoint(const QPointF &)), ui->heartRatePlot, SLOT(addPoint(const QPointF &)));

    //define visible range of QtBasicGraph
    ui->heartRatePlot->setRenderHints(QPainter::Antialiasing);
    ui->heartRatePlot->setYMinMax(-10.0, 10.0);
    ui->heartRatePlot->setXRange(180.0);
    ui->heartRatePlot->setFocusPolicy(Qt::NoFocus);

    connect(ui->heartRateNavigator->upButton(), SIGNAL(clicked()), this, SLOT(zoomYIn()));
    connect(ui->heartRateNavigator->downButton(), SIGNAL(clicked()), this, SLOT(zoomYOut()));
    connect(ui->heartRateNavigator->leftButton(), SIGNAL(clicked()), this, SLOT(zoomXOut()));
    connect(ui->heartRateNavigator->rightButton(), SIGNAL(clicked()), this, SLOT(zoomXIn()));
    connect(ui->heartRateNavigator->centerButton(), SIGNAL(clicked()), this, SLOT(resetZoom()));

    connect(ui->switchSystemOff, SIGNAL(toggled(bool)), this, SLOT(enableMedication(bool)));

    m_dripTimeLine = 0;
    setGraphAlertColor(false);
    setMedication(60);

    connect(m_pulseTimer, SIGNAL(timeout()), m_signalGenerator, SLOT(generateHeartbeat()));
    m_pulseTimer->setInterval(1333);
    m_pulseTimer->start();
}

PatientCareController::~PatientCareController()
{
}

void PatientCareController::setMedication(int value)
{
    if (m_dripTimeLine && value == m_dripTimeLine->endFrame()) {
        return;
    }

    const int start = ui->multiSliderMedicine->value();
    delete m_dripTimeLine;

    m_dripTimeLine = new QTimeLine(4000, this);
    connect(m_dripTimeLine, SIGNAL(frameChanged(int)), ui->multiSliderMedicine, SLOT(setValue(int)));

    m_dripTimeLine->setCurveShape(QTimeLine::LinearCurve);
    m_dripTimeLine->setFrameRange(start, value);
    m_dripTimeLine->start();
}

static QPicture createPicture(const QString& url)
{
    QPicture picture;
    QSvgRenderer renderer(url);
    QPainter painter(&picture);
    renderer.render(&painter);
    painter.end();
    return picture;
}

void PatientCareController::maximumDripRateExceeded(bool exceeded)
{
    if (exceeded) {
        ui->lblStatusFlow->setText("HIGH");
        ui->lblLedFlow->setPicture(createPicture(":/led-red.svg"));
    } else {
        ui->lblStatusFlow->setText("OK");
        ui->lblLedFlow->setPicture(createPicture(":/led-green.svg"));
    }
}

void PatientCareController::minimumDripRateExceeded(bool exceeded)
{
    if (exceeded) {
        ui->lblStatusFlow->setText("LOW");
        ui->lblLedFlow->setPicture(createPicture(":/led-red.svg"));
    } else {
        ui->lblStatusFlow->setText("OK");
        ui->lblLedFlow->setPicture(createPicture(":/led-green.svg"));
    }
}

void PatientCareController::maximumHeartRateExceeded(bool exceeded)
{
    if (exceeded) {
        ui->lblStatusHeartRate->setText("HIGH");
        ui->lblLedHeartRate->setPicture(createPicture(":/led-red.svg"));
    } else {
        ui->lblStatusHeartRate->setText("OK");
        ui->lblLedHeartRate->setPicture(createPicture(":/led-green.svg"));
    }
    setGraphAlertColor(exceeded);
}

void PatientCareController::minimumHeartRateExceeded(bool exceeded)
{
    if (exceeded) {
        ui->lblStatusHeartRate->setText("LOW");
        ui->lblLedHeartRate->setPicture(createPicture(":/led-red.svg"));
    } else {
        ui->lblStatusHeartRate->setText("OK");
        ui->lblLedHeartRate->setPicture(createPicture(":/led-green.svg"));
    }
    setGraphAlertColor(exceeded);
}

void PatientCareController::updateMedicineFlow(int value)
{
    ui->lblFlow->setText(QString("Drip Rate\n%1 ml/h").arg(value));

    int f = 40.0 + (100.0 - value) * 80.0 / 100.0;
    ui->multiSliderHeartRate->setValue(f);
    m_pulseTimer->setInterval(60.0 * 1000.0 / static_cast<double>(f));
}

void PatientCareController::updateHeartRate(int value)
{
    ui->lblRate->setText(QString("Pulse\n%1").arg(value));
}

void PatientCareController::setGraphAlertColor(bool alert)
{
    if (false) { // could be if (m_embedded)
        if (alert) {
            ui->heartRatePlot->setStyleSheet(
                "color: white; background-color: #8E0F26;");
        } else {
            ui->heartRatePlot->setStyleSheet(
                "color: white; background-color: rgba(74, 72, 74, 255);");
        }
    } else {
        if (alert) {
            ui->heartRatePlot->setStyleSheet(
                "color: white; background-color: qlineargradient(spread:pad, "
                "x1:0, y1:1, x2:0, y2:0, "
                "stop:0 #8E0F26, "
                "stop:0.3 #8E0F26, "
                "stop:1 rgba(74, 72, 74, 255));");
        } else {
            ui->heartRatePlot->setStyleSheet(
                "color: white; background-color: qlineargradient(spread:pad, "
                "x1:0, y1:1, x2:0, y2:0, "
                "stop:0 rgba(59, 57, 60, 255), "
                "stop:0.3 rgba(95, 95, 95, 255), "
                "stop:1 rgba(74, 72, 74, 255));");
        }
    }
}

void PatientCareController::updateFlowRange()
{
    ui->lblRangeFlow->setText(QString("Safe Range:\n %1 - %2")
        .arg(ui->multiSliderMedicine->bottomSlider()->value())
        .arg(ui->multiSliderMedicine->topSlider()->value()));
}

void PatientCareController::updatePulseRange()
{
    ui->lblRangePulse->setText(QString("Safe Range:\n %1 - %2")
        .arg(ui->multiSliderHeartRate->bottomSlider()->value())
        .arg(ui->multiSliderHeartRate->topSlider()->value()));
}

void PatientCareController::enableMedication(bool enable)
{
    if (enable) {
        connect(ui->medicineFlow, SIGNAL(valueChanged(int)), this, SLOT(setMedication(int)));
        setMedication(ui->medicineFlow->value());
    } else {
        setMedication(0);
        disconnect(ui->medicineFlow, SIGNAL(valueChanged(int)), this, SLOT(setMedication(int)));
    }
}

void PatientCareController::zoomXIn()
{
    if (ui->heartRatePlot->xRange() > 10.0)
        ui->heartRatePlot->setXRange(ui->heartRatePlot->xRange() - 10.0);
}

void PatientCareController::zoomXOut()
{
    if (ui->heartRatePlot->xRange() < 500.0)
        ui->heartRatePlot->setXRange(ui->heartRatePlot->xRange() + 10.0);
}

void PatientCareController::zoomYIn()
{
    if (ui->heartRatePlot->yRange() > 10.0)
        ui->heartRatePlot->setYMinMax(ui->heartRatePlot->yMin() + 1.0, ui->heartRatePlot->yMax() - 1.0);
}

void PatientCareController::zoomYOut()
{
    if (ui->heartRatePlot->yRange() < 100.0)
        ui->heartRatePlot->setYMinMax(ui->heartRatePlot->yMin() - 1.0, ui->heartRatePlot->yMax() + 1.0);
}

void PatientCareController::resetZoom()
{
    ui->heartRatePlot->setXRange(180.0);
    ui->heartRatePlot->setYMinMax(-10.0, 10.0);
}
