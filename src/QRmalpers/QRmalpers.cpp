#include "QRmalpers.h"
#include "common.h"
#include <iostream>
#include "QrToPng.h"
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

void mergeQR() {
    // Load images
    Mat3b img1 = imread("./res/malpers_raw.png");
    Mat3b img2 = imread("malpers_qr_int.png");
    Mat3b res_img2;
    Mat input_bgra;

    // Resize QR
    resize(img2, res_img2, Size(195, 195));

    // Create a black image
    Mat3b res(img1.rows, img1.cols, Vec3b(0, 0, 0));

    // Copy images in correct position
    img1.copyTo(res(Rect(0, 0, img1.cols, img1.rows)));
    res_img2.copyTo(res(Rect(339, 575, res_img2.cols, res_img2.rows)));

    cvtColor(res, input_bgra, COLOR_RGB2RGBA);

    // find all white pixel and set alpha value to zero:
    for (int y = 0; y < input_bgra.rows; ++y)
        for (int x = 0; x < input_bgra.cols; ++x)
        {
            cv::Vec4b& pixel = input_bgra.at<cv::Vec4b>(y, x);
            // if pixel is white
            if (pixel[0] == 255 && pixel[1] == 255 && pixel[2] == 255)
            {
                // set alpha to zero:
                pixel[3] = 0;
            }
        }

    // Show result
    imwrite("malpers_icono.png", input_bgra);
    //imshow("Resultado", input_bgra);
    waitKey();
}

QRmalpers::QRmalpers(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.verifyButton, SIGNAL(clicked()), this, SLOT(verifyCoordinates()));
    connect(ui.createButton, SIGNAL(clicked()), this, SLOT(createCoordinates()));
    connect(ui.showButton, SIGNAL(clicked()), this, SLOT(showIcon()));
}

void QRmalpers::verifyCoordinates() {
    // Checking correct coordinates and conversion DMS -> Decimal
    QString latitudeQ = ui.latitudeEdit->toPlainText();
    QString longitudeQ = ui.longitudeEdit->toPlainText();
    QByteArray baLat = latitudeQ.toLatin1();
    QByteArray baLon = longitudeQ.toLatin1();
    char* latitudeStr = baLat.data();
    char* longitudeStr = baLon.data();

    ui.showButton->setEnabled(false);

    double latitude = Str2LatLong(latitudeStr);
    double longitude = Str2LatLong(longitudeStr);

    if (abs(latitude) <= 90 && abs(longitude) <= 180 && latitude != 0) {
        latitudeQ = QString::number(latitude);
        longitudeQ = QString::number(longitude);
        QString url = "https://www.google.com/maps/@" + latitudeQ + "," + longitudeQ +",17z";
        this->statusBar()->setStyleSheet("color: green");
        this->statusBar()->showMessage(url);

        ui.createButton->setEnabled(true);
    }
    else {
        ui.createButton->setEnabled(false);
        this->statusBar()->setStyleSheet("color: red");
        this->statusBar()->showMessage("Error en las coordenadas!");
    }
}

void QRmalpers::createCoordinates() {
    QString url = this->statusBar()->currentMessage();
    std::string fileName_qr = "malpers_qr.png";
    std::string fileName_int = "malpers_qr_int.png";
    bool writeInt = false;
    bool writeBnW = false;

    // Check if directory exists and clean-up
    remove("malpers_qr.png");
    remove("malpers_qr_int.png");
    remove("malpers_icono.png");

    // Check if QR is integrated and/ or if B/W QR is generated
    bool integrate = ui.intBox->isChecked();
    bool bnwQR = ui.bnwBox->isChecked();

    //**************************
    // Integrated QR Logic
    //**************************
    if (integrate) {
        // Create Icon with QR
        int imgSize = 215;
        int minModulePixelSize = 1;
        auto QrPng = QrToPng(fileName_int, imgSize, minModulePixelSize, url.toStdString(), true, qrcodegen::QrCode::Ecc::HIGH);

        if (QrPng.writeToPNG(0)) {
            QPixmap QRpix;
            writeInt = true;

            if (QRpix.load("malpers_qr_int.png")) {
                ui.QRpng->setPixmap(QRpix);
            }

            mergeQR();
        }
    }

    //**************************
    // QR B&W Individual
    //**************************
    if (bnwQR) {
        // Create Icon with QR
        int imgSize = 215;
        int minModulePixelSize = 1;
        auto QrPng = QrToPng(fileName_qr, imgSize, minModulePixelSize, url.toStdString(), true, qrcodegen::QrCode::Ecc::HIGH);

        if (QrPng.writeToPNG(1)) {
            QPixmap QRpix;
            writeBnW = true;

            if (QRpix.load("malpers_qr.png")) {
                ui.QRpng->setPixmap(QRpix);
            }
        }
    }

    //****************************************
    // Status Bar Messages & Button Logic
    //****************************************
    if (!integrate && !bnwQR) {
        this->statusBar()->setStyleSheet("color: red");
        this->statusBar()->showMessage("Selecciona al menos una opcion a generar.");
    }
    else if (integrate && !bnwQR) {
        if (writeInt) {
            this->statusBar()->setStyleSheet("color: green");
            this->statusBar()->showMessage("QR generado e incrustado en MALPERS. Verificar que la localizacion es correcta");
            ui.createButton->setEnabled(false);
            ui.showButton->setEnabled(true);
            remove("malpers_qr_int.png");
        }
        else {
            this->statusBar()->setStyleSheet("color: red");
            this->statusBar()->showMessage("Error al crear el codigo QR.");
            ui.createButton->setEnabled(false);
            ui.showButton->setEnabled(false);
        }
    }
    else if (!integrate && bnwQR) {
        if (writeBnW) {
            this->statusBar()->setStyleSheet("color: green");
            this->statusBar()->showMessage("QR individual generado. Verificar que la localizacion es correcta");
            ui.createButton->setEnabled(false);
            ui.showButton->setEnabled(false);
        }
        else {
            this->statusBar()->setStyleSheet("color: red");
            this->statusBar()->showMessage("Error al crear el codigo QR.");
            ui.createButton->setEnabled(false);
            ui.showButton->setEnabled(false);
        }
    }
    else {
        if (writeInt && writeBnW) {
            this->statusBar()->setStyleSheet("color: green");
            this->statusBar()->showMessage("QRs generados. Verificar que la localizacion es correcta");
            ui.createButton->setEnabled(false);
            ui.showButton->setEnabled(true);
            remove("malpers_qr_int.png");
        }
        else {
            this->statusBar()->setStyleSheet("color: red");
            this->statusBar()->showMessage("Error al crear el codigo QR.");
            ui.createButton->setEnabled(false);
            ui.showButton->setEnabled(false);
        }
    }
}

void QRmalpers::showIcon() {
    Mat3b img1 = imread("malpers_icono.png");
    imshow("QR Icono", img1);
    waitKey(0);
    destroyAllWindows();
}