#!/usr/bin/env python

import subprocess, os, shutil, sys
from xml.etree import ElementTree

max_number_jobs = 2000

# execute a shell command
def call_shell(cmd):
  p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  p.wait()
  return p.poll(), p.stdout.read(), p.stderr.read()

def number_running_jobs():
  number_jobs = 0
  num, out, err = call_shell(['qstat','-x'])
  tree = ElementTree.fromstring(out)
  for job in tree.getiterator('Job'):
    state = job.find('job_state')
    if state.text == 'R':
      number_jobs += 1
  return number_jobs

def submit_jobs(files):
  n_submit = max_number_jobs - number_running_jobs()
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

if __name__ == "__main__":
  submit_jobs(sys.argv[1:])