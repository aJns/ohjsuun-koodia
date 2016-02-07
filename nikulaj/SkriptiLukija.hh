#ifndef SKRIPTILUKIJA_HH
#define SKRIPTILUKIJA_HH


// std
#include <iostream>

// Qt
#include <QMap>
#include <QList>
#include <QString>

// lua
#include "lua.hpp"

// joanka
#include "objekti.hh"


namespace joanka {
    typedef QMap<QString, QString> stringMap;
    class SkriptiLukija {
    public:
        SkriptiLukija(QString tiedostonimi="alustus");
        ~SkriptiLukija();

        QList<QString> taistelijaTyypit();
        QList<QString> varusteTyypit();
        stringMap lueObjekti(ObjektiTyyppi tyyppi=TAISTELIJA,
                QString objektitunnus="");
        QList<stringMap> lueKentta(QString kenttatunnus="");
        void paivitaKentanKoko();

    private:
        lua_State* luaYmparisto_;
        int luaTila;
        QList<QString> mahdollisetTaistelijat_;
        QList<QString> mahdollisetVarusteet_;

        void lataaSkriptiTiedosto(std::string tiedostonimi);
        void tulostaVirheet();
        void tulostaTaulukko(lua_State* L);
        void lisaaTaulukkoon(lua_State* L, stringMap& taulukko);
        void lisaaKenttaListaan(lua_State* L, QList<stringMap>& lista);
    };
}


#endif // SKRIPTILUKIJA_HH
