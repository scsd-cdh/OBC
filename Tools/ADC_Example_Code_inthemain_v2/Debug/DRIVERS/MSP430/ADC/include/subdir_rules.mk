################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
DRIVERS/MSP430/ADC/include/adc12_b.obj: C:/Users/space/Documents/OBC/DRIVERS/MSP430/ADC/include/adc12_b.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs1280/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccs1280/ccs/ccs_base/msp430/include" --include_path="C:/Users/space/workspace_v12/ADC_Example_Code_inthemain_v2" --include_path="C:/Users/space/Documents/OBC/DRIVERS/MSP430/ADC" --include_path="C:/Users/space/Documents/OBC/DRIVERS/MSP430/ADC/include" --include_path="C:/Users/space/Documents/OBC/DRIVERS/MSP430/ADC/include/inc" --include_path="C:/ti/ccs1280/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5969__ --define=_MPU_ENABLE -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="DRIVERS/MSP430/ADC/include/$(basename $(<F)).d_raw" --obj_directory="DRIVERS/MSP430/ADC/include" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


