# Лабораторная №2 по Операционным Системам

Выполнил: Эйдельман Виктор Арадьевич
Вариант: Linux LRU
Преподаватель: Рогачёв Михаил Сергеевич


## Задание:

Для оптимизации работы с блочными устройствами в ОС существует кэш страниц с данными, которыми мы производим операции чтения и записи на диск. Такой кэш позволяет избежать высоких задержек при повторном доступе к данным, так как операция будет выполнена с данными в RAM, а не на диске (вспомним пирамиду памяти).

В данной лабораторной работе необходимо реализовать блочный кэш в пространстве пользователя в виде динамической библиотеки (dll или so). Политику вытеснения страниц и другие элементы задания необходимо получить у преподавателя.

При выполнении работы необходимо реализовать простой API для работы с файлами, предоставляющий пользователю следующие возможности:

Открытие файла по заданному пути файла, доступного для чтения. Процедура возвращает некоторый хэндл на файл. Пример:
int lab2_open(const char *path).
Закрытие файла по хэндлу. Пример:
int lab2_close(int fd).
Чтение данных из файла. Пример:
ssize_t lab2_read(int fd, void buf[.count], size_t count).
Запись данных в файл. Пример:
ssize_t lab2_write(int fd, const void buf[.count], size_t count).
Перестановка позиции указателя на данные файла. Достаточно поддержать только абсолютные координаты. Пример:
​​​​​​​off_t lab2_lseek(int fd, off_t offset, int whence).
Синхронизация данных из кэша с диском. Пример:
int lab2_fsync(int fd).

Операции с диском разработанного блочного кеша должны производиться в обход page cache используемой ОС.

В рамках проверки работоспособности разработанного блочного кэша необходимо адаптировать указанную преподавателем программу-загрузчик из ЛР 1, добавив использование кэша. Запустите программу и убедитесь, что она корректно работает. Сравните производительность до и после.

## Обзор кода:

Для реализации кэша, создавался общий кэш и его блоки
```cpp

typedef struct CacheBlock {
    int fd;
    int number;
    int page_number;
    time_t data;
    char* page;
    struct CacheBlock *prev; // Указатель на предыдущий блок (для LRU)
    struct CacheBlock *next; // Указатель на следующий блок (для LRU)
} CacheBlock;

typedef struct {
    CacheBlock *cache_head; // Голова списка кэша
    CacheBlock *cache_tail; // Хвост списка кэша
    int cache_size;      // Количество блоков в кэше
} Cache;
```
Используя них выполнялилсь функции:
1)lab2_open - открывает файл и создаёт блок с ним в кэше.
2)lab2_read - создаёт или читает страницы файла добавляя их кэш, затем читает в буффер нужные элементы.
3)lab2_lseek - указывает место в файле.
4)lab2_write - заменяет значения файла локально в нашем кэше.
5)lab2_fsync - сохраняют значения в файл.
6)lab2_close - закрывает файли и удаляет все соответвующие ему блоки кэша.

## Сравнение работы нагрузчика с и без кэша

Без кэша:
![image](https://github.com/user-attachments/assets/5a47a0fe-4817-4eb8-b393-04a51ba8b48a)

С кэшем:
![image](https://github.com/user-attachments/assets/af413443-d687-4236-9f13-4424fc1fdbbf)

Как видно скорость с кэшем уменьшилась в 30 раз.


## Вывод:

В результате выполнения лабораторной работы, я познакомился с методами работы кэша.
