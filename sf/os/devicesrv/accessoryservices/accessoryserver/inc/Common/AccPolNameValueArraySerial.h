/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class defines static operations that are used in the 
*                serialization of a variable length name value array 
*                into/from a stream. This way it can be passed between 
*                process boundaries. The Accessory Server and the -Policy 
*                use the operations.
*
*/



#ifndef TACCPOLNAMEVALUEARRAYSERIAL_H
#define TACCPOLNAMEVALUEARRAYSERIAL_H

//  INCLUDES
#include <s32strm.h>
#include <AccPolNameValueRecord.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Accessory Policy Name Value Array Serialization
*
*  @lib AccPolicy
*  @since S60 3.1
*/
class TAccPolNameValueArraySerial
    {
    public:  // Constructors and destructor

    public:  // New functions

        /**
        * Externalize name value array data to the write stream.
        *
        * @since S60 3.1
        * @param aArray Name array to be externalized.
        * @param aStrem Stream to which the array is serialized to.
        * @return void
        */
        IMPORT_C static void ExternalizeL( const RArray<TAccPolNameValueRecord>& aNameValueArray, 
                                           RWriteStream& aStream );

    public: // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        TAccPolNameValueArraySerial();

    public:     // Data
    
    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // TACCPOLNAMEVALUEARRAYSERIAL_H

// End of File