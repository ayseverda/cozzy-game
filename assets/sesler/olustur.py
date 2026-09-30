import numpy as np
import wave
from pathlib import Path

SAMPLE_RATE = 44100
OUTPUT_DIR = Path(__file__).resolve().parent

def save_wav(filename, data):
    audio = np.int16(np.clip(data, -1.0, 1.0) * 32767)
    with wave.open(filename, 'w') as f:
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(SAMPLE_RATE)
        f.writeframes(audio.tobytes())

# --- 1. YENİLENMİŞ MEYVE TOPLAMA SESİ: Tok, Çıtır ve Tatlı Bir "Bloop" ---
def generate_better_pop():
    duration = 0.08
    t = np.linspace(0, duration, int(SAMPLE_RATE * duration))
    
    # Çok yükseklerden başlatıp rahatsız etmeyecek şekilde orta-üst bir tondan aşağı kaydırıyoruz
    freq = np.linspace(600, 250, len(t))
    
    # Sinüs ve yumuşak üçgen dalga karışımı (Asla çırtlak değil, tok ve sevimli)
    wave_data = 0.7 * np.sin(2 * np.pi * freq * t) + 0.3 * np.sin(2 * np.pi * (freq * 0.5) * t)
    wave_data *= np.exp(-t * 22) # Hızlıca sönümlenip tok bir vuruş bırakır
    
    save_wav(str(OUTPUT_DIR / "pop_yeni.wav"), wave_data)

# --- 2. YENİLENMİŞ MÜŞTERİ MUTLULUK SESİ: Sevimli ve Klasik Bir Onay ("İki Tonlu Tatlı Ses") ---
def generate_better_happy():
    # Klasik oyunlarda dükkan/görev tamamlandığında çalan o yumuşak "ba-ding!" tonu
    notes = [523.25, 783.99] # C5 ve G5 (Tatlı bir beşli aralık)
    note_duration = 0.1
    pause = 0.015
    
    full_data = []
    for i, freq in enumerate(notes):
        t = np.linspace(0, note_duration, int(SAMPLE_RATE * note_duration))
        # İkinci nota (mutluluk vuruşu) biraz daha canlı ve baskın çıkar
        amplitude = 0.8 if i == 1 else 0.6
        note_wave = amplitude * np.sin(2 * np.pi * freq * t) * np.exp(-t * 10)
        full_data.append(note_wave)
        if i < len(notes) - 1:
            full_data.append(np.zeros(int(SAMPLE_RATE * pause)))
            
    wave_data = np.concatenate(full_data)
    save_wav(str(OUTPUT_DIR / "hihi_yeni.wav"), wave_data)

if __name__ == "__main__":
    generate_better_pop()
    generate_better_happy()
    print("Yeni sesler hazır: pop_yeni.wav ve hihi_yeni.wav")
