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



#ifndef __CBTASYSTUBMAINSERVICEBASE_H_
#define __CBTASYSTUBMAINSERVICEBASE_H_

//  INCLUDES
#include <ASYMainServiceBase.h>
#include "AccessoryServer.h"
#include "BTAsyStubService.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
//class CAccessoryServer;

// CLASS DECLARATION

/**
*  Creates ASY main service
*  
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
										    
class CBTASYStubMainServiceBase : public CASYMainServiceBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.        
		* @return The created object.
        */
        static CBTASYStubMainServiceBase* NewL();	                    

		TInt GetTestCaseID();
		
    public: // From base class
                
		/**
        * Processes request command from server 
        * @since Series ?XX ?SeriesXX_version
        * @param aCommand
        * @param aTrId
        * @param aData
        * @return void
        */
        TInt StartL( /* TAny* aStartUpParams */ );
                
    private: // Functions

        /**
        * By default Symbian 2nd phase constructor is private.
        */        
	    CBTASYStubMainServiceBase();        

		/**
		* Destructor.
		*/
		~CBTASYStubMainServiceBase();
	
    private:    // Data
    
    CBTASYStubService* iService; //owned

    };

#endif // __CBTASYSTUBMAINSERVICEBASE_H_
            
// End of File