#include "../lib/utils.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

// Bir metnin (str) belirli bir son ek (suffix) ile bitip bitmediğini kontrol eden fonksiyon
int ends_with(const char *str, const char *suffix)
{
    // Eğer ana metin veya son ek NULL (geçersiz) ise 0 (yanlış) döndür
    if (!str || !suffix)
        return 0;

    // Metnin ve son ekin karakter uzunluklarını hesapla
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);

    // Eğer aranan son ekin uzunluğu, ana metinden daha büyükse metin bu ekle bitemez
    if (lensuffix > lenstr)
        return 0;

    // Ana metnin sonundan, son ekin uzunluğu kadar geriye git ve o kısmı son ek ile karşılaştır
    // Karşılaştırma sonucu eşitse (strncmp 0 dönerse) 1 (doğru), aksi halde 0 (yanlış) döndür
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

// Belirtilen dosyanın içeriğine bakarak sadece metin karakterlerinden oluşup oluşmadığını kontrol eden fonksiyon
int is_text_file(const char *filename)
{
    // Dosyayı ikili (binary) okuma modunda aç
    FILE *fp = fopen(filename, "rb");

    // Dosya bulunamaz veya açılamazsa 0 (yanlış) döndür
    if (!fp)
        return 0;

    int c;

    // Dosyanın sonuna (EOF - End Of File) gelene kadar karakter karakter (byte byte) oku
    while ((c = fgetc(fp)) != EOF)
    {
        // Okunan karakter standart yazdırılabilir ASCII aralığında (32 ile 126 arası) değilse,
        // ve aynı zamanda satır sonu (\n), satır başı (\r) veya sekme (\t) karakteri de değilse:
        if ((c < 32 || c > 126) && c != '\n' && c != '\r' && c != '\t')
        {
            // Bu dosyanın içerisinde okunamaz/binary veri vardır (metin dosyası değildir)
            fclose(fp); // Dosyayı kapat
            return 0;   // 0 (yanlış) döndür
        }
    }

    // Dosyanın tamamı tarandı ve hiçbir geçersiz karakter bulunmadı
    fclose(fp); // Dosyayı kapat
    return 1;   // 1 (doğru / metin dosyası) döndür
}