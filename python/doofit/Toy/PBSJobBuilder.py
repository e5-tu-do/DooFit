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
#  The proto job is basically the job to be run on the PBS cluster with a bunch
#  of keywords to be replaced (settings for the toy run, individual job settings
#  for one specific job and so on). See below for a list of keywords and 
#  possibilities.
# 
#  Also each iteration can get a unique seed starting at 1.
#
#  @section pbsjobbuilder_protojob The proto job file
#
#  Syntax/Example for a proto job file:
#  
#  @code
# #!/bin/sh
#
# #PBS -N %(job_name)s
# #PBS -o %(out_file)s
# #PBS -e %(err_file)s
# #PBS -m n
# #PBS -M phidomuell@googlemail.com
# #PBS -j oe
# #PBS -l walltime=%(walltime)s
# #PBS -l nodes=1:ppn=%(num_cpu)s
# #PBS -l vmem=%(vmem)s
# export SCRATCH=/local/$USER/$PBS_JOBID
#
# echo "Auto-generated PBS script"
# echo "Sin2Beta toy studies starting at `date`"
# . SetupProject.sh DaVinci v29r3p1 
# echo $0
# echo $PBS_JOBID > %(log_file)s
# echo ${PBS_JOBID} >> %(log_file)s
# cd %(cwd)s
#
# for i in `seq %(seeds)s`;
# do
#   time echo toy --scanstart=%(scan1_start)s --scanend=%(scan1_end)s --scan2start=%(scan2_start)s --scan2end=%(scan2_end)s --seed=$i --output=%(jobs_dir)s/result_%(job_number)s.root --num_cpu=%(num_cpu)s >> %(log_file)s
# done
#
# echo finished job number %(job_number)s
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
#@li @c vmem:     the number of total vmem the job can use
#@li @c seeds:    the seeds for the iterations in this individual job to use (will be generated as <tt>minseed maxseed</tt> compatible to the above @c for statement)
#@li @c job_number: the number of the individual job
#@li @c scan1_start: the current scan value 1 starting point
#@li @c scan1_end: the current scan value 1 ending point
#@li @c scan1_increment: the increment for scan value 1
#@li @c scan2_start: the current scan value 2 starting point
#@li @c scan2_end: the current scan value 2 ending point
#@li @c scan2_increment: the increment for scan value 2
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
def create_jobs(options, proto_script, jobs_dir, num_jobs, num_iterations_per_job, walltime, num_cpu):
  jobs_dir = os.path.realpath(os.path.abspath(os.path.expanduser(jobs_dir)))
  min_id = 0
  job_base_name = options.basename
  min_seed = options.minseed
  vmem = options.vmem_per_core*num_cpu
  if job_base_name == "pbs_job":
    job_base_name = os.path.basename(jobs_dir)
  if os.path.isfile(os.path.join(jobs_dir,'max_seed')):
    with open(os.path.join(jobs_dir,'max_seed')) as f:
      min_seed = int(f.read())+1
      print "File max_seed existing. Starting from seed " + str(min_seed)
  if os.path.isfile(os.path.join(jobs_dir,'max_id')):
    with open(os.path.join(jobs_dir,'max_id')) as f:
      min_id = int(f.read())+1
      print "File max_id existing. Starting from id " + str(min_id)
  if os.path.isfile(os.path.join(jobs_dir,'basename')):
    with open(os.path.join(jobs_dir,'basename')) as f:
      job_base_name = f.read().strip()
      print "File basename existing. Using basename " + job_base_name
  submit_file_name = os.path.join(jobs_dir,'submit_' + job_base_name + '.sh')
  submit_file = open(submit_file_name, 'w')
  submit_file.writelines('#!/bin/sh\n')
  scan2_value = options.scan2start
  job_index  = min_id

  bulk_dir = os.path.join(jobs_dir, "job_bulk_dir")
  os.makedirs(bulk_dir)

  while scan2_value <= options.scan2end:
    scan1_value = options.scan1start
    while scan1_value <= options.scan1end:
      for i in range(min_id,min_id+num_jobs):
        settings_dict = {
          'job_name'   : job_base_name + '_' + str(job_index),
          'out_file'   : os.path.join(bulk_dir,'o_' + job_base_name + '_' + str(job_index) + '.log'),
          'err_file'   : os.path.join(bulk_dir,'e_' + job_base_name + '_' + str(job_index) + '.log'),
          'log_file'   : os.path.join(bulk_dir,'l_' + job_base_name + '_' + str(job_index) + '.log'),
          'walltime'   : walltime,
          'num_cpu'    : str(num_cpu),
          'job_number' : str(job_index),
          'cwd'        : os.getcwd(),
          'scan1_start': scan1_value,
          'scan1_end'  : min(scan1_value+options.scan1increment*(options.scan1perjob-1),options.scan1end),
          'scan1_increment' : options.scan1increment,
          'scan2_start': scan2_value,
          'scan2_end'  : min(scan2_value+options.scan2increment*(options.scan2perjob-1),options.scan2end),
          'scan2_increment' : options.scan2increment,
          'jobs_dir'   : jobs_dir,
          'parametern' : options.parametern,
          'vmem'       : str(vmem)+"mb"
          }
        min_seed = create_single_job(proto_script, settings_dict, bulk_dir, num_iterations_per_job, min_seed)
        if len(options.queue) > 0:
          submit_file.writelines('qsub -q ' + options.queue + ' ' + os.path.join(bulk_dir,settings_dict['job_name']+'.sh\n'))
        else:
          submit_file.writelines('qsub ' + os.path.join(bulk_dir,settings_dict['job_name']+'.sh\n'))
        job_index += 1
      scan1_value += options.scan1increment*options.scan1perjob
    scan2_value += options.scan2increment*options.scan2perjob
  control_file_name = os.path.join(jobs_dir,'control_' + job_base_name + '')
  control_file = open(control_file_name, 'w')
  control_file.writelines(os.path.join(jobs_dir,job_base_name + '_*.sh'))
  maxseed_file_name = os.path.join(jobs_dir,'max_seed')
  maxseed_file = open(maxseed_file_name, 'w')
  maxseed_file.writelines(str(min_seed-1)+'\n')
  maxid_file_name = os.path.join(jobs_dir,'max_id')
  maxid_file = open(maxid_file_name, 'w')
  maxid_file.writelines(str(job_index-1)+'\n')
  basename_file_name = os.path.join(jobs_dir,'basename')
  basename_file = open(basename_file_name, 'w')
  basename_file.writelines(job_base_name)
  print 'Jobs successfully created. Maximum seed used: ' + str(min_seed-1) + ", maximum job id used: " + str(job_index-1)
  print 'Submit jobs via this command:'
  print 'sh ' + submit_file_name

