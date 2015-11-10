#!/bin/bash

cp ./addqueue /bin/
cp ./showqueue /bin/
cp ./rmqueue /bin/

chown user01:user01 /bin/addqueue
chown user01:user01 /bin/showqueue
chown user01:user01 /bin/rmqueue

chmod u+s /bin/addqueue
chmod u+s /bin/showqueue
chmod u+s /bin/rmqueue

mkdir -m 660 /home/user01/queue

chown user01:user01 /home/user01/queue
