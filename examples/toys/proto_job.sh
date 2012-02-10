#!/bin/sh

#PBS -N %(job_name)s
#PBS -o %(out_file)s
#PBS -e %(err_file)s
#PBS -m n
#PBS -M phidomuell@googlemail.com
#PBS -j oe
#PBS -l walltime=%(walltime)s
#PBS -l nodes=1:ppn=%(num_cpu)s
#PBS -l mem=5000mb
export SCRATCH=/local/$USER/$PBS_JOBID

echo "Auto-generated PBS script"
echo "DooFit toy studies starting at `date`"
. SetupProject.sh DaVinci v29r3p1 
module add gcc
echo $0
echo $PBS_JOBID > %(log_file)s
echo ${PBS_JOBID} >> %(log_file)s
cd %(cwd)s

for i in `seq %(seeds)s`;
do
  time ./bin/Jpsif0DooFitToy -c config.cfg --toyfac.random_seed=$i >> %(log_file)s
done

echo finished job number %(job_number)s
