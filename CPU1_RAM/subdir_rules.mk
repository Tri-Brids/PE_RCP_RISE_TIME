################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1961018414: ../test1.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs2040/ccs/utils/sysconfig_1.26.0/sysconfig_cli.bat" -s "C:/ti/C2000Ware_6_00_01_00/.metadata/sdk.json" -b "/boards/TMDSCNCD28379D" --script "C:/Users/Ha_Alif/Documents/RCP FINAL/lab_ePWM_eCAP_controlcard/test1.syscfg" -o "syscfg" --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/board.c: build-1961018414 ../test1.syscfg
syscfg/board.h: build-1961018414
syscfg/board.cmd.genlibs: build-1961018414
syscfg/board.opt: build-1961018414
syscfg/board.json: build-1961018414
syscfg/pinmux.csv: build-1961018414
syscfg/epwm.dot: build-1961018414
syscfg/device.c: build-1961018414
syscfg/device.h: build-1961018414
syscfg/adc.dot: build-1961018414
syscfg/c2000ware_libraries.cmd.genlibs: build-1961018414
syscfg/c2000ware_libraries.opt: build-1961018414
syscfg/c2000ware_libraries.c: build-1961018414
syscfg/c2000ware_libraries.h: build-1961018414
syscfg/clocktree.h: build-1961018414
syscfg: build-1961018414

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -O0 --opt_for_speed=0 --fp_mode=relaxed --include_path="C:/Users/Ha_Alif/Documents/RCP FINAL/lab_ePWM_eCAP_controlcard" --include_path="C:/Users/Ha_Alif/Documents/RCP FINAL/lab_ePWM_eCAP_controlcard/device" --include_path="C:/ti/C2000Ware_6_00_01_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/include" --define=DEBUG --define=CPU1 --define=C2K_GEN2_DEVICES --define=NO_SELECT_ECAP_INPUT --define=_FLASH --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/Ha_Alif/Documents/RCP FINAL/lab_ePWM_eCAP_controlcard/CPU1_RAM/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

f2837xd_codestartbranch.obj: C:/ti/C2000Ware_6_00_01_00/device_support/f2837xd/common/source/f2837xd_codestartbranch.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -O0 --opt_for_speed=0 --fp_mode=relaxed --include_path="C:/Users/Ha_Alif/Documents/RCP FINAL/lab_ePWM_eCAP_controlcard" --include_path="C:/Users/Ha_Alif/Documents/RCP FINAL/lab_ePWM_eCAP_controlcard/device" --include_path="C:/ti/C2000Ware_6_00_01_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/include" --define=DEBUG --define=CPU1 --define=C2K_GEN2_DEVICES --define=NO_SELECT_ECAP_INPUT --define=_FLASH --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/Ha_Alif/Documents/RCP FINAL/lab_ePWM_eCAP_controlcard/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -O0 --opt_for_speed=0 --fp_mode=relaxed --include_path="C:/Users/Ha_Alif/Documents/RCP FINAL/lab_ePWM_eCAP_controlcard" --include_path="C:/Users/Ha_Alif/Documents/RCP FINAL/lab_ePWM_eCAP_controlcard/device" --include_path="C:/ti/C2000Ware_6_00_01_00/driverlib/f2837xd/driverlib" --include_path="C:/ti/ccs2040/ccs/tools/compiler/ti-cgt-c2000_22.6.3.LTS/include" --define=DEBUG --define=CPU1 --define=C2K_GEN2_DEVICES --define=NO_SELECT_ECAP_INPUT --define=_FLASH --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/Ha_Alif/Documents/RCP FINAL/lab_ePWM_eCAP_controlcard/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


