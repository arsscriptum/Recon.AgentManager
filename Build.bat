@echo off
setlocal EnableDelayedExpansion

:: ==============================================================================
:: 
::      Build.bat
::
::      Build different configuration of the app
::
:: ==============================================================================
::   arsccriptum - made in quebec 2020 <guillaumeplante.qc@gmail.com>
:: ==============================================================================

goto :init

:init
    set "__scripts_root=%AutomationScriptsRoot%"
    call :read_script_root development\build-automation  BuildAutomation
    set "__script_file=%~0"
    set "__target_platform=%~2"
    set "__target_configuration=%~1"
    set "__script_path=%~dp0"
    set "__makefile=%__scripts_root%\make\make.bat"
    set "__lib_date=%__scripts_root%\batlibs\date.bat"
    set "__lib_out=%__scripts_root%\batlibs\out.bat"
    ::*** This is the important line ***
    set "__build_cfg_server=%__script_path%servercfg.ini"
    set "__build_cfg_client=%__script_path%clientcfg.ini"
    set "__bin_path=%__script_path%bin"
    set "__tmp_path=%__script_path%vs\___temp_compilation_files"
    set "__build_cancelled=0"
    goto :validate


:header
    echo. %__script_name% v%__script_version%
    echo.    This script is part of arsccriptum build wrappers.
    echo.
    goto :eof

:header_err
    echo.**************************************************
    echo.This script is part of arsccriptum build wrappers.
    echo.**************************************************
    echo.
    echo. YOU NEED TO HAVE THE BuildAutomation Scripts setup on you system...
    echo. https://github.com/arsccriptum/BuildAutomation
    goto :eof


:read_script_root
    set regpath=%OrganizationHKCU::=%
    for /f "tokens=2,*" %%A in ('REG.exe query %regpath%\%1 /v %2') do (
            set "__scripts_root=%%B"
        )
    goto :eof

:validate
    if not defined __scripts_root          call :header_err && call :error_missing_path __scripts_root & goto :eof
    if not exist %__makefile%  call :error_missing_script "%__makefile%" & goto :eof
    if not exist %__lib_date%  call :error_missing_script "%__lib_date%" & goto :eof
    if not exist %__lib_out%  call :error_missing_script "%__lib_out%" & goto :eof
    if not exist %__build_cfg_server%  call :error_missing_script "%__build_cfg_server%" & goto :eof
    if not exist %__build_cfg_client%  call :error_missing_script "%__build_cfg_client%" & goto :eof
    goto :test




:prebuild_header
    call %__lib_date% :getbuilddate
    call %__lib_out% :__out_d_red " ======================================================================="
    call %__lib_out% :__out_l_red " Compilation started for %cd%  %__target_configuration% %__target_platform%"  
    call %__lib_out% :__out_d_red " ======================================================================="
    goto :eof


:prebuild_clean
    call %__lib_out% :__out_d_yel " ======================================================================="
    call %__lib_out% :__out_l_yel "                              CLEANING                                  "  
    call %__lib_out% :__out_d_yel " ======================================================================="
    call %__lib_out% :__out_n_d_cya "DELETE "  
    call %__lib_out% :__out_d_grn "%__tmp_path%"  
    rmdir /Q /S %__tmp_path% 2> NUL
    call %__lib_out% :__out_n_d_cya "DELETE "  
    call %__lib_out% :__out_d_grn "%__bin_path%"  
    rmdir /Q /S %__bin_path% 2> NUL

    goto :eof

:: ==============================================================================
::   call make
:: ==============================================================================
:call_make_build_client
    set config=%1
    set platform=%2
    call %__makefile% /v /i %__build_cfg_client% /t Build /c %config% /p %platform%
    goto :finished

:call_make_build_server
    set config=%1
    set platform=%2
    call %__makefile% /v /i %__build_cfg_server% /t Build /c %config% /p %platform%
    goto :finished


:: =============================================================================
::   Build Win32
:: ==============================================================================

:build_client_debug
    call :call_make_build_client Debug Win32
    goto :eof

:build_client_release
    call :call_make_build_client Release Win32
    goto :eof

:build_server_debug
    call :call_make_build_server Debug Win32
    goto :eof

:build_server_release
    call :call_make_build_server Release Win32
    goto :eof


:: ==============================================================================
::   clean all
:: ==============================================================================
:clean
    call %__lib_out% :__out_l_red " CLEANING SERVER DEBUG"  
    call %__makefile% /v /i %__build_cfg_server% /t Clean /c Debug /p x86
    call %__lib_out% :__out_l_red " CLEANING SERVER RELEASE"  
    call %__makefile% /v /i %__build_cfg_server% /t Clean /c Release /p x86
    call %__lib_out% :__out_l_red " CLEANING CLIENT DEBUG"  
    call %__makefile% /v /i %__build_cfg_client% /t Clean /c Debug /p x86
    call %__lib_out% :__out_l_red " CLEANING CLIENT RELEASE"  
    call %__makefile% /v /i %__build_cfg_client% /t Clean /c Release /p x86
    goto :eof


:: ==============================================================================
::   Build
:: ==============================================================================
:build_debug
    call :build_client_debug
    call :build_server_debug
    goto :eof

:build_release
    call :build_client_release
    call :build_server_release
    goto :eof

:test 
    if "%__target_configuration%" == "" ( set __target_configuration=Release )
    if "%__target_platform%" == "" ( set __target_platform=x64 )
    call :prebuild_header
    call :build
    goto :eof

:build
    call :call_make_build_server %__target_configuration% %__target_platform%
    goto :eof

:error_missing_path
    echo.
    echo   Error
    echo    Missing path: %~1
    echo.
    goto :eof



:error_missing_script
    echo.
    echo    Error
    echo    Missing bat script: %~1
    echo.
    goto :eof



:finished
    call %__lib_out% :__out_d_grn "Build complete"
