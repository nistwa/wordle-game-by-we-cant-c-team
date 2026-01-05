/******************************************************************************
 * PROJE ADI    : Wordle Oyunu
 * TAKIM ADI    : We Can't C (without glasses)
 * KURUM/DERS   : Marmara Üniversitesi/Elektrik - Elektronik MÜhendisliği - 1.Sınıf - Bilgi Teknolojileri ve Algortimaya Giriş
 * TARİH        : 20.12.2025
 * VERSIYON     : 1.0
 * --------------------------------------------------------------------------
 * AÇIKLAMA:
 * Bu proje, popüler kelime bulmaca oyunu Wordle'ın C dili ile yazılmış
 * bir denemesidir. Kullanıcının kelimeleri tahmin etmesi üzerine kuruludur. Oyun VsCode üzerinde geliştirilmiştir.
 * 
 * 
 * CodeBlocks için;
 * #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
 * #include <windows.h>
 * üstte gördüğünüz düzenlemeyi yapın lütfen derleyiciden kaynaklıdır.
*
 *
 * --------------------------------------------------------------------------
 * TAKIM ÜYELERİ
 * 1. Şevval Türkan Kafes
 * 2. Rana Sultan Koca 170525016
 * 3. Nisanur Özpehlivan 170525844
 * 
 * --------------------------------------------------------------------------
 * Meraklısına;
 * Takım üyeleri ve kullanıcı için kodun açıklamaları yorum satırı ile koda entegre edilmiştir.
 *
 *****************************************************************************/


/*
 * _CRT_SECURE_NO_WARNINGS:
 * Visual Studio derleyicisinde scanf, strcpy gibi standart C fonksiyonları
 * için verilen "güvensiz fonksiyon" uyarılarını devre dışı bırakır.
 * Programın çalışmasını veya C dilinin davranışını değiştirmez,
 * yalnızca derleme sırasında gösterilen uyarıları kapatır. Vs code için kullanıldı.
 */
#define _CRT_SECURE_NO_WARNINGS


/* KÜTÜPHANE TANIMLAMALARI */
#include <stdio.h>    // Standart Giriş/Çıkış (Input/Output). printf ve scanf fonksiyonları için gereklidir.
#include <stdlib.h>   // Standart Kütüphane. rand() (rastgele sayı), system() (ekran temizleme) gibi araçlar için.
#include <string.h>   // String (Metin) İşlemleri. Kelime karşılaştırma (strcmp) ve kopyalama işlemleri için.
#include <time.h>     // Zaman fonksiyonları. Rastgele kelime seçerken her seferinde farklı sonuç almak (srand) için saati kullanacağız.
#include <conio.h>    // Konsol Giriş/Çıkış. Klavyeden anlık tuş okuma (getch) işlemleri için.
#include <windows.h>  // Windows API. Konsoldaki yazı renklerini (Yeşil/Sarı/Gri) değiştirmek ve ekran koordinatları için.

// Rastgele Sayı Üreteci
// Minimum ve maksimum sayı aralığında rastgele sayı üretir
// Kullanım: randnum(0, 15) - 0 ile 15 arası rastgele sayı
#define randnum(min, max) ((rand() % ((max) - (min) + 1)) + (min)) // rand() fonksiyonunu kullanarak min ile max (dahil) arasında rastgele tamsayı üretmek için tanımlanmıştır.
#define MAX_TOUR 6                                                 // Tur sayıları kodun başından tanımlandı.
#define START_TOUR 3

char quiz_words[100][10];       // Oyunda kullanılacak kelimeleri tutar; en fazla 100 kelime, her kelime 9 karakter + '\0'
char guess[10][10];            // Kullanıcının tahminlerini tutar; en fazla 10 tahmin, her tahmin 9 karakter + '\0'
int success[10][10];             // Her tahmindeki harflerin doğru/yanlış durumunu tutar (1: doğru, 0: yanlış)
char hedefKelime[10];            // O anki hedef kelime
char kategoriAdi[20];            // Seçilen kategori adı
int selectedQuiz = 0;           // Seçilen quiz/kategori indeksi
int current_lives = 0;          // Oyuncunun mevcut can sayısı
int max_lives_for_level = 0;     // Seçilen seviyeye göre maksimum can sayısı
int ilkGirisRehber = 1;         // Oyuna ilk girişte rehberin gösterilip gösterilmeyeceğini belirtir (1: göster)


