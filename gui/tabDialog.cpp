/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtWidgets>
#include "tabdialog.h"

///////////////////////////////////////////////////////////////////////////////////////
/// \brief TabDialog::TabDialog - hosts snapshots andd timeseries
/// \param parent
///
TabDialog::TabDialog(QWidget *parent)
   : QDialog(parent)
{
  tabWidget = new QTabWidget;
  snapshotTab = new SnapshotTab();
  timeseriesTab = new TimeseriesTab();
  tabWidget->addTab(snapshotTab, "Screenshot");
  tabWidget->addTab(timeseriesTab, "Time series");

  QVBoxLayout *mainLayout = new QVBoxLayout;
  mainLayout->addWidget(tabWidget);

  setLayout(mainLayout);
}

SnapshotTab::SnapshotTab(QWidget *parent)
    : QWidget(parent)
{   
    snapshotPlot = new SnapshotPlot( this );
    toolBar = new QToolBar( this );

    mapBox = new QComboBox( this );
    mapBox->addItem( "RGB" );
    mapBox->addItem( "Hue" );
    mapBox->addItem( "Alpha" );
    mapBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    timeBox = new QComboBox( this );
    timeBox->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );

    slider = new QSlider( Qt::Horizontal );
    slider->setRange( 0, 255 );
    slider->setValue( 255 );

    exportButton = new QPushButton( "Export", toolBar );
    contourButton = new QCheckBox( "Contour", toolBar );
    contourButton->setChecked( false );

    toolBar->addWidget( new QLabel("Color Map " ) );
    toolBar->addWidget( mapBox );
    toolBar->addWidget( new QLabel(" Time" ) );
    toolBar->addWidget( timeBox );
    toolBar->addWidget( new QLabel( " Opacity " ) );
    toolBar->addWidget( slider );
    toolBar->addWidget( new QLabel("   " ) );
    toolBar->addWidget( contourButton );
    toolBar->addWidget( exportButton );

    QObject::connect( mapBox, SIGNAL( currentIndexChanged( int ) ),
             snapshotPlot, SLOT( setColorMap( int ) ) );
    QObject::connect( timeBox, SIGNAL( currentIndexChanged( int ) ),
             snapshotPlot, SLOT( setTimeStep( int ) ) );
    QObject::connect( slider, SIGNAL( valueChanged( int ) ),
             snapshotPlot, SLOT( setAlpha( int ) ) );
    QObject::connect( contourButton, SIGNAL( toggled( bool ) ),
             snapshotPlot, SLOT( showContour( bool ) ) );
    QObject::connect( exportButton, SIGNAL( clicked() ),
             snapshotPlot, SLOT( exportPlot() ) );

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(snapshotPlot);
    setLayout(mainLayout);
}

TimeseriesTab::TimeseriesTab(QWidget *parent)
    : QWidget(parent)
{
    timeseriesPlot = new TimeseriesPlot( this );

    toolBar = new QToolBar( this );
    exportButton = new QPushButton( toolBar );
    exportButton->setText( "Export" );
    toolBar->addWidget( exportButton );

    QObject::connect( exportButton, SIGNAL( clicked() ), timeseriesPlot, SLOT( exportPlot() ) );

    QVBoxLayout *mainLayout = new QVBoxLayout( this );;
    mainLayout->setContentsMargins( 0, 0, 0, 0 );
    mainLayout->addWidget(toolBar);
    mainLayout->addWidget(timeseriesPlot);
    setLayout(mainLayout);
}




void SnapshotTab::update()
{
  timeBox->clear();
  for(int i=0; i < getSnapshotPlot()->getSnapshots().size(); i++)
    timeBox->addItem( QString::number(getSnapshotPlot()->getSnapshots()[i]->getTime()));

  getSnapshotPlot()->setTimeStep(0);  // 0 certainly exists
  getSnapshotPlot()->update();
}



