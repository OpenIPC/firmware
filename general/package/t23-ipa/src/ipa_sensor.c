/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2026 Difan Zhang */
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <linux/v4l2-subdev.h>
#include "t23_ipa.h"

/* log2(gain) in Q16 for each SmartSens SC1A4T V4L2_CID_ANALOGUE_GAIN index */
static const uint32_t sc1a4t_gain_log2[] = {
	0x00000, 0x00b46, 0x01690, 0x0212e, 0x02b80, 0x0358a,
	0x03f9f, 0x04923, 0x05269, 0x05b75, 0x06492, 0x06d30,
	0x0759c, 0x07dd8, 0x08628, 0x08e09, 0x095bf, 0x09d4d,
	0x0a4f1, 0x0ac32, 0x0b34f, 0x0ba49, 0x0c15b, 0x0c814,
	0x0ceae, 0x0d52a, 0x0dbbd, 0x0e200, 0x0e829, 0x0ee37,
	0x0f45d, 0x0fa3a, 0x0ffff, 0x10b45, 0x1168f, 0x1212d,
	0x12b7f, 0x13589, 0x13f9e, 0x14922, 0x15268, 0x15b74,
	0x16491, 0x16d2f, 0x1759b, 0x17dd7, 0x18627, 0x18e08,
	0x195be, 0x19d4c, 0x1a4f0, 0x1ac31, 0x1b34e, 0x1ba48,
	0x1c15a, 0x1c813, 0x1cead, 0x1d529, 0x1dbbc, 0x1e1ff,
	0x1e828, 0x1ee36, 0x1f45c, 0x1fa39, 0x1fffe, 0x20b44,
	0x2168e, 0x2212c, 0x22b7e, 0x23588, 0x23f9d, 0x24921,
	0x25267, 0x25b73, 0x26490, 0x26d2e, 0x2759a, 0x27dd6,
	0x28626, 0x28e07, 0x295bd, 0x29d4b, 0x2a4ef, 0x2ac30,
	0x2b34d, 0x2ba47, 0x2c159, 0x2c812, 0x2ceac, 0x2d528,
	0x2dbbb, 0x2e1fe, 0x2e827, 0x2ee35, 0x2f45b, 0x2fa38,
	0x2fffd, 0x30b43, 0x3168d, 0x3212b, 0x32b7d, 0x33587,
	0x33f9c, 0x34920, 0x35266, 0x35b72, 0x3648f, 0x36d2d,
	0x37599, 0x37dd5, 0x38625, 0x38e06, 0x395bc, 0x39d4a,
	0x3a4ee, 0x3ac2f, 0x3b34c, 0x3ba46, 0x3c158, 0x3c811,
	0x3ceab, 0x3d527, 0x3dbba, 0x3e1fd, 0x3e826, 0x3ee34,
	0x3f45a, 0x3fa37, 0x3fffc, 0x40b42, 0x4212a, 0x42b7c,
	0x43586, 0x43f9b, 0x4491f, 0x45265, 0x45b71, 0x4648e,
	0x46d2c, 0x47598, 0x47dd4, 0x48624, 0x48e05, 0x495bb,
	0x49d49, 0x4a4ed, 0x4ac2e, 0x4b34b, 0x4ba45, 0x4c157,
	0x4c810, 0x4ceaa, 0x4d526, 0x4dbb9, 0x4e1fc, 0x4e825,
	0x4ee33, 0x4f459, 0x4fa36, 0x4fffb, 0x50b41, 0x5168b,
	0x52129, 0x52b7b, 0x53585, 0x53f9a, 0x5491e, 0x55264,
	0x55b70, 0x5648d, 0x56d2b, 0x57597, 0x57dd3, 0x58623,
	0x58e04, 0x595ba, 0x59d48, 0x5a4ec, 0x5ac2d, 0x5b34a,
	0x5ba44, 0x5c156, 0x5c80f, 0x5cea9, 0x5d525, 0x5dbb8,
	0x5e1fb, 0x5e824, 0x5ee32, 0x5f458, 0x5fa35,
};

