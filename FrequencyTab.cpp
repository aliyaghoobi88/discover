#include "FrequencyTab.h"


const QString FrequencyTab::checkBoxLabels[8] = {QString("Va"), QString("Vb"), QString("Vc"), QString("Vn"), QString("Ia"), QString("Ib"), QString("Ic"), QString("In")};

FrequencyTab::FrequencyTab(QWidget *parent) : TabViewWidget(parent)
{
    QVBoxLayout *verticalLayout = new QVBoxLayout(this);
    QHBoxLayout *checkBoxLayout = new QHBoxLayout();

    frequencyScene = new FrequencyScene();
    frequencyView = new QGraphicsView();
    frequencyView->setScene(frequencyScene);

    frequencyView->setRenderHint(QPainter::Antialiasing);    // TODO: move into subclass?


    checkBoxMapper = new QSignalMapper(this);

    for (int i = 0; i < 8; i++) {
        activeWaveformCheckBox[i] = new QCheckBox(checkBoxLabels[i]);
        activeWaveformCheckBox[i]->setChecked(frequencyScene->getWaveformState(i));
        connect(activeWaveformCheckBox[i], SIGNAL(toggled(bool)), checkBoxMapper, SLOT(map()));
        checkBoxMapper->setMapping(activeWaveformCheckBox[i], i);
        checkBoxLayout->addWidget(activeWaveformCheckBox[i]);
    }

    connect(checkBoxMapper, SIGNAL(mapped(int)), this, SIGNAL(checkBoxToggled(int)));
    connect(this, SIGNAL(checkBoxToggled(int)), this, SLOT(setActiveWavefrom(int)));


    verticalLayout->addLayout(checkBoxLayout);
    verticalLayout->addWidget(frequencyView, 0, 0);

    this->views.append(frequencyView);

    connect(this, SIGNAL(redrawFrequencyScene()), this->frequencyScene, SLOT(redrawFrequencyScene()));
}

void FrequencyTab::update()
{
    frequencyScene->streamTableModelSelectionChanged(this->model, &this->index);
}

void FrequencyTab::removeView()
{
}

void FrequencyTab::setActiveWavefrom(int id)
{
    if (id >= 0 && id < 8) {
        qDebug() << id << activeWaveformCheckBox[id]->isChecked();
        frequencyScene->setWaveformState(id, activeWaveformCheckBox[id]->isChecked());
        emit redrawFrequencyScene();
    }
}
