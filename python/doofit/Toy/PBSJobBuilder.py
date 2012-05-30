#!/usr/bin/env python
import os, sys
from optparse import OptionParser

##@package PBSJobBuilder
#  This simple script can build PBS jobs based on a proto job.
#
#  @section pbsjobbuilder_overview Overview
#
#  Based on a supplied proto job (see examples/toys/proto_job.sh for an example 
#  job) this script will replace several keywords in this proto job and generate 
#  individual job files from this. The user can supply a job base name, job 
#  directory for jobs and logs, number of PBS jobs to run, number of iterations 
#  in each PBS job, walltime and number of CPUs to use.
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
#  time echo toy --scanstart=%(scan1_start)s --scanend=%(scan1_end)s --scan2start=%(scan2_start)s --scan2end=%(scan2_end)s --seed=$i --output=%(jobs_dir)s/result_%(job_number)s.root --num_cpu=%(num_cpu)s >> %(log_file)s
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
#@li @c scan1_start: the current scan value 1 starting point
#@li @c scan1_end: the current scan value 1 ending point
#@li @c scan2_start: the current scan value 2 starting point
#@li @c scan2_end: the current scan value 2 ending point
#@li @c jobs_dir:   the job directory
#@li @c parametern: additional arbitrary parameter n (as configured by <tt>-n</tt> or <tt>--parameter-n</tt>)
#
#@section pbsjobbuilder_calling Calling PBSJobBuilder.py
#
#Call PBSJobBuilder.py like this:
#
#  @code 
#PBSJobBuilder.py proto_script job_base_name jobs_dir num_pbs_jobs num_iterations_per_job walltime num_cpu min_seed
#  @endcode
#
#  Additional options are available via the <tt>--help</tt> option.
# 
#  Afterwards, a submit script <tt>submit_job_base_name.sh</tt> will be created 
#  inside the jobs directory to easily submit all jobs. <tt>num_pbs_jobs</tt> 
#  will be the number of PBS jobs per scan point.
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
def create_jobs(options, proto_script, job_base_name, jobs_dir, num_jobs, num_iterations_per_job, walltime, num_cpu, min_seed):
  jobs_dir = os.path.realpath(os.path.abspath(os.path.expanduser(jobs_dir)))
  submit_file_name = os.path.join(jobs_dir,'submit_' + job_base_name + '.sh')
  submit_file = open(submit_file_name, 'w')
  submit_file.writelines('#!/bin/sh\n')
  scan2_value = options.scan2start
  job_index  = 0
  while scan2_value <= options.scan2end:
    scan1_value = options.scan1start
    while scan1_value <= options.scan1end:
      for i in range(0,num_jobs):
        settings_dict = {
          'job_name'   : job_base_name + '_' + str(job_index),
          'out_file'   : os.path.join(jobs_dir,'o_' + job_base_name + '_' + str(job_index) + '.log'),
          'err_file'   : os.path.join(jobs_dir,'e_' + job_base_name + '_' + str(job_index) + '.log'),
          'log_file'   : os.path.join(jobs_dir,'l_' + job_base_name + '_' + str(job_index) + '.log'),
          'walltime'   : walltime,
          'num_cpu'    : str(num_cpu),
          'job_number' : str(job_index),
          'cwd'        : os.getcwd(),
          'scan1_start': scan1_value,
          'scan1_end'  : min(scan1_value+options.scan1increment*(options.scan1perjob-1),options.scan1end),
          'scan2_start': scan2_value,
          'scan2_end'  : min(scan2_value+options.scan2increment*(options.scan2perjob-1),options.scan2end),
          'jobs_dir'   : jobs_dir,
          'parametern' : options.parametern
          }
        min_seed = create_single_job(proto_script, settings_dict, jobs_dir, num_iterations_per_job, min_seed)
        submit_file.writelines('qsub ' + os.path.join(jobs_dir,settings_dict['job_name']+'.sh\n'))
        job_index += 1
      scan1_value += options.scan1increment*options.scan1perjob
    scan2_value += options.scan2increment*options.scan2perjob
  print 'Jobs successfully created. Maximum seed used: ' + str(min_seed-1)
  print 'Submit jobs via this command:'
  print 'sh ' + submit_file_name

if __name__ == "__main__":
  usage = """
 %prog proto_script job_base_name jobs_dir num_pbs_jobs num_iterations_per_job walltime num_cpu min_seed
    """
  parser = OptionParser(usage, version="0.1")
  parser.add_option("", "--scan1-start", action="store", type="float", dest="scan1start", default=0.0, help="Start value of scan parameter 1")
  parser.add_option("", "--scan1-end", action="store", type="float", dest="scan1end", default=0.0, help="End value of scan parameter 1")
  parser.add_option("", "--scan1-increment", action="store", type="float", dest="scan1increment", default=1.0, help="Increment value of scan parameter 1")
  parser.add_option("", "--scan1-per-job", action="store", type="float", dest="scan1perjob", default=1.0, help="Number of scan points per job (default 1) for scan parameter 1")
  parser.add_option("", "--scan2-start", action="store", type="float", dest="scan2start", default=0.0, help="Start value of scan parameter 2")
  parser.add_option("", "--scan2-end", action="store", type="float", dest="scan2end", default=0.0, help="End value of scan parameter 2")
  parser.add_option("", "--scan2-increment", action="store", type="float", dest="scan2increment", default=1.0, help="Increment value of scan parameter 2")
  parser.add_option("", "--scan2-per-job", action="store", type="float", dest="scan2perjob", default=1.0, help="Number of scan points per job (default 1) for scan parameter 2")
  parser.add_option("-n", "--parameter-n", action="store", type="int", dest="parametern", default=0, help="Additional arbitrary parameter n (e.g. number of toys)")
  (options, args) = parser.parse_args()
  if len(args) < 8:
    print 'Usage: ' + sys.argv[0] + ' proto_script job_base_name jobs_dir num_pbs_jobs num_iterations_per_job walltime num_cpu min_seed'
    sys.exit(1)
  create_jobs(options, args[0], args[1], args[2], int(args[3]), int(args[4]), args[5], int(args[6]), int(args[7]))
