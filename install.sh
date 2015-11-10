#!/bin/bash

chown user01:user01 ./addqueue
chown user01:user01 ./showqueue
chown user01:user01 /bin/rmqueue

chmod u+s ./addqueue
chmod u+s ./showqueue
chmod u+s /bin/rmqueue

cp ./addqueue /bin/
cp ./showqueue /bin/
cp ./rmqueue /bin/
