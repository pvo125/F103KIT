srec_cat F103_KIT.bin -bin -exclude 0x1C 0x28 -length-l-e 0x1C 4 -generate 0x20 0x28 -repeat-string F103_KIT -o F103_KIT.bin -bin 
srec_cat F103_KIT.bin -bin -crc32-l-e  -max-addr F103_KIT.bin -bin -o F103_KIT.bin -bin
