#ifndef DATASET_H
#define DATASET_H
#include "QString"
#include <QVariant>

class Dataset : public QObject
{
    Q_OBJECT
public slots:
    void getData();

public:
    explicit Dataset(QObject *parent = nullptr);
    QString source() const;
    void setSource(const QString &source);

    void process(QString &str);

    QMap < QString, QMap<QString, qint32> > m_data;

    QStringList getData_control() const;

    QStringList getData_sensors() const;

    void setProgress(const quint16 &progress);

    void abort();
signals:
    void progressChanged(quint16 newValue);
    void finished();

private:
    QString m_source="";
    int decode(const QString &mode);
    quint16 m_progress=0;
    QStringList m_data_control;
    QStringList m_data_sensors;
    bool _abort=false;
};

#endif // DATASET_H