/* log2(gain) in Q16 for SmartSens SC2336P/SC2337P gain LUT (sensor_sc2336p.c) */
static const uint32_t sc2336_gain_log2[] = {
	0x00000, 0x00b46, 0x01690, 0x0212e, 0x02b80, 0x0358a,
	0x03f9f, 0x04923, 0x05269, 0x05b76, 0x06492, 0x06d31,
	0x0759d, 0x07dd9, 0x08629, 0x08e09, 0x095c0, 0x09d4e,
	0x0a4f2, 0x0ac32, 0x0b350, 0x0ba4a, 0x0c15c, 0x0c814,
	0x0ceae, 0x0d52b, 0x0dbbe, 0x0e201, 0x0e829, 0x0ee38,
	0x0f45e, 0x0fa3b, 0x10000, 0x10b46, 0x11690, 0x1212e,
	0x12b80, 0x1358a, 0x13f9f, 0x14923, 0x15269, 0x15b76,
	0x16492, 0x16d31, 0x1759d, 0x17dd9, 0x18629, 0x18e09,
	0x195c0, 0x19d4e, 0x1a4f2, 0x1ac32, 0x1b350, 0x1ba4a,
	0x1c15c, 0x1c814, 0x1ceae, 0x1d52b, 0x1dbbe, 0x1e201,
	0x1e829, 0x1ee38, 0x1f45e, 0x1fa3b, 0x20000, 0x20b46,
	0x21690, 0x2212e, 0x22b80, 0x2358a, 0x23f9f, 0x24923,
	0x25269, 0x25b76, 0x26492, 0x26d31, 0x2759d, 0x27dd9,
	0x28629, 0x28e09, 0x295c0, 0x29d4e, 0x2a4f2, 0x2ac32,
	0x2b350, 0x2ba4a, 0x2c15c, 0x2c814, 0x2ceae, 0x2d52b,
	0x2dbbe, 0x2e201, 0x2e829, 0x2ee38, 0x2f45e, 0x2fa3b,
	0x30000, 0x30b46, 0x31690, 0x3212e, 0x32b80, 0x3358a,
	0x33f9f, 0x34923, 0x35269, 0x35b76, 0x36492, 0x36d31,
	0x3759d, 0x37dd9, 0x38629, 0x38e09, 0x395c0, 0x39d4e,
	0x3a4f2, 0x3ac32, 0x3b350, 0x3ba4a, 0x3c15c, 0x3c814,
	0x3ceae, 0x3d52b, 0x3dbbe, 0x3e201, 0x3e829, 0x3ee38,
	0x3f45e, 0x3fa3b, 0x40000, 0x40b46, 0x41690, 0x4212e,
	0x42b80, 0x4358a, 0x43f9f, 0x44923, 0x45269, 0x45b76,
	0x46492, 0x46d31, 0x4759d, 0x47dd9, 0x48629, 0x48e09,
	0x495c0, 0x49d4e, 0x4a4f2, 0x4ac32, 0x4b350, 0x4ba4a,
	0x4c15c, 0x4c814, 0x4ceae, 0x4d52b, 0x4dbbe, 0x4e201,
	0x4e829, 0x4ee38, 0x4f45e, 0x4fa3b, 0x50000,
};

/* log2(gain) in Q16 for each GalaxyCore GC2053 gain LUT index (gc2053.c) */
static const uint32_t gc2053_gain_log2[] = {
	0x00000, 0x03fac, 0x081fe, 0x0c140, 0x0fe2f, 0x13a40,
	0x17fc4, 0x1be36, 0x201cc, 0x24041, 0x28562, 0x2c3f6,
	0x301cc, 0x33b5b, 0x37d90, 0x3bc29, 0x40000, 0x43cf0,
	0x4b0b9, 0x4c585, 0x50213, 0x53e2f, 0x5c204, 0x5c7df,
	0x5e696, 0x5fff7,
};

#define SC1A4T_GAIN_STEPS  (sizeof(sc1a4t_gain_log2) / sizeof(sc1a4t_gain_log2[0]))
#define SC2336P_GAIN_STEPS 129
#define SC2336_GAIN_STEPS  (sizeof(sc2336_gain_log2) / sizeof(sc2336_gain_log2[0]))
#define GC2053_GAIN_STEPS  (sizeof(gc2053_gain_log2) / sizeof(gc2053_gain_log2[0]))

