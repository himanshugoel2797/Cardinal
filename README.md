# Cardinal
Cardinal is a hobbyist Operating System designed to be elegant. 
It takes heavy inspiration from Plan 9 and related OS's. 

This repository contains the kernel, userspace build configurations,
patches and scripts that are needed to build a working image.

## Project Status
The project is currently in active development. 
Join us at #Cardinal-OS on Freenode ( irc.freenode.net ) to keep up with development.

## Kernel
The kernel is a mostly complete microkernel. As a result it is fairly small.
It responsible for little more than task switching, memory management and 
interprocess communication.

## Userspace
The userspace of Cardinal is currently in development. It consists of the drivers,
servers and user applications. The userspace is divided into 3 rings by the kernel
with lower numbered rings having higher privileges. 

Ring 0, abbreviated as R0 has near kernel level syscall permissions, this is where
the services most core to the system are situated.

The drivers are situated in Ring 1 or R1. This ring has a more restrictions on which
syscalls are available to an application.

All other applications run in Ring 2 or R2. This ring has the most restrictions, only
allowing applications to make syscalls for shared memory and IPC.

### Ports
Currently the following have been ported to the OS:
	- OpenSSL
	- zlib
	- libpng

## Build Instructions
Execute the scripts/build-os.sh script in the source directory.

## IRC
Join us at #Cardinal-OS on Freenode ( irc.freenode.net ).
