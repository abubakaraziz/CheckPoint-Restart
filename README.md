# CheckPoint-Restart

This is a small project check pointing project. In checkpointing, we periodically save a process's image file onto the desk. So, in case a program crashes, the program 
is restarted using the checkpoint image saved.
Use make check to run the code. The code will first run hello program using checkpoint.c (./checkpoint ./hello). This will run hello program. Then a crash signal is
which crashes the hello program, but just before the hello program crashes, checkpoint.c files create a checkpoint of the currently running process and save it on the 
local disk. From the local disk, we can again run the crashed process by running ./restart -checkpointdatafile. 



