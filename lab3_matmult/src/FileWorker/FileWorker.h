#ifndef FILEWORKER_H
#define FILEWORKER_H

#include "../MatrixModel/MatrixModel.h"
#include "../ConfigStruct/TConfigStruct.h"

#include <string>
#include <fstream>

class FileWorker {
private:
    std::fstream stream;
    std::string filename;
    bool isOpen = false;
    bool isInput = true;
public:
    explicit FileWorker(std::string file, bool input);
    void readMat(MatrixModel *mat);
    void writeMat(const MatrixModel &mat);
    TConfigStruct readData();
    ~FileWorker();
};

#endif
