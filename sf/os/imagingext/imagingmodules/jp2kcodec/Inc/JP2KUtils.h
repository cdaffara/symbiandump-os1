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


#ifndef __JP2KUTILS_H__
#define __JP2KUTILS_H__

//  INCLUDES
#include <e32base.h>

// CONSTANTS
const TInt KImplementationPrecision = 32;

// MACROS

// DATA TYPES

typedef TInt32 TPrecInt;

struct TDiv
    {
    TInt quot; // Quotient
    TInt rem;  // Remainder
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  TJ2kUtils class used to provide common public static
*  methods used by all classes
*
*  JP2KCodec.dll
*  @since 2.6
*/
class TJ2kUtils
    {
    public:  // Constructors and destructor
        
    public: // New functions
        
        /**
        * Get the ceiling between two integers
        * @since 2.6
        * @param aL: first integer to compute.
        * @param aR: second integer to compute.
        * @return TInt32: the ceiling between two integers aL and aR.
        */
        static TInt32 CeilL( TInt32 aL, TInt32 aR );

        /**
        * Get the floor between two integers
        * @since 2.6
        * @param aL: first integer to compute.
        * @param aR: second integer to compute.
        * @return TInt32: the floor between two integers aL and aR.
        */
        static TInt32 FloorL( TInt32 aL, TInt32 aR );

        /**
        * Get the quotient and remainder
        * @since 2.6
        * @param aNum: first integer to compute.
        * @param aDenom: divisor integer.
        * @return TDiv: the structure contains the quotient and remainder.
        */
        static TDiv DivL( TInt aNum, TInt aDenom );

        /**
        * Get the log2 value of an integer
        * @since 2.6
        * @param aI: an integer to compute.
        * @return TInt32: the log2 value of the integer aI.
        */
        static TInt32 Log2( TUint32 aI );

        /**
        * Allocate 2-D array
        * @since 2.6
        * @param aRowSize: row size of the 2-D array.
        * @param aColSize: column size of the 2-D array.
        * @return TPrecInt**: a pointer of pointer to TPrecInt.
        */
        static TPrecInt** Alloc2DArrayL( TInt aRowSize, TInt aColSize );

        /**
        * Free 2-D array
        * @since 2.6
        * @param aPtr: a pointer of pointer to TPrecInt to be freed.
        */
        static void Free2DArray( TPrecInt **aPtr );

        /**
        * Wrapper to free 2-D array when put into TCleanupItem
        * @since 2.6
        * @param aPtr: a pointer of pointer to TAny to be freed.
        */
        static void Free2DArray( TAny *aPtr );

    public: // Functions from base classes
       
    protected:  // New functions        

    protected:  // Functions from base classes

    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data

    public:     // Friend classes
 
    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // __JP2KUTILS_H__
