#include "dataset.h"
#include <QFile>
#include <QtDebug>
#include <QTime>
#include <QDate>
#include <QCoreApplication>
#include <QThread>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
//Dataset::Dataset()
//{

//}

Dataset::Dataset(QObject *parent): QObject(parent)
{
    //initialize tags map
    m_tags={
        {"ana1", {tag_ana1, "Данные джойстика 1"}},
        {"ana2", {tag_ana2, "Данные джойстика 2"}},
        {"ana3", {tag_ana3, "Данные джойстика 2"}},
        {"gmod", {tag_gmod, "Режим работы"}},
        {"time", {tag_time, "Время"}},
        {"type", {tag_type, "Тип аппарата"}},
        {"_dev", {tag__dev, "Номер платы"}},
        {"svet", {tag_svet, "Яркость прожекторов"}},
        {"svet1", {tag_svet1, "Яркость прожектора 1"}},
        {"svet2", {tag_svet2, "Яркость прожектора 2"}},
        {"svet3", {tag_svet3, "Яркость прожектора 3"}},
        {"svet4", {tag_svet4, "Яркость прожектора 4"}},
        {"dig1", {tag_dig1, "Данные сигналов управления"}},
        {"dc1v", {tag_dc1v, "Напряжение +24 в ИП1"}},
        {"dc2v", {tag_dc2v, "Напряжение +24 в ИП1"}},
        {"toil", {tag_toil, "Температура масла 1"}},
        {"toi2", {tag_toi2, "Температура масла 2"}},
        {"temp", {tag_toi2, "Температура"}},
        {"poil", {tag_poil, "Давление масла 1"}},
        {"poi2", {tag_poi2, "Давление масла 2"}},
        {"pwrv", {tag_pwrv, "Напряжение фазы 1"}},
        {"pwv2", {tag_pwv2, "Напряжение фазы 2"}},
        {"pwv3", {tag_pwv3, "Напряжение фазы 3"}},
        {"vchs", {tag_vchs, "Напряжение между нулем и землей "}},
        {"pwra", {tag_pwra, "Ток фазы 1"}},
        {"pwa2", {tag_pwa2, "Ток фазы 2"}},
        {"tang", {tag_tang, "Тангаж - наклон"}},
        {"kren", {tag_kren, "Крен "}},
        {"spxy", {tag_spxy, "Данные координат"}},
        {"sp_X", {tag_sp_X, "Координата X"}},
        {"sp_Y", {tag_sp_Y, "Координата Y"}},
        {"drpm", {tag_drpm, "Обороты в сек."}},
        {"altm", {tag_altm, "Альтиметр"}},
        {"d0", {tag_d0,   "Контакт 1 - двиг"}},
        {"d1", {tag_d1,   "Контакт 2 - промывка"}},
        {"d2", {tag_d2,   "Контакт 3 - резерв"}},
        {"d3", {tag_d3,   "Контакт 4 - насос 2"}},
        {"d4", {tag_d4,   "Контакт 5 - камера 1"}},
        {"d5", {tag_d5,   "Контакт 6 - камера 1"}},
        {"d6", {tag_d6,   "Контакт 7 - камера 1"}},
        {"d7", {tag_d7,   "Контакт 8 - камера 1"}},
        {"cpdo", {tag_cpdo,   "Давления в системе"}},
        {"azmt", {tag_azmt,   "Азимут"}},
        {"magx", {tag_magx,   "Магнитное поле по оси X"}},
        {"magy", {tag_magy,   "Магнитное поле по оси Y"}},
        {"magz", {tag_magz,   "Магнитное поле по оси Z"}},
        {"_time", {tag_vtime,   "Временная отметка данных"}},
    };


    connect(this, SIGNAL(finished()),this,SLOT(finish()));
    qDebug()<<"Dataset object created.";
}

QString Dataset::source() const
{
    return m_source;
}

void Dataset::setSource(const QString &source)
{

    m_source = source;
    m_source.replace("file:///","");
    qDebug()<<"filename:"<<m_source;
}

void Dataset::setRig(const QString &rigtype)
{
    if (rigtype == "mgbuk")
        rig =Rigs::mgbu;
    else
        rig = Rigs::mgm7;

}

void Dataset::getData()
{
    qDebug()<<"data THREAD:"<<QThread::currentThreadId();
    QFile file(m_source);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return ;
    auto size=file.size();
    while (!file.atEnd()) {
        if (_abort) {
            qDebug()<<"data loading aborted";
            emit finished();
            return;
        }
        QString str=file.readLine();
        process(str);
        setProgress(100.0*file.pos()/size);
    }
    file.close();
    qDebug()<<"GetData finished";
    emit finished();
}

