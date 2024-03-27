#ifndef FILEWORKER_H
#define FILEWORKER_H

#include "../MatrixModel/MatrixModel.h"
#include "../ConfigStruct/TConfigStruct.h"

#include <string>
#include <fstream>

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
