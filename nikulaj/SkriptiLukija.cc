// std
#include <iostream>
#include <random>
#include <algorithm>

// Qt
#include <QTime>

// joanka
#include "Poikkeukset.hh"
#include "ObjektiTehdas.hh"


#include "SkriptiLukija.hh"


// satunnaislukugeneraattori
typedef std::mt19937 RNG;  // Mersenne Twister
RNG skriptiRNG;  

joanka::SkriptiLukija::SkriptiLukija(QString tiedostonimi)
    :luaTila(0)
{
    skriptiRNG.seed(QTime::currentTime().msecsSinceStartOfDay());

    luaYmparisto_ = luaL_newstate();
    luaL_openlibs(luaYmparisto_);

    lataaSkriptiTiedosto(tiedostonimi.toStdString());
    lua_getglobal(luaYmparisto_, "alustaRandom");
    lua_call(luaYmparisto_, 0, 0);

    // Otetaan lista määritellyistä taistelijoista
    lua_getglobal(luaYmparisto_, "mahdollisetTaistelijat");
    lua_call(luaYmparisto_, 0, 1);

    lua_pushnil(luaYmparisto_);
    if(!lua_istable(luaYmparisto_, -2)) {
        throw joanka::LuaError("tyhjä taulukko!");
    }
    lua_gettable(luaYmparisto_, -2);

    while(lua_next(luaYmparisto_, -2) != 0)
    {
        if(lua_isstring(luaYmparisto_, -1))
            mahdollisetTaistelijat_.append(lua_tostring(luaYmparisto_, -1));
        lua_pop(luaYmparisto_, 1);
    }

    // ja sama varusteille
    lua_getglobal(luaYmparisto_, "mahdollisetVarusteet");
    lua_call(luaYmparisto_, 0, 1);

    lua_pushnil(luaYmparisto_);
    if(!lua_istable(luaYmparisto_, -2)) {
        throw joanka::LuaError("tyhjä taulukko!");
    }
    lua_gettable(luaYmparisto_, -2);

    while(lua_next(luaYmparisto_, -2) != 0)
    {
        if(lua_isstring(luaYmparisto_, -1))
            mahdollisetVarusteet_.append(lua_tostring(luaYmparisto_, -1));
        lua_pop(luaYmparisto_, 1);
    }

    /* for(auto stringi : mahdollisetTaistelijat_) { */
    /*     std::cout << stringi.toStdString() << std::endl; */
    /* } */
    /* for(auto stringi : mahdollisetVarusteet_) { */
    /*     std::cout << stringi.toStdString() << std::endl; */
    /* } */
}

joanka::SkriptiLukija::~SkriptiLukija() {
    lua_close(luaYmparisto_);
}

joanka::stringMap joanka::SkriptiLukija::lueObjekti(ObjektiTyyppi tyyppi,
        QString objektitunnus) {
    joanka::stringMap arvot;
    if(tyyppi == ObjektiTyyppi::TAISTELIJA)
    {
        lua_getglobal(luaYmparisto_, "luoTaistelija");
        if(objektitunnus == "") {
            std::shuffle(mahdollisetTaistelijat_.begin(),
                    mahdollisetTaistelijat_.end(), skriptiRNG);
            objektitunnus = mahdollisetTaistelijat_.first();
        }
    }
    else if(tyyppi == ObjektiTyyppi::VARUSTE) 
    {
        lua_getglobal(luaYmparisto_, "luoVaruste");
        if(objektitunnus == "") {
            std::shuffle(mahdollisetVarusteet_.begin(),
                    mahdollisetVarusteet_.end(), skriptiRNG);
            objektitunnus = mahdollisetVarusteet_.first();
        }
    }
    else
    {
        std::cout << "SkriptiLukija::lueObjekti: Virheellinen tyyppi!"
            << std::endl;
    }

    lua_pushstring(luaYmparisto_, objektitunnus.toStdString().c_str());

    lua_call(luaYmparisto_, 1, 1);
    /* tulostaTaulukko(luaYmparisto_); */
    lisaaTaulukkoon(luaYmparisto_, arvot);

    return arvot;
}

