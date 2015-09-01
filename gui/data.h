#ifndef DATA_H
#define DATA_H

#include <QWizard>
#include <QVector>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cstring>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>


struct entry  // for pumping rate table
{
  double time;
  double value;

};

struct pumpseries
{                     // for wells
  int numberOfEntries;
  QString name; // to assign to wells (comparison with pumpseriesName)
  QVector<entry> entries;
};

struct properties  // aquifer
{
  double storativity;
  double transmissivity;

};

struct grid  // domain
{
   double length[2]; // x, y
   double resolution[2]; // x, y
};

struct wellData
{
   double location[2]; // x, y
   double pumpingRate;
   QString pumpseriesName; // time series to modify pumpingRate
};

struct timeData
{
    double end;
    int numberOfIntervals;
};

struct outputNode
{
    double x;
    double y;
};

struct outputData
{    
    QString outfileName;   // only name - path is path of input file from viewPage
    int outputTimingType;  // 0: all, 1: selected - only all suported by GUI
    int outputDistributionType; // 0: domain (screenshot), 1: nodes (timeSeries)
    QVector<outputNode> outputNodes; // for timeseries
};

///////////////////////////////////////////////////////////////////////////
/// \brief The CData class
/// hosts input data in structs
/// reading and writing of input data
///
class CData
{
  friend class WellsModel;
  friend class NodesModel;
  friend class PumpseriesModel;

  private:
    timeData timing;
    grid domain;
    properties aquifer;

    QVector<outputData> outputs;  // 0: snapshots, 1: timeseries
    QVector<wellData> wells;
    QVector<pumpseries> pumpseries_vec; // pumping rate time series

  public:
    CData();
    ~CData();

    // by using this counter, more than one
    // output node, well or pumping rate time series table can be
    // added or deleted in the pages later on (outputsPage, wellsPage)
    int modificationCounter;

    void readData(QString, bool);
    void writeData(QString);

    // getter
    properties getProperties() {return aquifer;}
    grid getGrid() {return domain;}

    timeData getTimeData() {return timing;}
    QVector<outputData> getOutputDatas() {return outputs;}
    QVector<outputNode> getOutputNodes() {return outputs[1].outputNodes;}

    QVector<wellData> getWellDatas() {return wells;}
    QVector<pumpseries> getPumpseriesVector() {return pumpseries_vec;} // for time-dependent pumping rates

    // setter
    void setProperties(properties _aquifer) {aquifer = _aquifer;}
    void setGrid(grid _domain) {domain = _domain;}

    void setTiming(timeData _timing) {timing = _timing;}
    void setOutputFilename(QString _filename, int ndx) {outputs[ndx].outfileName = _filename;}
    void setOutputNodes( QVector<outputNode>, int);

    void addOutputNode(outputNode node_inst) {outputs[1].outputNodes.push_back(node_inst);}
    void removeOutputNode(int node_ndx) {outputs[1].outputNodes.remove( node_ndx);}

    void addWell(wellData well_inst) {wells.push_back(well_inst);}
    void removeWell(int well_ndx) {wells.remove(well_ndx);} 
    void addPumpseries(pumpseries pumpseries_inst) {pumpseries_vec.push_back(pumpseries_inst);}
    void removePumpseries(int pumpseries_ndx) {pumpseries_vec.remove(pumpseries_ndx);}
};

#endif
