#ifndef LAB5_ESTATUS_H
#define LAB5_ESTATUS_H

enum EStatus{
    RUNNING,
    END
};

enum EProcessStatus {
    UNKNOWN, // ничего не известно о количестве задач процесса
    NO_TASKS, // у процесса нет задач, чтобы отдать мне
    ME // этот процесс - текущий
};

#endif
