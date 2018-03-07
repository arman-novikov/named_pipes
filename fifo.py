import os

path = "/tmp/my_named_pipe"
fifo = open(path, 'w')
#fifo.write("hi!\n")
fifo.write("do we have there\n")
fifo.flush()
fifo.close()

import os
>>> path = "/tmp/t_pipe"
>>> mkfifo(path)
>>> os.mkfifo(path)
>>> fifo = open(path, 'r')
>>> fifo.read()

