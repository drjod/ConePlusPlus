#include "nodesModel.h"
#include <cmath>
#include <QMessageBox>


NodesModel::NodesModel(QObject *parent)
    :QAbstractTableModel(parent)
{}

int NodesModel::rowCount(const QModelIndex & /*parent*/) const
{
    return ROWS_NODES;
}

int NodesModel::columnCount(const QModelIndex & /*parent*/) const
{
    return COLS_NODES;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief NodesModel::data - values to display in table
/// \param index: row, colum
/// \param role: default Qt::DisplayRole
/// \return generated QVariant()
///
QVariant NodesModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
      if ( index.row() < (int)input->data->getOutputDatas()[1].outputNodes.size() )
      {
      if (index.column() == 0)
         return index.row();
      else if  (index.column() == 1)
         return QString::number(input->data->getOutputDatas()[1].outputNodes[index.row()].x);
      else if   (index.column() == 2)
         return QString::number(input->data->getOutputDatas()[1].outputNodes[index.row()].y);
      // else error
      }
    }
    return QVariant();
}

/////////////////////////////////////////////////////////////////////////////
/// \brief NodesModel::setData - to change input (input->data->outputs[1].outputNodes[index.row()])
/// according to change of output node location in table
/// \param index: column and row in table
/// \param value: new value
/// \param role: default Qt::EditRole
/// \return true
///
bool NodesModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (role == Qt::EditRole)
  {
     //save value from editor to member m_gridData
     m_gridData[index.row()][index.column()] = value.toString();
        // m_gridData contains values which have been modified in table, the other values not!
     if(index.column() == 1)
     {
       input->data->outputs[1].outputNodes[index.row()].x = value.toDouble();
     }
     else if(index.column() == 2)
     {
       input->data->outputs[1].outputNodes[index.row()].y = value.toDouble();
     }
     else
        QMessageBox::information(0,"error in NodesModel::setData","value cannot be modified for column "
                                 + index.column());
   }
   return true;
}

Qt::ItemFlags NodesModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief NodesModel::headerData - header of nodes table
/// \param section:
/// \param orientation:
/// \param role: default Qt::DisplayRole
/// \return generated QVariant()
///
QVariant NodesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Number");
            case 1:
                return QString("X-coord [m]");
            case 2:
                return QString("Y-coord [m]");
            }
        }
    }
    return QVariant();
}
