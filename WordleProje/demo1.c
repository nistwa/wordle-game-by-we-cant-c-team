#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

// Rastgele sayı makrosu
#define randnum(min, max) ((rand() % (int)(((max) + 1) - (min))) + (min))

#define MAX_TOUR 6          
#define START_TOUR 3        

// Global Degiskenler
char quiz_words[100][10];    
char guess[10][10];
int success[10][10]; 
char hedefKelime[10];
char kategoriAdi[20]; 
int selectedQuiz = 0;         
int current_lives = 0; 
int max_lives_for_level = 0; 
int ipucuGoster = 0; 
int ilkGirisRehber = 1;

// Fonksiyonlar
void select_word();
int fileRead(int category_index, int tour); 
void display(int tourVal, int maxHak);
int checkRow(int guessNo); 
int play(int tour);
void reset_arrays();
void drawHearts();

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
            ipucuGoster = 0; 
            
            int random_cat = randnum(0, 3);
            
            if (fileRead(random_cat, tour)) { 
                int sonuc = play(tour);
                if (current_lives <= 0 || sonuc == 0) {
                    oyunKaybedildi = 1;
                    break; 
                }
                if (tour < MAX_TOUR) {
                    printf("\n\x1b[94mTEBRIKLER! %d Harfli kelimeyi bildiniz.\x1b[0m", tour);
                    printf("\nSonraki seviyeye gecmek icin [ENTER]...");
                    while(_getch() != 13);
                }
                tour++;
                ilkGirisRehber = 0;
            } else { 
                continue; 
            }
        }

        if (oyunKaybedildi) {
            printf("\n\x1b[101m\x1b[37m OYUN BITTI! \x1b[0m");
            printf("\nDogru cevap: \x1b[92m%s\x1b[0m\n", hedefKelime);
        } else if (tour > MAX_TOUR) {
            printf("\n\x1b[102m\x1b[30m TEBRIKLER, TUM BOLUMLERI GECTINIZ! \x1b[0m\n");
        }

        printf("\nTekrar oynamak ister misiniz? Evet: [ENTER], Hayir: [ESC]");
        int sonTus = _getch();
        if (sonTus == 13) {
            system("cls");
            ilkGirisRehber = 1;
        }
        else oyunDevam = 0;
    }
    return 0;
}

void drawHearts() {
    printf("\x1b[97mHP: ");
    for (int i = 0; i < max_lives_for_level; i++) {
        if (i < current_lives) printf("\x1b[31m%c \x1b[0m", 3); 
        else printf("\x1b[90m%c \x1b[0m", 3); 
    }
    printf("\x1b[37m(%d/%d)\n", current_lives, max_lives_for_level);
}

void display(int tourVal, int maxHak) {
    system("cls"); 
    if (ilkGirisRehber) {
        printf("\x1b[96m=== WE-CAN'T-NOT-C: WORDLE ADVENTURE ===\x1b[0m\n\n");
        printf("\x1b[93mOYUN REHBERI VE RENKLER:\x1b[0m\n");
        printf("\x1b[42m\x1b[30m  A  \x1b[0m -> YESIL: Dogru yer.  \x1b[43m\x1b[30m  A  \x1b[0m -> SARI: Yanlis yer.  \x1b[100m  A  \x1b[0m -> GRI: Yok.\n");
        printf("\x1b[93mKURALLAR:\x1b[0m Yanlis tahmin/ipucu 1 \x1b[31m%c\x1b[0m goturur ve 1 SATIR harcar.\n", 3);
        printf("\x1b[90m---------------------------------------------------------------------------\x1b[0m\n\n");
    }

    printf("\x1b[96m=== WE-CAN'T-NOT-C TEAM: WORDLE ===\x1b[0m\n");
    drawHearts();
    printf("\x1b[37mKategori: \x1b[93m%s\x1b[0m | Seviye: \x1b[95m%d Harf\x1b[0m\n\n", kategoriAdi, tourVal);

    for (int i = 0; i < maxHak; i++) {
        if (guess[i][0] != 'x') {
            for (int j = 0; j < tourVal; j++) {
                if (success[i][j] == 1) printf("\x1b[42m\x1b[30m  %c  \x1b[0m ", guess[i][j]); 
                else if (success[i][j] == 2) printf("\x1b[43m\x1b[30m  %c  \x1b[0m ", guess[i][j]); 
                else printf("\x1b[100m\x1b[37m  %c  \x1b[0m ", guess[i][j]); 
            }
            printf("\n\n");
        } else {
            for(int k=0; k<tourVal; k++) printf("\x1b[100m     \x1b[0m ");
            printf("\n\n");
        }
    }

    if (ipucuGoster) {
        printf("\x1b[36m--- KELIME BANKASI ---\x1b[0m\n");
        for (int i = 0; i < selectedQuiz; i++) {
            printf("\x1b[97m%s  ", quiz_words[i]);
            if ((i + 1) % 5 == 0) printf("\n");
        }
        printf("\x1b[0m\n");
    } else {
        printf("\x1b[90m[Ipucu: '?' | Bedel: 1 KALP (%c) + 1 SATIR]\x1b[0m\n", 3);
    }
}

