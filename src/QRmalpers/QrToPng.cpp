//
// Created by remy on 02-06-20.
//

#include "QrToPng.h"

QrToPng::QrToPng(std::string fileName, int imgSize, int minModulePixelSize, std::string text,
                 bool overwriteExistingFile, qrcodegen::QrCode::Ecc ecc) :
        _fileName(std::move(fileName)), _size(imgSize), _minModulePixelSize(minModulePixelSize), _text(std::move(text)),
        _overwriteExistingFile(overwriteExistingFile), _ecc(ecc) {
}

bool QrToPng::writeToPNG(int bn) {
    /* text is required */
    if (_text.empty())
        return false;


    if (!_overwriteExistingFile && fs::exists(_fileName))
        return false;

    auto _qr = qrcodegen::QrCode::encodeText("", _ecc);
    try {
        _qr = qrcodegen::QrCode::encodeText(_text.c_str(), _ecc);
    }
    catch (const std::length_error &e) {
        std::cerr << "Failed to generate QR code, too much data. Decrease _ecc, enlarge size or give less text."
                  << std::endl;
        std::cerr << "e.what(): " << e.what() << std::endl;
        return false;
    }

    if (_overwriteExistingFile && fs::exists(_fileName))
        if (!fs::copy_file(_fileName, _fileName + ".tmp", fs::copy_options::overwrite_existing))
            return false;

    auto result = _writeToPNG(_qr, bn);

    if (result)
        fs::remove(_fileName + ".tmp");

    return result;

}

bool QrToPng::_writeToPNG(const qrcodegen::QrCode &qrData, int bn) const {
    std::ofstream out(_fileName.c_str(), std::ios::binary);
    int pngWH = _imgSizeWithBorder(qrData);
    TinyPngOut pngout(pngWH, pngWH, out);

    auto qrSize = qrData.getSize();
    auto qrSizeWithBorder = qrData.getSize() + 2;
    if (qrSizeWithBorder > _size)
        return false; // qrcode doesn't fit

    int qrSizeFitsInMaxImgSizeTimes = _size / qrSizeWithBorder;
    int pixelsWHPerModule = qrSizeFitsInMaxImgSizeTimes;

    if (qrSizeFitsInMaxImgSizeTimes < _minModulePixelSize)
        return false; // image would be to small to scan

    std::vector<uint8_t> tmpData;
    uint8_t blackPixelR = 0x1C;
    uint8_t blackPixelG = 0x25;
    uint8_t blackPixelB = 0x2C;
    uint8_t whitePixelR = 0x5B;
    uint8_t whitePixelG = 0xB1;
    uint8_t whitePixelB = 0xBC;
    if (bn == 1) {
        blackPixelR = 0x1C;
        blackPixelG = 0x25;
        blackPixelB = 0x2C;
        whitePixelR = 0xFF;
        whitePixelG = 0xFF;
        whitePixelB = 0xFF;
    }

    /* The below loop converts the qrData to RGB8.8.8 pixels and writes it with
     * the tinyPNGoutput library. since we probably have requested a larger
     * qr module pixel size we must transform the qrData modules to be larger
     * pixels (than just 1x1). */

    // border above
    for (int i = 0; i < pngWH; i++) // row
        for (int j = 0; j < pixelsWHPerModule; j++) // module pixel (height)
            tmpData.insert(tmpData.end(), { whitePixelR, whitePixelG, whitePixelB });

    pngout.write(tmpData.data(), static_cast<size_t>(tmpData.size() / 3));
    tmpData.clear();

    for (int qrModuleAtY = 0; qrModuleAtY < qrSize; qrModuleAtY++) {
        for (int col = 0; col < pixelsWHPerModule; col++) {
            // border left
            for (int i = 0; i < qrSizeFitsInMaxImgSizeTimes; ++i)
                tmpData.insert(tmpData.end(), { whitePixelR, whitePixelG, whitePixelB });

            // qr module to pixel
            for (int qrModuleAtX = 0; qrModuleAtX < (qrSize); qrModuleAtX++) {
            for (int row = 0; row < qrSizeFitsInMaxImgSizeTimes; ++row) {
                    if (qrData.getModule(qrModuleAtX, qrModuleAtY)) {
                        // insert saves us a for loop or 3 times the same line.
                        tmpData.insert(tmpData.end(), { blackPixelR, blackPixelG, blackPixelB });
                    } else {
                        tmpData.insert(tmpData.end(), { whitePixelR, whitePixelG, whitePixelB });
                    }
                }
            }
            // border right
            for (int i = 0; i < qrSizeFitsInMaxImgSizeTimes; ++i)
                tmpData.insert(tmpData.end(), { whitePixelR, whitePixelG, whitePixelB });

            // write the entire  row
            pngout.write(tmpData.data(), static_cast<size_t>(tmpData.size() / 3));
            tmpData.clear();
        }
    }

    // border below
    for (int i = 0; i < pngWH; i++) // row
        for (int j = 0; j < pixelsWHPerModule; j++) // module pixel (height)
            tmpData.insert(tmpData.end(), { whitePixelR, whitePixelG, whitePixelB });

    pngout.write(tmpData.data(), static_cast<size_t>(tmpData.size() / 3));
    tmpData.clear();

    return fs::exists(_fileName);
}


uint32_t QrToPng::_imgSize(const qrcodegen::QrCode &qrData) const {
    return (_size / qrData.getSize()) * qrData.getSize();
}

uint32_t QrToPng::_imgSizeWithBorder(const qrcodegen::QrCode &qrData) const {
    return (_size / (qrData.getSize() + 2)) * (qrData.getSize() + 2);
}
