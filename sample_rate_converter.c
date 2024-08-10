#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include "fir_coeffs.h"

#define INPUT_SIZE 160
#define UPSAMPLE_FACTOR 2
#define DOWNSAMPLE_FACTOR 1

#define ABS_FLOAT(x) ((x) > 0 ? (x):(0-(x)))

#define Word64 long long
#define Word32 int
#define Word16 short

Word64 s64_mul_s32_s32(Word32 x, Word32 y)
{
    Word64 prod;

    prod = ((Word64)x)*((Word64)y);

    return prod;
}

Word64 s64_mla_s32_s32(Word64 sum,Word32 x, Word32 y)
{
    Word64 prod;

    prod = ((Word64)x)*((Word64)y);

    sum = sum + prod;

    return sum;
}

Word32 float_to_fixed_conv(float x, Word32 qfactor)
{
    return ((Word32)(x*(pow(2,qfactor))));
}

Word16 float_to_fixed_conv_16bit(float x, Word32 qfactor)
{
    return ((Word16)(x*(pow(2,qfactor))));
}

float fixed_to_float_conv(Word32 x, Word32 qfactor)
{
    return (((float)x)/((float)(pow(2,qfactor))));
}

Word32 delay_line[NUM_TAPS+INPUT_SIZE*UPSAMPLE_FACTOR-1];

void fir_filter_fxd_pt(Word32* in, Word32* coeffs, Word32* out,Word32 num_of_filt_coeffs, Word32 frame_size)
{
    Word64 acc;     // accumulator for MACs
    Word32 *coeffp; // pointer to coefficients
    Word32 *inputp; // pointer to input samples
    Word32 n;
    Word32 k;
 
    // put the new samples at the high end of the buffer
    memcpy( &delay_line[num_of_filt_coeffs - 1], in,
            frame_size * sizeof(int) );
 
    // apply the filter to each input sample
    for ( n = 0; n < frame_size; n++ ) 
    {
        // calculate out n
        coeffp = coeffs;
        inputp = &delay_line[num_of_filt_coeffs - 1 + n];
        acc = 0;
        for ( k = 0; k < num_of_filt_coeffs; k++ ) 
        {
            acc = s64_mla_s32_s32(acc, (*coeffp++), (*inputp--));
        }
        out[n] = (Word32)(acc >> 31);
    }
    // shift input samples back in time for next time
    memmove( &delay_line[0], &delay_line[frame_size],
            (num_of_filt_coeffs - 1) * sizeof(int));
 
}
// Function to upsample the input signal
void upsample(const int *input, int *output, int input_size, int up_factor) {
    for (int i = 0; i < input_size; i++) {
        output[i * up_factor] = input[i];
        for (int j = 1; j < up_factor; j++) {
            output[i * up_factor + j] = 0;  // Insert zeros between samples
        }
    }
}

// Function to downsample the input signal
void downsample(const int *input, int *output, int input_size, int down_factor) {
    for (int i = 0; i < input_size; i += down_factor) {
        output[i / down_factor] = input[i];
    }
}

int main() {
    // Example input signal
    short input_st[INPUT_SIZE];
    int input[INPUT_SIZE],i,j;
    FILE *fin,*fout;
    int upsampled_size = INPUT_SIZE * UPSAMPLE_FACTOR;
    int *upsampled = (int *)calloc(upsampled_size, sizeof(int));
    int *filtered = (int *)calloc(upsampled_size, sizeof(int));
    int downsampled_size = upsampled_size / DOWNSAMPLE_FACTOR;
    int *output = (int *)calloc(downsampled_size, sizeof(int));

    fin = fopen("..\\16khz_speech.pcm","rb");
    fout = fopen("..\\32khz_speech.pcm","wb");

    for(i = 0; i < 340; i++)
    {
        fread(input_st, INPUT_SIZE, sizeof(short), fin);
        for(j = 0; j < INPUT_SIZE; j++)
        {
            input[j] = (((int)(input_st[j])) << 16);
        }
        // Upsample the input signal
        upsample(input, upsampled, INPUT_SIZE, UPSAMPLE_FACTOR);

        // Apply FIR filter after upsampling
        fir_filter_fxd_pt(upsampled, fir_coeffs,filtered, NUM_TAPS,upsampled_size);

        // Downsample the filtered signal
        downsample(filtered, output, upsampled_size, DOWNSAMPLE_FACTOR);

        fwrite(output,downsampled_size,sizeof(int),fout);
    }

    // Clean up
    free(upsampled);
    free(filtered);
    free(output);
    fclose(fin);
    fclose(fout);

    return 0;
}
