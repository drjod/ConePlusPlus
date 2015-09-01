#include "wizard.h"
#include <QMessageBox>
#include <unistd.h>

#include <QRegExp>
#include <QRegExpValidator>
class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QComboBox;

wizard::wizard(QWidget *parent)
    : QWizard(parent)
{
    setWindowTitle("Cone++");

    IntroPage *introPage = new IntroPage;
    AquiferPage *aquiferPage = new AquiferPage;
    OutputsPage *outputsPage = new OutputsPage;
    WellsPage *wellsPage = new WellsPage;
    RunPage *runPage = new RunPage;
    ViewPage *viewPage = new ViewPage;

    input = new CInput();
    // propagate input through pages
    introPage->setInput(input);
    aquiferPage->setInput(input);
    outputsPage->setInput(input);
    wellsPage->setInput(input);
    runPage->setInput(input);
    viewPage->setInput(input);

    addPage(introPage);
    addPage(aquiferPage);
    addPage(outputsPage);
    addPage(wellsPage);
    addPage(runPage);
    addPage(viewPage);

    setStartId(Page_Intro); // first page shown
    QObject::connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));

#ifndef Q_OS_MAC
    setWizardStyle(ModernStyle);
#endif
    setOption(HaveHelpButton, true);
    //setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo.png"));
}

void wizard::showHelp()
{
    QString message;

    switch (currentId())
    {
      case Page_Intro:
        message = "Provide input file name and path. "
                  "You can modify input data next. "
                  "You can also directly run simulations or view plots if they already exist.";
        break;
      case Page_Aquifer:
        message = "Enter positive values in SI-units. "
                  "Rectangular (raster) grid with equidistant nodes.";
        break;
      case Page_Outputs:
        message = "Simulations generate one snapshot and one time series plot. "
                  "Nodes in time series plots are located in the cell centre of the raster grid. "
                  "Output time steps are equidistant.";
        break;
      case Page_Wells:
        message = "Take time series tables for time-dependent pumping rates. "
                  "The pumping rate is then mutliplied by the value in the time series "
                  "table each time step for a specific well. "
                  "Give values in SI units (e.g. pumping rate in m<sup>3</sup>/s).";
        break;
      case Page_Run:
        message = "Provide path to simulator. "
                  "Then run simulations.";
        break;
      case Page_View:
        message = "Select time for snapshot. "
                  "Use mouse to zoom in and out.";
        break;
      default:
        message = "Sorry, I cannot help you now.";
    }
    QMessageBox::information(this, "Cone++ Help", message);
}

void IntroPage::initializePage()
{
  fileNameLineEdit->setText("pump.in"); // field
  input->setPath(QDir::currentPath());
}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle("Welcome to Cone++");

    // upper part - get input file name and path
    fileNameLabel = new QLabel("Name:");
    fileNameLineEdit = new QLineEdit;

    browseButton = createButton("Browse...", this, SLOT(browse()));
    modifyRadioButton = new QRadioButton("Modify input data");
    runRadioButton = new QRadioButton("Run simulation");
    viewRadioButton = new QRadioButton("View results");
    modifyRadioButton->setChecked(true);
    directoryLabel = new QLabel("Directory:");
    directoryComboBox = createComboBox(QDir::currentPath());

    inputBox = new QGroupBox("Input file");
    QGridLayout *inputLayout = new QGridLayout;
    inputLayout->addWidget(fileNameLabel,0,0);
    inputLayout->addWidget(fileNameLineEdit,0,1);
    inputLayout->addWidget(directoryLabel,1,0);
    inputLayout->addWidget(directoryComboBox,1,1);
    inputLayout->addWidget(browseButton, 1, 2);
    inputBox->setLayout(inputLayout);

    registerField("fileName", fileNameLineEdit);
    QObject::connect(directoryComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(setDirectory(int)));

    // lower part - user can select to modify parameter from input file
    // or directly go to simulations / plots
    selectionBox = new QGroupBox("Select");
    QVBoxLayout *selectionBoxLayout = new QVBoxLayout;
    selectionBoxLayout->addWidget(modifyRadioButton);
    selectionBoxLayout->addWidget(runRadioButton);
    selectionBoxLayout->addWidget(viewRadioButton);
    selectionBox->setLayout(selectionBoxLayout);

    QGridLayout *selectLayout = new QGridLayout;
    selectLayout->addWidget(inputBox, 0, 0);
    selectLayout->addWidget(selectionBox, 2, 0, 2, 2);
    setLayout(selectLayout);
}

void IntroPage::browse()
{
    input->setPath(QFileDialog::getExistingDirectory(this,
                               "Select directory", QDir::currentPath()));

     if (directoryComboBox->findText(input->getPath()) == -1)
     {
        directoryComboBox->addItem(input->getPath());
     }
     directoryComboBox->setCurrentIndex(directoryComboBox->findText(input->getPath()));
}

