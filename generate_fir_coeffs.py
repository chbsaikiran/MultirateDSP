import numpy as np
import scipy.signal as signal
import sys

def float_to_fixed(coeffs, q_factor):
    fixed_coeffs = []
    for coeff in coeffs:
        fixed_val = int(round(coeff * (1 << q_factor)))  # Scale by 2^Q and round
        fixed_coeffs.append(fixed_val)
    return fixed_coeffs

def generate_fir_coeffs(sampling_rate, cutoff_freq, num_taps, q_factor, filename="fir_coeffs.h"):
    # Normalize the cutoff frequency with respect to the Nyquist frequency
    nyquist_rate = sampling_rate / 2.0
    normalized_cutoff = cutoff_freq / nyquist_rate

    # Generate FIR filter coefficients using the window method
    coeffs = signal.firwin(num_taps, normalized_cutoff, window='hamming')

    # Convert the floating-point coefficients to fixed-point
    fixed_coeffs = float_to_fixed(coeffs, q_factor)

    # Write the fixed-point coefficients to a .h file
    with open(filename, 'w') as f:
        f.write("#ifndef FIR_COEFFS_H\n")
        f.write("#define FIR_COEFFS_H\n\n")
        f.write(f"// FIR filter coefficients generated for\n")
        f.write(f"// Sampling Rate: {sampling_rate} Hz\n")
        f.write(f"// Cutoff Frequency: {cutoff_freq} Hz\n")
        f.write(f"// Number of Taps: {num_taps}\n")
        f.write(f"// Q Factor: {q_factor}\n\n")
        f.write(f"#define NUM_TAPS {num_taps}\n\n")
        f.write("int32_t fir_coeffs[NUM_TAPS] = {\n")
        for i, coeff in enumerate(fixed_coeffs):
            f.write(f"    {coeff}")
            if i < len(fixed_coeffs) - 1:
                f.write(",\n")
            else:
                f.write("\n")
        f.write("};\n\n")
        f.write("#endif // FIR_COEFFS_H\n")

    print(f"Fixed-point FIR coefficients have been written to {filename}")

if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage: python generate_fir_coeffs_fixed_point.py <sampling_rate> <cutoff_freq> <num_taps> <q_factor>")
        sys.exit(1)

    sampling_rate = float(sys.argv[1])
    cutoff_freq = float(sys.argv[2])
    num_taps = int(sys.argv[3])
    q_factor = int(sys.argv[4])
    generate_fir_coeffs(sampling_rate, cutoff_freq, num_taps, q_factor)
