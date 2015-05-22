/*
 Embedded Widgets Demo
 Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).*
 Contact:  Qt Software Information (qt-info@nokia.com)**
 This file may be used under the terms of the Embedded Widgets Demo License
 Agreement.
*/

#include "datagenerator.h"

#include <QtCore/QTimer>
#include <QtCore/QDebug>
/*!
    \class SignalGenerator datagenerator.h
    \brief Fill the model used for QtBasicGraph with data.
*/
SignalGenerator::SignalGenerator(QObject *parent)
    : QObject(parent), m_tickstate(0), m_tick(0)
{
    m_timer = new QTimer(this);

    //connect to update data
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    m_timer->start(50);
}

SignalGenerator::~SignalGenerator()
{
}

void SignalGenerator::tick()
{
    QPointF pt(m_tick++, 0);

    if (m_tickstate == 1) {
        pt.setY(5.0);
        ++m_tickstate;
    } else if (m_tickstate == 2) {
        pt.setY(-2.0);
        m_tickstate = 0;
    } else {
        pt.setY(0.5*qrand() / static_cast<double>(RAND_MAX));
    }
    emit newPoint(pt);
}

void SignalGenerator::generateHeartbeat()
{
    m_tickstate = 1;
}
