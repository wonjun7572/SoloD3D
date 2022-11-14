xcopy			/y			.\Engine\Bin\Engine.lib			.\Reference\Librarys\

xcopy			/y			.\Engine\Bin\Engine.dll			.\Client\Bin\Debug\
xcopy			/y			.\Engine\Bin\Engine.dll			.\Client\Bin\Release\

xcopy			/y/s		.\Engine\Public\*.*				.\Reference\Headers\
xcopy			/y			.\Engine\ImGui\*.h				.\Reference\Headers\