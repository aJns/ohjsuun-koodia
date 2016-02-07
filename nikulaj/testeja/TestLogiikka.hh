#ifndef TESTLOGIIKKA
#define TESTLOGIIKKA


// Qt
#include <QtTest/QtTest>


#include "Logiikka.hh"


class TestLogiikka : public QObject
{
    Q_OBJECT
private slots:
    void testaaLogiikanRakentaja();
};


#endif // TESTLOGIIKKA