/* Fonksiyonlar */
void konsol_ayarla();                           // Konsol ayarlarını yapar
void oyun_dongusu();                           // Ana oyun döngüsü
int seviye_dongusu();                          // Seviye ilerlemesi
void yeniden_oyun_sorgusu(int *oyunDevam);     // Tekrar oynama kontrolü
int fileRead(int category_index, int tour);    // Seçilen kategoriye göre dosyadan kelimeleri okur
void select_word();                            // Okunan kelimeler arasından hedef kelimeyi seçer
void display(int tourVal, int maxHak);         // Oyun ekranını ve mevcut durumu ekrana yazdırır
int checkRow(int guessNo);                     // Girilen tahminin doğruluğunu kontrol eder
int play(int tour);                            // Oyunun ana akışını çalıştırır
void reset_arrays();                           // Oyun için kullanılan dizileri sıfırlar
void drawHearts();                             // Kalan canları gösterir


/* ===================== MAIN ===================== */
int main() {
    konsol_ayarla();
    oyun_dongusu();
    return 0;
}

/* ===================== ANA FONKSİYONLAR ===================== */

void konsol_ayarla() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // Standart çıktı (konsol) tanıtıcısını alır
    DWORD dwMode = 0;                              // Konsol mod bilgilerini tutacak değişken
    GetConsoleMode(hOut, &dwMode);                 // Mevcut konsol modlarını okur
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING); // ANSI renk ve kaçış kodlarını etkinleştirir

    // Rastgele sayı üreteci başlatma
    // srand() fonksiyonu her program çalıştığında farklı sonuç üretmesi için kullanılır
    // time(NULL) ile zamana bağlı bir başlangıç (seed) değeri verilir
    srand((unsigned int)time(NULL));
}

void oyun_dongusu() {
    int oyunDevam = 1;                             // Oyunun devam edip etmeyeceğini kontrol eden bayrak

    while (oyunDevam) {                 // Oyun tamamen bitene kadar ana döngü
        int oyunKaybedildi = seviye_dongusu();

        if (oyunKaybedildi) {               // Oyuncu herhangi bir seviyede kaybettiyse
            printf("\n\x1b[101m\x1b[37m OYUN BITTI \x1b[0m");     // Kırmızı arka planla oyun bitti mesajı
            printf("\nDogru cevap: \x1b[92m%s\x1b[0m\n", hedefKelime);  // Doğru kelimeyi yeşil renkte gösterir
        } else {                                                          // Tüm seviyeler başarıyla tamamlandıysa
            printf("\n\x1b[102m\x1b[30m TUM SEVIYELERI GECTIN! \x1b[0m\n");  // Yeşil arka planla başarı mesajı
        }

        yeniden_oyun_sorgusu(&oyunDevam);
    }
}

int seviye_dongusu() {
    int tour = START_TOUR;          // Başlangıç kelime uzunluğu
    int oyunKaybedildi = 0;         // Oyunun kaybedilip kaybedilmediğini tutar

    while (tour <= MAX_TOUR) {           // Seviye seviye ilerleyen oyun döngüsü
        reset_arrays();                  // Önceki turdan kalan verileri temizler
        int random_cat = randnum(0, 3);  // Rastgele kelime seçimi

        if (!fileRead(random_cat, tour))   // Dosya okuma başarısızsa turu yeniden dener
            continue;

        if (!play(tour)) {          // Oyun turu oynanır
            oyunKaybedildi = 1;     // Oyuncu kaybettiyse işaretlenir
            break;                  // Seviye döngüsünden çıkılır
        }

        if (tour < MAX_TOUR) {         // Son seviye değilse bilgilendirme mesajı
            printf("\n\x1b[94mTEBRIKLER! %d harfli kelimeyi bildiniz.\x1b[0m", tour);
            printf("\nDevam etmek icin [ENTER]");
            while (_getch() != 13);     // Enter tuşuna basılana kadar bekler
        }
        tour++;                 // Bir sonraki seviyeye geçilir
        ilkGirisRehber = 0;     // Rehber yalnızca ilk turda gösterilir
    }

    return oyunKaybedildi;
}

void yeniden_oyun_sorgusu(int *oyunDevam) {
    printf("\nTekrar oynamak ister misiniz? [ENTER] / [ESC]");   // Oyuncuya yeniden oynama seçeneği sunar
    int tus = _getch();                // Klavyeden basılan tuşu bekler ve okur
    if (tus == 13) {                   // ENTER tuşuna basıldıysa
        system("cls");                 // Konsol ekranını temizler
        ilkGirisRehber = 1;            // Yeni oyunda rehberin tekrar gösterilmesini sağlar
    } else {                           // ESC veya başka bir tuşa basıldıysa
        *oyunDevam = 0;                // Oyun döngüsünü sonlandırır
    }
}

