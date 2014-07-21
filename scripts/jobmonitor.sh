#!/bin/sh

num_running=$(qstat | grep fkruse | egrep ":[0-9]{2} R" | wc -l)
num_queued=$(qstat | grep fkruse | egrep -v ":[0-9]{2} R" | wc -l)
num_running_total=$(qstat -q | tail -n 1 | awk '{ print $1; }')
num_queued_total=$(qstat -q | tail -n 1 | awk '{ print $2; }')

echo "Own jobs (R/Q): ${num_running}/${num_queued} [all jobs (R/Q): ${num_running_total}/${num_queued_total}]"
qstat | grep fkruse

