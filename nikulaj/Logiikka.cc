// std
#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <algorithm>

// Qt
#include <QtCore/qmath.h>
#include <QDebug>
#include <QTime>

// joanka
#include "ObjektiTehdas.hh"
#include "Taistelija.hh"


#include "Logiikka.hh"

const int PISTE_RAJA(3);
const float TEKOALY_VIIVE_S(0.2);
// Staattinen jäsenmuuttuja täytyy "toteuttaa", eli määritellä uudestaan
// lähdetiedostossa
QList<joanka::KayttoliittymaViesti> joanka::Logiikka::kayttoliittymaViestit_;

// satunnaislukugeneraattori
typedef std::mt19937 RNG;  // Mersenne Twister
RNG logiikkaRNG;  

joanka::Logiikka::Logiikka()
: nykyinenNakyma_(joanka::VALIKKONAKYMA),
    tila_(joanka::Logiikka::VALIKOSSA),
    objektiLista_{},
    valittuObjekti_(nullptr),
    tekoaly_(nullptr),
    pelaajaVoitti_(false),
    havitytTaistelut_(0),
    voitetutTaistelut_(0),
    valuuttaaJaljella_(100)
{
    logiikkaRNG.seed(QTime::currentTime().msecsSinceStartOfDay());
    joanka::ObjektiTehdas::KentanKoko koko =
        joanka::ObjektiTehdas::instanssi().kentanKoko();
    for(int x = 1; x <= koko.leveys; x++) {
        for(int y = 1; y <= koko.korkeus; y++) {
            kentanKoordinaatit_.append(Koordinaatti(x, y));
        }
    }

    alustaTurnaus();
}

joanka::Logiikka::~Logiikka() {
    for(auto objekti : objektiLista_) {
        if(objekti) { delete objekti; }
    }
    objektiLista_.clear();

    for(auto objekti : kuolleetObjektit_) {
        delete objekti;
    }
    kuolleetObjektit_.clear();

    for(auto avain : pelaajat_.keys()) {
        delete pelaajat_[avain];
    }
    pelaajat_.clear();

    for(auto komento : komentoLista_) {
        delete komento;
    }
    komentoLista_.clear();
}

void joanka::Logiikka::paivitaLogiikka() {
    if(tila_ == joanka::Logiikka::PELAAJAVUORO ||
            tila_ == joanka::Logiikka::TIETOKONEVUORO ||
            tila_ == joanka::Logiikka::TAISTELUKESKEN) // ||
        /* tila_ == joanka::Logiikka::TAISTELULOPPU) */
    {
        tarkistaTaistelunTilanne();
        kasitteleTapahtumat();
        kasitteleKomennot();
        for(auto objekti : objektiLista_) {
            objekti->paivita();
            if(!objekti->elossa()) {
                /* QString viesti = QString("0x%1").arg((quintptr)objekti, */ 
                /*         QT_POINTER_SIZE * 2, 16, QChar('0')); */
                if(objekti->kerro_tyyppi() == joanka::TAISTELIJA) {
                    QString viesti(objekti->kerro_tunnus());
                    viesti.append(" kuoli!");
                    lisaaKayttoliittymaViesti(viesti);
                }
                objektiLista_.removeAll(objekti);
                kuolleetObjektit_.append(objekti);

                if(tekoaly_) { tekoaly_->poistaObjekti(objekti); }
            }
        }
        if(tila_ == TIETOKONEVUORO && tekoaly_ != nullptr) {
            bool siirtojaJaljella = tekoaly_->laskeSiirtoja(objektiLista_,
                    pelaajat_, toimintapisteet_, *this);
            if(siirtojaJaljella) {
                std::this_thread::sleep_for(std::chrono::duration<float>(TEKOALY_VIIVE_S));
            } 
            else {
                tila_ = PELAAJAVUORO;
                alustaVuoro();
                joanka::Logiikka::lisaaKayttoliittymaViesti(
                        "------------------------------------");
                joanka::Logiikka::lisaaKayttoliittymaViesti("Sinun vuorosi!");
            }
        }
    }
    else {
        while(!tapahtumaLista_.isEmpty()) {
            auto tapahtuma = tapahtumaLista_.takeFirst();
            kasitteleNakymaSiirtyma(tapahtuma);
        }
    }
}

