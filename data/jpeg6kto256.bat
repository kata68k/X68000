ECHO 画像を１６色に変換して保存します
jpeged -L0,0 %1
PS16G -512 -256
APICG -S0,0,255,255 %2
APICG -PP %2