int play(int tour) {
    int maxHak = tour + 1;
    max_lives_for_level = maxHak;
    current_lives = maxHak; 

    select_word();

    for (int d = 0; d < maxHak; d++) {
        char input[20];
        while(1) {
            display(tour, maxHak);
            if (current_lives <= 0) return 0;
            
            printf("\n\x1b[97mTahmininizi girin: \x1b[0m");
            scanf("%s", input);

            if (strcmp(input, "?") == 0) {
                if (current_lives > 1) { 
                    ipucuGoster = 1;
                    current_lives--;
                    for(int k=0; k<tour; k++) {
                        guess[d][k] = '-'; 
                        success[d][k] = 3; 
                    }
                    goto ipucuKullanildi; 
                } else {
                    printf("\n\x1b[31mSon kalbinle ipucu alamazsin!\x1b[0m");
                    Sleep(1000);
                }
            } else if (strlen(input) != (size_t)tour) {
                printf("\n\x1b[31mLutfen %d harfli bir kelime girin!\x1b[0m", tour);
                Sleep(800);
            } else {
                strcpy(guess[d], input);
                ilkGirisRehber = 0;
                break; 
            }
        }

        if (checkRow(d)) {
            display(tour, maxHak);
            return 1;
        } else {
            current_lives--; 
        }

        ipucuKullanildi: ; 
    }
    return 0;
}

int fileRead(int category_index, int tour) {
    FILE *file = NULL;
    switch(category_index) {
        case 0: file = fopen("hayvan.txt", "r"); strcpy(kategoriAdi, "HAYVANLAR"); break;
        case 1: file = fopen("ulke.txt", "r"); strcpy(kategoriAdi, "ULKELER"); break;
        case 2: file = fopen("sehir.txt", "r"); strcpy(kategoriAdi, "SEHIRLER"); break;
        case 3: file = fopen("bitki.txt", "r"); strcpy(kategoriAdi, "BITKILER"); break;
    }
    if (file == NULL) return 0;

    char line[100]; 
    int i = 0;
    while(fgets(line, sizeof(line), file) != NULL && i < 100) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == (size_t)tour) { 
            strncpy(quiz_words[i], line, tour);
            quiz_words[i][tour] = '\0';
            i++;
        }
    }
    selectedQuiz = i; 
    fclose(file); 
    return (i > 0);
}

void select_word() {
    if (selectedQuiz > 0) {
        int r = randnum(0, selectedQuiz - 1);
        strcpy(hedefKelime, quiz_words[r]);
        for(int i=0; i<(int)strlen(hedefKelime); i++) 
            hedefKelime[i] = toupper((unsigned char)hedefKelime[i]);
    }
}

int checkRow(int guessNo) {
    int uzunluk = (int)strlen(hedefKelime);
    int dogruSayisi = 0;
    int hedefKullanildi[10] = {0}; 

    for(int k=0; k<uzunluk; k++) 
        guess[guessNo][k] = toupper((unsigned char)guess[guessNo][k]);

    for (int i = 0; i < uzunluk; i++) {
        if (guess[guessNo][i] == hedefKelime[i]) { 
            success[guessNo][i] = 1; 
            hedefKullanildi[i] = 1; 
            dogruSayisi++; 
        } else { 
            success[guessNo][i] = 0; 
        }
    }

    for (int i = 0; i < uzunluk; i++) {
        if (success[guessNo][i] == 1) continue; 
        success[guessNo][i] = 0; 
        for (int j = 0; j < uzunluk; j++) {
            if (guess[guessNo][i] == hedefKelime[j] && hedefKullanildi[j] == 0) {
                success[guessNo][i] = 2; 
                hedefKullanildi[j] = 1; 
                break;
            }
        }
    }
    return (dogruSayisi == uzunluk);
}

void reset_arrays() {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            guess[i][j] = 'x'; 
            success[i][j] = 0;
        }
    }
}