void AquiferPage::initializePage()
{
     // pass data from introPage to simulator input
     input->setInputFileName(field("fileName").toString());
     input->read(true);

     // initialize fields of aquiferPage
     storativityLineEdit->setText(QString::number(input->data->getProperties().storativity));
     transmissivityLineEdit->setText(QString::number(input->data->getProperties().transmissivity));
     lengthXLineEdit->setText(QString::number(input->data->getGrid().length[0]));
     lengthYLineEdit->setText(QString::number(input->data->getGrid().length[1]));
     resolutionXLineEdit->setText(QString::number(input->data->getGrid().resolution[0]));
     resolutionYLineEdit->setText(QString::number(input->data->getGrid().resolution[1]));
}

AquiferPage::AquiferPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle("Aquifer");

    // aquifer properties
    storativityLabel = new QLabel("Storativity [m<sup>-1</sup>]:");
    transmissivityLabel = new QLabel("Transmissivity [m<sup>2</sup>/s]:");

    storativityLineEdit = new QLineEdit;
    transmissivityLineEdit = new QLineEdit;

    nonnegativeDouble(storativityLineEdit);
    nonnegativeDouble(transmissivityLineEdit);

    registerField("transmissivity", transmissivityLineEdit);
    registerField("storativity", storativityLineEdit);

    // domain grid
    lengthLabel = new QLabel("Length [m]");
    resolutionLabel = new QLabel("Resolution [m]");
    lengthXLabel = new QLabel("in x direction:");
    lengthYLabel = new QLabel("in y direction:");
    resolutionXLabel = new QLabel("in x direction:");
    resolutionYLabel = new QLabel("in y direction:");

    resolutionXLineEdit = new QLineEdit;
    resolutionYLineEdit = new QLineEdit;
    lengthXLineEdit = new QLineEdit;
    lengthYLineEdit = new QLineEdit;

    nonnegativeDouble(resolutionXLineEdit);
    nonnegativeDouble(resolutionYLineEdit);
    nonnegativeDouble(lengthXLineEdit);
    nonnegativeDouble(lengthYLineEdit);

    registerField("lengthX", lengthXLineEdit);
    registerField("resolutionX", resolutionXLineEdit);
    registerField("lengthY", lengthYLineEdit);
    registerField("resolutionY", resolutionYLineEdit);

    // layouts
    propertiesBox = new QGroupBox("Properties");
    rastergridBox = new QGroupBox("Raster grid");

    QGridLayout *propertiesLayout = new QGridLayout;
    propertiesLayout->addWidget(storativityLabel, 0, 0);
    propertiesLayout->addWidget(storativityLineEdit, 0, 1);
    propertiesLayout->addWidget(transmissivityLabel, 0, 2);
    propertiesLayout->addWidget(transmissivityLineEdit, 0, 3);
    propertiesBox->setLayout(propertiesLayout);

    QGridLayout *rastergridLayout = new QGridLayout;
    rastergridLayout->addWidget(lengthLabel, 2, 0);
    rastergridLayout->addWidget(lengthXLabel, 3, 0);
    rastergridLayout->addWidget(lengthXLineEdit, 3, 1);
    rastergridLayout->addWidget(lengthYLabel, 3, 2);
    rastergridLayout->addWidget(lengthYLineEdit, 3, 3);
    rastergridLayout->addWidget(resolutionLabel, 4, 0);
    rastergridLayout->addWidget(resolutionXLabel, 5, 0);
    rastergridLayout->addWidget(resolutionXLineEdit, 5, 1);
    rastergridLayout->addWidget(resolutionYLabel, 5, 2);
    rastergridLayout->addWidget(resolutionYLineEdit, 5, 3);
    rastergridBox->setLayout(rastergridLayout);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(propertiesBox, 0, 0);
    mainLayout->addWidget(rastergridBox, 2, 0);
    setLayout(mainLayout);
}

void OutputsPage::initializePage()
{
    // pass data from aquiferPage to simulator input
    properties aquifer_current;
    aquifer_current.storativity =  field("storativity").toDouble();
    aquifer_current.transmissivity =  field("transmissivity").toDouble();
    input->data->setProperties(aquifer_current);

    grid domain_current;
    domain_current.length[0] = field("lengthX").toDouble();
    domain_current.length[1] = field("lengthY").toDouble();
    domain_current.resolution[0] = field("resolutionX").toDouble();
    domain_current.resolution[1] = field("resolutionY").toDouble();
    input->data->setGrid(domain_current);

    // put to simulator and pass MINIMUM_WELL_TO_NODE_DISTANCE to there
    if ( 0.5 * domain_current.resolution[0] <= MINIMUM_WELL_TO_NODE_DISTANCE
         || 0.5 * domain_current.resolution[1] <= MINIMUM_WELL_TO_NODE_DISTANCE  )
        QMessageBox::information(0,"Warning","Nodes to close to place wells between them. "
                                   "Return to aquifer page and increase resolution.");

    // initialize fields of outputsPage
    snapshotLineEdit->setText(input->data->getOutputDatas()[0].outfileName);
    timeseriesLineEdit->setText(input->data->getOutputDatas()[1].outfileName);

    endLineEdit->setText(QString::number(input->data->getTimeData().end));
    numberOfIntervalsLineEdit->setText(QString::number(input->data->getTimeData().numberOfIntervals));

    // pass input pointer to output nodes table
    nodesModel->setInput(input);
}

