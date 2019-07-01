#include "dataset.h"
#include <QFile>
#include <QtDebug>
#include <QTime>

Dataset::Dataset()
{

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

int Dataset::getData()
{
    QFile file(m_source);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0;
    auto size=file.size();
    while (!file.atEnd()) {
        QString str=file.readLine();
        process(str);
        m_progress=(100*file.pos())/size;
    }
    m_data_control.removeAt(0);
    m_data_sensors.removeAt(0);
    qDebug()<<"["<<m_data_control<<"]";
    qDebug()<<"["<<m_data_sensors<<"]";
    return m_data.size();
}

void Dataset::process(QString &str)
{
    /*
      Debug:13:47:49:972  Rig read : "{type:mgbu_;dc1v:227;dc2v:0;toil:18;toi2:0;poil:1;poi2:0;pwrv:236;pwv2:231;pwv3:235;vchs:43;pwra:1;pwa2:1;pwa3:0;leak:0;tang:4;kren:-3;spxy:64;drpm:0;altm:1}DEADBEEF"(:0, )
      Debug:13:48:12:757  Options with recording Cam1:  :clock-jitter=10000,:clock-synchro=0,:ods,:spu,:high-priority,:network-caching=300,:sout=#duplicate{dst=display,dst=std{access=file,mux=mp4,dst=D:/video/hyco-cam1-below-07-06-2019_13-48-12.mpg}}(qrc:/main.qml:153, getrecordoption)
     */
//    if (str.indexOf("Rig read")>0)
//        qDebug()<<str;
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
        if (pair[0]=="gmod") {
            m_data[stime][pair[0]] = decode(pair[1]);

        } else {
            m_data[stime][pair[0]] = pair[1].toInt();

        }
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
    }
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

QStringList Dataset::getData_sensors() const
{
    return m_data_sensors;
}

QStringList Dataset::getData_control() const
{
    return m_data_control;
}
