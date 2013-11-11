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
*				implements the functions specific to Last Known Position Area request.
*
*/



#ifndef CPOSLASTKNOWNPOSAREAHANDLER_H_
#define CPOSLASTKNOWNPOSAREAHANDLER_H_

// INCLUDES
// TODO  ----------------------------------------- Remove the ones not needed
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
// TODO -------------------------------------------------------------------------------
//  Index of the pointer to a container that holds pointer descriptors, needed to point
//	to the clients request data during asynchronous requests */
const TInt KParamLKPAreaReqPos   = 0;
const TInt KParamLKPAreaReqArea  = 1;
// The timeout value for get last known position area request is 10seconds
const TTimeIntervalMicroSeconds KLastKnownPosAreaTimeOut(10000000);
//--------------------------------------------------------------------------------------

// CLASS DECLARATION
/* 
 * Active object to request the LastKnownPositionArea from the 
 * Location monitor 
 */
class CPosLastKnownPosAreaHandler : public CPosLocMonitorReqHandlerBase
	{
public: 
	// Two-phased Constructors 
	static CPosLastKnownPosAreaHandler* NewL(); 
    // Destructors
	~CPosLastKnownPosAreaHandler();  
	// Function to request Position info from Location Monitor
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
	
	TPositionInfo   iPositionInfo;
	TPositionAreaExtendedInfo iPositionAreaInfo;
	CPosCallbackTimer* 	iTimeoutTimer; // The timeout value is hardcoded in the constant KLastKnownPosAreaTimeOut
	};



#endif /*CPOSLASTKNOWNPOSAREAHANDLER_H_*/

