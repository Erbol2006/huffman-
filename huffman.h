#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h> 
#include <stdlib.h>  //для работы с памятью
#include <string.h> 
//СТРУКТУРА ДЛЯ ХРАНЕНИЯ ЧАСТОТ СИМВОЛОВ
//используется на первом этапе алгоритма для подсчета частот
typedef struct {
    unsigned char symbol;//сам символ
    int frequency;// сколько раз этот символ встречаеться в тексте
}SymbolFreq;

//СТРУКТРУ УЗЛА ДЕРЕВА ХАФФМАНА
//Структура стhоится так, что символы с большей частотой находяться ближе к корню
typedef struct HuffmanNode{
    unsigned char symbol;//символ(для листьев дерева)
    int frequency;//частота символа или сумма частот дочерних узлов
    struct HuffmanNode *left; //левый потомок(код 0)
    struct HuffmanNode *right;//правый потомок(код 1)
} HuffmanNode;

//СТРУКТУРА ДЛЯ ХРАНЕНИЯ КОДА ХАФФМАНА
//Каждому символу присваивается уникальный бинарный код
typedef struct {
    unsigned int bits; //сам код в виде битов
    int length; //длина кода в битаъ
} HuffmanCode;

//ОСНОВНЫЕ ФУНКИИ АЛГОР-МА ХАФФМАНА
//compress_to_huffman - сжимает текст и сохраняет 
//decompress_from_huffman - распаковывает текст из сжатого файла в расширении(.bin)
void compress_to_huffman(const char *text, const char *output_file);
char *decompress_from_huffman(const char *input_file);

//ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ 

//подсчет частот символов
void calculate_frequencies(const char *text, SymbolFreq *freqs, int *count);
void build_codes(HuffmanNode *node, HuffmanCode table [256], unsigned int code, int length); //построение кодов
void free_tree(HuffmanNode *node); //освобождение памяти дерева

//ФУНКЦИЯ ДЛЯ ВИЗУАЛИЗАЦИИ И АНАЛИЗА
void print_huffman_tree(HuffmanNode *node, int level);//вывод структуры дерева
void print_symbol_codes(HuffmanCode codes[256], SymbolFreq *freqs, int count); //вывод таблицы кодов 
void print_compression_stats(const char *input_file, const char *output_file); //вывод статистики сжатия 
#endif 
