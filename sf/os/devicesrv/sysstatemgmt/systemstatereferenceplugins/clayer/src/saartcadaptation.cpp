// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "saartcadaptation.h"

#include "startupadaptationadapter.h"
#include "ssmdebug.h"
#include "clayerpanic.h"
/*
 * Creates a new object associated with the passed in CStartupAdaptationAdapter
 * 
 * @internalComponent
 */
CSaaRtcAdaptation* CSaaRtcAdaptation::NewL(CStartupAdaptationAdapter* aAdapter)
	{
	CSaaRtcAdaptation* self = new (ELeave) CSaaRtcAdaptation(aAdapter);
	return self;
	}

/*
 * Destructor for this object
 * 
 * @internalComponent
 */
CSaaRtcAdaptation::~CSaaRtcAdaptation()
	{

	}

/*
 * Decrements the reference count for this object, deleting it if necessary
 * 
 * @internalComponent
 */
void CSaaRtcAdaptation::Release()
	{
	// This MClass is owned by the singleton CStartupAdaptationAdapter class so
	// release should do nothing.
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaRtcAdaptation::ValidateRtc(TDes8& aValidityPckg, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstand requests so set up command id
	SetCommandId(StartupAdaptation::EValidateRTCTime);
	// No parameters to set to pass in
	
	// Remember the package to pass data back into
	iReturnDes = &aValidityPckg;
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaRtcAdaptation::SetWakeupAlarm(TDesC8& aAlarmTimePckg, TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstand requests so set up command id
	SetCommandId(StartupAdaptation::ESetWakeupAlarm);
	// Copy across the parameters into iWakeupAlarmPckg
	TPckg<TTime> timePckg(iWakeupAlarmPckg().iTime);
	timePckg.Copy(aAlarmTimePckg);
	iWakeupAlarmPckg().iUniversalTimeOffset = 0;
	iWakeupAlarmPckg().iDaylightSavingOffset = 0;
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}

	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaRtcAdaptation::UnsetWakeupAlarm(TRequestStatus& aStatus)
	{
	// If this adaptation is busy then complete with KErrInUse
	if(Busy())
		{
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, KErrInUse);
		return;
		}
	// Set this request status
	SetRequestStatus(&aStatus);
	aStatus = KRequestPending;
	// No outstand requests so set up command id
	SetCommandId(StartupAdaptation::ECancelWakeupAlarm);
	// No parameters to set to pass in
	
	// Pass this to the adapter
	TRAPD(err, iAdapter->QueueDispatchL(this));
	if(err != KErrNone)
		{
		// Failed to queue adaptation, complete with error
		SetRequestStatus(NULL);
		TRequestStatus* statusPtr = &aStatus;
		User::RequestComplete(statusPtr, err);
		}
	}

/*
 * 
 * 
 * @internalComponent
 */
void CSaaRtcAdaptation::Cancel()
	{
	CancelRequest();
	}
	
/*
 * Constructs a new state adaptation object and associates it with aAdapter
 * 
 * @internalComponent
 */
CSaaRtcAdaptation::CSaaRtcAdaptation(CStartupAdaptationAdapter* aAdapter)
: CAdaptationBase(aAdapter)
	{

	}

/**
 * See CAdaptationBase for description of method.
 *  
 * @internalComponent
 */
void CSaaRtcAdaptation::RequestComplete(const StartupAdaptation::TCommand __DEBUG_ONLY(aCommandId), TDesC8& aRetPckg)
	{
	DEBUGPRINT3A("SAA - Response received from adaptation with commandId: %d, expecting %d", aCommandId, CommandId());
	__ASSERT_DEBUG(aCommandId == CommandId(), CLAYER_PANIC(ECLayerUnexpectedCommandResponse));
	switch(CommandId())
		{
		case StartupAdaptation::EValidateRTCTime:
			{
			StartupAdaptation::TResponsePckg responsePckg;
			responsePckg.Copy(aRetPckg);
			// if responsePckg is KErrNone then the RTC is valid, otherwise it isn't valid, use this to set the value
			TPckgBuf<TBool> booleanPckg(responsePckg() == KErrNone);
			iReturnDes->Copy(booleanPckg);
			// Null iReturnDes as it has no further use
			iReturnDes = NULL;
			CompleteRequestStatus(KErrNone);
			break;
			}
		case StartupAdaptation::ESetWakeupAlarm:
			{
			StartupAdaptation::TResponsePckg responsePckg;
			responsePckg.Copy(aRetPckg);
			CompleteRequestStatus(responsePckg());
			break;
			}
		case StartupAdaptation::ECancelWakeupAlarm:
			// Always successful so nothing to check in aRetPckg
			CompleteRequestStatus(KErrNone);
			break;
		default:
			CLAYER_PANIC(ECLayerUnknownCommandResponse);
			break;
		}
	}

/**
 * See CAdaptationBase for description of method.
 *  
 * @internalComponent
 */
TDesC8* CSaaRtcAdaptation::ParameterPckg()
	{
	TDesC8* ptr = NULL;
	switch(CommandId())
		{
		case StartupAdaptation::EValidateRTCTime:
			ptr = &iNullBuf;
			break;
		case StartupAdaptation::ESetWakeupAlarm:
			ptr = &iWakeupAlarmPckg;
			break;
		case StartupAdaptation::ECancelWakeupAlarm:
			ptr = &iNullBuf;
			break;
		default:
			ptr = NULL;
			break;
		}
	return ptr;
	}
