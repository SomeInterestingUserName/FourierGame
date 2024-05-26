#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QStringList>

#define MAX_VAL 3
#define NUM_SLIDERS_MAX 10
#define NUM_PTS 2000

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    congratulationMessages << "Nice!"
        << "Awesome!"
        << "Fantastic!"
        << "Excellent!"
        << "Terrific!"
        << "Great!"
        << "Spot-On!";

    difficultyNumSliders = {3, 5, 10, 10};
    difficultyMaxValue = {3, 5, 10, 100};

    sineWave = {1.0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    squareWave = {1.0, 0, 1/3.0, 0, 1/5.0, 0, 1/7.0, 0, 1/9.0, 0};
    triangleWave = {1.0, 0, -1/9.0, 0, 1/25.0, 0, -1/49.0, 0, 1/81, 0};
    sawtoothLWave = {1.0, -1/2.0, 1/3.0, -1/4.0, 1/5.0, -1/6.0, 1/7.0, -1/8.0, 1/9.0, -1/10.0};
    sawtoothRWave = {-1.0, 1/2.0, -1/3.0, 1/4.0, -1/5.0, 1/6.0, -1/7.0, 1/8.0, -1/9.0, 1/10.0};

    samples = {&sineWave, &squareWave, &triangleWave, &sawtoothLWave, &sawtoothRWave};


    sliders = QVector<QSlider *>(NUM_SLIDERS_MAX);
    for(int i=0; i<NUM_SLIDERS_MAX; i++){
        QSlider *slider = new QSlider(this);
        slider->setOrientation(Qt::Vertical);
        slider->setMaximum(MAX_VAL);
        slider->setMinimum(-MAX_VAL);
        slider->setValue(0);
        connect(slider, &QSlider::valueChanged, this, &MainWindow::onSliderMoved);

        QLabel *label = new QLabel(QString::number(i+1)+"x", this);
        label->setAlignment(Qt::AlignCenter);
        ui->sliderGrid->addWidget(slider, 0, i);
        ui->sliderGrid->addWidget(label, 1, i);
        slider->show();
        label->show();
        sliders[i] = slider;
    }
    setDifficulty(ui->difficultySelector->currentIndex());
    x = QVector<double>(NUM_PTS);
    y = QVector<double>(NUM_PTS);
    yr = QVector<double>(NUM_PTS);
    ref = QVector<int>(NUM_SLIDERS_MAX);

    ui->plot->xAxis->setRange(0, 6*M_PI);
    ui->plot->yAxis->setRange(-5,5);

    for(int i=0; i<x.length(); i++){
        x[i] = (6*M_PI)*i/double(x.length());
        y[i] = 0;
    }
    ui->plot->addGraph();
    ui->plot->graph(0)->setPen(QPen(QBrush(Qt::blue), 3));
    ui->plot->graph(0)->setData(x,y);

    ui->plot->addGraph();
    ui->plot->graph(1)->setPen(QPen(QBrush(Qt::red), 3));

    randomRef();

    congratulation = new QCPItemText(ui->plot);
    congratulation->setPositionAlignment(Qt::AlignTop | Qt::AlignLeft);
    congratulation->position->setType(QCPItemPosition::ptAxisRectRatio);
    congratulation->position->setCoords(0.1, 0);
    congratulation->setFont(QFont(font().family(), 36));
    congratulation->setColor(Qt::black);
    congratulation->setText("");

    ui->plot->graph(1)->setData(x,yr);
    ui->plot->graph(1)->setName("Reference Signal");
    ui->plot->graph(0)->setName("Your Signal");
    ui->plot->legend->setVisible(true);

    ui->plot->replot();
}

void MainWindow::randomRef(){
    QVector<int> random_ref(NUM_SLIDERS_MAX);
    for(int i=0; i<numSlidersUsed; i++){
        random_ref[i] = QRandomGenerator::global()->bounded(-numSliderTicks, numSliderTicks);
    }
    for(int i=numSlidersUsed; i<NUM_SLIDERS_MAX; i++){
        random_ref[i] = 0;
    }
    setRef(&random_ref);
}

