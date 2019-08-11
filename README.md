# Linux Kernel Modules

This repository contains the kernel modules I've done to learn how Linux modules work. The project was part of a complementary project for the subject *Estructura de Sistemas Operativos* (Operating-System Structure).

The modules were tested using Ubuntu 16.04.4 LTS with Linux version 4.13.0-39-generic, and Fedora 27 with Linux version 4.15.13-300 

## Contents

#### Character Drivers 
- `charEcho`: The device has a buffer that can be written or read by any process (The buffer is emptied after a process reads it)
- `charQuantumBreak`: Creates a device that, when opened, changes the scheduling policy of the process to FIFO. (It is designed to give a process the full CPU to make the OS crash/slow down)


#### Proc Files
- `procPid`: Creates a proc file that when read, returns the pid of the process that opened it.
- `cuentaSecuencial`: Creates a proc file that when read, iterates through an array, printing every number between 0 and 99. It's a test to understand how sequence files work.
- `procProcess`: Creates a proc file that simulates a basic version of `ps -ef`. It prints the name, pid, and policy of every process.

#### Interrupt Handlers (IRQ)
- `K-Input`: Allows processes to get keyboard input from anywhere (not bound to the shell) using a Character device. 

## Requirements
* **Ubuntu**
```
sudo apt-get install linux-headers-$(uname -r)
```
```
sudo apt-get install build-essential
```
* **Fedora**
```
sudo dnf install make automake gcc gcc-c++ kernel-devel
```
## Building and loading a module

To build a module, you only have to run **`make`** in the module folder.
To load the module, use **`sudo insmod module.ko`** (where *module* is the name of the module")

To unload the module use **`sudo rmmod module.ko`**

If when *insmoded*, the message `ERROR: could not insert module module.ko: Invalid module format` appears, do `make clean`, `make` and try to load it again.

**Remember to open/run every file(device, proc file or C Test) with sudo permissions, or it won't work**