QList<joanka::stringMap> joanka::SkriptiLukija::lueKentta(QString kenttatunnus) {
    paivitaKentanKoko();
    QList<joanka::stringMap> kentta;
    lua_getglobal(luaYmparisto_, "luoKentta");
    lua_pushstring(luaYmparisto_, kenttatunnus.toStdString().c_str());
    lua_call(luaYmparisto_, 1, 1);

    /* tulostaTaulukko(luaYmparisto_); */
    lisaaKenttaListaan(luaYmparisto_, kentta);

    return kentta;
}

void joanka::SkriptiLukija::tulostaVirheet() {
    if (luaTila != 0) {
        std::cerr << "Lua virhe! " << "-- " << lua_tostring(luaYmparisto_, -1)
            << std::endl;
        lua_pop(luaYmparisto_, 1);
    }
}

void joanka::SkriptiLukija::tulostaTaulukko(lua_State *L)
{
    lua_pushnil(L);
    lua_gettable(L, -2);

    while(lua_next(L, -2) != 0)
    {
        if(lua_isstring(L, -1))
            printf("%s = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
        else if(lua_isnumber(L, -1))
            printf("%s = %f\n", lua_tostring(L, -2), lua_tonumber(L, -1));
        else if(lua_istable(L, -1))
            tulostaTaulukko(L);

        lua_pop(L, 1);
    }
}

void joanka::SkriptiLukija::lisaaTaulukkoon(lua_State* L,
        joanka::stringMap& taulukko) {

    lua_pushnil(L);
    if(!lua_istable(L, -2)) {
        throw joanka::LuaError("tyhjä taulukko!");
    }
    lua_gettable(L, -2);

    while(lua_next(L, -2) != 0)
    {
        if(lua_isstring(L, -1))
            taulukko[lua_tostring(L, -2)] =  lua_tostring(L, -1);
        else if(lua_isnumber(L, -1))
            taulukko[lua_tostring(L, -2)] =  lua_tostring(L, -1);
        else if(lua_istable(L, -1))
            lisaaTaulukkoon(L, taulukko);

        lua_pop(L, 1);
    }
}

void joanka::SkriptiLukija::lisaaKenttaListaan(lua_State* L,
        QList<joanka::stringMap>& lista) {

    lua_pushnil(L);
    if(!lua_istable(L, -2)) {
        throw joanka::LuaError("tyhjä taulukko!");
    }
    lua_gettable(L, -2);

    while(lua_next(L, -2) != 0)
    {
        if(lua_istable(L, -1)) {
            joanka::stringMap taulukko;
            lisaaTaulukkoon(L, taulukko);
            lista.append(taulukko);
        }

        lua_pop(L, 1);
    }
}

void joanka::SkriptiLukija::lataaSkriptiTiedosto(std::string tiedostonimi) {
    std::string tiedostopolku = "../skriptit/";
    tiedostopolku.append(tiedostonimi);
    tiedostopolku.append(".lua");
    std::cout << "Luetaan skripti: " << tiedostopolku << std::endl;
    luaTila = luaL_loadfile(luaYmparisto_, tiedostopolku.c_str());
    if(luaTila == 0) {
        luaTila = lua_pcall(luaYmparisto_, 0, LUA_MULTRET, 0);
    }
    tulostaVirheet();
}

void joanka::SkriptiLukija::paivitaKentanKoko() {
    joanka::stringMap kentanKoko;

    lua_getglobal(luaYmparisto_, "kentanKoko");
    lua_call(luaYmparisto_, 0, 1);
    lisaaTaulukkoon(luaYmparisto_, kentanKoko);
    int leveys = kentanKoko["leveys"].toInt();
    int korkeus = kentanKoko["korkeus"].toInt();
    std::cout << "kentan leveys " << leveys << " ja korkeus " << korkeus << std::endl;
    joanka::ObjektiTehdas::instanssi().asetaKentanKoko(leveys, korkeus);
}
