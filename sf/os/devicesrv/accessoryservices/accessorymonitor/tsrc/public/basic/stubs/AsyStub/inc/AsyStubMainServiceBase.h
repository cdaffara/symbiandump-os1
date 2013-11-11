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


#ifndef CASYSTUBMAINSERVICEBASE_H
#define CASYSTUBMAINSERVICEBASE_H

//  INCLUDES
#include <ASYMainServiceBase.h>
#include "AccessoryServer.h"
#include "AsyStubService.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Creates ASY main service
*/
class CASYStubMainServiceBase : public CASYMainServiceBase
    {
public:  // Constructors and destructor
    
    /**
    * Two-phased constructor.        
	* @return The created object.
    */
    static CASYStubMainServiceBase* NewL();	                    
    
	TInt GetTestCaseID();
	
	TTFCapability GetAndRemoveCapability();
	
public: // From base class
    
	/**
    * Processes request command from server
    */
    TInt StartL();
    
private: // Functions
    
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    CASYStubMainServiceBase();
    
	/**
	* Destructor.
	*/
	~CASYStubMainServiceBase();
	
private:    // Data
    
    CASYStubService* iService;
    
    };

#endif // CASYSTUBMAINSERVICEBASE_H

// End of File