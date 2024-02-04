#define T0OBJ 0
#define T1OBJ 30
#define FREEBB 0
#define FREEIMG 0
#define FREESTR 81

BYTE *rs_strings[] = {
"   Status:",
"",
"",
"Reading     ",
"",
"",
"    Drive:",
"",
"",
"A",
"     Name:",
"",
"",
"Disk Name   ",
"",
"",
"   Folder:",
"",
"",
"FILENAME.EXT",
"",
"",
"     File:",
"",
"",
"FILENAME.EXT",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"",
"  Button  ",
"  Button  ",
"  Button  ",
"   Quit   ",
"      PRESS UNDO TO ABORT     ",
"",
"",
"Installation Program Copyright ½ 1991, Gribnif Software",
"",
"",
"Blah Blah Installation   ",
"",
"",
" Use During Installation ",
"",
"",
"   Choose the Drives to  ",
"",
"",
"Source Drive:       ",
"Destination Drive:  ",
" Install  ",
"   Quit   "};

LONG rs_frstr[] = {
0};

BITBLK rs_bitblk[] = {
0};

LONG rs_frimg[] = {
0};

ICONBLK rs_iconblk[] = {
0};

TEDINFO rs_tedinfo[] = {
0L, 1L, 2L, 3, 0, 1, 0x1180, 0x0, 0, 11,1,
3L, 4L, 5L, 3, 0, 0, 0x1180, 0x0, 0, 13,1,
6L, 7L, 8L, 3, 0, 1, 0x1180, 0x0, 0, 11,1,
10L, 11L, 12L, 3, 0, 1, 0x1180, 0x0, 0, 11,1,
13L, 14L, 15L, 3, 0, 0, 0x1180, 0x0, 0, 13,1,
16L, 17L, 18L, 3, 0, 1, 0x1180, 0x0, 0, 11,1,
19L, 20L, 21L, 3, 0, 0, 0x1180, 0x0, 0, 13,1,
22L, 23L, 24L, 3, 0, 1, 0x1180, 0x0, 0, 11,1,
25L, 26L, 27L, 3, 0, 0, 0x1180, 0x0, 0, 13,1,
28L, 29L, 30L, 3, 6, 0, 0x1180, 0x0, -1, 1,1,
31L, 32L, 33L, 3, 6, 0, 0x1180, 0x0, -1, 1,1,
34L, 35L, 36L, 3, 6, 0, 0x1180, 0x0, -1, 1,1,
37L, 38L, 39L, 3, 6, 0, 0x1180, 0x0, -1, 1,1,
40L, 41L, 42L, 3, 6, 0, 0x1180, 0x0, -1, 1,1,
43L, 44L, 45L, 3, 6, 0, 0x1180, 0x0, -1, 1,1,
46L, 47L, 48L, 3, 6, 0, 0x1180, 0x0, -1, 1,1,
49L, 50L, 51L, 3, 6, 0, 0x1180, 0x0, -1, 1,1,
52L, 53L, 54L, 3, 6, 0, 0x1180, 0x0, -1, 1,1,
55L, 56L, 57L, 3, 6, 0, 0x1180, 0x0, -1, 1,1,
62L, 63L, 64L, 5, 6, 0, 0x1100, 0x0, -1, 31,1,
65L, 66L, 67L, 5, 6, 0, 0x1200, 0x0, -1, 56,1,
68L, 69L, 70L, 3, 6, 2, 0x1180, 0x0, -1, 26,1,
71L, 72L, 73L, 3, 0, 2, 0x1180, 0x0, 0, 26,1,
74L, 75L, 76L, 3, 0, 2, 0x1180, 0x0, 0, 26,1};

