# Remote development

## Files

> [!NOTE]
> The .cmd files can be launched from both windows and linux. On linux, it will launch the .sh file. On windows, 
> it will attempt to launch the .sh file through git-bash. Git bash must be installed in the default directory for 
> this to work!

`config.sh`: Contains the login information to login to the remote server and other config options. This file does
not contain any passwords. There is generally no need to edit this file.

`open_console.sh`: Launches ulog-decoder on the remote server configured for the last flashed firmware. Only one user
may be connected to the console at any given time.

`build_and_upload.sh`: Builds the firmware, uploads it to the remote server, flashes the board and starts gdbserver.
On windows, cmake will not be invoked unless it is in the PATH.

`forward_usb.sh`: Attaches a USB over IP device.

`setup_key.sh`: Generates a new SSH key if none exist then prints out the public key.

## Usage through CLion

On CLion, remote development should be used in the following manner:
1. (Windows only) Select `CDH (CMake)` and click the hammer icon to build the firmware.
2. Select `CDH (Remote Upload + GDB)` and click the run icon. Do not stop this process
until you are done debugging.
3. Select `CDH (Remote Logs)` and click the run icon. This step will fail if another user is looking at the logs.
4. Select `CDH (Remote Debug)` and click the debug icon. You should be able to use the debugging features of the IDE as
if you were standing in front of the board. This step will reset the board and may be repeated to reset the board again.
5. Stop all running processes when you are done using the board so that the next person may use it.

## Getting access

All modern versions of Linux and Windows should have openssh installed by default. If this is not the case on windows,
follow the instructions at https://web.archive.org/web/20250227204217/https://www.hawaii.edu/askus/1874

Assuming that openssh is installed, run the setup_key.cmd file, copy the public key it has printed and send it to
someone on discord. That person should add your key to the .ssh/authorized_keys file on the remote server.

The first time you connect, you will see the following message: 
```
The authenticity of host '[iris.alexkar598.dev]:42999 ([-]:42999)' can't be established.
ED25519 key fingerprint is: SHA256:buTtsKKIoO6m4RmMYY+vjX9EZxQg3BYsq6s+M22Nsq0
This key is not known by any other names.
Are you sure you want to continue connecting (yes/no/[fingerprint])? 
```
You must type yes and then press enter. You will not be reasked this question.