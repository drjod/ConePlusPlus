#ifndef WELLSMODEL_H
#define WELLSMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include "input.h"

const int COLS_WELLS = 5;  // ndx, x, y, pumpingRate, timeSeriesName
const int ROWS_WELLS = 100;  // maximum number of wells

///////////////////////////////////////////////////////////////////////////
/// \brief The WellsModel class
/// to modify already existing wells in table
///
class WellsModel : public QAbstractTableModel
{
    Q_OBJECT
  public:
    WellsModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE ;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    void setInput(CInput *_input){ input = _input;}

  private:
    CInput *input;
    int numberOfNodes; // is number of rows
    QString m_gridData[ROWS_WELLS][COLS_WELLS];

  //Q_SIGNALS:
  //  void editCompleted(QString [][COLS_WELLS] );
};


#endif
