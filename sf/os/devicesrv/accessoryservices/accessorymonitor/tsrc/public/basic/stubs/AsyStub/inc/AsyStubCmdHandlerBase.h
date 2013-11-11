/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used as an reference implementation
*
*/


#ifndef CASYSTUBCMDHANDLERBASE_H
#define CASYSTUBCMDHANDLERBASE_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <AsyCommandHandlerBase.h>
#include <AccPolValueRecord.h>   //TAccPolValueType
#include "AsyCmdTypes.h"
#include "AsyStubCmdObserver.h"

#ifdef __WINS__
static TInt iDistributor = 0;
#endif

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CAsyStubCmdObserver;

// CLASS DECLARATION

/**
*  Process request command from AccessoryServer
*/
class CASYStubCmdHandlerBase : public CASYCommandHandlerBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aCmdService callback to ASY Command Handler.
		* @return The created object.
        */
        static CASYStubCmdHandlerBase* NewL();
        
        /**
        * Destructor.
        */
        ~CASYStubCmdHandlerBase();

    public: // From base class
        
		/**
        * Processes request command from server 
        * @param aCommand Command to process.
        * @param aCmdParams Params of command to process.
        */
        void ProcessCommandL( const TProcessCmdId aCommand, const TASYCmdParams& aCmdParams );

    private: //functions
             
		/**
        * C++ default constructor.
        */
   		CASYStubCmdHandlerBase();
   		
   		/**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Timer start
        */
        void Start(TInt aTimeMs, TInt64 aValue, TAccPolValueType aValueType );
        
        /**
        * Timer time out
        */
        static TInt TimeOutL( TAny* aThis );
        
        /**
        * Timer time out
        */
        void DoTimeOutL();

    private:    // Data
    
    	CAsyStubCmdObserver* iCmdHandler;
    	CPeriodic* iTimer;
    	TInt iResponse;
    	TAccPolValueType iResponseType;
    	
   	public:     // Friend classes
        friend void CAsyStubCmdObserver::RunL();
    	
    };

#endif // CASYSTUBCMDHANDLERBASE_H
