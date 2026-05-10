RM3100 Test (Zephyr, SAMV71)


Files in this folder:
- src/main.c        : RM3100 test (two sensors)
- prj.conf          : enables I2C + console output
- CMakeLists.txt    : Zephyr app build file


Build (from repo root):
west build -p always -b sam_v71_xult/samv71q21b Tools/RM3100_Test_Zephyr


Flash :
west flash


Run:
- Open the serial console
- You should see REVID/HSHAKE, CC readback, optional BIST raw, and X/Y/Z counts


Config:
- RM_STOP_BETWEEN: 0 repeated-start (default), 1 STOP between phases
- RM_DO_BIST: 1 enable BIST logging, 0 skip