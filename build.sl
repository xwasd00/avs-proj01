#!/bin/bash
#SBATCH --job-name avs_proj01
#SBATCH -p qcpu_exp
#SBATCH -A DD-23-135
#SBATCH -n 1 
#SBATCH -t 0:20:00
#SBATCH --output=%x.ID-%j.out
#SBATCH --error=%x.ID-%j.err

cd $SLURM_SUBMIT_DIR
ml purge
ml Advisor intel-compilers/2022.1.0 CMake/3.23.1-GCCcore-11.3.0 matplotlib

[ -d build ] && rm -rf build
[ -d build ] || mkdir build
cd build

CC=icc CXX=icpc cmake ..
make


#for calc in "ref" "batch" "line"; do
for calc in "line"; do
    rm -rf Advisor-$calc
    mkdir Advisor-$calc

    advixe-cl -collect roofline -project-dir Advisor-$calc  -- ./mandelbrot -c $calc -s 4096

done

rm -rf compare.out *.npz *.png
bash ../scripts/compare.sh 2>&1 > compare.out
for file in *.npz
do
    python3 ../scripts/visualise.py "$file" --save "${file%.*}.png"
done
