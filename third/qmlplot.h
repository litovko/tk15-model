#ifndef QMLPLOT_H
#define QMLPLOT_H

#include <QtQuick>
class QCustomPlot;
class QCPAbstractPlottable;
class QCPItemLine;

class CustomPlotItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString graph_name READ graph_name NOTIFY graph_nameChanged())
    Q_PROPERTY(QString coord_x  READ coord_x NOTIFY coord_xChanged())
    Q_PROPERTY(QString coord_y  READ coord_y NOTIFY coord_yChanged())

public:
    CustomPlotItem( QQuickItem* parent = nullptr );
    virtual ~CustomPlotItem();

    void paint( QPainter* painter );

    Q_INVOKABLE void initCustomPlot();

    QCustomPlot *CustomPlot() const;

    QString name() const;
    int index_by_name(QString &name);

    QString graph_name() const;
    void setGraph_name(const QString &graph_name);

    QString coord_x() const;
    void setCoord_x(const QString &coord_x);

    QString coord_y() const;
    void setCoord_y(const QString &coord_y);
    void setCoord_y(const double &coord_y);

public slots:
    void showCursor(QMouseEvent *event);
signals:
    void graph_nameChanged();
    void coord_xChanged();
    void coord_yChanged();

protected:
    void routeMouseEvents( QMouseEvent* event );
    void routeWheelEvents( QWheelEvent* event );

    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseDoubleClickEvent( QMouseEvent* event );
    virtual void wheelEvent( QWheelEvent *event );
    virtual void hoverMoveEvent(QHoverEvent *event);
    virtual void timerEvent(QTimerEvent *event);

private:
    QCustomPlot *m_CustomPlot=nullptr;
    int                  m_timerId;
    QString m_name="plot";
    QString m_graph_name="";
    QString m_coord_x="";
    QString m_coord_y="";
    QCPItemLine *cursor = nullptr;

private slots:
    void graphClicked( QCPAbstractPlottable* plottable );
    void onCustomReplot();
    void updateCustomPlotSize();

};

#endif // QMLPLOT_H
