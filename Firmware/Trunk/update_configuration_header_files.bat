@echo off
cd configuration_editor\mcc
java -jar mcc-1.1.1.jar -ghf configuration_atmega8.xml ..\..\configuration_atmega8.h
java -jar mcc-1.1.1.jar -ghf configuration_atmega8.xml ..\..\configuration_atmega168.h
pause