void Dataset::finish()
{

    if (m_data_control.length())
        m_data_control.removeAt(0);
    if (m_data_sensors.length())
        m_data_sensors.removeAt(0);
    //    m_data_sensors.removeAt(m_data_sensors.indexOf("type"));
    qDebug()<<"["<<m_data_control<<"]";
    qDebug()<<"["<<m_data_sensors<<"]";
    //    qDebug()<<"15:25:09:416 ===="<<m_data["15:25:09:416"];
}

void Dataset::process(QString &str)
{
    //    if (_line == 0) {
    //        if (str.indexOf("Open")>=0)
    //            rig = Rigs::mgm7;  //TODO Надо во всех логах сразу писать название девайса.
    //        else
    //            rig = Rigs::mgbu;
    //    }
    _line++;
    if(rig ==  Rigs::mgbu) {
        str.prepend("[").chop(1);
        str.append("]");
    }
    if (true) { //rig ==  Rigs::mgm7
        //        qDebug()<<"line:"<<_line;
        process3(str);
        return;
    }
    static auto _posx=0; //запоминаем предыдущее положение механизма по Х
    static auto _posy=0; //запоминаем предыдущее положение механизма по Y
    auto n = str.indexOf("\"{", 10);
    if (n <= 0) return;
    auto t =str.indexOf(":")+1;

    QString stime=str.mid(t, 12);
    n = str.indexOf("{",n);
    t = str.indexOf("}",n);
    QString st = str.mid(n+1, t-n-1);

    QStringList list=st.split(";");
    for( const auto& el: list ) {
        QStringList pair = el.split(":");
        m_data[stime]["time"]=QTime(0,0,0).msecsTo(QTime::fromString(stime,"hh:mm:ss:zzz"));
        switch (m_tags[pair[0]].first) {
        case tag_gmod:
            m_data[stime][pair[0]] = decode(pair[1]);
            break;
        case tag_spxy:
            _posx=spxy_to_X(pair[1].toInt(), _posx);
            m_data[stime]["sp_X"] = 10*_posx;
            _posy=spxy_to_Y(pair[1].toInt(), _posy);
            m_data[stime]["sp_Y"] = -_posy;
            break;
        case tag_dig1:{
            //            Гидравлический насос
            //            Насос промывки
            //            Резерв
            //            Гидравлический насос №2
            //            Камера 1 (инжектор)
            //            Камера 2 (инжектор)
            //            Камера 3 (инжектор)
            //            Камера 4 (инжектор)
            uint dig = pair[1].toUInt();
            m_data[stime]["d0"] = bool(dig&1);
            m_data[stime]["d1"] = bool(dig&2);
            m_data[stime]["d2"] = bool(dig&4);
            m_data[stime]["d3"] = bool(dig&8);
            m_data[stime]["d4"] = bool(dig&16);
            m_data[stime]["d5"] = bool(dig&32);
            m_data[stime]["d6"] = bool(dig&64);
            m_data[stime]["d7"] = bool(dig&128);
            break;
        }
        case tag_svet: {
            quint16 s=pair[1].toUInt();
            m_data[stime]["svet1"] = s & 0xF;
            m_data[stime]["svet2"] = (s >>= 4) & 0xF;
            m_data[stime]["svet3"] = (s >>= 4) & 0xF;
            m_data[stime]["svet4"] = (s >>= 4) & 0xF;
            break;}

        default:
            m_data[stime][pair[0]] = pair[1].toInt();
            //    qWarning()<<"Неизвестный тэг: "<<pair[0]<<" !";
        }



    }
    //добавляем тэг типа данных
    if( m_data[stime].contains("gmod") ) {
        m_data[stime]["_dat"] = 1;
        m_data_control.append(m_data[stime].keys()); //litovko думаю что здесь сильно тормозит каждый раз
    }
    else
        if( m_data[stime].contains("type") ) {
            m_data[stime]["_dat"] = 2;
            m_data_sensors.append(m_data[stime].keys());
        }
        else
            m_data[stime]["_dat"] = 0;

    m_data_control.removeDuplicates(); //litovko думаю что здесь сильно тормозит каждый раз
    m_data_sensors.removeDuplicates();
    //qDebug()<<m_data_control;
}
void Dataset::process2(QString &str)
{
    static auto _posx=0; //запоминаем предыдущее положение механизма по Х
    static auto _posy=0; //запоминаем предыдущее положение механизма по Y
    bool _send = str.indexOf("sent:");
    auto n = str.indexOf("[{", 20);
    if (n <= 0) return;
    auto t =str.indexOf("}]")+1;
    QString st = str.mid(n, t-n+1);
    QString stime = str.mid(9,12);

    int dd = str.mid(3,2).toInt();
    int mm = str.mid(6,2).toInt();
    int yy = QDate::currentDate().toString("yyyy").toInt();
    //    qDebug()<<stime<<">>"<<st << dd << mm << yy;
    QDate d(yy, mm, dd);
    QDateTime dt(d.startOfDay( Qt::LocalTime));
    //    qDebug()<<"--"<<stime <<dt;
    dt = dt.addMSecs(QTime(0,0,0).msecsTo(QTime::fromString(stime,"hh:mm:ss:zzz")));
    //    qDebug()<<stime <<dt<< dt.toMSecsSinceEpoch()<<double(dt.toMSecsSinceEpoch())/1000.0;
    //    m_data[stime]["time"]=QTime(0,0,0).msecsTo(QTime::fromString(stime,"hh:mm:ss:zzz"))/1000.0;
    m_data[stime]["time"]=(dt.toMSecsSinceEpoch());
    st.replace("\\","");
    qDebug()<<stime<<"2>>"<<st;
    QJsonDocument doc = QJsonDocument::fromJson(st.toUtf8());
    qDebug()<<"doc:"<<doc.isObject()<<doc.isNull()<<doc.isEmpty();
    if (doc.isArray()) {
        QJsonArray ja=doc.array();
        qDebug()<<"ja:"<<ja;
        for (int i = 0; i < ja.size(); i++) { // цикл по каждой плате
            if (_send && i==1) break; //  вторую плату для данных пропускаем
            QJsonObject jv= ja.at(i).toObject();
            for ( auto&& key: jv.keys()) {
                qDebug()<< i << "m:" << key<<" = "<<jv[key];
                switch (m_tags[key].first) {
                case tag_type:
                    m_data[stime][key] = decode(jv[key].toString());

                    break;
                case tag_gmod:
                    m_data[stime][key] = decode(jv[key].toString());
                    break;
                case tag_spxy:
                    _posx=spxy_to_X(jv[key].toString().toInt(), _posx);
                    m_data[stime]["sp_X"] = 10*_posx;
                    _posy=spxy_to_Y(jv[key].toString().toInt(), _posy);
                    m_data[stime]["sp_Y"] = -_posy;
                    break;
                case tag_dig1:{
                    //            Гидравлический насос
                    //            Насос промывки
                    //            Резерв
                    //            Гидравлический насос №2
                    //            Камера 1 (инжектор)
                    //            Камера 2 (инжектор)
                    //            Камера 3 (инжектор)
                    //            Камера 4 (инжектор)
                    uint dig = jv[key].toString().toUInt();
                    m_data[stime]["dig1"] = dig;
                    m_data[stime]["d0"] = bool(dig&1);
                    m_data[stime]["d1"] = bool(dig&2);
                    m_data[stime]["d2"] = bool(dig&4);
                    m_data[stime]["d3"] = bool(dig&8);
                    m_data[stime]["d4"] = bool(dig&16);
                    m_data[stime]["d5"] = bool(dig&32);
                    m_data[stime]["d6"] = bool(dig&64);
                    m_data[stime]["d7"] = bool(dig&128);
                    break;
                }
                case tag_svet: {

                    quint16 s=jv[key].toString().toUInt();
                    m_data[stime]["svet1"] = s & 0xF;
                    m_data[stime]["svet2"] = (s >>= 4) & 0xF;
                    m_data[stime]["svet3"] = (s >>= 4) & 0xF;
                    m_data[stime]["svet4"] = (s >>= 4) & 0xF;
                    break;}
                case tag__dev:{

                    break;
                }
                    //              case tag_ana1:
                    //              case tag_ana2:
                    //              case tag_ana3: {
                    //                  if (m_data[stime]["_dev"] == 0)
                    //                     {
                    ////                      qDebug()<<"_dev:"<<m_data[stime]["_dev"] ;
                    //                      break;
                    //                  }

                    //              }
                default:{
                    m_data[stime][key] = jv[key].toInt();
                    //                  if (stime == "15:24:10:181")
                    //                      qDebug()<<stime<<"::::"<<key<<m_data[stime][key]<<jv[key].toInt();

                    //                  if (key=="ana1")
                    //                      if (m_data[stime][key]>0)
                    //                          qDebug()<<stime<<":"<<key<<m_data[stime][key];
                    //    qWarning()<<"Неизвестный тэг: "<<pair[0]<<" !";
                }
                    //                  qDebug()<<"15:24:10:181 ===="<<m_data["15:24:10:181"]<<"stime:"<<stime;
                }



            }
            //добавляем тэг типа данных
            if( m_data[stime].contains("dig1") ) { // Данные управления
                m_data[stime]["_dat"] = 1;
                m_data_control.append(m_data[stime].keys()); //litovko думаю что здесь сильно тормозит каждый раз
            }
            else
                if( m_data[stime].contains("type") ) {
                    m_data[stime]["_dat"] = 2;
                    m_data_sensors.append(m_data[stime].keys());
                }
                else
                    m_data[stime]["_dat"] = 0;
            //                            qDebug()<<">>"<<m_data[stime];
            m_data_control.removeDuplicates(); //litovko думаю что здесь сильно тормозит каждый раз
            m_data_sensors.removeDuplicates();
        }

    }

}

