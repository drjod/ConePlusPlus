#include "input.h"
#include "pumpseriesModel.h"
#include <QMessageBox>


PumpseriesModel::PumpseriesModel(QObject *parent)
    :QAbstractTableModel(parent)
{}

int PumpseriesModel::rowCount(const QModelIndex & /*parent*/) const
{
    return ROWS_PUMPSERIES;
}

int PumpseriesModel::columnCount(const QModelIndex & /*parent*/) const
{
    return COLS_PUMPSERIES;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief PumpModel::data - values to display in table
/// \param index: row, colum
/// \param role: default Qt::DisplayRole
/// \return generated QVariant()
///
QVariant PumpseriesModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
      if (index.row() < input->data->pumpseries_vec[pumpseriesNumber].numberOfEntries)
      {
        if (index.column() == 0)
        {
           return QString::number(
                       input->data->pumpseries_vec[pumpseriesNumber].entries[index.row()].time);
        }
        else if   (index.column() == 1)
        {
           return QString::number(
                       input->data->pumpseries_vec[pumpseriesNumber].entries[index.row()].value);
        }
      }
      // else error
    }

    return QVariant();
}

/////////////////////////////////////////////////////////////////////////////
/// \brief PumpModel::setData - to change input
/// (input->data->pumpseries_vec[pumpseriesNumber]->entries)
/// according to change in pumping rate in table (no change of time value through table)
/// \param index: column and row in table
/// \param value: new value
/// \param role: default Qt::EditRole
/// \return true
///
bool PumpseriesModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        //save value from editor to member m_gridData
        m_gridData[index.row()][index.column()] = value.toString();
        if(index.column() == 1)
        {
          if (value.toDouble() >= 0 && index.row()
                  < input->data->pumpseries_vec[pumpseriesNumber].numberOfEntries)
            input->data->pumpseries_vec[pumpseriesNumber].entries[index.row()].value = value.toDouble();
        }
    }
    return true;
}

Qt::ItemFlags PumpseriesModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief PumpModel::headerData - header of pumping rate table
/// \param section:
/// \param orientation:
/// \param role: default Qt::DisplayRole
/// \return generated QVariant()
///
QVariant PumpseriesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
  {
    if (orientation == Qt::Horizontal) {
       switch (section)
       {
         case 0:
           return QString("Time");
         case 1:
           return QString("Value");
       }
    }
  }
  return QVariant();
}
