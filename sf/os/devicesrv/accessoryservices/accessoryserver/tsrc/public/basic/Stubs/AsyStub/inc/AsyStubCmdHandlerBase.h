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
* Description:  ?Description
*
*/


#ifndef __CASYSTUBCMDHANDLERBASE_H_
#define __CASYSTUBCMDHANDLERBASE_H_

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "AsyCmdTypes.h"
#include "AsyStubCmdObserver.h"
#include <AsyCommandHandlerBase.h>
#include <AccPolValueRecord.h>//TAccPolValueType

#ifdef __WINS__
static 	TInt iDistributor = 0;
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
*  ?other_description_lines
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
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
        * @since Series ?XX ?SeriesXX_version
        * @param aCommand
        * @param aTrId
        * @param aData
        * @return void
        */
        void ProcessCommandL( const TProcessCmdId aCommand, const TASYCmdParams& aCmdParams );
        
        void ProcessObjectL( const TProcessCmdId aCommand, const TASYCmdParams& aCmdParams  );	

    private: //functions
             
		/**
        * C++ default constructor.
        * @param aParams Initialisation parameters.
        */
   		CASYStubCmdHandlerBase();
   		
   		/**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        void Start(TInt aTimeMs, TInt64 aValue, TAccPolValueType aValueType );
        //For callbacks
        static TInt TimeOutL( TAny* aThis );
        void DoTimeOutL();

    private:    // Data
    
    	CAsyStubCmdObserver* iCmdHandler;
    	CPeriodic* iTimer;
    	TInt iResponse;
    	TAccPolValueType iResponseType;
    	
    	TInt volume;
	    TInt min;
	    TInt max;
	    TInt res;
   	public:     // Friend classes
        friend void CAsyStubCmdObserver::RunL();
    	
    };

#endif // CASYSTUBCMDHANDLERBASE_H_
