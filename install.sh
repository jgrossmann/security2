#!/bin/bash

#Copy files to /bin, change owner to user01 (spool user), 
#setuid to user01, create protected spool queue directory
#with only rwx permissions for user01, set owner of directory
#to user01

cp ./addqueue /bin/
cp ./showqueue /bin/
cp ./rmqueue /bin/

chown user01:user01 /bin/addqueue
chown user01:user01 /bin/showqueue
chown user01:user01 /bin/rmqueue

chmod u+s /bin/addqueue
chmod u+s /bin/showqueue
chmod u+s /bin/rmqueue

rm -r -f /home/user01/queue
mkdir -m 700 /home/user01/queue

chown user01:user01 /home/user01/queue