static const uint32_t imx327_gain_log2[] = {
	0, 3266, 6531, 9797, 13062, 16328, 19594, 22859,
	26125, 29390, 32656, 35921, 39187, 42453, 45718, 48984,
	52249, 55515, 58781, 62046, 65312, 68577, 71843, 75109,
	78374, 81640, 84905, 88171, 91436, 94702, 97968, 101233,
	104499, 107764, 111030, 114296, 117561, 120827, 124092, 127358,
	130624, 133889, 137155, 140420, 143686, 146951, 150217, 153483,
	156748, 160014, 163279, 166545, 169811, 173076, 176342, 179607,
	182873, 186139, 189404, 192670, 195935, 199201, 202466, 205732,
	208998, 212263, 215529, 218794, 222060, 225326, 228591, 231857,
	235122, 238388, 241654, 244919, 248185, 251450, 254716, 257981,
	261247, 264513, 267778, 271044, 274309, 277575, 280841, 284106,
	287372, 290637, 293903, 297169, 300434, 303700, 306965, 310231,
	313496, 316762, 320028, 323293, 326559,
};

static const uint32_t jxh63p_gain_log2[] = {
	0, 5731, 11136, 16248, 21097, 25710, 30109, 34312,
	38336, 42195, 45904, 49472, 52910, 56228, 59433, 62534,
	65536, 71267, 76672, 81784, 86633, 91246, 95645, 99848,
	103872, 107731, 111440, 115008, 118446, 121764, 124969, 128070,
	131072, 136803, 142208, 147320, 152169, 156782, 161181, 165384,
	169408, 173267, 176976, 180544, 183982, 187300, 190505, 193606,
	196608, 202339, 207744, 212856, 217705, 222318, 226717, 230920,
	234944, 238803, 242512, 246080, 249518, 252836, 256041, 259142,
	262144, 267875, 273280, 278392, 283241, 287854, 292253, 296456,
	300480, 304339, 308048, 311616, 315054, 318372, 321577, 324678,
};

static const uint32_t sc1346_gain_log2[] = {
	0, 2886, 5776, 8494, 11136, 13706, 16287, 18723,
	21097, 23413, 25746, 27952, 30108, 32216, 34344, 36361,
	38335, 40269, 42225, 44082, 45903, 47689, 49499, 51220,
	52910, 54570, 56253, 57856, 59433, 60983, 62557, 64058,
	65535, 68421, 71311, 74029, 76671, 79241, 81822, 84258,
	86632, 88948, 91281, 93487, 95643, 97751, 99879, 101896,
	103870, 105804, 107760, 109617, 111438, 113224, 115034, 116755,
	118445, 120105, 121788, 123391, 124968, 126518, 128092, 129593,
	131070, 133956, 136846, 139564, 142206, 144776, 147357, 149793,
	152167, 154483, 156816, 159022, 161178, 163286, 165414, 167431,
	169405, 171339, 173295, 175152, 176973, 178759, 180569, 182290,
	183980, 185640, 187323, 188926, 190503, 192053, 193627, 195128,
	196605, 199491, 202381, 205099, 207741, 210311, 212892, 215328,
	217702, 220018, 222351, 224557, 226713, 228821, 230949, 232966,
	234940, 236874, 238830, 240687, 242508, 244294, 246104, 247825,
	249515, 251175, 252858, 254461, 256038, 257588, 259162, 260663,
	262140, 265026, 270634, 273276, 275846, 278427, 280863, 283237,
	285553, 287886, 290092, 292248, 294356, 296484, 298501, 300475,
	302409, 304365, 306222, 308043, 309829, 311639, 313360, 315050,
	316710, 318393, 319996, 321573, 323123, 324697, 326198, 327675,
	330561, 333451, 336169, 338811, 341381, 343962, 346398, 348772,
	351088, 353421, 355627, 357783, 359891, 362019, 364036, 366010,
	367944, 369900, 371757, 373578, 375364, 377174, 378895, 380585,
	382245, 383928, 385531, 387108, 388658, 390232, 391733,
};

