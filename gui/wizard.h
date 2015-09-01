#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>
#include <QtWidgets>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QObject>
#include <QDoubleValidator>
#include <QDir>
#include <QTableWidget>
#include <QProcess>
#include <QTextEdit>
#include <sstream>
#include <cmath>
#include "input.h"
#include "wellsModel.h"
#include "pumpseriesModel.h"
#include "nodesModel.h"
#include "tabDialog.h"

#define MINIMUM_WELL_TO_NODE_DISTANCE 1.

///////////////////////////////////////////////////////////////////////////
/// \brief The wizard class - GUI of Cone++ simulator
///
class wizard : public QWizard
{
  Q_OBJECT

  public:
    wizard(QWidget *parent = 0);
    enum { Page_Intro, Page_Aquifer, Page_Outputs, Page_Wells, Page_Run, Page_View };

  private:
    CInput* input;

  public Q_SLOTS:
    void showHelp();
};

//////////////////////////////////////////////////////////////////////////////
/// \brief The IntroPage class
/// 1. get input file data
/// 2. main menue
///
class IntroPage : public QWizardPage
{
  Q_OBJECT

  public:
    IntroPage(QWidget *parent = 0);
    int nextId() const Q_DECL_OVERRIDE;
    void setInput(CInput *_input){ input = _input;}

  private:
    CInput* input;
    QLabel *fileNameLabel, *inputLabel, *directoryLabel;
    QLineEdit *fileNameLineEdit;
    QRadioButton *modifyRadioButton, *runRadioButton, *viewRadioButton;
    QPushButton *browseButton;
    QComboBox *directoryComboBox;
    QGroupBox *inputBox, *selectionBox;

  public Q_SLOTS:
    void browse(); // for input file directory
    void setDirectory(int number){input->setPath(directoryComboBox->itemText(number));}

  protected:
    void initializePage() Q_DECL_OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////////
/// \brief The AquiferPage class
/// for input: aquifer parameter, 2D domain raster grid
///
class AquiferPage : public QWizardPage
{
  Q_OBJECT

  public:
    AquiferPage(QWidget *parent = 0);

    int nextId() const Q_DECL_OVERRIDE;
    void setInput(CInput *_input){ input = _input;}

  private:
    CInput *input;
    QLabel *transmissivityLabel, *storativityLabel;
    QLineEdit *transmissivityLineEdit, *storativityLineEdit;

    QLabel *lengthLabel, *resolutionLabel;
    QLabel *lengthXLabel, *lengthYLabel, *resolutionXLabel, *resolutionYLabel;
    QLineEdit *lengthXLineEdit, *lengthYLineEdit, *resolutionXLineEdit, *resolutionYLineEdit;

    QGroupBox *propertiesBox, *rastergridBox;

  protected:
    void initializePage() Q_DECL_OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////////
/// \brief The OutputsPage class
/// input: output nodes and time
/// already existing node locations can be modified in table
/// nodes can be added or deleted via pop-up windows
///
class OutputsPage : public QWizardPage
{
  Q_OBJECT

  public:
    OutputsPage(QWidget *parent = 0);
    int nextId() const Q_DECL_OVERRIDE;
    void setInput(CInput *_input){ input = _input;}

  private:
    CInput *input;
    NodesModel *nodesModel;
    QTableView *nodesTableView;

    QLabel *snapshotLabel, *timeseriesLabel;
    QLineEdit *snapshotLineEdit, *timeseriesLineEdit;

    QPushButton *addButton, *applyButton, *deleteButton;

    QLabel *locationXLabel, *locationYLabel, *nodeNumberLabel;
    QLineEdit *locationXLineEdit, *locationYLineEdit, *nodeNumberLineEdit;

    QLabel *endLabel;  // total simulation time
    QLabel *numberOfIntervalsLabel;
    QLineEdit *endLineEdit, *numberOfIntervalsLineEdit;

    QGroupBox *filesBox, *timesBox, *leftBox, *rightBox, *buttonBox; // for layouts

