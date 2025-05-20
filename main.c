// В main мы реализуем так скажем интерфейс в консоле
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "huffman.h"

//функция для чтения текстового файла 
//возвращает указатель на строку с содержимым файла 
char* read_text_file (const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Ошибка -> не удалось открыть файл %s для чтения\n", filename);
        return NULL;
    }
    //определяем размер файла при помощи комбинации функций fseek() и fteil()
    fseek(file, 0, SEEK_END); //перемещаем указатель позиции в конец файла
    long size = ftell(file); //получаем текущую позицию указателя(которая равна размеру файла в байтах)
    fseek(file, 0, SEEK_SET);//ну и возврощаем указатель позиции обратно в начало файла

    //выделяем память и читаем файл 
    char* content = malloc(size + 1);
    if (!content){
        fclose(file);
        return NULL;
    }
    fread (content, 1, size, file); //читает данные из файла в выделенную память
    content[size] = '\0'; //устанавливаем нуль-терминатор в конец данных, чтобы превратить в массив символов
    fclose(file);
    return content; //возврощает указатель на выделенную память
}

//функция для вывода справки по использованию программы 
void print_usage(const char *program_name){
    printf ("Использование:\n");
    printf("  %s --compress <входной файл> <выходной файл> - сжать файл\n", program_name);
    printf("  %s --decompress <входной файл> <выходной файл> - распаковать файл\n", program_name);
}

int main(int argc, char *argv[]){
    //для начала проверим кол-во аргументов командной строки 
    if (argc != 4){
        print_usage(argv[0]);
        return 1;
    }
    //получаем аргументы командной строки 
    const char *mode = argv[1]; //режим работы(сжатие/распаковка)
    const char *input_file = argv[2];//входной файл
    const char *output_file = argv[3]; // выходной файл 

    if (strcmp(mode, "--compress") == 0){
        //то есть если у нас режим сжатия 
        //для начала читаем используя функцию, которая выше 1) входной файл
        char *text = read_text_file(input_file);
        if (!text){
            return 1;
        }
        printf("Исходный текст:\n%s\n", text);
        //дальше уже сжимаем текст и сохраняем в выходной файл
        compress_to_huffman(text, output_file);
        //свобода для памяти
        free(text);
        printf("Сжатие завершено. Результат сохранён в %s\n", output_file);
        return 0;
    } else if (strcmp(mode, "--decompress") == 0){
        //то у нас режим распаковки 
        char *decompressed = decompress_from_huffman(input_file);
        if (!decompressed){
            return 1; 
        }
        //сохраняем распакованный текст в выходной файл
        FILE *f = fopen(output_file, "w");
        if (!f){
            printf("Ошибка -> не удалось открыть файл %s для записи\n", output_file);
            free(decompressed);
            return 1;
        }
        fprintf(f, "%s", decompressed);
        fclose(f);
        free(decompressed);
        printf("Распаковка завершена. Результат сохранён в %s\n", output_file);
        return 0;
    }
    //неизвестный режим
    printf("Неизвестный режим: %s\n", mode);
    print_usage(argv[0]);
    return 1;
}