const QList<joanka::Objekti*>& joanka::Logiikka::peliObjektit() const {
    return objektiLista_;
}

bool joanka::Logiikka::objektiValittu() const {
    if(valittuObjekti_ != nullptr) { return true; }
    else return false;
    /* return valittuObjekti_; */
}

void
joanka::Logiikka::lisaaKayttoliittymaTapahtuma(joanka::KayttoliittymaTapahtuma
        tapahtuma) {
    tapahtumaLista_.append(tapahtuma);
}

void joanka::Logiikka::kasitteleTapahtumat() {
    while(!tapahtumaLista_.isEmpty()) {
        auto tapahtuma = tapahtumaLista_.takeFirst();
        if(tapahtuma.tyyppi == joanka::VALINTA) {
            valittuObjekti_ = tapahtuma.kohde;
        }
        else if(tapahtuma.tyyppi == joanka::NAKYMA_SIIRTYMA) {
            kasitteleNakymaSiirtyma(tapahtuma);
        }
        else if(tapahtuma.tyyppi == joanka::VUORONVAIHTO) {
            if(tila_ == PELAAJAVUORO) {
                tila_ = TIETOKONEVUORO;
                alustaVuoro();
                joanka::Logiikka::lisaaKayttoliittymaViesti(
                        "------------------------------------");
                joanka::Logiikka::lisaaKayttoliittymaViesti("Tietokoneen vuoro!");
            }
        }
        else if(tila_ == PELAAJAVUORO && valittuObjekti_ != nullptr) {
            if(valittuObjekti_->kerro_omistaja() == pelaajat_[PELAAJA_TUNNUS]) {
                komentoLista_.append(new Komento(valittuObjekti_, tapahtuma));
            }
        }
    }
}

const joanka::Objekti* joanka::Logiikka::valittuObjekti() const {
    return valittuObjekti_;
}

const QMap<QString, joanka::Objekti*>& joanka::Logiikka::pelaajat() const {
    return pelaajat_;
}

joanka::NAKYMA joanka::Logiikka::nykyinenNakyma() const {
    return nykyinenNakyma_;
}

void joanka::Logiikka::alustaVuoro() {
    for(auto objekti : toimintapisteet_.keys()) {
        toimintapisteet_[objekti] = 5;
    }
}

void joanka::Logiikka::kasitteleNakymaSiirtyma(joanka::KayttoliittymaTapahtuma tapahtuma) {
    if(tapahtuma.tyyppi == joanka::NAKYMA_SIIRTYMA) {
        if(nykyinenNakyma_ == joanka::PELINAKYMA) {
            switch(tapahtuma.kohdeNakyma) {
                case joanka::MANAGEROINTINAKYMA:
                    {
                        if(pelaajaVoitti_) { voitetutTaistelut_++;
                            muutaValuuttaa(50);}
                        else { havitytTaistelut_++;
                            muutaValuuttaa(10);}
                        if(havitytTaistelut_ > PISTE_RAJA) {
                            lisaaKayttoliittymaViesti("Hävisit turnauksen!", joanka::TURNAUKSEN_TILA);
                            tila_ = joanka::Logiikka::TAISTELULOPPU;
                        }
                        if(voitetutTaistelut_ > PISTE_RAJA) {
                            lisaaKayttoliittymaViesti("Voitit turnauksen!", joanka::TURNAUKSEN_TILA);
                            tila_ = joanka::Logiikka::TAISTELULOPPU;
                        }
                        pelaajaVoitti_ = false;
                        tila_ = MANAGEROINNISSA;
                        nykyinenNakyma_ = joanka::MANAGEROINTINAKYMA;
                        std::cout << "Managerointiin!" << std::endl;
                        lopetaTaistelu();
                        break;
                    }
                    /* case joanka::PELINAKYMA: */
                    /*     { */
                    /*         alustaTaistelu(); */
                    /*         tila_ = PELAAJAVUORO; */
                    /*         nykyinenNakyma_ = joanka::PELINAKYMA; */
                    /*         std::cout << "Peliin!" << std::endl; */
                    /*         break; */
                    /*     } */
                case joanka::VALIKKONAKYMA:
                    {
                        tila_ = VALIKOSSA;
                        nykyinenNakyma_ = joanka::VALIKKONAKYMA;
                        alustaTurnaus();
                        std::cout << "Valikkoon!" << std::endl;
                        break;
                    }
                case joanka::ASETUSNAKYMA:
                    {
                        tila_ = VALIKOSSA;
                        nykyinenNakyma_ = joanka::ASETUSNAKYMA;
                        std::cout << "Asetuksiin!" << std::endl;
                        break;
                    }
                default:
                    break;
            }
        }
        else {
            switch(tapahtuma.kohdeNakyma) {
                case joanka::MANAGEROINTINAKYMA:
                    {
                        tila_ = MANAGEROINNISSA;
                        nykyinenNakyma_ = joanka::MANAGEROINTINAKYMA;
                        std::cout << "Managerointiin!" << std::endl;
                        break;
                    }
                case joanka::PELINAKYMA:
                    {
                        if(alustaTaistelu()) {
                            tila_ = PELAAJAVUORO;
                            nykyinenNakyma_ = joanka::PELINAKYMA;
                            std::cout << "Peliin!" << std::endl;
                        }
                        break;
                    }
                case joanka::VALIKKONAKYMA:
                    {
                        tila_ = VALIKOSSA;
                        nykyinenNakyma_ = joanka::VALIKKONAKYMA;
                        alustaTurnaus();
                        std::cout << "Valikkoon!" << std::endl;
                        break;
                    }
                case joanka::ASETUSNAKYMA:
                    {
                        tila_ = VALIKOSSA;
                        nykyinenNakyma_ = joanka::ASETUSNAKYMA;
                        std::cout << "Asetuksiin!" << std::endl;
                        break;
                    }
                default: ;
            }
        }
    }
}

