#ifndef LAB5_RANKDATA_H
#define LAB5_RANKDATA_H


struct RankData {
    int rank, workload;
    bool operator<(const RankData &other);
};


#endif //LAB5_RANKDATA_H
