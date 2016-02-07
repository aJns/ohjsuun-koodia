#ifndef TESTTAISTELIJA
#define TESTTAISTELIJA


// Qt
#include <QtTest/QtTest>

#include "Taistelija.hh"


class TestTaistelija : public QObject
{
    Q_OBJECT
private slots:
    void testaaTaistelu();
};


#endif // TESTTAISTELIJA