int joanka::Logiikka::laskeToimintapisteet(joanka::KayttoliittymaTapahtuma&
        tapahtuma, joanka::Objekti* kohde) {
    if(tapahtuma.tyyppi == joanka::LIIKKUMINEN) {
        float etaisyysX = tapahtuma.xKoordinaatti - kohde->kerro_x();
        float etaisyysY = tapahtuma.yKoordinaatti - kohde->kerro_y();
        float etaisyys = qSqrt(qPow(etaisyysX, 2) + qPow(etaisyysY, 2));

        std::cout << "Matkan " << etaisyys << " liikkuminen veisi " <<
            qCeil(etaisyys) << " toimintapistettä." << std::endl;

        return qCeil(etaisyys);
    }
    if(tapahtuma.tyyppi == joanka::HYOKKAYS) {
        return 1;
    }
    else {
        return 0;
    }
}

joanka::KayttoliittymaViesti joanka::Logiikka::lueKayttoliittymaViesti() {
    if (!kayttoliittymaViestit_.isEmpty())
        return kayttoliittymaViestit_.takeFirst();
    else
        return KayttoliittymaViesti("");
}

void joanka::Logiikka::lisaaKayttoliittymaViesti(QString viesti, ViestiTaso taso) {
    qDebug() << viesti;
    if(taso == joanka::NORMAALI) {
        viesti.prepend("-");
    }
    kayttoliittymaViestit_.append(KayttoliittymaViesti(viesti, taso));

}

int joanka::Logiikka::voitetutTaistelut() const {
    return voitetutTaistelut_;
}

int joanka::Logiikka::havitytTaistelut() const {
    return havitytTaistelut_;
}

void joanka::Logiikka::lisaaUusiObjekti(joanka::ObjektiTyyppi tyyppi, QString omistaja) {
    joanka::ObjektiTehdas* tehdasPtr = &joanka::ObjektiTehdas::instanssi();
    objektiLista_.append(tehdasPtr->luoObjekti(tyyppi));
    if(tyyppi == joanka::ObjektiTyyppi::TAISTELIJA) {
        /* objektiLista_.back()->aseta_omistaja(pelaajat_[omistaja]); */
        vaihdaOmistajaa(objektiLista_.back(), omistaja);

        toimintapisteet_[objektiLista_.back()] = 5;

        if(omistaja == VASTUSTAJA_TUNNUS) {
            joanka::Objekti* objekti = objektiLista_.back();
            objektiLista_.append(tehdasPtr->luoObjekti(joanka::VARUSTE));
            objekti->lisaa_omistajuus(objektiLista_.back());
        }
    }
}

