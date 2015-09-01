#ifndef PUMPSERIESMODEL_H
#define PUMPSERIESMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include "input.h"

const int COLS_PUMPSERIES = 2;   // time, value
const int ROWS_PUMPSERIES = 100; // maximum number of time series

///////////////////////////////////////////////////////////////////////////
/// \brief The PumpseriesModel class
/// to modify already existing tables for time dependent pumping rates
/// pumping rate is multiplied with value from here later in the simulations
///
/// if simulation time steps are changed later, there is a mismatch
/// with time steps in the time series table (simulator gives warning)
///
class PumpseriesModel : public QAbstractTableModel
{
  Q_OBJECT

  public:
    PumpseriesModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE ;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    void setPumpseriesNumber(int _pumpseriesNumber) {pumpseriesNumber = _pumpseriesNumber;}
    void setInput(CInput *_input){ input = _input;}

  private:
    CInput *input;
    QString m_gridData[ROWS_PUMPSERIES][COLS_PUMPSERIES];
    int pumpseriesNumber;

  Q_SIGNALS:
    void editCompleted(QString [][COLS_PUMPSERIES] );
};


#endif
