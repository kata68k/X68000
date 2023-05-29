ECHO OFF
JOYDRV3.X data\JOYDRV3.CNF
rem PCM8A.X
rem MACSDRV.X
rem MOON.x -f10 -m640
ZMUSIC.X -M -N -T64 -W16 -Sdata\music\68SND.ZMS -Bdata\se\BK.ZPD
BattleKata.x
ZMUSIC.X -R
rem MOON.x -r
rem MACSDRV.X -r
rem PCM8A.X -r
JOYDRV3.X -R
