#pragma once

#include "AlifCore_EvalState.h"
#include "AlifCore_Object.h"
#include "AlifCore_Eval.h" // remove if include other hreader with AlifCore_Eval.h inside it
#include "AlifCore_Long.h" // remove if include other hreader with AlifCore_Long.h inside it
//#include "AlifCore_Import.h"
#include "AlifCore_Parser.h"
#include "AlifCore_Thread.h"


#define ALIF_DURERUNSTATE_INIT(dureRun)												\
	{																				\
        .selfInitialized = 0,														\
		.interpreters = {.nextID = -1},												\
		.mainThreadID = 0,															\
		.threads = ALIFTHREAD_DURERUN_INIT(dureRun.threads),						\
		.autoTSSKey = 0,															\
		.trashTSSKey = 0,															\
		.parser = PARSER_DURERUN_STATE_INIT,										\
        .staticObjects = { \
            .singletons = { \
                .smallInts = ALIF_SMALL_INTS_INIT, \
                /*.bytesEmpty = ALIFBYTES_SIMPLE_INIT(0, 0),*/ \
                .bytesCharacters = ALIF_BYTES_CHARACTERS_INIT, \
                .strings = { \
                    .literals = ALIF_STR_LITERALS_INIT, \
                    .identifiers = ALIF_STR_IDENTIFIERS_INIT, \
                    /*.ascii = ALIF_STR_ASCII_INIT,*/ \
                    /*.latin1 = ALIF_STR_LATIN1_INIT,*/ \
                }, \
                .tupleEmpty = { \
                    .objBase = ALIFVAROBJECT_HEAD_INIT(&_alifTupleType_, 0), \
                }, \
                /*.hamtBitmapNodeEmpty = { \
                    .objBase = ALIFVAROBJECT_HEAD_INIT(&_alifHamtBitmapNodeType_, 0), \
                },*/ \
                /*.contextTokenMissing = { \
                    .objBase = ALIFOBJECT_HEAD_INIT(&_alifContextTokenMissingType_), \
                },*/ \
            }, \
        }, \
        .mainInterpreter = ALIF_INTERPRETERSTATE_INIT(_dureRun_.mainInterpreter),	\
	}


#define ALIF_INTERPRETERSTATE_INIT(interpreter)						\
    {																\
		.eval = {.recursionLimit = ALIF_DEFAULT_RECURSION_LIMIT},	\
		.qsbr = {													\
			.wrSeq = QSBR_INITIAL,									\
			.rdSeq = QSBR_INITIAL,									\
		},															\
		.types = {													\
			.nextVersionTag = _ALIF_TYPE_BASE_VERSION_TAG,			\
		},															\
    }
        //IMPORTS_INIT,



#define ALIFBYTES_SIMPLE_INIT(_ch, _len) \
    { \
        .objBase = ALIFVAROBJECT_HEAD_INIT(&_alifBytesType_, (_len)), \
        .hash = -1, \
        .val = { static_cast<char>(_ch) }, \
    }
#define ALIFBYTES_CHAR_INIT(_ch) \
    { \
        ALIFBYTES_SIMPLE_INIT((_ch), 1) \
    }

#define ALIFUSTR_ASCII_BASE_INIT(_litr, ASCII) \
    { \
        .objBase = ALIFOBJECT_HEAD_INIT(&_alifUStrType_), \
        .length = sizeof(_litr) - 1, \
        .hash = -1, \
        .state = { \
            .kind = 1, \
            .compact = 1, \
            .ascii = (ASCII), \
            .staticallyAllocated = 1, \
        }, \
    }
#define ALIFASCIIOBJECT_INIT(_litr)									\
	{																\
		.ascii = ALIFUSTR_ASCII_BASE_INIT(_litr, 1),				\
		.data = _litr												\
	}
#define INIT_STR(_name, _litr) \
    .alif ## _name = ALIFASCIIOBJECT_INIT(_litr)
#define INIT_ID(_name) \
    .alif ## _name = ALIFASCIIOBJECT_INIT(#_name)
#define ALIFUSTR_LATIN1_INIT(_litr, UTF8) \
    { \
        .latin1 = { \
            .base = ALIFUSTR_ASCII_BASE_INIT((_litr), 0), \
            .utf8 = (UTF8), \
            .utf8Length = sizeof(UTF8) - 1, \
        }, \
        .data = (_litr), \
    }




















