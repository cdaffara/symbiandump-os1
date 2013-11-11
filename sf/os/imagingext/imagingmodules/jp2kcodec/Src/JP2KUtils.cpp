/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  TJ2kUtils class used to provide common public static
*                functions used by all classes in JP2KCodec.
*
*/


// INCLUDE FILES
#include "JP2KUtils.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TUint32 KLog2Table[] = { 0,1,2,4,8,16,32,64,128,256,512,1024,2048,
                               4096,8192,16384,32768,65536,131072,262144,
                               524288,1048576,2097152,4194304,8388608,
                               16777216,33554432,67108864,134217728,
                               268435456,536870912,1073741824,
                               KMaxTUint32 };

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TJ2kUtils::CeilL
// Get the ceiling between two integers.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt32 TJ2kUtils::CeilL( TInt32 aL, TInt32 aR )
    {
    // Check that no divided by zero calculation are done.
    if ( aR == 0 )
    	{
    	User::Leave( KErrCorrupt );
    	}

    if ( aL < 0 )
        {
        if ( aR < 0 )
            {
            aL = -aL;
            aR = -aR;
            return ( aL % aR ) ? ( aL / aR ) + 1 : aL / aR;
            }
        else
            {
            return -( ( -aL ) / aR );
            }
        }
    else if ( aR < 0 )
        {
        return -( aL / ( -aR ) );
        }
    else
        {
        return ( aL % aR ) ? ( aL / aR ) + 1 : aL / aR;
        }
    }

// -----------------------------------------------------------------------------
// TJ2kUtils::FloorL
// Get the floor between two integers.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt32 TJ2kUtils::FloorL( TInt32 aL, TInt32 aR )
    {
    // Check that no divided by zero calculation are done.
    if ( aR == 0 )
    	{
    	User::Leave( KErrCorrupt );
    	}
    
    if ( ( aL < 0 || aR < 0 ) && !( aL < 0 && aR < 0 ) )
        {
        return aL / aR - 1;
        }
    else
        {
        return aL / aR;
        }
    }

// -----------------------------------------------------------------------------
// TJ2kUtils::DivL
// Get the quotient and remainder.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDiv TJ2kUtils::DivL( TInt aNum, TInt aDenom )
    {
    // Check that no divided by zero calculation are done.
    if ( aDenom == 0 )
    	{
    	User::Leave( KErrCorrupt );
    	}
    
    TDiv aDiv;
    aDiv.quot = aNum / aDenom;
    aDiv.rem = aNum % aDenom;

    /*
     * The ANSI standard says that |r.quot| <= |n/d|, where
     * n/d is to be computed in infinite precision.  In other
     * words, we should always truncate the quotient towards
     * 0, never -infinity.
     *
     * Machine division and remainer may work either way when
     * one or both of n or d is negative.  If only one is
     * negative and r.quot has been truncated towards -inf,
     * r.rem will have the same sign as denom and the opposite
     * sign of num; if both are negative and r.quot has been
     * truncated towards -inf, r.rem will be positive ( will
     * have the opposite sign of num ).  These are considered
     * `wrong'.
     *
     * If both are num and denom are positive, r will always
     * be positive.
     *
     * This all boils down to:
     * if num >= 0, but r.rem < 0, we got the wrong answer.
     * In that case, to get the right answer, add 1 to r.quot and
     * subtract denom from r.rem.
     */
    if ( aNum >= 0 && aDiv.rem < 0 ) 
        {
        aDiv.quot++;
        aDiv.rem -= aDenom;
        }
    return aDiv;
    }

// -----------------------------------------------------------------------------
// TJ2kUtils::Log2
// Get he log2 value of an integer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt32 TJ2kUtils::Log2( TUint32 aI )
    {
    TInt32 index = 0;
    TInt32 count = sizeof( KLog2Table ) / sizeof( TUint32 );
    for ( index = 0; index < count; ++index )
        {
        if ( KLog2Table[index] > aI )
            {
            break;   //lint !e960    Break is OK.
            }
        }
    return index - 2;
    }

// -----------------------------------------------------------------------------
// TJ2kUtils::Alloc2DArrayL
// Allocate 2-D array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPrecInt** TJ2kUtils::Alloc2DArrayL( TInt aRowSize, TInt aColSize )
    {
    TPrecInt **ptr = STATIC_CAST( TPrecInt**, User::Alloc( aRowSize * sizeof( TPrecInt* ) ) );
    if ( !ptr )
        {
        User::Leave( KErrNoMemory );
        }
    CleanupStack::PushL( ptr );

    // If first allocation fail, it will not reach here
    TPrecInt *ptrRow = STATIC_CAST( TPrecInt*, User::Alloc( aRowSize * aColSize * sizeof( TPrecInt ) ) );
    if ( !ptrRow )
        {
        User::Leave( KErrNoMemory );
        }
    Mem::FillZ( ptrRow, ( aRowSize * aColSize * sizeof( TPrecInt ) ) );
    for ( TInt i = 0; i < aRowSize; ++i )
        {
        ptr[i] = ptrRow + ( i * aColSize );
        }

    CleanupStack::Pop( 1 );
    return ptr;
    }

// -----------------------------------------------------------------------------
// TJ2kUtils::Free2DArray
// Free 2-D array.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TJ2kUtils::Free2DArray( TPrecInt **aPtr )
    {
    if ( aPtr )
        {
        if ( aPtr[0] )
            {
            User::Free( aPtr[0] );
            }
        User::Free( aPtr );
        }
    }

// -----------------------------------------------------------------------------
// TJ2kUtils::Free2DArray
// Wrapper to free 2-D array when put into TCleanupItem.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TJ2kUtils::Free2DArray( TAny *aPtr ) //lint !e1714 Referenced from CleanupStack item.
    {
    Free2DArray( ( TPrecInt ** )aPtr );
    }

