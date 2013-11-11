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
* Description:  Basic utilities to handle conversion and object safe deletion.
*
*/


#ifndef  __JP2KIMAGEUTILS_H__
#define __JP2KIMAGEUTILS_H__

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// TEMPLATE DECLARATION

/**
*  Basic reading utility class.
*  Utility class with methods for standard 
*  reading stuff from a TUint8* string. 
*
 * JP2KCodec.dll
 * @since 2.6
*/
class PtrReadUtil
    {
    public:  // Constructors and destructor
        
    public: // New functions
        
        /**
        * Reads big endian value from given buffer.
        * @since 2.6
        * @param aPtr: Pointer to buffer
        * @TUint16: Read value.
        */
        static TUint16 ReadBigEndianUint16( const TUint8* aPtr );

        /**
        * Reads big endian value from given buffer.
        * @since 2.6
        * @param aPtr: Pointer to buffer
        * @TUint32: Read value.
        */
        static TUint32 ReadBigEndianUint32( const TUint8* aPtr );

        /**
        * Reads big endian value from given buffer and increments pointer.
        * @since 2.6
        * @param aPtr: Pointer to buffer
        * @TUint16: Read value.
        */
        static TUint16 ReadBigEndianUint16Inc( const TUint8*& aPtr );

        /**
        * Reads big endian value from given buffer and increments pointer.
        * @since 2.6
        * @param aPtr: Pointer to buffer
        * @TUint32: Read value.
        */
        static TUint32 ReadBigEndianUint32Inc( const TUint8*& aPtr );

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

#include "JP2KImageUtils.inl"

#endif  // __JP2KIMAGEUTILS_H__



