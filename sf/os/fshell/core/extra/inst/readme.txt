These two executables exists to allow fshell to eclipse the ROM based "econs.dll". This can't be done
within the .SIS file because the app installer doesn't allow ROM files to be eclipsed.

fshell_inst.exe copies "<install_drive>:\sys\bin\iocons.dll" to "<install_drive>:\sys\bin\econs.dll".
It is launched by the tool-kit .SIS file at install time.

fshell_uinst.exe deletes "<install_drive>:\sys\bin\econs.dll".
It is launched by the tool-kit .SIS file at uninstall time.

