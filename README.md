# Kirjanpito-ohjelma

**Avoimen lähdekoodin kirjanpito-ohjelma pienille organisaatioille.**

Tämä on [Kitsas](https://github.com/artoh/kitupiikki)-ohjelmasta (alun perin julkaistu
nimellä [Kitupiikki](https://kitupiikki.info)) muokattu, itsenäisesti ylläpidetty versio.

> **Huomio:** Tämä on muokattu versio (muokattu 2026). **Kitsas Oy ei tarjoa mitään tukea tälle
> ohjelmistolle** eikä vastaa sen toiminnasta. Kitsas Oy:hyn ei tule ottaa yhteyttä
> tätä versiota koskevissa asioissa.

Comments, variable names, documentation and the software itself are, of course, in Finnish only!

## Tavoitteet

- helppokäyttöisyys
- tositteiden sähköinen käsittely pdf-muodossa
- sähköisen arkiston muodostaminen
- sisäänrakennettu laskutus
- muodostaa tuloslaskelman, taseen, tase-erittelyn

Kirjanpito tallennetaan omalle tietokoneelle SQLite-muodossa. Kitsas Oy:n
pilvipalvelu ei ole osa tätä versiota.

## Vaatimukset

Ohjelma käyttää [Qt-kirjastoa](https://qt.io) versio vähintään 6.4 (kaikki ominaisuudet 6.8).
Käytössä on mm. QtWidgets-, QtPdf- ja QtWebEngine-moduulit.

Zip-tiedostojen käsittelyyn käytetään [libzip](https://libzip.org)-kirjastoa.

Lataa ja asenna Qt-kirjastot osoitteesta https://qt.io/download.

Linuxissa poppler on helppo asentaa järjestelmään:

    sudo apt-get install libpoppler-qt5-1 libpoppler-qt5-dev

ja libzip

    sudo apt-get install libzip-dev

## Kääntäminen

Käytössä on QMake. Kääntäminen on helpointa tehdä [QtCreatorin](http://doc.qt.io/qtcreator/)
ympäristössä. Komentorivillä kääntyy komennoilla

    qmake kitsasproject.pro && make qmake_all
    make

## Alkuperäinen ohjelmisto

Tämä ohjelma perustuu Kitsas-ohjelmaan.

- Alkuperäinen tekijä: Arto Hyvättinen ja Kitsas Oy
- Alkuperäinen kotisivu: [kitsas.fi](https://kitsas.fi)
- Käyttöohjeet (soveltuvin osin): [kitsas.fi/docs](https://kitsas.fi/docs)

## Ylläpito

DevTerra – [github.com/Xyntexx/kitupiikki](https://github.com/Xyntexx/kitupiikki)

## Lisenssi

GNU General Public License 3 – katso [LICENSE](LICENSE) seuraavilla lisenssin mukaisilla lisäehdoilla:

Jaettaessa muokatuksi ohjelmisto on

- merkittävä selkeästi muutetuksi
- esitettävä selkeästi, ettei Kitsas Oy tarjoa mitään tukea muokatulle ohjelmistolle
- vältettävä käyttämästä Kitsas Oy:n nimeä muokatun ohjelmiston yhteydessä
