#include "huffman.h"//наш заголовочный файл
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <limits.h> //для INT_MAX
#include <locale.h> //для setlocale

//максимальное количество узлов в дереве Хаффмана
//для 256 символов (1 байт) нужно 2*256 - 1 узлов
#define MAX_TREE_NODES 511  // 2*256 - 1 для 256 символов

//глобальная переменные для работы с очередью узлов 
int node_count = 0; //текущее кол-во узлов в очереди 
HuffmanNode* nodes[MAX_TREE_NODES]; //массив узлов для построения дерева 

//Функция для обмена узлов в очереди 
//Алгоритм Хаффмана использует min-heap для эффективного выбора
// двух узлов с наименьшей частотой.
void swap_nodes (int i, int j){
    HuffmanNode* temp = nodes[i];//сохраняем первый узел во временной переменной 
    nodes[i] = nodes[j]; //заменяем первый узел вторым
    nodes[j] = temp; //заменяем второй узел сохраненным первым
}

//ФУНКЦИЯ ДЛЯ ПОДНЯТИЯ УЗЛА ВВЕРХ ПО КУЧЕ
//ИСПОЛЬЗУЕТСЯ ПРИ ДОБАВЛЕНИИ НОВОГО УЗЛА
//то есть, это пирамида, где каждый родитель меньше своих детей (min-heap).
// Если новый элемент нарушает порядок (например, он меньше родителя), мы 
//"поднимаем" его вверх, пока он не займёт правильное место.
void sift_up(int index){
    while (index > 0){ //пока не дошли до корня 
        int parent = (index - 1) / 2; // находим индекс родителя
        // если родитель родитель меньше или равен текущему узлу
        if (nodes[parent]->frequency <= nodes[index]->frequency){
            //то, прекращаем подъем
            break;
        }
        swap_nodes(parent, index);//меняем местами с родителем
        index = parent; //перехлдим к родителю
    }
}

//ФУНКЦИЯ ДЛЯ ОПУСКАНИЯ УЗЛА ВНИЗ ПО КУЧЕ
//ИСПОЛЬЗУЕТСЯ ПРИ ИЗВЛЕЧЕНИИ МИНИМАЛЬНОГО УЗЛА
/*
Восстанавливает структуру min-heap после удаления корневого элемента 
(с минимальной частотой). Перемещает узел вниз, пока он не займет 
правильное положение
*/
void sift_down(int index){
    while(1){//бесконечный цикл, пока не найдем правильное место 
        int min_idx = index; //предпологаем, что текущий узел минимальный
        int left = 2 * index + 1; //индекс левого потока
        int right = 2 * index + 2; //индекс правого потока
        //находим узел с минимальной частотой среди текущего и его потомков
        if (left < node_count && nodes[left]->frequency < nodes[min_idx]->frequency){
            min_idx = right; //правый потомок меньше текущего узла
        }
        if (min_idx == index) {//если текущий узел уже минимальный 
            break; //то прекращаем опускание
        }
        swap_nodes(index, min_idx);//меняем местами с минимальным потомком
        index = min_idx; // переходим к минимальному потомку
    }
}

//ФУНКЦИЯ ДЛЯ ПОДСЧЕТА ЧАСТОТ СИМВОЛОВ В ТЕКСТЕ
//т.е создает таблицу частот встречаемости каждого символа в тексте
void calculate_frequencies(const char *text, SymbolFreq *freqs, int *count){
    while (*text){//пока не конец строки
        unsigned char symbol = *text;//получаем текущий символ
        int found = 0; // флаг, найден ли символ в массиве
        //ищем символ в массиве freqs
        for (int i = 0; i < *count; ++i){
            if (freqs[i].symbol == symbol) { //если символ найден
                freqs[i].frequency++; //увеличиваем его частоту 
                found = 1; //устанавливаем флаг
                break; //прекращаем поиск
            }
        }
        //если символ не найден, добавляем новый
        if (!found){
            freqs[*count].symbol = symbol;//сохраняем символ
            freqs[*count].frequency = 1;//устанавливаем частоту в 1
            (*count)++;//увеличиваем счетчик уникальных символов 
        }
        text++; //переходим к следующему символу
    }
}

//ФУНКЦИЯ ДЛЯ ДОБАВЛЕНИЯ УЗЛА В ОЧЕРЕДЬ
//Проверяет переполнение
//Добавляет узел в конец массива
//Вызывает sift_up для восстановления структуры кучи
void insert_node(HuffmanNode *node){
    if (node_count >= MAX_TREE_NODES) { //проверяем переполнение 
        fprintf(stderr, "ошибка -> превышен максимальный размер очереди\n");
        exit(1);
    }
    nodes[node_count] = node; //добавляем узел в конец очереди
    sift_up(node_count);//поднимаем узел на нужную позицию
    node_count++;//увеличиваем счетчик узлов
}
//LOX

