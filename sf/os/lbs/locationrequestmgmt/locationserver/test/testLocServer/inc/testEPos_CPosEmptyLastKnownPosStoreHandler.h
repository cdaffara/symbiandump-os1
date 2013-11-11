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
* Description:  	This class inherits the common functionalities for requests to the 
*				Location Monitor from EPos_CPosLocMonitorReqHandlerBase.h and also
*				implements the functions specific to empty last known position store req.
*
*/



#ifndef CPOSEMPTYLASTKNOWNPOSSTOREHANDLER_H_
#define CPOSEMPTYLASTKNOWNPOSSTOREHANDLER_H_

// INCLUDES
// TODO Remove the ones not needed --------------------------------------------------------------
#include <e32base.h>
#include <e32std.h>
#include <e32def.h>
#include <e32cmn.h>
#include "LbsErrors.h" 

#include "EPos_Global.h" 		// For DEBUG_TRACE
#include "EPos_ServerPanic.h" 	// For EPosServerPanicPositionerNotInitialized
// TODO ---------------------------------------------------------------------- -------------------
// Test components of location server to use the test version of the location monitor interface
#include "testEPos_CPosLocMonitorReqHandlerBase.h"
// Test Location Monitor interface
#include "lbstestlocmonitorapi.h"
// TODO ------------------------------------------------------------------------------------------

// FORWARD DECLARATIONS
class CPosCallbackTimer;	// Used for timeout

// CONSTANT DEFINITIONS
// The timeout value for the EmptyLastKnownPositionStore request is 10seconds
const TTimeIntervalMicroSeconds KEmptyLastKnownPosStoreTimeOut(10000000);

// CLASS DECLARATION
/* 
 * Active object to request the LastKnownPosition from the 
 * Location monitor 
 */
class CPosEmptyLastKnownPosStoreHandler : public CPosLocMonitorReqHandlerBase
	{
public: 
	// Two-phased Constructors 
	static CPosEmptyLastKnownPosStoreHandler* NewL(); 
    // Destructors
	~CPosEmptyLastKnownPosStoreHandler();  

    void EmptyLastKnownPosStoreL(const RLbsLocMonitorSession& aLocMonitorSession, const RMessage2& aMessage);
    void CancelEmptyLastKnownPosStoreL(const RMessage2& aMessage);
    
private:
	// C++ Default constructor
	CPosEmptyLastKnownPosStoreHandler(); 
	void ConstructL(); 	
	static TInt HandleTimeOut(TAny* aRequestHandler);
	
protected:  
	// From CActive
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();
    
private:
	// Session with the location monitor
	RLbsLocMonitorSession 	iLocMonitorSession; 
	CPosCallbackTimer* 	iTimeoutTimer; 		// The timeout value is hardcoded in the constant KLastKnownPosTimeOut
	};



#endif /*CPOSEMPTYLASTKNOWNPOSSTOREHANDLER_H_*/

