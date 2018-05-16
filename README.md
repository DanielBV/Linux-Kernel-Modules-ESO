# Linux Kernel Modules

This repository contains the kernel modules I've done to learn how Linux modules work. This was part of a complementary project for the subject *Estructura de Sistemas Operativos* (Operating-System Structure).

The modules where tested using Ubuntu 16.04.4 LTS and Linux version 4.13.0-39-generic 

## Contents

#### Character Drivers 
- `charEcho`: The device has a buffer that can be written or read by any process (The buffer is emptied when a process reads it)
- `charQuantumBreak`: Module with the same functionality than charEcho, but when the device is opened, it changes the scheduling policy of the process to FIFO. (It is designed to give a process the full CPU to make the OS crash/slow down)


#### Proc Files
- `procPid`: Creates a proc file that when read, returns the pid of the process that opened it.
- `cuentaSecuencial`: Creates a proc file that when read, iterates through an array, printing the every number between 0 and 99. It's a test to understand how sequence files work.
- `procProcess`: Creates a proc file that simulates a basic version of `ps -ef`. It prints the name, pid and policy of every process.

#### Interrupt Handlers (IRQ)
- `K-Input`: Allows processes to get keyboard input from anywhere (not bound to the shell) using a Character device. 

## Requirements

To compile the module you need the linux-headers build-essentials:
```
sudo apt-get install linux-headers-$(uname -r)
```
```
sudo apt-get install build-essential
```
## Building and loading a module

To build a module, you only have to run **`make`** in the module folder. Several files will be created. To load the module, use **`sudo insmod module.ko`** (where *module* is the name of the module")

To unload the module use **`sudo rmmod module.ko`**

If when *insmoded*, the message `ERROR: could not insert module module.ko: Invalid module format` appears, do `make clean`, `make` and try to load it again.

**Remember to open/run every file(device, proc file or C Test) with sudo permissions, or it won't work**
