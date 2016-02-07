// std
#include <memory>


#include "TestObjektiTehdas.hh"


void TestObjektiTehdas::testaaTaistelijanLuonti() {
    joanka::Objekti* objektiPtr =
        joanka::ObjektiTehdas::instanssi().luoObjekti(joanka::TAISTELIJA);

    if(objektiPtr == nullptr) {
        QFAIL("luoObjekti() palauttaa nullptr!");
    }

    delete objektiPtr;
}

void TestObjektiTehdas::testaaVarusteenLuonti() {
    joanka::Objekti* objektiPtr =
        joanka::ObjektiTehdas::instanssi().luoObjekti(joanka::VARUSTE);

    if(objektiPtr == nullptr) {
        QFAIL("luoObjekti() palauttaa nullptr!");
    }

    delete objektiPtr;
}

void TestObjektiTehdas::testaaKentanLuonti() {
    QList<joanka::Objekti*> kenttalista =
        joanka::ObjektiTehdas::instanssi().generoiKentta();
    QCOMPARE(kenttalista.size(), 25);

    kenttalista.clear();
}
