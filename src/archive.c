#include "../lib/archive.h"
#include "../lib/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h> 

// Belirtilen dosyaları tek bir arşiv dosyasında birleştiren ana fonksiyon
int create_archive(int file_count, char *files[], const char *output, const char *directory)
{
    // Maksimum dosya sayısı sınırını kontrol et
    if (file_count > MAX_FILES)
    {
        printf("En fazla 32 dosya desteklenir!\n");
        return 1;
    }

    long total_size = 0; // Toplam arşiv boyutunu takip etmek için değişken
    char header[MAX_HEADER] = ""; // Arşivin başlık (metadata) bilgisini tutacak dizi

    // Girilen tüm dosyalar için geçerlilik kontrolü ve başlık oluşturma döngüsü
    for (int i = 0; i < file_count; i++)
    {
        // Dosyanın bir metin dosyası olup olmadığını kontrol et
        if (!is_text_file(files[i]))
        {
            printf("%s giriş dosyasının formatı uyumsuzdur!\n", files[i]);
            return 1;
        }

        struct stat st;

        // Dosyanın bilgilerine (boyut, izinler vb.) erişmeye çalış
        if (stat(files[i], &st) != 0)
        {
            perror(files[i]);
            return 1;
        }

        // Dosya boyutunu toplam boyuta ekle
        total_size += st.st_size;

        // Toplam boyutun belirlenen maksimum sınırı aşıp aşmadığını kontrol et
        if (total_size > MAX_TOTAL_SIZE)
        {
            printf("Toplam boyut 200 MB sınırını geçti!\n");
            return 1;
        }

        char temp[512];

        // Dosya adını, izinlerini (octal formatta) ve boyutunu başlık formatına uygun bir şekilde hazırla
        sprintf(temp, "|%s,%o,%ld|",
                files[i],
                st.st_mode & 0777, // Sadece yetki bitlerini (rwx) al
                st.st_size);

        // Hazırlanan bu dosya bilgisini ana başlık dizisine ekle
        strcat(header, temp);
    }

    // Başlık metninin toplam uzunluğunu hesapla
    int header_size = strlen(header);

    // --- YENİ DOSYA YOLU OLUŞTURMA VE KLASÖR KONTROLÜ MANTIĞI ---
    char final_output_path[1024];

    // Eğer hedef bir klasör belirtilmişse
    if (directory != NULL && strlen(directory) > 0)
    {
        struct stat st_dir = {0};
        
        // Belirtilen klasör mevcut değilse oluştur
        if (stat(directory, &st_dir) == -1) 
        {
            // 0777 izni: Okuma, yazma ve çalıştırma yetkileri tanımlanır
            if (mkdir(directory, 0777) != 0) 
            {
                perror("Hedef dizin oluşturulamadı");
                return 1;
            }
        }

        // Klasör adını ve dosya adını birleştirir (Örn: m1/arsiv.sau)
        snprintf(final_output_path, sizeof(final_output_path), "%s/%s", directory, output);
    }
    else
    {
        // Eğer directory girilmediyse doğrudan çıktı adını kullanır (Örn: arsiv.sau)
        snprintf(final_output_path, sizeof(final_output_path), "%s", output);
    }
    // ------------------------------------------------------------

    // Artık dinamik olarak hazırlanan final_output_path hedef alınarak yazma yapılıyor
    // Arşiv dosyasını binary (ikili) yazma modunda aç
    FILE *out = fopen(final_output_path, "wb");

    // Dosya açılamadıysa hata ver
    if (!out)
    {
        perror(final_output_path);
        return 1;
    }

    // İlk olarak başlığın boyutunu 10 haneli olacak şekilde dosyanın en başına yaz
    fprintf(out, "%010d", header_size);
    // Ardından oluşturulan metadata başlığını dosyaya yaz
    fwrite(header, 1, header_size, out);

    char buffer[1024]; // Dosya okuma/yazma işlemleri için tampon bellek

    // Dosyaların içeriklerini arşive yazmak için döngü
    for (int i = 0; i < file_count; i++)
    {
        // Okunacak girdi dosyasını binary modda aç
        FILE *in = fopen(files[i], "rb");

        // Okunacak dosya açılamazsa işlemi iptal et ve çıkış dosyasını kapat
        if (!in)
        {
            perror(files[i]);
            fclose(out);
            return 1;
        }

        size_t bytes;

        // Dosya sonuna gelene kadar parça parça oku ve arşiv dosyasına yaz
        while ((bytes = fread(buffer, 1, sizeof(buffer), in)) > 0)
        {
            fwrite(buffer, 1, bytes, out);
        }

        // Okuması biten girdi dosyasını kapat
        fclose(in);
    }

    // Tüm yazma işlemleri bittikten sonra arşiv dosyasını kapat
    fclose(out);

    // İşlemin başarıyla tamamlandığını kullanıcıya bildir
    printf("Dosyalar birleştirildi ve '%s' konumuna kaydedildi.\n", final_output_path);

    // Başarılı çıkış kodu döndür
    return 0;
}