/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Math operations
*
*/


#include <e32std.h>
#include "sensordatacompensatormathoperations.h"
#include "trace.h"

#if defined( SDCOMP_USE_FIXED_POINT ) || defined( SDCOMP_USE_FIXED_POINT64 )

const TInt KTableShift = 31; // Bits used for decimals in sine table
const TInt K90Deg = 90;
const TInt K180Deg = 180;
const TInt K270Deg = 270;
const TInt K360Deg = 360;

// Contains fixed point sine value for every degree from 0 to 90.
const TInt KFixedPointSinTable[] =
    {
    0, 37478757, 74946098, 112390610, 149800886, // 4
    187165532, 224473165, 261712422, 298871958, 335940455, // 9
    372906621, 409759197, 446486956, 483078710, 519523314, // 14
    555809666, 591926714, 627863454, 663608942, 699152287, // 19
    734482664, 769589311, 804461533, 839088708, 873460289, // 24
    907565806, 941394869, 974937174, 1008182503, 1041120731, // 29
    1073741823, 1106035843, 1137992954, 1169603421, 1200857615, // 34
    1231746017, 1262259217, 1292387920, 1322122950, 1351455248, // 39
    1380375880, 1408876036, 1436947034, 1464580325, 1491767490, // 44
    1518500249, 1544770457, 1570570114, 1595891360, 1620726481, // 49
    1645067914, 1668908243, 1692240207, 1715056697, 1737350765, // 54
    1759115619, 1780344629, 1801031329, 1821169418, 1840752761, // 59
    1859775392, 1878231518, 1896115516, 1913421940, 1930145516, // 64
    1946281151, 1961823931, 1976769119, 1991112165, 2004848699, // 69
    2017974536, 2030485679, 2042378316, 2053648824, 2064293772, // 74
    2074309916, 2083694204, 2092443780, 2100555976, 2108028323, // 79
    2114858545, 2121044559, 2126584483, 2131476629, 2135719506, // 84
    2139311823, 2142252484, 2144540595, 2146175457, 2147156574, // 89
    2147483647 // 90
    };

// ---------------------------------------------------------------------------
// FixedPointSin_0_360
// ---------------------------------------------------------------------------
//
static TInt FixedPointSin_0_360( TInt aDeg )
    {
    FUNC_LOG;
    TInt ret( 0 );
    if ( aDeg <= K90Deg )
        {
        ret = KFixedPointSinTable[ aDeg ];
        }
    else if ( aDeg <= K180Deg )
        {
        ret = KFixedPointSinTable[ K90Deg - ( aDeg - K90Deg ) ];
        }
    else if ( aDeg <= K270Deg )
        {
        ret = -KFixedPointSinTable[ aDeg - K180Deg ];
        }
    else // aDeg <= 360
        {
        ret = -KFixedPointSinTable[ K90Deg - ( aDeg - K270Deg ) ];
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// FixedPointSin
// ---------------------------------------------------------------------------
//
TInt FixedPointSin( TInt aDeg, TInt aDecBits )
    {
    FUNC_LOG;
    aDeg = aDeg % K360Deg; // Clip and look up sine value
    TInt ret( aDeg >= 0 ? FixedPointSin_0_360( aDeg ) : -FixedPointSin_0_360( -aDeg ) );
    if ( aDecBits <= KTableShift ) // Scale sine value to requested range
        {
        ret >>= ( KTableShift - aDecBits );
        }
    else
        {
        ret <<= ( aDecBits - KTableShift );
        }
    return ret;
    }

#endif // SDCOMP_USE_FIXED_POINT || SDCOMP_USE_FIXED_POINT64
