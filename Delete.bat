rmdir /s /q "ipch"
rmdir /s /q "Debug"
rmdir /s /q "Release"

rmdir /s /q .\Client\Default\Debug
rmdir /s /q .\Client\Default\Release

rmdir /s /q .\Engine\Default\Debug
rmdir /s /q .\Client\Default\Release

rmdir /s /q .\Reference\Headers

del /f /q .\Client\Bin\Client.exe
del /f /q .\Client\Bin\Client.ilk
del /f /q .\Client\Bin\Client.pdb
del /f /q  HarryPotter.VC.db