#include <QString>
#include <iostream>

// joanka
#include "Poikkeukset.hh"
#include "SkriptiLukija.hh"
#include "ObjektiTehdas.hh"
#include "SkriptiLukija.hh"
#include "KenttaLaatta.hh"
#include "Taistelija.hh"
#include "Varuste.hh"


joanka::ObjektiTehdas& joanka::ObjektiTehdas::instanssi() {
    static joanka::ObjektiTehdas instanssi;

    return instanssi;
}

joanka::ObjektiTehdas::ObjektiTehdas() {
}

joanka::Objekti* joanka::ObjektiTehdas::luoObjekti(joanka::ObjektiTyyppi tyyppi) {
    stringMap tiedot;
    tiedot = lukija_.lueObjekti(tyyppi);
    if (tyyppi==joanka::TAISTELIJA) {
        return luoTaistelija(tiedot);
    }
    else if (tyyppi==joanka::VARUSTE) {
        return luoVaruste(tiedot);
    }
    return nullptr;
}

QList<joanka::Objekti*> joanka::ObjektiTehdas::generoiKentta() {
    QList<joanka::Objekti*> kenttalista;
    QList<joanka::stringMap> luettuLista = lukija_.lueKentta();

    for(auto kenttalaatta : luettuLista) {
        QString tyyppi = kenttalaatta.take("tyyppi");
        QString kuva = kenttalaatta.take("kuva");
        int xKoordinaatti = kenttalaatta.take("xKoordinaatti").toInt();
        int yKoordinaatti = kenttalaatta.take("yKoordinaatti").toInt();
        int hyokkayslisa = kenttalaatta.take("hyokkayslisa").toInt();
        int puolustuslisa = kenttalaatta.take("puolustuslisa").toInt();
        kenttalista.append(new joanka::KenttaLaatta(tyyppi, xKoordinaatti,
                    yKoordinaatti, kuva, hyokkayslisa, puolustuslisa));
    }

    return kenttalista;
}


joanka::Objekti* joanka::ObjektiTehdas::luoTaistelija(QMap<QString, QString> tiedot) {
    //taistelijan tarvitsemat tiedot:
    QString tunnus{""};
    QString kuva{""};
    QString kuvaus{"Ei kuvausta."};
    int voima{0};
    int nopeus{1};
    int panssari{0};
    int kantama{1};

    QMap<QString, QString>::const_iterator i = tiedot.constBegin();
    //std::cout << "Tehdas saa:" << std::endl;
    while (i != tiedot.constEnd()) {
        //std::cout << i.key().toStdString() << ": " << i.value().toStdString() << std::endl;
        if (i.key()=="taistelijatyyppi") {
            tunnus = i.value();
        }
        else if(i.key()=="kuva") {
            kuva = i.value();
        }
        else if(i.key()=="kuvaus") {
            kuvaus = i.value();
        }
        else if(i.key()=="hyokkays") {
            voima = i.value().toInt();
        }
        else if(i.key()=="nopeus") {
            nopeus = i.value().toInt();
        }
        else if(i.key()=="puolustus") {
            panssari = i.value().toInt();
        }
        else if(i.key()=="kantama") {
            kantama = i.value().toInt();
        }
        ++i;
    }
    std::cout << "Taistelijan voima " << voima << std::endl;
    std::cout << "Taistelijan panssari " << panssari << std::endl;
    Taistelija* taistelija;
    taistelija = new Taistelija(tunnus, kuva, kuvaus, voima, nopeus, panssari, kantama);
    return taistelija;
}

joanka::Objekti* joanka::ObjektiTehdas::luoVaruste(QMap<QString, QString> tiedot) {
    //taistelijan tarvitsemat tiedot:
    QString tunnus{""};
    QString kuva{""};
    QString kuvaus{"Ei kuvausta."};
    int voima{0};
    int panssari{0};

    QMap<QString, QString>::const_iterator i = tiedot.constBegin();
    //std::cout << "Tehdas saa:" << std::endl;
    while (i != tiedot.constEnd()) {
        /* std::cout << i.key().toStdString() << ": " << i.value().toStdString() << std::endl; */
        if (i.key()=="tunnus" || i.key()=="varustetyyppi") {
            tunnus = i.value();
        }
        else if(i.key()=="kuva") {
            kuva = i.value();
        }
        else if(i.key()=="kuvaus") {
            kuvaus = i.value();
        }
        else if(i.key()=="voima" || i.key()=="hyokkays") {
            voima = i.value().toInt();
        }
        else if(i.key()=="panssari" || i.key()=="puolustus") {
            panssari = i.value().toInt();
        }
        ++i;
    }
    Varuste* varuste;
    varuste = new Varuste(tunnus, kuva, kuvaus, voima, panssari);
    return varuste;
}

void joanka::ObjektiTehdas::asetaKentanKoko(int leveys, int korkeus) {
    kentanKoko_.leveys = leveys;
    kentanKoko_.korkeus = korkeus;
}

joanka::ObjektiTehdas::KentanKoko joanka::ObjektiTehdas::kentanKoko() {
    lukija_.paivitaKentanKoko();
    return kentanKoko_;
}
