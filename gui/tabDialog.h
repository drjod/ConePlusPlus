#ifndef TABDIALOG_H
#define TABDIALOG_H

#include "timeseriesPlot.h"
#include "snapshotPlot.h"
#include <QDialog>
#include <qwt_scale_draw.h>

///////////////////////////////////////////////////////////////////////////
/// \brief The MyScaleDraw class
/// to scale snapshot
///
class MyScaleDraw: public QwtScaleDraw
{
  private:
    double scalingFactor; // to scale domain

  public:
    MyScaleDraw( double _scalingFactor )
    {
        setTickLength( QwtScaleDiv::MajorTick, 10 );
        setTickLength( QwtScaleDiv::MinorTick, 2 );
        setTickLength( QwtScaleDiv::MediumTick, 0 );

        setLabelRotation( 0 );
        setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter );

        setSpacing( 10 );
        scalingFactor = _scalingFactor;
    }

    virtual QwtText label( double value ) const
    {
        QwtText h=QwtText(QString::number(value*scalingFactor));
        return h;
    }
};

///////////////////////////////////////////////////////////////////////////
/// \brief The SnapshotTab class
/// contains toolbar and snapshot plot
///
class SnapshotTab : public QWidget
{
  //  Q_OBJECT

  public:
    SnapshotPlot* getSnapshotPlot(void) {return snapshotPlot;}
    void setMapBox(int ndx) { mapBox->setCurrentIndex(ndx); }
    explicit SnapshotTab(QWidget *parent = 0);
    void update();

  private:
    SnapshotPlot *snapshotPlot;
    QToolBar *toolBar;
    QComboBox *mapBox, *timeBox;
    QSlider *slider;
    QCheckBox *contourButton;
    QPushButton *exportButton;
};

///////////////////////////////////////////////////////////////////////////
/// \brief The SnapshotTab class
/// contains export button and timeseries plot
///
class TimeseriesTab : public QWidget
{
   //Q_OBJECT

  public:
    explicit TimeseriesTab(QWidget *parent = 0);
    TimeseriesPlot* getTimeseriesPlot(void) {return timeseriesPlot;}


  private:
    TimeseriesPlot *timeseriesPlot;
    QToolBar *toolBar;
    QPushButton *exportButton;
};


class TabDialog  : public QDialog
{
 //   Q_OBJECT

public:
    explicit TabDialog(QWidget *parent = 0);

    SnapshotTab* getSnapshotTab(void) {return snapshotTab;}
    TimeseriesTab* getTimeseriesTab(void) {return timeseriesTab;}

private:
   QTabWidget *tabWidget;
   SnapshotTab *snapshotTab;
   TimeseriesTab *timeseriesTab;

};

#endif