  public Q_SLOTS:
    void add(); // output node
    void apply_add();
    void _delete(); // output node
    void apply_delete();
    void outputNodesUpdate(QString gridData[][COLS_NODES]);

  protected:
    void initializePage() Q_DECL_OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////////
/// \brief The WellsPage class
/// input: already existing wells can be relocated or pumping rate changed
/// wells can be added or deleted via pup-up windows
/// time dependent pumping rate with tables (modified, added, deleted with pop-up windows)
///
class WellsPage : public QWizardPage
{
  Q_OBJECT

  public:
    WellsPage(QWidget *parent = 0);
    int nextId() const Q_DECL_OVERRIDE;
    void setInput(CInput *_input){ input = _input;}

private:
  CInput *input;
  WellsModel *wellsModel;
  PumpseriesModel *pumpseriesModel;

  QTableView *wellsTableView, *pumpseriesTableView;
  QString numberOfWells;

  QComboBox *pumpseriesBox, *pumpseriesBoxInAddWell;
  QGroupBox *wellButtonBox, *pumpseriesButtonBox;

  QPushButton *addWellButton, *applyWellButton, *deleteWellButton;
  QPushButton *showPumpseriesButton; // opens window to change selected table
  QPushButton *addPumpseriesButton, *deletePumpseriesButton, *applyPumpseriesButton;
  QPushButton *yesButton, *noButton, *closeButton;

  QLabel *wellNumberLabel, *locationXLabel, *locationYLabel;
  QLabel *pumpingRateLabel, *pumpseriesNameLabel, *deleteLabel;
  QLineEdit *wellNumberLineEdit, *locationXLineEdit, *locationYLineEdit;
  QLineEdit *pumpingRateLineEdit, *pumpseriesNameLineEdit;

  QVector<QString> pumpseriesName;
  int currentPumpseriesNumber; // in pumpseriesComboBox

  public Q_SLOTS:
    void addWell();
    void apply_addWell();
    void deleteWell();
    void apply_deleteWell();

    void modifyPumpseries();  // pumping rate time series
    void addPumpseries();
    void apply_addPumpseries();
    void deletePumpseriesPotentially(); // just needs confirmation now
    void deletePumpseries();

    void wellsUpdate(QString gridData[][COLS_WELLS]);
    void pumpseriesUpdate(QString gridData[][COLS_PUMPSERIES]);
    void setCurrentPumpseriesNumber(int _currentPumpseriesNumber);

  protected:
    void initializePage() Q_DECL_OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////////
/// \brief The RunPage class
/// provide name and path of simulator
/// safe and run simulations
///
class RunPage : public QWizardPage
{
  Q_OBJECT

  public:
    RunPage(QWidget *parent = 0);

    int nextId() const Q_DECL_OVERRIDE;
    void setInput(CInput *_input){ input = _input;}


  private:
    CInput *input;

    QProcess *process; // to run Cone++
    QPushButton *runButton, *browseButton;
    QTextEdit *screenoutputTextEdit; // for output from Cone++
    QString executablePath;
                         // name of executable obtained from register field

    QLabel *executableNameLabel, *directoryLabel;
    QLineEdit *executableNameLineEdit;

    QComboBox *directoryComboBox;
    QGroupBox *simulatorBox;

  public Q_SLOTS:
    void runSimulation();
    void processOutput();

    void browse();
    void setDirectory(int);

  protected:
    void initializePage() Q_DECL_OVERRIDE;
};

//////////////////////////////////////////////////////////////////////////////
/// \brief The ViewPage class
/// output: snapshots and timeseries
///
class ViewPage : public QWizardPage
{
  Q_OBJECT

  public:
    ViewPage(QWidget *parent = 0);
    void setInput(CInput *_input){ input = _input;}

  private:
    CInput *input;
    TabDialog *tabDialog;

  protected:
    void initializePage() Q_DECL_OVERRIDE;
};

QPushButton* createButton(const QString &, QWizardPage *, const char *);
QComboBox* createComboBox(const QString &text = QString());

bool checkNodeLocation( double, double, CInput*, double);
void nonnegativeDouble(QLineEdit*);
void nonnegativeInt(QLineEdit*);

#endif

