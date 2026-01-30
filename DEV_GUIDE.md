# Setting up the project

1. Install [CLion](https://www.jetbrains.com/clion/download) (it is free for non-commercial use).
2. Follow the instructions in the `Installing dependencies` section
   at https://docs.zephyrproject.org/latest/develop/getting_started/index.html#install-dependencies. <br>
   **Do not follow the instructions for `Get Zephyr and install Python dependencies`**
3. Create a folder in which the project will reside
4. (Windows Only) Open `cmd.exe` and navigate using the `cd` command to the folder you have created. **Do not use
   PowerShell, it will not work.**
5. Run `python3 -m venv venv` inside the folder you have created
6. (Windows Only) Run `venv\Scripts\activate.bat`. You will need to run this command every time you wish to use a `west`
   command.
7. (Linux Only) Run `source venv/bin/activate`. You will need to run this command every time you wish to use a `west`
   command.
8. Run `pip install west` inside the folder that contains the `venv` folder
9. Run `west init -m https://github.com/scsd-cdh/OBC.git .` inside the same folder
10. Run `west update` inside the same folder. You will need to rerun this command every time the west.yaml file is
    updated. (Don't forget to rerun the venv activate command first!)
11. (Windows Only) Run `cmd /c scripts\utils\west-packages-pip-install.cmd` inside the same folder
12. (Linux Only) Run `west packages pip --install`
13. Run `west zephyr-export`
14. Run `west sdk install -t arm-zephyr-eabi` inside the same folder
15. Open the `application` folder in CLion.
16. Right click `APP/COMMS_App/CMakeLists.txt` or `APP/CDH_App/CMakeLists.txt` and select `Load West Project`. If this option is not available:
    1. If the `Convert to CMake Project` option appears, CLion has failed to detect west/zephyr and something went
       wrong.
    2. If the `Convert to West Project` option appears, everything is fine, it just means the project is already loaded.
17. See [Configure the toolchain and CMake profile](https://docs.zephyrproject.org/latest/develop/tools/clion.html#configure-the-toolchain-and-cmake-profile)
and [Enable RTOS integration](https://docs.zephyrproject.org/latest/develop/tools/clion.html#enable-rtos-integration)
for details on configuring CLion. You will not need to setup a CMake Profile, but you will need to create a toolchain
and then configure the Zephyr Profile to use it.
18. In the top-right corner, select "COMMS (OpenOCD)" or "CDh (OpenOCD)" in the dropdown.
    1. Click the hammer to compile the code.
    2. Click the play button to compile, and upload the code to a connected board.
    3. Click the debug button to compile, upload the code to a connected board and start debugging.
19. See Tools/remote_dev/readme.MD for information about remote development.

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
```

# Branch Naming

- feature/<task_name> : add a new feature or module to the code
- bugfix/<task_name> : fixing a bug or updating existing code in general

# How to Contribute Code

- Create a branch for each task (use Branch Naming)
- For each task add .c file for function implementation and .h file for function definitions.
- Once done with development and testing. Create a PR (use PR template if available)
- Assign a reviwer and review code together.

https://www.conventionalcommits.org/en/v1.0.0/#summary:~:text=Conventional%20Commits%201.0.0-,Summary,-The%20Conventional%20Commits

