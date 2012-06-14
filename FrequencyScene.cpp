#include "FrequencyScene.h"

#define VOLTAGE_LINE_ALPHA  210
#define CURRENT_LINE_ALPHA  150
#define MIN_Y_VALUE         2       // -log10(0.01)

const QColor FrequencyScene::waveformColors[8] = {QColor(180, 33, 38, VOLTAGE_LINE_ALPHA),
                                                  QColor(240, 181, 0, VOLTAGE_LINE_ALPHA),
                                                  QColor(36, 78, 198, VOLTAGE_LINE_ALPHA),
                                                  QColor(180, 180, 180, VOLTAGE_LINE_ALPHA),
                                                  QColor(180, 33, 38, CURRENT_LINE_ALPHA),
                                                  QColor(240, 181, 0, CURRENT_LINE_ALPHA),
                                                  QColor(36, 78, 198, CURRENT_LINE_ALPHA),
                                                  QColor(180, 180, 180, CURRENT_LINE_ALPHA)};

FrequencyScene::FrequencyScene(QObject *parent) : QGraphicsScene(parent)
{
    // enable only the first waveform by default
    for (int i = 0; i < 8; i++) {
        if (i == 0) {
            activeWaveform[i] = true;
        }
        else {
            activeWaveform[i] = false;
        }

        pen[i] = QPen(waveformColors[i]);
        path[i] = QPainterPath();
        pathItem[i] = QGraphicsScene::addPath(path[i], pen[i]);
        pathItem[i]->hide();
    }

    QColor plotLineColor = QColor(180, 180, 180);

    QPen plotLinePen = QPen(plotLineColor);
    plotLinePen.setCosmetic(true);
    //plotLinePen.setWidth(2);
    plotLinePen.setCapStyle(Qt::RoundCap);

    horizontalPlotLine = QGraphicsScene::addLine(0.0, 0.0, 0.0, 0.0, plotLinePen);
    verticalPlotLine = QGraphicsScene::addLine(0.0, 0.0, 0.0, 0.0, plotLinePen);

    connect(this, SIGNAL(callRedrawFrequencyScene()), this, SLOT(redrawFrequencyScene()));
}

void FrequencyScene::streamTableModelSelectionChanged(StreamTableModel *streamTableModel, QPersistentModelIndex *index)
{
    this->streamTableModel = streamTableModel;
    this->index = index;

    //qDebug() << "in FrequencyScene::streamTableModelSelectionChanged()" << this->index->row() << this->streamTableModel;

    emit callRedrawFrequencyScene();
}

void FrequencyScene::streamRemoved()
{
    // TODO
}

void FrequencyScene::redrawFrequencyScene()
{
    draw();
}

void FrequencyScene::draw() {
    qDebug() << "in FrequencyScene::draw()";
    StreamTableRow *stream;

    if (this->streamTableModel != NULL && this->index != NULL && this->index->isValid()) {
        stream = this->streamTableModel->getRowFromIndex(this->index);
    }
    else {
        return;
    }

    if (stream == NULL) {
        //qDebug() << "stream is null";
        return;
    }
    //qDebug() << "stream is not null";

    for (int i = 0; i < 8; i++) {
        if (this->getWaveformState(i) == true) {

            bool pathEmpty = path[i].isEmpty();

            for (quint32 p = 0; p < stream->getNumberOfFreqPoints(i); p++) {
                qreal x = (stream->getFreqPoint(i, p).x());
                qreal y = (stream->getFreqPoint(i, p).y());

                if (y > MIN_Y_VALUE) {
                    y = MIN_Y_VALUE;
                }

                if (pathEmpty) {
                    if (p == 0) {
                        path[i].moveTo(x, y);
                    }
                    else {
                        path[i].lineTo(x, y);
                    }
                }
                else {
                    path[i].setElementPositionAt(p, x, y);
                }
            }

//            if (pathItem[i] == NULL) {
//                qDebug() << "pathItem[i] is null";
//                pathItem[i] = QGraphicsScene::addPath(path[i], pen[i]);
//            }
//            else {
//                pathItem[i]->setPath(path[i]);
//            }
            pathItem[i]->setPath(path[i]);
            if (!pathItem[i]->isVisible()) {
                pathItem[i]->show();
            }
        }
        else {
            pathItem[i]->hide();
        }
    }

    qDebug() << stream->getFreqPoint(0, stream->getNumberOfFreqPoints(0) - 1).x();

    qreal maxFreqValue = (stream->getFreqPoint(0, stream->getNumberOfFreqPoints(0) - 1).x());
    qreal maxMagnitudeValue = -(3.0/*stream->getFreqPoint(0, stream->getNumberOfFreqPoints(0) - 1).y()*/);

    horizontalPlotLine->setLine(0, -log10(0.01), maxFreqValue, -log10(0.01));
    verticalPlotLine->setLine(0, -log10(0.01), 0, maxMagnitudeValue);

    QGraphicsView *view = this->views().first();
    //qDebug() << this->views().size() << this->views().first();
    view->fitInView(this->sceneRect(), Qt::IgnoreAspectRatio);
}
