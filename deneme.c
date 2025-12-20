#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_KELIME 50
#define MAX_SATIR 100
#define DENEME_HAKKI 6

void turkce_kucuk_harf(char *str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] + 32;
        }

        else if ((unsigned char)str[i] == 0xC4 && (unsigned char)str[i+1] == 0xB0) {
            str[i] = 0xC4; str[i+1] = 0xB1; i++; // İ -> ı
        }
        else if ((unsigned char)str[i] == 0xC3 && (unsigned char)str[i+1] == 0x87) {
            str[i] = 0xC3; str[i+1] = 0xA7; i++; // Ç -> ç
        }
        else if ((unsigned char)str[i] == 0xC5 && (unsigned char)str[i+1] == 0x9E) {
            str[i] = 0xC5; str[i+1] = 0x9F; i++; // Ş -> ş
        }
        else if ((unsigned char)str[i] == 0xC4 && (unsigned char)str[i+1] == 0x9E) {
            str[i] = 0xC4; str[i+1] = 0x9F; i++; // Ğ -> ğ
        }
        else if ((unsigned char)str[i] == 0xC3 && (unsigned char)str[i+1] == 0x96) {
            str[i] = 0xC3; str[i+1] = 0xB6; i++; // Ö -> ö
        }
        else if ((unsigned char)str[i] == 0xC3 && (unsigned char)str[i+1] == 0x9C) {
            str[i] = 0xC3; str[i+1] = 0xBC; i++; // Ü -> ü
        }
    }
}

char* rastgele_kelime_sec(const char *dosya_adi) {
    FILE *dosya = fopen(dosya_adi, "r");
    if (!dosya) {
        printf("Hata: %s dosyasi acilamadi!\n", dosya_adi);
        printf("Lutfen programin bulundugu klasorde '%s' dosyasinin oldugunu kontrol edin.\n", dosya_adi);
        printf("Suanki klasor: C:\\c-cpp\\output\\ olmali\n");
        return NULL;
    }
    
    char kelimeler[MAX_SATIR][MAX_KELIME];
    int sayac = 0;
    
    while (fgets(kelimeler[sayac], MAX_KELIME, dosya) && sayac < MAX_SATIR) {
        kelimeler[sayac][strcspn(kelimeler[sayac], "\n")] = 0;
        kelimeler[sayac][strcspn(kelimeler[sayac], "\r")] = 0;
        
        if (strlen(kelimeler[sayac]) > 0) {
            turkce_kucuk_harf(kelimeler[sayac]);
            sayac++;
        }
    }
    fclose(dosya);
    
    if (sayac == 0) {
        printf("Hata: Dosyada kelime bulunamadi!\n");
        return NULL;
    }

    srand(time(NULL));
    int rastgele = rand() % sayac;
    
    char *secilen = (char*)malloc(strlen(kelimeler[rastgele]) + 1);
    strcpy(secilen, kelimeler[rastgele]);
    
    return secilen;
}

void tahmin_kontrol(const char *dogru, const char *tahmin) {
    int len = strlen(tahmin);
    char sonuc[MAX_KELIME * 20] = "";
    char kullanildi[MAX_KELIME] = {0};

    for (int i = 0; i < len; i++) {
        if (tahmin[i] == dogru[i]) {
            kullanildi[i] = 1;
        }
    }
    
    printf("  ");
    for (int i = 0; i < len; i++) {
        if (tahmin[i] == dogru[i]) {

            printf("\033[32m%c\033[0m ", tahmin[i]);
        } else {

            int bulundu = 0;
            for (int j = 0; j < len; j++) {
                if (!kullanildi[j] && tahmin[i] == dogru[j]) {
                    printf("\033[33m%c\033[0m ", tahmin[i]);
                    kullanildi[j] = 1;
                    bulundu = 1;
                    break;
                }
            }
            if (!bulundu) {

                printf("\033[90m%c\033[0m ", tahmin[i]);
            }
        }
    }
    printf("\n");    
}

int main() {
    printf("========================================\n");
    printf("      SEHIRLER WORDLE OYUNU\n");
    printf("========================================\n\n");
    
    char *dogru_kelime = rastgele_kelime_sec("sehir.txt");

    if (!dogru_kelime) {
        dogru_kelime = rastgele_kelime_sec("..\\sehir.txt");
    }

    if (!dogru_kelime) {
        dogru_kelime = rastgele_kelime_sec("..\\WordleProje\\sehir.txt");
    }
    
    if (!dogru_kelime) {
        printf("\nLutfen deneme.exe ile ayni klasorde sehir.txt dosyasinin oldugunu kontrol edin.\n");
        return 1;
    }
    
    int kelime_uzunluk = strlen(dogru_kelime);
    printf("Kelime %d harfli bir sehir ismi.\n", kelime_uzunluk);
    printf("%d tahmin hakkiniz var.\n\n", DENEME_HAKKI);
    printf("Renk Aciklamasi:\n");
    printf("  \033[32mYesil\033[0m = Dogru harf, dogru yer\n");
    printf("  \033[33mSari\033[0m  = Dogru harf, yanlis yer\n");
    printf("  \033[90mGri\033[0m   = Yanlis harf\n\n");
    
    char tahmin[MAX_KELIME];
    int kazandi = 0;
    
    for (int deneme = 1; deneme <= DENEME_HAKKI; deneme++) {
        printf("Tahmin %d/%d: ", deneme, DENEME_HAKKI);
        fgets(tahmin, MAX_KELIME, stdin);
        tahmin[strcspn(tahmin, "\n")] = 0;
        
        turkce_kucuk_harf(tahmin);
        
        if (strlen(tahmin) != kelime_uzunluk) {
            printf("  Lutfen %d harfli bir kelime girin!\n\n", kelime_uzunluk);
            deneme--;
            continue;
        }
        
        if (strcmp(tahmin, dogru_kelime) == 0) {
            printf("  ");
            for (int i = 0; i < kelime_uzunluk; i++) {
                printf("\033[32m%c\033[0m ", tahmin[i]);
            }
            printf("\n\n");
            printf("========================================\n");
            printf("  TEBRIKLER! %d denemede bildiniz!\n", deneme);
            printf("========================================\n");
            kazandi = 1;
            break;
        } else {
            tahmin_kontrol(dogru_kelime, tahmin);
            printf("\n");
        }
    }
    
    if (!kazandi) {
        printf("========================================\n");
        printf("  Maalesef bilemdiniz!\n");
        printf("  Dogru kelime: %s\n", dogru_kelime);
        printf("========================================\n");
    }
    
    free(dogru_kelime);
    return 0;
}