static const uint32_t sc200ai_gain_log2[] = {
	0, 1500, 2886, 4342, 5776, 7101, 8494, 9781,
	11136, 12471, 13706, 15005, 16287, 17474, 18723, 19879,
	21097, 22300, 23413, 24587, 25746, 26820, 27952, 29002,
	30108, 31202, 32216, 33286, 34344, 35325, 36361, 37321,
	38335, 39338, 40269, 41252, 42225, 43128, 44082, 44967,
	45903, 46829, 47689, 48599, 49499, 50336, 51220, 52041,
	52910, 53770, 54570, 55415, 56253, 57032, 57856, 58622,
	59433, 60236, 60983, 61773, 62557, 63286, 64058, 64775,
	65535, 66989, 68467, 69877, 71266, 72636, 74029, 75359,
	76671, 77964, 79281, 80540, 81782, 83009, 84258, 85452,
	86632, 87797, 88985, 90122, 91245, 92355, 93487, 94571,
	95643, 96703, 97785, 98821, 99846, 100860, 101896, 102888,
	103870, 104842, 105835, 106787, 107730, 108663, 109617, 110532,
	111438, 112335, 113253, 114134, 115006, 115704, 117166, 118606,
	120025, 121449, 122826, 124183, 125521, 126840, 128141, 129424,
	130691, 131963, 133196, 134413, 135615, 136801, 137973, 139131,
	140274, 141425, 142541, 143644, 144735, 145813, 146879, 147932,
	148975, 150025, 151045, 152054, 153052, 154039, 155017, 155984,
	156942, 157908, 158846, 159776, 160696, 161607, 162510, 163404,
	164290, 165184, 166053, 166914, 167768, 168614, 169452, 170283,
	171107, 171939, 172749, 173552, 174348, 175137, 175920, 176696,
	177466, 178244, 179002, 179753, 180499, 181239, 182701, 184155,
	185573, 186971, 188348, 189718, 191056, 192375, 193676, 194971,
	196237, 197487, 198720, 199948, 201150, 202336, 203508, 204676,
	205820, 206949, 208066, 209179, 210270, 211348, 212414, 213477,
	214520, 215550, 216570, 217589, 218549, 219574, 220497, 221501,
	222405, 223389, 224364, 225241, 226196, 227142, 227994, 228922,
	229758, 230669, 231572, 232385, 233271, 234149, 234940, 235803,
	236580, 237428, 238269, 239026, 239853, 240672, 241411, 242216,
	242943, 243736, 244523, 245232, 246006, 246774, 248223, 249649,
	251055, 252506, 253870, 255215, 256540, 257910, 259199, 260470,
	261725, 263022, 264243, 265449, 266640, 267871, 269032, 270179,
	271312, 272484, 273590, 274683, 275764, 276883, 277939, 278983,
	280015, 281085, 282096, 283095, 284084, 285109, 286078, 287036,
	287985, 288969, 289899, 290819, 291731, 292677, 293571, 294457,
	295335, 296245, 297107, 297960, 298806, 299684, 300514, 301338,
	302154, 303002, 303804, 304599, 305388, 306207, 306982, 307751,
	308514, 309307, 310058, 310802, 311541, 312309, 313758, 315218,
	316623, 318041, 319405, 320781, 322107, 323445, 324734, 326035,
	327290, 328557, 329778, 331013, 332203, 333406, 334567, 335741,
	336874, 338019, 339125, 340244, 341324, 342418, 343474, 344542,
	345575, 346620, 347631, 348654, 349643, 350644, 351613, 352594,
	353542, 354504, 356376, 357299, 358212, 359117, 360013, 360901,
	361780, 362652, 363515, 364371, 365219, 366059, 366892, 367718,
	368537, 369348, 370153, 370951, 371742, 372527, 373305, 374077,
	374842, 375379, 375601, 376355, 377102,
};

static const uint32_t sc2310_gain_log2[] = {
	0, 2886, 5776, 8494, 11136, 13706, 16287, 18723,
	21097, 23414, 25746, 27953, 30109, 32217, 34345, 36361,
	38336, 40270, 42226, 44082, 45904, 47690, 49500, 51220,
	52910, 54571, 56254, 57857, 59433, 60984, 62558, 64059,
	65536, 68468, 71267, 74030, 76672, 79283, 81784, 84259,
	86633, 88986, 91246, 93489, 96091, 98956, 101736, 104437,
	107063, 109618, 112106, 114530, 116894, 119200, 121451, 123649,
	125798, 127899, 129954, 131965, 133935, 135864, 137755, 139609,
	141427, 143211, 144962, 146681, 148369, 150027, 151657, 153260,
	154836, 156385, 157910, 159411, 161610, 164475, 167256, 169958,
	172584, 175140, 177628, 180052, 182416, 184722, 186974, 189172,
	191321, 193423, 195478, 197490, 199460, 201389, 203280, 205134,
	206953, 208736, 210487, 212207, 213895, 215554, 217184, 218786,
	220362, 221912, 223437, 224938, 227146, 230011, 232792, 235494,
	238120, 240676, 243164, 245588, 247952, 250258, 252510, 254708,
	256857, 258959, 261014, 263026, 264996, 266925, 268816, 270670,
	272489, 274273, 276023, 277743, 279431, 281090, 282720, 284322,
	285898, 287448, 288973, 290474, 292682, 295547, 298328, 301030,
	303656, 306212, 308700, 311124, 313488, 315794, 318046, 320244,
	322393, 324495, 326550, 328562, 330532, 332461, 334352, 336206,
	338025, 339809, 341559, 343279, 344967, 346626, 348256, 349858,
	351434, 352984, 354509, 356010,
};

