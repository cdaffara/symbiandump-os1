// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <lbspositioninfo.h>
#include <lbscriteria.h>
#include "TestSession.h"
#include "TestServer.h"
#include "TestPositionerSubSes.h"
#include "TestDummyValues.h"
#include "TestDummyExtClasses.h"
#include "Utility.h"


CEposServSession::CEposServSession()
:	CSession2()
	{
	__DECLARE_NAME(_S("CEposServSession"));
	}

void CEposServSession::CreateL()
//
// 2nd phase construct for sessions - called by the CServer framework
//
	{
	iObjectIx = CObjectIx::NewL();
	iContainerIx = CObjectConIx::NewL();
	iObjectCon = iContainerIx->CreateL();
	iNotifyModuleStatusEventTimer = new (ELeave) CRequestTimer;
	iNotifyModuleStatusEventTimer->ConstructL();
	Server().AddSession();
	}


CEposServSession::~CEposServSession()
	{
	if (iObjectIx)
		{
		TInt count = iObjectIx->Count();

		// Close any objects that the client did not close
		for(TInt i=0; i<count; i++)
			{
			CObject* theObj = (*iObjectIx)[i];
			if (theObj)
				{
				TInt handle = iObjectIx->At(theObj);
				iObjectIx->Remove(handle);
				}
			}
		delete iObjectIx;
		}

	if (iContainerIx)
		{
		iContainerIx->Remove(iObjectCon);
		delete iContainerIx;
		}

	delete iNotifyModuleStatusEventTimer;

	Server().DropSession();
	}

void CEposServSession::ServiceL(const RMessage2& aMessage)
	{
	// Only requests that leave when they are setting up are completed
	// here. Cannot complete all requests as a couple are asynchronous.
	TRAPD(error, DispatchL(aMessage))
	if (error)
		aMessage.Complete(error);
	}

void CEposServSession::DispatchL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
        {
	//
	// Open / Close subsession requests
	case EPositionerOpen:
		NewPositionerL(aMessage);
		break;
	case EPositionerOpenModuleId:
		NewPositionerFromModuleIdL(aMessage);
		break;
	case EPositionerOpenCriteria:
		NewPositionerFromCriteriaL(aMessage);
		break;
	case EPositionerClose:
		ClosePositionerL(aMessage);
		break;

	//
	// Standard requests
	case EPositionServerGetDefaultModuleId:
		GetDefaultModuleIdL(aMessage);
		break;
	case EPositionServerGetNumModules:
		GetNumModulesL(aMessage);
		break;
	case EPositionServerGetModuleInfoByIndex:
		GetModuleInfoByIndexL(aMessage);
		break;
	case EPositionServerGetModuleInfoById:
		GetModuleInfoByIdL(aMessage);
		break;
	case EPositionServerGetModuleStatus:
		GetModuleStatusL(aMessage);
		break;
	case EPositionServerNotifyModuleStatusEvent:
		NotifyModuleStatusEvent(aMessage);
		break;

	//
	// Cancel requests
	case EPositionServerNotifyModuleStatusEventCancel:
		NotifyModuleStatusEventCancel(aMessage);
		break;

	//
	// Requests for sub-sessions
	default:
		CEposPositioner* theObj = static_cast<CEposPositioner*>(iObjectIx->At(aMessage.Int3()));
		theObj->ServiceL(aMessage);
		return;
        }
	}

void CEposServSession::TimerCompleted(const RMessage2& aMessage)
	{
	ASSERT(aMessage.Function() == EPositionServerNotifyModuleStatusEvent);
	TRAPD(err, NotifyModuleStatusEventCompleteL(aMessage))
	if (err)
		aMessage.Complete(err);
	}

void CEposServSession::NewPositionerL(const RMessage2& aMessage)
	{
	CEposPositioner* newObject = CEposPositioner::NewL();
	CleanupClosePushL(*newObject);
	iObjectCon->AddL(newObject);
	TInt handle = iObjectIx->AddL(newObject);
	TPckgC<TInt> pH(handle);
	Utility::Write(aMessage, 3, pH);
	CleanupStack::Pop(newObject);
	aMessage.Complete(KErrNone);
	}

