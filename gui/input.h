#ifndef INPUT_H
#define INPUT_H

#include "data.h"

///////////////////////////////////////////////////////////////////////////
/// \brief The CInput class
/// manages access to input data
///
class CInput
{
  private:
    QString path;
    QString inputFileName;
    bool inputReadFlag; // true if input has been read, false else
                        // to avoid multiple generation of same pumping rate time series
  public:
    CData* data;
    CInput();
    ~CInput();

    void read(bool);
    void write(){data->writeData(path + "/" + inputFileName);}

    void setPath(QString _path) {path = _path;}
    void setInputFileName(QString _inputFileName) {inputFileName = _inputFileName;}
    QString getPath(){ return path;}
    void resetInputReadFlag() {inputReadFlag = false;}

    bool getInputReadFlag() { return inputReadFlag;}
};


#endif