static const uint32_t sc2331_gain_log2[] = {
	0, 2886, 5776, 8494, 11136, 13706, 16287, 18723,
	21097, 23414, 25746, 27953, 30109, 32217, 34345, 36361,
	38336, 40270, 42226, 44082, 45904, 47690, 49500, 51220,
	52910, 54571, 56254, 57857, 59433, 60984, 62558, 64059,
	65536, 68422, 71312, 74030, 76672, 79242, 81823, 84259,
	86633, 88950, 91282, 93489, 95645, 97753, 99881, 101897,
	103872, 105806, 107762, 109618, 111440, 113226, 115036, 116756,
	118446, 120107, 121790, 123393, 124969, 126520, 128094, 129595,
	131072, 133958, 136848, 139566, 142208, 144778, 147359, 149795,
	152169, 154486, 156818, 159025, 161181, 163289, 165417, 167433,
	169408, 171342, 173298, 175154, 176976, 178762, 180572, 182292,
	183982, 185643, 187326, 188929, 190505, 192056, 193630, 195131,
	196608, 199494, 202384, 205102, 207744, 210314, 212895, 215331,
	217705, 220022, 222354, 224561, 226717, 228825, 230953, 232969,
	234944, 236878, 238834, 240690, 242512, 244298, 246108, 247828,
	249518, 251179, 252862, 254465, 256041, 257592, 259166, 260667,
	262144, 265030, 267920, 270638, 273280, 275850, 278431, 280867,
	283241, 285558, 287890, 290097, 292253, 294361, 296489, 298505,
	300480, 302414, 304370, 306226, 308048, 309834, 311644, 313364,
	315054, 316715, 318398, 320001, 321577, 323128, 324702, 326203,
};

static const uint32_t sp140a_gain_log2[] = {
	0, 5731, 11136, 16248, 21097, 25710, 30109, 34312,
	38336, 42195, 45904, 49472, 52910, 56228, 59433, 62534,
	65536, 71267, 76672, 81784, 86633, 91246, 95645, 99848,
	103872, 107731, 111440, 115008, 118446, 121764, 124969, 128070,
	131072, 136803, 142208, 147320, 152169, 156782, 161181, 165384,
	169408, 173267, 176976, 180544, 183982, 187300, 190505, 193606,
	196608, 202339, 207744, 212856, 217705, 222318, 226717, 231432,
	234944, 238803, 242512, 246080, 249518, 252836, 256041, 259142,
	261774,
};

static const struct {
	const char *name;
	const uint32_t *gain;
	unsigned int count;
} sensor_gains[] = {
	{ "gc2053", gc2053_gain_log2, 26 },
	{ "imx327", imx327_gain_log2, 101 },
	{ "jxf23", jxh63p_gain_log2, 64 },
	{ "jxf37pa", jxh63p_gain_log2, 64 },
	{ "jxf38p", jxh63p_gain_log2, 64 },
	{ "jxh63p", jxh63p_gain_log2, 80 },
	{ "sc1346", sc1346_gain_log2, 191 },
	{ "sc1a4t", sc1a4t_gain_log2, 191 },
	{ "sc1a4ts1", sc1a4t_gain_log2, 191 },
	{ "sc200ai", sc200ai_gain_log2, 365 },
	{ "sc2310", sc2310_gain_log2, 172 },
	{ "sc2331", sc2331_gain_log2, 160 },
	{ "sc2336", sc2336_gain_log2, 161 },
	{ "sc2336p", sc2336_gain_log2, 161 },
	{ "sc2337p", sc2336_gain_log2, 161 },
	{ "sp140a", sp140a_gain_log2, 65 },
};

