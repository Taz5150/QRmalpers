//
// Created by remy on 07-06-20.
//
#include "QrToPng.h"

int main() {

    std::string qrText = "https://www.google.es/maps/@40.4483707,-3.6195874,20z";
    std::string fileName = "malpers.png";

    int imgSize = 140;
    int minModulePixelSize = 3;
    auto exampleQrPng1 = QrToPng(fileName, imgSize, minModulePixelSize, qrText, true, qrcodegen::QrCode::Ecc::MEDIUM);

    std::cout << "Writing Example QR code 1 (normal) to " << fileName << " with text: '" << qrText << "', size: " <<
              imgSize << "x" << imgSize << ", qr module pixel size: " << minModulePixelSize << ". " << std::endl;
    if (exampleQrPng1.writeToPNG())
        std::cout << "Success!" << std::endl;
    else
        std::cerr << "Failure..." << std::endl;

    return 0;
}