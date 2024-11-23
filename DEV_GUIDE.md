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