static const uint32_t *legacy_gain_table(int maximum)
{
	if (maximum == (int)GC2053_GAIN_STEPS - 1)
		return gc2053_gain_log2;
	if (maximum == (int)SC2336P_GAIN_STEPS - 1 ||
	    maximum == (int)SC2336_GAIN_STEPS - 1)
		return sc2336_gain_log2;
	return maximum == (int)SC1A4T_GAIN_STEPS - 1 ? sc1a4t_gain_log2 : NULL;
}

int ipa_sensor_gain_init(struct ipa_sensor *s, const char *name, int maximum)
{
	s->gain_log2 = NULL;
	if (name && *name) {
		for (unsigned int i = 0; i < sizeof(sensor_gains) / sizeof(sensor_gains[0]); i++) {
			if (!strcmp(name, sensor_gains[i].name) &&
			    (unsigned int)maximum < sensor_gains[i].count) {
				s->gain_log2 = sensor_gains[i].gain;
				break;
			}
		}
	} else {
		s->gain_log2 = legacy_gain_table(maximum);
	}
	if (!s->gain_log2)
		return -ENODEV;
	s->gain_index_max = maximum;
	return 0;
}

static int get_ctrl(int fd, uint32_t id, int *value)
{
	struct v4l2_control c = { .id = id };

	if (ioctl(fd, VIDIOC_G_CTRL, &c) < 0)
		return -errno;
	*value = c.value;
	return 0;
}

static int set_ctrl(int fd, uint32_t id, int value)
{
	struct v4l2_control c = { .id = id, .value = value };

	return ioctl(fd, VIDIOC_S_CTRL, &c) < 0 ? -errno : 0;
}

/* Line period from the pixel rate and the line length, 0 when unknown */
static double line_time_us(int fd, int width)
{
	struct v4l2_ext_control rate = { .id = V4L2_CID_PIXEL_RATE };
	struct v4l2_ext_controls ctrls = { .count = 1, .controls = &rate };
	int hblank = 0;

	if (ioctl(fd, VIDIOC_G_EXT_CTRLS, &ctrls) < 0 || rate.value64 <= 0 ||
	    get_ctrl(fd, V4L2_CID_HBLANK, &hblank))
		return 0;
	return (width + hblank) * 1e6 / rate.value64;
}

static int query_sensor_dimensions(int fd, int *width, int *height)
{
	struct v4l2_subdev_format sdfmt = {
		.which = V4L2_SUBDEV_FORMAT_ACTIVE,
		.pad = 0,
	};
	struct v4l2_format fmt = { .type = V4L2_BUF_TYPE_VIDEO_CAPTURE };

	if (ioctl(fd, VIDIOC_SUBDEV_G_FMT, &sdfmt) == 0 &&
	    sdfmt.format.width > 0 && sdfmt.format.height > 0) {
		*width = sdfmt.format.width;
		*height = sdfmt.format.height;
		return 0;
	}
	if (ioctl(fd, VIDIOC_G_FMT, &fmt) == 0 &&
	    fmt.fmt.pix.width > 0 && fmt.fmt.pix.height > 0) {
		*width = fmt.fmt.pix.width;
		*height = fmt.fmt.pix.height;
		return 0;
	}
	return -errno ? -errno : -EINVAL;
}

int ipa_sensor_init(struct ipa_sensor *s, int fd, const char *name)
{
	struct v4l2_queryctrl exposure = { .id = V4L2_CID_EXPOSURE };
	struct v4l2_queryctrl gain = { .id = V4L2_CID_ANALOGUE_GAIN };
	struct v4l2_queryctrl vblank = { .id = V4L2_CID_VBLANK };
	int width = 0, height = 0;
	int ret;

	memset(s, 0, sizeof(*s));
	s->fd = fd;
	if (ioctl(fd, VIDIOC_QUERYCTRL, &exposure) < 0 ||
	    ioctl(fd, VIDIOC_QUERYCTRL, &gain) < 0 ||
	    ioctl(fd, VIDIOC_QUERYCTRL, &vblank) < 0)
		return -errno;
	ret = query_sensor_dimensions(fd, &width, &height);
	if (ret)
		return ret;
	if (gain.minimum || gain.maximum < 0)
		return -ENODEV;
	ret = ipa_sensor_gain_init(s, name, gain.maximum);
	if (ret)
		return ret;
	s->exposure_min = exposure.minimum;
	s->exposure_max = exposure.maximum;
	s->height = height;
	ret = get_ctrl(fd, V4L2_CID_EXPOSURE, &s->exposure);
	if (!ret)
		ret = get_ctrl(fd, V4L2_CID_ANALOGUE_GAIN, &s->gain_index);
	if (!ret)
		ret = get_ctrl(fd, V4L2_CID_VBLANK, &s->vblank);
	if (ret)
		return ret;
	/*
	 * The sensor's default frame is the shortest AE uses unless told
	 * otherwise; an earlier run may have left a longer one.
	 */
	s->vblank_min = s->vblank_max = vblank.default_value;
	s->exposure_margin = s->height + s->vblank - s->exposure_max;
	s->line_us = line_time_us(fd, width);
	return 0;
}