void Dataset::process3(QString &str)
{
    QString stime, st;
    static auto _posx=0; //запоминаем предыдущее положение механизма по Х
    static auto _posy=0; //запоминаем предыдущее положение механизма по Y
    bool _send = str.indexOf("sent:");
    auto n = str.indexOf("[{", 0);
    if (n < 0) return;
    auto t =str.indexOf("}]")+1;
    st = str.mid(n, t-n+1);
    int dd, mm, yy;

    if (rig == Rigs::mgm7) {
        stime = str.mid(8,12);
        dd = str.midRef(2,2).toInt();
        mm = str.midRef(5,2).toInt();
        yy = QDate::currentDate().toString("yyyy").toInt();
    }
    else
    {
        stime = str.mid(31,12);
        dd = str.midRef(20,2).toInt();
        mm = str.midRef(23,2).toInt();
        yy = QDate::currentDate().toString("yyyy").toInt();
        st = str;
    }
    //    qDebug()<<stime<<"1>>"<<st;
    QDate d(yy, mm, dd);
    QDateTime dt(d.startOfDay( Qt::LocalTime));
    //    qDebug()<<"--"<<stime <<dt;
    dt = dt.addMSecs(QTime(0,0,0).msecsTo(QTime::fromString(stime,"hh:mm:ss.zzz")));
//        qDebug()<<stime <<dt<< dt.toMSecsSinceEpoch()<<double(dt.toMSecsSinceEpoch())/1000.0;
    //    m_data[stime]["time"]=QTime(0,0,0).msecsTo(QTime::fromString(stime,"hh:mm:ss:zzz"))/1000.0;
    qint64 ddd=dt.toMSecsSinceEpoch();
    qDebug()<<ddd;
    m_data[stime]["time"]=(dt.toMSecsSinceEpoch());
    QJsonDocument doc = QJsonDocument::fromJson(st.toUtf8());
    //    qDebug()<<"doc:"<<doc.isObject()<<doc.isNull()<<doc.isEmpty();
    if (doc.isArray()) {
        QJsonArray ja=doc.array();
        //        qDebug()<<"ja:"<<ja;
        for (int i = 0; i < ja.size(); i++) { // цикл по каждой плате
            if (_send && i==1) break; //  вторую плату для данных пропускаем
            QJsonObject jv= ja.at(i).toObject();
            for ( auto&& key: jv.keys()) {
                //              qDebug()<< i << "m:" << key;
                switch (m_tags[key].first) {
                case tag_vtime:
                    m_data[stime]["time"]=QDateTime::fromString(jv[key].toString(),"dd.MM.yyyy hh:mm:ss.zzz").toMSecsSinceEpoch(); //29.10.2022 15:55:53.314
                    qDebug()<<stime<<jv[key].toString()<<QDateTime::fromString(jv[key].toString(),"dd.MM.yyyy hh:mm:ss.zzz")<<m_data[stime]["time"];
                    break;
                case tag_gmod:
                    m_data[stime][key] = decode(jv[key].toString());
                    break;
                case tag_spxy:
                    _posx=spxy_to_X(jv[key].toString().toInt(), _posx);
                    m_data[stime]["sp_X"] = 10*_posx;
                    _posy=spxy_to_Y(jv[key].toString().toInt(), _posy);
                    m_data[stime]["sp_Y"] = -_posy;
                    break;
                case tag_dig1:{
                    //            Гидравлический насос
                    //            Насос промывки
                    //            Резерв
                    //            Гидравлический насос №2
                    //            Камера 1 (инжектор)
                    //            Камера 2 (инжектор)
                    //            Камера 3 (инжектор)
                    //            Камера 4 (инжектор)

                    uint dig = jv[key].toInt();
                    //                    qDebug()<<"DIG:"<<jv[key]<<" == "<<dig<<(10*(dig&1)) ;
                    m_data[stime]["d0"] = bool(dig&1);
                    m_data[stime]["d1"] = bool(dig&2);
                    m_data[stime]["d2"] = bool(dig&4);
                    m_data[stime]["d3"] = bool(dig&8);
                    m_data[stime]["d4"] = bool(dig&16);
                    m_data[stime]["d5"] = bool(dig&32);
                    m_data[stime]["d6"] = bool(dig&64);
                    m_data[stime]["d7"] = bool(dig&128);
                    m_data[stime]["dig1"] = jv[key].toInt();
                    break;
                }
                case tag_svet: {

                    quint16 s=jv[key].toString().toUInt();
                    m_data[stime]["svet1"] = s & 0xF;
                    m_data[stime]["svet2"] = (s >>= 4) & 0xF;
                    m_data[stime]["svet3"] = (s >>= 4) & 0xF;
                    m_data[stime]["svet4"] = (s >>= 4) & 0xF;
                    break;}
                case tag__dev:{

                    break;
                }
                    //              case tag_ana1:
                    //              case tag_ana2:
                    //              case tag_ana3: {
                    //                  if (m_data[stime]["_dev"] == 0)
                    //                     {
                    ////                      qDebug()<<"_dev:"<<m_data[stime]["_dev"] ;
                    //                      break;
                    //                  }

                    //              }
                case tag_cpdo: {
                    QString s = jv[key].toString().replace(" ","");

                    QString sub = s.mid(0,4);
                    bool ok;
                    int pressure = sub.toInt(&ok, 16);
                    //                    qDebug()<< pressure<<sub << s ;
                    qreal _a = 0.00782881;
                    m_data[stime]["poil"] = pressure*_a*10000;
                    break;
                }
                default:
                    m_data[stime][key] = jv[key].toInt();
                }



            }
            //добавляем тэг типа данных
            if( m_data[stime].contains("dig1") ) { // Данные управления
                m_data[stime]["_dat"] = 1;
                m_data_control.append(m_data[stime].keys()); //litovko думаю что здесь сильно тормозит каждый раз
            }
            else
                if( m_data[stime].contains("type") ) {
                    m_data[stime]["_dat"] = 2;
                    m_data_sensors.append(m_data[stime].keys());
                }
                else
                    m_data[stime]["_dat"] = 0;
            //                            qDebug()<<">>"<<m_data[stime];
            m_data_control.removeDuplicates(); //litovko думаю что здесь сильно тормозит каждый раз
            m_data_sensors.removeDuplicates();
        }

    }

}


