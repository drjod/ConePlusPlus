#include "input.h"
#include "wizard.h"


CInput::CInput()
{
    data = new CData();
    inputReadFlag = false; // nothing read yet
}

/////////////////////////////////////////////////////////////////////////////
/// \brief CInput::read - controls access to input data file when reading
/// \param accessFromInitPage true: if called after initPage (from aquiferPage::initialize())
///                                 then input always read to allow modification in inputFileName
///                           false: else - called from runPage or viewPage
///                                  read than only if input has not been read yet
///                                  this is the case if user has not selected 'modify input data' in initPage

void CInput::read(bool accessFromInitPage){
    if ( inputReadFlag == false || accessFromInitPage == true )
    {
      data->readData(path + "/" + inputFileName, inputReadFlag);
      inputReadFlag = true;
    }
}

