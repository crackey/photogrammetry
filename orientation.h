#ifndef ORIENTATION_H
#define ORIENTATTION_H

class Orientation : public QObject
{
    Q_OBJECT
public:
    Orientation(QString ctl, QString pht, QObject* parent = 0);
    ~Orientation();

    bool relative();
    bool absolute();

private:
    int photoData(double** data);
    double m_orient[12]; // 0-5 was orient elements for left photo which
                         // should normally be 0, 6-11 was the relative orientation
                         // of the right photo. m_orient[11] is Bx.
    QString m_pht;
    QString m_ctl;
};

#endif

