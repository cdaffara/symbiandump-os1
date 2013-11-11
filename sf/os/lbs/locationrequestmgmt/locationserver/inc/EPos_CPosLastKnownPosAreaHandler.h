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
* Description:  	This class inherits the common functionalities for requests to the 
*				Location Monitor from EPos_CPosLocMonitorReqHandlerBase.h and also
*				implements the functions specific to Last Known Position Area request.
*
*/



#ifndef CPOSLASTKNOWNPOSAREAHANDLER_H_
#define CPOSLASTKNOWNPOSAREAHANDLER_H_

//--------------------------------------------------------------------------------------
// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32def.h>
#include <e32cmn.h>
#include "LbsErrors.h" 
#include "EPos_Global.h" 		// For DEBUG_TRACE
#include "EPos_ServerPanic.h" 	// For EPosServerPanicPositionerNotInitialized

#include "EPos_CPosLocMonitorReqHandlerBase.h"
#include "rlbslocmonitorsession.h"
#include "rlbslocmonitorareapositioner.h"


//--------------------------------------------------------------------------------------
// FORWARD DECLARATIONS
class CPosCallbackTimer;	// Used for timeout


//--------------------------------------------------------------------------------------
// CONSTANT DEFINITIONS
//  Index of the pointer to a container that holds pointer descriptors, needed to point
//	to the clients request data during asynchronous requests */
const TInt KParamLKPAreaReqPos   = 0;
const TInt KParamLKPAreaReqArea  = 1;

// The timeout value for get last known position area request is 10seconds
const TTimeIntervalMicroSeconds KLastKnownPosAreaTimeOut(10000000);


//--------------------------------------------------------------------------------------
// CLASS DECLARATION
/**
 The active object that handles requests for last known position with area info from 
 clients. Owns subsession with the location monitor and uses the sub-session handle to 
 forward the above requests to the location monitor. Inherits the basic functionalities from 
 CPosLocMonitorReqHandlerBase.
 */
class CPosLastKnownPosAreaHandler : public CPosLocMonitorReqHandlerBase
	{
public: 
	// Two-phased Constructors 
	static CPosLastKnownPosAreaHandler* NewL(); 
    // Destructors
	~CPosLastKnownPosAreaHandler();  
	
	// Functions to request last known position with area info from the Location Monitor
    void GetLastKnownPosAreaL(RLbsLocMonitorSession& aLocMonSession, const RMessage2& aMessage);
    void CancelGetLastKnownPosAreaL(const RMessage2& aMessage);
    void NotifyOnEmptyLastKnownPosStoreReq();
    
private:
	// C++ Default constructor
	CPosLastKnownPosAreaHandler(); 
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
	RLbsLocMonitorAreaPositioner 	iLocMonAreaPositioner; 
	
	TPositionInfo   			iPositionInfo;
	TPositionAreaExtendedInfo 	iPositionAreaInfo;
	TPosAreaReqParams 			iParameters;
	CPosCallbackTimer* 			iTimeoutTimer; // The timeout value is hardcoded in the constant KLastKnownPosAreaTimeOut
	};
#endif /*CPOSLASTKNOWNPOSAREAHANDLER_H_*/

