#!/usr/bin/env python
import os, sys

##@package PBSJobBuilder
#  This simple script can build PBS jobs based on a proto job.
#  
#  Based on a supplied proto job (see examples/toys for an example job) this 
#  script will replace several keywords in this proto job and generate individual
#  job files from this. The user can supply a job base name, job directory for 
#  jobs and logs, number of PBS jobs to run, number of iterations in each PBS
#  job, walltime and number of CPUs to use.
#  
#  Also each iteration can get a unique seed starting at 1.
#  
#  Syntax/Example for keywords in the proto job file:
#  
#  @code
#  #!/bin/sh
#  
#  #PBS -N %(job_name)s
#  #PBS -o %(out_file)s
#  #PBS -e %(err_file)s
#  #PBS -m n
#  #PBS -M phidomuell@googlemail.com
#  #PBS -j oe
#  #PBS -l walltime=%(walltime)s
#  #PBS -l nodes=1:ppn=%(num_cpu)s
#  #PBS -l mem=5000mb
#  export SCRATCH=/local/$USER/$PBS_JOBID
#  
#  echo "Auto-generated PBS script"
#  echo "Sin2Beta toy studies starting at `date`"
#  . SetupProject.sh DaVinci v29r3p1 
#  echo $0
#  echo $PBS_JOBID > %(log_file)s
#  echo ${PBS_JOBID} >> %(log_file)s
#  cd %(cwd)s
#  
#  for i in `seq %(seeds)s`;
#  do
#  time echo toy --seed=$i --output=result_%(job_number)s.root --num_cpu=%(num_cpu)s >> %(log_file)s
#  done
#  
#  echo finished job number %(job_number)s
#  @endcode
#  
#

def create_single_job(proto_script, settings_dict, jobs_dir, num_iterations, min_seed):
  settings_dict['seeds'] = str(min_seed) +  ' ' + str(min_seed+num_iterations-1)
  file = open(proto_script)
  script = file.read() % settings_dict
  file_write = open(os.path.join(jobs_dir,settings_dict['job_name']+'.sh'), 'w')
  file_write.write(script)
  return min_seed + num_iterations

def create_jobs(proto_script, job_base_name, jobs_dir, num_jobs, num_iterations_per_job, walltime, num_cpu):
  min_seed = 1
  jobs_dir = os.path.realpath(os.path.abspath(os.path.expanduser(jobs_dir)))
  for i in range(0,num_jobs):
    settings_dict = {
      'job_name'   : job_base_name + '_' + str(i),
      'out_file'   : os.path.join(jobs_dir,'o_' + job_base_name + '_' + str(i) + '.log'),
      'err_file'   : os.path.join(jobs_dir,'e_' + job_base_name + '_' + str(i) + '.log'),
      'log_file'   : os.path.join(jobs_dir,'l_' + job_base_name + '_' + str(i) + '.log'),
      'walltime'   : walltime,
      'num_cpu'    : str(num_cpu),
      'job_number' : str(i),
      'cwd'        : os.getcwd()
      }
    min_seed = create_single_job(proto_script, settings_dict, jobs_dir, num_iterations_per_job, min_seed)