void MainWindow::onSliderMoved(){
    for(int i=0; i<x.length(); i++){
        double y_temp = 0.0;
        for(int j=0; j<NUM_SLIDERS_MAX; j++){
            double slider_value = sliders[j]->value()/double(numSliderTicks);
            y_temp += slider_value*sin((j+1)*x[i]);
        }
        y[i] = y_temp;
    }

    int distFromRef = 0;
    for(int i=0; i<numSlidersUsed; i++){
        distFromRef += qAbs(sliders[i]->value() - ref[i]);
    }
    if(distFromRef == 0){
        congratulation->setText(congratulationMessages[QRandomGenerator::global()->bounded(0, congratulationMessages.size())]);
    }
    else if(distFromRef <= (numSlidersUsed*0.4)){
        congratulation->setText("Almost There...");
    }
    else if(distFromRef <= (numSlidersUsed)){
        congratulation->setText("Really Close...");
    }
    else{
        congratulation->setText("");
    }
    ui->plot->graph(0)->setData(x,y);
    ui->plot->replot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_resetButton_clicked()
{
    for(int i=0; i<NUM_SLIDERS_MAX; i++){
        sliders[i]->setValue(0);
    }
}


void MainWindow::on_hintButton_clicked()
{
    QString hintMessage = "";
    for(int i=0; i<numSlidersUsed; i++){
        if(sliders[i]->value() > ref[i]){
            hintMessage.append(QString("%1x: Too High!<br/>").arg(i+1));
        }
        else if(sliders[i]->value() < ref[i]){
            hintMessage.append(QString("%1x: Too Low!<br/>").arg(i+1));
        }
    }
    QMessageBox::information(this, "", hintMessage);
}

void MainWindow::on_newButton_clicked()
{
    randomRef();
    on_resetButton_clicked();
    ui->plot->replot();
}

void MainWindow::setDifficulty(int difficulty){
    numSlidersUsed = difficultyNumSliders[difficulty];
    numSliderTicks = difficultyMaxValue[difficulty];
    for(int i=0; i<NUM_SLIDERS_MAX; i++){
        sliders[i]->setMaximum(numSliderTicks);
        sliders[i]->setMinimum(-numSliderTicks);
    }
    for(int i=0; i<numSlidersUsed; i++){
        sliders[i]->setEnabled(true);
    }
    for(int i=numSlidersUsed; i<NUM_SLIDERS_MAX; i++){
        sliders[i]->setEnabled(false);
    }
}

void MainWindow::on_difficultySelector_currentIndexChanged(int index)
{
    setDifficulty(index);
    on_newButton_clicked();
}

void MainWindow::sampleRef(QVector<double> *s){
    int len = s->length();
    QVector<int> s_int(len);
    for(int i=0; i<len; i++){
        s_int[i] = qRound((*s)[i]*numSliderTicks);
    }
    setRef(&s_int);
}

void MainWindow::setRef(QVector<int> * s){
    for(int i=0; i<numSlidersUsed; i++){
        ref[i] = (*s)[i];
    }
    for(int i=numSlidersUsed; i<NUM_SLIDERS_MAX; i++){
        ref[i] = 0;
    }

    for(int i=0; i<x.length(); i++){
        double y_temp = 0.0;
        for(int j=0; j<NUM_SLIDERS_MAX; j++){
            double ref_value = ref[j]/double(numSliderTicks);
            y_temp += ref_value*sin((j+1)*x[i]);
        }
        yr[i] = y_temp;
    }
    ui->plot->graph(1)->setData(x,yr);
}

void MainWindow::on_comboBox_activated(int index)
{
    sampleRef(samples[index]);
    ui->plot->replot();
}

