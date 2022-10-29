#ifndef DATASET_H
#define DATASET_H
#include "QString"
#include <QVariant>
#pragma once

enum TagsValue {tag_ana1,
                tag_ana2,
                tag_ana3,
                tag_gmod,
                tag_time,
                tag_type,
                tag__dev,
                tag_svet,
                tag_svet1,
                tag_svet2,
                tag_svet3,
                tag_svet4,
                tag_dig1,
                tag_dc1v,
                tag_dc2v,
                tag_toil,
                tag_toi2,
                tag_poil,
                tag_poi2,
                tag_pwrv,
                tag_pwv2,
                tag_pwv3,
                tag_vchs,
                tag_pwra,
                tag_pwa2,
                tag_pwa3,
                tag_leak,
                tag_tang,
                tag_kren,
                tag_spxy,
                tag_sp_X,
                tag_sp_Y,
                tag_drpm,
                tag_altm,
                tag_d0,
                tag_d1,
                tag_d2,
                tag_d3,
                tag_d4,
                tag_d5,
                tag_d6,
                tag_d7,
                tag_cpdo
               };
enum Rigs {
    mgbu,
    mgm7
};


class Dataset : public QObject
{
    Q_OBJECT

public slots:
    void getData();
    void finish();

public:

    Q_ENUM(TagsValue)
    Q_ENUM(Rigs)

    Rigs rig;
    explicit Dataset(QObject *parent = nullptr);
    QString source() const;
    void setSource(const QString &source);

    void process(QString &str);
    void process2(QString &str);

    QMap <QString, QPair<TagsValue,QString>> m_tags;
    QMap < QString, QMap<QString, qint64> > m_data;

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
    int spxy_to_X(const quint16 spxy, qint16 def);
    int spxy_to_Y(const quint16 spxy, qint16 def);
    quint16 m_progress=0;
    QStringList m_data_control;
    QStringList m_data_sensors;
    bool _abort=false;
    long int _line=0;
};

#endif // DATASET_H
