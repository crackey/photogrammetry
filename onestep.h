#ifndef ONESTEP_H
#define ONESTEP_H

#include <QObject>

class QString;

class Onestep : public QObject
{
    Q_OBJECT
public:
    Onestep(QString ctl, QString pht, QObject* parent);
    ~Onestep();

    bool calculate();
    int orient(double** o);
    int result(int** index, double** r);

private:
    bool exact(double* x, int n);
    double z_(double* orient, double* ctl);
    void prepare(double* a, double*l, double* pht, double* ctl, double* o, int* idx, 
            double f, int npht, int nmatched);
    double* m_result;
    double m_orient[12];
    int* m_index;
    int m_pnum;
    QString m_ctl;
    QString m_pht;
};

#endif

