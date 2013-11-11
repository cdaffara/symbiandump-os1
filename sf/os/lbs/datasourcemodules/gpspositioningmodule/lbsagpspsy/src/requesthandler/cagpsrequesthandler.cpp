// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @InternalComponent
*/

#include "cagpsrequesthandler.h"
#include "cpositionerq.h"
#include "psylogging.h"
#include "tagpsrequestparameters.h"
#include "tpositionercall.h"
#include "agpsrequestconstants.h"

const TInt KMergeTableRows = 4;
const TInt KMergeTableColumns = 4;

const TInt KModeIndexNone = 0;
const TInt KModeIndexA = 1;
const TInt KModeIndexTB = 2;
const TInt KModeIndexTA = 3;


const TInt ModeMergeTable[KMergeTableColumns][KMergeTableRows] = 
	{
		//None			A,					TB,					TA
		{KErrGeneral,	KAutonomousMode,	KTerminalBasedMode,	KTerminalAssistedMode}, // None
		{KErrGeneral,	KAutonomousMode, 	KTerminalBasedMode,	KErrGeneral},			// A
		{KErrGeneral,	KTerminalBasedMode,	KTerminalBasedMode,	KErrGeneral},			// TB
		{KErrGeneral,	KErrGeneral,		KErrGeneral,		KTerminalAssistedMode}	// TA
	};