/* ===================== GORSEL ===================== */
void drawHearts() {
    // Oyuncunun mevcut canını ve maksimum canını sade şekilde gösterir
    printf("HP: %d/%d\n", current_lives, max_lives_for_level);
}


void display(int tourVal, int maxHak) {
    system("cls"); // Konsol ekranını temizler

    if (ilkGirisRehber) {  // Oyuna ilk girişte rehber bilgilerini gösterir
        printf("\x1b[96m=== WE CAN'T C | WORDLE ADVENTURE ===\x1b[0m\n\n"); //mavi
        
        printf("\x1b[42m A \x1b[0m Dogru yer | ");
        printf("\x1b[43m A \x1b[0m Yanlis yer | ");
        printf("\x1b[100m A \x1b[0m Yok\n");
        
        printf("\x1b[90mYanlis tahmin = 1 can gider\x1b[0m\n");
        printf("--------------------------------------------------\n\n");
    }

    printf("\x1b[96mWE CAN'T C TEAM - WORDLE\x1b[0m\n");
    printf("[!] Turkce karakter kullanmayin (c->c, g->g, i->i, o->o, s->s, u->u) Ornek: kus yerine KUS yazin\n");
    
    
    drawHearts(); // Oyuncunun mevcut can durumunu ekrana yazar
    
    printf("Kategori: \x1b[93m%s\x1b[0m | Seviye: %d harf\n\n", kategoriAdi, tourVal);  // Kategori adını ve kelime uzunluğunu gösterir

    for (int i = 0; i < maxHak; i++) {      // Toplam tahmin hakkı kadar satır çizer
        if (guess[i][0] != 'x') {          // Bu satırda daha önce tahmin yapıldıysa
            for (int j = 0; j < tourVal; j++) {
                if (success[i][j] == 1) {        // Harf doğru yerdeyse
                    printf("\x1b[42m");          // Yeşil 
                    printf("\x1b[37m");          // Beyaz 
                    printf(" %c ", guess[i][j]); // Harfi yazdırır
                    printf("\x1b[0m ");
                } else if (success[i][j] == 2) { // Harf yanlış yerdeyse
                    printf("\x1b[43m");          // Sarı 
                    printf("\x1b[30m");          // Siyah 
                    printf(" %c ", guess[i][j]);
                    printf("\x1b[0m ");
                } else {                        // Harf kelimede yoksa
                    printf("\x1b[100m");        // Gri
                    printf("\x1b[37m");         // Beyaz
                    printf(" %c ", guess[i][j]);
                    printf("\x1b[0m ");
                }
            }
        } else {                                 // Bu satırda henüz tahmin yapılmadıysa
            for (int k = 0; k < tourVal; k++) {  // Boş kutular çizilir
                printf("\x1b[100m");             // Gri arka plan
                printf("   ");                   // Boş hücre
                printf("\x1b[0m ");
            }
        }
        printf("\n\n"); // Her tahmin satırından sonra boşluk bırakır
    }

    /* === KATEGORI KELIMELERI === */
    printf("\x1b[95m=== %s KELIME HAVUZU ===\x1b[0m\n", kategoriAdi);
    
    for (int i = 0; i < selectedQuiz; i++) {  // Kategoriye ait kelimeleri sırayla listeler
        printf("%s ", quiz_words[i]);        // Kelimeyi ekrana yazar
        if ((i + 1) % 5 == 0)               // Her 5 kelimede bir satır atlar
            printf("\n");
    }
    printf("\n");
}

