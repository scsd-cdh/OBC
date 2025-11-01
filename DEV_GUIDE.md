# Setting up the project

## CLion (Recommended)
1. Install [CLion](https://www.jetbrains.com/clion/download) (it is free for non-commercial use).
2. Install [NodeJS](https://nodejs.org/dist/latest/) (Get the one called node-vXX.X.X-x64.msi), make sure to tick Add to PATH when asked by the installer. You may need to restart your open applications or your computer for the changes to apply. 
   1. On linux, you can and should get nodejs from your package manager instead.
3. Clone this repo but **DO NOT OPEN IT IN CLION YET**.
   1. If you have accidentally opened the project, close it and run `git clean -X -f -d`.
4. Run once `setup_toolchain_windows.bat` or `setup_toolchain_linux.sh` depending on your OS.
5. Open the project in CLion and click through the first time setup prompts, you should not need to change anything in here.
6. Go in File -> Settings -> Build, Execution, Deployment -> Embedded Development.
7. Click on the dots next to "OpenOCD location" and select `<where_you_cloned_the_repo>/toolchain/openocd/bin/openocd`.
8. At the top-right corner, select "CDH (OpenOCD)" in the dropdown.
   1. Click the hammer to compile the code.
   2. Click the play button to compile, upload the code to a connected board.
   3. Click the debug button to compile, upload the code to a connected board and start debugging.

## Other environments
1. Install CMake and Make through whichever means.
2. Install [NodeJS](https://nodejs.org/dist/latest/) (Get the one called node-vXX.X.X-x64.msi), make sure to tick Add to PATH when asked by the installer. You may need to restart your open applications or your computer for the changes to apply.
   1. On linux, you can and should get nodejs from your package manager instead.
3. Clone this repo.
4. Run once `setup_toolchain_windows.bat` or `setup_toolchain_linux.sh` depending on your OS.
5. Run `cmake -B build --toolchain ProjectFiles/CDH/SAM7.cmake -G "Unix Makefiles"` once.
   1. You only need to run this step once.
6. Run `cmake --build build` to compile the code.
7. Run `cmake --build build -t CDH_FLASH` to compile the code and upload it to a connected boarc.

# Repository  Structure 
```plaintext
OBC/

APP                 # board specific code (application layer)
├── ADCS
├── AI_PAYLOAD
├── BMS
├── PDS_App
├── TRANSCEIVERS
└── YEAST_PAYLOAD

DRIVERS/MSP430      # processor specific code for MSP
├── GPIO
├── I2C
└── UTILS

DRIVERS/SAMv71      # processor specific code for  SAMV71

MIDDLEWARE
├── MRAM
├── MSG_PROC
├── PWR_CTRL
├── WATCHDOG
└── tinyprotocol     

ProjectFiles        # CCS studio and ATmel studio files
├── 
└── 

Docs                # all docs go here
├── Scheamtics
└── DataSheets

Tools               # this is for any side tools
├── Rown PDS
├── TinyProtocol Example
├── SPI_CC_Studio_proj/msp430fr59...
├── UART_CC_Studio_proj/msp430fr...

# Branch Naming 
- feature/<task_name> : add a new feature or module to the code
- bugfix/<task_name> : fixing a bug or updating exisitin code in general 

# How to Contribute Code 
- Create a branch for each task (use Branch Naming)
- For each task add .c file for function implementation and .h file for function definitions.
- Once done with development and testing. Create a PR (use PR template if available)
- Assign a reviwer and review code together.

https://www.conventionalcommits.org/en/v1.0.0/#summary:~:text=Conventional%20Commits%201.0.0-,Summary,-The%20Conventional%20Commits

