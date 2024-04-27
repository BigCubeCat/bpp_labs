# Параллельная реализация метода Якоби в трехмерной области

## Сборка
```bash
cmake CMakeLists.txt -B build 
cd build
make
```

## Подготовка к запуску
1. Отредактировать исходную функцию в `main.cpp`:
```c++
...

int main() {
    // funciton to edit
    auto srcFunction = [](double x, double y, double z, double a) {
        return 6 - a * vectorSize(x, y, z);
    };
...
```
2. [Собрать](#сборка)
3. Поставить парметры в окружение среды:
`(пример .env файла)`
```bash
x0=0
y0=0
z0=0

Dx=2
Dy=2
Dz=2

Nx=10
Ny=10
Nz=10

a=0.0000001

epsilon=0.00001
```

4. [Запустить](#запуск)

## Запуск
см пункт [сборка](#сборка)
**Простой заупуск**
```bash
mpirun -np 4 ./build/Jacobi
```
**Запуск на кластере для выявления зависимости от числа процессов**
```bash
python3 test/run.py test/config/count_process.json 
```

**Включить профилирование**
В `main.cpp` раскоментировать строку 
```cpp
#define PROFILE
```

## Полный текст зания
[[task.pdf]]
