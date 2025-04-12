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
- [Software Guidelines](#software-guidelines)
- [Contribution Guide](#Contribution-Guide)
- [Appendix](#appendix)
- [License](#license)

---

## Overview

The CDH Software System manages all aspects of our spacecraft’s command and data handling. The project is structured into three distinct layers:

1. **Embedded:** Directly manages hardware components such as microcontrollers and peripheral interfaces.
2. **OS Level:** Provides a set of APIs that build on the lower-level drivers to deliver system-level services.
3. **Debugging Interfaces:** Comprises web-based UIs and logging tools to generate test reports and facilitate diagnostics.



---

## Components Involved

Currently working on:
![CDH Software Stack](https://github.com/user-attachments/assets/63521e22-d05b-4150-9548-007b64960eca)




**The various software components are listed below:**

```mermaid
flowchart LR
    A["Space Concordia <br> SC-FREYR"] --> B["Debugging Interfaces"] & C["OS Level"]  & F["Sensors &amp; Actuators <br> Communication"] & H["Drivers"]
    B --> B1["In-house Logger"] & B2["Battery Testing WSP"] 
    C --> C1["Power Control API"]  & C2["Message Processor API"] & C3["Watch Dog API"] & C4["RTC API"] & C5["Sensors API"] & C6["Actuators API"] & C7["MRAM"]
    F --> F1["Actuators"] & F2["Sensors"]
    F1 --> F1a["Reaction Wheels"] & F1b["Magnetorquer"] & F1c["Heaters"]
    F2 --> F2a["Magnetometer"] & F2b["Sun Sensor"] & F2c["Thermister"]
    H --> H1["SPI"] & H2["I2C"] & H3["GPIO"] & H4["ADC"] & H5["LVDS"] & H6["UART"]
```

## Contribution Guide

Before you begin: 
Check out the Github essentials: [#Docs/Github_Essentials.md](https://github.com/scsd-cdh/OBC/blob/d00afaf845758b6a269e30d44f57f17d18876272/Docs/Github_Essentials.md)

Your should be assigned an issue, 
You can go check it's requirements listed and start implementing it. Make sure you write good commit's with good descriptions.
Then once you merge it into the stable branch we have an idea of what you did. 

This is how you're development flow should look like:
[#Docs/branching_rules.md](https://github.com/scsd-cdh/OBC/blob/7fbe7e40980c13327b7881f3bf36f775a66de1ec/Docs/branching_rules.md)

The following document lists all the protocols required inter-MCU communication: [Protocols](https://docs.google.com/document/d/1QJ-23KT9wzDGa4bX3uof-pEGOELUUwvdz8o87ZT44fY/edit?tab=t.0)  -> Feel free to add required protocols whenever you want. Leave a comment for every change made. 

**MSP430 Project Setup and Structure**
[MSP Structure](https://github.com/scsd-cdh/OBC/blob/045e9a8daf7846750b0903a72c4fdc19a29c6233/DEV_GUIDE.md)

**SAMV71 Project Setup and Structure**
[Coming Soon](https://www.youtube.com/shorts/4neZwq696J4)
## Software Guidelines

Our development adheres to NASA's Rule of Ten. For more details, please refer to the 
- For C, [NASA Rule of Ten](https://web.eecs.umich.edu/~imarkov/10rules.pdf).
- Branching, [Successful Git Branching Model](https://nvie.com/posts/a-successful-git-branching-model/)
- Commit Naming, [Conventional Commits Specification](https://www.conventionalcommits.org/en/v1.0.0/)

Branching strategy: 
[Successful Git Branching Model](https://nvie.com/posts/a-successful-git-branching-model/).



---


### Helpful Resources
**Simplified Schematics from EPS:** [schematics](https://github.com/scsd-cdh/OBC/tree/4f78fb7ef3817c7a7aa3b54e0cb5d2de58017581/Docs/Schematics)
- Learning Resources: [link](https://github.com/scsd-cdh/OBC/blob/d4c28b9f10d4c328e8bc8e315ff671d0299aac05/Docs/Learning%20Resources.md) 
- [How to Create Software Design Documents (Lucidchart)](https://www.lucidchart.com/blog/how-to-create-software-design-documents)
- [Digital Jhelms' GitHub Gist](https://gist.github.com/digitaljhelms/4287848)


---

## License

This project is licensed under the [MIT License](LICENSE).

---


