SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\MentorGraphics\9.5PADS\SDD_HOME\common\win32\bin;C:\MentorGraphics\9.5PADS\SDD_HOME\common\win32\lib;C:\MentorGraphics\9.5PADS\MGC_HOME.ixn\bin;C:\MentorGraphics\9.5PADS\MGC_HOME.ixn\lib;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\qtil\ADK_Toolkit_1.2.2.13_x64\tools\python27;C:\qtil\ADK_Toolkit_1.2.2.13_x64\tools\python27\Scripts;C:\GNUARM\4.9\bin\bin;%USERPROFILE%\AppData\Local\Microsoft\WindowsApps;
SET CPU_TYPE=HC32F030F8TA
SET CPU_VENDOR=HDSC
SET UV2_TARGET=dma_hw_trigger_burst_Release
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\output\release\startup_hc32f030._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\system_hc32f030.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\user_app_led.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\ddl.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\interrupts_hc32f030.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\sysctrl.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\dmac.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\adc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\bgr.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\user_driver_pwm_dma.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\bt.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\flash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\spi.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\user_driver_timer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\user_driver_adc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\user_driver_uart.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\uart.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\output\release\user_driver_flash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\output\release\dma_hw_trigger_burst.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\output\release\dma_hw_trigger_burst.axf" --i32combined --output ".\output\release\dma_hw_trigger_burst.hex"
