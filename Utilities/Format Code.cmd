for /R %%i in (..\EXE\*.cpp) do Uncrustify\uncrustify.exe -c Uncrustify\max.cfg --no-backup "%%i"
for /R %%i in (..\EXE\*.h) do Uncrustify\uncrustify.exe -c Uncrustify\max.cfg --no-backup "%%i"
for /R %%i in (..\DLL\*.cpp) do Uncrustify\uncrustify.exe -c Uncrustify\max.cfg --no-backup "%%i"
for /R %%i in (..\DLL\*.h) do Uncrustify\uncrustify.exe -c Uncrustify\max.cfg --no-backup "%%i"