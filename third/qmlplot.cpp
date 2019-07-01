#include "qmlplot.h"
#include "qcustomplot/qcustomplot.h"
#include <QDebug>

CustomPlotItem::CustomPlotItem( QQuickItem* parent ) : QQuickPaintedItem( parent )
    , m_CustomPlot( nullptr ), m_timerId( 0 )
{
    setFlag( QQuickItem::ItemHasContents, true );
    setAcceptedMouseButtons( Qt::AllButtons );

    connect( this, &QQuickPaintedItem::widthChanged, this, &CustomPlotItem::updateCustomPlotSize );
    connect( this, &QQuickPaintedItem::heightChanged, this, &CustomPlotItem::updateCustomPlotSize );
}

CustomPlotItem::~CustomPlotItem()
{
    delete m_CustomPlot;
    m_CustomPlot = nullptr;

    if(m_timerId != 0) {
        killTimer(m_timerId);
    }
}

void CustomPlotItem::initCustomPlot()
{
    if (m_CustomPlot)
        delete m_CustomPlot;
    else
        m_CustomPlot = new QCustomPlot();

    updateCustomPlotSize();
    m_CustomPlot->addGraph();
    m_CustomPlot->graph( 0 )->setPen( QPen( Qt::red ) );
    m_CustomPlot->xAxis->setLabel( "t" );
    //m_customPlot->xAxis->setTicker(textTicker);
    //yAxis->setTickLabelType(QCPAxis::ltDateTime);
    //yAxis->setDateTimeFormat("hh:mm:ss");

    m_CustomPlot->yAxis->setLabel( "S" );
    m_CustomPlot->xAxis->setRange( 0, 500 );
    m_CustomPlot->yAxis->setRange( 0, 500 );
    // STYLING

    m_CustomPlot->setBackground(QBrush(Qt::black));
    //
    m_CustomPlot->xAxis->setBasePen(QPen( Qt::white ));
    m_CustomPlot->xAxis->setLabelColor(Qt::white);
    m_CustomPlot->xAxis->setTickPen(QPen(Qt::white));
    m_CustomPlot->xAxis->setTickLabelColor(Qt::white);
    m_CustomPlot->xAxis->setSubTickPen(QPen(Qt::white));

    m_CustomPlot->yAxis->setBasePen(QPen( Qt::white ));
    m_CustomPlot->yAxis->setLabelColor(Qt::white);
    m_CustomPlot->yAxis->setTickPen(QPen(Qt::white));
    m_CustomPlot->yAxis->setTickLabelColor(Qt::white);
    m_CustomPlot->yAxis->setSubTickPen(QPen(Qt::white));

    m_CustomPlot ->setInteractions( QCP::iRangeDrag | QCP::iRangeZoom );
    QSharedPointer<QCPAxisTickerTime> dateTicker(new QCPAxisTickerTime);
    dateTicker->setTimeFormat("%d:%h:%m:%s");
    m_CustomPlot->xAxis->setTicker(dateTicker);

    //startTimer(5);

    connect( m_CustomPlot, &QCustomPlot::afterReplot, this, &CustomPlotItem::onCustomReplot );

    m_CustomPlot->replot();
}


void CustomPlotItem::paint( QPainter* painter )
{
    if (m_CustomPlot)
    {
        QPixmap    picture( boundingRect().size().toSize() );
        QCPPainter qcpPainter( &picture );

        m_CustomPlot->toPainter( &qcpPainter );

        painter->drawPixmap( QPoint(), picture );
    }
}

void CustomPlotItem::mousePressEvent( QMouseEvent* event )
{
    qDebug() << Q_FUNC_INFO;
    routeMouseEvents( event );
}

void CustomPlotItem::mouseReleaseEvent( QMouseEvent* event )
{
    qDebug() << Q_FUNC_INFO;
    routeMouseEvents( event );
}

void CustomPlotItem::mouseMoveEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void CustomPlotItem::mouseDoubleClickEvent( QMouseEvent* event )
{
    qDebug() << Q_FUNC_INFO;
    routeMouseEvents( event );
}

void CustomPlotItem::wheelEvent( QWheelEvent *event )
{
    routeWheelEvents( event );
}

void CustomPlotItem::timerEvent(QTimerEvent *event)
{
    static double t, U;
    U = ((double)rand() / RAND_MAX) * 5;
    m_CustomPlot->graph(0)->addData(t, U);
    qDebug() << Q_FUNC_INFO << QString("Adding dot t = %1, S = %2").arg(t).arg(U);
    t++;
    m_CustomPlot->replot();
}

QString CustomPlotItem::name() const
{
    return m_name;
}

int CustomPlotItem::index_by_name(QString &name)
{
    int foundIndex = -1;
    for (int i=0; i<CustomPlot()->graphCount(); ++i)
    {
      if (CustomPlot()->graph(i)->name() == name)
      {
        foundIndex = i;
        break;
      }
    }
    return foundIndex;
}

QCustomPlot *CustomPlotItem::CustomPlot() const
{
    return m_CustomPlot;
}

void CustomPlotItem::graphClicked( QCPAbstractPlottable* plottable )
{
    qDebug() << Q_FUNC_INFO << QString( "Clicked on graph '%1 " ).arg( plottable->name() );
}

void CustomPlotItem::routeMouseEvents( QMouseEvent* event )
{
    if (m_CustomPlot)
    {
        QMouseEvent* newEvent = new QMouseEvent( event->type(), event->localPos(), event->button(), event->buttons(), event->modifiers() );
        QCoreApplication::postEvent( m_CustomPlot, newEvent );
    }
}

void CustomPlotItem::routeWheelEvents( QWheelEvent* event )
{
    if (m_CustomPlot)
    {
        QWheelEvent* newEvent = new QWheelEvent( event->pos(), event->delta(), event->buttons(), event->modifiers(), event->orientation() );
        QCoreApplication::postEvent( m_CustomPlot, newEvent );
    }
}

void CustomPlotItem::updateCustomPlotSize()
{
    if (m_CustomPlot)
    {
        m_CustomPlot->setGeometry(0, 0, (int)width(), (int)height());
        m_CustomPlot->setViewport(QRect(0, 0, (int)width(), (int)height()));
    }
}

void CustomPlotItem::onCustomReplot()
{
    //qDebug() << Q_FUNC_INFO;
    update();
}
