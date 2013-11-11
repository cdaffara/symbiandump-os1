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
// GPS Integration Module Observer for AGPS Manager UNIT testing
// 
//

#include "ctestgpsmoduleobserver.h"
#include <lbs/lbslocdatasourceclasstypes.h>

CTestGpsModuleObserver* CTestGpsModuleObserver::NewL()
	{
	CTestGpsModuleObserver* self = new(ELeave) CTestGpsModuleObserver();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestGpsModuleObserver::CTestGpsModuleObserver()
	{
	}

CTestGpsModuleObserver::~CTestGpsModuleObserver()
	{
	delete iRequestedGpsOptions;
	}

void CTestGpsModuleObserver::ConstructL()
	{
	CTestObserverBase::ConstructL();
	}

// Callback from test GPS module to let the test know that the 
// AGPS Manager has requested a location update.
void CTestGpsModuleObserver::RequestLocationUpdate(const TTime& aTargetTime, const TLbsLocRequestQuality& aQuality)
	{
	iObservedGpsEvents |= ELocationUpdateReqCall;
	
	iRequestedTargetTime = aTargetTime;
	
	// convert from external quality to internal
	iRequestedQuality.SetMaxFixTime(aQuality.MaxFixTime());
	iRequestedQuality.SetMinHorizontalAccuracy(aQuality.MinHorizontalAccuracy());
	iRequestedQuality.SetMinVerticalAccuracy(aQuality.MinVerticalAccuracy());
	
	SignalObserverCallBack(ECallBackErrorNone);
	}

// Callback from test GPS module to let the test know that the 
// AGPS Manager has requested a location update to be cancelled.
void CTestGpsModuleObserver::CancelLocationRequest()
	{
	iObservedGpsEvents |= ECancelLocationCall;	
	SignalObserverCallBack(ECallBackErrorNone);
	}

// Callback from test GPS module to let the test know that the 
// AGPS Manager has advised a new power mode
void CTestGpsModuleObserver::AdvisePowerMode(CLbsLocationSourceGpsBase::TPowerMode aMode)
	{
	iObservedGpsEvents |= EAdvisePowerModeCall;

	iAdvisedPowerMode = aMode;
	
	SignalObserverCallBack(ECallBackErrorNone);	
	}

// Callback from test GPS module to let the test know that the 
// AGPS Manager has set new GPS options.
void CTestGpsModuleObserver::SetGpsOptions(const TLbsGpsOptions& aGpsOptions)
	{
	iObservedGpsEvents |= ESetOptionsCall;
	
	delete iRequestedGpsOptions;
	
	switch(aGpsOptions.ClassType())
		{
		case ELbsGpsOptionsArrayClass:
			{
			TLbsGpsOptionsItem opsItem;
			const TLbsGpsOptionsArray& optionsArray = static_cast<const TLbsGpsOptionsArray&>(aGpsOptions);
			iRequestedGpsOptions = new TLbsGpsOptionsArray();
			ASSERT(NULL != iRequestedGpsOptions);
			iRequestedGpsOptions->SetGpsMode(optionsArray.GpsMode());
			for (TUint i= 0; i<optionsArray.NumOptionItems(); i++)
				{
				optionsArray.GetOptionItem(i, opsItem);
				__ASSERT_ALWAYS(KErrNone == (static_cast<TLbsGpsOptionsArray*>(iRequestedGpsOptions))->AppendOptionItem(opsItem), User::Invariant());
				}
			}
		break;
		
		case ELbsGpsOptionsClass:
			{
			iRequestedGpsOptions = new TLbsGpsOptions();
			ASSERT(NULL != iRequestedGpsOptions);
			*iRequestedGpsOptions = aGpsOptions;
			}
		break;
		
		default:
			ASSERT(EFalse);
		break;
		}
	
	SignalObserverCallBack(ECallBackErrorNone);
	}

// Callback from test GPS module to let the test know that the 
// AGPS Manager has reported availability of assistance data.
void CTestGpsModuleObserver::AssistanceDataEvent(TInt aError, TLbsAsistanceDataGroup aDataMask)
	{
	iObservedGpsEvents |= EAssistanceDataCall;
	
	iAssistanceDataEventError = aError;
	iAssistanceDataMask = aDataMask;		
	
	SignalObserverCallBack(ECallBackErrorNone);
	}

// Method called by the test to check if the AGPS Manager has issued
// a location update request to the GPS module. Calling this method decreases
// the callback count in this object. When ETrue is returned, the reported
// values of aTargetTime and aQuality are also returned.
TBool CTestGpsModuleObserver::WasLocationUpdateRequested(TTime& aTargetTime, TLbsLocRequestQualityInt& aQuality)
	{
	TBool ret = EFalse;
	if (CheckCallbackObserved(ELocationUpdateReqCall))
		{
		aTargetTime = iRequestedTargetTime;
		aQuality = iRequestedQuality;
		ret = ETrue;
		}
	return ret;
	}

// Method called by the test to check if the AGPS Manager has cancelled
// a location update request with the GPS module. Calling this method decreases
// the callback count in this object
TBool CTestGpsModuleObserver::WasLocationUpdateCancelled()
	{
	TBool ret = EFalse;
	if (CheckCallbackObserved(ECancelLocationCall))
		{
		ret = ETrue;
		}
	return ret;		
	}
	
// Method called by the test to check if the AGPS Manager has issued
// a power mode advice to the GPS module. Calling this method decreases
// the callback count in this object. When ETrue is returned, the reported
// value of aPowerMode is also returned.
TBool CTestGpsModuleObserver::WasPowerModeAdvised(CLbsLocationSourceGpsBase::TPowerMode& aPowerMode)
	{
	TBool ret = EFalse;
	if (CheckCallbackObserved(EAdvisePowerModeCall))
		{
		aPowerMode = iAdvisedPowerMode;
		ret = ETrue;
		}
	return ret;		
	}

// Method called by the test to check if the AGPS Manager has sent
// new GPS options to the GPS module. Calling this method decreases
// the callback count in this object. When ETrue is returned, the 
// new GPS option are also returned in aGpsOptions.
TBool CTestGpsModuleObserver::WereGpsOptionsSet(TLbsGpsOptions& aGpsOptions)
	{
	TBool ret = EFalse;
	if (CheckCallbackObserved(ESetOptionsCall))
		{
		// Return EFalse if the test tries to pass in a class type different from the one
		// sent from the Manager in SetGpsOptions()
		if ((NULL == iRequestedGpsOptions) ||
			(aGpsOptions.ClassType() != iRequestedGpsOptions->ClassType()))
			{
			ret = EFalse;
			}
		else
			{
			// Panic if no options are available
			ASSERT(NULL != iRequestedGpsOptions);

			switch (aGpsOptions.ClassType())
				{
				case ELbsGpsOptionsArrayClass:
					{
					TLbsGpsOptionsItem opsItem;
					TLbsGpsOptionsArray& optionsArray = static_cast<TLbsGpsOptionsArray&>(aGpsOptions);
					optionsArray.SetGpsMode(iRequestedGpsOptions->GpsMode());
					for (TUint i= 0; i<(static_cast<TLbsGpsOptionsArray*>(iRequestedGpsOptions))->NumOptionItems(); i++)
						{
						__ASSERT_ALWAYS(KErrNone == (static_cast<TLbsGpsOptionsArray*>(iRequestedGpsOptions))->GetOptionItem(i, opsItem), User::Invariant());
						__ASSERT_ALWAYS(KErrNone == optionsArray.AppendOptionItem(opsItem), User::Invariant());
						}
					}
				break;
		
				case ELbsGpsOptionsClass:
					aGpsOptions = *iRequestedGpsOptions;
				break;
		
				default:
					ASSERT(EFalse);
				break;
				}
			ret = ETrue;
			}	
		}
	return ret;		
	}
	
// Method called by the test to check if the AGPS Manager has reported
// availability of new assistance data to the GPS module. Calling this
// method decreases the callback count in this object.
// When ETrue is returned, the new GPS option are also returned in
// aGpsOptions.
TBool CTestGpsModuleObserver::WereAssistanceDataReported(TInt& aError, TLbsAsistanceDataGroup& aDataMask)
	{
	TBool ret = EFalse;
	if (CheckCallbackObserved(EAssistanceDataCall))
		{
		aError = iAssistanceDataEventError;
		aDataMask =	iAssistanceDataMask;
		ret = ETrue;
		}
	return ret;		
	}

// Returns true if aCall is among the callbacks received. If it was
// then aCall is removed from the list of received  callbacks.
TBool CTestGpsModuleObserver::CheckCallbackObserved(const TObservedGpsModuleCalls aCall)
	{
	TBool ret = EFalse;
	if (iObservedGpsEvents & aCall)
		{	
		// Clear this call's flag from the list of received calls
	 	iObservedGpsEvents = iObservedGpsEvents & ~aCall;
	 	CallbackHandled();
	 	ret = ETrue;
		}
	return ret;
	}
	
	
// Getters for the values observed in calls from AGPS Manager to GPS module
// ========== MAY NOT BE NEEDED, COMMENT OUT FOR THE TIME BEING ============
/*
const TTime& CTestGpsModuleObserver::TargetTime()
	{
		
	}

const TLbsLocRequestQuality& CTestGpsModuleObserver::RequestQuality()
	{
		
	}

	
const TPowerMode& CTestGpsModuleObserver::PowerMode()
	{
		
	}

const TLbsGpsOptions& CTestGpsModuleObserver::GpsOptions()
	{
		
	}

	
TInt CTestGpsModuleObserver::AssistanceDataEventError()
	{
		
	}

TLbsAsistanceDataGroup CTestGpsModuleObserver::AssistanceDataMask()
	{
		
	}
*/

