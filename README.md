<!-- Top Images: Three identical images -->
<div align="center">
  <img src="https://github.com/user-attachments/assets/3487323d-b1ab-42d3-95d7-171848ec610e" alt="EPS" width="200" style="margin: 0 10px;">
  <img src="https://github.com/user-attachments/assets/e5974ec4-5c03-4e11-9943-247dc1a2335c" alt="EPS" width="200" style="margin: 0 10px;">
  <img src="https://github.com/user-attachments/assets/1a330da9-d749-4397-97ef-e2ad4217c84d" alt="EPS" width="200" style="margin: 0 10px;">
</div>

<!-- Two-Column Header with Icon and Title -->
<table>
  <tr>
    <td style="vertical-align: middle; padding-right: 10px;">
      <img src="https://github.com/user-attachments/assets/c4271b34-01f1-4f24-80ce-8c2c151b5e25" alt="Icon" width="300">
    </td>
    <td style="vertical-align: middle;">
      <h1 style="margin: 0;">CDH Software System</h1>
      <p style="margin: 5px 0 0 0;">
        CDH Software System is the core of our spacecraft’s command and data handling architecture. It is organized into three primary layers that handle embedded hardware operations, system-level APIs, and user-facing debugging interfaces.
      </p>
    </td>
  </tr>
</table>


[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/your_username/your_repo/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub Issues](https://img.shields.io/github/issues/your_username/your_repo.svg)](https://github.com/your_username/your_repo/issues)
[![Conventional Commits](https://img.shields.io/badge/commit-conventional-green.svg)](https://www.conventionalcommits.org/en/v1.0.0/)

---

## Table of Contents

- [Overview](#overview)
- [Components Involved](#components-involved)
  - [Embedded Systems](#embedded-systems)
  - [OS Level](#os-level)
  - [Debugging Interfaces](#debugging-interfaces)
- [Communication Protocols](#communication-protocols)
- [Task Decomposition and Assignment](#task-decomposition-and-assignment)
- [Software Guidelines](#software-guidelines)
- [Appendix](#appendix)
- [Component List](#component-list)
- [License](#license)

---

## Overview

The CDH Software System manages all aspects of our spacecraft’s command and data handling. The project is structured into three distinct layers:

1. **Embedded:** Directly manages hardware components such as microcontrollers and peripheral interfaces.
2. **OS Level:** Provides a set of APIs that build on the lower-level drivers to deliver system-level services.
3. **Debugging Interfaces:** Comprises web-based UIs and logging tools to generate test reports and facilitate diagnostics.

Our branching strategy follows the principles outlined in the [Successful Git Branching Model](https://nvie.com/posts/a-successful-git-branching-model/).

---

## Components Involved

### Embedded Systems

This layer includes the hardware components that execute the core processing tasks:

- **SAMV71 Microcontroller:**  
  Acts as the primary processor, sending commands to the MSP430 devices.
- **MSP430 Microcontrollers:**  
  These space-grade microcontrollers interface with various peripherals to collect sensor data before passing it back to the SAMV71.

> **Note:** MSP430 microcontrollers are less powerful compared to the SAMV71s. Their specifications are detailed in the appendix.

The overall system block diagram is shown below:

![CDH SW Block Diagram](https://t9014385614.p.clickup-attachments.com/t9014385614/7b63fd11-c62e-4876-bb3f-77da2c721079/CDH_SW_Block_Diag.png)

A detailed view of the embedded system is available here:

![Detailed Embedded Diagram](https://t9014385614.p.clickup-attachments.com/t9014385614/48e73c75-fce2-479e-96bd-495d54704f3b/CDH_SW_Block_Diag.drawio%20(2).png)

> **Attention:** Please discuss with Nabil/Callum regarding the placement of IRQs. Also, verify that the label currently marked as SRAM on the bottom right is corrected to S-BAND.

### OS Level

This layer abstracts hardware details via a collection of APIs built on the low-level driver code. It simplifies development by offering modular, high-level system functionalities.

### Debugging Interfaces

This layer includes:
- **Web-based UIs:** For real-time system monitoring and control.
- **Logging and Test Reporting:** Tools that generate detailed logs and diagnostic reports to aid in troubleshooting.

---

## Communication Protocols

The system leverages multiple communication protocols to interface between components:

1. **I2C:** Used for communication between MSP430s and the SAMV71.
2. **QSPI:** Interfaces with MRAM chips.
3. **UART:** Facilitates communication between the MSP430 and the transceiver chip (AX100) in the communications sub-module.
4. **GPIO:** Typically used for simple on/off signals (e.g., an IRQ for power cycling).
5. **LVDS:** Ensures reliable routing of images from the imager payload (Zetane) to the communications submodule.

---

## Task Decomposition and Assignment

The tasks are split into three layers:

- **Driver Layer:** Implements the core hardware interfacing code.
- **Middleware Layer:** Builds on the drivers to provide higher-level functionalities.  
  *Todo: Add "State Resolver" to middleware.*
- **Application Layer:** Implements top-level functions utilizing the middleware services.

The following diagram outlines the task breakdown:

![Task Decomposition Diagram](https://t9014385614.p.clickup-attachments.com/t9014385614/505a3bf6-cc8f-4bcb-b4a1-8affedb17321/CDH_SW_Block_Diag.drawio.png)

---

## Software Guidelines

Our development adheres to NASA's Rule of Ten. For more details, please refer to the [NASA Rule of Ten](https://web.eecs.umich.edu/~imarkov/10rules.pdf).

---

## Appendix

### Helpful Resources

- [How to Create Software Design Documents (Lucidchart)](https://www.lucidchart.com/blog/how-to-create-software-design-documents)
- [Successful Git Branching Model](https://nvie.com/posts/a-successful-git-branching-model/)
- [Conventional Commits Specification](https://www.conventionalcommits.org/en/v1.0.0/)
- [Digital Jhelms' GitHub Gist](https://gist.github.com/digitaljhelms/4287848)

---

## Component List

*Note: Ensure that each component is assigned a unique ID and the list is maintained as the project evolves.*

| Component                | ID           | Description                                          |
| ------------------------ | ------------ | ---------------------------------------------------- |
| SAMV71 Microcontroller   | `SAMV71-001` | Primary processor for command distribution           |
| MSP430 Microcontroller   | `MSP430-001` | Secondary processor interfacing with peripherals     |
| MRAM Chip                | `MRAM-001`   | Non-volatile memory for data storage                 |
| Transceiver (AX100)      | `AX100-001`  | Facilitates communications in the Comms sub-module   |
| Imager Payload (Zetane)  | `ZETANE-001` | Captures and routes image data to the Comms sub-module |

---

## License

This project is licensed under the [MIT License](LICENSE).

---

Feel free to update the badges, URLs, and content as your project evolves.
