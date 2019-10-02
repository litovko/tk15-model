#include <QtGui/QGuiApplication>
#include <QApplication>

#include <QtQml>
#include <QSettings>

#define giko_name "HYCO"
#define giko_program "TK-15 Model"
#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QTextStream>
#include "third/qmlplot.h"
#include "build.h"



static QFile logfile;
static QTextStream out(&logfile);
static bool recordinglog=false;
static QTextCodec *codec = QTextCodec::codecForName("KOI8-R");

//Включение и отключение записи логов
extern void toggle_log(bool recordlog) {
    if (!recordlog) {
        if(logfile.isOpen()) {
            logfile.write("Close\n");
            logfile.flush();
            out.flush();
            logfile.close();
        }
        recordinglog=false;
        return;
    }
    if (!logfile.isOpen()) {
        logfile.setFileName("log/tk15_log_"+QDateTime::currentDateTime().toString("dd-MM-yyyy_hh-mm-ss-zzz.log"));
        logfile.open(QIODevice::WriteOnly | QIODevice::Text);
        logfile.write("Open\n");
    }
    recordinglog=true;
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "D:%s %s (%s:%u, %s)\n",QTime::currentTime().toString("hh:mm:ss:zzz ").toLocal8Bit().data(), localMsg.constData(), context.file, context.line, context.function);
        out<<"Debug:"<<QTime::currentTime().toString("hh:mm:ss:zzz ").toUtf8().data()<<" "<<msg<<"("<<context.file<<":"<<context.line<<", "<<context.function<<")\n";
        break;
    case QtWarningMsg:
        fprintf(stderr, "W:%s %s (%s:%u, %s)\n",QTime::currentTime().toString("hh:mm:ss:zzz ").toLocal8Bit().data(), localMsg.constData(), context.file, context.line, context.function);
        out<<"Warning:"<<QTime::currentTime().toString("hh:mm:ss:zzz ").toLocal8Bit().data()<<" "<<msg<<"("<<context.file<<":"<<context.line<<", "<<context.function<<")\n";
        break;
    case QtCriticalMsg:
        fprintf(stderr, "C:%s %s (%s:%u, %s)\n",QTime::currentTime().toString("hh:mm:ss:zzz ").toLocal8Bit().data(), localMsg.constData(), context.file, context.line, context.function);
        out<<"Critical:"<<QTime::currentTime().toString("hh:mm:ss:zzz ").toLocal8Bit().data()<<" "<<localMsg.constData()<<"("<<context.file<<":"<<context.line<<", "<<context.function<<")\n";
        break;
    case QtFatalMsg:
        fprintf(stderr, "F:%s %s (%s:%u, %s)\n",QTime::currentTime().toString("hh:mm:ss:zzz ").toLocal8Bit().data(), localMsg.constData(), context.file, context.line, context.function);
        out<<"FATAL:"<<QTime::currentTime().toString("hh:mm:ss:zzz ").toLocal8Bit().data()<<" "<<msg<<"("<<context.file<<":"<<context.line<<", "<<context.function<<")\n";
        abort();
    default:
        abort();
    }
    if(logfile.isOpen()) {
        out.flush();
        logfile.flush();
    }
}


int main(int argc, char *argv[])
{
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache);
    //QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
    qInstallMessageHandler(myMessageOutput);
    toggle_log(true);

    setlocale(LC_ALL, ""); // избавляемся от кракозябров в консоли

    QSettings settings(giko_name, giko_program);
    qDebug()<<QTime::currentTime().toString("hh:mm:ss:zzz ")<<"Start"<<giko_name<<"  "<<giko_program;
    qDebug() << "Build number:" << BUILD <<"REV:"<<GIT_REV;
    QCoreApplication::setAttribute(Qt::AA_DisableShaderDiskCache);
    QApplication app(argc, argv);
    app.setOrganizationName(giko_name);
    app.setOrganizationDomain("hyco.ru");
    app.setApplicationName(giko_program);
    qmlRegisterType<Model>(giko_name, 1, 0, "Model");
    qmlRegisterType<CustomPlotItem>("CustomPlot", 1, 0, "CustomPlotItem");

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    qDebug()<<"Движок загружен"<<giko_name<<"  "<<giko_program;

    int ex=app.exec();
    qDebug()<<"Завершение работы программы:"<<'"'<<giko_name<<"/"<<giko_program<<'"';
    //toggle_log(false);
    out.flush();
    return ex;
}

