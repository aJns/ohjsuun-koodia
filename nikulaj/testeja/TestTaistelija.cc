#include "TestTaistelija.hh"


void TestTaistelija::testaaTaistelu() {
    QString nimi("pulsu");
    QString kuva("pulsu.png");
    int voima(3);
    int nopeus(2);
    int panssari(1);
    joanka::Taistelija taistelija1(nimi, kuva, voima, nopeus, panssari);
    joanka::Taistelija taistelija2(nimi, kuva, voima, nopeus, panssari);

    taistelija1.taistele(taistelija2);

    int oikeaTerveys1 = 100 + taistelija1.panssari() - taistelija2.voima();
    int oikeaTerveys2 = 100 + taistelija2.panssari() - taistelija1.voima();

    QCOMPARE(taistelija1.terveys(), oikeaTerveys1);

    QCOMPARE(taistelija2.terveys(), oikeaTerveys2);
}
