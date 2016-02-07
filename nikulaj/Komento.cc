// std
#include <iostream>

// Qt
#include <QtCore/qmath.h>

// joanka
#include "Logiikka.hh"


#include "Komento.hh"


joanka::Komento::Komento(joanka::Objekti* toteuttaja, joanka::Objekti* kohde,
        int x, int y)
    : toteuttaja_(toteuttaja), taistelijaKohde_(nullptr),
    kenttalaattaKohde_(nullptr), x_(x), y_(y), jarkevaKomento_(true)
{
    if(toteuttaja == nullptr) { jarkevaKomento_ = false; }
    if(kohde->kerro_tyyppi() == TAISTELIJA) {
        taistelijaKohde_ = kohde;
    }
    else if(kohde->kerro_tyyppi() == KENTTALAATTA) {
        kenttalaattaKohde_ = kohde;
    }
    toimintapisteHinta_= laskeToimintapisteHinta();
}

joanka::Komento::Komento(joanka::Objekti* toteuttaja,
        joanka::KayttoliittymaTapahtuma& kayttoliittymaTapahtuma)
    : toteuttaja_(toteuttaja),
    taistelijaKohde_(nullptr),
    kenttalaattaKohde_(nullptr),
    x_(kayttoliittymaTapahtuma.xKoordinaatti),
    y_(kayttoliittymaTapahtuma.yKoordinaatti), jarkevaKomento_(true)
{
    if(toteuttaja == nullptr) { jarkevaKomento_ = false; }
    if(kayttoliittymaTapahtuma.tyyppi == joanka::LIIKKUMINEN) {
        kenttalaattaKohde_ = kayttoliittymaTapahtuma.kohde;
    }
    else if(kayttoliittymaTapahtuma.tyyppi == joanka::HYOKKAYS) {
        taistelijaKohde_ = kayttoliittymaTapahtuma.kohde;
    }
    else {
        jarkevaKomento_ = false;
    }
    toimintapisteHinta_= laskeToimintapisteHinta();
}

bool joanka::Komento::toteuta(QMap<joanka::Objekti*, int>& toimintapisteMappi) {
    // Järjetön komento "suoritetaan", se ei vain tee mitään.
    if(!jarkevaKomento_) {
        joanka::Logiikka::lisaaKayttoliittymaViesti("Komento ei ole järkevä!");
        return true;
    }

    int toimintapisteet = toimintapisteMappi[toteuttaja_];
    if(toimintapisteHinta_ > toimintapisteet) {
        joanka::Logiikka::lisaaKayttoliittymaViesti("Ei tarpeeksi toimintapisteitä!");
        return true;
    }
    if(kenttalaattaKohde_ != nullptr) {
        toteuttaja_->aseta_sijainti(x_, y_);
        toteuttaja_->interaktio(kenttalaattaKohde_);
        toimintapisteMappi[toteuttaja_] -= toimintapisteHinta_;
    }
    else if(taistelijaKohde_ != nullptr){
        if(toteuttaja_->interaktio(taistelijaKohde_) && 
                toteuttaja_->kerro_omistaja() != taistelijaKohde_->kerro_omistaja()) {
            toimintapisteMappi[toteuttaja_] -= toimintapisteHinta_;
        }
        else {
            joanka::Logiikka::lisaaKayttoliittymaViesti("Hyökkäys ei onnistunut");
        }
    }
    QString viesti = toteuttaja_->kerro_tunnus();
    viesti.append(":lla on ");
    viesti.append(QString::number(toimintapisteMappi[toteuttaja_]));
    viesti.append(" toimintapistettä jäljellä.");
    joanka::Logiikka::lisaaKayttoliittymaViesti(viesti);

    return true;
}

int joanka::Komento::toimintapisteHinta() const {
    return toimintapisteHinta_;
}

int joanka::Komento::laskeToimintapisteHinta() {
    if(taistelijaKohde_ == nullptr) {
        int etaisyysX = toteuttaja_->kerro_x() - x_;
        int etaisyysY = toteuttaja_->kerro_y() - y_;
        int etaisyys = qCeil(qSqrt(qPow(etaisyysX, 2) + qPow(etaisyysY, 2)));
        int toimintapisteet = qCeil(etaisyys);

        if(toteuttaja_->palauta_lukuja().contains("nopeus")) {
            // parempi funktio olisi parempi
            toimintapisteet = qCeil(toimintapisteet /
                    toteuttaja_->palauta_lukuja()["nopeus"]);
            if(toimintapisteet < 1 && etaisyys > 0) { toimintapisteet = 1; }    // nyt prkl
        }

        QString stringi = "Matkan ";
        stringi.append(QString::number(etaisyys));
        stringi.append(" liikkuminen vie ");
        stringi.append(QString::number(toimintapisteet));
        stringi.append(" toimintapistettä.");
        joanka::Logiikka::lisaaKayttoliittymaViesti(stringi);

        return toimintapisteet;
    }
    else {
        if(toteuttaja_->kerro_omistaja() != taistelijaKohde_->kerro_omistaja() &&
                toteuttaja_->kerro_tyyppi() == joanka::TAISTELIJA &&
                taistelijaKohde_->kerro_tyyppi() == joanka::TAISTELIJA) {
            return 2;
        }
        else {
            return 0;
        }
    }
}
