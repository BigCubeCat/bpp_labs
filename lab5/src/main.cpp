#include <iostream>
#include "Core.h"
#include "Config.h"

#include <pthread.h>
#include <stdio.h>

//переменная для сборки окончательного результата
int globalRes = 0;
//векторы, которые предстоит умножить
int v1[100], v2[100];
//
int ids[4] = {0, 1, 2, 3};
//четрыре объекта типа "описатель потока"
pthread_t thrs[4];
//мьютекс
pthread_mutex_t mutex;

//функция потока
void *prod(void *me) {
//узнали номер потока
    int offset = *((int *) me);
//вычислили смещение в векторе до "своей" четверти
    offset *= 25;
//вычисление частичного результата
    int res = 0;
    for (int i = offset; i < offset + 25; i++)
        res += v1[i] * v2[i];
//захват мьютекса
    pthread_mutex_lock(&mutex);
//добавление к глобальному результату при исключительном владении глобальной переменной
    globalRes += res;
//освобождение мьютекса
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char **argv) {

    Config conf = Config(argc, argv);
    std::cout << factorial(4) << std::endl;

    //тут где-то должна быть инициализация массивов

    //атрибуты потока
    pthread_attr_t attrs;

    //инициализация атрибутов потока
    if (0 != pthread_attr_init(&attrs)) {
        perror("Cannot initialize attributes");
        abort();
    };
    //установка атрибута "присоединенный"
    if (0 != pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE)) {
        perror("Error in setting attributes");
        abort();
    }
    //порождение четырех потоков
    for (int i = 0; i < 4; i++)
        if (0 != pthread_create(&thrs[i], &attrs, prod, &ids[i])) {
            perror("Cannot create a thread");
            abort();
        }
    //освобождение ресурсов, занимаемых описателем атрибутов
    pthread_attr_destroy(&attrs);
    //ожидание завершения порожденных потоков
    for (int i = 0; i < 4; i++)
        if (0 != pthread_join(thrs[i], NULL)) {
            perror("Cannot join a thread");
            abort();
        }

    return 0;
}
