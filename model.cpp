#include "model.h"
#include <QTcpSocket>
#include <QTime>
#include "third/qcustomplot/qcustomplot.h"
#include <algorithm>


Model::Model(QObject *parent) : QObject(parent)
{
    m_tcp_server = new QTcpServer(parent);
    connect(m_tcp_server, SIGNAL(newConnection()), this, SLOT(serverConnection()));
    connect(&m_play_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(this, SIGNAL(filenameChanged()), this, SLOT(readfile()));
    //connect(this, SIGNAL(progressChanged(quint16)), this, SLOT(setProgress(quint16)));
    //создаем массив переменных
    values["type"] = {"Тип аппарата", 0};
    values["toil"] = {"Температура масла 1", 0};
    values["toi2"] = {"Температура масла 2", 0};
    //values["temp"] = {"Температура контроллера", 0};
    //values["humi"] = {"Влажность контроллера", 0};
    values["poil"] = {"Давление масла 1", 0};
    values["poi2"] = {"Давление масла 2", 0};
    values["drpm"] = {"Обороты", 0};
    values["pwrv"] = {"Напряжение 1", 0};
    values["pwv2"] = {"Напряжение 2", 0};
    values["pwv3"] = {"Напряжение 3", 0};
    //values["pwrs"] = {"Напряжение корпус-нейтраль", 0};
    values["pwra"] = {"Ток фазы 1", 0};
    values["pwa2"] = {"Ток фазы 2", 0};
    values["pwa3"] = {"Ток фазы 3", 0};
    values["leak"] = {"Ток утечки", 0};
    values["altm"] = {"Альтиметр", 0};
    values["dc1v"] = {"Питание +24В 1", 0};
    values["dc2v"] = {"Питание +24В 2", 0};
    values["spxy"] = {"Датчики положения", 24};
    values["tang"] = {"Тангаж", 0};
    values["kren"] = {"Крен", 0};

    outputs["out_x"] = {"Координата по оси X", 0};
    outputs["out_y"] = {"Координата по оси Y", 0};
    outputs["out_rpm"] = {"Обороты", 0};
    outputs["out_pressure"] = {"Давление масла", 0};
    outputs["out_current"] = {"Сила тока", 0};
    outputs["out_temp"] = {"Температура масла", 0};

    //создаем параметры графиков
    graph_params["type"] = {"#FFFFFF", 0};
    graph_params["toil"] = {"#FFAAFF", 1};
    graph_params["toi2"] = {"#FFFFFF", 1};
    //values["temp"] = {"Температура контроллера", 0};
    //values["humi"] = {"Влажность контроллера", 0};
    graph_params["poil"] = {"#AAFF00", 1};
    graph_params["poi2"] = {"#FFFF00", 1};
    graph_params["drpm"] = {"#287628", 1};
    graph_params["pwrv"] = {"#FF1000", 1};
    graph_params["pwv2"] = {"#FF2000", 1};
    graph_params["pwv3"] = {"#FF3000", 1};
    //values["pwrs"] = {"Напряжение корпус-нейтраль", 0};
    graph_params["pwra"] = {"#AA88FF", 0.1};
    graph_params["pwa2"] = {"#FF20FF", 0.1};
    graph_params["pwa3"] = {"#FF10FF", 0.1};
    graph_params["leak"] = {"#0050FF", 1};
    graph_params["altm"] = {"#FF50FF", 1};
    graph_params["dc1v"] = {"#FF50FF", 1};
    graph_params["dc2v"] = {"#FF50FF", 1};
    graph_params["spxy"] = {"#FF80FF", 0};
    graph_params["tang"] = {"#FF90FF", 1};
    graph_params["kren"] = {"#FF007F", 1};
    graph_params["vchs"] = {"#DE517C", 1};
    listen();

}

QString Model::address() const
{
    return m_address;
}

void Model::setAddress(const QString &address)
{
    m_address = address;
    emit addressChanged();
}

uint16_t Model::port() const
{
    return m_port;
}

void Model::setPort(const uint16_t &port)
{
    m_port = port;
    emit portChanged();
}

void Model::serverConnection()
{
    m_tcp_server->pauseAccepting();
    if(m_client_connection) {qDebug()<<"Уже подключено"<<m_client_connection; return;}
    qDebug()<<"Подключено";
    setConnected(true);

    m_client_connection = m_tcp_server->nextPendingConnection();
    connect(m_client_connection, SIGNAL(disconnected()),this, SLOT(clientDisconnected()));
    connect(m_client_connection, SIGNAL(readyRead()),this, SLOT(readSlot()));
}

void Model::clientDisconnected()
{
    qWarning()<<"Соединение разорвано";
    disconnect(m_client_connection, nullptr, nullptr, nullptr);
    m_client_connection->deleteLater();
    m_client_connection=nullptr;
    setConnected(false);
}

void Model::readSlot()
{
    QString str=m_client_connection->readAll();
    if (str.length()==0) return;
    QRegExp const separator("(;|:)");
    str.resize(str.indexOf("}"));
    str.remove(0, 1);
    QStringList strlist=str.split(separator);
    for ( auto i=0; i<strlist.length(); i+=2  )
    {
        inputs.insert(strlist[i], strlist[i+1]);
    }
    process();
    sendData();
}

void Model::update()
{
    QTime next=m_current_time.addMSecs(m_play_timer.interval());
    QString ns=next.toString("hh:mm:ss:zzz");
    m_current_key =m_dataset_ptr->m_data.upperBound(ns).key();
    m_current_time = QTime::fromString(m_current_key, "hh:mm:ss:zzz");

    qDebug()<<"timer"<<m_dataset_ptr->m_data[m_current_key]<< " " <<m_current_key;
}

void Model::readfile()
{
    if(m_dataset_ptr) delete m_dataset_ptr;
    m_dataset_ptr = new Dataset(this);
    connect(m_dataset_ptr, SIGNAL(progressChanged(quint16)), this, SLOT(setProgress(quint16)));
    m_dataset_ptr->setSource(m_filename);
    int res = m_dataset_ptr->getData();
    qDebug()<<"res:"<<res;
    //qDebug()<<m_dataset_ptr->m_data.end().key();

    m_data_control=m_dataset_ptr->getData_control();
    m_data_sensors=m_dataset_ptr->getData_sensors();
    plotdata();
    emit data_controlChanged();
    emit data_sensorsChanged();
}

void Model::play()
{

    if (filename().length()==0) return;
    stop_listen();

    // получаем первый отсчет данных - начальное время
    QString first =  m_dataset_ptr->m_data.firstKey();
    QString last  =  m_dataset_ptr->m_data.lastKey();
    m_current_time = QTime::fromString(first, "hh:mm:ss:zzz");
    m_current_key = first;


    qDebug()<<"s:"<<first<<" f:"<<last;
    printdata();
    m_play_timer.start(100);
}

void Model::listen()
{
    m_play_timer.stop();
    if(m_client_connection) return;
    if (m_tcp_server->listen(QHostAddress::Any,  m_port))
        qDebug()<<"Ожидание подключения:"<<m_tcp_server->errorString();
    else
        qWarning()<<"Ошибка листенера:"<<m_tcp_server->errorString();
}

void Model::stop_listen()
{
    if(m_client_connection) {
        disconnect(m_client_connection, nullptr, nullptr, nullptr);
        m_client_connection->close();
        m_client_connection->deleteLater();
        setConnected(false);
    }
    m_client_connection=nullptr;
    m_tcp_server->close();
    qDebug()<<"Листенер остановлен";
}

void Model::sendData()
{
    QString str="{";
    for (auto pair: values.keys()) {
        str+=pair+":"+QString::number(values[pair].second)+";";
    }
    str.resize(str.length()-1);
    str+="}FFFFFF";
    m_client_connection->write(str.toLocal8Bit());
}

void Model::process()
{
    values["dc1v"].second = 235 + qrand()%20;
    values["dc2v"].second = 235 + qrand()%20;
    values["toil"].second = 20 + qrand()%2;
    values["toi2"].second = 20 + qrand()%2;

    values["drpm"].second = 0;
    values["pwrv"].second = 250 + qrand()%10;
    values["pwv2"].second = 250 + qrand()%10;
    values["pwv3"].second = 250 + qrand()%10;
    //values["pwrs"] = {"Напряжение корпус-нейтраль", 0};
    values["pwra"].second = qrand()%10;
    values["pwa2"].second = qrand()%10;
    values["pwa3"].second = qrand()%10;
    values["leak"].second = qrand()%10;
    values["altm"].second = 0;

    values["spxy"].second = 128+32768;
    values["tang"].second = qrand()%10;
    values["kren"].second = qrand()%10;


}

void Model::printdata()
{
    for(auto el: m_dataset_ptr->m_data.toStdMap()){
        if (el.second["_dat"]==0) qDebug()<<el.first<<"|"<<el;
    }

}

void Model::toggle(QString tag, bool state)
{
    qDebug()<<"tag:"<<tag<<" state:"<<state;
    m_chart->CustomPlot()->graph(m_chart->index_by_name(tag))->setVisible(state);
    m_chart->CustomPlot()->replot();
}

QString Model::color(QString tag)
{
    return graph_params[tag].first;
}



void Model::plotdata()
{
    //yAxis->setTickLabelType(QCPAxis::ltDateTime);
    //yAxis->setDateTimeFormat("hh:mm:ss");
    m_chart->CustomPlot()->clearGraphs();
    for(auto sens: m_data_sensors) {
        m_chart->CustomPlot()->addGraph()->setName(sens);
    }
    m_data_sensors.removeAt(m_data_sensors.indexOf("time"));
    qDebug()<<m_chart->name();

    int x;
    auto xmin = INT_MAX, xmax = INT_MIN;
    auto ymin = INT_MAX, ymax = INT_MIN;
    for(auto el: m_dataset_ptr->m_data.toStdMap()) { //цикл по всему набору данных
        if (el.second["_dat"] != 2) continue;
        x = el.second["time"];
        auto findname = [this](QString theName)->int {
            return m_chart->index_by_name(theName);
        };
        //qDebug()<<"       t="<<el.first;
        for (auto v: el.second.toStdMap() ) {
            //qDebug()<<"tag="<< v.first<<" v="<<v.second;
            int e=findname(v.first);
            if (e < 0) continue;
            m_chart->CustomPlot()->graph(e)->addData(x/1000.0, v.second);
            xmin = std::min(x,xmin);
            //ymin = std::min(y,ymin);
            xmax = std::max(x,xmax);
            //ymax = std::max(y,ymax);
        }
        for (int i=0; i<m_chart->CustomPlot()->graphCount(); ++i)
        {
            auto s=m_chart->CustomPlot()->graph(i)->name();
            m_chart->CustomPlot()->graph(i)->setPen( QPen( graph_params[s].first ) );
            m_chart->CustomPlot()->graph(i)->setVisible(false);
        }
        //m_chart->CustomPlot()->graph(0)->setPen( QPen( Qt::red ) );
        //y = el.second["pwra"];
        //qDebug()<<">>"<<x<<"=="<< y;
    }
    ymin=0; ymax=300;
    qDebug()<<"xmin:"<<xmin<<" xmax:"<< xmax;
    qDebug()<<"ymin:"<<ymin<<" ymax:"<< ymax;
    m_chart->CustomPlot()->xAxis->setRange( xmin/1000.0, xmax/1000.0 );
    m_chart->CustomPlot()->yAxis->setRange( ymin, ymax*2 );
    m_chart->CustomPlot()->replot();

}

void Model::fill_out(const QString str)
{
    auto el = m_dataset_ptr->m_data.lowerBound(str);
    if (el.value()["_dat"]==2) {
        qDebug()<<el.key()<<"|"<<el.value();
    }
}

quint16 Model::progress() const
{
    return m_progress;
}

void Model::setProgress(const quint16 &progress)
{
    m_progress = progress;
    qDebug()<<"pr:"<<progress;
    emit progressChanged();

}

QStringList Model::data_sensors() const
{
    return m_data_sensors;
}

QStringList Model::data_control() const
{
    return m_data_control;
}

CustomPlotItem *Model::chart() const
{
    return m_chart;
}

void Model::setChart(CustomPlotItem *chart)
{
    m_chart = chart;
    emit chartChanged();
}

QString Model::filename() const
{
    return m_filename;
}

void Model::setFilename(const QString &filename)
{
    m_filename = filename;

    emit filenameChanged();
}

bool Model::connected() const
{
    return m_connected;
}

void Model::setConnected(bool connected)
{
    m_connected = connected;
    emit connectedChanged();
}
