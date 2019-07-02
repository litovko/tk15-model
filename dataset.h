#ifndef DATASET_H
#define DATASET_H
#include "QString"
#include <QVariant>

class Dataset : public QObject
{
    Q_OBJECT
public:
    explicit Dataset(QObject *parent = nullptr);
    QString source() const;
    void setSource(const QString &source);
    int getData();
    void process(QString &str);

    QMap < QString, QMap<QString, qint32> > m_data;

    QStringList getData_control() const;

    QStringList getData_sensors() const;

    void setProgress(const quint16 &progress);
signals:
    void progressChanged(quint16 newValue);

private:
    QString m_source="";
    int decode(const QString &mode);
    quint16 m_progress=0;
    QStringList m_data_control;
    QStringList m_data_sensors;
};

#endif // DATASET_H
