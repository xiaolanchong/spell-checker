rem forfiles /P data /S /M *_in.txt /C "..\x64\Release\spell-checker.exe @file @fname.out.txt" 

set app=..\Release\spell-checker.exe
%app% data\01_assingment.in.txt   out\01_assingment.out.txt
%app% data\02_2_consecutive_edits.in.txt out\02_2_consecutive_edits.out.txt
%app% data\03_long_word.in.txt    out\03_long_word.out.txt
%app% data\04_empty.in.txt        out\04_empty.out.txt
%app% data\05_empty_dict.in.txt   out\05_empty_dict.out.txt
%app% data\06_empty_text.in.txt   out\06_empty_text.out.txt
%app% data\07_long_text.in.txt    out\07_long_text.out.txt