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
*                   serialization of a variable length subblock name array 
*                   into/from a stream. This way it can be passed between 
*                   process boundaries. The Accessory Server and the -Policy 
*                   use the operations.
*
*/



#ifndef TACCPOLCAPSSUBBLOCKNAMEARRAYSERIAL_H
#define TACCPOLCAPSSUBBLOCKNAMEARRAYSERIAL_H

//  INCLUDES
#include <s32strm.h>
#include <AccPolSubblockNameArray.h>

// CLASS DECLARATION

/**
*  Accessory Policy Capability Subblock Name Array Serialization
*
*  @lib AccPolicy
*  @since S60 3.1
*/
class TAccPolCapsSubblockNameArraySerial
    {
    public:  // Constructors and destructor

    public:  // New functions

        /**
        * Internalize name array data from the read stream.
        *
        * @since S60 3.1
        * @param aStrem Stream from which the array is serialized from.
        * @param aArray Name array to which the data is internalized.
        * @return void
        */
        IMPORT_C static void InternalizeL( RReadStream& aStream, 
                                           CAccPolSubblockNameArray& aArray  );

        /**
        * Externalize name array data to the write stream.
        *
        * @since S60 3.1
        * @param aArray Name array to be externalized.
        * @param aStrem Stream to which the array is serialized to.
        * @return void
        */
        IMPORT_C static void ExternalizeL( const CAccPolSubblockNameArray& aArray, 
                                           RWriteStream& aStream );

    public: // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        TAccPolCapsSubblockNameArraySerial();

    public:     // Data
    
    protected:  // Data

    private:    // Data

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif      // TACCPOLCAPSSUBBLOCKNAMEARRAYSERIAL_H

// End of File