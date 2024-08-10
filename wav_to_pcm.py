import wave
import sys

def wav_to_pcm(wav_file, pcm_file):
    # Open the WAV file
    with wave.open(wav_file, 'rb') as wav:
        # Read the WAV file properties
        n_channels = wav.getnchannels()
        sample_width = wav.getsampwidth()
        framerate = wav.getframerate()
        n_frames = wav.getnframes()

        print(f"Channels: {n_channels}")
        print(f"Sample Width: {sample_width} bytes")
        print(f"Frame Rate: {framerate} frames/second")
        print(f"Number of Frames: {n_frames}")

        # Read the audio data
        pcm_data = wav.readframes(n_frames)

        # Write the PCM data to a file
        with open(pcm_file, 'wb') as pcm:
            pcm.write(pcm_data)

        print(f"PCM data has been written to {pcm_file}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python wav_to_pcm.py <input_wav_file> <output_pcm_file>")
        sys.exit(1)

    wav_file = sys.argv[1]
    pcm_file = sys.argv[2]
    wav_to_pcm(wav_file, pcm_file)