void CEposServSession::NewPositionerFromModuleIdL(const RMessage2& aMessage)
	{
	TPckgBuf<TPositionModuleId> id;
	User::LeaveIfError(Utility::Read(aMessage, 0, id));

	CEposPositioner* newObject = CEposPositioner::NewL();
	CleanupClosePushL(*newObject);
	iObjectCon->AddL(newObject);
	TInt handle = iObjectIx->AddL(newObject);
	TPckgC<TInt> pH(handle);
	Utility::Write(aMessage, 3, pH);
	CleanupStack::Pop(newObject);
	aMessage.Complete(KErrNone);
	}

void CEposServSession::NewPositionerFromCriteriaL(const RMessage2& aMessage)
	{
	TInt dataSize = aMessage.GetDesLength(0);
	TUint8* buffer = new (ELeave) TUint8[dataSize];
	CleanupStack::PushL(buffer);

	TPtr8 ptrToBuf(buffer, dataSize);
	User::LeaveIfError(Utility::Read(aMessage, 0, ptrToBuf));

	TPositionCriteriaBase* criteriaBase =
		reinterpret_cast<TPositionCriteriaBase*>(buffer);

	if(criteriaBase->PositionClassType()&EPositionCriteriaClass)
		{
		CEposPositioner* newObject = CEposPositioner::NewL();
		CleanupClosePushL(*newObject);
		iObjectCon->AddL(newObject);
		TInt handle = iObjectIx->AddL(newObject);
		TPckgC<TInt> pH(handle);
		Utility::Write(aMessage, 3, pH);
		CleanupStack::Pop(newObject);
		}

	CleanupStack::PopAndDestroy(buffer);

	aMessage.Complete(KErrNone);
	}

void CEposServSession::ClosePositionerL(const RMessage2& aMessage)
	{
	CObject* theObj = reinterpret_cast<CObject*>(iObjectIx->At(aMessage.Int3()));

	if (theObj == NULL)
		aMessage.Complete(KErrBadHandle);
	else
		{
		CObject* theObject = reinterpret_cast<CObject*>(theObj->Owner());
		iObjectIx->Remove(aMessage.Int3());
		if (theObject)
			theObject->Close();
		aMessage.Complete(KErrNone);
		}
	}

void CEposServSession::GetDefaultModuleIdL(const RMessage2& aMessage)
	{
	TPckgBuf<TPositionModuleId> moduleId;
	moduleId() = TUid::Uid(DUMMY_MODULE_ID_DEF);
	TInt err = Utility::Write(aMessage, 0, moduleId);
	aMessage.Complete(err);
	}

void CEposServSession::GetNumModulesL(const RMessage2& aMessage)
	{
	TPckgBuf<TUint> numModules;
	numModules() = DUMMY_NUM_MODULES;
	TInt err = Utility::Write(aMessage, 0, numModules);
	aMessage.Complete(err);
	}

void CEposServSession::GetModuleInfoByIndexL(const RMessage2& aMessage)
	{
	TPckgBuf<TInt> moduleIndex;
	User::LeaveIfError(Utility::Read(aMessage, 0, moduleIndex));

	TInt dataSize = aMessage.GetDesLength(1);
	TUint8* buffer = new (ELeave) TUint8[dataSize];
	CleanupStack::PushL(buffer);

	TPtr8 ptrToBuf(buffer, dataSize);
	User::LeaveIfError(Utility::Read(aMessage, 1, ptrToBuf));

	TPositionModuleInfoBase* moduleInfoBase = 
		reinterpret_cast<TPositionModuleInfoBase*>(buffer);

	TInt err = KErrArgument;

	if (moduleInfoBase->PositionClassType() & EPositionModuleInfoClass)
		{
		err = KErrNone;
		TPositionModuleInfo* moduleInfo = 
			reinterpret_cast<TPositionModuleInfo*>(buffer);
		err = SetDummyPositionModuleInfo(moduleIndex(), *moduleInfo);
		if (!err)
			User::LeaveIfError(Utility::Write(aMessage, 1, ptrToBuf));
		}

	aMessage.Complete(err);
	CleanupStack::PopAndDestroy(buffer);
	}

