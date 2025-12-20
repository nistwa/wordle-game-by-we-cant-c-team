#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // enableVirtualTerminal ve GetStdHandle için
#include <time.h>    // srand(time(NULL)) için
#include <string.h>  // string fonksiyonları için
#include <ctype.h>   // toUpper için

// Wordle oyunu için Sabitler
#define MAX_WORDS 48
#define MAX_WORD_LEN 32
#define MAX_ATTEMPTS 10 // Genel maksimum deneme hakkı
#define START_TOUR 1
#define _CRT_SECURE_NO_WARNINGS 1 // Visual Studio'da uyarıları kapatmak için

// Global Diziler
char quiz_words[MAX_WORDS][MAX_WORD_LEN];  // Okunacak kelimeler
char guess[MAX_ATTEMPTS][MAX_WORD_LEN];    // Tahminler
int success[MAX_ATTEMPTS][MAX_WORD_LEN];   // Başarı durumu (0: yanlış, 1: yanlış yer, 2: doğru)
int selectedQuiz = 0; // Seçilen kelimenin indeksi
int devam = 1;        // Oyun döngüsünü kontrol eden bayrak

// Global Değişkenler
int selectedCategory = 0;
int word_count_global = 0;
int current_word_length = 0;
int current_max_attempts = 0; // Mevcut kategoriye özel deneme hakkı

// Kategori Bilgileri
char* categories[] = {"hayvan.txt", "ulke.txt", "sehir.txt", "bitki.txt"};
char* category_names[] = {"HAYVAN", "ULKE", "SEHIR", "BITKI"};
int category_word_lengths[] = {3, 4, 5, 6};    // Kategoriye özel kelime uzunlukları
int category_max_attempts[] = {4, 5, 6, 6}; // Kategoriye özel deneme hakları

// Fonksiyon Prototipleri (Fotoğraftakine benzer ve mevcut koda uygun)
void select_word();
int fileRead(int tour);
void display(int mode, int quizNo);
int checkRow(int guessNo);
int play(int tour);
void decision(int tour);
void reset_arrays();
void toUpperCase(char *str);
void clearScreen();
void enableVirtualTerminal();
int randnum(int min, int max);
void select_random_category();
void show_game_stats(int won, int attempts);
void trim_newline(char *s);

// Ana Fonksiyon
int main()
{
    // Rastgele sayı üreteci başlangıcı
    srand((unsigned)time(NULL));
    enableVirtualTerminal(); // Renkli çıktılar için (Windows'ta)
    int tour = START_TOUR;

    // Ana oyun döngüsü
    while (devam)
    {
        reset_arrays();          // Dizileri sıfırla
        select_random_category(); // Rastgele bir kategori seç
        
        // Dosya okuma ve kelime seçimi
        if (!fileRead(tour)) {
            break; // Dosya okunamıyorsa döngüyü sonlandır
        }

        play(tour);    // Oyun turunu oyna
        decision(tour); // Tekrar oynama kararı
        tour++;        // Tur sayısını artır
    }
    return 0;
}

// Windows konsolunda ANSI renk kodlarını etkinleştirir
void enableVirtualTerminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

// Global dizileri (tahminler, başarı durumu ve kelimeler) sıfırlar
void reset_arrays() {
    memset(guess, '\0', sizeof(guess));
    memset(success, 0, sizeof(success));
    memset(quiz_words, '\0', sizeof(quiz_words));
}

// Rastgele bir kategori seçer ve oyun parametrelerini ayarlar
void select_random_category() {
    selectedCategory = randnum(0, 3);
    current_word_length = category_word_lengths[selectedCategory];
    current_max_attempts = category_max_attempts[selectedCategory];
}

