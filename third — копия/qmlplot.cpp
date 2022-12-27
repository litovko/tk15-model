#include "qmlplot.h"
#include "qcustomplot/qcustomplot.h"
#include <QDebug>

CustomPlotItem::CustomPlotItem( QQuickItem* parent ) : QQuickPaintedItem( parent )
    , m_CustomPlot( nullptr ), m_timerId( 0 )
{
    setFlag( QQuickItem::ItemHasContents, true );
    setAcceptedMouseButtons( Qt::AllButtons );
    setAcceptHoverEvents(true);
    connect( this, &QQuickPaintedItem::widthChanged, this, &CustomPlotItem::updateCustomPlotSize );
    connect( this, &QQuickPaintedItem::heightChanged, this, &CustomPlotItem::updateCustomPlotSize );
}

CustomPlotItem::~CustomPlotItem()
{
    if (cursor){
        delete cursor;
        cursor = nullptr;
    }
    if (m_CustomPlot) {
        delete m_CustomPlot;
        m_CustomPlot = nullptr;
    }

    if(m_timerId != 0) {
        killTimer(m_timerId);
    }
}

void CustomPlotItem::initCustomPlot()
{
    if (cursor){
        delete cursor;
        cursor = nullptr;
    }
    if (m_CustomPlot) {
        delete m_CustomPlot;
        m_CustomPlot = nullptr;
    }


    m_CustomPlot = new QCustomPlot();
    cursor = new QCPItemLine(m_CustomPlot);
    cursor->setPen(QPen( Qt::red ));
    connect(m_CustomPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(showCursor(QMouseEvent*)));

    updateCustomPlotSize();
    m_CustomPlot->addGraph();
    m_CustomPlot->graph( 0 )->setPen( QPen( Qt::red ) );
    m_CustomPlot->graph( 0 )->setVisible(false);
    m_CustomPlot->graph( 0 )->addData(0,0);
    m_CustomPlot->graph( 0 )->addData(10,100);
    m_CustomPlot->graph( 0 )->addData(20,0);
    m_CustomPlot->xAxis->setLabel( "время" );
    //m_customPlot->xAxis->setTicker(textTicker);
    //yAxis->setTickLabelType(QCPAxis::ltDateTime);
    //yAxis->setDateTimeFormat("hh:mm:ss");

    m_CustomPlot->yAxis->setLabel( "Знач." );
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

    m_CustomPlot ->setInteractions( QCP::iRangeDrag | QCP::iRangeZoom|QCP::iSelectPlottables );
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
    qDebug() << Q_FUNC_INFO<<"!!!";
    routeMouseEvents( event );
}

void CustomPlotItem::mouseReleaseEvent( QMouseEvent* event )
{
    qDebug() << Q_FUNC_INFO<<"###";
    routeMouseEvents( event );
}

void CustomPlotItem::mouseMoveEvent( QMouseEvent* event )
{
    //qDebug() << Q_FUNC_INFO<<"***";
    routeMouseEvents( event );
}

void CustomPlotItem::mouseDoubleClickEvent( QMouseEvent* event )
{
    qDebug() << Q_FUNC_INFO<<"|||"<<m_name;
    routeMouseEvents( event );
}

void CustomPlotItem::wheelEvent( QWheelEvent *event )
{
    routeWheelEvents( event );
}

void CustomPlotItem::hoverMoveEvent(QHoverEvent *event)
{
    QMouseEvent mouseEvent(QEvent::MouseMove,event->posF(),Qt::NoButton,Qt::NoButton,event->modifiers());
    routeMouseEvents(&mouseEvent);
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

QString CustomPlotItem::coord_y() const
{
    return m_coord_y;
}

void CustomPlotItem::setCoord_y(const QString &coord_y)
{
    if (  m_coord_y == coord_y ) return;
    m_coord_y = coord_y;
    emit coord_yChanged();
}

void CustomPlotItem::setCoord_y(const double &coord_y)
{
    static double last=0;
    if (qFuzzyCompare(last+1,coord_y+1)) return;
    last=coord_y;
    setCoord_y(QString::number(coord_y, 'g',3));
}

QString CustomPlotItem::coord_x() const
{
    return m_coord_x;
}

void CustomPlotItem::setCoord_x(const QString &coord_x)
{
    if ( m_coord_x == coord_x ) return;
    m_coord_x = coord_x;
    emit coord_xChanged();
}

QString CustomPlotItem::graph_name() const
{
    return m_graph_name;
}

void CustomPlotItem::setGraph_name(const QString &graph_name)
{
    if (m_graph_name == graph_name) return;
    m_graph_name = graph_name;
    emit graph_nameChanged();
}

QString CustomPlotItem::name() const
{
    return m_name;
}

int CustomPlotItem::index_by_name(QString &name) //перебором по всем графикам
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

void CustomPlotItem::showCursor(QMouseEvent *event)
{
    if (_onshow) return;
    _onshow=true;
        double mouseX = m_CustomPlot->xAxis->pixelToCoord(event->pos().x());
        double startPos = m_CustomPlot->yAxis->pixelToCoord(0);
        double endPos = m_CustomPlot->yAxis->pixelToCoord(m_CustomPlot->size().height());
        cursor->start->setCoords(mouseX, startPos);
        cursor->end->setCoords(mouseX, endPos);
        int index = m_CustomPlot->graph(0)->findBegin(mouseX);
        double x = m_CustomPlot->graph(0)->dataMainKey(index);
        setCoord_x(QTime::fromMSecsSinceStartOfDay(x*1000).toString("hh:mm:ss:zzz"));

        for (auto gr:m_CustomPlot->selectedGraphs()) {
            qDebug()<<"curs"<<gr->name()<<"index:"<<index<<"maxindex:"<<gr->dataCount()<<mouseX;
            setCoord_y(gr->dataMainValue(index));
            qDebug()<<"::";
            setGraph_name(gr->name());
            break;
        }
        m_CustomPlot->replot();
        _onshow=false;
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