//ФУНКЦИЯ ДЛЯ ИЗВЛЕЧЕНИЯ УЗОА С МИНИМАЛЬНОЙ ЧАСТОТОЙ
HuffmanNode* remove_min(){
    if (node_count == 0) {//если очередь пуста
        return NULL;
    }
    HuffmanNode* min = nodes[0];//сохраняем минимальный узел
    nodes[0] = nodes[--node_count]; //заменяем корень последним узлом
    if (node_count > 0){ //если в очереди остались узлы
        sift_down(0); //опускаем новый корень на нужнюю позицию
    }
    return min; //возвращаем минимальеный узел
}

//ФУНКЦИЯ ДЛЯ ПОСТРОЕНИЯ КОДОВ ХАФФМАНА
//рекурсивно обходит дерево и строит коды для каждого символа
void build_codes(HuffmanNode *node, HuffmanCode table[256], unsigned int code, int length){
    if (!node->left && !node->right){ //если это лист дерева
        table[node->symbol].bits = code; //сохранием код
        table[node->symbol].length = length; //сохарянем длину кода
        return;
    }
    //рекурсивно строим коды для левого (0) и правого (1) поддеревьев
    if (node->left){
        build_codes(node->left, table, (code << 1), length + 1); // добавляем 0
    } 
    if (node->right){
        build_codes(node->right, table, (code << 1) | 1, length + 1); // добавляем 1
    }
}

//ФУНКЦИЯ ДЛЯ ОСВОБОЖДЕНИЯ ПАМЯТИ ДЕРЕВА
void free_tree(HuffmanNode *node){
    if (node){//если узел существует 
        free_tree(node->left); //освобождаем левое поддерево 
        free_tree(node->right); //освобождаем правое поддерево
        free(node);//освобождаем сам узел
    }
}

//ФУНКЦИЯ ДЛЯ ВЫВОДА ТАБЛИЦЫ КОДОВ
void print_symbol_codes(HuffmanCode codes[256], SymbolFreq *freqs, int count){
    printf("\nТаблица кодов Хаффмана:\n");
    printf("=====================\n");
    printf("Символ | Частота | Код\n");
    printf("-------|---------|-----\n");

    for (int i = 0; i < count; i++){//для каждого символа
        unsigned char symbol = freqs[i].symbol;//получаем символ
        printf("'%c'    | %7d | ", symbol, freqs[i].frequency);//выводим символ и частоту
        
        //выводим бинаргый код
        for (int j = codes[symbol].length - 1; j >= 0; j--){
            printf("%d", (codes[symbol].bits >> j) & 1);//выводим биты кода
        }
        printf("\n");
    }
}

//ФУНКЦИЯ ДЛЯ ВЫВОДА СТАТИСТИКИ СЖАТИЯ
void print_compression_stats(const char *input_file, const char *output_file){
    FILE *in = fopen(input_file, "rb"); 
    FILE *out = fopen(output_file, "rb");
    if (!in || !out) {  // Если не удалось открыть файлы
        printf("Ошибка при открытии файлов для сравнения размеров\n");
        return;
    }
    //теперь определяем размеры файлов
    fseek(in, 0, SEEK_END); //переходим в конец входного файла
    fseek(out, 0, SEEK_END); //переходим в кконец выходного файла
    long input_size = ftell(in);//получаем размер входношо файла
    long output_size = ftell(out);//получаем размер выходного
    fclose(in);
    fclose(out);
    printf("\nСтатистика сжатия:\n");
    printf("=================\n");
    printf("Размер исходного файла: %ld байт\n", input_size);
    printf("Размер сжатого файла: %ld байт\n", output_size);
    printf("Коэффициент сжатия: %.2f%%\n", 
           (1.0 - (double)output_size / input_size) * 100);
}