int Dataset::decode(const QString &mode)
{
    if (mode=="grup1") return 10;
    if (mode=="grup2") return 20;
    if (mode=="grup3") return 30;
    if (mode=="grup4") return 40;
    if (mode=="grup5") return 50;
    if (mode=="npa__") return 70;
    return 222;
}

int Dataset::spxy_to_X(const quint16 spxy, qint16 def)
{
    switch (spxy&255) {
    case 1: return -1;
    case 2: return -2;
    case 4: return -3;
    case 8: return 1;
    case 16: return 2;
    case 32: return 3;
    case 128: return 0;
    }
    return def;
}


int Dataset::   spxy_to_Y(const quint16 spxy, qint16 def)
{
    //    qDebug()<<">"<<(spxy>>8);
    switch (spxy>>8) {
    case 1: return -50;
    case 2: return 50;
    case 4: return 100;
    case 8: return 200;
    case 16: return 400;
    case 36: return 600;
    case 32: return 600;
    case 64: return 1000;
    case 128: return 0;
    case 0: return 0;
    }
    return def;
}

void Dataset::setProgress(const quint16 &progress)
{
    if( m_progress==progress) return;
    m_progress = progress;
    emit progressChanged(m_progress);
    //    qDebug()<<"progress:"<<m_progress;
    //    QCoreApplication::processEvents();
}

void Dataset::abort()
{
    _abort = true;
}

QStringList Dataset::getData_sensors() const
{
    return m_data_sensors;
}

QStringList Dataset::getData_control() const
{
    return m_data_control;
}
