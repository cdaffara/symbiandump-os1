// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef FONTDEFS_H
#define FONTDEFS_H

/*
 * The Series 60 Sans Font is a propietary font
 * owned by Nokia.  It must be installed on target machines
 * in order to run the test properly, i.e.
 * epoc32/release/winscw/udeb/z/resource/fonts/s60ssb_v500.ttf
 * needs to be present.  This is delivered by doing a test build
 * of graphics/internaltestfonts
 * When we switch over to a proper S60 SDK, this font will be
 * standard.
 */
_LIT(KNokiaSeries60Font, "Series 60 Sans");

_LIT(KFullScreenSampleText,
"Marigold Flowers:\n\
\
Text from http://en.wikipedia.org/wiki/Tagetes\n\
\
Tagetes is a genus of 52 species of annual and perennial herbaceous plants in the daisy \
family (Asteraceae or Compositae). They are native to the area stretching from the \
southwestern United States into Mexico and south throughout South America.  \
They are known almost universally in North America as Marigold (not to be confused \
\
with the genus Calendula, which goes by the same name in some areas), or variously \
as Mexican marigolds (or cempasúchil), African marigolds (usually referring to cultivars \
and hybrids of T. erecta, although this species is not native to Africa), or \
French marigolds (usually referring to hybrids and cultivars of T. patula, many \
of which were developed in France although the species is not native to that country).  \
At least one species is a naturalized weed in Africa, Hawaii, and Australia. See \"marigold\" \
for other species commonly called marigold.\n\
Uses and Breeding\n\
\
The foliage has a musky/pungent scent, though some later varieties have been bred to be scentless. \
It is said to deter some common insect pests (although it is recorded as a food plant for some \
Lepidoptera larvae including Dot Moth), as well as nematodes. Tagetes are hence often used in \
companion planting. T. minuta (Khakibush), originally from South America, has been used as a \
source of essential oil, known as tagette, for the perfume industry as well as a flavourant \
in the food and tobacco industries in South Africa, where the species is also a useful \
pioneer plant in the reclamation of disturbed land. Some of the perennial species are deer, \
rabbit, rodent and javalina resistant. The petals of Tagetes are rich in the orange-yellow \
carotenoid lutein and as such extracts of T. erecta are used as a food colour (INS-Number E161b)"); 

_LIT(KRightToLeftText,
"\x5d0\x5d1\x5d2\x5d3\x5d4\x5d5\x5d6\x5d7\x5d8\x5d9\x5da\x5db\x5dc\x5dd\x5de\x5df\n\
\x5e0\x5e1\x5e2\x5e3\x5e4\x5e5\x5e6\x5e7\x5e8\x5e9\x5ea\n\
\x621\x622\x623\x624\x625\x626\x627\x628\x629\x62a\x62b\x62c\x62d\x62e\x62f\n\
\x640\x641\x642\x643\x644\x645\x646\x647\x648\x649\x64a\x64b\x64c\x64d\x64e\x64f\n\
\x631\x632\x633\x634\x635\x636\x637\x638\x639\x63a\n");

_LIT(KComplexFontsText,
"Class RWindow\n\
\n\
class RWindow : public RDrawableWindow;\n\
\n\
Description\n\
\n\
Handle to a standard window.\n\
\n\
An RWindow is a handle to a server-side window which can be \x3b1\x3b2\x3b3 displayed and drawn to,\
         and whose redraws are performed by the application. \x41a\x440\x43e\x43b\x438\x43a l\x151pices \n\
\n\
Derivation\n\
\n\
\x25cb MWsClientClass - Base class for all classes whose objects are clients of the window server.\n\
\x25a0 RWindowTreeNode - A handle to a node in the server-side window tree.\n\
\x25a0 RWindowBase - Client-side handle to a server-side window.\n\
\x25a0 RDrawableWindow - Handle to a drawable window.\n\
\x25a0 RWindow - Handle to a standard window.\n");

enum TFullScreenTextPresentationStyle
    {
    EInsetMargin = 1,               // space 1 pixel around window that text sits inside
    EGapBetweenTextLines = 2,       // space between text lines
    ESideBearingsAllowance = 5,     // shorten the text line to accomodate "W" characters leaking out
    EMaximumTextLines = 100,        // screens won't be so big as to let us show 100 lines of text
    EBackgroundColor = 0xe7fed8,    // light green background
    EDesiredFontHeight = 13,        // font height for text lines
    };

#endif