void CEposServSession::GetModuleInfoByIdL(const RMessage2& aMessage)
	{
	TPckgBuf<TPositionModuleId> moduleId;
	User::LeaveIfError(Utility::Read(aMessage, 0, moduleId));

	TInt dataSize = aMessage.GetDesLength(1);
	TUint8* buffer = new (ELeave) TUint8[dataSize];
	CleanupStack::PushL(buffer);

	TPtr8 ptrToBuf(buffer, dataSize);
	User::LeaveIfError(Utility::Read(aMessage, 1, ptrToBuf));

	TPositionModuleInfoBase* moduleInfoBase = 
		reinterpret_cast<TPositionModuleInfoBase*>(buffer);

	TInt err = KErrArgument;

	if (moduleInfoBase->PositionClassType() & EPositionModuleInfoClass)
		{
		err = KErrNone;
		TPositionModuleInfo* moduleInfo = 
			reinterpret_cast<TPositionModuleInfo*>(buffer);

		switch (moduleId().iUid)
			{
		case DUMMY_MODULE_ID1:
			err = SetDummyPositionModuleInfo(0, *moduleInfo);
			break;
		case DUMMY_MODULE_ID2:
			err = SetDummyPositionModuleInfo(1, *moduleInfo);
			break;
		case DUMMY_MODULE_ID3:
			err = SetDummyPositionModuleInfo(2, *moduleInfo);
			break;
		default:
			err = KErrNotFound;
			}

		if (!err)
			User::LeaveIfError(Utility::Write(aMessage, 1, ptrToBuf));
		}

	aMessage.Complete(err);
	CleanupStack::PopAndDestroy(buffer);
	}

void CEposServSession::GetModuleStatusL(const RMessage2& aMessage)
	{
	TPckgBuf<TPositionModuleId> moduleId;
	User::LeaveIfError(Utility::Read(aMessage, 1, moduleId));

	TInt dataSize = aMessage.GetDesLength(0);
	TUint8* buffer = new (ELeave) TUint8[dataSize];
	CleanupStack::PushL(buffer);
	
	TPtr8 ptrToBuf(buffer, dataSize);
	User::LeaveIfError(Utility::Read(aMessage, 0, ptrToBuf));

	TPositionModuleStatusBase* moduleStatusBase = 
		reinterpret_cast<TPositionModuleStatusBase*>(buffer);

	TInt err = KErrArgument;	//Complete with KErrArgument if we don't recognise data type.

	if(moduleStatusBase->PositionClassType()&EPositionModuleStatusClass)
		{
		err = KErrNone;
		TPositionModuleStatus* moduleStatus = 
			reinterpret_cast<TPositionModuleStatus*>(buffer);

		switch (moduleId().iUid)
			{
		case DUMMY_MODULE_ID1:
			moduleStatus->SetDeviceStatus(DUMMY_MODULE_STATUS1);
			moduleStatus->SetDataQualityStatus(DUMMY_MODULE_DATA_QUAL_STATUS1);
			break;
		case DUMMY_MODULE_ID2:
			moduleStatus->SetDeviceStatus(DUMMY_MODULE_STATUS2);
			moduleStatus->SetDataQualityStatus(DUMMY_MODULE_DATA_QUAL_STATUS2);
			break;
		case DUMMY_MODULE_ID3:
			moduleStatus->SetDeviceStatus(DUMMY_MODULE_STATUS3);
			moduleStatus->SetDataQualityStatus(DUMMY_MODULE_DATA_QUAL_STATUS3);
			break;
		default:
			err = KErrNotFound;
			}
		}

	//Extend for more data types here.

	if(err==KErrNone)
		err = Utility::Write(aMessage, 0, ptrToBuf);
	aMessage.Complete(err);
	CleanupStack::PopAndDestroy(buffer);
	}

void CEposServSession::NotifyModuleStatusEvent(const RMessage2& aMessage)
	{
	iNotifyModuleStatusEventTimer->Request(aMessage, this);
	}

