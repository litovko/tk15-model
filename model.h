#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QTcpServer>
#include <QTimer>
#include <QTime>
#include <QThread>
#include "dataset.h"
#include "third/qmlplot.h"

typedef QMap < QString, QPair<QString, quint16> > TMapValues;
Q_DECLARE_METATYPE(TMapValues)

class Model : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged())
    Q_PROPERTY(bool onload READ onload NOTIFY onloadChanged())
    Q_PROPERTY(quint16 progress READ progress NOTIFY progressChanged())
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(CustomPlotItem *chart READ chart WRITE setChart NOTIFY chartChanged)
    Q_PROPERTY(QStringList data_control READ data_control NOTIFY data_controlChanged)
    Q_PROPERTY(QStringList data_sensors READ data_sensors NOTIFY data_sensorsChanged)
    //Q_PROPERTY(TMapValues values MEMBER m_values NOTIFY valuesChanged)

public:
    explicit Model(QObject *parent = nullptr);
    ~Model();

    QString address() const;
    void setAddress(const QString &address);

    uint16_t port() const;
    void setPort(const uint16_t &port);

    bool connected() const;
    void setConnected(bool connected);

    QString filename() const;
    void setFilename(const QString &filename);

    CustomPlotItem *chart() const;
    void setChart(CustomPlotItem *chart);

    QStringList data_control() const;

    QStringList data_sensors() const;

    quint16 progress() const;




    bool onload() const;
    void setOnload(bool onload);

    TMapValues values() const;

    const QString &rigtype() const;
    void setRigtype(const QString &newRigtype);

signals:
    void addressChanged();
    void portChanged();
    void connectedChanged();
    void filenameChanged();
    void chartChanged();
    void data_controlChanged();
    void data_sensorsChanged();
    void progressChanged();
    void dataLoaded();
    void onloadChanged();
    void valuesChanged();

    void rigtypeChanged();

public slots:
    void setProgress(const quint16 &progress);
    void serverConnection();
    void clientDisconnected();
    void readSlot(); //read data from socket
    void update();
    void readfile();
    void plotdata();
    void finishFileLoading();
    Q_INVOKABLE void play();
    Q_INVOKABLE void listen();
    Q_INVOKABLE void stop_listen();
    Q_INVOKABLE void printdata();
    Q_INVOKABLE void toggle(QString tag, bool state);
    Q_INVOKABLE QString color(QString tag);
    Q_INVOKABLE void tagcolor(QString tag, QString color);
    Q_INVOKABLE void stop_readfile();
    Q_INVOKABLE QString tag_description(const QString &tag);

private:
    void sendData();
    void process();

    void fill_out(const QString str);

    QString m_address="localhost";
    uint16_t m_port=1212;

    QTcpServer* m_tcp_server=nullptr;
    QTcpSocket* m_client_connection = nullptr;
    bool m_connected = false;
    bool m_onload = false;

    TMapValues m_values;
    QMap < QString, QString>  inputs;
    QMap < QString, QPair<QString, qint32> > outputs;
    QMap < QString, QPair<QString, double>> graph_params; //цвет и коэффициент графика
    QString m_filename="";
    Dataset* m_dataset_ptr = nullptr;

    QThread m_thread;

    QTimer m_play_timer;
    QTime m_current_time;
    QString m_current_key;

    quint16 m_progress=0;

    //===
    CustomPlotItem *m_chart=nullptr;
    QStringList m_data_control;
    QStringList m_data_sensors;
    QString m_rigtype = "mgbuk";

    Q_PROPERTY(QString rigtype READ rigtype WRITE setRigtype NOTIFY rigtypeChanged)
};

#endif // MODEL_H
