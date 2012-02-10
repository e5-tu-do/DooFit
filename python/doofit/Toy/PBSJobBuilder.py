#!/usr/bin/env python
import os, sys

##@package PBSJobBuilder
#  This simple script can build PBS jobs based on a proto job.
#
#  @section pbsjobbuilder_overview Overview
#
#  Based on a supplied proto job (see examples/toys for an example job) this 
#  script will replace several keywords in this proto job and generate individual
#  job files from this. The user can supply a job base name, job directory for 
#  jobs and logs, number of PBS jobs to run, number of iterations in each PBS
#  job, walltime and number of CPUs to use.
#  
#  Also each iteration can get a unique seed starting at 1.
#
#  @section pbsjobbuilder_protojob The proto job file
#
#  Syntax/Example for a proto job file:
#  
#  @code
##!/bin/sh
#
##PBS -N %(job_name)s
##PBS -o %(out_file)s
##PBS -e %(err_file)s
##PBS -m n
##PBS -M phidomuell@googlemail.com
##PBS -j oe
##PBS -l walltime=%(walltime)s
##PBS -l nodes=1:ppn=%(num_cpu)s
##PBS -l mem=5000mb
#export SCRATCH=/local/$USER/$PBS_JOBID
#
#echo "Auto-generated PBS script"
#echo "Sin2Beta toy studies starting at `date`"
#. SetupProject.sh DaVinci v29r3p1 
#echo $0
#echo $PBS_JOBID > %(log_file)s
#echo ${PBS_JOBID} >> %(log_file)s
#cd %(cwd)s
#
#for i in `seq %(seeds)s`;
#do
#  time echo toy --seed=$i --output=result_%(job_number)s.root --num_cpu=%(num_cpu)s >> %(log_file)s
#done
#
#echo finished job number %(job_number)s
#@endcode
#
#The following keywords are supported:
#
#@li @c job_name: the individual job's name
#@li @c out_file: the stdout log file for the individual job
#@li @c err_file: the stderr log file for the individual job
#@li @c log_file: the log file for the individual job
#@li @c walltime: the walltime for the job
#@li @c num_cpu:  the number of CPUs to use
#@li @c seeds:    the seeds for the iterations in this individual job to use (will be generated as <tt>minseed maxseed</tt> compatible to the above @c for statement)
#@li @c job_number: the number of the individual job
#
#@section pbsjobbuilder_calling Calling PBSJobBuilder.py
#
#Call PBSJobBuilder.py like this:
#
#  @code 
#PBSJobBuilder.py proto_script job_base_name jobs_dir num_pbs_jobs num_iterations_per_job walltime num_cpu
#  @endcode
# 
#  Afterwards, a submit script <tt>submit_job_base_name.sh</tt> will be created 
#  inside the jobs directory to easily submit all jobs.
#

## Generate one job file
#
def create_single_job(proto_script, settings_dict, jobs_dir, num_iterations, min_seed):
  settings_dict['seeds'] = str(min_seed) +  ' ' + str(min_seed+num_iterations-1)
  file = open(proto_script)
  script = file.read() % settings_dict
  file_write = open(os.path.join(jobs_dir,settings_dict['job_name']+'.sh'), 'w')
  file_write.write(script)
  return min_seed + num_iterations

## Generate all job files and the submit script
#
def create_jobs(proto_script, job_base_name, jobs_dir, num_jobs, num_iterations_per_job, walltime, num_cpu):
  min_seed = 1
  jobs_dir = os.path.realpath(os.path.abspath(os.path.expanduser(jobs_dir)))
  submit_file_name = os.path.join(jobs_dir,'submit_' + job_base_name + '.sh')
  submit_file = open(submit_file_name, 'w')
  submit_file.writelines('#!/bin/sh\n')
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
    submit_file.writelines('qsub ' + os.path.join(jobs_dir,settings_dict['job_name']+'.sh\n'))
  print 'sh ' + submit_file_name

if __name__ == "__main__":
  if len(sys.argv) < 8:
    print 'Usage: ' + sys.argv[0] + ' proto_script job_base_name jobs_dir num_pbs_jobs num_iterations_per_job walltime num_cpu'
  create_jobs(sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4]), int(sys.argv[5]), sys.argv[6], int(sys.argv[7]))