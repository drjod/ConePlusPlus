#include "input.h"
#include "data.h"
#include "wellsModel.h"
#include <QMessageBox>


WellsModel::WellsModel(QObject *parent)
    :QAbstractTableModel(parent)
{}

int WellsModel::rowCount(const QModelIndex & /*parent*/) const
{
    return ROWS_WELLS;
}

int WellsModel::columnCount(const QModelIndex & /*parent*/) const
{
    return COLS_WELLS;
}

QVariant WellsModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if ( index.row() < (int)input->data->getWellDatas().size() )
        {
        if (index.column() == 0)
           return index.row();
        else if  (index.column() == 1)
           return QString::number(input->data->getWellDatas()[index.row()].location[0]);
        else if   (index.column() == 2)
           return QString::number(input->data->getWellDatas()[index.row()].location[1]);
        else if   (index.column() == 3)
           return QString::number(input->data->getWellDatas()[index.row()].pumpingRate);
        else if   (index.column() == 4)
           return input->data->getWellDatas()[index.row()].pumpseriesName;
        // else error
        }
    }
    return QVariant();
}


bool WellsModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    bool table_flag = false; // to check if table exists

    if (role == Qt::EditRole)
    {
        //save value from editor to member m_gridData
        m_gridData[index.row()][index.column()] = value.toString();
        if(index.column() == 1)
            input->data->wells[index.row()].location[0] = value.toDouble();
        if(index.column() == 2)
            input->data->wells[index.row()].location[1] = value.toDouble();
        if(index.column() == 3)
        {
            if (value.toDouble() >= 0)
              input->data->wells[index.row()].pumpingRate = value.toDouble();
            else
               QMessageBox::information(0,"error","Pumping rate must be positive");
        }
        if(index.column() == 4) {
            if(value.toString() == "No")
                table_flag = true;
            for(int i = 0; i <  (int)input->data->pumpseries_vec.size(); i++)
                if(value.toString() == input->data->pumpseries_vec[i].name)
                   table_flag = true;

            if (table_flag == true)
              input->data->wells[index.row()].pumpseriesName = value.toString();
        }
    }
    return true;
}

Qt::ItemFlags WellsModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief NodesModel::headerData - header of wells table
/// \param section:
/// \param orientation:
/// \param role: default Qt::DisplayRole
/// \return generated QVariant()
///
QVariant WellsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Number");
            case 1:
                return QString("X-coord");
            case 2:
                return QString("Y-coord");
            case 3:
                return QString("Pumping rate");
            case 4:
                return QString("Time series name");
            }
        }
    }
    return QVariant();
}