// Dosyadan kelimeleri okur
int fileRead(int tour) {
    (void)tour;
    char* filename = categories[selectedCategory];
    char filepath[260];
    FILE *file = NULL;

    // Dosya yolu denemeleri (Mevcut kodunuzdan alındı)
    file = fopen(filename, "r");
    // ... diğer fopen denemeleri ...

    if (file == NULL) {
        clearScreen();
        printf("\n===========================================\n");
        printf("          HATA: DOSYA BULUNAMADI!\n");
        printf("===========================================\n");
        printf("\nAranan dosya: %s\n", filename);
        devam = 0;
        return 0;
    }

    int count = 0;
    char temp_line[MAX_WORD_LEN];

    printf("\n--- Okunan Kelimeler ---\n");
    
    // Kelimeleri dinamik olarak okuma ve kaydetme
    while (fgets(temp_line, sizeof(temp_line), file) != NULL) {
        
        trim_newline(temp_line); // Satır sonu karakterlerini temizle
        
        if (strlen(temp_line) == 0) continue; // Boş satırları atla
        if (count >= MAX_WORDS) break; // Dizi sınırını aşma kontrolü

        // Kelimenin tamamını kopyala (MAX_WORD_LEN-1 ile sınırlandırılmış)
        strncpy(quiz_words[count], temp_line, MAX_WORD_LEN-1);
        quiz_words[count][MAX_WORD_LEN-1] = '\0'; // NULL sonlandırma
        
        toUpperCase(quiz_words[count]); // Büyük harfe çevir
        
        // Opsiyonel: Okunan kelimeyi görelim
        printf("%s (%zu Harf)\n", quiz_words[count], strlen(quiz_words[count])); 
        
        count++;
    }

    printf("--------------------------\n\n");

    word_count_global = count;
    fclose(file);

    if (word_count_global == 0) {
        printf("\nHata: %s dosyasi bos veya okunamadi!\n", filename);
        devam = 0;
        return 0;
    }
    
    // ÖNEMLİ: current_word_length'ı dosyadan gelen ilk kelimenin gerçek uzunluğuna göre ayarla
    // Bu, 3, 4, 5 veya 6 harfli kelimelerle dinamik olarak çalışmayı sağlar.
    current_word_length = (int)strlen(quiz_words[0]); 
    // Deneme hakkını da kategori ayarlarına göre geri getiriyoruz:
    current_max_attempts = category_max_attempts[selectedCategory];
    
    select_word();
    return 1;
}


// quiz_words dizisinden rastgele bir kelime seçer
void select_word() {
    selectedQuiz = randnum(0, word_count_global - 1);
}

// Belirtilen aralıkta (min ve max dahil) rastgele sayı üretir
int randnum(int min, int max) {
    if (max < min) return min;
    return min + rand() % (max - min + 1);
}

// Oyunun oynandığı ana döngü

int play(int tour) {
    (void)tour;
    int attempts = 0;
    int won = 0;
    char input_buffer[MAX_WORD_LEN];

    while (attempts < current_max_attempts && !won) {
        clearScreen();
        display(1, attempts);

        printf("\nTahmin %d/%d: ", attempts + 1, current_max_attempts);

        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            printf("Hata: Giris yapilamadi!\n");
            continue;
        }
        trim_newline(input_buffer);
        toUpperCase(input_buffer);

        if ((int)strlen(input_buffer) != current_word_length) {
            printf("\n\033[41m\033[37m UYARI \033[0m Lutfen %d harfli bir kelime girin!\n", current_word_length);
            printf("Devam etmek icin Enter'a basin...");
            fgets(input_buffer, sizeof(input_buffer), stdin);
            continue;
        }

        // Geçerli tahmin kaydet ve kontrol et
        strncpy(guess[attempts], input_buffer, MAX_WORD_LEN-1);
        guess[attempts][MAX_WORD_LEN-1] = '\0';
        checkRow(attempts);

        // Kazanma kontrolü
        won = 1;
        for (int i = 0; i < current_word_length; i++) {
            if (success[attempts][i] != 2) {
                won = 0;
                break;
            }
        }
        attempts++;

        if (won) {
            clearScreen();
            display(1, attempts);
            show_game_stats(1, attempts);
            return 1;
        }
    }

    // Kaybetme durumu
    if (!won) {
        clearScreen();
        display(1, attempts);
        show_game_stats(0, attempts);
    }

    return 0;
}

// Oyun sonu istatistiklerini gösterir
void show_game_stats(int won, int attempts) {
    printf("\n\n");
    if (won) {
        printf("===========================================\n");
        printf("  \033[42m\033[30m  TEBRIKLER! KAZANDINIZ!  \033[0m\n");
        printf("===========================================\n");
        printf("Dogru Kelime: \033[1;32m%s\033[0m\n", quiz_words[selectedQuiz]);
        printf("Toplam Deneme: %d / %d\n", attempts, current_max_attempts);

        int success_percent = (int)((float)(current_max_attempts - attempts + 1) / current_max_attempts * 100.0f);
        if (success_percent < 0) success_percent = 0;
        printf("Basari Yuzdesi: %d%%\n", success_percent);
    } else {
        printf("===========================================\n");
        printf("  \033[41m\033[37m  MAALESEF KAYBETTINIZ!  \033[0m\n");
        printf("===========================================\n");
        printf("Dogru Kelime: \033[1;33m%s\033[0m\n", quiz_words[selectedQuiz]);
        printf("Toplam Deneme: %d / %d\n", attempts, current_max_attempts);
    }
    printf("===========================================\n");
    printf("\nDevam etmek icin Enter'a basin...");
    char tmp[MAX_WORD_LEN];
    fgets(tmp, sizeof(tmp), stdin); // Kullanıcının Enter'a basmasını bekle
}

