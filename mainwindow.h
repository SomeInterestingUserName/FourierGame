#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSliderMoved();

    void on_resetButton_clicked();

    void on_hintButton_clicked();

    void on_newButton_clicked();

    void on_difficultySelector_currentIndexChanged(int index);

    void on_comboBox_activated(int index);

private:
    Ui::MainWindow *ui;
    QVector<double> x,y,yr;
    QVector<QSlider *> sliders;
    QVector<int> ref;
    QStringList congratulationMessages;
    QCPItemText *congratulation;
    QVector<int> difficultyNumSliders;
    QVector<int> difficultyMaxValue;
    void randomRef();
    void setDifficulty(int slidersUsed);
    void sampleRef(QVector<double> * s);
    void setRef(QVector<int> * s);
    int numSlidersUsed;
    int numSliderTicks;
    QVector<double> squareWave, sineWave, triangleWave, sawtoothLWave, sawtoothRWave;
    QVector<QVector<double>*> samples;
};
#endif // MAINWINDOW_H
