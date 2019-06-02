#!/bin/sh
# Command to use the count registers inside the processor.
sudo modprobe msr
# Command to compile with the necessary flags to use the tools of likwid.
gcc -O3 -mavx -march=native main.c SistemasLineares.c matriz.c utils.c -o main -lm -llikwid -DLIKWID_PERFMON

# Cleaning up the output file.
> out.txt

# Loop to iterate different matrix sizes.
for i in 32 50 64
do
    likwid-perfctr -C 3 -m -g FLOPS_DP ./main $i | grep "Timestamp:" -m 1 | grep -Po "\d+(\.\d+)?" >> timestampDatax.dat
done



plot "timestampData.dat" using 1:2 with lines,\
"timestampData.dat" using 1:3 with lines,\
"timestampData.dat" using 1:4 with lines




    # likwid-perfctr -C 3 -m -g FLOPS_DP ./main $i | grep "DP MFLOP/s" -m 1 | grep -Po "\d+(\.\d+)?"
