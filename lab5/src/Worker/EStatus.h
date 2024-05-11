#ifndef LAB5_ESTATUS_H
#define LAB5_ESTATUS_H

enum EStatus{
    WORKER,
    END
};

enum EProcessStatus {
    UNKNOWN, // ничего не известно о количестве задач процесса
    NO_TASKS, // у процесса нет задач, чтобы отдать мне
    HAS_TASKS, // процесс может мне отдать хотя бы одну задачу
    NO_ANSWER, // процесс не ответил на запрос (упал или заверщен)
    ME // этот процесс - текущий
};

#endif