void joanka::Logiikka::vaihdaOmistajaa(joanka::Objekti* objekti, QString uusiOmistaja) {
    if(pelaajat_.contains(uusiOmistaja)) {
        objekti->aseta_omistaja(pelaajat_[uusiOmistaja]);
    }
    else {
        pelaajat_[uusiOmistaja] = new joanka::Objekti();
        objekti->aseta_omistaja(pelaajat_[uusiOmistaja]);
    }
}

void joanka::Logiikka::tarkistaTaistelunTilanne() {
    bool pelaajaHavinnyt = true;
    bool tietokoneHavinnyt = true;
    for(auto taistelija : pelaajat_[PELAAJA_TUNNUS]->kerro_omistettavat()) {
        if(taistelija->elossa()) {
            pelaajaHavinnyt = false;
            break;
        }
    }
    for(auto taistelija : pelaajat_[VASTUSTAJA_TUNNUS]->kerro_omistettavat()) {
        if(taistelija->elossa()) {
            tietokoneHavinnyt = false;
            break;
        }
    }
    if(pelaajaHavinnyt) {
        tila_ = joanka::Logiikka::TAISTELULOPPU;
        lisaaKayttoliittymaViesti("Peli loppui, hävisit", joanka::TAISTELUN_TILA);
    }
    if(tietokoneHavinnyt) {
        tila_ = joanka::Logiikka::TAISTELULOPPU;
        pelaajaVoitti_ = true;
        lisaaKayttoliittymaViesti("Peli loppui, voitit", joanka::TAISTELUN_TILA);
    }
}

bool joanka::Logiikka::alustaTaistelu() {
    bool pelaajallaTaistelijoita = false;
    for(auto taistelija : pelaajat_[PELAAJA_TUNNUS]->kerro_omistettavat()) {
        if(taistelija->elossa()) {
            pelaajallaTaistelijoita = true;
            break;
        }
    }
    if(!pelaajallaTaistelijoita) {
        lisaaKayttoliittymaViesti("Sinulla ei ole taistelijoita!",
                joanka::KRIITTINEN);
        return false;
    }

    // Näytetään latausikkuna
    joanka::NAKYMA seuraavaNakyma = nykyinenNakyma_;
    nykyinenNakyma_ = LATAUSNAKYMA;

    while(pelaajat_[""]->kerro_omistettavat().size() < 3) {
        lisaaUusiObjekti(joanka::TAISTELIJA, "");
    }

    while(pelaajat_[VASTUSTAJA_TUNNUS]->kerro_omistettavat().size() < 2) {
        lisaaUusiObjekti(joanka::TAISTELIJA, VASTUSTAJA_TUNNUS);
    }
    joanka::ObjektiTehdas::KentanKoko koko =
        joanka::ObjektiTehdas::instanssi().kentanKoko();
    std::uniform_int_distribution<uint32_t> xKoordinaattiJakauma(1, koko.leveys);
    std::uniform_int_distribution<uint32_t> yKoordinaattiJakauma(1, koko.korkeus);

    QList<Koordinaatti> avoimetKoordinaatit(kentanKoordinaatit_);
    std::shuffle(avoimetKoordinaatit.begin(), avoimetKoordinaatit.end(), logiikkaRNG);

    for(auto objekti : objektiLista_) {
        if(objekti->kerro_tyyppi() == joanka::TAISTELIJA) {
            if(objekti->kerro_omistaja() == pelaajat_[PELAAJA_TUNNUS] ||
                    objekti->kerro_omistaja() == pelaajat_[VASTUSTAJA_TUNNUS]) {
                int x = xKoordinaattiJakauma(logiikkaRNG);
                int y = yKoordinaattiJakauma(logiikkaRNG);

                if(avoimetKoordinaatit.size() > 0) {
                    Koordinaatti kord = avoimetKoordinaatit.takeFirst();
                    x = kord.x;
                    y = kord.y;
                }
                objekti->aseta_sijainti(x, y);
                for(auto laatta : objektiLista_) {
                    if(laatta->kerro_tyyppi() == KENTTALAATTA) {
                        if(laatta->kerro_x() == objekti->kerro_x() &&
                                laatta->kerro_y() == objekti->kerro_y()) {
                            objekti->interaktio(laatta);
                            break;
                        }
                    }
                }
            }
            else {
                // Purkkaa
                objekti->aseta_sijainti(-50, -50);
            }
        }
    }
    tekoaly_ = std::unique_ptr<joanka::Tekoaly>(new
            joanka::Tekoaly(pelaajat_[VASTUSTAJA_TUNNUS]));
    alustaVuoro();

    pelaajaVoitti_ = false;

    nykyinenNakyma_ = seuraavaNakyma;
    return true;
}

