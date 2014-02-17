#!/bin/sh

num_running=$(qstat | grep fkruse | egrep ":[0-9]{2} R" | wc -l)
num_jobs=$(qstat | grep fkruse | wc -l)

echo "Jobs: ${num_running}/${num_jobs}"
qstat | grep fkruse

