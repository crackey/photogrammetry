#ifndef ORIENTATION_H
#define ORIENTATTION_H

#include <QObject>

class QString;

class Orientation : public QObject
{
    Q_OBJECT
public:
    Orientation(QString ctl, QString pht, QObject* parent = 0);
    ~Orientation();

    int result(int** index, double** data) const;
    int relativeOrientElements(double** data, double** s) const;
    bool relative();
    bool absolute();

private:
    bool exact(double* data);

//    int photoData(double** data, double* f);
    double m_orient[12]; // 0-5 was orient elements for left photo which
                         // should normally be 0, 6-11 for right photo
    double m_limit;
    double* m_rol;    // relative orient elements.
    double* m_rols;   // relative orient elements residual.
    double m_aol[7];    // absolute orient elements.
    double* m_modelpoints;
    double* m_result;
    int* m_index;
    int m_phtnum;
    QString m_pht;
    QString m_ctl;
};

#endif

