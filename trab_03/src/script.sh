#!/bin/bash
# Command to use the count registers inside the processor.
sudo modprobe msr
# Command to compile with the necessary flags to use the tools of likwid.
gcc -O3 -mavx -march=native main.c SistemasLineares.c matriz.c utils.c -o main -lm -llikwid -DLIKWID_PERFMON

# Cleaning up files.
> L3.dat
> L2CACHE.dat
> FLOPS_DP.dat

array=(32 50 64 100 128 200 256 300 400 512 1000 1024 2000 2048 3000 4000 4096 5000 10000)

# [L3] Loop to iterate different matrix sizes.
for i in ${array[*]}
do
    VAR1=$(likwid-perfctr -C 3 -m -g L3 ./main 25 | grep "L3 bandwidth \[MBytes/s\]"  -m 1 | grep -Po "\d+\.\d+")
    VAR2="$i ${VAR1}"
    echo "$VAR2" >> L3.dat
done

# [L2CACHE] Loop to iterate different matrix sizes.
for i in ${array[*]}
do
    VAR1=$(likwid-perfctr -C 3 -m -g L2CACHE ./main $i | grep "L2 miss ratio"  -m 1 | grep -Po "\d+\.\d+")
    VAR2="$i ${VAR1}"
    echo "$VAR2" >> L2CACHE.dat
done

# Cleaning up files.
> multMatPtrVetTimeReport.dat
> multMatRowVetTimeReport.dat
> multMatColVetTimeReport.dat
> normaMaxTimeReport.dat
> normaEuclTimeReport.dat

# [FLOPS_DP] Loop to iterate different matrix sizes.
for i in ${array[*]}
do
    VAR1=$(likwid-perfctr -C 3 -m -g FLOPS_DP ./main $i | grep "DP MFLOP/s" -m 1 | grep -Po "\d+(\.\d+)?")
    VAR2="$i ${VAR1}"
    echo "$VAR2" >> FLOPS_DP.dat
done
