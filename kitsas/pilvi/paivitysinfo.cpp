/*
   Copyright (C) Arto Hyvättinen ja Kitsas Oy
   Copyright (C) 2026 Xyntexx

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "paivitysinfo.h"

#include "db/kirjanpito.h"

#include <QNetworkReply>
#include <QJsonDocument>
#include <QSettings>
#include <QApplication>
#include "kieli/kielet.h"
#include "pilvi/pilvimodel.h"
#include "versio.h"
#include "aloitussivu/loginservice.h"

#include <QTimer>

PaivitysInfo::PaivitysInfo(QObject *parent)
    : QObject{parent}
{
    QTimer::singleShot(100, this, &PaivitysInfo::pyydaInfo);
}

QDate PaivitysInfo::buildDate()
{
    QString koostepaiva(__DATE__);      // Tämä päivittyy aina versio.h:ta muutettaessa
    return QDate::fromString( koostepaiva.mid(4,3) + koostepaiva.left(3) + koostepaiva.mid(6), Qt::RFC2822Date);

}

void PaivitysInfo::pyydaInfo()
{
    // Tama muokattu versio ei laheta kayttotilastoja eika tee paivitystarkistusta
    // Kitsas Oy:n palvelimelle. Ilmoitetaan vain aloitussivulle, etta info on "saapunut".
    emit infoSaapunut();
}

void PaivitysInfo::infoSaapui()
{
   QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
   if( !reply->error()) {

        QVariantMap map = QJsonDocument::fromJson( reply->readAll() ).toVariant().toMap();

        Kirjanpito::asetaOhjeOsoite( map.value("docs").toString() );
        setService("feedback", map.value("feedback").toString());
        kp()->settings()->setValue("TilastoPaivitetty", QDate::currentDate());

        asetaInfot( map.value("info").toList());
        emit infoSaapunut();
   } else {
        QNetworkReply::NetworkError error = reply->error();
        info( "varoitus", tr("Palvelimeen ei saada yhteyttä"), LoginService::verkkovirheteksti(error, reply->errorString()),
             QString(), "verkkovirhe.png");
        emit infoSaapunut();
        emit verkkovirhe(error);
   }
    reply->deleteLater();
}
