/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




/**
 @file
 @internalTechnology 
*/

#ifndef HWRMSESSION_H
#define HWRMSESSION_H

//  INCLUDES
#include <e32base.h>
//#include "HWRMServer.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CHWRMServer;
class CHWRMService;

// CLASS DECLARATION

/**
*  Represents a session for a client thread on the server-side.
*  A session acts as a channel of communication between the client 
*  and the server. 
*
*/
class CHWRMSession : public CSession2
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHWRMSession* NewL(CHWRMServer& aServer);
        
        /**
        * Destructor.
        */
        virtual ~CHWRMSession();
        
    public: // New functions
        
    public: // Functions from base classes
 
    protected:  // New functions
        
    protected:  // Functions from base classes
        
        /**
        * From CSession2
        * @param aMessage The message containing the client request
        */
        virtual void ServiceL( const RMessage2& aMessage );

    private:

        /**
        * C++ default constructor.
        */
        CHWRMSession(CHWRMServer& aServer);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Calls CTaskOrganizer's function to execute operation
        * @param aMessage The message containing the client request
        */
        void DispatchMessageL( const RMessage2& aMessage );
        
    public:     // Data
    
    protected:  // Data

    private:    // Data

    	/**
    	* HWRM server reference. 
    	* Not owned.
    	*/
    	CHWRMServer& iHWRMServer; 

        /**
    	* Pointer to the service object.
    	*/
    	CHWRMService* iService;

    	/**
    	* A flag to indicate if message completion is needed in ServiceL.
    	* Async and plugin calls set this to false.
    	*/ 
    	TBool iCompletionNeeded;
    	

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };

#endif      // HWRMSESSION_H 
            
// End of File
