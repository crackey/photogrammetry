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

private:
    bool exact(double* x, int n);
    double z_(double* orient, double* ctl);
    void ma(double* a, double* pht, double* ctl, double* o, int nc, int ictl, 
            int iup, double f, int npht, int nmatched, int i);
    double* m_result;
    double m_orient[12];
    int* m_index;
    QString m_ctl;
    QString m_pht;
};

#endif

