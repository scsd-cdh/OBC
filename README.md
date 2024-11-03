
[![](https://t9014385614.p.clickup-attachments.com/t9014385614/ef91f96b-fe1c-4da7-ae60-9722250d628f/image.png)](https://nvie.com/posts/a-successful-git-branching-model/)

### Components involved

* * *

  

![](https://t9014385614.p.clickup-attachments.com/t9014385614/7b63fd11-c62e-4876-bb3f-77da2c721079/CDH_SW_Block_Diag.png)

_Remove this later:_ [_https://app.diagrams.net/?state=%7B%22ids%22:%5B%221Lrvki\_HJC8g2YscnLJUFlnQhUQTD1xV3%22%5D,%22action%22:%22open%22,%22userId%22:%22111935358387644137052%22,%22resourceKeys%22:%7B%7D%7D_](https://app.diagrams.net/?state=%7B%22ids%22:%5B%221Lrvki_HJC8g2YscnLJUFlnQhUQTD1xV3%22%5D,%22action%22:%22open%22,%22userId%22:%22111935358387644137052%22,%22resourceKeys%22:%7B%7D%7D)

  

  

There are three different sections which the various components are divided into,

  

1. **Embedded:** This level consists of all the hardware components involved. There are two main _programmable_ components_\-_ The SAMV71 and the MSP430, both of these are _space-grade_ microcontrollers. These were the components used in SC-ODIN, our clubs last spacecraft endeavor.

> The programs we write mainly run on these processors. Their specs are listed in the appendix. But in general, MSP430 sare much less powerful athan the SAMV71s.

The SAMV71's sole purpose is to send the MSP430s commands. The MSP430s are connected to other peripherals and collect information, which is then retreived by the SAMV71 and processed accordingly.

> ![](https://t9014385614.p.clickup-attachments.com/t9014385614/48e73c75-fce2-479e-96bd-495d54704f3b/CDH_SW_Block_Diag.drawio%20(2).png)

  

  

  

  

  

  

  

  

> This is subject to change - Discuss with Nabil/Callum as to where the IRQs are supposed to be located. Also its not SRAM(on the bottom right), its S-BAND. Also update this.

  

The following communication protocols will be used:

1.     1.     1. _I2C -_ Mainly used to communicate between the MSP430s and SAMV71.
        2. _QSPI_ - Used to interface with the MRAM chips.
        3. _UART_ - Used to communicate between the MSP430 and Transciever Chip(_AX100)_ in the communications sub-module.
        4. _GPIO_ - Usually just one wire(_IRQ_) directly connected to the microcontroller, used for setting/reading flags or sending an on/off signal for power cycling.
        5. _LVDS_ - Used to reliable routing images to the Comms Submodule from the Imager payload(_Zetane_).

  

1. **OS Level:** The OS level components will likely include several APIs which will use the drivers implemented in the previous section.
2. **Debugging Interfaces:** This level of the stack will be a software suite comprising of several web-app **UIs** and which will also provide functionality for generating **logs** and **test reports**.

  

### Task Decomposition and Assignment

* * *

  

![](https://t9014385614.p.clickup-attachments.com/t9014385614/505a3bf6-cc8f-4bcb-b4a1-8affedb17321/CDH_SW_Block_Diag.drawio.png)

_Todo: Add "State Resolver" to middleware._

  

The tasks are decomposed into three distinct stack. The lowest level of the stack will implement the `driver` code. The `middleware` will then use this to implement its own functionality. The `App` layer will implement the top level functions using the middleware.

  

# APPENDIX

  

_Stuff that may be helpful_

[https://www.lucidchart.com/blog/how-to-create-software-design-documents](https://www.lucidchart.com/blog/how-to-create-software-design-documents)

[https://nvie.com/posts/a-successful-git-branching-model/](https://nvie.com/posts/a-successful-git-branching-model/)

Commit Convention: [https://www.conventionalcommits.org/en/v1.0.0/]

[

gist.github.com

https://gist.github.com/digitaljhelms/4287848

](https://gist.github.com/digitaljhelms/4287848)

  

_Also Make sure to include/create a component list with ID for Each component._