void joanka::Logiikka::alustaTurnaus() {
    // Näytetään latausikkuna
    joanka::NAKYMA seuraavaNakyma = nykyinenNakyma_;
    nykyinenNakyma_ = LATAUSNAKYMA;

    voitetutTaistelut_ = 0;
    havitytTaistelut_ = 0;
    pelaajaVoitti_ = false;
    valuuttaaJaljella_ = 100;

    for(auto objekti : objektiLista_) {
        kuolleetObjektit_.append(objekti);
    }
    objektiLista_.clear();

    for(auto pelaaja : pelaajat_.keys()) {
        for(auto objekti : pelaajat_[pelaaja]->kerro_omistettavat()) {
            pelaajat_[pelaaja]->poista_omistajuus(objekti);
        }
    }

    if(!pelaajat_.contains("")) {
        pelaajat_[""] = new joanka::Objekti();
    }
    if(!pelaajat_.contains(PELAAJA_TUNNUS)) {
        pelaajat_[PELAAJA_TUNNUS] = new joanka::Objekti();
    }
    if(!pelaajat_.contains(VASTUSTAJA_TUNNUS)) {
        pelaajat_[VASTUSTAJA_TUNNUS] = new joanka::Objekti();
    }

    joanka::ObjektiTehdas* tehdasPtr = &joanka::ObjektiTehdas::instanssi();

    for(int i=0; i < 4; i++) {
        lisaaUusiObjekti(joanka::VARUSTE);
    }
    while(pelaajat_[""]->kerro_omistettavat().size() < 3) {
        lisaaUusiObjekti(joanka::TAISTELIJA, "");
    }
    while(pelaajat_[PELAAJA_TUNNUS]->kerro_omistettavat().size() < 2) {
        lisaaUusiObjekti(joanka::TAISTELIJA, PELAAJA_TUNNUS);
    }

    objektiLista_.append(tehdasPtr->generoiKentta());

    nykyinenNakyma_ = seuraavaNakyma;
}

void joanka::Logiikka::lopetaTaistelu() {
    // Näytetään latausikkuna
    joanka::NAKYMA seuraavaNakyma = nykyinenNakyma_;
    nykyinenNakyma_ = LATAUSNAKYMA;

    for(auto objekti : objektiLista_) {
        objekti->alusta();
        if(!objekti->elossa()) {
            /* QString viesti = QString("0x%1").arg((quintptr)objekti, */ 
            /*         QT_POINTER_SIZE * 2, 16, QChar('0')); */
            QString viesti(objekti->kerro_tunnus());
            viesti.append(" kuoli!");
            lisaaKayttoliittymaViesti(viesti);
            objektiLista_.removeAll(objekti);
            kuolleetObjektit_.append(objekti);

            if(tekoaly_) { tekoaly_->poistaObjekti(objekti); }
        }
    }
    if(tekoaly_) { tekoaly_->alusta(); }
    valittuObjekti_ = nullptr;

    nykyinenNakyma_ = seuraavaNakyma;
}

void joanka::Logiikka::kasitteleKomennot() {
    for(auto komento : komentoLista_) {
        if(komento->toteuta(toimintapisteet_)) {
            komentoLista_.removeAll(komento);
            delete komento;
        }
    }
}

int joanka::Logiikka::valuuttaaJaljella() const {
    return valuuttaaJaljella_;
}

void joanka::Logiikka::muutaValuuttaa(int maara) {
    valuuttaaJaljella_ += maara;
}

void joanka::Logiikka::lisaaKomento(Komento* komento) {
    komentoLista_.append(komento);
}

void joanka::Logiikka::poistaKuolleet() {
    for(auto objekti : kuolleetObjektit_) {
        delete objekti;
        objekti = nullptr;
    }
    kuolleetObjektit_.clear();
}

joanka::KayttoliittymaViesti::KayttoliittymaViesti() 
    : viesti(""), taso(NORMAALI)
{}

joanka::KayttoliittymaViesti::KayttoliittymaViesti(QString viesti,
        ViestiTaso taso)
: viesti(viesti), taso(taso)
{}
