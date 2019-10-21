BYTE get_symbol(char chr)
{
BYTE symbol = 0;
switch (chr){
case  '!':
symbol=0x21;
break;
case  '"':
symbol=0x22;
break;
case  '#':
symbol=0x23;
break;
case  '$':
symbol=0x24;
break;
case  '%':
symbol=0x25;
break;
case  '&':
symbol=0x26;
break;
case  '(':
symbol=0x28;
break;
case  ')':
symbol=0x29;
break;
case  '*':
symbol=0x2A;
break;
case  '+':
symbol=0x2B;
break;
case  '-':
symbol=0x2D;
break;
case  '.':
symbol=0x2E;
break;
case  '/':
symbol=0x2F;
break;
case  '0':
symbol=0x30;
break;
case  '1':
symbol=0x31;
break;
case  '2':
symbol=0x32;
break;
case  '3':
symbol=0x33;
break;
case  '4':
symbol=0x34;
break;
case  '5':
symbol=0x35;
break;
case  '6':
symbol=0x36;
break;
case  '7':
symbol=0x37;
break;
case  '8':
symbol=0x38;
break;
case  '9':
symbol=0x39;
break;
case  ':':
symbol=0x3A;
break;
case  ';':
symbol=0x3B;
break;
case  '<':
symbol=0x3C;
break;
case  '=':
symbol=0x3D;
break;
case  '>':
symbol=0x3E;
break;
case  '?':
symbol=0x3F;
break;
case  '@':
symbol=0x40;
break;
case  'A':
symbol=0x41;
break;
case  'B':
symbol=0x42;
break;
case  'C':
symbol=0x43;
break;
case  'D':
symbol=0x44;
break;
case  'E':
symbol=0x45;
break;
case  'F':
symbol=0x46;
break;
case  'G':
symbol=0x47;
break;
case  'H':
symbol=0x48;
break;
case  'I':
symbol=0x49;
break;
case  'J':
symbol=0x4A;
break;
case  'K':
symbol=0x4B;
break;
case  'L':
symbol=0x4C;
break;
case  'M':
symbol=0x4D;
break;
case  'N':
symbol=0x4E;
break;
case  'O':
symbol=0x4F;
break;
case  'P':
symbol=0x50;
break;
case  'Q':
symbol=0x51;
break;
case  'R':
symbol=0x52;
break;
case  'S':
symbol=0x53;
break;
case  'T':
symbol=0x54;
break;
case  'U':
symbol=0x55;
break;
case  'V':
symbol=0x56;
break;
case  'W':
symbol=0x57;
break;
case  'X':
symbol=0x58;
break;
case  'Y':
symbol=0x59;
break;
case  'Z':
symbol=0x5A;
break;
case  '[':
symbol=0x5B;
break;
case  ']':
symbol=0x5D;
break;
case  '^':
symbol=0x5E;
break;
case  '_':
symbol=0x5F;
break;
case  '`':
symbol=0x60;
break;
case  'a':
symbol=0x61;
break;
case  'b':
symbol=0x62;
break;
case  'c':
symbol=0x63;
break;
case  'd':
symbol=0x64;
break;
case  'e':
symbol=0x65;
break;
case  'f':
symbol=0x66;
break;
case  'g':
symbol=0x67;
break;
case  'h':
symbol=0x68;
break;
case  'i':
symbol=0x69;
break;
case  'j':
symbol=0x6A;
break;
case  'k':
symbol=0x6B;
break;
case  'l':
symbol=0x6C;
break;
case  'm':
symbol=0x6D;
break;
case  'n':
symbol=0x6E;
break;
case  'o':
symbol=0x6F;
break;
case  'p':
symbol=0x70;
break;
case  'q':
symbol=0x71;
break;
case  'r':
symbol=0x72;
break;
case  's':
symbol=0x73;
break;
case  't':
symbol=0x74;
break;
case  'u':
symbol=0x75;
break;
case  'v':
symbol=0x76;
break;
case  'w':
symbol=0x77;
break;
case  'x':
symbol=0x78;
break;
case  'y':
symbol=0x79;
break;
case  'z':
symbol=0x7A;
break;
case  '{':
symbol=0x7B;
break;
case  '|':
symbol=0x7C;
break;
case  '}':
symbol=0x7D;
break;
case  '~':
symbol=0x7E;
break;
case  '':
symbol=0x7F;
break;
case  'Ä':
symbol=0x80;
break;
case  'Å':
symbol=0x81;
break;
case  'Ç':
symbol=0x82;
break;
case  'É':
symbol=0x83;
break;
case  'Ñ':
symbol=0x84;
break;
case  'Ö':
symbol=0x85;
break;
case  'Ü':
symbol=0x86;
break;
case  'á':
symbol=0x87;
break;
case  'à':
symbol=0x88;
break;
case  'â':
symbol=0x89;
break;
case  'ä':
symbol=0x8A;
break;
case  'ã':
symbol=0x8B;
break;
case  'å':
symbol=0x8C;
break;
case  'ç':
symbol=0x8D;
break;
case  'é':
symbol=0x8E;
break;
case  'è':
symbol=0x8F;
break;
case  'ê':
symbol=0x90;
break;
case  'ë':
symbol=0x91;
break;
case  'í':
symbol=0x92;
break;
case  'ì':
symbol=0x93;
break;
case  'î':
symbol=0x94;
break;
case  'ï':
symbol=0x95;
break;
case  'ñ':
symbol=0x96;
break;
case  'ó':
symbol=0x97;
break;
case  'ô':
symbol=0x99;
break;
case  'õ':
symbol=0x9B;
break;
case  'ú':
symbol=0x9C;
break;
case  'ù':
symbol=0x9D;
break;
case  'û':
symbol=0x9E;
break;
case  'ü':
symbol=0x9F;
break;
case  '†':
symbol=0xA0;
break;
case  '°':
symbol=0xA1;
break;
case  '¢':
symbol=0xA2;
break;
case  '£':
symbol=0xA3;
break;
case  '§':
symbol=0xA4;
break;
case  '•':
symbol=0xA5;
break;
case  '¶':
symbol=0xA6;
break;
case  'ß':
symbol=0xA7;
break;
case  '®':
symbol=0xA8;
break;
case  '©':
symbol=0xA9;
break;
case  '™':
symbol=0xAA;
break;
case  '´':
symbol=0xAB;
break;
case  '¨':
symbol=0xAC;
break;
case  '≠':
symbol=0xAD;
break;
case  'Æ':
symbol=0xAE;
break;
case  'Ø':
symbol=0xAF;
break;
case  '∞':
symbol=0xB0;
break;
case  '±':
symbol=0xB1;
break;
case  '≤':
symbol=0xB2;
break;
case  '≥':
symbol=0xB3;
break;
case  '¥':
symbol=0xB4;
break;
case  'µ':
symbol=0xB5;
break;
case  '∂':
symbol=0xB6;
break;
case  '∑':
symbol=0xB7;
break;
case  '∏':
symbol=0xB8;
break;
case  'π':
symbol=0xB9;
break;
case  '∫':
symbol=0xBA;
break;
case  'ª':
symbol=0xBB;
break;
case  'º':
symbol=0xBC;
break;
case  'Ω':
symbol=0xBD;
break;
case  'æ':
symbol=0xBE;
break;
case  'ø':
symbol=0xBF;
break;
case  '¿':
symbol=0xC0;
break;
case  '¡':
symbol=0xC1;
break;
case  '¬':
symbol=0xC2;
break;
case  '√':
symbol=0xC3;
break;
case  'ƒ':
symbol=0xC4;
break;
case  '≈':
symbol=0xC5;
break;
case  '∆':
symbol=0xC6;
break;
case  '«':
symbol=0xC7;
break;
case  '»':
symbol=0xC8;
break;
case  '…':
symbol=0xC9;
break;
case  ' ':
symbol=0xCA;
break;
case  'À':
symbol=0xCB;
break;
case  'Ã':
symbol=0xCC;
break;
case  'Õ':
symbol=0xCD;
break;
case  'Œ':
symbol=0xCE;
break;
case  'œ':
symbol=0xCF;
break;
case  '–':
symbol=0xD0;
break;
case  '—':
symbol=0xD1;
break;
case  '“':
symbol=0xD2;
break;
case  '”':
symbol=0xD3;
break;
case  '‘':
symbol=0xD4;
break;
case  '’':
symbol=0xD5;
break;
case  '÷':
symbol=0xD6;
break;
case  '◊':
symbol=0xD7;
break;
case  'ÿ':
symbol=0xD8;
break;
case  'Ÿ':
symbol=0xD9;
break;
case  '⁄':
symbol=0xDA;
break;
case  '€':
symbol=0xDB;
break;
case  '‹':
symbol=0xDC;
break;
case  '›':
symbol=0xDD;
break;
case  'ﬁ':
symbol=0xDE;
break;
case  'ﬂ':
symbol=0xDF;
break;
case  '‡':
symbol=0xE0;
break;
case  '·':
symbol=0xE1;
break;
case  '‚':
symbol=0xE2;
break;
case  '„':
symbol=0xE3;
break;
case  '‰':
symbol=0xE4;
break;
case  'Â':
symbol=0xE5;
break;
case  'Ê':
symbol=0xE6;
break;
case  'Á':
symbol=0xE7;
break;
case  'Ë':
symbol=0xE8;
break;
case  'È':
symbol=0xE9;
break;
case  'Í':
symbol=0xEA;
break;
case  'Î':
symbol=0xEB;
break;
case  'Ï':
symbol=0xEC;
break;
case  'Ì':
symbol=0xED;
break;
case  'Ó':
symbol=0xEE;
break;
case  'Ô':
symbol=0xEF;
break;
case  '':
symbol=0xF0;
break;
case  'Ò':
symbol=0xF1;
break;
case  'Ú':
symbol=0xF2;
break;
case  'Û':
symbol=0xF3;
break;
case  'Ù':
symbol=0xF4;
break;
case  'ı':
symbol=0xF5;
break;
case  'ˆ':
symbol=0xF6;
break;
case  '˜':
symbol=0xF7;
break;
case  '¯':
symbol=0xF8;
break;
case  '˘':
symbol=0xF9;
break;
case  '˙':
symbol=0xFA;
break;
case  '˚':
symbol=0xFB;
break;
case  '¸':
symbol=0xFC;
break;
case  '˝':
symbol=0xFD;
break;
case  '˛':
symbol=0xFE;
break;
case  'ˇ':
symbol=0xFF;
break;
}
return symbol;	
}

void send_string(char line[25])
{
	int i = 0;
	while (line[i] != '\0')
	{
		ssd1306_putchar(get_symbol(line[i]));
		i++;
	}
}