//ОСНОВНАЯ ФУНКЦИЯ СЖАТИЯ
void compress_to_huffman(const char *text, const char *output_file){
    //устанавливаем UTF-8 локаль
    setlocale(LC_ALL, "en_US.UTF-8");

    //1. Подсчитываем частоты символов
    SymbolFreq freqs[256]; //массив для хранения частот 
    int count = 0; //счетчик для уникальных символов
    calculate_frequencies(text, freqs, &count);
    //2. СТроим дерево Хаффмана
    node_count = 0; //обнуляем счетчик узлов
    for (int i = 0; i < count; i++){//для каждого символа
        HuffmanNode *n = malloc(sizeof(HuffmanNode));//создаем новый узел
        n->symbol = freqs[i].symbol;//сохраняем символ
        n->frequency = freqs[i].frequency;//сохраняем частоту
        n->left = n->right = NULL; //объявляем потомков
        insert_node(n);//добавляем узел в очередь
    }
    //объединяем узлы с минимальными частотами
    while (node_count > 1){//пока не останется один узел
        HuffmanNode *n1 = remove_min(); //извлекаем первый минимальный узел
        HuffmanNode *n2 = remove_min(); //извлекаем второй минимальный узел
        HuffmanNode *parent = malloc(sizeof(HuffmanNode)); //создаем родительский узел 
        parent->symbol = 0;//внутренние узлы не содрежат симвоолов
        parent->frequency = n1->frequency + n2->frequency; //суммируем частоты
        parent->left = n1; //устанавливем левого потомка
        parent->right = n2; //правого потомка
        insert_node(parent);//добавляем родительский узел в очередь
        }

        //получаем корень дерева
        HuffmanNode *root = remove_min();

        // 3. Строим коды для символов 
        HuffmanCode codes[256] = {0};//массив для хранения кодов
        build_codes(root, codes, 0, 0);//строи коды

        //чтобы было интереснее найдем самый частый и самый редкий символ в нашем тексте
        //но в консоли он не будет выводиться.......
        int max_freq = 0;
        unsigned char max_char = 0;
        for (int i = 0; i < count; i++){
            if (freqs[i].frequency > max_freq){
                max_freq = freqs[i].frequency;
                max_char = freqs[i].symbol;
            }
        }
        //теперь выводим информацию
        printf("\nСамый частый символ: ");
        if (max_char >= 32) { //печатаемые символы
            printf("'%c'", max_char);
        } else {
            switch(max_char){
                case 10: printf("(перенос строки)"); break;
                case 9: printf("(табуляция)"); break;
                case 13: printf("(возврат каретки)"); break;
                default: printf("(специальный симпол)"); 
            }
        }
        printf(" (частота: %d)\n", max_freq);
        printf("Его бинарный код: ");
        for (int i = codes[max_char].length - 1; i >= 0; i--){
            printf("%d", (codes[max_char].bits >> i) & 1);
        }
        printf(" (длина: %d бит)\n", codes[max_char].length);
        printf("Комментарий: ");
        if (max_char >= 32){
            printf("Частый символ получил короткий код\n");
        } else {
            printf("Специальный символ получил длинный код из за низкой частоты\n");
        }
        printf("\n");

        //4. записываем сжатые данные в файл
        FILE *f = fopen (output_file, "wb"); //открываем файл для записи
        if (!f) {
            fprintf(stderr, "Ошибка -> не удалось открыть файл для запсии\n");
            exit(1);
        }

        //созраняем количество уникальных символов
        fwrite(&count, sizeof(int), 1, f);//записываем кол-во символов
        for (int i = 0; i < count; i++){ //lдля кажого символа
            fputc(freqs[i].symbol, f);//записываем символ
            fwrite(&freqs[i].frequency, sizeof(int), 1, f);//записываем частоту 
        }
        
        //теперь записываем сами сжатые данные
        unsigned char bit_buffer = 0; //буфер для накопления битов 
        int bit_count = 0;//счетчик битов в буфере
        for (int i = 0; text[i]; i++){//для каждого символа в тексте
            unsigned char c = (unsigned char)text[i]; //получаем символ
            HuffmanCode code = codes[c]; // получаем его код
            for (int j = code.length - 1; j >= 0; j--){//для каждого битаь кода
                int bit = (code.bits >> j) & 1; //извлекаем бит
                bit_buffer = (bit_buffer << 1) | bit; //добавляем бит в буфер
                bit_count++;//увеличиваем счетчик битов
                if (bit_count == 8){//если буфер заполнен
                    fputc(bit_buffer, f); //записываем байт в файл 
                    bit_buffer = 0; //очищаем буфер
                    bit_count = 0;//ну и сбрасывем счетчик 

                }
            }
        }
        //записываем оставшиеся биты
        if (bit_count > 0){
            bit_buffer <<= (8 - bit_count);//сдвигаме биты влево
            fputc(bit_buffer, f);//записываем последний байт 
        }
        fclose(f); 
        free_tree(root);//освобождаем паямть дерева
        
        //ну и выводим статистику сжатия
        print_compression_stats("input.txt", output_file);
}





