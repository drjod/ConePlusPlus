#ifndef NODESMODEL_H
#define NODESMODEL_H

#include <QAbstractTableModel>
#include <QString>
#include "input.h"


const int COLS_NODES = 3; // ndx, x-coord, y-coord
const int ROWS_NODES = 100; // maximum number of output nodes

///////////////////////////////////////////////////////////////////////////
/// \brief The NodesModel class
/// to modify already existing output nodes in table
///
class NodesModel : public QAbstractTableModel
{
    Q_OBJECT
  public:
    NodesModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex & index) const Q_DECL_OVERRIDE ;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
    void setInput(CInput *_input){ input = _input;}

  private:
    CInput *input;
    QString m_gridData[ROWS_NODES][COLS_NODES];

  //Q_SIGNALS:
  //  void editCompleted(QString [][COLS_NODES] );
};

extern bool checkNodeLocation( double, double, CInput*, double);

#endif