// Tahmin edilen satırı (guessNo) doğru kelimeye göre kontrol eder ve success dizisini doldurur
int checkRow(int guessNo) {
    char temp_correct[MAX_WORD_LEN];
    char temp_guess[MAX_WORD_LEN];

    strncpy(temp_correct, quiz_words[selectedQuiz], MAX_WORD_LEN-1);
    temp_correct[MAX_WORD_LEN-1] = '\0';
    strncpy(temp_guess, guess[guessNo], MAX_WORD_LEN-1);
    temp_guess[MAX_WORD_LEN-1] = '\0';

    // success dizisini sıfırla
    for (int i = 0; i < current_word_length; i++) success[guessNo][i] = 0;

    // 1. Aşama: Yeşilleri (Doğru Harf & Doğru Yer) işaretle
    for (int i = 0; i < current_word_length; i++) {
        if (temp_guess[i] == temp_correct[i]) {
            success[guessNo][i] = 2; // Yeşil
            temp_correct[i] = '*';   // Doğru kelimedeki harfi tüket
            temp_guess[i] = '#';     // Tahmindeki harfi tüket
        }
    }

    // 2. Aşama: Sarıları (Doğru Harf & Yanlış Yer) işaretle
    for (int i = 0; i < current_word_length; i++) {
        if (temp_guess[i] != '#') { // Tüketilmiş harfleri atla
            for (int j = 0; j < current_word_length; j++) {
                if (temp_guess[i] == temp_correct[j]) {
                    success[guessNo][i] = 1; // Sarı
                    temp_correct[j] = '*';   // Doğru kelimedeki harfi tüket
                    break;
                }
            }
        }
    }

    return 1;
}

// Oyun ekranını ve tahmin tahtasını gösterir
void display(int mode, int quizNo) {
    (void)mode;
    printf("\n");
    printf("===========================================\n");
    printf("            WORDLE OYUNU\n");
    printf("===========================================\n");
    printf("Kategoriniz: \033[1;36m%s\033[0m\n", category_names[selectedCategory]);
    printf("Kelime Uzunlugu: %d harf\n", current_word_length);
    printf("Deneme Hakkiniz: %d\n", current_max_attempts);
    printf("===========================================\n");
    printf("Suan: %d / %d deneme\n", quizNo, current_max_attempts);

    printf("Ilerleme: [");
    for (int i = 0; i < current_max_attempts; i++) {
        if (i < quizNo) printf("\033[32m■\033[0m");
        else printf("□");
    }
    printf("]\n");
    printf("===========================================\n\n");

    if (quizNo > 0) {
        printf("--- TAHMINLERINIZ ---\n\n");
        for (int i = 0; i < quizNo; i++) {
            printf("  ");
            for (int j = 0; j < current_word_length; j++) {
                char ch = guess[i][j];
                if (ch == '\0') ch = ' ';
                
                // ANSI renk kodları ile çıktı
                if (success[i][j] == 2) {
                    printf("\033[42m\033[30m %c \033[0m ", ch); // Yeşil (Doğru yer)
                } else if (success[i][j] == 1) {
                    printf("\033[43m\033[30m %c \033[0m ", ch); // Sarı (Yanlış yer)
                } else {
                    printf("\033[41m\033[37m %c \033[0m ", ch); // Kırmızı (Yanlış harf)
                }
            }
            printf("\n");
        }
        printf("\n");
    }

    printf("Renk Aciklamasi: ");
    printf("\033[42m\033[30m YESIL \033[0m = Dogru yer  ");
    printf("\033[43m\033[30m SARI \033[0m = Yanlis yer  ");
    printf("\033[41m\033[37m KIRMIZI \033[0m = Yanlis harf\n");
}

// Oyuncuya tekrar oynamak isteyip istemediğini sorar
void decision(int tour) {
    (void)tour;
    printf("\n\nTekrar oynamak ister misiniz? (E/H): ");
    char choice[8];
    if (fgets(choice, sizeof(choice), stdin) == NULL) {
        devam = 0;
        return;
    }
    // Sadece ilk karaktere bak ve büyük/küçük harf duyarlılığını kaldır
    if (toupper((unsigned char)choice[0]) == 'E') {
        devam = 1;
    } else {
        devam = 0;
        clearScreen();
        printf("\n\n");
        printf("===========================================\n");
        printf("      OYUN BITTI! TESEKKURLER!\n");
        printf("===========================================\n");
        printf("\n  Oynadiginiz icin tesekkurler!\n");
        printf("  Tekrar bekleriz! :)\n");
        printf("\n===========================================\n\n");
    }
}

// Konsol ekranını temizler (Windows için "cls")
void clearScreen() {
    system("cls");
}

// Verilen dizideki tüm karakterleri büyük harfe çevirir
void toUpperCase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}

// Dizinin sonundaki yeni satır (\n) veya satır başı (\r) karakterlerini siler
void trim_newline(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    while (len > 0 && (s[len-1] == '\n' || s[len-1] == '\r')) {
        s[len-1] = '\0';
        len--;
    }
}