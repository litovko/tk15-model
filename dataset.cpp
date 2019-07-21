#include "dataset.h"
#include <QFile>
#include <QtDebug>
#include <QTime>
#include <QCoreApplication>
#include <QThread>
//Dataset::Dataset()
//{

//}

Dataset::Dataset(QObject *parent): QObject(parent)
{
    //initialize tags map
    m_tags={
        {"ana1",tag_ana1},
        {"ana2",tag_ana2},
        {"ana3",tag_ana3},
        {"gmod",tag_gmod},
        {"time",tag_time},
        {"svet",tag_svet},
        {"dig1",tag_dig1},
        {"dc1v",tag_dc1v},
        {"dc2v",tag_dc2v},
        {"toil",tag_toil},
        {"toi2",tag_toi2},
        {"poil",tag_poil},
        {"poi2",tag_poi2},
        {"pwrv",tag_pwrv},
        {"pwv2",tag_pwv2},
        {"pwv3",tag_pwv3},
        {"vchs",tag_vchs},
        {"pwra",tag_pwra},
        {"pwa2",tag_pwa2},
        {"pwa3",tag_pwa3},
        {"leak",tag_leak},
        {"tang",tag_tang},
        {"kren",tag_kren},
        {"spxy",tag_spxy},
        {"sp_X",tag_sp_X},
        {"sp_Y",tag_sp_Y},
        {"drpm",tag_drpm},
        {"altm",tag_altm}
    };


    connect(this, SIGNAL(finished()),this,SLOT(finish()));
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
    emit finished();
}

void Dataset::finish()
{
    m_data_control.removeAt(0);
    m_data_sensors.removeAt(0);
    m_data_sensors.removeAt(m_data_sensors.indexOf("type"));
    qDebug()<<"["<<m_data_control<<"]";
    qDebug()<<"["<<m_data_sensors<<"]";
}

void Dataset::process(QString &str)
{
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
        switch (m_tags[pair[0]]) {
        case tag_gmod:
            m_data[stime][pair[0]] = decode(pair[1]);
            break;
        case tag_spxy:
            _posx=spxy_to_X(pair[1].toInt(), _posx);
            m_data[stime]["sp_X"] = 10*_posx;
            _posy=spxy_to_Y(pair[1].toInt(), _posy);
            m_data[stime]["sp_Y"] = -_posy/2;
            break;
        case tag_dig1:

            break;
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
            m_data_control.append(m_data[stime].keys());
        }
        else
            if( m_data[stime].contains("type") ) {
                m_data[stime]["_dat"] = 2;
                m_data_sensors.append(m_data[stime].keys());
            }
            else
                m_data[stime]["_dat"] = 0;

    m_data_control.removeDuplicates();
    m_data_sensors.removeDuplicates();
    //qDebug()<<m_data_control;
}

int Dataset::decode(const QString &mode)
{
    if (mode=="grup1") return 1;
    if (mode=="grup2") return 2;
    if (mode=="grup3") return 3;
    if (mode=="grup4") return 4;
    if (mode=="grup5") return 5;
    return 0;
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
    //qDebug()<<">"<<(spxy>>8);
    switch (spxy>>8) {
        case 1: return -50;
        case 2: return 50;
        case 4: return 100;
        case 8: return 200;
        case 16: return 400;
        case 32: return 600;
        case 64: return 1000;
        case 128: return 0;
    }
    return def;
}

void Dataset::setProgress(const quint16 &progress)
{
    if( m_progress==progress) return;
    m_progress = progress;
    emit progressChanged(m_progress);
    //qDebug()<<"progress:"<<m_progress;
    //QCoreApplication::processEvents();
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
