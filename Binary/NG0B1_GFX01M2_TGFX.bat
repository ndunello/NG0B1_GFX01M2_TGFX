:: STM32CubeProgrammer Utility flash script

@ECHO OFF
@setlocal
::COLOR 0B

:: Current Directory
@SET CUR_DIR=%CD%

@SET HEX_FILE="NG0B1_GFX01M2_TGFX.hex"
@IF NOT EXIST "%HEX_FILE%" @ECHO %HEX_FILE% Does not exist !! && GOTO goError

:: Board ID
@SET BOARD_ID=0
:: External Loader Name
@SET EXT_LOADER=MX25L6433F_GFX01M2_STM32G0

@SET STM32_PROGRAMMER_PATH="%ProgramFiles(x86)%\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
@IF NOT EXIST %STM32_PROGRAMMER_PATH% @SET STM32_PROGRAMMER_PATH="%ProgramW6432%\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
@IF NOT EXIST %STM32_PROGRAMMER_PATH% @SET STM32_PROGRAMMER_PATH="%ProgramFiles%\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin"
@IF NOT EXIST %STM32_PROGRAMMER_PATH% @ECHO STM32CubeProgrammer is not installed !! && GOTO goError
@IF NOT EXIST %STM32_PROGRAMMER_PATH% @ECHO %STM32_PROGRAMMER_PATH% Does not exist !! && GOTO goError

@SET STM32_EXT_FLASH_LOADER=%CUR_DIR%\%EXT_LOADER%.stldr
@IF NOT EXIST %STM32_EXT_FLASH_LOADER% @SET STM32_EXT_FLASH_LOADER=%CUR_DIR%\..\..\..\..\..\Utilities\PC_Software\ExternalLoader\%EXT_LOADER%.stldr
@IF NOT EXIST %STM32_EXT_FLASH_LOADER% @SET STM32_EXT_FLASH_LOADER=%STM32_PROGRAMMER_PATH%\ExternalLoader\%EXT_LOADER%.stldr
@IF NOT EXIST %STM32_EXT_FLASH_LOADER% @ECHO %STM32_EXT_FLASH_LOADER% Does not exist !! && GOTO goError

TITLE STM32CubeProgrammer Utility for NG0B1_GFX01M2_TGFX

:: Add STM32CubeProgrammer to the PATH
@SET PATH=%STM32_PROGRAMMER_PATH%;%PATH%

@ECHO.
@ECHO =================================================
@ECHO Erase and Flash all memories and reboot the board
@ECHO =================================================
@ECHO. 
@ECHO Using %STM32_EXT_FLASH_LOADER%
@ECHO. 
STM32_Programmer_CLI.exe -c port=SWD index=%BOARD_ID% reset=HWrst -el %STM32_EXT_FLASH_LOADER% -d %HEX_FILE% -v -HardRst -vb 1
@IF NOT ERRORLEVEL 0 (
  @GOTO goError
)

@GOTO goOut

:goError
@SET RETERROR=%ERRORLEVEL%
@COLOR 0C
@ECHO.
@ECHO Failure Reason Given is %RETERROR%
@PAUSE
@COLOR 07
@EXIT /b %RETERROR%

:goOut
PAUSE