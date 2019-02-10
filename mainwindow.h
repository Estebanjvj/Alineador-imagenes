#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QPoint>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnAbrir_clicked();

    void on_btnAlinear_clicked();

private:
    Ui::MainWindow *ui;

    const QString DEFAULT_PATH = "C:/Users/zaory/Documents/Los dataset/C1T1";
    cv::VideoCapture video;

    void showImageOnce(cv::Mat theMat, std::string nombre);
    QPoint getSSD(cv::Mat originalMat, cv::Mat cropMat, int offy, int offx);
    int getOffx(cv::Mat m);
    int getOffy(cv::Mat m);
    cv::Mat promImage(cv::Mat a, cv::Mat b, int myX, int myY, int t);
};

#endif // MAINWINDOW_H
