# Cozzy

Cozzy, bahçesinde malzeme yetiştirdiğin, mutfağında tarifler hazırladığın ve küçük kafende hayvan müşterileri ağırladığın piksel sanat tarzında bir oyundur. C++ ve raylib ile geliştirilmiştir.

<img src="assets/cozzy.png" alt="Cozzy oyun ekranı" width="300"> &nbsp;
<img src="assets/bunny.png" alt="Tavşan müşteri" width="300">

## Özellikler

- Bahçe, tezgâh ve mutfak arasında geçiş
- Ürün ekme, toplama ve envanter yönetimi
- Malzemelerle tarif hazırlama ve hayvan müşterilere servis yapma
- Her servis için 10 bağ puanı kazanma
- Bağ puanı arttıkça açılan, hikâye ve görseller içeren anı defteri
- Mutfakta tarif kitabı ve sahneye göre değişen ses efektleri

## Tarifler

| Malzemeler | Yemek | Müşteri |
|---|---|---|
| Buğday + havuç | Havuçlu kek | Tavşan |
| Buğday + çilek | Granola | Kapibara |
| Buğday + elma | Elmalı turta | Kirpi |
| Buğday + salatalık | Sandviç | Kurbağa |

## Kontroller

| Tuş | İşlev |
|---|---|
| Ok tuşları | Bahçede hareket; tezgâhta sahne değiştirme; mutfakta malzeme seçme |
| `E` | Ürün topla, malzeme seç, sipariş al veya hazır yemeği teslim et |
| `F` | Bahçede seçili boş alana ürün ek |
| `Space` | Seçilen iki malzemeyle tarif hazırla |
| `Q` | Mutfaktaki malzeme seçimlerini temizle |
| `B` | Mutfaktan tezgâha dön |
| `I` | Envanteri aç/kapat |
| `J` | Mutfakta tarif kitabını, diğer sahnelerde anı defterini aç/kapat |
| `Sol` / `Sağ` veya fare tekerleği | Açık kitapta sayfa çevir |
| `Esc` | Açık kitabı kapat |

Sipariş sırası tavşan, kapibara, kurbağa ve kirpidir. Tezgahta `E` ile siparişi al; uygun yemeği hazırladıktan sonra tekrar `E` ile teslim et. Her tamamlanan servis 10 bağ puanı kazandırır. Anı defterinin ilk sayfası oyun başında açıktır; sonraki sayfalar her 10 puanda bir açılır.

## Derleme ve çalıştırma

Gerekenler: CMake 3.15 veya üstü, C++11 destekli bir derleyici ve raylib kaynağını indirmek için internet bağlantısı. Windows'ta MSYS2 UCRT64 derleyicisi kullanılıyorsa, derlemeden önce derleyici araçlarını `PATH`'e ekle:

```powershell
$env:PATH = "C:\msys64\ucrt64\bin;$env:PATH"
cmake -S . -B build
cmake --build build
.\build\cozzy.exe
```

CMake, raylib 5.5'i ilk yapılandırmada indirip derlemeye hazırlar. Oyunu `assets/` klasörünün bulunduğu proje kökünden çalıştır; paketlenmiş sürümde de `assets/` klasörü exe ile aynı klasörde bulunmalıdır.

## Anı defteri hikâyelerini düzenleme

Hikâyeler `assets/hikayeler.txt` dosyasındadır. UTF-8 destekleyen bir metin düzenleyici kullan. Her `[PAGE n]` bölümü bir defter sayfasını belirtir; `TITLE:` başlığı, `LEFT:` fotoğraf altı metnini, `RIGHT:` devam metnini belirler. Sayfa numaralarını ve alan etiketlerini koru. Fotoğraf sırası `main.cpp` içindeki `DEFTER_FOTO_YOLLARI` dizisinde tutulur.

## Proje yapısı

```text
assets/          Oyun görselleri, fontlar, sesler ve hikâye dosyası
main.cpp         Oyun döngüsü, kitaplar, bağ puanı ve ses tetikleme
bahce.cpp/.h     Bahçe sahnesi, hareket, ekim ve ürün toplama
tezgah.cpp/.h    Müşteriler, siparişler ve teslim
mutfak.cpp/.h    Mutfak sahnesi ve tarif hazırlama
oyun_verileri.h  Sahne, müşteri, yemek ve envanter verileri
KULLANIM.txt     Kontroller ve kısa kullanım rehberi
CMakeLists.txt   CMake yapılandırması
```

## Windows demosunu paketleme

Oyunculara dağıtmak için `cozzy.exe`, `assets/` klasörü ve derleyicinin ihtiyaç duyduğu çalışma zamanı DLL'lerini aynı klasöre koyup ZIP olarak paketle. Kaynak kodu derlemek için MSYS2 UCRT64 ortamı gerekir; oyuncuların CMake kurması gerekmez.
