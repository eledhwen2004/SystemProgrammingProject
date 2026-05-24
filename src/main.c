#include <stdio.h>
#include <string.h>

#include "../lib/archive.h"
#include "../lib/extract.h"

// Programın ana giriş noktası. Komut satırı argümanlarını işler ve ilgili işlemleri yönlendirir.
int main(int argc, char *argv[])
{
    // Yeterli sayıda argüman girilip girilmediğini kontrol et
    if (argc < 2)
    {
        // Kullanıcıya programın doğru kullanım şeklini göster
        printf("Kullanım:\n");  
        printf("tarsau -b file1 file2 ... -o archive.sau\n");
        printf("tarsau -a archive.sau [directory]\n");
        return 1; // Hatalı kullanım durumunda programdan çık
    }
    
    // Eğer ilk argüman "-b" ise arşiv oluşturma (birleştirme) işlemini başlat
    if (strcmp(argv[1], "-b") == 0)
    {
        char *directory = "";     // Hedef klasör (varsayılan olarak boş)
        char *output = "a.sau";   // Çıktı dosyasının varsayılan adı
        char *files[32];          // Birleştirilecek girdi dosyalarını tutacak dizi
        int file_count = 0;       // Girdi dosyalarının sayacı

        // 2. indisten başlayarak girilen tüm argümanları kontrol et
        for (int i = 2; i < argc; i++)
        {
            // Eğer "-o" parametresi ile karşılaşılırsa
            if (strcmp(argv[i], "-o") == 0)
            {
                // Çıktı dosyasının adı girilmiş mi diye kontrol et
                if (i + 1 < argc)
                {
                    output = argv[i + 1]; // "-o" dan sonraki argümanı çıktı dosyası adı olarak belirle
                    
                    // Eğer çıktı dosyasından sonra bir argüman daha varsa onu hedef dizin olarak kabul et
                    if(i+2 < argc){
                        directory = argv[i + 2];
                    }
                    break; // "-o" parametresi ve sonrasındaki argümanlar işlendikten sonra döngüden çık
                }
            }
            else
            {
                // Eğer "-o" değilse, argümanı bir girdi dosyası olarak kabul et ve listeye ekle
                files[file_count++] = argv[i];
            }
        }

        // Toplanan argümanlarla arşiv oluşturma fonksiyonunu çağır ve sonucunu döndür
        return create_archive(file_count, files, output, directory);
    }

    // Eğer ilk argüman "-a" ise arşivden çıkarma (ayıklama) işlemini başlat
    else if (strcmp(argv[1], "-a") == 0)
    {
        // Açılacak arşiv dosyasının adının girilip girilmediğini kontrol et
        if (argc < 3)
        {
            printf("Arşiv dosyası belirtilmedi!\n");
            return 1; // Eksik argüman varsa hata kodu ile çık
        }

        const char *archive = argv[2]; // Açılacak olan arşiv dosyası
        const char *directory = NULL;  // Dosyaların çıkarılacağı hedef dizin (varsayılan olarak boş)

        // Eğer ekstra bir argüman girilmişse onu hedef dizin olarak al
        if (argc >= 4)
            directory = argv[3];

        // Arşivden çıkarma fonksiyonunu çağır ve sonucunu döndür
        return extract_archive(archive, directory);
    }

    // Geçersiz bir parametre girildiyse (ne "-b" ne de "-a")
    else
    {
        printf("Geçersiz parametre!\n");
        return 1; // Hata kodu ile çık
    }
}