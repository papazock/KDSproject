#!/usr/bin/env bash

for i in {1..16}
do
    rm kds$i.sh
    echo "#!/usr/bin/env bash" >>"kds$i.sh"
    echo "#PBS -N ChrisJob" >>"kds$i.sh"
    echo "#PBS -j oe" >>"kds$i.sh"
    echo "#PBS -m n" >>"kds$i.sh"
    echo "#PBS -l nodes=$i:ppn=4" >>"kds$i.sh"
    echo "#PBS -l walltime=0:00:30" >>"kds$i.sh"
    echo "cd KDSproject" >>"kds$i.sh"
    echo "mpirun -np $i ./App -1 -1 dataPoints -1 -1" >>"kds$i.sh"
    qsub kds$i.sh
    echo "new Job created and Queqed, script will pause for 5secs"
    sleep 5
done