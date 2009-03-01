#include "proto.h"

extern int comments;

void comment(unsigned char low, unsigned char high)
{
  unsigned int addr = high * 256 + low;

if (comments)
{
  printf("	; ");
  switch (addr)
  {
    case 0x2100: printf("Screen Display Register");
      if (comments > 1) { spaces(10);
        printf(";a0000bbbb a: 0=screen on, 1=screen off  b = brightness");
      } break;
    case 0x2101: printf("OAM Size and Data Area Designation");
      if (comments > 1) { spaces(10);
        printf(";aaabbccc a = Size  b = Name Selection  c = Base Selection");
      } break;
    case 0x2102: printf("Address for Accessing OAM"); break;
    case 0x2104: printf("OAM Data Write"); break;
    case 0x2105: printf("BG Mode and Tile Size Setting");
      if (comments > 1) { spaces(10);
        printf(";abcdefff abcd = BG tile size (4321), 0 = 8x8, 1 = 16x16");
        spaces(10);  printf(";e = BG 3 High Priority  f = BG Mode");
    } break;
    case 0x2106: printf("Mosaic Size and BG Enable");
      if (comments > 1) { spaces(10);
        printf(";aaaabbbb a = Mosaic Size  b = Mosaic BG Enable");
      } break;
    case 0x2107: printf("BG 1 Address and Size");
      if (comments > 1) { spaces(10);
    printf(";aaaaaabb a = Screen Base Address (Upper 6-bit)  b = Screen Size");
    } break;
    case 0x2108: { printf("BG 2 Address and Size");
      if (comments > 1) { spaces(10);
    printf(";aaaaaabb a = Screen Base Address (Upper 6-bit).  b = Screen Size");
    } break;
    case 0x2109: printf("BG 3 Address and Size");
      if (comments > 1) { spaces(10);
    printf(";aaaaaabb a = Screen Base Address (Upper 6-bit)  b = Screen Size");
    } break;
    case 0x210A: printf("BG 4 Address and Size");
      if (comments > 1) { spaces(10);
    printf(";aaaaaabb a = Screen Base Address (Upper 6-bit)  b = Screen Size");
    } break;
    case 0x210b: printf("BG 1 & 2 Tile Data Designation");
      if (comments > 1) { spaces(10);
    printf(";aaaabbbb a = BG 2 Tile Base Address  b = BG 1 Tile Base Address");
    } break;
    case 0x210c: printf("BG 3 & 4 Tile Data Designation");
      if (comments > 1) { spaces(10);
    printf(";aaaabbbb a = BG 4 Tile Base Address  b = BG 3 Tile Base Address");
    } break;
    case 0x210d: printf("BG 1 Horizontal Scroll Offset"); break;
    case 0x210e: printf("BG 1 Vertical Scroll Offset"); break;
    case 0x210f: printf("BG 2 Horizontal Scroll Offset"); break;
    case 0x2110: printf("BG 2 Vertical Scroll Offset"); break;
    case 0x2111: printf("BG 3 Horizontal Scroll Offset"); break;
    case 0x2112: printf("BG 3 Vertical Scroll Offset"); break;
    case 0x2113: printf("BG 4 Horizontal Scroll Offset"); break;
    case 0x2114: printf("BG 4 Vertical Scroll Offset"); break;
    case 0x2115: printf("VRAM Address Increment Value"); break;
    case 0x2116: printf("Address for VRAM Read/Write (Low Byte)"); break;
    case 0x2117: printf("Address for VRAM Read/Write (High Byte)"); break;
    case 0x2118: printf("Data for VRAM Write (Low Byte)"); break;
    case 0x2119: printf("Data for VRAM Write (High Byte)"); break;
    case 0x211a: printf("Initial Setting for Mode 7");
      if (comments > 1) { spaces(10);
    printf(";aa0000bc a = Screen Over  b = Vertical Flip  c = Horizontal Flip");
      } break;
    case 0x211b: printf("Mode 7 Matrix Parameter A"); break;
    case 0x211c: printf("Mode 7 Matrix Parameter B"); break;
    case 0x211d: printf("Mode 7 Matrix Parameter C"); break;
    case 0x211e: printf("Mode 7 Matrix Parameter D"); break;
    case 0x211f: printf("Mode 7 Center Position X"); break;
    case 0x2120: printf("Mode 7 Center Position Y"); break;
    case 0x2121: printf("Address for CG-RAM Write"); break;
    case 0x2122: printf("Data for CG-RAM Write"); break;
    case 0x2123: printf("BG 1 and 2 Window Mask Settings");
      if (comments > 1) { spaces(10);
    printf(";aaaabbbb a = BG 2 Window Settings  b = BG 1 Window Settings");
      } break;
    case 0x2124: printf("BG 3 and 4 Window Mask Settings");
      if (comments > 1) { spaces(10);
    printf(";aaaabbbb a = BG 4 Window Settings  b = BG 3 Window Settings");
      } break;
    case 0x2125: printf("OBJ and Color Window Settings");
      if (comments > 1) { spaces(10);
    printf(";aaaabbbb a = Color Window Settings  b = OBJ Window Settings");
      } break;
    case 0x2126: printf("Window 1 Left Position Designation"); break;
    case 0x2127: printf("Window 1 Right Position Designation"); break;
    case 0x2128: printf("Window 2 Left Postion Designation"); break;
    case 0x2129: printf("Window 2 Right Postion Designation"); break;
    case 0x212a: printf("BG 1, 2, 3 and 4 Window Logic Settings");
      if (comments > 1) { spaces(10);
      printf(";aabbccdd a = BG 4  b = BG 3  c = BG 2  d = BG 1");
      } break;
    case 0x212b: printf("Color and OBJ Window Logic Settings");
      if (comments > 1) { spaces(10);
        printf(";0000aabb a = Color Window  b = OBJ Window");
      } break;
    case 0x212c: printf("Background and Object Enable");
      if (comments > 1) { spaces(10);
   printf(";000abcde a = Object  b = BG 4  c = BG 3  d = BG 2  e = BG 1");
      } break;
    case 0x212d: printf("Sub Screen Designation");
      if (comments > 1) { spaces(10);
    printf(";000abcde a = Object  b = BG 4  c = BG 3  d = BG 2  e = BG 1");
      } break;
    case 0x212e: printf("Window Mask Designation for Main Screen");
      if (comments > 1) { spaces(10);
    printf(";000abcde a = Object  b = BG 4  c = BG 3  d = BG 2  e = BG 1");
      } break;
    case 0x212f: printf("Window Mask Designation for Sub Screen");
      if (comments > 1) { spaces(10);
    printf(";000abcde a = Object  b = BG 4  c = BG 3  d = BG 2  e = BG 1");
      } break;
    case 0x2130: printf("Initial Settings for Color Addition");
      if (comments > 1) { spaces(10);
    printf(";aabb00cd a = Main Color Window On/Off b = Sub Color Window On/Off");
    spaces(10); printf(";c = Fixed Color Add/Subtract Enable  d = Direct Select");
      } break;
    case 0x2131: printf("Add/Subtract Select and Enable");
      if (comments > 1) { spaces(10);
    printf(";abcdefgh  a = 0 = Addition, 1 = Subtraction  b = 1/2 Enable");
    spaces(10);
    printf(";cdefgh = Enables  c = Back, d = Object, efgh = BG 4, 3, 2, 1");
      } break;
    case 0x2132: printf("Fixed Color Data");
      if (comments > 1) { spaces(10);
      printf(";abcddddd a = Blue  b = Green  c = Red  dddd = Color Data");
      } break;
    case 0x2133: printf("Screen Initial Settings");
      if (comments > 1) { spaces(10);
    printf(";ab00cdef a = External Sync  b = ExtBG Mode  c = Pseudo 512 Mode");
    spaces(10); printf(";d = Vertical Size  e = Object-V Select  f = Interlace");
      } break;
    case 0x2134: printf("Multiplication Result (Low Byte)"); break;
    case 0x2135: printf("Multiplication Result (Mid Byte)"); break;
    case 0x2136: printf("Multiplication Result (High Byte)"); break;
    case 0x2137: printf("Software Latch for H/V Counter"); break;
    case 0x2138: printf("Read Data from OAM (Low-High)"); break;
    case 0x2139: printf("Read Data from VRAM (Low)"); break;
    case 0x213a: printf("Read Data from VRAM (High)"); break;
    case 0x213b: printf("Read Data from CG-RAM (Low-High)"); break;
    case 0x213c: printf("H-Counter Data"); break;
    case 0x213d: printf("V-Counter Data"); } break;
    case 0x213e: case 0x213f: printf("PPU Status Flag"); break;
    case 0x2140: case 0x2141: case 0x2142: case 0x2143:
      printf("APU I/O Port"); break;
    case 0x4200: printf("NMI, V/H Count, and Joypad Enable");
      if (comments > 1) { spaces(10);
    printf(";a0bc000d a = NMI  b = V-Count  c = H-Count  d = Joypad");
     } break;
    case 0x4201: printf("Programmable I/O Port Output"); break;
    case 0x4202: printf("Multiplicand A"); break;
    case 0x4203: printf("Multplier B"); break;
    case 0x4204: printf("Dividend (Low Byte)"); break;
    case 0x4205: printf("Dividend (High-Byte)"); break;
    case 0x4206: printf("Divisor B"); break;
    case 0x4207: printf("H-Count Timer (Upper 8 Bits)"); break;
    case 0x4208: printf("H-Count Timer MSB (Bit 0)"); break;
    case 0x4209: printf("V-Count Timer (Upper 8 Bits)"); break;
    case 0x420a: printf("V-Count Timer MSB (Bit 0)"); break;
    case 0x420b: printf("Regular DMA Channel Enable");
      if (comments > 1) { spaces(10);
    printf(";abcdefgh  a = Channel 7 .. h = Channel 0: 0 = Enable  1 = Disable");
      } break;
    case 0x420c: printf("H-DMA Channel Enable");
      if (comments > 1) { spaces(10);
    printf(";abcdefgh  a = Channel 7 .. h = Channel 0: 0 = Enable  1 = Disable");
      } break;
    case 0x420d: printf("Cycle Speed Designation");
      if (comments > 1) { spaces(10);
        printf(";0000000a a: 0 = 2.68 MHz, 1 = 3.58 MHz"); } break;
    case 0x4210: printf("NMI Enable");
      if (comments > 1) { spaces(10);
        printf(";a0000000 a: 0 = Disabled, 1 = Enabled"); } break;
    case 0x4211: printf("IRQ Flag By H/V Count Timer");
      if (comments > 1) { spaces(10);
      printf(";a0000000 a: 0 = H/V Timer Disabled, 1 = H/V Timer is Time Up");
      } break;
    case 0x4212: printf("H/V Blank Flags and Joypad Status");
      if (comments > 1) { spaces(10);
    printf(";ab00000c a = V Blank  b = H Blank  c = Joypad Ready to Be Read");
      } break;
    case 0x4213: printf("Programmable I/O Port Input"); break;
    case 0x4214: printf("Quotient of Divide Result (Low Byte)"); break;
    case 0x4215: printf("Quotient of Divide Result (High Byte)"); break;
    case 0x4216: printf("Product/Remainder Result (Low Byte)"); break;
    case 0x4217: printf("Product/Remainder Result (High Byte)"); break;
    case 0x4218: case 0x421a: case 0x421c: case 0x421e:
      printf("Joypad %d Data (Low Byte)",(addr-0x4217)/2+1);
        if (comments > 1) { spaces(10);
        printf(";abcd0000 a = Button A  b = X  c = L  d = R"); } break;
    case 0x4219: case 0x421b: case 0x421d: case 0x421f:
      printf("Joypad %d Data (High Byte)",(addr-0x4218)/2+1);
        if (comments > 1) { spaces(10);
    printf(";abcdefgh a = B b = Y c = Select d = Start efgh = Up/Dn/Lt/Rt");
      } break;
    case 0x4300: case 0x4310: case 0x4320: case 0x4330: case 0x4340:
    case 0x4350: case 0x4360: case 0x4370:
      printf("Parameters for DMA Transfer");
      if (comments > 1) { spaces(10);
    printf(";ab0cdeee a = Direction  b = Type  c = Inc/Dec  d = Auto/Fixed");
    spaces(10); printf(";e = Word Size Select");
      } break;
    case 0x4301: case 0x4311: case 0x4321: case 0x4331: case 0x4341:
    case 0x4351: case 0x4361: case 0x4371:
      printf("B Address"); break;
    case 0x4302: case 0x4312: case 0x4322: case 0x4332: case 0x4342:
    case 0x4352: case 0x4362: case 0x4372:
      printf("A Address (Low Byte)"); break;
    case 0x4303: case 0x4313: case 0x4323: case 0x4333: case 0x4343:
    case 0x4353: case 0x4363: case 0x4373:
      printf("A Address (High Byte)"); break;
    case 0x4304: case 0x4314: case 0x4324: case 0x4334: case 0x4344:
    case 0x4354: case 0x4364: case 0x4374:
      printf("A Address Bank"); break;
    case 0x4305: case 0x4315: case 0x4325: case 0x4335: case 0x4345:
    case 0x4355: case 0x4365: case 0x4375:
      printf("Number Bytes to Transfer (Low Byte) (DMA)"); break;
    case 0x4306: case 0x4316: case 0x4326: case 0x4336: case 0x4346:
    case 0x4356: case 0x4366: case 0x4376:
      printf("Number Bytes to Transfer (High Byte) (DMA)"); break;
    case 0x4307: case 0x4317: case 0x4327: case 0x4337: case 0x4347:
    case 0x4357: case 0x4367: case 0x4377:
      printf("Data Bank (H-DMA)"); break;
    case 0x4308: case 0x4318: case 0x4328: case 0x4338: case 0x4348:
    case 0x4358: case 0x4368: case 0x4378:
      printf("A2 Table Address (Low Byte)"); break;
    case 0x4309: case 0x4319: case 0x4329: case 0x4339: case 0x4349:
    case 0x4359: case 0x4369: case 0x4379:
      printf("A2 Table Address (High Byte)"); break;
    case 0x430a: case 0x431a: case 0x432a: case 0x433a: case 0x434a:
    case 0x435a: case 0x436a: case 0x437a:
      printf("Number of Lines to Transfer (H-DMA)"); break;

  }
 }
}

