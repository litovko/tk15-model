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
    //connect(this, SIGNAL(dataLoaded()), this, SLOT(plotdata()));
    //connect(this, SIGNAL(progressChanged(quint16)), this, SLOT(setProgress(quint16)));
    //создаем массив переменных
    m_values["type"] = {"Тип аппарата", 0};
    m_values["toil"] = {"Температура масла 1", 0};
    m_values["toi2"] = {"Температура масла 2", 0};
    //values["temp"] = {"Температура контроллера", 0};
    //values["humi"] = {"Влажность контроллера", 0};
    m_values["poil"] = {"Давление масла 1", 0};
    m_values["poi2"] = {"Давление масла 2", 0};
    m_values["drpm"] = {"Обороты", 0};
    m_values["pwrv"] = {"Напряжение 1", 0};
    m_values["pwv2"] = {"Напряжение 2", 0};
    m_values["pwv3"] = {"Напряжение 3", 0};
    //values["pwrs"] = {"Напряжение корпус-нейтраль", 0};
    m_values["pwra"] = {"Ток фазы 1", 0};
    m_values["pwa2"] = {"Ток фазы 2", 0};
    m_values["pwa3"] = {"Ток фазы 3", 0};
    m_values["leak"] = {"Ток утечки", 0};
    m_values["altm"] = {"Альтиметр", 0};
    m_values["dc1v"] = {"Питание +24В 1", 0};
    m_values["dc2v"] = {"Питание +24В 2", 0};
    m_values["spxy"] = {"Датчики положения", 24};
    m_values["spxy"] = {"Датчики положения", 0};
    m_values["spxy"] = {"Датчики положения", 0};
    m_values["tang"] = {"Тангаж", 0};
    m_values["kren"] = {"Крен", 0};

    outputs["out_x"] = {"Координата по оси X", 0};
    outputs["out_y"] = {"Координата по оси Y", 0};
    outputs["out_rpm"] = {"Обороты", 0};
    outputs["out_pressure"] = {"Давление масла", 0};
    outputs["out_current"] = {"Сила тока", 0};
    outputs["out_temp"] = {"Температура масла", 0};

    //создаем параметры графиков
    graph_params["type"] = {"#FFFFFF", 1};
    graph_params["toil"] = {"#FFAAFF", 1};
    graph_params["toi2"] = {"#FFFFFF", 1};
    graph_params["poil"] = {"#FFFFFF", 0.0001};
    graph_params["poi2"] = {"#FFFF00", 1};
    graph_params["temp"] = {"#FFAA00", 1};
    graph_params["drpm"] = {"#287628", 1};
    graph_params["pwrv"] = {"#FF1000", 1};
    graph_params["pwv2"] = {"#FF2000", 1};
    graph_params["pwv3"] = {"#FF3000", 1};
    graph_params["pwra"] = {"#AA88FF", 0.1};
    graph_params["pwa2"] = {"#FF20FF", 0.1};
    graph_params["pwa3"] = {"#FF10FF", 0.1};
    graph_params["leak"] = {"#0050FF", 10};
    graph_params["altm"] = {"#FF50FF", 1};
    graph_params["dc1v"] = {"#FF50FF", 0.1};
    graph_params["dc2v"] = {"#FF50FF", 0.1};
    graph_params["spxy"] = {"#FF80FF", 1};
    graph_params["sp_X"] = {"#008000", 1};
    graph_params["sp_Y"] = {"#0080B0", 1};
    graph_params["tang"] = {"#FF90FF", 1.0};
    graph_params["kren"] = {"#FF007F", 1.0};
    graph_params["vchs"] = {"#DE517C", 1.0};

    graph_params["ana1"] = {"#5D5D00", 1};
    graph_params["ana2"] = {"#5D5D08", 1};
    graph_params["ana3"] = {"#5D5D10", 1};
    graph_params["dig1"] = {"#5D5D1D", 1};
    graph_params["svet"] = {"#5D5D1D", 1};
    graph_params["svet1"] = {"#5D5D1D", 1};
    graph_params["svet2"] = {"#5D5D1D", 1};
    graph_params["svet3"] = {"#5D5D1D", 1};
    graph_params["svet4"] = {"#5D5D1D", 1};
    graph_params["gmod"]  = {"#ADADAD", 10};

    graph_params["d0"] = {"#ADAD00", 10};
    graph_params["d1"] = {"#ADAD10", 11};
    graph_params["d2"] = {"#ADAD20", 12};
    graph_params["d3"] = {"#ADAD30", 13};
    graph_params["d4"] = {"#ADAD40", 14};
    graph_params["d5"] = {"#ADAD50", 15};
    graph_params["d6"] = {"#ADAD60", 16};
    graph_params["d7"] = {"#ADAD70", 17};
    graph_params["dig1"] = {"#ADAD70", 1};
