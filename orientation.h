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

    bool relative();
    bool absolute();

private:
    bool exact(double* data);

//    int photoData(double** data, double* f);
    double m_orient[12]; // 0-5 was orient elements for left photo which
                         // should normally be 0, 6-11 for right photo
    static const double m_limit = 3e-5;
    double m_rol[5];    // relative orient elements.
    double m_aol[7];    // absolute orient elements.
    double* m_modelpoints;
    QString m_pht;
    QString m_ctl;
};

#endif

