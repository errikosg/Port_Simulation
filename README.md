
## Port Simulation

This project (written in C) focuses on communication between **processes** using **POSIX semaphores** and **shared memory** while simulating a port with vessels. Vessels have always a set size(small/medium/large) and the port has limited amount of positions from each category. A vessel may only occupy same or larger position inside the port.<br>

Port:
![homepage](https://github.com/errikosg/Port_Simulation/blob/master/img/port.png)
<br>

There are three types of processes:
1. **Vessels**, that have to: wait in line to enter the port, take an open spot when assigned to, stay inside for a random amount of time, pay fee, wait in line to exit port and finally exit. All these action happen after communication with the port-master.
2. **Port master**, process responsible for managing the port and communicating with the vessels. It is responsible for assigning correctly the vessels to the corresponding positions and that only one vessel may enter or leave at a time while updating the **public ledger** with the port state, time statistics and more.
3. **Monitor**, process responsible for constantly monitoring the port state and showing the info as well as several statistics to the user between certain time intervals. 

<br> The public ledger is a crucial aspect of the problem, as it resides in the shared memory segment and is accessible by every process. Vessels access it to calculate the docking fee, the port-master writes statistics and the monitor reads statistics and info.

### Compile and Run
A makefile is included. To run the program: **./myport -l configfile** , where configfile is ConfigFile.txt in current edition. Process myport is responsible for the setup and initiation of all the other processes.


