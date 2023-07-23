//명령어(파일/디렉터리 복사)	옵션(덮어쓰기 물어보지 않음)		원본 파일이 있는 위치						사본 파일을 저장할 위치
xcopy						/y								.\Engine\Bin\Engine.dll					.\Client\Bin\

xcopy						/y								.\Engine\Bin\Engine.lib					.\Reference\Librarys\
xcopy						/y								.\Engine\ThirdPartyLib\*.lib			.\Reference\Librarys\
xcopy						/y/s							.\Engine\Public\*.*						.\Reference\Headers\
xcopy						/y								.\Engine\Bin\ShaderFiles\*.hlsl			.\Reference\ShaderFiles\
xcopy						/y								.\Engine\Bin\ShaderFiles\*.hpp			.\Reference\ShaderFiles\

xcopy						/y								.\Engine\Bin\Engine.dll					.\Tool\Bin\

