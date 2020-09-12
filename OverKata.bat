ECHO OFF
rem PCM8A.X
rem ZMUSIC.X -M -N -P128 -W0 -B data\se\OverKata.ZPD
rem ZMSC.X -M -N  -P128 -W0 -B data\se\OverKata.ZPD
MPCM.X
ZMSC3.X -J -F4 -N
OverKata.x
ZMSC3.X -J -R
MPCM.X /r
rem ZMSC.X -R
rem ZMUSIC.X -R
rem PCM8A.X -R
