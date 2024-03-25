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

    configStruct.n = static_cast<int>(configStruct.matrixA->countRows);
    configStruct.m = static_cast<int>(configStruct.matrixA->countColumns);
    configStruct.k = static_cast<int>(configStruct.matrixB->countColumns);

    return configStruct;
}

FileWorker::~FileWorker() {
    inputStream.close();
}

void FileWorker::readMat(MatrixModel *mat) {
    std::string word;
    for (size_t row = 0; row < mat->countRows; ++row) {
        for (size_t col = 0; col < mat->countColumns; ++col) {
            inputStream >> word;
            mat->setValue(row, col, std::atof(word.c_str()));
        }
    }
}
