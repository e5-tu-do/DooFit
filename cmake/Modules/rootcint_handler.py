#!/usr/bin/env python

import sys, subprocess, os, shutil, time

# execute a shell command
def call_shell(cmd):
  p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
  return p

def touch(fname, times = None):
    fhandle = file(fname, 'a')
    try:
        os.utime(fname, times)
    finally:
        fhandle.close()

# simple file locking
def lock_file(file):
  lock_file_name = file + '.lock'
  if os.path.exists(lock_file_name):
    return False
  else:
    touch(lock_file_name)
    return True

def is_locked(file):
  lock_file_name = file + '.lock'
  print "lock time diff: " + str(time.time()-os.path.getctime(lock_file_name))
  if os.path.exists(lock_file_name) and time.time()-os.path.getctime(lock_file_name)<60:
    return True
  else:
    return False

def unlock_file(file):
  lock_file_name = file + '.lock'
  if os.path.exists(lock_file_name):
    os.remove(lock_file_name)

def generate_dictionary(rootcint, dict_file, args):
  include_dirs = []
  input_files = []
  for arg in args:
    if os.path.isdir(arg):
      include_dirs.append('-I'+arg)
    else:
      input_files.append(arg)

  dict_file_completed = dict_file + "_completed"
  cmd = rootcint + ' '.join(include_dirs) + " -f " + dict_file + " -c -p " + ' '.join(input_files)
  if lock_file(dict_file): 
    p = call_shell(cmd)
    r = p.wait()
    if r != 0:
      print p.stderr.read()
      print p.stdout.read()
    shutil.copy(dict_file, dict_file_completed)
    unlock_file(dict_file)
  else:
    print >> sys.stderr, "File " + dict_file + " is locked. Waiting for unlock."
    while is_locked(dict_file):
      time.sleep(1)
    
if __name__ == "__main__":
  if len(sys.argv) < 4:
    print "Usage: " + sys.argv[0] + " rootcint dict_file input_files"
    sys.exit(0)
  if len(sys.argv) > 4:
    print "Additional parameters: " + str(sys.argv[4:])
  generate_dictionary(sys.argv[1], sys.argv[2], sys.argv[3:])
