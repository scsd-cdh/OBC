<!-- Top Images: Three identical images -->
<div align="center">
  <img src="https://github.com/user-attachments/assets/3487323d-b1ab-42d3-95d7-171848ec610e" alt="EPS" width="400" style="margin: 0 10px;">
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

Currently working on:
![CDH Software Stack](https://github.com/user-attachments/assets/63521e22-d05b-4150-9548-007b64960eca)




**The various software components are listed below:**

```mermaid
graph TD
    A[Space Concordia <br> SC-FREYR] --> B[Debugging Interfaces]
    A --> C[OS Level]
    A --> D[Embedded]
    A --> E[Other ICs]
    A --> F[Sensors & Actuators <br> Communication]
    A --> G[Inter-Processor <br> Communication]

    %% Debugging Interfaces
    B --> B1[In-house Logger]
    B --> B2[Battery Testing WSP]
    B --> B3[LSP]

    %% OS Level
    C --> C1[Power Control API]
    C --> C2[Message Processor API]
    C --> C3[Watch Dog API]
    C --> C4[RTC API]
    C --> C5[Sensors API]
    C --> C6[Actuators API]

    %% Embedded
    D --> D1[MSP430 FR5999]
    D --> D2[SAMV71]
    D --> D3[Watchdog?]

    %% Other ICs
    E --> E1[MRAM]

    %% Sensors & Actuators Communication
    F --> F1[Actuators]
    F --> F2[Sensors]

    F1 --> F1a[Reaction Wheels]
    F1 --> F1b[Magnetorquer]

    F2 --> F2a[Thermocouples]
    F2 --> F2b[IMU]
    F2 --> F2c[Sun Sensor]

    %% Inter-Processor Communication
    G --> G1[AX100]
    G --> G2[S-Band]

```
**IRQs are raised to request fetching updates from each board. Here is an interface diagram.**

<p align="center">
  <img src="https://github.com/user-attachments/assets/e677aaba-2f24-4457-b89e-081dfd59ca01" alt="Main OBC" width="300" />
</p>


---

## Task Decomposition and Assignment



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
