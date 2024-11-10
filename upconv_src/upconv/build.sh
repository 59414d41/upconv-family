#!/bin/sh

gcc -static -Wl,--stack,10485760 -O2 -msse2 -fopenmp -ftree-vectorize -o upconv_no_log.exe upconv.c fft_filter.c fileio.c mconv.c wav2raw.c start_exec.c bit_extend_table.c declip_eq_init.c hfa_eq_init.c ./../PLG_AUDIO_IO/PLG_AUDIO_IO.c -lfftw3-3 -liconv -lgsl -lgslcblas -D_FILE_OFFSET_BITS=64

gcc -static -Wl,--stack,10485760 -O2 -msse2 -fopenmp -ftree-vectorize -o upconv_log.exe upconv.c fft_filter.c fileio.c mconv.c wav2raw.c start_exec.c bit_extend_table.c declip_eq_init.c hfa_eq_init.c ./../PLG_AUDIO_IO/PLG_AUDIO_IO.c -lfftw3-3 -liconv -lgsl -lgslcblas -D_FILE_OFFSET_BITS=64 -DUPCONV_LOG