//ТАААК теперь функция распаковки бинарного файла нашего сжатого текста
char *decompress_from_huffman(const char *input_file){
    //Для начала надо прочитать таблицу частот из файла
    FILE *f = fopen(input_file, "rb");  
    //првоерка
    if (!f){
        fprintf(stderr, "Ошибка-> не удалось открыть файл\n");
        exit(1);
    }
    
    int count; //кол-во уникальных символов
    fread(&count, sizeof(int), 1, f);//читаем кол-во символов
    SymbolFreq freqs[256];//массив нужен для хранения частот
    for(int i = 0; i < count; i++){
        freqs[i].symbol = fgetc(f);//читаем символ
        fread(&freqs[i].frequency, sizeof(int), 1, f); //читаме частоту
    }

    //после того как мы получили данные, чтроим дерево Хаффмана
    node_count = 0;//обнуляем счетчик узлов
    for (int i = 0; i < count; i++){
        HuffmanNode *n = malloc(sizeof(HuffmanNode));//создаем новый узел
        n->symbol = freqs[i].symbol;//сохраянем символ
        n->frequency = freqs[i].frequency; //сохраняем частоту 
        n->left = n->right = NULL; //инициализируем потомков
        insert_node(n); //добавляем узел в очередь
    }
    while (node_count > 1) {//пока не останется один узел
        HuffmanNode *n1 = remove_min(); //извлеккаем первый мин-ый узел
        HuffmanNode *n2 = remove_min();//также второй мин-ый узел
        HuffmanNode *parent = malloc(sizeof(HuffmanNode)); //создаем родительский узел
        parent->symbol = 0; //внутренние узлы не сожержат символов
        parent->frequency = n1->frequency + n2->frequency; //суммируем чатсоты 
        parent->left = n1; //устанавливаем левый потомое
        parent->right = n2;//устанавливаме правого потомка
        insert_node(parent);//добавляем родительский узел в очередь
    }

    HuffmanNode *root = remove_min(); //получаем корень дерева

    //Теперь ВЫЧИСЛЯЕМ РАЗМЕР РАСПАКОВАННОГО ТЕКСТА
    int total_length = 0;//общий размер текста
    for(int i = 0; i < count; i++){
        total_length += freqs[i].frequency; //суммируем частоты 
    }

    //Теперь РАСПАКОВЫВАЕМ ДАННЫЕ
    char *output = malloc(total_length + 1); //выделяем память для текста
    if (!output){
        fprintf(stderr, "Ошибка->не удалось выделить пямять\n");
        exit(1);
    }
    int bit;//текущий бит
    unsigned char byte = 0; //текущий байт 
    int bit_pos = 0;//позиция бита в байте
    int output_pos = 0;//позиция в выходном тексте

    HuffmanNode *current = root;//начинаем с корня дерева
    while (output_pos < total_length){//пока не распаковали весь теккст
        if (bit_pos == 0){ //если прочитали все биты из байта
            if (fread(&byte, 1, 1, f) != 1) break; //читаем новый байт
            bit_pos = 8; //устанавливаем счетчик битов
        }
        bit = (byte >> 7) & 1;//извелкакем старший бит
        byte <<= 1;//сдвигаем байт влево
        bit_pos--;//уменьшаем счетчик битов
        
        //Двигаемся по дереву в зависимости от бита
        current = bit ? current->right : current->left;//идем вправо или влево
        if (!current->left && !current->right){//если дошли до листа
            output[output_pos++] = current->symbol;//сохраянем символ
            current = root;//возвращаемся к корню
        }
    }

    output[output_pos] = '\0';//добавляем завершающий нуль
    fclose(f);
    free_tree(root);
    return(output); //возвращаем распакованный текст 
}

//функция для вывода дерева Хаффмаана
void print_tree(HuffmanNode *node, int level){
    if (node == NULL) return;
    //выводим отступы для визуализации 
    for (int i = 0; i < level; i++){
        printf("  ");
    }

    if (node->left == NULL && node->right == NULL){
        //лист дерева -> выводим символ и частоту 
        printf("'%c' (freq: %d)\n", node->symbol, node->frequency);
    } else {
        //внутренний узел -> выводим частоту 
        printf("Node (freq: %d)\n", node->frequency);
    }
    //теперь рекусривно выводим левое и правое поддеревья
    print_tree(node->left, level + 1);
    print_tree(node->right, level + 1);
}
//ФУНКЦИЯ ДЛЯ ВЫВОДА КОДОВ ХАФФМАНА
void print_codes(HuffmanNode *node, unsigned int code, int length){
    if (node == NULL) return;
    if (node->left == NULL && node->right == NULL){
        //лист дерева, то есть выводим символ и его код
        printf("'%c': ", node->symbol);
        for (int i = length - 1; i >= 0; i--){
            printf("%d", (code >> i) & 1);
        }
        printf(" (length: %d)\n", length);
        return;
    }
    //рекурсивно строим коды для леовго и правого поддеревьев
    print_codes(node->left, code << 1, length + 1);
    print_codes(node->right, (code << 1) | 1, length + 1);
}