/**
 CRequestHandler::NewL
 Two-phased constructor.
*/
CAgpsRequestHandler* CAgpsRequestHandler::NewL(CPositionerQ* aPositionerQ)
    {
    CAgpsRequestHandler* self = new( ELeave ) CAgpsRequestHandler(aPositionerQ);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

/*
* CAgpsRequestHandler::~CAgpsRequestHandler
* Destructor.
*/
CAgpsRequestHandler::~CAgpsRequestHandler()
    {
    TRACESTRING("CAgpsRequestHandler::~CAgpsRequestHandler()")
	// Send standby advice for first positioner
	TTracking tracking;
	IssueCancel(iPositionerQ->IterETrue(tracking), TLbsPositionUpdateRequestBase::EPowerAdviceOff);

	delete iAGPSRequestBus;
    }

/**
@param aPositionerQ	the positioner q 
*/
CAgpsRequestHandler::CAgpsRequestHandler(CPositionerQ* aPositionerQ) 
	{
	iPositionerQ = aPositionerQ;
	}

/**
construct the request bus by channel identifier
*/
void CAgpsRequestHandler::ConstructL()
	{
	CRequestHandler::ConstructL();
	
	iAGPSRequestBus = CAgpsRequestBus::NewL(KChannelArray[KAGpsManagerToLocServerChannelIndex]);

	// Send standby advice for first positioner
	TTracking tracking;
	IssueStatus(iPositionerQ->IterETrue(tracking), TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
	
	User::LeaveIfError(LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, iCapabilities));
	}

/**
* Merges the positioning methods and checks that the requested method is
* compatible with the hardware capabiliies. Some hardware can support multiple
* modes which can be simultaneous. 
*
* @param aNewReq
*/
TInt CAgpsRequestHandler::MergeMethod(TLbsNetPosRequestMethodInt& aNewReq)
	{
	TInt error = KErrNone;
	TLbsNetPosMethodInt mergedMethod;
	TLbsNetPosMethodInt newMethod;
	
	iMergedRequest.RequestMethod().GetPosMethod(0, mergedMethod);
	aNewReq.GetPosMethod(0, newMethod);
	
	TInt mergedModeIndex = ModeToIndex(mergedMethod.PosMode());
	TInt newModeIndex	= ModeToIndex(newMethod.PosMode());
	TInt resultMode = ModeMergeTable[mergedModeIndex][newModeIndex];

		
	if(iCapabilities == TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB)
		{
		if(resultMode == KErrGeneral)
			{
			resultMode = KErrInUse;		
			}
		}
	else if(iCapabilities == (TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased 
				| TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted))
		{
		if(resultMode == KErrGeneral)
			{
			resultMode = KErrInUse;		
			}
		}
	else if(iCapabilities == TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased)
		{
		if((resultMode == KErrGeneral) || (newModeIndex == KModeIndexTA) 
				|| (mergedModeIndex == KModeIndexTA))
			{
			resultMode = KErrCorrupt;		
			}
		}
	else if(iCapabilities == TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted)
		{
		if((resultMode == KErrGeneral) || (newModeIndex != KModeIndexTA) 
				|| (mergedModeIndex == KModeIndexTB) || (mergedModeIndex == KModeIndexA))
			{
			resultMode = KErrCorrupt;		
			}
		}
	else
		{
		resultMode = KErrCorrupt;
		}
	
	
	if((resultMode == KAutonomousMode) 
		|| (resultMode == KTerminalBasedMode) 
		|| (resultMode == KTerminalAssistedMode))
		{
		mergedMethod.SetPosMethod(KLbsPositioningMeansGps, resultMode);
		iMergedRequest.RequestMethod().SetPosMethods(&mergedMethod, 1);
		}
	else
		{
		error = resultMode;
		}
		
	return error;
	}

/**
Merge the request with the local iMergedRequest

@param aTime The target time of the request
@param aTimeForFix The maximum fix time
@param aQuality The requested position info quality
@return KErr depending on the reason for the failure of the method merge
@see CPositionerSubSession
*/
TInt CAgpsRequestHandler::MergeRequest(const TRequestParameters* aRequestParameters, const TBool aTracking, 
							TLbsPositionUpdateRequestBase::TPowerAdvice aPower)
	{
	TPositionQuality posQuality; 
	TLbsPositionUpdateRequest newReq;
	TLbsLocRequestQuality newRequestQuality;
	const TAgpsRequestParameters* reqParams = static_cast<const TAgpsRequestParameters*>(aRequestParameters);

	reqParams->iCriteria.GetRequiredQuality(posQuality);
	
	BuildUpdateRequest(newRequestQuality, reqParams->iTimeForFix, posQuality);
	newReq.TargetTime() 	= reqParams->iTargetTime;
	
	newReq.RequestQuality().SetMaxFixTime(newRequestQuality.MaxFixTime());
	newReq.RequestQuality().SetMinHorizontalAccuracy(newRequestQuality.MinHorizontalAccuracy());
	newReq.RequestQuality().SetMinVerticalAccuracy(newRequestQuality.MinVerticalAccuracy());

	newReq.NewClient() 		= reqParams->iNewClient;
	newReq.Tracking() 		= aTracking;
	newReq.SetPowerAdvice(aPower);
	
	TLbsNetPosMethodInt mArray[1];
	mArray[0].SetPosMethod(KLbsPositioningMeansGps, reqParams->iAGPSMode);
	newReq.RequestMethod().SetPosMethods(mArray, 1);

	if(newReq.TargetTime() < iMergedRequest.TargetTime())
		{
		iMergedRequest.TargetTime() = newReq.TargetTime();
		}
		
	if(newReq.RequestQuality().MinHorizontalAccuracy() < iMergedRequest.RequestQuality().MinHorizontalAccuracy())
		{
		iMergedRequest.RequestQuality().SetMinHorizontalAccuracy(newReq.RequestQuality().MinHorizontalAccuracy());
		}
		
	if(newReq.RequestQuality().MinVerticalAccuracy() < iMergedRequest.RequestQuality().MinVerticalAccuracy())
		{
		iMergedRequest.RequestQuality().SetMinVerticalAccuracy(newReq.RequestQuality().MinVerticalAccuracy());
		}

	if(newReq.RequestQuality().MaxFixTime() < iMergedRequest.RequestQuality().MaxFixTime())
		{
		iMergedRequest.RequestQuality().SetMaxFixTime(newReq.RequestQuality().MaxFixTime());
		}
		
	iMergedRequest.NewClient() = iMergedRequest.NewClient() || newReq.NewClient();
	iMergedRequest.Tracking()  = iMergedRequest.Tracking()  || newReq.Tracking();
 
 	TLbsPositionUpdateRequest& reqNC = const_cast<TLbsPositionUpdateRequest&>(newReq);
 	iMergedRequest.SetPowerAdvice(reqNC.PowerAdvice());
 	
 	return MergeMethod(newReq.RequestMethod());
	}

/** Used before merging */
void CAgpsRequestHandler::ResetMergedRequest()
	{
	iMergedRequest.NewClient()  = EFalse;
	iMergedRequest.Tracking()   = EFalse;
	iMergedRequest.TargetTime() = KMaxTInt64;
	iMergedRequest.RequestQuality().SetMinHorizontalAccuracy(KMaxTReal32);
	iMergedRequest.RequestQuality().SetMinVerticalAccuracy(KMaxTReal32);
	iMergedRequest.RequestQuality().SetMaxFixTime(KMaxTInt64);
	TLbsNetPosMethodInt mArray[1];
	mArray[0].SetPosMethod(KLbsPositioningMeansGps, 0);
	iMergedRequest.RequestMethod().SetPosMethods(mArray, 1);
	}	

/**
Publish the request to the request bus
*/
void CAgpsRequestHandler::IssueMergedRequestL()
	{
    TRACESTRING("CAgpsRequestHandler::IssueMergedRequestL()")
	iCurrentRequest = iMergedRequest;
	TLbsPositionUpdateRequest request = iCurrentRequest;
	
	// If not tracking force the request target time to be 0,
	// the AGPS integration module manager expects this behaviour
	if (!request.Tracking())
		{
		request.TargetTime() = TTime(0);
		}
		
	iAGPSRequestBus->PublishRequestL(request);
	}
	
/**
Publish the Cancel request to the request bus.
Cancel always disables tracking (the tracking flag is EFalse by default)
*/
void CAgpsRequestHandler::IssueCancel(TBool aTracking, TLbsPositionUpdateRequestBase::TPowerAdvice aPower)
	{
	TRACESTRING("CAgpsRequestHandler::IssueCancel()")
	TLbsPositionUpdateRequestCancel cancel;

	// note, here we choose to send EPowerAdviceStandby straight away
	cancel.SetPowerAdvice(aPower);
	cancel.Tracking() = aTracking;
	
	TRAP_IGNORE(iAGPSRequestBus->PublishRequestL(cancel));
	}
	
/**
Publish statutory information (e.g tracking) to the request bus.
*/
void CAgpsRequestHandler::IssueStatus(TBool aTracking, TLbsPositionUpdateRequestBase::TPowerAdvice aPower)
   	{
   	TLbsPositionUpdateRequestStatus status;
   	status.Tracking() = aTracking;
 	status.SetPowerAdvice(aPower);
   	TRAP_IGNORE(iAGPSRequestBus->PublishRequestL(status));
   	}


TInt CAgpsRequestHandler::ModeToIndex(TInt aMode)
	{
	if(aMode == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted))
		{
		return KModeIndexTA;
		}
	else if(aMode == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted))
		{
		return KModeIndexTB;
		}
	else if(aMode == TPositionModuleInfo::ETechnologyTerminal)
		{
		return KModeIndexA;
		}
	else
		{
		return KModeIndexNone;
		}
	}