//    listen();
    emit valuesChanged();
}

Model::~Model()
{
    qDebug()<<"model destructor";
    if (m_dataset_ptr) m_dataset_ptr->abort();
    m_thread.terminate();\
    m_thread.wait();
    if (m_dataset_ptr) delete m_dataset_ptr;
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
    m_data_control.clear();
    m_data_sensors.clear();
    emit data_controlChanged();
    emit data_sensorsChanged();
    qDebug()<<"THREAD:"<<QThread::currentThreadId();
    m_dataset_ptr = new Dataset();
    m_dataset_ptr->setSource(m_filename);
    m_dataset_ptr->moveToThread(&m_thread);
    connect(&m_thread,      SIGNAL(started()),  m_dataset_ptr, SLOT(getData()));
    connect(m_dataset_ptr,  SIGNAL(progressChanged(quint16)), this, SLOT(setProgress(quint16)));
    connect(m_dataset_ptr,  SIGNAL(finished()), this, SLOT(finishFileLoading()));
    setOnload(true);
    m_thread.start();
}

void Model::finishFileLoading()
{
    m_thread.quit();
    setProgress(0);
    m_data_control=m_dataset_ptr->getData_control();
    m_data_sensors=m_dataset_ptr->getData_sensors();
    setOnload(false);
    if (m_data_control.isEmpty() || m_data_sensors.isEmpty()) return;

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
//    printdata();
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
    for (auto pair: m_values.keys()) {
        str+=pair+":"+QString::number(m_values[pair].second)+";";
    }
    str.resize(str.length()-1);
    str+="}FFFFFF";
    m_client_connection->write(str.toLocal8Bit());
}

void Model::process()
{
//    m_values["dc1v"].second = 235 + rand()%20;
//    m_values["dc2v"].second = 235 + rand()%20;
//    m_values["toil"].second = 20 + qrand()%2;
//    m_values["toi2"].second = 20 + qrand()%2;

//    m_values["drpm"].second = 0;
//    m_values["pwrv"].second = 250 + qrand()%10;
//    m_values["pwv2"].second = 250 + qrand()%10;
//    m_values["pwv3"].second = 250 + qrand()%10;
//    //values["pwrs"] = {"Напряжение корпус-нейтраль", 0};
//    m_values["pwra"].second = qrand()%10;
//    m_values["pwa2"].second = qrand()%10;
//    m_values["pwa3"].second = qrand()%10;
//    m_values["leak"].second = qrand()%10;
//    m_values["altm"].second = 0;

//    m_values["spxy"].second = 128+32768;
//    m_values["tang"].second = qrand()%10;
//    m_values["kren"].second = qrand()%10;


}

void Model::printdata()
{
    qDebug()<<"m_data.count:"<<m_dataset_ptr->m_data.count();
    for(auto el: m_dataset_ptr->m_data.toStdMap()){
         qDebug()<<el.first<<"|"<<el.second;
    }

}

void Model::toggle(QString tag, bool state)
{
    //qDebug()<<"tag:"<<tag<<" state:"<<state;
    m_chart->CustomPlot()->graph(m_chart->index_by_name(tag))->setVisible(state);
    m_chart->CustomPlot()->replot();
}

QString Model::color(QString tag)
{
    return graph_params[tag].first;
}

void Model::tagcolor(QString tag, QString color)
{
    graph_params[tag].first=color;
    m_chart->CustomPlot()->graph(m_chart->index_by_name(tag))->setPen( QPen( graph_params[tag].first ) );
    m_chart->CustomPlot()->replot();
}

void Model::stop_readfile()
{
    if(!m_dataset_ptr) return;
    m_dataset_ptr->abort();
}

QString Model::tag_description(const QString &tag)
{
    //qDebug()<<m_values[tag].first;
    return m_dataset_ptr->m_tags[tag].second;
}