if __name__ == "__main__":
  usage = """
 %prog proto_script jobs_dir num_pbs_jobs num_iterations_per_job walltime num_cpu
    """
  parser = OptionParser(usage, version="1.0")
  parser.add_option("-q", "--queue", action="store", type="string", dest="queue", default="", help="PBS queue to use")
  parser.add_option("-b", "--basename", action="store", type="string", dest="basename", default="pbs_job", help="PBS job base name")
  parser.add_option("-s", "--minseed", action="store", type="int", dest="minseed", default=1, help="Minimal seed to use")
  parser.add_option("", "--scan1-start", action="store", type="float", dest="scan1start", default=0.0, help="Start value of scan parameter 1")
  parser.add_option("", "--scan1-end", action="store", type="float", dest="scan1end", default=0.0, help="End value of scan parameter 1")
  parser.add_option("", "--scan1-increment", action="store", type="float", dest="scan1increment", default=1.0, help="Increment value of scan parameter 1")
  parser.add_option("", "--scan1-per-job", action="store", type="float", dest="scan1perjob", default=1.0, help="Number of scan points per job (default 1) for scan parameter 1")
  parser.add_option("", "--scan2-start", action="store", type="float", dest="scan2start", default=0.0, help="Start value of scan parameter 2")
  parser.add_option("", "--scan2-end", action="store", type="float", dest="scan2end", default=0.0, help="End value of scan parameter 2")
  parser.add_option("", "--scan2-increment", action="store", type="float", dest="scan2increment", default=1.0, help="Increment value of scan parameter 2")
  parser.add_option("", "--scan2-per-job", action="store", type="float", dest="scan2perjob", default=1.0, help="Number of scan points per job (default 1) for scan parameter 2")
  parser.add_option("-n", "--parameter-n", action="store", type="int", dest="parametern", default=0, help="Additional arbitrary parameter n (e.g. number of toys)")
  parser.add_option("-m", "--vmem-per-core", action="store", type="int", dest="vmem_per_core", default=3000, help="Required vmem per core in PBS notation (in MB, default 3000)")
  (options, args) = parser.parse_args()
  if len(args) < 6:
    print 'Usage: ' + sys.argv[0] + ' proto_script jobs_dir num_pbs_jobs num_iterations_per_job walltime num_cpu'
    sys.exit(1)
  create_jobs(options, args[0], args[1], int(args[2]), int(args[3]), args[4], int(args[5]))
