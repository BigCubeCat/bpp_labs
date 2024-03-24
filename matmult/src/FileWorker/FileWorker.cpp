#include <iostream>
#include <utility>
#include "./FileWorker.h"

FileWorker::FileWorker(std::string file) : filename(std::move(file)) {
    inputStream.open(filename);
    isOpen = inputStream.is_open();
}

TConfigStruct FileWorker::readData() {
    TConfigStruct configStruct{};
    if (!isOpen) { return configStruct; }
    std::string word;
    int sizes[3] = {0};
    for (int i = 0; i < 3; ++i) {
        inputStream >> word;
        sizes[i] = std::stoi(word);
    }
    configStruct.matrixA = new MatrixModel(sizes[0], sizes[1]);
    configStruct.matrixB = new MatrixModel(sizes[1], sizes[2]);

    readMat(configStruct.matrixA);
    readMat(configStruct.matrixB);

    return configStruct;
}

FileWorker::~FileWorker() {
    inputStream.close();
}

void FileWorker::readMat(MatrixModel *mat) {
    std::string word;
    for (size_t row = 0; row < mat->height; ++row) {
        for (size_t col = 0; col < mat->width; ++col) {
            inputStream >> word;
            mat->setValue(row, col, std::atof(word.c_str()));
        }
    }
}
