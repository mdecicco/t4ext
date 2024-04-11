@echo off

set /p class_name="Enter class name: "
set /p dir_name="Enter dir name: "

set header_name="%class_name%.h"
set src_name="%class_name%.cpp"

set printf="C:\Program Files\Git\usr\bin\printf.exe"

IF "%dir_name%" NEQ "" (
    cd "./src/%dir_name%"
    %printf% "#pragma once\n" >> %header_name%
    %printf% "#include <types.h>\n\n" >> %header_name%
    %printf% "namespace t4ext {\n" >> %header_name%
    %printf% "    class %class_name% {\n" >> %header_name%
    %printf% "        public:\n" >> %header_name%
    %printf% "            %class_name%();\n" >> %header_name%
    %printf% "            ~%class_name%();\n" >> %header_name%
    %printf% "    };\n" >> %header_name%
    %printf% "};" >> %header_name%

    %printf% "#include <%dir_name%/%header_name%>\n\n" >> %src_name%
    %printf% "namespace t4ext {\n" >> %src_name%
    %printf% "    %class_name%::%class_name%() {\n" >> %src_name%
    %printf% "    }\n" >> %src_name%
    %printf% "\n" >> %src_name%
    %printf% "    %class_name%::~%class_name%() {\n" >> %src_name%
    %printf% "    }\n" >> %src_name%
    %printf% "};" >> %src_name%

    cd "../../"
)

IF "%dir_name%" == "" (
    cd "./src"
    %printf% "#pragma once\n" >> %header_name%
    %printf% "#include <types.h>\n\n" >> %header_name%
    %printf% "namespace t4ext {\n" >> %header_name%
    %printf% "    class %class_name% {\n" >> %header_name%
    %printf% "        public:\n" >> %header_name%
    %printf% "            %class_name%();\n" >> %header_name%
    %printf% "            ~%class_name%();\n" >> %header_name%
    %printf% "    };\n" >> %header_name%
    %printf% "};" >> %header_name%

    %printf% "#include <%header_name%>\n\n" >> %src_name%
    %printf% "namespace t4ext {\n" >> %src_name%
    %printf% "    %class_name%::%class_name%() {\n" >> %src_name%
    %printf% "    }\n" >> %src_name%
    %printf% "\n" >> %src_name%
    %printf% "    %class_name%::~%class_name%() {\n" >> %src_name%
    %printf% "    }\n" >> %src_name%
    %printf% "};" >> %src_name%

    cd "../"
)

copy /b CMakeLists.txt +,,