void Model::plotdata()
{
    qDebug()<<"plotdata: m_data.count:"<<m_dataset_ptr->m_data.count();
    //yAxis->setTickLabelType(QCPAxis::ltDateTime);

    m_chart->CustomPlot()->clearGraphs();
    //добавляем графики сенсоров
    for(auto sens: m_data_sensors) {
        m_chart->CustomPlot()->addGraph()->setName(sens);
//        qDebug()<<sens;
    }
    //добавляем графики управляющих сигналов
    for(auto sens: m_data_control) {
        m_chart->CustomPlot()->addGraph()->setName(sens);
    }
    m_data_sensors.removeAt(m_data_sensors.indexOf("time"));
    qDebug()<<"count:"<<m_chart->CustomPlot()->graphCount();
    if (m_chart->CustomPlot()->graphCount() == 0) return;
    int x;
    static auto _gmod=0; //litovko сделано чтобы переключение режимов выглядело меандром - без наклонов.
    auto xmin = INT_MAX, xmax = INT_MIN;
    auto ymin = INT_MAX, ymax = INT_MIN;
    for(auto el: m_dataset_ptr->m_data.toStdMap()) { //цикл по всему набору данных - по временнЫм отсчетам
        //if (el.second["_dat"] != 2) continue;
        x = el.second["time"];
        x = QTime::fromString(el.first,"hh:mm:ss:zzz").msecsSinceStartOfDay();
        qDebug()<<x<<el.first<<QString("%1").arg((( double)(x)/1000.0)) << QString::number((( double)(x)/1000.0), 'g', 8);
        xmin = std::min(x,xmin);
        xmax = std::max(x,xmax);
//        qDebug()<<"t="<<el.first<<"xmin:"<<xmin<<"x:"<<x;
        for (auto v: el.second.toStdMap() ) { //цикл по тегам в одном временном отсчете
//            if (el.first=="15:24:10:181")
//                    qDebug()<<el.first<<":"<<v.first<<v.second;
//            qDebug()<<"tag="<< v.first<<" v="<<v.second;
            int e=[this](QString theName)->int {
                return m_chart->index_by_name(theName);
            }(v.first);
            if (e < 0) continue;
            if (v.first!="gmod")
                m_chart->CustomPlot()->graph(e)->addData(double(x)/1000.0, v.second*graph_params[v.first].second);
            else
                if (_gmod==v.second)
                    m_chart->CustomPlot()->graph(e)->addData(double(x)/1000.0, v.second*graph_params[v.first].second);
                else {
                    _gmod=v.second;
                    m_chart->CustomPlot()->graph(e)->addData(double(x)/1000.0, qQNaN()); //линия до этой точки не отрисовывается.
                }

            //ymin = std::min(y,ymin);

            //ymax = std::max(y,ymax);
        }


    }
    for (int i=0; i<m_chart->CustomPlot()->graphCount(); ++i)
    {
        auto s=m_chart->CustomPlot()->graph(i)->name();
        qDebug()<<"Graph:"<<s<<" datacount:"<<m_chart->CustomPlot()->graph(i)->dataCount();
        //qDebug()<<"set color to :"<<s;
        m_chart->CustomPlot()->graph(i)->setPen( QPen( graph_params[s].first ) );
        m_chart->CustomPlot()->graph(i)->setVisible(false);
        m_chart->CustomPlot()->graph(i)->setSelectable(QCP::stWhole);
        m_chart->CustomPlot()->graph(i)->selectionDecorator()->setBrush(QColor(255,255,255,20));
        if (s=="gmod")
            m_chart->CustomPlot()->graph(i)->setBrush(QBrush(QColor(255,0,0,20)));
    }
    ymin=0; ymax=300;
    qDebug()<<"xmin:"<<xmin<<" xmax:"<< xmax;
    qDebug()<<"ymin:"<<ymin<<" ymax:"<< ymax;
    m_chart->CustomPlot()->xAxis->setRange( xmin/1000.0, xmax/1000.0 );
    QSharedPointer<QCPAxisTickerTime> dateTimeTicker(new QCPAxisTickerTime);
////    dateTimeTicker->setTimeSpec(Qt::UTC);
    dateTimeTicker->setTimeFormat("%h:%m:%z");
//    m_chart->CustomPlot()->xAxis->setTicker(dateTimeTicker);
//    m_chart->CustomPlot()->xAxis->setNumberFormat("mm:ss:zzz");
    m_chart->CustomPlot()->yAxis->setRange( ymin, ymax*2 );
    m_chart->CustomPlot()->replot();
//    printdata();

}

void Model::fill_out(const QString str)
{
    auto el = m_dataset_ptr->m_data.lowerBound(str);
    if (el.value()["_dat"]==2) {
        qDebug()<<el.key()<<"|"<<el.value();
    }
}

TMapValues Model::values() const
{
    return  m_values;
}

bool Model::onload() const
{
    return m_onload;
}

void Model::setOnload(bool onload)
{
    m_onload = onload;
    emit onloadChanged();
}

quint16 Model::progress() const
{
    return m_progress;
}



void Model::setProgress(const quint16 &progress)
{
    m_progress = progress;
    //qDebug()<<"pr:"<<progress;
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
