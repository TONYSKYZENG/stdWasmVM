This is a multi-thread webassembly virtual-machine, and may be a component of forge-compute in the future
The original version is LHBS's ewasmVM from github, however, this one IS NOT A COPY, hasing its own unique features, which are as follows:
1.can support multi-thread(in this early version, just a switch of task0 and task1).
2.managed with cmake, more compitable and configureble. 
3.using dynamic memory allocation for stacks/callstacks/branch-table.
To build(linux-x64):
mkdir build
cmake ..
make
(if the vos is not generated, do these again)
To run:
cd bin
./webasm
To transplant:
edit the CMakelists in each folder, to match your commpiler and host's behavior(eg:esp32,win32...)
Bugs:
Still can not handle a thread's return value, may cause core dump.
Note:
The tonydatapack (will be in extlib) is made for the future, this is not used and completed yet, so far it is not uploaded.
You can use source insight(3.5 or above) to edit the code.
The Posix features are also for the future. 