/* ===================== OYUN ===================== */
int play(int tour) {
    int maxHak = tour + 1;                      // Seviye için maksimum tahmin/can sayısı
    max_lives_for_level = maxHak;               // Seviye bazlı maksimum canı ayarlar
    current_lives = maxHak;                     // Oyuncunun başlangıç canını ayarlar

    select_word();                              // Bu seviye için hedef kelimeyi seçer

    for (int d = 0; d < maxHak; d++) {          // Her tahmin hakkı için döngü
        char input[20];                         // Kullanıcıdan alınan tahmini tutar
        
        while (1) {                             // Geçerli tahmin girilene kadar döner
            display(tour, maxHak);              // Oyun ekranını günceller
            
            if (current_lives <= 0)             // Can kalmadıysa
                return 0;                       // Seviye kaybedildi

            printf("\nTahmin gir: ");           // Kullanıcıdan tahmin ister  
            scanf("%s", input);                 // Tahmini okur

            if (strlen(input) != (size_t)tour) {   // Girilen kelime uzunluğu hatalıysa
                printf("\n\x1b[31m%d harfli kelime gir!\x1b[0m", tour);  // Uyarı verir
                Sleep(700);                                              // Mesajın görünmesi için kısa süre bekler
            } else {
                strcpy(guess[d], input);                    // Geçerli tahmini diziye kopyalar
                break;                                      // Tahmin alma döngüsünden çıkar
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
    FILE* file = NULL;                          // Dosya işaretçisi

    if (category_index == 0) {
        file = fopen("hayvan.txt", "r");          // Hayvanlar kategorisi dosyasını açar
        strcpy(kategoriAdi, "HAYVANLAR");         // Kategori adını ayarlar
    }
    else if (category_index == 1) {
        file = fopen("ulke.txt", "r");
        strcpy(kategoriAdi, "ULKELER");
    }
    else if (category_index == 2) {
        file = fopen("sehir.txt", "r");
        strcpy(kategoriAdi, "SEHIRLER");
    }
    else if (category_index == 3) {
        file = fopen("bitki.txt", "r");
        strcpy(kategoriAdi, "BITKILER");
    }

    if (!file)                              // Dosya açılamazsa
        return 0;                           // Okuma başarısız

    char line[100];                         // Dosyadan okunan satırı tutar
    int i = 0;                              // Okunan geçerli kelime sayısı
    
    while (fgets(line, sizeof(line), file) != NULL) {      // Dosyadan satır satır okur
        line[strcspn(line, "\r\n")] = 0;                  // Satır sonu karakterlerini temizler

        if (strlen(line) == (size_t)tour) {      // Kelime uzunluğu istenen tur uzunluğuna eşitse
            for (int j = 0; j < tour; j++) {     // Kelimenin her harfini
                quiz_words[i][j] = line[j];      // Kelime havuzuna kopyalar
            }
            quiz_words[i][tour] = '\0';        // String sonlandırıcısını ekler
            i++;                              // Okunan kelime sayısını artırır
        }
    }

    selectedQuiz = i;  // Okunan kelime sayısını global değişkende saklar
    fclose(file);      // Açılan dosyayı kapatır
    return i > 0;      // En az bir kelime okunduysa başarılı (1), aksi halde başarısız (0) döndürür
}


void select_word() {
    int r = randnum(0, selectedQuiz - 1);  // Kelime havuzundan rastgele bir indeks seçer
    strcpy(hedefKelime, quiz_words[r]);    // Seçilen kelimeyi hedef kelimeye kopyalar

    for (int i = 0; hedefKelime[i]; i++)           // Hedef kelimenin tüm karakterleri için
        hedefKelime[i] = toupper(hedefKelime[i]);  // Harfleri büyük harfe çevirir
}


int checkRow(int g) {
    int len = strlen(hedefKelime);  // Hedef kelimenin uzunluğunu alır
    int used[10] = {0};             // Hedef kelimedeki kullanılan harfleri işaretler
    int dogru = 0;                  // Doğru yerdeki harf sayısını tutar


    for (int i = 0; i < len; i++)
        guess[g][i] = toupper(guess[g][i]);   // Doğru yerdeki harf sayısını tutar

    for (int i = 0; i < len; i++) {
        if (guess[g][i] == hedefKelime[i]) {    // Harf doğru yerdeyse
            success[g][i] = 1;                  // Doğru yer olarak işaretler
            used[i] = 1;                        // Bu harfi kullanılmış sayar
            dogru++;                             // Doğru harf sayısını artırır
        } else {
            success[g][i] = 0;
        }
    }

    for (int i = 0; i < len; i++) {              // Tahmindeki her harf için
        if (success[g][i])                       // Harf zaten doğru yerdeyse
            continue;                            // Kontrolü atlar

        for (int j = 0; j < len; j++) {          // Hedef kelimedeki harfler arasında arar
            if (!used[j] && guess[g][i] == hedefKelime[j]) { // Harf kelimede var ama farklı yerdeyse
                success[g][i] = 2;               // Yanlış yerde doğru harf olarak işaretler
                used[j] = 1;                     // Aynı harfin tekrar kullanılmasını engeller
                break;                           // Bu harf için aramayı bitirir
            }
        }
    }

    return dogru == len;  // Tüm harfler doğruysa true döndürür
}

void reset_arrays() {
    for (int i = 0; i < 10; i++)                 // Tahmin satırları üzerinde döner
        for (int j = 0; j < 10; j++) {           // Her tahmindeki karakterleri dolaşır
            guess[i][j] = 'x';                   // Tahmin dizisini başlangıç durumuna getirir
            success[i][j] = 0;                   // Harf başarı durumlarını sıfırlar
        }
}
