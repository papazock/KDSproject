#!/usr/bin/env bash



for i in {1..8}
do
    rm kds$i.sh
    echo "#!/usr/bin/env bash" >>"kds$i.sh"
    echo "#PBS -N ChrisJob" >>"kds$i.sh"
    echo "#PBS -j oe" >>"kds$i.sh"
    echo "#PBS -m n" >>"kds$i.sh"
    echo "#PBS -l nodes=$i" >>"kds$i.sh"
    echo "#PBS -l walltime=0:00:30" >>"kds$i.sh"
    echo "cd KDSproject" >>"kds$i.sh"
    echo "mpirun -np $i ./App -1 -1 dataPoints -1 -1" >>"kds$i.sh"
    qsub kds$i.sh
    echo "new Job created and queued. nodes= $i, script will pause for 8secs"
    sleep 8
done