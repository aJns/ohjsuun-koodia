#ifndef TESTOBJEKTITEHDAS
#define TESTOBJEKTITEHDAS


// Qt
#include <QtTest/QtTest>


#include "ObjektiTehdas.hh"


class TestObjektiTehdas : public QObject
{
    Q_OBJECT
private slots:
    void testaaTaistelijanLuonti();
    void testaaVarusteenLuonti();
    void testaaKentanLuonti();
};


#endif // TESTOBJEKTITEHDAS