OutputsPage::OutputsPage(QWidget *parent)
    : QWizardPage(parent)
{
  setTitle("Output");

  // simulation time
  endLabel = new QLabel("Total [s]");
  numberOfIntervalsLabel = new QLabel("Number of intervals");

  endLineEdit = new QLineEdit;
  numberOfIntervalsLineEdit = new QLineEdit;

  nonnegativeDouble(endLineEdit);
  nonnegativeInt(numberOfIntervalsLineEdit);

  registerField("timing_end", endLineEdit);
  registerField("timing_numberOfIntervals", numberOfIntervalsLineEdit);

  // snapshot
  snapshotLabel = new QLabel("Snapshot");
  snapshotLineEdit = new QLineEdit;

  // timeseries
  timeseriesLabel = new QLabel("Time series");
  timeseriesLineEdit = new QLineEdit;

  nodesTableView = new QTableView(this);
  nodesModel = new NodesModel(this);
  nodesTableView->setModel(nodesModel);
  //QObject::connect(nodesModel, SIGNAL(editCompleted(QString [][])),
  //            this, SLOT(outputNodesUpdate(QString [][])));

  registerField("snapshot_filename", snapshotLineEdit);
  registerField("timeseries_filename", timeseriesLineEdit);

  addButton = createButton("Add node", this, SLOT(add()));
  deleteButton = createButton("Delete node", this, SLOT(_delete()));

  // layouts
  filesBox = new QGroupBox("File names");
  QGridLayout *filesLayout = new QGridLayout;
  filesLayout->addWidget(snapshotLabel, 0, 0);
  filesLayout->addWidget(snapshotLineEdit, 0, 1);
  filesLayout->addWidget(timeseriesLabel, 1, 0);
  filesLayout->addWidget(timeseriesLineEdit, 1, 1);
  filesBox->setLayout(filesLayout);

  timesBox = new QGroupBox("Simulation time");
  QGridLayout *timesLayout = new QGridLayout;
  timesLayout->addWidget(endLabel);
  timesLayout->addWidget(endLineEdit);
  timesLayout->addWidget(numberOfIntervalsLabel);
  timesLayout->addWidget(numberOfIntervalsLineEdit);
  timesBox->setLayout(timesLayout);

  leftBox = new QGroupBox("General");
  QVBoxLayout *leftLayout = new QVBoxLayout;
  leftLayout->addWidget(filesBox);
  leftLayout->addWidget(timesBox);
  leftBox->setLayout(leftLayout);

  buttonBox = new QGroupBox("");
  QHBoxLayout *buttonLayout = new QHBoxLayout;
  buttonLayout->addWidget(addButton);
  buttonLayout->addWidget(deleteButton);
  buttonBox->setLayout(buttonLayout);

  rightBox = new QGroupBox("Time series nodes");
  QVBoxLayout *rightLayout = new QVBoxLayout;
  rightLayout->addWidget(nodesTableView);
  rightLayout->addWidget(buttonBox);
  rightBox->setLayout(rightLayout);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(leftBox, 0, 0);
  mainLayout->addWidget(rightBox, 0, 1);
  setLayout(mainLayout);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief OutputsPage::outputNodesUpdate - store output nodes from table
/// in input data for simulator
/// \param gridData
///
void OutputsPage::outputNodesUpdate(QString gridData[][COLS_NODES])
{
    QVector<outputNode> outputNodes;
    for(int i = 0; i < (int)input->data->getOutputDatas()[i].outputNodes.size() ; i++)
    {
        outputNode node_inst;

        node_inst.x=QString(gridData[i][1]).toDouble();
        node_inst.y=QString(gridData[i][2]).toDouble();

        outputNodes.push_back(node_inst);
    }
    input->data->setOutputNodes(outputNodes, 1);
}

void OutputsPage::add()
{
    QWidget *window = new QWidget;

    // location
    locationXLabel = new QLabel("X-coord [m]:");
    locationXLineEdit = new QLineEdit;
    nonnegativeDouble(locationXLineEdit);

    locationYLabel = new QLabel("Y-coord [m]:");
    locationYLineEdit = new QLineEdit;
    nonnegativeDouble(locationYLineEdit);

    input->data->modificationCounter++;
    registerField("node_x" +
                  QString::number(input->data->modificationCounter), locationXLineEdit);
    registerField("node_y" +
                  QString::number(input->data->modificationCounter), locationYLineEdit);

    applyButton = createButton("&Apply", this, SLOT(apply_add()));
    QObject::connect(applyButton,SIGNAL(clicked()),window,SLOT(close()));

    // layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(locationXLabel, 0, 0);
    layout->addWidget(locationXLineEdit, 0, 1);
    layout->addWidget(locationYLabel, 1, 0);
    layout->addWidget(locationYLineEdit, 1, 1);
    layout->addWidget(applyButton);

    window->setWindowTitle("Add node");
    window->setLayout(layout);
    window->show();
}

void OutputsPage::apply_add()
{
    // add new output node to input for simulator
    outputNode node_inst;

    node_inst.x =  QString(field("node_x" +
                                 QString::number(input->data->modificationCounter)).toString()).toDouble();
    node_inst.y =  QString(field("node_y" +
                                 QString::number(input->data->modificationCounter)).toString()).toDouble();

    if ( checkNodeLocation( node_inst.x, node_inst.y, input, 1.e-10) == true ) // lock if node is on raster grid
       input->data->addOutputNode(node_inst);
    else
       QMessageBox::information(0,"error","Node does not exist. "
                                          "Consider that nodes are located in the cell centre.");
}

void OutputsPage::_delete()
{
   QWidget *window = new QWidget;

   // output node number
    nodeNumberLabel = new QLabel("Node number:");
    nodeNumberLineEdit = new QLineEdit;
    nonnegativeInt(nodeNumberLineEdit);

    input->data->modificationCounter++;
    registerField("nodeNumber" +
                  QString::number(input->data->modificationCounter), nodeNumberLineEdit);

    applyButton = createButton("Apply", this, SLOT(apply_delete()));
    QObject::connect(applyButton,SIGNAL(clicked()),window,SLOT(close()));

    //layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nodeNumberLabel, 0, 0);
    layout->addWidget(nodeNumberLineEdit, 0, 1);
    layout->addWidget(applyButton);

    window->setWindowTitle("Delete node");
    window->setLayout(layout);
    window->show();
}

void OutputsPage::apply_delete()
{
    int nodeNumber = QString(field("nodeNumber" +
               QString::number(input->data->modificationCounter)).toString()).toInt();

    if ( nodeNumber < (int)input->data->getOutputNodes().size() && nodeNumber > -1 )
    {
       input->data->removeOutputNode(nodeNumber);
    }
    else
        QMessageBox::information(0,"error","Node number " + field("nodeNumber" +
                                 QString::number(input->data->modificationCounter)).toString() +
                                 " does not exist");
}

void WellsPage::initializePage()
{
    // pass data from outputPage to simulator input
    input->data->setOutputFilename ( field("snapshot_filename").toString(), 0 );
    input->data->setOutputFilename ( field("timeseries_filename").toString(), 1 );

    timeData timing_current;
    timing_current.end = field("timing_end").toDouble();
    timing_current.numberOfIntervals = field("timing_numberOfIntervals").toInt();
    input->data->setTiming(timing_current);

    // check if outputNodes location match with a grid node
    for (int i = 0; i < input->data->getOutputDatas()[1].outputNodes.size(); i++ )
    {
        if ( checkNodeLocation( input->data->getOutputDatas()[1].outputNodes[i].x,
                                input->data->getOutputDatas()[1].outputNodes[i].y,
                                input, 1.e-10 ) == false )
           QMessageBox::information(0,"warning","Node " + QString::number(i)
                                    + " does not exist on raster grid. "
                                    "Simulator will provide drawdown from closest grid node.");
    }

    // initialization of wellsPage
    if (pumpseriesBox->count() == 0)
    {             // comboBox for pumping rate time series not initialized - so initialize now
      pumpseriesBox->addItem("No");
      for(int i = 0; i <(int)input->data->getPumpseriesVector().size(); i++)
        pumpseriesBox->addItem(input->data->getPumpseriesVector()[i].name);
    }
    currentPumpseriesNumber = -1; // this exists for sure (ndx 0 in comboBox)
    pumpseriesBox->setCurrentIndex(0); // no time series

     // pass input pointer to wells table
    wellsModel->setInput(input);
}

WellsPage::WellsPage(QWidget *parent)
    : QWizardPage(parent)
{
  setTitle("Wells");

  // wells table
  wellsTableView = new QTableView(this);
  wellsModel = new WellsModel(this);
  wellsTableView->setModel(wellsModel);
  //QObject::connect(wellsModel, SIGNAL(editCompleted(QString [][])),
  //                             this, SLOT(wellsUpdate(QString [][])));

  addWellButton = createButton("Add well", this, SLOT(addWell()));
  deleteWellButton = createButton("Delete well", this, SLOT(deleteWell()));

  // buttons for tables to modify pumping rate
  showPumpseriesButton = createButton("Modify table", this,  SLOT(modifyPumpseries()));
  addPumpseriesButton = createButton("Add table", this, SLOT(addPumpseries()));
  deletePumpseriesButton = createButton("Delete table", this, SLOT(deletePumpseriesPotentially()));

  pumpseriesBox = new QComboBox( this );

  QObject::connect(pumpseriesBox, SIGNAL( currentIndexChanged(int)),
                   this, SLOT(setCurrentPumpseriesNumber(int)));

  // layouts
  wellButtonBox = new QGroupBox("");
  QHBoxLayout *wellButtonLayout = new QHBoxLayout;
  wellButtonLayout->addWidget(addWellButton);
  wellButtonLayout->addWidget(deleteWellButton);
  wellButtonBox->setLayout(wellButtonLayout);

  pumpseriesButtonBox = new QGroupBox("Time series tables");
  QHBoxLayout *pumpseriesButtonLayout = new QHBoxLayout;
  pumpseriesButtonLayout->addWidget(pumpseriesBox );
  pumpseriesButtonLayout->addWidget(showPumpseriesButton);
  pumpseriesButtonLayout->addWidget(addPumpseriesButton);
  pumpseriesButtonLayout->addWidget(deletePumpseriesButton);
  pumpseriesButtonBox->setLayout(pumpseriesButtonLayout);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(wellsTableView,0,0);
  mainLayout->addWidget(wellButtonBox,1,0);
  mainLayout->addWidget(pumpseriesButtonBox,5,0 );
  setLayout(mainLayout);
}

void WellsPage::addPumpseries()
{
    QWidget *window = new QWidget;

    // series table name
    pumpseriesNameLabel = new QLabel("Name:");
    pumpseriesNameLineEdit = new QLineEdit;

    input->data->modificationCounter++;
    registerField("pumpseriesName" +
                  QString::number(input->data->modificationCounter), pumpseriesNameLineEdit);

    applyPumpseriesButton = createButton("Apply", this, 0);

    QObject::connect(applyPumpseriesButton, SIGNAL(clicked()), this, SLOT( apply_addPumpseries()) );
    QObject::connect(applyPumpseriesButton, SIGNAL(clicked()),window, SLOT(close()));

    QObject::connect(pumpseriesNameLineEdit,SIGNAL(textChanged(QString)), this, SLOT(setPumpseriesInstanceName()));

    // layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(pumpseriesNameLabel, 0, 0);
    layout->addWidget(pumpseriesNameLineEdit, 0, 1);
    layout->addWidget(applyPumpseriesButton);

    window->setWindowTitle("Add table");
    window->setLayout(layout);
    window->show();
}

void  WellsPage::setCurrentPumpseriesNumber(int pumpBoxNDX) // slot
{
   currentPumpseriesNumber = pumpBoxNDX - 1;
}

void WellsPage::modifyPumpseries()
{
  if (currentPumpseriesNumber > -1)
  {
    QWidget *window = new QWidget;
    // table to change pumping rates in time series
    // time step values are set according to specification from outputPage
    // !!! time step values are not adapted if user goes back to outputPage
    // and changes timing specification later on (simulator gives warning).
    // in this case, the user has to generate new tables and delete the old ones
    pumpseriesTableView = new QTableView(this);
    pumpseriesModel = new PumpseriesModel(this);
    pumpseriesModel->setInput(input);
    pumpseriesModel->setPumpseriesNumber(currentPumpseriesNumber);
    pumpseriesTableView->setModel(pumpseriesModel);
    closeButton = createButton("Close", this, 0);

    QObject::connect(closeButton,SIGNAL(clicked()),window,SLOT(close()));

    // layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(pumpseriesTableView, 0, 0);
    layout->addWidget(closeButton, 1, 0);
    window->setLayout(layout);
    window->setWindowTitle("Time series table");
    window->show();
  }
  else
    QMessageBox::information(0,"Info","Select table");
}

void WellsPage::apply_addPumpseries()
{
    pumpseries pumpseries_inst;
    entry entry_inst;

    pumpseries_inst.name = field("pumpseriesName" +
                            QString::number(input->data->modificationCounter)).toString();

    // initialize with time steps known from outputPage
    // and one for the value that modifies pumping rate by multiplication
    // (meaning no modification of the pumping rate from the wells table in the simulations for now)
    pumpseries_inst.numberOfEntries = input->data->getTimeData().numberOfIntervals;
    for (int i = 0; i < pumpseries_inst.numberOfEntries; i++)
    {                                     // equidistant time steps from specifiaction in outputPage
        entry_inst.time = (i + 1) * input->data->getTimeData().end / pumpseries_inst.numberOfEntries;
        entry_inst.value = 1.; // initialization (no modification of pumpingRate, it is multiplied by one)
        pumpseries_inst.entries.push_back(entry_inst);
    }

    input->data->addPumpseries(pumpseries_inst);
    pumpseriesBox->addItem(pumpseries_inst.name);

}

/////////////////////////////////////////////////////////////////////////////
/// \brief WellsPage::pumpseriesUpdate - store data from pumping rate time series table
/// in input data for simulator
/// \param gridData
///
void WellsPage::pumpseriesUpdate(QString gridData[][COLS_PUMPSERIES])
{
    for(int i = 0; i < (int)input->data->getPumpseriesVector()[currentPumpseriesNumber].entries.size() ; i++)
    {
        input->data->getPumpseriesVector()[currentPumpseriesNumber].entries[i].time = gridData[i][0].toDouble();
        input->data->getPumpseriesVector()[currentPumpseriesNumber].entries[i].value = gridData[i][1].toDouble();
    }
}

void WellsPage::deletePumpseriesPotentially()
{
  if(currentPumpseriesNumber > -1)
  {              // delete table if yes is clicked (for confirmation),
                 // otherwise just close window
    QWidget *window = new QWidget;

    // yes and no button
    deleteLabel = new QLabel("delete table " +
                             input->data->getPumpseriesVector()[currentPumpseriesNumber].name + "?");

    yesButton = createButton("Yes", this,  0);
    noButton = createButton("No", this, 0);

    QObject::connect(yesButton,SIGNAL(clicked()),this,SLOT(deletePumpseries()));
    QObject::connect(yesButton,SIGNAL(clicked()),window,SLOT(close()));
    QObject::connect(noButton,SIGNAL(clicked()),window,SLOT(close()));

    // layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(deleteLabel , 0, 0);
    layout->addWidget(yesButton , 1, 0);
    layout->addWidget(noButton, 1, 1);

    window->setLayout(layout);
    window->setWindowTitle("Delete table");
    window->show();
  }
  else      // no table was selected
    QMessageBox::information(0,"Info","Select table");
}

void WellsPage::deletePumpseries()
{
  input->data->removePumpseries(currentPumpseriesNumber);

  // update comboBox
  pumpseriesBox->clear();
  pumpseriesBox->addItem("No");
  for(int i = 0; i <  (int)input->data->getPumpseriesVector().size(); i++)
    pumpseriesBox->addItem(  input->data->getPumpseriesVector()[i].name);

  currentPumpseriesNumber = -1;  // reset comboBox index (to no pumpseries)
  pumpseriesBox->setCurrentIndex(0); // first entry
                                //(no pumpseries <-> no modification of pumping rate)
                                // surely exists
}

/////////////////////////////////////////////////////////////////////////////
/// \brief WellsPage::wellsUpdate - store well data from table
/// in input data for simulator
/// \param gridData
///
void WellsPage::wellsUpdate(QString gridData[][COLS_WELLS])
{
   for(int i = 0; i < (int)input->data->getWellDatas().size() ; i++)
    {
      input->data->getWellDatas()[i].location[0]=QString(gridData[i][1]).toDouble();
      input->data->getWellDatas()[i].location[1]=QString(gridData[i][2]).toDouble();
      input->data->getWellDatas()[i].pumpingRate=QString(gridData[i][3]).toDouble();
    }
}

void WellsPage::addWell()
{
    QWidget *window = new QWidget;

    // Combo box to select pumping rate time series
    pumpseriesBoxInAddWell = new QComboBox(this);
    pumpseriesBoxInAddWell->clear();
    //pumpseriesBoxInAddWell->addItem( "No" );
    for(int i = 0; i < (int)pumpseriesBox->count(); i++)
      pumpseriesBoxInAddWell->addItem( pumpseriesBox->itemText(i));

    currentPumpseriesNumber = -1;  // reset comboBox index (to no pumpseries)
    pumpseriesBoxInAddWell->setCurrentIndex(0);

    QObject::connect(pumpseriesBoxInAddWell, SIGNAL( currentIndexChanged(int)),
                     this, SLOT(setCurrentPumpseriesNumber(int)));

    // location and pumping rate
    locationXLabel = new QLabel("X coordinate:");
    locationYLabel = new QLabel("Y coordinate:");
    pumpingRateLabel = new QLabel("Pumping rate:");
    pumpseriesNameLabel = new QLabel("Table name:");

    locationXLineEdit = new QLineEdit;
    locationYLineEdit = new QLineEdit;
    pumpingRateLineEdit = new QLineEdit;
    pumpseriesNameLineEdit = new QLineEdit;

    nonnegativeDouble(locationXLineEdit);
    nonnegativeDouble(locationYLineEdit);
    nonnegativeDouble(pumpingRateLineEdit);

    input->data->modificationCounter++;
    registerField("well_x" +
                  QString::number(input->data->modificationCounter), locationXLineEdit);
    registerField("well_y" +
                  QString::number(input->data->modificationCounter), locationYLineEdit);
    registerField("well_pumpingRate" +
                  QString::number(input->data->modificationCounter), pumpingRateLineEdit);

    // apply after values entered
    applyWellButton = createButton("Apply", this, SLOT(apply_addWell()));
    QObject::connect(applyWellButton,SIGNAL(clicked()),window,SLOT(close()));

    // layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(locationXLabel, 0, 0);
    layout->addWidget(locationXLineEdit, 0, 1);
    layout->addWidget(locationYLabel, 1, 0);
    layout->addWidget(locationYLineEdit, 1, 1);
    layout->addWidget(pumpingRateLabel, 2, 0);
    layout->addWidget(pumpingRateLineEdit, 2, 1);
    layout->addWidget(pumpseriesNameLabel, 3, 0);
    layout->addWidget(pumpseriesBoxInAddWell, 3, 1);
    layout->addWidget(applyWellButton);

    window->setWindowTitle("Add well");
    window->setLayout(layout);
    window->show();
}

void WellsPage::apply_addWell()
{
    // add new well to input for simulator
    wellData well_inst;
    well_inst.location[0] =  QString(field("well_x" +
                                           QString::number(input->data->modificationCounter)).toString()).toDouble();
    well_inst.location[1] =  QString(field("well_y" +
                                           QString::number(input->data->modificationCounter)).toString()).toDouble();
    well_inst.pumpingRate =  QString(field("well_pumpingRate" +
                                           QString::number(input->data->modificationCounter)).toString()).toDouble();

    if ( checkNodeLocation( well_inst.location[0],  // lock if well is on raster grid node
                            well_inst.location[1], input, MINIMUM_WELL_TO_NODE_DISTANCE ) == true )
    {
      QMessageBox::information(0,"error","Well to close to node." );
    }
    else
    {
      if (currentPumpseriesNumber == -1)
        well_inst.pumpseriesName = "No";
      else
        well_inst.pumpseriesName = input->data->getPumpseriesVector()[currentPumpseriesNumber].name;

      if (well_inst.pumpingRate >= 0)
        input->data->addWell(well_inst);
      else
        QMessageBox::information(0,"error","Pumping rate must be positive");
    }
}

void WellsPage::deleteWell()
{
    QWidget *window = new QWidget;

    // well number and apply button
    wellNumberLabel = new QLabel("Well number:");
    wellNumberLineEdit = new QLineEdit;

    registerField("wellNumber" + QString::number(input->data->modificationCounter), wellNumberLineEdit);

    applyWellButton = createButton("Apply", this, SLOT(apply_deleteWell()));
    QObject::connect(applyWellButton,SIGNAL(clicked()),window,SLOT(close()));

    // layout
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(wellNumberLabel, 0, 0);
    layout->addWidget(wellNumberLineEdit, 0, 1);
    layout->addWidget(applyWellButton);

    window->setWindowTitle("Delete well");
    window->setLayout(layout);
    window->show();
}

void WellsPage::apply_deleteWell()
{
    // delete well from input for simulator
    int nodeNumber = QString(field("wellNumber" +
                                   QString::number(input->data->modificationCounter)).toString()).toInt();

    if ( nodeNumber < (int)input->data->getWellDatas().size() && nodeNumber > -1 )
    {
       input->data->removeWell(nodeNumber);
    }
    else
        QMessageBox::information(0,"error", "Well number " +  field("wellNumber" +
                                                                    QString::number(input->data->modificationCounter)).toString() +
                                 " does not exist");

    input->data->modificationCounter++;
}

void RunPage::initializePage()
{
  // lock if wells are close to raster grid nodes
  // because than the calculated drawdown gets huge
  for ( int i=0; i < (int)input->data->getWellDatas().size(); i++ )
  {
    if ( checkNodeLocation( input->data->getWellDatas()[i].location[0],
                            input->data->getWellDatas()[i].location[1],
                            input, MINIMUM_WELL_TO_NODE_DISTANCE ) == true )
    {
      QMessageBox::information(0,"Warning","Well " + QString::number(i) + " to close to node." );
    }
  }

  // initialize runPage
  executableNameLineEdit->setText("simulator.exe");
  executablePath = QDir::currentPath();
}

RunPage::RunPage(QWidget *parent)
    : QWizardPage(parent)
{
  setTitle("Simulation");

  // name and path of executable
  executableNameLabel = new QLabel("Name:");
  directoryLabel = new QLabel("Directory:");
  executableNameLineEdit = new QLineEdit;

  registerField("executableName", executableNameLineEdit);
  directoryComboBox = createComboBox(QDir::currentPath());

  browseButton = createButton("Browse...", this, SLOT(browse()));
  QObject::connect(directoryComboBox, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(setDirectory(int)));

  // simulation start and output
  runButton = createButton("Save and Run", this, SLOT(runSimulation()));
  screenoutputTextEdit = new QTextEdit;

  // layout
  simulatorBox = new QGroupBox("Executable");
  QGridLayout *inputLayout = new QGridLayout;
  inputLayout->addWidget(executableNameLabel,0,0);
  inputLayout->addWidget(executableNameLineEdit,0,1);
  inputLayout->addWidget(directoryLabel,1,0);
  inputLayout->addWidget(directoryComboBox,1,1);
  inputLayout->addWidget(browseButton, 1, 2);
  simulatorBox->setLayout(inputLayout);

  QGridLayout *mainLayout = new QGridLayout;
  mainLayout->addWidget(screenoutputTextEdit, 0, 0);
  mainLayout->addWidget(runButton, 1, 0);
  mainLayout->addWidget(simulatorBox, 2, 0);
  setLayout(mainLayout);
}

void RunPage::runSimulation()
{
  // input for simulator
  if(input->getInputReadFlag() == true)
    input->write();
  else
  {
    input->setInputFileName(field("fileName").toString());
    input->read(false); // for direct simulation
                        // (without going through input file modification windows)
  }

  // generate run process
  process = new QProcess(this);
  QString executableFile =  executablePath + "/" + field("executableName").toString();

  if (! access( QString(executableFile).toStdString().c_str(), X_OK))
    process->start(executableFile);
  else
    QMessageBox::information(0,"error","No access to executable "
                             + executableFile + ".");

  // get simulator output into text Editor on window
  connect (process, SIGNAL(readyReadStandardOutput()),
           this, SLOT(processOutput()));
}

void RunPage::processOutput()
{
    QByteArray data = process->readAllStandardOutput();
    screenoutputTextEdit->append(QString(data));
}

void RunPage::browse()
{
    // select path to executable with combo box
    executablePath = QFileDialog::getExistingDirectory(this,
                               "Select directory", QDir::currentPath());

     if (directoryComboBox->findText(executablePath) == -1)
     {
        directoryComboBox->addItem(executablePath);
     }
     directoryComboBox->setCurrentIndex(directoryComboBox->findText(executablePath));
}

void RunPage::setDirectory(int number)
{
    executablePath = directoryComboBox->itemText(number);
}

void ViewPage::initializePage()
{
   if(input->getInputReadFlag() == false)
   {
     // get file names and paths if no input has been read
     input->setInputFileName(field("fileName").toString());
     input->read(false); // for direct simulation or plot view
                         // (without going through input file modification windows)
   }

   // snapshot
   QString snapshotFileName = input->getPath() + "/" + input->data->getOutputDatas()[0].outfileName;
   if( tabDialog->getSnapshotTab()->getSnapshotPlot()->readSnapshotFile(snapshotFileName) == true)
   {
     tabDialog->getSnapshotTab()->setMapBox(0); // RGB
     tabDialog->getSnapshotTab()->update();
   }
   else
       QMessageBox::information(0, "error", "ERROR - failed reading snapshots data");

   // timeseries
   QString timeseriesFileName = input->getPath() + "/" + input->data->getOutputDatas()[1].outfileName;
   if(  tabDialog->getTimeseriesTab()->getTimeseriesPlot()->readTimeseriesFile(timeseriesFileName) == true)
   {
     tabDialog->getTimeseriesTab()->getTimeseriesPlot()->update();
   }
   else
      QMessageBox::information(0, "error", "ERROR - failed reading timeseries data");
}

ViewPage::ViewPage(QWidget *parent)
    : QWizardPage(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    tabDialog = new TabDialog();

    layout->addWidget(tabDialog);
    setLayout(layout);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief ...Page::nextId - members to propagete through wizard pages
/// \return number: of next page
///
int IntroPage::nextId() const
{
   if (modifyRadioButton->isChecked())
   {
     return wizard::Page_Aquifer;
   }
   else if (viewRadioButton->isChecked())
   {
     return wizard::Page_View;
   }
   else
   {
     return wizard::Page_Run;
   }
}

int AquiferPage::nextId() const
{
    return wizard::Page_Outputs;
}

int OutputsPage::nextId() const
{
    return wizard::Page_Wells;
}

int WellsPage::nextId() const
{
    return wizard::Page_Run;
}

int RunPage::nextId() const
{
    return wizard::Page_View;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief createButton - it responds if clicked
/// \param text: label
/// \param page: of wizzard
/// \param member: SLOT
/// \return QPushButton*
///
QPushButton* createButton(const QString &text, QWizardPage *page, const char *member)
{
    QPushButton *button = new QPushButton(text);
    QObject::connect(button, SIGNAL(clicked()), page, member);
    return button;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief createComboBox - widget to select input and simulator path
/// \param text: current path
/// \return QComboBox*
///
QComboBox* createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief nonnegativeDouble - restricts line edit input to double >=0
/// \param lineEdit: input
///
void nonnegativeDouble(QLineEdit *lineEdit)
{
  QDoubleValidator *validator = new QDoubleValidator( 0., 1e10, 10); // min, max, numberOfDigits
  validator->setNotation(QDoubleValidator::ScientificNotation);
  lineEdit->setValidator(validator);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief nonnegativeInt - restricts line edit input to Int 0, 1, 2, 3, ...
/// \param lineEdit: input
///
void nonnegativeInt(QLineEdit *lineEdit)
{
  QIntValidator *validator = new QIntValidator( 1, 10000);// min, max
  lineEdit->setValidator(validator);
}


/////////////////////////////////////////////////////////////////////////////
/// \brief checkNodeLocation - if output or well node is at raster grid node
/// \param x: node location - x direction
/// \param y: node location - y direction
/// \param input: contains raster grid
/// \param range: minimum distance between node and grid nodes
/// \return true if node (x,y) closer than range to grid node, else false
///
bool checkNodeLocation( double x, double y, CInput *input, double range)
{

  double distanceSquared;
  double rangeSquared = range * range;
  double gridNode[2];

  for(gridNode[1] = 0.5 * input->data->getGrid().resolution[1]; gridNode[1] <= input->data->getGrid().length[1]; gridNode[1] += input->data->getGrid().resolution[1] )
  {
    for(gridNode[0] = 0.5 * input->data->getGrid().resolution[0]; gridNode[0] <= input->data->getGrid().length[0]; gridNode[0] += input->data->getGrid().resolution[0] )
    {
      distanceSquared = ( gridNode[0] - x ) * ( gridNode[0] - x ) +  ( gridNode[1] - y ) * ( gridNode[1] - y );
      if ( distanceSquared < rangeSquared )
         return true;
    }
  }
  return false;
}