void CEposServSession::NotifyModuleStatusEventCompleteL(const RMessage2& aMessage)
	{
	TPckgBuf<TPositionModuleId> moduleId;
	User::LeaveIfError(Utility::Read(aMessage, 1, moduleId));

	// Note: API document says moduleId could be zero to indicate all modules
	if (   moduleId().iUid != DUMMY_MODULE_ID1
	    && moduleId().iUid != DUMMY_MODULE_ID2
	    && moduleId().iUid != DUMMY_MODULE_ID3)
		{
		aMessage.Complete(KErrNotFound);
		return;
		}

	// Note: in a real implementation, would need to check the requested events mask

	TInt dataSize = aMessage.GetDesLength(0);
	TUint8* buffer = new (ELeave) TUint8[dataSize];
	CleanupStack::PushL(buffer);

	TPtr8 ptrToBuf(buffer, dataSize);
	User::LeaveIfError(Utility::Read(aMessage, 0, ptrToBuf));

	TPositionModuleStatusEventBase* moduleStatusEventBase = 
		reinterpret_cast<TPositionModuleStatusEventBase*>(buffer);

	TInt err = KErrArgument;	//Complete with KErrArgument if we don't recognise data type.

	if(moduleStatusEventBase->PositionClassType()&EModuleStatusEventTestExtension)
		{
		//Do nothing to the object. We need to test that the reset worked client side.
		//See test NotifyModuleStatusEvent_DerivedEventClass
		}

	else if(moduleStatusEventBase->PositionClassType()&EPositionModuleStatusEventClass)
		{
		err = KErrNone;
		TPositionModuleStatusEvent* moduleStatusEvent = 
			reinterpret_cast<TPositionModuleStatusEvent*>(buffer);

		TPositionModuleStatus moduleStatus;
		moduleStatus.SetDeviceStatus(DUMMY_STATUS_EVENT_DEV_STATUS);
		moduleStatus.SetDataQualityStatus(DUMMY_STATUS_EVENT_DEV_QUAL_STATUS);
		moduleStatusEvent->SetModuleId(moduleId());
		moduleStatusEvent->SetModuleStatus(moduleStatus);
		moduleStatusEvent->SetOccurredEvents(DUMMY_STATUS_EVENT_OCC_EVENTS);
		}

	if(err == KErrNone)
		err = Utility::Write(aMessage, 0, ptrToBuf);

	aMessage.Complete(err);
	CleanupStack::PopAndDestroy(buffer);
	}

void CEposServSession::NotifyModuleStatusEventCancel(const RMessage2& aMessage)
/**
 @param aMessage is the cancel request message, not the original request message.
 */
	{
	iNotifyModuleStatusEventTimer->CancelMessage();
	aMessage.Complete(KErrNone);
	}

