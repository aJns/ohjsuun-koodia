// joanka
#include "Poikkeukset.hh"


#include "TestSkriptiLukija.hh"


TestSkriptiLukija::TestSkriptiLukija()
    : lukija_()
{}

void TestSkriptiLukija::testaaTaistelijanLukeminen() {
    int taistelijaMapinKoko = 5;

    joanka::stringMap map1 = lukija_.lueObjekti();
    QCOMPARE(map1.size(), taistelijaMapinKoko);

    joanka::stringMap map2 = lukija_.lueObjekti(joanka::TAISTELIJA);
    QCOMPARE(map2.size(), taistelijaMapinKoko);

    joanka::stringMap map3 = lukija_.lueObjekti(joanka::TAISTELIJA, "pulsu");
    QCOMPARE(map3.size(), taistelijaMapinKoko);

    QVERIFY_EXCEPTION_THROWN(lukija_.lueObjekti(joanka::TAISTELIJA, "lolmies"),
            joanka::LuaError);
}

void TestSkriptiLukija::testaaVarusteenLukeminen() {
    int varusteMapinKoko = 4;

    joanka::stringMap map1 = lukija_.lueObjekti(joanka::VARUSTE);
    QCOMPARE(map1.size(), varusteMapinKoko);

    joanka::stringMap map2 = lukija_.lueObjekti(joanka::VARUSTE, "miekka");
    QCOMPARE(map2.size(), varusteMapinKoko);

    QVERIFY_EXCEPTION_THROWN(lukija_.lueObjekti(joanka::VARUSTE, "lolpyssy"),
            joanka::LuaError);
}

void TestSkriptiLukija::testaaKentanLukeminen() {
    int kenttaMapinKoko = 4;

    QList<joanka::stringMap> list1 = lukija_.lueKentta();
    QCOMPARE(list1.size(), 25);
    for(auto kenttalaatta : list1) {
        QCOMPARE(kenttalaatta.size(), kenttaMapinKoko);
    }
    /* QVERIFY_EXCEPTION_THROWN(lukija_.lueKentta("lolkenttä"), joanka::LuaError); */
}
