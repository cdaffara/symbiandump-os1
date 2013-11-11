/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Public API class of the heap-allocated Generic ID, 
*                which provides means to describe, detect, connect and control 
*                an enhancement without need to use HW and/or provider 
*                specific definitions.
*
*/


#ifndef CACCCONGENERICID_H
#define CACCCONGENERICID_H

//  INCLUDES
#include <AccPolGenericID.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Heap-allocated Generic ID class definition to be used in accessory connect and update.
*
*  @lib AccControl.lib
*  @since S60 3.1
*/
class CAccConGenericID : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CAccConGenericID* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CAccConGenericID();

    public: // New functions
        
        /**
        * Get Generic ID header.
        *
        * @since S60 3.1
        * @return Generic ID header.
        */
        IMPORT_C TAccPolGenericID GenericID() const;

    public: // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CAccConGenericID();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public:     // Data
    
    protected:  // Data

    private:    // Data
    
        // Instance of Generic ID static attribute data structure
        TAccPolGenericID iGenericID;
        
        //Storage buffer for capability data
        CBufFlat* iNameValueBuf;//Owned

    public:     // Friend classes
        
    protected:  // Friend classes
        
    private:    // Friend classes
        friend class TAccPolGenericIDAccessor;    
    };

#endif      // CACCCONGENERICID_H   
            
// End of File
