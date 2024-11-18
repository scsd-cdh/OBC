# Repository  Structure 
```plaintext
OBC/
# board specific code (application layer)
APP
├── ADCS
├── AI_PAYLOAD
├── BMS
├── PDS_App
├── TRANSCEIVERS
└── YEAST_PAYLOAD
# processor specific code for MSP and SAMV71
DRIVERS/MSP430
├── GPIO
├── I2C
├── SPI_CC_Studio_proj/msp430fr59...
├── UART_CC_Studio_proj/msp430fr...
└── UTILS

MIDDLEWARE
├── MRAM
├── MSG_PROC
├── PWR_CTRL
├── WATCHDOG
└── tinyprotocol     
# CCS studio and ATmel studio files
ProjectFiles
├── 
└── 
# all docs go here
Docs
├── Scheamtics
└── DataSheets
# this is for any side tools
Tools
```

# Branch Naming 
- feature/<task_name> : add a new feature or module to the code
- bugfix/<task_name> : fixing a bug or updating exisitin code in general 

# How to Contribute Code 
- Create a branch for each task (use Branch Naming)
- For each task add .c file for function implementation and .h file for function definitions.
- Once done with development and testing. Create a PR (use PR template if available)
- Assign a reviwer and review code together.

https://www.conventionalcommits.org/en/v1.0.0/#summary:~:text=Conventional%20Commits%201.0.0-,Summary,-The%20Conventional%20Commits

