rem THIS IS NOT WORKING AND I CAN NOT FIGURE OUT WHY

rem #
rem # This runs MSXSL.EXE to generate two files used by LTKCPP.
rem #
rem # You can download MSXSL.EXE from this URL
rem #   http://www.microsoft.com/downloads/details.aspx?FamilyId=2FB55371-C94E-4373-B0E9-DB4816552E41&displaylang=en
rem #
rem # Or do a websearch for
rem #   MXSXL.EXE download
rem # and choose the Microsoft site
rem #
rem # This BAT script assumes that MSXSL.EXE is in the current directory

set LLRPDEF=..\..\Definitions\Core\llrp-1x0-def.xml

.\msxsl.exe -o out_ltkcpp.h %LLRPDEF% ltkcpp_gen_h.xslt RegistryName='Core'
.\msxsl.exe -o out_ltkcpp.inc %LLRPDEF% ltkcpp_gen_cpp.xslt RegistryName='Core'
