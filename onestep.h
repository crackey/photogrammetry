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
    QString m_ctl;
    QString m_pht;
};

#endif

