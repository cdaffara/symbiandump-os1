/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*				implements the functions specific to Last Known Position request.
*
*/



#ifndef CPOSLASTKNOWNPOSHANDLER_H_
#define CPOSLASTKNOWNPOSHANDLER_H_

//--------------------------------------------------------------------------------------
// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32def.h>
#include <e32cmn.h>
#include "LbsErrors.h" 
#include "EPos_CPosLocMonitorReqHandlerBase.h"
#include "EPos_Global.h" 		// For DEBUG_TRACE
#include "EPos_ServerPanic.h" 	// For EPosServerPanicPositionerNotInitialized
#include "rlbslocmonitorsession.h"
#include "rlbslocmonitorareapositioner.h"

//--------------------------------------------------------------------------------------
// FORWARD DECLARATIONS
class CPosCallbackTimer;	// Used for timeout

//--------------------------------------------------------------------------------------
// CONSTANT DEFINITIONS
// The timeout value for get last known position request is 10seconds
const TTimeIntervalMicroSeconds KLastKnownPosTimeOut(10000000);

//--------------------------------------------------------------------------------------
// CLASS DECLARATION
/**
  The active object that handles requests for last known position from clients. 
  Owns subsession with the location monitor and uses the sub-session handle to 
  forward the above requests to the location monitor. Inherits the basic functionalities 
  from CPosLocMonitorReqHandlerBase.
 */
class CPosLastKnownPosHandler : public CPosLocMonitorReqHandlerBase
	{
public: 
	// Two-phased Constructors 
	static CPosLastKnownPosHandler* NewL(); 
    // Destructors
	~CPosLastKnownPosHandler();  
	// Function to request last known position info from the Location Monitor
    void GetLastKnownPosL(RLbsLocMonitorSession& aLocMonSession, const RMessage2& aMessage);
    void CancelGetLastKnownPosL(const RMessage2& aMessage);
    void NotifyOnEmptyLastKnownPosStoreReq();
    
private:
	// C++ Default constructor
	CPosLastKnownPosHandler(); 
	void ConstructL(); 	
	static TInt HandleTimeOut(TAny* aRequestHandler);
	
protected:  
	// From CActive
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();
    
private:
	RMessage2 		iMessage;
	// Subsession with the location monitor
	RLbsLocMonitorAreaPositioner 	iLocMonPositioner; 
	
	TPositionInfo   				iPositionInfo;
	CPosCallbackTimer* 				iTimeoutTimer; // The timeout value is hardcoded in the constant KLastKnownPosTimeOut
	};



#endif /*CPOSLASTKNOWNPOSHANDLER_H_*/