/* ------------------------------ AlifCore_DureRunInitGenerated.h ------------------------------ */


#define ALIF_SMALL_INTS_INIT { \
    ALIFLONG_DIGIT_INIT(-5), \
    ALIFLONG_DIGIT_INIT(-4), \
    ALIFLONG_DIGIT_INIT(-3), \
    ALIFLONG_DIGIT_INIT(-2), \
    ALIFLONG_DIGIT_INIT(-1), \
    ALIFLONG_DIGIT_INIT(0), \
    ALIFLONG_DIGIT_INIT(1), \
    ALIFLONG_DIGIT_INIT(2), \
    ALIFLONG_DIGIT_INIT(3), \
    ALIFLONG_DIGIT_INIT(4), \
    ALIFLONG_DIGIT_INIT(5), \
    ALIFLONG_DIGIT_INIT(6), \
    ALIFLONG_DIGIT_INIT(7), \
    ALIFLONG_DIGIT_INIT(8), \
    ALIFLONG_DIGIT_INIT(9), \
    ALIFLONG_DIGIT_INIT(10), \
    ALIFLONG_DIGIT_INIT(11), \
    ALIFLONG_DIGIT_INIT(12), \
    ALIFLONG_DIGIT_INIT(13), \
    ALIFLONG_DIGIT_INIT(14), \
    ALIFLONG_DIGIT_INIT(15), \
    ALIFLONG_DIGIT_INIT(16), \
    ALIFLONG_DIGIT_INIT(17), \
    ALIFLONG_DIGIT_INIT(18), \
    ALIFLONG_DIGIT_INIT(19), \
    ALIFLONG_DIGIT_INIT(20), \
    ALIFLONG_DIGIT_INIT(21), \
    ALIFLONG_DIGIT_INIT(22), \
    ALIFLONG_DIGIT_INIT(23), \
    ALIFLONG_DIGIT_INIT(24), \
    ALIFLONG_DIGIT_INIT(25), \
    ALIFLONG_DIGIT_INIT(26), \
    ALIFLONG_DIGIT_INIT(27), \
    ALIFLONG_DIGIT_INIT(28), \
    ALIFLONG_DIGIT_INIT(29), \
    ALIFLONG_DIGIT_INIT(30), \
    ALIFLONG_DIGIT_INIT(31), \
    ALIFLONG_DIGIT_INIT(32), \
    ALIFLONG_DIGIT_INIT(33), \
    ALIFLONG_DIGIT_INIT(34), \
    ALIFLONG_DIGIT_INIT(35), \
    ALIFLONG_DIGIT_INIT(36), \
    ALIFLONG_DIGIT_INIT(37), \
    ALIFLONG_DIGIT_INIT(38), \
    ALIFLONG_DIGIT_INIT(39), \
    ALIFLONG_DIGIT_INIT(40), \
    ALIFLONG_DIGIT_INIT(41), \
    ALIFLONG_DIGIT_INIT(42), \
    ALIFLONG_DIGIT_INIT(43), \
    ALIFLONG_DIGIT_INIT(44), \
    ALIFLONG_DIGIT_INIT(45), \
    ALIFLONG_DIGIT_INIT(46), \
    ALIFLONG_DIGIT_INIT(47), \
    ALIFLONG_DIGIT_INIT(48), \
    ALIFLONG_DIGIT_INIT(49), \
    ALIFLONG_DIGIT_INIT(50), \
    ALIFLONG_DIGIT_INIT(51), \
    ALIFLONG_DIGIT_INIT(52), \
    ALIFLONG_DIGIT_INIT(53), \
    ALIFLONG_DIGIT_INIT(54), \
    ALIFLONG_DIGIT_INIT(55), \
    ALIFLONG_DIGIT_INIT(56), \
    ALIFLONG_DIGIT_INIT(57), \
    ALIFLONG_DIGIT_INIT(58), \
    ALIFLONG_DIGIT_INIT(59), \
    ALIFLONG_DIGIT_INIT(60), \
    ALIFLONG_DIGIT_INIT(61), \
    ALIFLONG_DIGIT_INIT(62), \
    ALIFLONG_DIGIT_INIT(63), \
    ALIFLONG_DIGIT_INIT(64), \
    ALIFLONG_DIGIT_INIT(65), \
    ALIFLONG_DIGIT_INIT(66), \
    ALIFLONG_DIGIT_INIT(67), \
    ALIFLONG_DIGIT_INIT(68), \
    ALIFLONG_DIGIT_INIT(69), \
    ALIFLONG_DIGIT_INIT(70), \
    ALIFLONG_DIGIT_INIT(71), \
    ALIFLONG_DIGIT_INIT(72), \
    ALIFLONG_DIGIT_INIT(73), \
    ALIFLONG_DIGIT_INIT(74), \
    ALIFLONG_DIGIT_INIT(75), \
    ALIFLONG_DIGIT_INIT(76), \
    ALIFLONG_DIGIT_INIT(77), \
    ALIFLONG_DIGIT_INIT(78), \
    ALIFLONG_DIGIT_INIT(79), \
    ALIFLONG_DIGIT_INIT(80), \
    ALIFLONG_DIGIT_INIT(81), \
    ALIFLONG_DIGIT_INIT(82), \
    ALIFLONG_DIGIT_INIT(83), \
    ALIFLONG_DIGIT_INIT(84), \
    ALIFLONG_DIGIT_INIT(85), \
    ALIFLONG_DIGIT_INIT(86), \
    ALIFLONG_DIGIT_INIT(87), \
    ALIFLONG_DIGIT_INIT(88), \
    ALIFLONG_DIGIT_INIT(89), \
    ALIFLONG_DIGIT_INIT(90), \
    ALIFLONG_DIGIT_INIT(91), \
    ALIFLONG_DIGIT_INIT(92), \
    ALIFLONG_DIGIT_INIT(93), \
    ALIFLONG_DIGIT_INIT(94), \
    ALIFLONG_DIGIT_INIT(95), \
    ALIFLONG_DIGIT_INIT(96), \
    ALIFLONG_DIGIT_INIT(97), \
    ALIFLONG_DIGIT_INIT(98), \
    ALIFLONG_DIGIT_INIT(99), \
    ALIFLONG_DIGIT_INIT(100), \
    ALIFLONG_DIGIT_INIT(101), \
    ALIFLONG_DIGIT_INIT(102), \
    ALIFLONG_DIGIT_INIT(103), \
    ALIFLONG_DIGIT_INIT(104), \
    ALIFLONG_DIGIT_INIT(105), \
    ALIFLONG_DIGIT_INIT(106), \
    ALIFLONG_DIGIT_INIT(107), \
    ALIFLONG_DIGIT_INIT(108), \
    ALIFLONG_DIGIT_INIT(109), \
    ALIFLONG_DIGIT_INIT(110), \
    ALIFLONG_DIGIT_INIT(111), \
    ALIFLONG_DIGIT_INIT(112), \
    ALIFLONG_DIGIT_INIT(113), \
    ALIFLONG_DIGIT_INIT(114), \
    ALIFLONG_DIGIT_INIT(115), \
    ALIFLONG_DIGIT_INIT(116), \
    ALIFLONG_DIGIT_INIT(117), \
    ALIFLONG_DIGIT_INIT(118), \
    ALIFLONG_DIGIT_INIT(119), \
    ALIFLONG_DIGIT_INIT(120), \
    ALIFLONG_DIGIT_INIT(121), \
    ALIFLONG_DIGIT_INIT(122), \
    ALIFLONG_DIGIT_INIT(123), \
    ALIFLONG_DIGIT_INIT(124), \
    ALIFLONG_DIGIT_INIT(125), \
    ALIFLONG_DIGIT_INIT(126), \
    ALIFLONG_DIGIT_INIT(127), \
    ALIFLONG_DIGIT_INIT(128), \
    ALIFLONG_DIGIT_INIT(129), \
    ALIFLONG_DIGIT_INIT(130), \
    ALIFLONG_DIGIT_INIT(131), \
    ALIFLONG_DIGIT_INIT(132), \
    ALIFLONG_DIGIT_INIT(133), \
    ALIFLONG_DIGIT_INIT(134), \
    ALIFLONG_DIGIT_INIT(135), \
    ALIFLONG_DIGIT_INIT(136), \
    ALIFLONG_DIGIT_INIT(137), \
    ALIFLONG_DIGIT_INIT(138), \
    ALIFLONG_DIGIT_INIT(139), \
    ALIFLONG_DIGIT_INIT(140), \
    ALIFLONG_DIGIT_INIT(141), \
    ALIFLONG_DIGIT_INIT(142), \
    ALIFLONG_DIGIT_INIT(143), \
    ALIFLONG_DIGIT_INIT(144), \
    ALIFLONG_DIGIT_INIT(145), \
    ALIFLONG_DIGIT_INIT(146), \
    ALIFLONG_DIGIT_INIT(147), \
    ALIFLONG_DIGIT_INIT(148), \
    ALIFLONG_DIGIT_INIT(149), \
    ALIFLONG_DIGIT_INIT(150), \
    ALIFLONG_DIGIT_INIT(151), \
    ALIFLONG_DIGIT_INIT(152), \
    ALIFLONG_DIGIT_INIT(153), \
    ALIFLONG_DIGIT_INIT(154), \
    ALIFLONG_DIGIT_INIT(155), \
    ALIFLONG_DIGIT_INIT(156), \
    ALIFLONG_DIGIT_INIT(157), \
    ALIFLONG_DIGIT_INIT(158), \
    ALIFLONG_DIGIT_INIT(159), \
    ALIFLONG_DIGIT_INIT(160), \
    ALIFLONG_DIGIT_INIT(161), \
    ALIFLONG_DIGIT_INIT(162), \
    ALIFLONG_DIGIT_INIT(163), \
    ALIFLONG_DIGIT_INIT(164), \
    ALIFLONG_DIGIT_INIT(165), \
    ALIFLONG_DIGIT_INIT(166), \
    ALIFLONG_DIGIT_INIT(167), \
    ALIFLONG_DIGIT_INIT(168), \
    ALIFLONG_DIGIT_INIT(169), \
    ALIFLONG_DIGIT_INIT(170), \
    ALIFLONG_DIGIT_INIT(171), \
    ALIFLONG_DIGIT_INIT(172), \
    ALIFLONG_DIGIT_INIT(173), \
    ALIFLONG_DIGIT_INIT(174), \
    ALIFLONG_DIGIT_INIT(175), \
    ALIFLONG_DIGIT_INIT(176), \
    ALIFLONG_DIGIT_INIT(177), \
    ALIFLONG_DIGIT_INIT(178), \
    ALIFLONG_DIGIT_INIT(179), \
    ALIFLONG_DIGIT_INIT(180), \
    ALIFLONG_DIGIT_INIT(181), \
    ALIFLONG_DIGIT_INIT(182), \
    ALIFLONG_DIGIT_INIT(183), \
    ALIFLONG_DIGIT_INIT(184), \
    ALIFLONG_DIGIT_INIT(185), \
    ALIFLONG_DIGIT_INIT(186), \
    ALIFLONG_DIGIT_INIT(187), \
    ALIFLONG_DIGIT_INIT(188), \
    ALIFLONG_DIGIT_INIT(189), \
    ALIFLONG_DIGIT_INIT(190), \
    ALIFLONG_DIGIT_INIT(191), \
    ALIFLONG_DIGIT_INIT(192), \
    ALIFLONG_DIGIT_INIT(193), \
    ALIFLONG_DIGIT_INIT(194), \
    ALIFLONG_DIGIT_INIT(195), \
    ALIFLONG_DIGIT_INIT(196), \
    ALIFLONG_DIGIT_INIT(197), \
    ALIFLONG_DIGIT_INIT(198), \
    ALIFLONG_DIGIT_INIT(199), \
    ALIFLONG_DIGIT_INIT(200), \
    ALIFLONG_DIGIT_INIT(201), \
    ALIFLONG_DIGIT_INIT(202), \
    ALIFLONG_DIGIT_INIT(203), \
    ALIFLONG_DIGIT_INIT(204), \
    ALIFLONG_DIGIT_INIT(205), \
    ALIFLONG_DIGIT_INIT(206), \
    ALIFLONG_DIGIT_INIT(207), \
    ALIFLONG_DIGIT_INIT(208), \
    ALIFLONG_DIGIT_INIT(209), \
    ALIFLONG_DIGIT_INIT(210), \
    ALIFLONG_DIGIT_INIT(211), \
    ALIFLONG_DIGIT_INIT(212), \
    ALIFLONG_DIGIT_INIT(213), \
    ALIFLONG_DIGIT_INIT(214), \
    ALIFLONG_DIGIT_INIT(215), \
    ALIFLONG_DIGIT_INIT(216), \
    ALIFLONG_DIGIT_INIT(217), \
    ALIFLONG_DIGIT_INIT(218), \
    ALIFLONG_DIGIT_INIT(219), \
    ALIFLONG_DIGIT_INIT(220), \
    ALIFLONG_DIGIT_INIT(221), \
    ALIFLONG_DIGIT_INIT(222), \
    ALIFLONG_DIGIT_INIT(223), \
    ALIFLONG_DIGIT_INIT(224), \
    ALIFLONG_DIGIT_INIT(225), \
    ALIFLONG_DIGIT_INIT(226), \
    ALIFLONG_DIGIT_INIT(227), \
    ALIFLONG_DIGIT_INIT(228), \
    ALIFLONG_DIGIT_INIT(229), \
    ALIFLONG_DIGIT_INIT(230), \
    ALIFLONG_DIGIT_INIT(231), \
    ALIFLONG_DIGIT_INIT(232), \
    ALIFLONG_DIGIT_INIT(233), \
    ALIFLONG_DIGIT_INIT(234), \
    ALIFLONG_DIGIT_INIT(235), \
    ALIFLONG_DIGIT_INIT(236), \
    ALIFLONG_DIGIT_INIT(237), \
    ALIFLONG_DIGIT_INIT(238), \
    ALIFLONG_DIGIT_INIT(239), \
    ALIFLONG_DIGIT_INIT(240), \
    ALIFLONG_DIGIT_INIT(241), \
    ALIFLONG_DIGIT_INIT(242), \
    ALIFLONG_DIGIT_INIT(243), \
    ALIFLONG_DIGIT_INIT(244), \
    ALIFLONG_DIGIT_INIT(245), \
    ALIFLONG_DIGIT_INIT(246), \
    ALIFLONG_DIGIT_INIT(247), \
    ALIFLONG_DIGIT_INIT(248), \
    ALIFLONG_DIGIT_INIT(249), \
    ALIFLONG_DIGIT_INIT(250), \
    ALIFLONG_DIGIT_INIT(251), \
    ALIFLONG_DIGIT_INIT(252), \
    ALIFLONG_DIGIT_INIT(253), \
    ALIFLONG_DIGIT_INIT(254), \
    ALIFLONG_DIGIT_INIT(255), \
    ALIFLONG_DIGIT_INIT(256), \
}



