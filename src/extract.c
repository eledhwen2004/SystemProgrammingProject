#include "../lib/extract.h"
#include "../lib/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// Belirtilen arşiv dosyasının içerisindeki dosyaları dışarı çıkartan (ayıklayan) fonksiyon
int extract_archive(const char *archive_name, const char *directory)
{
    // Arşiv dosyasının uzantısının ".sau" olup olmadığını kontrol et
    if (!ends_with(archive_name, ".sau"))
    {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        return 1;
    }

    // Arşiv dosyasını ikili (binary) okuma modunda aç
    FILE *fp = fopen(archive_name, "rb");

    // Dosya açılamadıysa hata ver ve çık
    if (!fp)
    {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        return 1;
    }

    // Başlık boyutunu okumak için 10 karakterlik (+1 null) bir dizi oluştur
    char size_str[11];
    size_str[10] = '\0'; // String bitiş karakteri

    // Arşiv dosyasının en başından 10 bayt okuyarak metadata uzunluğunu al
    fread(size_str, 1, 10, fp);

    // Okunan string formatındaki boyutu tam sayıya (integer) dönüştür
    int header_size = atoi(size_str);

    // Başlık boyutunun geçerli (mantıklı) sınırlar içinde olup olmadığını kontrol et
    if (header_size <= 0 || header_size >= MAX_HEADER)
    {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        fclose(fp);
        return 1;
    }

    // Başlık metnini (metadata) hafızada tutmak için dinamik bellek ayır
    char *header = malloc(header_size + 1);

    // Bellek ayrılamadıysa işlemi iptal et
    if (!header)
    {
        fclose(fp);
        return 1;
    }

    // Dosyadaki başlık bilgilerini ayrılan belleğe oku
    fread(header, 1, header_size, fp);
    header[header_size] = '\0'; // Güvenlik için string'i sonlandır

    // Eğer dosyaların çıkartılacağı bir hedef klasör belirtilmişse
    if (directory)
    {
        // Hedef klasörü oluştur (0777: tüm okuma, yazma ve çalıştırma izinleriyle)
        mkdir(directory, 0777);
    }

    // Başlık bilgisini '|' karakterine göre parçalayarak ilk dosya bilgisine ulaş
    char *token = strtok(header, "|");

    // Ayrıştırılacak dosya bilgisi (token) bulunduğu sürece döngüye devam et
    while (token)
    {
        char filename[MAX_NAME]; // Dosya adını tutacak dizi
        int permissions;         // Dosyanın erişim izinleri (octal formatta)
        long filesize;           // Dosyanın boyutu

        // Parçalanan metinden dosya adını, izinleri ve boyutu değişkenlere ata
        // Format: "dosyadi.txt,izinler,boyut"
        if (sscanf(token, "%[^,],%o,%ld",
                   filename,
                   &permissions,
                   &filesize) != 3)
        {
            // Format beklenenden farklıysa belleği temizle ve hata ver
            printf("Arşiv dosyası uygunsuz veya bozuk!\n");

            free(header);
            fclose(fp);

            return 1;
        }

        char path[512];

        // Eğer hedef klasör belirtilmişse, dosya yolunu klasörle birleştir (Örn: klasor/dosya.txt)
        if (directory)
            sprintf(path, "%s/%s", directory, filename);
        else // Hedef klasör yoksa dosyayı doğrudan mevcut konuma çıkar
            sprintf(path, "%s", filename);

        // Çıkartılacak yeni dosyayı ikili (binary) yazma modunda aç
        FILE *out = fopen(path, "wb");

        // Dosya oluşturulamazsa işlemi iptal et ve temizlik yap
        if (!out)
        {
            perror(path);

            free(header);
            fclose(fp);

            return 1;
        }

        char buffer[1024]; // Veri kopyalama işlemi için tampon bellek
        long remaining = filesize; // Kopyalanması gereken kalan dosya boyutu

        // Dosyanın tamamı yazılana kadar kopyalama döngüsüne devam et
        while (remaining > 0)
        {
            // Kalan boyut buffer'dan büyükse buffer kadar, küçükse kalan miktar kadar okuma yap
            int chunk = remaining > sizeof(buffer)
                            ? sizeof(buffer)
                            : remaining;

            // Arşiv dosyasından veri oku
            fread(buffer, 1, chunk, fp);
            // Okunan veriyi yeni çıkartılan dosyaya yaz
            fwrite(buffer, 1, chunk, out);

            // Kalan boyutu eksilt
            remaining -= chunk;
        }

        // Yeni oluşturulan dosya ile işimiz bitti, kapat
        fclose(out);

        // Çıkartılan dosyaya, arşivde kayıtlı olan orijinal erişim yetkilerini uygula
        chmod(path, permissions);

        // Bir sonraki dosyanın başlık bilgisine geçmek için parçalamaya devam et
        token = strtok(NULL, "|");
    }

    // Başlık bilgisi için ayrılan dinamik belleği serbest bırak
    free(header);

    // Arşiv dosyasını kapat
    fclose(fp);

    /*
        Arşiv başarıyla açıldıktan sonra
        .sau dosyasını sil
    */
    if (remove(archive_name) == 0)
    {
        printf("%s arşiv dosyası silindi.\n", archive_name);
    }
    else
    {
        printf("%s arşiv dosyası silinemedi.\n", archive_name);
    }

    // İşlemin başarıyla tamamlandığını kullanıcıya bildir
    printf("Dosyalar başarıyla açıldı.\n");

    return 0; // Başarı kodu döndür
}