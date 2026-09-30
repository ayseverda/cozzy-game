import math
import struct
import wave
from pathlib import Path

SAMPLE_RATE = 22050
BUZZER_FREQUENCY = 2200
VOLUME = 0.22
OUTPUT_DIR = Path(__file__).resolve().parent


def make_beeps(on_times, gap=0.045):
    """Make same-pitch buzzer pulses; on_times are pulse lengths in seconds."""
    samples = []
    for pulse_index, duration in enumerate(on_times):
        count = int(SAMPLE_RATE * duration)
        ramp = max(1, int(SAMPLE_RATE * 0.002))
        for i in range(count):
            envelope = min(1.0, i / ramp, (count - 1 - i) / ramp)
            wave_value = 1.0 if math.sin(2 * math.pi * BUZZER_FREQUENCY * i / SAMPLE_RATE) >= 0 else -1.0
            samples.append(wave_value * VOLUME * max(0.0, envelope))
        if pulse_index + 1 < len(on_times):
            samples.extend([0.0] * int(SAMPLE_RATE * gap))
    return samples


def save_wav(filename, samples):
    pcm = bytearray()
    for sample in samples:
        sample = max(-1.0, min(1.0, sample))
        pcm.extend(struct.pack("<h", int(sample * 32767)))
    with wave.open(str(OUTPUT_DIR / filename), "wb") as wav:
        wav.setnchannels(1)
        wav.setsampwidth(2)
        wav.setframerate(SAMPLE_RATE)
        wav.writeframes(pcm)


if __name__ == "__main__":
    # Meyve toplama: iki minik pit.
    save_wav("pop_yeni.wav", make_beeps([0.055, 0.075], gap=0.055))
    # Yemek hazirlama: diririm ritmi, son bip daha uzun.
    save_wav("hihi_yeni.wav", make_beeps([0.055, 0.055, 0.055, 0.16], gap=0.04))
    # Teslim: iki daha uzun, memnuniyet bildirimi.
    save_wav("classic_diririm.wav", make_beeps([0.10, 0.13], gap=0.07))
    print("Buzzer tarzinda WAV sesleri olusturuldu.")
