#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QPoint>
#include <QFileDialog>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <string>

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnAbrir_clicked()
{
    //cargar el vidio
    QString nameFileInput = QFileDialog::getOpenFileName(this,       //path QString
         tr("Open Video"),
         DEFAULT_PATH,
         tr("Video Files (*.avi)"));

    if(nameFileInput == NULL) return;
    ui->lbPath->setText(nameFileInput);
    string pathFileInput = nameFileInput.toUtf8().constData();  //same path but std string
    VideoCapture video(pathFileInput);
    this->video = video;
}

void MainWindow::showImageOnce(cv::Mat theMat, std::string nombre)
{
    cv::namedWindow(nombre);                                    // create image window named "My Image"
    cv::imshow(nombre, theMat);                                 // show the image on window
}

QPoint MainWindow::getSSD(Mat originalMat, Mat cropMat, int offy, int offx)
{
    Mat3b oMat = (Mat3b) originalMat;
    Mat3b cMat = (Mat3b) cropMat;

    Vec3b ssd;
    ssd[0]=999; ssd[1] =999; ssd[2]=999;
    QPoint best(0,0);

    int max = originalMat.cols-(offy*2);
    ui->progressBar->setMaximum(max);
    int p=0;

    for (int i = offy; i < originalMat.cols-offy; ++i) {
        for (int j = offx; j < originalMat.rows-offx; ++j) {
            Vec3f ssd_temp;
            ssd_temp[0]=0; ssd_temp[1] =0; ssd_temp[2]=0;
            for (int x = 0; x < cMat.rows; ++x) {
                for (int y = 0; y < cMat.cols; ++y) {
                    Vec3f g = cMat(y,x);                                     //Cada pixel de la imagen cortada
                    Vec3f f_temp = oMat(j+y-offy,i+x-offx);
                    //Aplicar la formula
                    ssd_temp[0] += pow( (f_temp[0]- (g[0])) , 2 );         //hasta aqui bastaría para grayscale
                    ssd_temp[1] += pow( (f_temp[1]- (g[1])) , 2 );
                    ssd_temp[2] += pow( (f_temp[2]- (g[2])) , 2 );
                }
            }
            if (ssd[0]+ssd[1]+ssd[2] > ssd_temp[0]+ssd_temp[1]+ssd_temp[2]) {
                //qDebug()<<"|||||||||||||||| switch |||||||||||||||||||||||";
                ssd[0] = ssd_temp[0];
                ssd[1] = ssd_temp[1];
                ssd[2] = ssd_temp[2];
                best.setX(i); best.setY(j);
            }
            qDebug() <<"X"<<i-offx<<"Y"<<j-offy<<"| [" <<ssd[0] <<":"<<ssd[1]<<":"<<ssd[2]<<"]";
        }
        //qDebug()<< "------------------------------------\n";
        ui->progressBar->setValue(p++);
    }
    //qDebug() << "--------------"<<best<<"------------";
    ui->lbCoordenadas->setText("("+QString::number(best.x())+","+QString::number(best.y())+")");
    return best;
}

int MainWindow::getOffx(Mat m)
{
    int z = m.cols;
    if(z == z/2*2){
        qDebug() << "ERROR PERRO, LA IMAGEN ES PAR";
        return -1;
    }
    else
        return (z-1)/2;
}

int MainWindow::getOffy(Mat m)
{
    int z = m.rows;
    if(z == z/2*2){
        qDebug() << "ERROR PERRO, LA IMAGEN ES PAR";
        return -1;
    }
    else
        return (z-1)/2;
}

/* metodo desobediente */
Mat MainWindow::promImage(Mat a, Mat b, int myX, int myY, int t)
{
    qDebug() <<"myX"<<myX<<"myY"<<myY<<"t"<<t;
    for (int y = myX; y < myX + b.rows; y++) {
        for (int x = myY; x < myY + b.cols; x++) {
            //declarar los vectores a iterar
            cv::Vec3b vecA = a.at<cv::Vec3b>(cv::Point(x, y));
            cv::Vec3b vecB = b.at<cv::Vec3b>(cv::Point(x, y));

            //aplicar formula
            /*vecA[0] = (((t - 1) / t)*vecA[0] + (1 / t)*vecB[0]);
            vecA[1] = (((t - 1) / t)*vecA[1] + (1 / t)*vecB[1]);
            vecA[2] = (((t - 1) / t)*vecA[2] + (1 / t)*vecB[2]);*/

            vecA[2] = (vecA[2]+vecB[1])/2;
            vecA[1] = vecA[2];
            vecA[0] = 0;
            //a(x,y) = imgrow;

            a.at<cv::Vec3b>(cv::Point(x, y)) = vecA;
            qDebug() <<"x"<<x<<"y"<<y;
        }
    }
    return a;
}

void MainWindow::on_btnAlinear_clicked()
{
    int x = 450;
    int y = 185;
    int w = 179;
    int h = 179;

    int offx, offy;
    QPoint current;
    Mat base, crop;

    //current = QPoint(372, 178);
    current = QPoint(274, 539); //para probar

    video >> base;
    for (int frame = 1; frame < ui->slider->value(); ++frame) {

        video >> crop;
        crop = crop(cv::Rect(x,y,w,h));
        offx = getOffx(crop);
        offy = getOffy(crop);
        //current = getSSD(base,crop,offy, offx);
        base = promImage(base, crop, current.x()-offx, current.y()-offy, frame+1);

        ui->progressBar->setValue(0);
        showImageOnce(base, "base"+frame);
        showImageOnce(crop, "crop"+frame);
    }
    //showImageOnce(base, "Funciono");
}
