// std
#include <iostream>

// Qt
#include <QMap>
#include <QtMath>

// joanka
#include "objekti.hh"
#include "GrafiikkaKomponenttiRajapinta.hh"
#include "paaikkuna.hh"


#include "SyoteKasittelija.hh"


joanka::SyoteKasittelija::SyoteKasittelija(joanka::Logiikka* logiikka)
    : logiikka_(logiikka)
{
}

void joanka::SyoteKasittelija::lisaaTapahtuma(QGraphicsSceneMouseEvent& tapahtuma) {
    QPoint tapahtumapaikka;
    if(tapahtuma.button() == Qt::LeftButton) {
        tapahtumapaikka = tapahtuma.buttonDownScenePos(Qt::LeftButton).toPoint();
    }
    else if(tapahtuma.button() == Qt::RightButton) {
        tapahtumapaikka = tapahtuma.buttonDownScenePos(Qt::RightButton).toPoint();
    }
    else {
        return;
    }

    tapahtumapaikka /= PIXMAP_LEVEYS;

    joanka::Objekti* kohde = laskeKohde(tapahtumapaikka, TAISTELIJA);

    bool palautaSyote = true;

    joanka::KayttoliittymaTapahtuma kayttoliittymaTapahtuma(joanka::VALINTA);
    if(tapahtuma.button() == Qt::LeftButton) {
        kayttoliittymaTapahtuma =
            joanka::KayttoliittymaTapahtuma(joanka::VALINTA, -1, -1, kohde);
        std::cout << kohde << " valittu!" << std::endl;
    }
    else if(tapahtuma.button() == Qt::RightButton) {
        if(kohde == nullptr) {
            Objekti* kohdelaatta = laskeKohde(tapahtumapaikka, KENTTALAATTA);
            kayttoliittymaTapahtuma =
                joanka::KayttoliittymaTapahtuma(joanka::LIIKKUMINEN, kohdelaatta->kerro_x(),
                        kohdelaatta->kerro_y(), kohdelaatta);
        }
        else if(logiikka_->valittuObjekti() != nullptr) {
            kayttoliittymaTapahtuma = 
                joanka::KayttoliittymaTapahtuma(joanka::HYOKKAYS, -1, -1,
                        kohde);
            std::cout << "Valittu objekti hyökkää " << kohde << " kimppuun!" <<
                std::endl;
        }
        else {
            joanka::Logiikka::lisaaKayttoliittymaViesti("Ei järkevää komentoa");
            palautaSyote = false;
        }
    }
    if(palautaSyote) {
        logiikka_->lisaaKayttoliittymaTapahtuma(kayttoliittymaTapahtuma);
    }
}

joanka::Objekti* joanka::SyoteKasittelija::laskeKohde(QPoint tapahtumapaikka,
        joanka::ObjektiTyyppi kohdetyyppi) {
    joanka::Objekti* kohde = nullptr;
    QMap<joanka::Objekti*, float> kandidaatit;
    for(auto objekti : logiikka_->peliObjektit()) {
        if(objekti->kerro_tyyppi() == kohdetyyppi) {
            float xEtaisyys = objekti->kerro_x() - tapahtumapaikka.rx();
            float yEtaisyys = objekti->kerro_y() - tapahtumapaikka.ry();
            float etaisyys = qSqrt(xEtaisyys*xEtaisyys + yEtaisyys*yEtaisyys);
            if(etaisyys < VALINTA_ETAISYYS) {
                kandidaatit.insert(objekti, etaisyys);
            }
        }
    }

    while(kandidaatit.size() > 1) {
        joanka::Objekti* objekti1;
        joanka::Objekti* objekti2;
        float etaisyys1 = -1;
        float etaisyys2 = -1;
        QMapIterator<joanka::Objekti*, float> i(kandidaatit);
        objekti1 = i.next().key();
        etaisyys1 = kandidaatit[objekti1];
        objekti2 = i.next().key();
        etaisyys2 = kandidaatit[objekti2];

        if(qAbs(etaisyys1 - etaisyys2) < 0.1) {
            if(objekti1->kerro_omistaja() == logiikka_->pelaajat()[PELAAJA_TUNNUS]) {
                kandidaatit.remove(objekti2);
            }
            else {
                kandidaatit.remove(objekti1);
            }
        }
        else if(etaisyys1 > etaisyys2) {
            kandidaatit.remove(objekti1);
        }
        else {
            kandidaatit.remove(objekti2);
        }
    }

    if(kandidaatit.size() == 1) {
        QMapIterator<joanka::Objekti*, float> i(kandidaatit);
        kohde = i.next().key();
    }
    else {
        kohde = nullptr;
    }
    return kohde;
}

void joanka::SyoteKasittelija::siirrymanagerointiin() {
    /* logiikka_->siirryManagerointinakymaan(); */
    /* std::cout << "Managerointinakymaan!" << std::endl; */
    joanka::KayttoliittymaTapahtuma tapahtuma = 
        joanka::KayttoliittymaTapahtuma(joanka::NAKYMA_SIIRTYMA, -1, -1,
                nullptr, joanka::MANAGEROINTINAKYMA);
    logiikka_->lisaaKayttoliittymaTapahtuma(tapahtuma);
}

void joanka::SyoteKasittelija::siirryasetuksiin() {
    joanka::KayttoliittymaTapahtuma tapahtuma = 
        joanka::KayttoliittymaTapahtuma(joanka::NAKYMA_SIIRTYMA, -1, -1,
                nullptr, joanka::ASETUSNAKYMA);
    logiikka_->lisaaKayttoliittymaTapahtuma(tapahtuma);
}

void joanka::SyoteKasittelija::siirryvalikkoon() {
    joanka::KayttoliittymaTapahtuma tapahtuma = 
        joanka::KayttoliittymaTapahtuma(joanka::NAKYMA_SIIRTYMA, -1, -1,
                nullptr, joanka::VALIKKONAKYMA);
    logiikka_->lisaaKayttoliittymaTapahtuma(tapahtuma);
}

void joanka::SyoteKasittelija::siirrypelinakymaan() {
    /* logiikka_->siirryPelinakymaan(); */
    /* std::cout << "Pelinäkymään!" << std::endl; */
    joanka::KayttoliittymaTapahtuma tapahtuma = 
        joanka::KayttoliittymaTapahtuma(joanka::NAKYMA_SIIRTYMA, -1, -1,
                nullptr, joanka::PELINAKYMA);
    logiikka_->lisaaKayttoliittymaTapahtuma(tapahtuma);
}

void joanka::SyoteKasittelija::luovutavuoro() {
    joanka::KayttoliittymaTapahtuma tapahtuma = 
        joanka::KayttoliittymaTapahtuma(joanka::VUORONVAIHTO);
    logiikka_->lisaaKayttoliittymaTapahtuma(tapahtuma);
}

void joanka::SyoteKasittelija::poistaKuolleet() {
    logiikka_->poistaKuolleet();
}
