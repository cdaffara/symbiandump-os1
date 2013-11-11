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
#include <s32mem.h>
#include <lbspositioninfo.h>
#include <lbssatellite.h>
#include "TestPositionerSubSes.h"
#include "TestSession.h"
#include "TestServer.h"
#include "Utility.h"

#include "TestDummyValues.h"
#include "TestDummyExtClasses.h"

CEposPositioner* CEposPositioner::NewL()
	{
	CEposPositioner* self = new (ELeave) CEposPositioner;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CEposPositioner::CEposPositioner()
	{
	}

void CEposPositioner::ConstructL()
	{
	iGetLastKnownPositionTimer = new (ELeave) CRequestTimer;
	iGetLastKnownPositionTimer->ConstructL();
	iNotifyPositionUpdateTimer = new (ELeave) CRequestTimer;
	iNotifyPositionUpdateTimer->ConstructL();
	}

CEposPositioner::~CEposPositioner()
	{
	iRequestors.ResetAndDestroy();
	delete iGetLastKnownPositionTimer;
	delete iNotifyPositionUpdateTimer;
	}

void CEposPositioner::ServiceL(const RMessage2& aMessage)
	{
	switch (aMessage.Function())
		{
	//
	// Standard requests
	case EPositionerSetSingleRequestor:
		SetRequestorL(aMessage);
		break;
	case EPositionerSetMultipleRequestors:
		SetMultipleRequestorsL(aMessage);
		break;
	case EPositionerSetUpdateOptions:
		SetUpdateOptionsL(aMessage);
		break;
	case EPositionerGetUpdateOptions:
		GetUpdateOptionsL(aMessage);
		break;
	case EPositionerGetLastKnownPosition:
		GetLastKnownPosition(aMessage);
		break;
	case EPositionerNotifyPositionUpdate:
		NotifyPositionUpdate(aMessage);
		break;

	//
	// Cancel requests
	case EPositionerGetLastKnownPositionCancel:
		GetLastKnownPositionCancel(aMessage);
		break;
	case EPositionerNotifyPositionUpdateCancel:
		NotifyPositionUpdateCancel(aMessage);
		break;

	default:
		aMessage.Complete(KErrNotSupported);
		break;
		}
	}

void CEposPositioner::TimerCompleted(const RMessage2& aMessage)
	{
	TInt err = KErrNone;
	switch (aMessage.Function())
		{
	case EPositionerGetLastKnownPosition:
		TRAP(err, GetLastKnownPositionCompleteL(aMessage))
		break;
	case EPositionerNotifyPositionUpdate:
		TRAP(err, NotifyPositionUpdateCompleteL(aMessage))
		break;
	default:
		ASSERT(EFalse);
		break;
		}
	if (err)
		aMessage.Complete(err);
	}

void CEposPositioner::SetRequestorL(const RMessage2& aMessage)
	{
	TPckgBuf<CRequestor::TRequestorType> requestorType;
	TPckgBuf<CRequestor::TRequestorFormat> requestorFormat;

	User::LeaveIfError(Utility::Read(aMessage, 0, requestorType));
	User::LeaveIfError(Utility::Read(aMessage, 1, requestorFormat));

	HBufC* data = HBufC::NewLC(aMessage.GetDesLength(2));
	TPtr dataPtr(data->Des());
	User::LeaveIfError(Utility::Read(aMessage, 2, dataPtr));

	TInt err = KErrNone;
	CRequestor* requestor = CRequestor::NewL(requestorType(), requestorFormat(), *data);
	iRequestors.ResetAndDestroy();
	err = iRequestors.Append(requestor);
	if (err != KErrNone)
		delete requestor;
	CleanupStack::PopAndDestroy(data);
	aMessage.Complete(err);
	}

void CEposPositioner::SetMultipleRequestorsL(const RMessage2& aMessage)
	{
	HBufC8* buf = HBufC8::NewLC(aMessage.GetDesLength(0));
	TPtr8 ptr(buf->Des());
	User::LeaveIfError(Utility::Read(aMessage, 0, ptr));

	RDesReadStream strm(*buf);

	// Must not modify iRequestors at all until the entire new set
	// of requestors has been successfully read from the client.
	RRequestorStack requestors;
	CleanupClosePushL(requestors);
	strm >> requestors;

	iRequestors.ResetAndDestroy();
	iRequestors = requestors;
	CleanupStack::Pop(&requestors);

	CleanupStack::PopAndDestroy(buf);
	aMessage.Complete(KErrNone);
	}

void CEposPositioner::SetUpdateOptionsL(const RMessage2& aMessage)
	{
	const TInt desLength = aMessage.GetDesLength(0);
	TUint8* buffer = new (ELeave) TUint8[desLength];
	CleanupStack::PushL(buffer);

	TPtr8 ptrToBuf(buffer, desLength);
	User::LeaveIfError(Utility::Read(aMessage, 0, ptrToBuf));

	TPositionUpdateOptionsBase* updateOptionsBase = 
		reinterpret_cast<TPositionUpdateOptionsBase*>(buffer);

	TInt err = KErrArgument;
	if(updateOptionsBase->PositionClassType()==iUpdateOptions.PositionClassType())
		{
		iUpdateOptions = *(static_cast<TPositionUpdateOptions*>(updateOptionsBase));
		err = KErrNone;
		}
	aMessage.Complete(err);
	CleanupStack::PopAndDestroy(buffer);
	}

void CEposPositioner::GetUpdateOptionsL(const RMessage2& aMessage)
	{
	const TInt desLength = aMessage.GetDesLength(0);
	TUint8* buffer = new (ELeave) TUint8[desLength];
	CleanupStack::PushL(buffer);

	TPtr8 ptrToBuf(buffer, desLength);
	User::LeaveIfError(Utility::Read(aMessage, 0, ptrToBuf));

	TPositionUpdateOptionsBase* updateOptionsBase = 
		reinterpret_cast<TPositionUpdateOptionsBase*>(buffer);

	TInt err = KErrArgument;
	if(updateOptionsBase->PositionClassType()==iUpdateOptions.PositionClassType())
		{
		err = Utility::Write(aMessage, 0,
			TPtrC8(reinterpret_cast<TUint8*>(&iUpdateOptions), sizeof(iUpdateOptions)));
		}
	aMessage.Complete(err);
	CleanupStack::PopAndDestroy(buffer);
	}

void CEposPositioner::GetLastKnownPosition(const RMessage2& aMessage)
	{
	iGetLastKnownPositionTimer->Request(aMessage, this);
	}

void CEposPositioner::GetLastKnownPositionCompleteL(const RMessage2& aMessage)
	{
	if (!AreRequestorsAcceptedL())
		{
		aMessage.Complete(KErrAccessDenied);
		return;
		}

	const TInt desLength = aMessage.GetDesLength(0);
	TUint8* buffer = new (ELeave) TUint8[desLength];
	CleanupStack::PushL(buffer);

	TPtr8 ptrToBuf(buffer, desLength);	
	User::LeaveIfError(Utility::Read(aMessage, 0, ptrToBuf));

	TPositionInfoBase* updateOptionsBase = 
		reinterpret_cast<TPositionInfoBase*>(buffer);

	TInt err = KErrArgument;
	if(updateOptionsBase->PositionClassType() == EPositionInfoClass)
		{
		// GetLastKnownPosition only accepts the EPositionClassInfo extension,
		// ie. TPositionInfo, and NOT its derivatives.

		TPckgBuf<TPositionInfo> positionInfo;

		TPosition lastKnownPosition;
		lastKnownPosition.SetCoordinate(
			DUMMY_LAST_KNOWN_LATITUDE,
			DUMMY_LAST_KNOWN_LONGITUDE,
			DUMMY_LAST_KNOWN_ALTITUDE);
		lastKnownPosition.SetAccuracy(
			DUMMY_LAST_KNOWN_HORIZONTAL_ACCURACY,
			DUMMY_LAST_KNOWN_VERTICAL_ACCURACY);
		lastKnownPosition.SetCurrentTime();
		positionInfo().SetPosition(lastKnownPosition);
		err = Utility::Write(aMessage, 0, positionInfo);
		}

	aMessage.Complete(err);
	CleanupStack::PopAndDestroy(buffer);
	}

void CEposPositioner::GetLastKnownPositionCancel(const RMessage2& aMessage)
/**
 @param aMessage is the cancel request message, not the original request message.
 */
	{
	iGetLastKnownPositionTimer->CancelMessage();
	aMessage.Complete(KErrNone);
	}

void CEposPositioner::NotifyPositionUpdate(const RMessage2& aMessage)
	{
	iNotifyPositionUpdateTimer->Request(aMessage, this);
	}

void CEposPositioner::NotifyPositionUpdateCompleteL(const RMessage2& aMessage)
	{
	if (!AreRequestorsAcceptedL())
		{
		aMessage.Complete(KErrAccessDenied);
		return;
		}

	TInt bufferSize = aMessage.GetDesLength(0);
	TUint8* buffer = new (ELeave) TUint8[bufferSize];
	CleanupStack::PushL(buffer);

	TPtr8 ptrToBuffer(buffer, bufferSize, bufferSize);
	User::LeaveIfError(Utility::Read(aMessage, 0, ptrToBuffer));

	TPositionClassTypeBase* infoBase = reinterpret_cast<TPositionClassTypeBase*>(buffer);

	if (infoBase->PositionClassType() & EPositionInfoClass)
		{
		TPositionInfo* genInfo = reinterpret_cast<TPositionInfo*>(buffer);
		TPosition position;

		position.SetCoordinate(DUMMY_NOTIFY_POS_UPDATE_LATITUDE, DUMMY_NOTIFY_POS_UPDATE_LONGITUDE, DUMMY_NOTIFY_POS_UPDATE_ALTITUDE);
		position.SetAccuracy(DUMMY_NOTIFY_POS_UPDATE_HORIZONTAL_ACCURACY, DUMMY_NOTIFY_POS_UPDATE_VERTICAL_ACCURACY);
		position.SetCurrentTime();
		genInfo->SetPosition(position);
		}

	if (infoBase->PositionClassType() & EPositionCourseInfoClass)
		{
		TPositionCourseInfo* genInfo = reinterpret_cast<TPositionCourseInfo*>(buffer);
		TCourse course;
		course.SetSpeed(DUMMY_NOTIFY_POS_UPDATE_SPEED);
		course.SetHeading(DUMMY_NOTIFY_POS_UPDATE_HEADING);
		course.SetSpeedAccuracy(DUMMY_NOTIFY_POS_UPDATE_SPEED_ACCURACY);
		course.SetHeadingAccuracy(DUMMY_NOTIFY_POS_UPDATE_HEADING_ACCURACY);
		genInfo->SetCourse(course);
		}

	if (infoBase->PositionClassType() & EPositionSatelliteInfoClass)
		{
		TPositionSatelliteInfo* genInfo = reinterpret_cast<TPositionSatelliteInfo*>(buffer);
		TSatelliteData satellite;

		satellite.SetSatelliteId(DUMMY_NOTIFY_POS_UPDATE_SATELLITE_ID1);
		satellite.SetAzimuth(DUMMY_NOTIFY_POS_UPDATE_AZIMUTH1);
		satellite.SetElevation(DUMMY_NOTIFY_POS_UPDATE_ELEVATION1);
		satellite.SetIsUsed(DUMMY_NOTIFY_POS_UPDATE_IS_USED1);
		satellite.SetSignalStrength(DUMMY_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH1);
		genInfo->AppendSatelliteData(satellite);

		satellite.SetSatelliteId(DUMMY_NOTIFY_POS_UPDATE_SATELLITE_ID2);
		satellite.SetAzimuth(DUMMY_NOTIFY_POS_UPDATE_AZIMUTH2);
		satellite.SetElevation(DUMMY_NOTIFY_POS_UPDATE_ELEVATION2);
		satellite.SetIsUsed(DUMMY_NOTIFY_POS_UPDATE_IS_USED2);
		satellite.SetSignalStrength(DUMMY_NOTIFY_POS_UPDATE_SIGNAL_STRENGTH2);
		genInfo->AppendSatelliteData(satellite);
		}

	if (infoBase->PositionClassType() & EPositionClassTestExtension)
		{
		TExtPosInfo* genInfo = reinterpret_cast<TExtPosInfo*>(buffer);

		genInfo->iGalaxy = DUMMY_NOTIFY_POS_UPDATE_EXT_GALAXY;
		genInfo->iSolarSystem = DUMMY_NOTIFY_POS_UPDATE_EXT_SOLARSYS;
		genInfo->iStarDate = DUMMY_NOTIFY_POS_UPDATE_EXT_STARDATE;
		}	

	User::LeaveIfError(Utility::Write(aMessage, 0, ptrToBuffer));

	aMessage.Complete(KErrNone);
	CleanupStack::PopAndDestroy(buffer);
	}

void CEposPositioner::NotifyPositionUpdateCancel(const RMessage2& aMessage)
/**
 @param aMessage is the cancel request message, not the original request message.
 */
	{
	iNotifyPositionUpdateTimer->CancelMessage();
	aMessage.Complete(KErrNone);
	}

//
// Privacy checking functions
//

TBool CEposPositioner::AreRequestorsAcceptedL()
	{
	TInt count = iRequestors.Count();
	if (count < 1)
		return EFalse;
	for (TInt i=0; i<count; i++)
		{
		CRequestor::TRequestorType requestorType = iRequestors[i]->RequestorType();
		CRequestor::TRequestorFormat requestorFormat = iRequestors[i]->RequestorFormat();
		const TDesC& requestorData = iRequestors[i]->RequestorData();

		if (   requestorData != DUMMY_REQUESTOR_NAME_ACCEPTED1
		    && requestorData != DUMMY_REQUESTOR_NAME_ACCEPTED2)
			return EFalse;
		if (   requestorType != DUMMY_REQUESTOR_TYPE_ACCEPTED1
		    && requestorType != DUMMY_REQUESTOR_TYPE_ACCEPTED2)
			return EFalse;
		if (   requestorFormat != DUMMY_REQUESTOR_FORMAT_ACCEPTED1
		    && requestorFormat != DUMMY_REQUESTOR_FORMAT_ACCEPTED2)
			return EFalse;
		}
	return ETrue;
	}
