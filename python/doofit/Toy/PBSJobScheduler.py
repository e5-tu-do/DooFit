#!/usr/bin/env python

import subprocess, os, shutil, sys, glob
from xml.etree import ElementTree

max_number_jobs = 2000

# execute a shell command
def call_shell(cmd, sh=False):
  p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=sh)
  p.wait()
  return p.poll(), p.stdout.read(), p.stderr.read()

def number_running_jobs():
  number_jobs = 0
  num, out, err = call_shell(['qstat -u `whoami` | egrep " (Q|R) " | wc -l'], sh=True)
  return int(out)
#  tree = ElementTree.fromstring(out)
#  for job in tree.getiterator('Job'):
#    state = job.find('job_state')
#    if state.text == 'R':
#      number_jobs += 1
#      print '.'
#  return number_jobs

def submit_jobs(files):
  n_running = number_running_jobs()
  n_submit = max_number_jobs - n_running
  print 'There are', n_running, 'jobs currently running. We can submit another', n_submit, 'jobs.'
  while n_submit > 0 and len(files) > 0:
    file = os.path.expanduser(os.path.abspath(files.pop(0)))
    print 'Submitting job', file
    if os.path.exists(file):
      subprocess.check_call(['qsub',file])
      done_dir = os.path.join(os.path.dirname(file),'done_jobs')
      if not os.path.isdir(done_dir):
        os.makedirs(done_dir)
      shutil.move(file, os.path.join(os.path.dirname(file),'done_jobs',os.path.basename(file)))
      n_submit -= 1
    else: 
      raise Exception("Job " + file + " does not exist!")

def auto_submit(control_file):
  print 'Submitting jobs from control file', os.path.expanduser(os.path.abspath(control_file))
  f = open(control_file, 'r')
  for line in f:
    #print glob.glob(line)
    submit_jobs(glob.glob(line))

if __name__ == "__main__":
  if len(sys.argv) > 2:
    submit_jobs(sys.argv[1:])
  elif len(sys.argv) == 2:
    auto_submit(sys.argv[1])
  else:
    print 'Usage', sys.argv[0], 'control_file or list of jobs'