#define ALIF_BYTES_CHARACTERS_INIT { \
    ALIFBYTES_CHAR_INIT(0), \
    ALIFBYTES_CHAR_INIT(1), \
    ALIFBYTES_CHAR_INIT(2), \
    ALIFBYTES_CHAR_INIT(3), \
    ALIFBYTES_CHAR_INIT(4), \
    ALIFBYTES_CHAR_INIT(5), \
    ALIFBYTES_CHAR_INIT(6), \
    ALIFBYTES_CHAR_INIT(7), \
    ALIFBYTES_CHAR_INIT(8), \
    ALIFBYTES_CHAR_INIT(9), \
    ALIFBYTES_CHAR_INIT(10), \
    ALIFBYTES_CHAR_INIT(11), \
    ALIFBYTES_CHAR_INIT(12), \
    ALIFBYTES_CHAR_INIT(13), \
    ALIFBYTES_CHAR_INIT(14), \
    ALIFBYTES_CHAR_INIT(15), \
    ALIFBYTES_CHAR_INIT(16), \
    ALIFBYTES_CHAR_INIT(17), \
    ALIFBYTES_CHAR_INIT(18), \
    ALIFBYTES_CHAR_INIT(19), \
    ALIFBYTES_CHAR_INIT(20), \
    ALIFBYTES_CHAR_INIT(21), \
    ALIFBYTES_CHAR_INIT(22), \
    ALIFBYTES_CHAR_INIT(23), \
    ALIFBYTES_CHAR_INIT(24), \
    ALIFBYTES_CHAR_INIT(25), \
    ALIFBYTES_CHAR_INIT(26), \
    ALIFBYTES_CHAR_INIT(27), \
    ALIFBYTES_CHAR_INIT(28), \
    ALIFBYTES_CHAR_INIT(29), \
    ALIFBYTES_CHAR_INIT(30), \
    ALIFBYTES_CHAR_INIT(31), \
    ALIFBYTES_CHAR_INIT(32), \
    ALIFBYTES_CHAR_INIT(33), \
    ALIFBYTES_CHAR_INIT(34), \
    ALIFBYTES_CHAR_INIT(35), \
    ALIFBYTES_CHAR_INIT(36), \
    ALIFBYTES_CHAR_INIT(37), \
    ALIFBYTES_CHAR_INIT(38), \
    ALIFBYTES_CHAR_INIT(39), \
    ALIFBYTES_CHAR_INIT(40), \
    ALIFBYTES_CHAR_INIT(41), \
    ALIFBYTES_CHAR_INIT(42), \
    ALIFBYTES_CHAR_INIT(43), \
    ALIFBYTES_CHAR_INIT(44), \
    ALIFBYTES_CHAR_INIT(45), \
    ALIFBYTES_CHAR_INIT(46), \
    ALIFBYTES_CHAR_INIT(47), \
    ALIFBYTES_CHAR_INIT(48), \
    ALIFBYTES_CHAR_INIT(49), \
    ALIFBYTES_CHAR_INIT(50), \
    ALIFBYTES_CHAR_INIT(51), \
    ALIFBYTES_CHAR_INIT(52), \
    ALIFBYTES_CHAR_INIT(53), \
    ALIFBYTES_CHAR_INIT(54), \
    ALIFBYTES_CHAR_INIT(55), \
    ALIFBYTES_CHAR_INIT(56), \
    ALIFBYTES_CHAR_INIT(57), \
    ALIFBYTES_CHAR_INIT(58), \
    ALIFBYTES_CHAR_INIT(59), \
    ALIFBYTES_CHAR_INIT(60), \
    ALIFBYTES_CHAR_INIT(61), \
    ALIFBYTES_CHAR_INIT(62), \
    ALIFBYTES_CHAR_INIT(63), \
    ALIFBYTES_CHAR_INIT(64), \
    ALIFBYTES_CHAR_INIT(65), \
    ALIFBYTES_CHAR_INIT(66), \
    ALIFBYTES_CHAR_INIT(67), \
    ALIFBYTES_CHAR_INIT(68), \
    ALIFBYTES_CHAR_INIT(69), \
    ALIFBYTES_CHAR_INIT(70), \
    ALIFBYTES_CHAR_INIT(71), \
    ALIFBYTES_CHAR_INIT(72), \
    ALIFBYTES_CHAR_INIT(73), \
    ALIFBYTES_CHAR_INIT(74), \
    ALIFBYTES_CHAR_INIT(75), \
    ALIFBYTES_CHAR_INIT(76), \
    ALIFBYTES_CHAR_INIT(77), \
    ALIFBYTES_CHAR_INIT(78), \
    ALIFBYTES_CHAR_INIT(79), \
    ALIFBYTES_CHAR_INIT(80), \
    ALIFBYTES_CHAR_INIT(81), \
    ALIFBYTES_CHAR_INIT(82), \
    ALIFBYTES_CHAR_INIT(83), \
    ALIFBYTES_CHAR_INIT(84), \
    ALIFBYTES_CHAR_INIT(85), \
    ALIFBYTES_CHAR_INIT(86), \
    ALIFBYTES_CHAR_INIT(87), \
    ALIFBYTES_CHAR_INIT(88), \
    ALIFBYTES_CHAR_INIT(89), \
    ALIFBYTES_CHAR_INIT(90), \
    ALIFBYTES_CHAR_INIT(91), \
    ALIFBYTES_CHAR_INIT(92), \
    ALIFBYTES_CHAR_INIT(93), \
    ALIFBYTES_CHAR_INIT(94), \
    ALIFBYTES_CHAR_INIT(95), \
    ALIFBYTES_CHAR_INIT(96), \
    ALIFBYTES_CHAR_INIT(97), \
    ALIFBYTES_CHAR_INIT(98), \
    ALIFBYTES_CHAR_INIT(99), \
    ALIFBYTES_CHAR_INIT(100), \
    ALIFBYTES_CHAR_INIT(101), \
    ALIFBYTES_CHAR_INIT(102), \
    ALIFBYTES_CHAR_INIT(103), \
    ALIFBYTES_CHAR_INIT(104), \
    ALIFBYTES_CHAR_INIT(105), \
    ALIFBYTES_CHAR_INIT(106), \
    ALIFBYTES_CHAR_INIT(107), \
    ALIFBYTES_CHAR_INIT(108), \
    ALIFBYTES_CHAR_INIT(109), \
    ALIFBYTES_CHAR_INIT(110), \
    ALIFBYTES_CHAR_INIT(111), \
    ALIFBYTES_CHAR_INIT(112), \
    ALIFBYTES_CHAR_INIT(113), \
    ALIFBYTES_CHAR_INIT(114), \
    ALIFBYTES_CHAR_INIT(115), \
    ALIFBYTES_CHAR_INIT(116), \
    ALIFBYTES_CHAR_INIT(117), \
    ALIFBYTES_CHAR_INIT(118), \
    ALIFBYTES_CHAR_INIT(119), \
    ALIFBYTES_CHAR_INIT(120), \
    ALIFBYTES_CHAR_INIT(121), \
    ALIFBYTES_CHAR_INIT(122), \
    ALIFBYTES_CHAR_INIT(123), \
    ALIFBYTES_CHAR_INIT(124), \
    ALIFBYTES_CHAR_INIT(125), \
    ALIFBYTES_CHAR_INIT(126), \
    ALIFBYTES_CHAR_INIT(127), \
    ALIFBYTES_CHAR_INIT(128), \
    ALIFBYTES_CHAR_INIT(129), \
    ALIFBYTES_CHAR_INIT(130), \
    ALIFBYTES_CHAR_INIT(131), \
    ALIFBYTES_CHAR_INIT(132), \
    ALIFBYTES_CHAR_INIT(133), \
    ALIFBYTES_CHAR_INIT(134), \
    ALIFBYTES_CHAR_INIT(135), \
    ALIFBYTES_CHAR_INIT(136), \
    ALIFBYTES_CHAR_INIT(137), \
    ALIFBYTES_CHAR_INIT(138), \
    ALIFBYTES_CHAR_INIT(139), \
    ALIFBYTES_CHAR_INIT(140), \
    ALIFBYTES_CHAR_INIT(141), \
    ALIFBYTES_CHAR_INIT(142), \
    ALIFBYTES_CHAR_INIT(143), \
    ALIFBYTES_CHAR_INIT(144), \
    ALIFBYTES_CHAR_INIT(145), \
    ALIFBYTES_CHAR_INIT(146), \
    ALIFBYTES_CHAR_INIT(147), \
    ALIFBYTES_CHAR_INIT(148), \
    ALIFBYTES_CHAR_INIT(149), \
    ALIFBYTES_CHAR_INIT(150), \
    ALIFBYTES_CHAR_INIT(151), \
    ALIFBYTES_CHAR_INIT(152), \
    ALIFBYTES_CHAR_INIT(153), \
    ALIFBYTES_CHAR_INIT(154), \
    ALIFBYTES_CHAR_INIT(155), \
    ALIFBYTES_CHAR_INIT(156), \
    ALIFBYTES_CHAR_INIT(157), \
    ALIFBYTES_CHAR_INIT(158), \
    ALIFBYTES_CHAR_INIT(159), \
    ALIFBYTES_CHAR_INIT(160), \
    ALIFBYTES_CHAR_INIT(161), \
    ALIFBYTES_CHAR_INIT(162), \
    ALIFBYTES_CHAR_INIT(163), \
    ALIFBYTES_CHAR_INIT(164), \
    ALIFBYTES_CHAR_INIT(165), \
    ALIFBYTES_CHAR_INIT(166), \
    ALIFBYTES_CHAR_INIT(167), \
    ALIFBYTES_CHAR_INIT(168), \
    ALIFBYTES_CHAR_INIT(169), \
    ALIFBYTES_CHAR_INIT(170), \
    ALIFBYTES_CHAR_INIT(171), \
    ALIFBYTES_CHAR_INIT(172), \
    ALIFBYTES_CHAR_INIT(173), \
    ALIFBYTES_CHAR_INIT(174), \
    ALIFBYTES_CHAR_INIT(175), \
    ALIFBYTES_CHAR_INIT(176), \
    ALIFBYTES_CHAR_INIT(177), \
    ALIFBYTES_CHAR_INIT(178), \
    ALIFBYTES_CHAR_INIT(179), \
    ALIFBYTES_CHAR_INIT(180), \
    ALIFBYTES_CHAR_INIT(181), \
    ALIFBYTES_CHAR_INIT(182), \
    ALIFBYTES_CHAR_INIT(183), \
    ALIFBYTES_CHAR_INIT(184), \
    ALIFBYTES_CHAR_INIT(185), \
    ALIFBYTES_CHAR_INIT(186), \
    ALIFBYTES_CHAR_INIT(187), \
    ALIFBYTES_CHAR_INIT(188), \
    ALIFBYTES_CHAR_INIT(189), \
    ALIFBYTES_CHAR_INIT(190), \
    ALIFBYTES_CHAR_INIT(191), \
    ALIFBYTES_CHAR_INIT(192), \
    ALIFBYTES_CHAR_INIT(193), \
    ALIFBYTES_CHAR_INIT(194), \
    ALIFBYTES_CHAR_INIT(195), \
    ALIFBYTES_CHAR_INIT(196), \
    ALIFBYTES_CHAR_INIT(197), \
    ALIFBYTES_CHAR_INIT(198), \
    ALIFBYTES_CHAR_INIT(199), \
    ALIFBYTES_CHAR_INIT(200), \
    ALIFBYTES_CHAR_INIT(201), \
    ALIFBYTES_CHAR_INIT(202), \
    ALIFBYTES_CHAR_INIT(203), \
    ALIFBYTES_CHAR_INIT(204), \
    ALIFBYTES_CHAR_INIT(205), \
    ALIFBYTES_CHAR_INIT(206), \
    ALIFBYTES_CHAR_INIT(207), \
    ALIFBYTES_CHAR_INIT(208), \
    ALIFBYTES_CHAR_INIT(209), \
    ALIFBYTES_CHAR_INIT(210), \
    ALIFBYTES_CHAR_INIT(211), \
    ALIFBYTES_CHAR_INIT(212), \
    ALIFBYTES_CHAR_INIT(213), \
    ALIFBYTES_CHAR_INIT(214), \
    ALIFBYTES_CHAR_INIT(215), \
    ALIFBYTES_CHAR_INIT(216), \
    ALIFBYTES_CHAR_INIT(217), \
    ALIFBYTES_CHAR_INIT(218), \
    ALIFBYTES_CHAR_INIT(219), \
    ALIFBYTES_CHAR_INIT(220), \
    ALIFBYTES_CHAR_INIT(221), \
    ALIFBYTES_CHAR_INIT(222), \
    ALIFBYTES_CHAR_INIT(223), \
    ALIFBYTES_CHAR_INIT(224), \
    ALIFBYTES_CHAR_INIT(225), \
    ALIFBYTES_CHAR_INIT(226), \
    ALIFBYTES_CHAR_INIT(227), \
    ALIFBYTES_CHAR_INIT(228), \
    ALIFBYTES_CHAR_INIT(229), \
    ALIFBYTES_CHAR_INIT(230), \
    ALIFBYTES_CHAR_INIT(231), \
    ALIFBYTES_CHAR_INIT(232), \
    ALIFBYTES_CHAR_INIT(233), \
    ALIFBYTES_CHAR_INIT(234), \
    ALIFBYTES_CHAR_INIT(235), \
    ALIFBYTES_CHAR_INIT(236), \
    ALIFBYTES_CHAR_INIT(237), \
    ALIFBYTES_CHAR_INIT(238), \
    ALIFBYTES_CHAR_INIT(239), \
    ALIFBYTES_CHAR_INIT(240), \
    ALIFBYTES_CHAR_INIT(241), \
    ALIFBYTES_CHAR_INIT(242), \
    ALIFBYTES_CHAR_INIT(243), \
    ALIFBYTES_CHAR_INIT(244), \
    ALIFBYTES_CHAR_INIT(245), \
    ALIFBYTES_CHAR_INIT(246), \
    ALIFBYTES_CHAR_INIT(247), \
    ALIFBYTES_CHAR_INIT(248), \
    ALIFBYTES_CHAR_INIT(249), \
    ALIFBYTES_CHAR_INIT(250), \
    ALIFBYTES_CHAR_INIT(251), \
    ALIFBYTES_CHAR_INIT(252), \
    ALIFBYTES_CHAR_INIT(253), \
    ALIFBYTES_CHAR_INIT(254), \
    ALIFBYTES_CHAR_INIT(255), \
}


#define ALIF_STR_LITERALS_INIT { \
    INIT_STR(Empty, ""), \
}

#define ALIF_STR_IDENTIFIERS_INIT { \
    INIT_ID(CANCELLED), \
    INIT_ID(__classGetItem__), \
	INIT_ID(__doc__), \
	INIT_ID(__getAttr__), \
	INIT_ID(__hash__), \
	INIT_ID(__lengthHint__), \
	INIT_ID(__loader__), \
    INIT_ID(__name__), \
    INIT_ID(__new__), \
    INIT_ID(__origClass__), \
	INIT_ID(__package__), \
	INIT_ID(__spec__), \
	INIT_ID(hasLocation), \
	INIT_ID(mro), \
	INIT_ID(origin), \
}