OBJECT rs_object[] = {
-1, 1, 29, G_BOX, NONE, SHADOWED, 0x11100L, 0,0, 58,21,
12, 2, 11, G_BOX, NONE, 0x30, 0xFF1140L, 1041,1, 23,5,
3, -1, -1, G_TEXT, NONE, NORMAL, 0x0L, 0,0, 10,1,
4, -1, -1, G_TEXT, NONE, NORMAL, 0x1L, 11,0, 12,1,
5, -1, -1, G_TEXT, NONE, NORMAL, 0x2L, 0,1, 10,1,
6, -1, -1, G_STRING, NONE, NORMAL, 0x9L, 11,1, 1,1,
7, -1, -1, G_TEXT, NONE, NORMAL, 0x3L, 0,2, 10,1,
8, -1, -1, G_TEXT, NONE, NORMAL, 0x4L, 11,2, 12,1,
9, -1, -1, G_TEXT, NONE, NORMAL, 0x5L, 0,3, 10,1,
10, -1, -1, G_TEXT, NONE, NORMAL, 0x6L, 11,3, 12,1,
11, -1, -1, G_TEXT, NONE, NORMAL, 0x7L, 0,4, 10,1,
1, -1, -1, G_TEXT, NONE, NORMAL, 0x8L, 11,4, 12,1,
23, 13, 22, G_BOX, NONE, NORMAL, 0xFF1140L, 1,7, 56,10,
14, -1, -1, G_TEXT, NONE, NORMAL, 0x9L, 1,0, 54,1,
15, -1, -1, G_TEXT, NONE, NORMAL, 0xAL, 1,1, 54,1,
16, -1, -1, G_TEXT, NONE, NORMAL, 0xBL, 1,2, 54,1,
17, -1, -1, G_TEXT, NONE, NORMAL, 0xCL, 1,3, 54,1,
18, -1, -1, G_TEXT, NONE, NORMAL, 0xDL, 1,4, 54,1,
19, -1, -1, G_TEXT, NONE, NORMAL, 0xEL, 1,5, 54,1,
20, -1, -1, G_TEXT, NONE, NORMAL, 0xFL, 1,6, 54,1,
21, -1, -1, G_TEXT, NONE, NORMAL, 0x10L, 1,7, 54,1,
22, -1, -1, G_TEXT, NONE, NORMAL, 0x11L, 1,8, 54,1,
12, -1, -1, G_TEXT, NONE, NORMAL, 0x12L, 1,9, 54,1,
27, 24, 26, G_BOX, NONE, NORMAL, 0x1170L, 3,2065, 36,2,
25, -1, -1, G_BUTTON, 0x5, NORMAL, 0x3AL, 1,2048, 10,1,
26, -1, -1, G_BUTTON, 0x5, NORMAL, 0x3BL, 13,2048, 10,1,
23, -1, -1, G_BUTTON, 0x5, NORMAL, 0x3CL, 25,2048, 10,1,
28, -1, -1, G_BUTTON, 0x5, NORMAL, 0x3DL, 1067,18, 10,1,
29, -1, -1, G_TEXT, NONE, NORMAL, 0x13L, 18,2067, 1046,1,
0, -1, -1, G_TEXT, LASTOB, NORMAL, 0x14L, 8,20, 42,1,
-1, 1, 28, G_BOX, NONE, SHADOWED, 0x11100L, 0,0, 1052,17,
2, -1, -1, G_TEXT, NONE, NORMAL, 0x15L, 2,1, 25,1,
5, 3, 4, G_BOX, NONE, SHADOWED, 0xFF1140L, 2,3, 25,2,
4, -1, -1, G_TEXT, NONE, NORMAL, 0x16L, 0,1, 25,1,
2, -1, -1, G_TEXT, NONE, NORMAL, 0x17L, 0,0, 25,1,
6, -1, -1, G_STRING, NONE, NORMAL, 0x4DL, 1028,6, 20,1,
9, 7, 8, G_IBOX, NONE, NORMAL, 0x1100L, 1035,7, 6,3,
8, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x41FF1100L, 1,1, 2,1,
6, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x42FF1100L, 3,1, 2,1,
10, -1, -1, G_STRING, NONE, NORMAL, 0x4EL, 1028,10, 20,1,
27, 11, 26, G_IBOX, NONE, NORMAL, 0x1140L, 1028,11, 19,4,
12, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x41FF1100L, 2,1, 2,1,
13, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x49FF1100L, 2,2, 2,1,
14, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x42FF1100L, 4,1, 2,1,
15, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x4AFF1100L, 4,2, 2,1,
16, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x43FF1100L, 6,1, 2,1,
17, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x4BFF1100L, 6,2, 2,1,
18, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x44FF1100L, 8,1, 2,1,
19, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x4CFF1100L, 8,2, 2,1,
20, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x45FF1100L, 10,1, 2,1,
21, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x4DFF1100L, 10,2, 2,1,
22, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x46FF1100L, 12,1, 2,1,
23, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x4EFF1100L, 12,2, 2,1,
24, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x47FF1100L, 14,1, 2,1,
25, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x4FFF1100L, 14,2, 2,1,
26, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x48FF1100L, 16,1, 2,1,
10, -1, -1, G_BOXCHAR, 0x11, NORMAL, 0x50FF1100L, 16,2, 2,1,
28, -1, -1, G_BUTTON, 0x7, NORMAL, 0x4FL, 3,15, 10,1,
0, -1, -1, G_BUTTON, 0x25, NORMAL, 0x50L, 1039,15, 10,1};

LONG rs_trindex[] = {
0L,
30L};

struct foobar {
	WORD	dummy;
	WORD	*image;
	} rs_imdope[] = {
0};

#define NUM_STRINGS 81
#define NUM_FRSTR 0
#define NUM_IMAGES 0
#define NUM_BB 0
#define NUM_FRIMG 0
#define NUM_IB 0
#define NUM_TI 24
#define NUM_OBS 59
#define NUM_TREE 2

BYTE pname[] = "INSTALL.RSC";