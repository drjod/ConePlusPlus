


class CSnapshot
{
  private:
    double time;
    double *x, *y, *drawdown;
    int numberOfNodes[2];

  public:
    CSnapshot(double, int, int, int);
    ~CSnapshot();

    void readSnapshotData(QTextStream *in, int);
    double getTime(void);
    double getDrawdown(int);
    double getNumberOfNodes(int);

};


extern QVector<CSnapshot*> snapshots;

extern void readData(QString);
