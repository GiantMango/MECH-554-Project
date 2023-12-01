@echo off
set dt=%DATE:~6,4%_%DATE:~3,2%_%DATE:~0,2%
set dt=%dt: =0%
xcopy "Project\Debug\Project.hex" "project.hex"
ren "project.hex" "project_%dt%.hex"
pause