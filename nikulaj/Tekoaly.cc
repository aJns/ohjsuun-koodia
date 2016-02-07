// std
#include <random>
#include <algorithm>
#include <iostream>

// Qt
#include <QtCore/QtMath>
#include <QTime>

// joanka
#include "Logiikka.hh"


#include "Tekoaly.hh"


// Varmistetaan että tekoäly ei jää loputtomasti laskemaan siirtoja
const int LASKU_RAJOITUS = 100;
// satunnaislukugeneraattori
std::mt19937 tekoalyRNG;  

joanka::Tekoaly::Tekoaly(joanka::Objekti* pelaaja)
    : pelaaja_(pelaaja),
    laskuri(0)
{
}

joanka::Tekoaly::~Tekoaly() {
    viholliset_.clear();
    kohteet_.clear();
}

bool joanka::Tekoaly::laskeSiirtoja(QList<joanka::Objekti*>& objektilista,
        joanka::pelaajaMappi& pelaajat,
        joanka::toimintapisteMappi& toimintapisteet,
        joanka::Logiikka& logiikka) {

    if(viholliset_.isEmpty()) {
        for(auto objekti : objektilista) {
            if(objekti->kerro_tyyppi() == joanka::TAISTELIJA) {
                joanka::Objekti* omistaja = objekti->kerro_omistaja();
                if(omistaja != pelaaja_ && omistaja != nullptr && omistaja != pelaajat[""]) {
                    viholliset_.append(objekti);
                }
            }
        }
        if(viholliset_.isEmpty()) {
            laskuri=0;
            return false;
        }
    }
    bool palautusarvo = false;
    for(auto taistelija : pelaaja_->kerro_omistettavat()) {
        if(kohteet_[taistelija] == nullptr) {
            std::shuffle(viholliset_.begin(), viholliset_.end(), tekoalyRNG);
            kohteet_[taistelija] = viholliset_.at(0);
        }
        if(toimintapisteet[taistelija] > 0 && kohteet_[taistelija] != nullptr) {
            if(taistelija->interaktio_mahdollinen(kohteet_[taistelija]) &&
                    toimintapisteet[taistelija] > 1) {
                joanka::Komento* komento = new joanka::Komento(taistelija,
                        kohteet_[taistelija]);
                logiikka.lisaaKomento(komento);
                palautusarvo = true;
            }
            else {
                int xEtaisyys = taistelija->kerro_x() -
                    kohteet_[taistelija]->kerro_x();
                int yEtaisyys = taistelija->kerro_y() -
                    kohteet_[taistelija]->kerro_y();

                int etaisyys = qCeil(qSqrt(qPow(xEtaisyys, 2) + qPow(yEtaisyys, 2)));
                if(qAbs(etaisyys) == 1) { break; }

                int x = taistelija->kerro_x();
                int y = taistelija->kerro_y();

                int nopeus = 1;
                if(taistelija->palauta_lukuja().contains("nopeus")) {
                    nopeus = taistelija->palauta_lukuja()["nopeus"];
                }

                if(qAbs(xEtaisyys) >= qAbs(yEtaisyys)) {
                    if(qAbs(xEtaisyys) <= nopeus) {
                        nopeus = etaisyys - 1;
                    }
                    if(xEtaisyys > 0) {
                        x = taistelija->kerro_x() - nopeus;
                        y = taistelija->kerro_y();
                    }
                    else {
                        x = taistelija->kerro_x() + nopeus;
                        y = taistelija->kerro_y();
                    }
                }
                else {
                    if(qAbs(yEtaisyys) <= nopeus) {
                        nopeus = etaisyys - 1;
                    }
                    if(yEtaisyys > 0) {
                        x = taistelija->kerro_x();
                        y = taistelija->kerro_y() - nopeus;
                    }
                    else {
                        x = taistelija->kerro_x();
                        y = taistelija->kerro_y() + nopeus;
                    }
                }
                joanka::Objekti* kenttalaatta = nullptr;
                for(auto objekti : objektilista) {
                    if(objekti->kerro_tyyppi() == joanka::KENTTALAATTA) {
                        if(objekti->kerro_x() == x && objekti->kerro_y() == y) {
                            kenttalaatta = objekti;
                            break;
                        }
                    }
                }
                joanka::Komento* komento = new joanka::Komento(taistelija,
                        kenttalaatta, x, y);
                logiikka.lisaaKomento(komento);
                palautusarvo = true;
            }
        }
        if(palautusarvo) {
            /* QString viesti = QString("0x%1").arg((quintptr)taistelija, */ 
            /*         QT_POINTER_SIZE * 2, 16, QChar('0')); */
            QString viesti = taistelija->kerro_tunnus();
            viesti.append(":lla on ");
            viesti.append(QString::number(toimintapisteet[taistelija]));
            viesti.append(" toimintapistettä jäljellä.");
            joanka::Logiikka::lisaaKayttoliittymaViesti(viesti);
        }
    }
    if(laskuri > LASKU_RAJOITUS) {
        laskuri = 0;
        return false;
    }
    if(palautusarvo == false) {
        laskuri = 0;
    }
    else {
        laskuri++;
    }
    return palautusarvo;
}

void joanka::Tekoaly::poistaObjekti(joanka::Objekti* objekti) {
    viholliset_.removeAll(objekti);
    for(auto avain : kohteet_.keys()) {
        if(kohteet_[avain] == objekti) {
            kohteet_.remove(avain);
        }
    }
    pelaaja_->poista_omistajuus(objekti);
}

void joanka::Tekoaly::alusta() {
    viholliset_.clear();
    kohteet_.clear();
}