/* Allow AE to lengthen frames down to min_fps. */
void ipa_sensor_frame_limits(struct ipa_sensor *s, double min_fps)
{
	struct v4l2_queryctrl q = { .id = V4L2_CID_VBLANK };
	int vblank;

	if (min_fps <= 0 || !s->line_us || ioctl(s->fd, VIDIOC_QUERYCTRL, &q) < 0)
		return;
	vblank = (int)(1e6 / (min_fps * s->line_us)) - s->height;
	if (vblank > q.maximum)
		vblank = q.maximum;
	s->vblank_max = vblank > s->vblank_min ? vblank : s->vblank_min;
}

int ipa_sensor_exposure_max(const struct ipa_sensor *s, int vblank)
{
	return s->height + vblank - s->exposure_margin;
}

uint32_t ipa_sensor_gain_log2(const struct ipa_sensor *s, int index)
{
	const uint32_t *lut = s->gain_log2;

	if (index < 0)
		index = 0;
	if (index > s->gain_index_max)
		index = s->gain_index_max;
	return lut[index];
}

/* Largest gain step not above the request */
int ipa_sensor_gain_index(const struct ipa_sensor *s, uint32_t gain_log2_q16)
{
	int i;

	for (i = s->gain_index_max; i > 0; i--)
		if (ipa_sensor_gain_log2(s, i) <= gain_log2_q16)
			break;
	return i;
}

static int set_vblank(struct ipa_sensor *s, int vblank)
{
	int ret;

	if (vblank == s->vblank)
		return 0;
	ret = set_ctrl(s->fd, V4L2_CID_VBLANK, vblank);
	if (!ret) {
		s->vblank = vblank;
		s->exposure_max = ipa_sensor_exposure_max(s, vblank);
	}
	return ret;
}

int ipa_sensor_apply(struct ipa_sensor *s, int exposure, int gain_index,
		     int vblank)
{
	int ret = 0;

	if (vblank < s->vblank_min)
		vblank = s->vblank_min;
	if (vblank > s->vblank_max)
		vblank = s->vblank_max;
	/* Lengthen the frame before the exposure needs it, shorten it after. */
	if (vblank > s->vblank)
		ret = set_vblank(s, vblank);
	if (exposure < s->exposure_min)
		exposure = s->exposure_min;
	if (exposure > s->exposure_max)
		exposure = s->exposure_max;
	if (!ret && exposure != s->exposure) {
		ret = set_ctrl(s->fd, V4L2_CID_EXPOSURE, exposure);
		if (!ret)
			s->exposure = exposure;
	}
	if (!ret && gain_index != s->gain_index) {
		ret = set_ctrl(s->fd, V4L2_CID_ANALOGUE_GAIN, gain_index);
		if (!ret)
			s->gain_index = gain_index;
	}
	if (!ret && vblank < s->vblank &&
	    s->exposure <= ipa_sensor_exposure_max(s, vblank))
		ret = set_vblank(s, vblank);
	return ret;
}

/*
 * A frame-sync slave must run the master's frame length. Returns 1 when
 * it changed, so AE can plan the exposure again.
 */
int ipa_sensor_follow(struct ipa_sensor *s, int master_fd)
{
	int vblank = 0, longest;

	if (get_ctrl(master_fd, V4L2_CID_VBLANK, &vblank) || vblank == s->vblank)
		return 0;
	s->vblank_min = s->vblank_max = vblank;
	longest = ipa_sensor_exposure_max(s, vblank);
	if (s->exposure > longest &&
	    !set_ctrl(s->fd, V4L2_CID_EXPOSURE, longest))
		s->exposure = longest;
	return set_vblank(s, vblank) ? 0 : 1;
}