//
// Helpers
//
TInt CEposServSession::SetDummyPositionModuleInfo(TInt aNum, TPositionModuleInfo& aPositionModuleInfo)
	{
	TPositionQuality posQuality;
	switch (aNum)
		{
	case 0:
		aPositionModuleInfo.SetModuleId(TUid::Uid(DUMMY_MODULE_ID1));
		aPositionModuleInfo.SetIsAvailable(DUMMY_MODULE_INFO_IS_AVAILABLE1);
		aPositionModuleInfo.SetTechnologyType(DUMMY_MODULE_INFO_TECHNOLOGY_TYPE1);
		aPositionModuleInfo.SetDeviceLocation(DUMMY_MODULE_INFO_DEVICE_LOCATION1);
		aPositionModuleInfo.SetClassesSupported(EPositionInfoFamily, DUMMY_MODULE_INFO_CLASSES_SUPPORTED1);
		aPositionModuleInfo.SetCapabilities(DUMMY_MODULE_INFO_CAPABILITIES1);
		aPositionModuleInfo.SetVersion(DUMMY_MODULE_INFO_VERSION1);
		posQuality.SetTimeToFirstFix(DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX1);
		posQuality.SetTimeToNextFix(DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX1);
		posQuality.SetHorizontalAccuracy(DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY1);
		posQuality.SetVerticalAccuracy(DUMMY_MODULE_INFO_VERTICAL_ACCURACY1);
		posQuality.SetCostIndicator(DUMMY_MODULE_INFO_COST_INDICATOR1);
		posQuality.SetPowerConsumption(DUMMY_MODULE_INFO_POWER_CONSUMPTION1);
		aPositionModuleInfo.SetPositionQuality(posQuality);
		aPositionModuleInfo.SetModuleName(DUMMY_MODULE_INFO_MOD_NAME1);
		break;
	case 1:
		aPositionModuleInfo.SetModuleId(TUid::Uid(DUMMY_MODULE_ID2));
		aPositionModuleInfo.SetIsAvailable(DUMMY_MODULE_INFO_IS_AVAILABLE2);
		aPositionModuleInfo.SetTechnologyType(DUMMY_MODULE_INFO_TECHNOLOGY_TYPE2);
		aPositionModuleInfo.SetDeviceLocation(DUMMY_MODULE_INFO_DEVICE_LOCATION2);
		aPositionModuleInfo.SetCapabilities(DUMMY_MODULE_INFO_CAPABILITIES2);
		aPositionModuleInfo.SetClassesSupported(EPositionInfoFamily, DUMMY_MODULE_INFO_CLASSES_SUPPORTED2);
		aPositionModuleInfo.SetVersion(DUMMY_MODULE_INFO_VERSION2);
		posQuality.SetTimeToFirstFix(DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX2);
		posQuality.SetTimeToNextFix(DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX2);
		posQuality.SetHorizontalAccuracy(DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY2);
		posQuality.SetVerticalAccuracy(DUMMY_MODULE_INFO_VERTICAL_ACCURACY2);
		posQuality.SetCostIndicator(DUMMY_MODULE_INFO_COST_INDICATOR2);
		posQuality.SetPowerConsumption(DUMMY_MODULE_INFO_POWER_CONSUMPTION2);
		aPositionModuleInfo.SetPositionQuality(posQuality);
		aPositionModuleInfo.SetModuleName(DUMMY_MODULE_INFO_MOD_NAME2);
		break;
	case 2:
		aPositionModuleInfo.SetModuleId(TUid::Uid(DUMMY_MODULE_ID3));
		aPositionModuleInfo.SetIsAvailable(DUMMY_MODULE_INFO_IS_AVAILABLE3);
		aPositionModuleInfo.SetTechnologyType(DUMMY_MODULE_INFO_TECHNOLOGY_TYPE3);
		aPositionModuleInfo.SetDeviceLocation(DUMMY_MODULE_INFO_DEVICE_LOCATION3);
		aPositionModuleInfo.SetCapabilities(DUMMY_MODULE_INFO_CAPABILITIES3);
		aPositionModuleInfo.SetClassesSupported(EPositionInfoFamily, DUMMY_MODULE_INFO_CLASSES_SUPPORTED3);
		aPositionModuleInfo.SetVersion(DUMMY_MODULE_INFO_VERSION3);
		posQuality.SetTimeToFirstFix(DUMMY_MODULE_INFO_TIME_TO_FIRST_FIX3);
		posQuality.SetTimeToNextFix(DUMMY_MODULE_INFO_TIME_TO_NEXT_FIX3);
		posQuality.SetHorizontalAccuracy(DUMMY_MODULE_INFO_HORIZONTAL_ACCURACY3);
		posQuality.SetVerticalAccuracy(DUMMY_MODULE_INFO_VERTICAL_ACCURACY3);
		posQuality.SetCostIndicator(DUMMY_MODULE_INFO_COST_INDICATOR3);
		posQuality.SetPowerConsumption(DUMMY_MODULE_INFO_POWER_CONSUMPTION3);
		aPositionModuleInfo.SetPositionQuality(posQuality);
		aPositionModuleInfo.SetModuleName(DUMMY_MODULE_INFO_MOD_NAME3);
		break;
	default:
		return KErrNotFound;
		}
	return KErrNone;
	}
