rem forfiles /P data /S /M *_in.txt /C "..\x64\Release\spell-checker.exe @file @fname.out.txt" 

set app=..\x64\Release\spell-checker.exe
%app% data\01_assingment_in.txt   out\01_assingment_out.txt
%app% data\02_empty_in.txt        out\02_empty_out.txt
%app% data\03_empty_text_in.txt   out\03_empty_text_out.txt
%app% data\04_empty_dict_in.txt   out\04_empty_dict_out.txt