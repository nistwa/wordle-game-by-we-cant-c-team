#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

#define randnum(min, max) ((rand() % ((max) - (min) + 1)) + (min))

#define MAX_TOUR 6
#define START_TOUR 3

char quiz_words[100][10];
char guess[10][10];
int success[10][10];
char hedefKelime[10];
char kategoriAdi[20];

int selectedQuiz = 0;
int current_lives = 0;
int max_lives_for_level = 0;
int ilkGirisRehber = 1;

/* Fonksiyonlar */
int fileRead(int category_index, int tour);
void select_word();
void display(int tourVal, int maxHak);
int checkRow(int guessNo);
int play(int tour);
void reset_arrays();
void drawHearts();

/* ===================== MAIN ===================== */
int main() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    srand((unsigned int)time(NULL));

    int oyunDevam = 1;

    while (oyunDevam) {
        int tour = START_TOUR;
        int oyunKaybedildi = 0;

        while (tour <= MAX_TOUR) {
            reset_arrays();

            int random_cat = randnum(0, 3);
            if (!fileRead(random_cat, tour)) continue;

            if (!play(tour)) {
                oyunKaybedildi = 1;
                break;
            }

            if (tour < MAX_TOUR) {
                printf("\n\x1b[94mTEBRIKLER! %d harfli kelimeyi bildiniz.\x1b[0m", tour);
                printf("\nDevam etmek icin [ENTER]");
                while (_getch() != 13);
            }

            tour++;
            ilkGirisRehber = 0;
        }

        if (oyunKaybedildi) {
            printf("\n\x1b[101m\x1b[37m OYUN BITTI \x1b[0m");
            printf("\nDogru cevap: \x1b[92m%s\x1b[0m\n", hedefKelime);
        } else {
            printf("\n\x1b[102m\x1b[30m TUM SEVIYELERI GECTIN! \x1b[0m\n");
        }

        printf("\nTekrar oynamak ister misiniz? [ENTER] / [ESC]");
        int tus = _getch();
        if (tus == 13) {
            system("cls");
            ilkGirisRehber = 1;
        } else oyunDevam = 0;
    }
    return 0;
}

/* ===================== GORSEL ===================== */
void drawHearts() {
    printf("\x1b[97mHP: ");
    for (int i = 0; i < max_lives_for_level; i++) {
        if (i < current_lives)
            printf("\x1b[31m%c \x1b[0m", 3);
        else
            printf("\x1b[90m%c \x1b[0m", 3);
    }
    printf("\x1b[37m(%d/%d)\n", current_lives, max_lives_for_level);
}

void display(int tourVal, int maxHak) {
    system("cls");

    if (ilkGirisRehber) {
        printf("\x1b[96m=== WE CAN'T C | WORDLE ADVENTURE ===\x1b[0m\n\n");
        printf("\x1b[42m A \x1b[0m Dogru yer | ");
        printf("\x1b[43m A \x1b[0m Yanlis yer | ");
        printf("\x1b[100m A \x1b[0m Yok\n");
        printf("\x1b[90mYanlis tahmin = 1 can gider\x1b[0m\n");
        printf("--------------------------------------------------\n\n");
    }

    printf("\x1b[96mWE CAN'T C TEAM - WORDLE\x1b[0m\n");
    drawHearts();
    printf("Kategori: \x1b[93m%s\x1b[0m | Seviye: %d harf\n\n", kategoriAdi, tourVal);

    for (int i = 0; i < maxHak; i++) {
        if (guess[i][0] != 'x') {
            for (int j = 0; j < tourVal; j++) {
                if (success[i][j] == 1)
                    printf("\x1b[42m %c \x1b[0m ", guess[i][j]);
                else if (success[i][j] == 2)
                    printf("\x1b[43m %c \x1b[0m ", guess[i][j]);
                else
                    printf("\x1b[100m %c \x1b[0m ", guess[i][j]);
            }
        } else {
            for (int k = 0; k < tourVal; k++)
                printf("\x1b[100m   \x1b[0m ");
        }
        printf("\n\n");
    }

    /* === KATEGORI KELIMELERI === */
    printf("\x1b[95m=== %s KELIME HAVUZU ===\x1b[0m\n", kategoriAdi);
    for (int i = 0; i < selectedQuiz; i++) {
        printf("%s  ", quiz_words[i]);
        if ((i + 1) % 5 == 0) printf("\n");
    }
    printf("\n");
}

/* ===================== OYUN ===================== */
int play(int tour) {
    int maxHak = tour + 1;
    max_lives_for_level = maxHak;
    current_lives = maxHak;

    select_word();

    for (int d = 0; d < maxHak; d++) {
        char input[20];

        while (1) {
            display(tour, maxHak);

            if (current_lives <= 0)
                return 0;

            printf("\nTahmin gir: ");
            scanf("%s", input);

            if (strlen(input) != (size_t)tour) {
                printf("\n\x1b[31m%d harfli kelime gir!\x1b[0m", tour);
                Sleep(700);
            } else {
                strcpy(guess[d], input);
                break;
            }
        }

        if (checkRow(d)) {
            display(tour, maxHak);
            return 1;
        }
        current_lives--;
    }
    return 0;
}

/* ===================== DOSYA ===================== */
int fileRead(int category_index, int tour) {
    FILE* file = NULL;

    if (category_index == 0) { file = fopen("hayvan.txt", "r"); strcpy(kategoriAdi, "HAYVANLAR"); }
    if (category_index == 1) { file = fopen("ulke.txt", "r"); strcpy(kategoriAdi, "ULKELER"); }
    if (category_index == 2) { file = fopen("sehir.txt", "r"); strcpy(kategoriAdi, "SEHIRLER"); }
    if (category_index == 3) { file = fopen("bitki.txt", "r"); strcpy(kategoriAdi, "BITKILER"); }

    if (!file) return 0;

    char line[100];
    int i = 0;

    while (fgets(line, sizeof(line), file) && i < 100) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == (size_t)tour)
            strcpy(quiz_words[i++], line);
    }

    selectedQuiz = i;
    fclose(file);
    return i > 0;
}

void select_word() {
    int r = randnum(0, selectedQuiz - 1);
    strcpy(hedefKelime, quiz_words[r]);
    for (int i = 0; hedefKelime[i]; i++)
        hedefKelime[i] = toupper(hedefKelime[i]);
}

int checkRow(int g) {
    int len = strlen(hedefKelime);
    int used[10] = {0};
    int dogru = 0;

    for (int i = 0; i < len; i++)
        guess[g][i] = toupper(guess[g][i]);

    for (int i = 0; i < len; i++) {
        if (guess[g][i] == hedefKelime[i]) {
            success[g][i] = 1;
            used[i] = 1;
            dogru++;
        } else success[g][i] = 0;
    }

    for (int i = 0; i < len; i++) {
        if (success[g][i]) continue;
        for (int j = 0; j < len; j++) {
            if (!used[j] && guess[g][i] == hedefKelime[j]) {
                success[g][i] = 2;
                used[j] = 1;
                break;
            }
        }
    }
    return dogru == len;
}

void reset_arrays() {
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 10; j++) {
            guess[i][j] = 'x';
            success[i][j] = 0;
        }
}
