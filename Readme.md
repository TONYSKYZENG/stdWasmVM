This is a simple tool set of edge computing based on multi-thread webassembly virtual-machine,
The VM originates from LHBS's ewasmVM from github, hasing its own unique features, though, which are as follows:
1.can support multi-thread(in this early version, just a switch of task0 and task1).
2.managed with cmake, more compitable and configureble. 
3.using dynamic memory allocation for stacks/callstacks/branch-table.
The net tool set is designed for multi-platform use, and it's structure from low to high is:
networkBsp(or system api)->netsp_low->netsp_high->TDP->vapor or cfp. 
Note the vapor is designed specially for the process of edge-computing, and the cfg is designed for simple configuration
of IOT devices. So far, I have created drivers for linux(32bit or 64 bit, X86 or arm are all OK), and esp32 in freeRTOS and LWIP.
However the esp32 driver has limited functtions and some un-solved bugs.
As for the edge-computing, we use the unitnode and lsnode to manage all computing cores, so far the policy is as simple as
"transfer running content to who is free".
Each edge can connet directly to the local center, and has the ability to communicate eachother bypassing the center.
To build(linux):
mkdir build
cmake ..(if you do not want the stack imformation to be printed, using -DVM_BUILD_DEBUG_VM=OFF)
make
sudo make install
To run:
cd ../bin
./webasm {CENTER_IP} center port
Acknowledge that the center is now a qt-gui application with the shared-libs just build by this package.
So far, I have no plan to upload the qt source, as it is still fragile.FIX ME?
There is also a good news that the netsp can build on the qt's tcpsocket.
