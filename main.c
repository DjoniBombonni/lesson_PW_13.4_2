#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void draw_line(char plot[20][81], int x1, int y1, int x2, int y2, char symbol) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    while (1) {
        if (x1 >= 0 && x1 < 80 && y1 >= 0 && y1 < 20) {
            plot[19 - y1][x1] = symbol;
        }
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void plot_signal(double *signal, int N, const char *title, int color_code) {
    printf("%s:\n", title);
    double min_val = signal[0], max_val = signal[0];
    for (int i = 1; i < N; i++) {
        if (signal[i] < min_val) min_val = signal[i];
        if (signal[i] > max_val) max_val = signal[i];
    }

    int width = 80; //ширина графика
    int height = 20; //высота графика
    char plot[height][width + 1];
    memset(plot, ' ', sizeof(plot));
    for (int i = 0; i < height; i++) {
        plot[i][width] = '\0';
    }

    int prev_x = -1, prev_y = -1;
    for (int i = 0; i < N; i++) {
        int x = (int)((double)i / (N - 1) * (width - 1));
        int y = (int)((signal[i] - min_val) / (max_val - min_val) * (height - 1));
        if (prev_x != -1 && prev_y != -1) {
            draw_line(plot, prev_x, prev_y, x, y, '*');
        }
        prev_x = x;
        prev_y = y;
    }

    //Ось Y с метками
    printf("\033[%dm", color_code);
    for (int i = 0; i < height; i++) {
        double val = max_val - (double)i / (height - 1) * (max_val - min_val);
        printf("%6.2f |%s\n", val, plot[i]);
    }

    printf("\033[0m");

    //Ось X с метками
    printf("       ");
    for (int i = 0; i < width; i += 10) {
        printf("%-10d", i);
    }

    printf("\n");
    printf("\n");
}

int main() {
    int N;

    do {
        printf("Введите размер массива (положительное целое число): ");
        if (scanf("%d", &N) != 1) {

            //Очистка буфера от некорректного ввода (буквы, символы)
            while (getchar() != '\n');
            printf("Ошибка: введите корректное целое число.\n");
            N = 0;  //Сброс, чтобы цикл продолжился
        }
        else if (N <= 0) {
            printf("Ошибка: размер должен быть положительным.\n");
        }
    } while (N <= 0);

    double *array = (double *)malloc(N * sizeof(double));
    double *clean_array = (double *)malloc(N * sizeof(double));

    if (array == NULL || clean_array == NULL) {
        printf("Ошибка выделения памяти!\n");
        printf("Перезапустите программу и попробуйте снова.\n");
        return 1;
    }

    srand(time(NULL));

    for (int i = 0; i < N; i++) {
        double t = (double)i / N; //нормированное время
        clean_array[i] = sin(2 * M_PI * t); //гармонический сигнал (синусоида)
        //добавляем шумовую компоненту: случайное значение от -0.1 до 0.1
        array[i] = clean_array[i] + ((double)rand() / RAND_MAX - 0.5) * 0.2;
    }

    //Расчёт статистики
    double signal_power = 0.0, noise_power = 0.0;
    for (int i = 0; i < N; i++) {
        signal_power += clean_array[i] * clean_array[i];
        double noise = array[i] - clean_array[i];
        noise_power += noise * noise;
    }

    signal_power /= N;
    noise_power /= N;
    double snr = 10.0 * log10(signal_power / noise_power);

    printf("Статистика сигнала:\n");
    printf("Мощность сигнала: %.4f\n", signal_power);
    printf("Мощность шума: %.4f\n", noise_power);
    printf("SNR (отношение сигнал/шум): %.2f dB\n", snr);
    printf("\n");
    printf("Таблица сигнала:\n");
    printf("i\t t\t\t Чистый\t\t Шум\t\t Зашумлённый\n");

    for (int i = 0; i < N; i++) {
        double t = (double)i / N;
        double noise = array[i] - clean_array[i];
        printf("%d\t%.2f\t\t%.4f\t\t%.4f\t\t%.4f\n", i, t, clean_array[i], noise, array[i]);
    }

    printf("\n");

    plot_signal(clean_array, N, "Чистый сигнал", 34);
    plot_signal(array, N, "Зашумлённый сигнал", 31);

    free(clean_array);
    free(array);
    
    return 0;
}