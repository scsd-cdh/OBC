# Setting up the project

1. Install [CLion](https://www.jetbrains.com/clion/download) (it is free for non-commercial use).
2. Follow the instructions in the `Installing dependencies` section
   at https://docs.zephyrproject.org/latest/develop/getting_started/index.html#install-dependencies. <br>
   **Do not follow the instructions for `Get Zephyr and install Python dependencies`**
3. Create a main project folder in which the project will reside
4. (Windows Only) Inside File Explorer for the main project folder, click on the [address bar (ignore path in image)](https://uis.georgetown.edu/wp-content/uploads/2019/05/win10-fileexplorer-addrbar.png)  and enter `cmd` to open the Command Prompt at the exact path.
5. Run `python3 -m venv venv` inside the main project folder you have created
6. (Windows Only) Run `.venv\Scripts\activate.bat`. You will need to run this command every time you wish to use a `west`
   command.
7. (Linux Only) Run `source venv/bin/activate`. You will need to run this command every time you wish to use a `west`
   command.
8. Run `pip install west` inside the main project folder.
9. Run `west init -m https://github.com/scsd-cdh/OBC.git .` inside the main project folder.
10. Run `west update` inside the main project folder. You will need to rerun this command every time the west.yaml file is
    updated. (Don't forget to rerun the venv activate command first!)
11. Run `west zephyr-export` inside the main project folder.
11. (Windows Only) Inside File Explorer for the main project folder, click on the [address bar (ignore path in image)](https://uis.georgetown.edu/wp-content/uploads/2019/05/win10-fileexplorer-addrbar.png)  and enter `powershell` to open the PowerShell at the exact path. 
Run `python -m pip install @((west packages pip) -split ' ')` from the main project file. You can then close PowerShell and return to working on cmd.
12. (Linux Only) Run `west packages pip --install`
14. Run `./zephyr west sdk install -t arm-zephyr-eabi` inside the main project folder.
15. Open CLion and select to open the `application` folder within the main project folder.
16. From CLion click the gear icon on the top left of the window and click on `Settings...`.
17. From the section `Build, Execution, Deployement`, Click on `Toolchain` and edit the toolchain to resemble the [image provided](https://docs.zephyrproject.org/latest/_images/clion_toolchain_mingw.webp), with the environment file provided being the path towards venv's activate.bat from your main project folder. You could optionally name your toolchain to indicate that it is being used for Zephyr.
18. Click `Apply` to save the changes.
19. From the `Settings...` menu, click on `CMake`. From here ensure that the Toolchain used is the Zephyr toolchain you created previously. Specify that the Generator that you want to use is `Ninja` and within the CMake options, write `-G Ninja -DBOARD=sam_v71_xult/samv71q21`
20. Click `Apply` to save the changes.
21. Go to `Settings... -> Build,Execution,Deployment -> Embedded Development -> RTOS Integration` and set `Enable RTOS Integration`
22. Click `Apply` to save the changes.
16. Right click `APP/COMMS_App/CMakeLists.txt` or `APP/CDH_App/CMakeLists.txt` and select `Load West Project`. If this option is not available:
    1. If the `Convert to CMake Project` option appears, CLion has failed to detect west/zephyr and something went
       wrong.
    2. If the `Convert to West Project` option appears, everything is fine, it just means the project is already loaded.
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

