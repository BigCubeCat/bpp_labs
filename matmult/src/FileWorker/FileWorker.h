#ifndef FILEWORKER_H
#define FILEWORKER_H

#include "../MatrixModel/MatrixModel.h"

#include <string>
#include <fstream>

struct TConfigStruct {
    MatrixModel *matrixA;
    MatrixModel *matrixB;
};

class FileWorker {
private:
    std::ifstream inputStream;
    std::string filename;
    bool isOpen = false;
public:
    explicit FileWorker(std::string file);
    void readMat(MatrixModel *mat);
    TConfigStruct readData();
    ~FileWorker();
};

#endif
