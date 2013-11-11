// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#include <lbs/lbsselflocatelogevent.h>
#include <logfilterandeventconstants.hrh>

/** set the local application uid */
EXPORT_C void CLbsSelfLocateLogEvent::SetLocalApp(TUid aUid)
	{
	iLocalApp = aUid;
	}

/** get the location application uid */
EXPORT_C TUid CLbsSelfLocateLogEvent::LocalApp() const
	{
	return iLocalApp;
	}

/**
 Create self locate data logging event object
 @param aUid, TUid of the local application
 @param aPosInfo, TPositionInfoBase pointer of the TPositionInfo data
 @return CLbsSelfLocateLogEvent object
*/
EXPORT_C CLbsSelfLocateLogEvent* CLbsSelfLocateLogEvent::NewL(TUid aUid, TPositionInfoBase* aPosInfo)
	{
	CLbsSelfLocateLogEvent* self = new(ELeave) CLbsSelfLocateLogEvent;
	CleanupStack::PushL(self);
	self->ConstructL(aUid, aPosInfo);
	CleanupStack::Pop(self);		
	return self;
	}

/** construction function */
void CLbsSelfLocateLogEvent::ConstructL(TUid aUid, TPositionInfoBase* aPosInfo)
	{
	BaseConstructL();
	iLocalApp = aUid;
	iPosInfo = CopyPosInfoToEvent(aPosInfo);
	}
	
CLbsSelfLocateLogEvent::~CLbsSelfLocateLogEvent()
	{
	}

/** implementation of inherited externalization function */
void CLbsSelfLocateLogEvent::DoExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt32L(iLocalApp.iUid);
	WritePosInfo(aWriteStream, iPosInfo);
	}

/** implementation of inherited internalization function */
void CLbsSelfLocateLogEvent::DoInternalizeL(RReadStream& aReadStream)
	{
	iLocalApp = TUid::Uid(aReadStream.ReadInt32L());
	ReadPosInfo(aReadStream, iPosInfo);
	}

/** implementation of inherited update object function */
void CLbsSelfLocateLogEvent::DoUpdateL(const CLbsLogEvent& aSourceLogEvent)
	{
	__ASSERT_DEBUG(aSourceLogEvent.EventType() == TUid::Uid(KLogLbsSelfLocateEventType), User::Panic(KLbsLoggerPanic, KErrArgument));
	
	const CLbsSelfLocateLogEvent* event = (reinterpret_cast<const CLbsSelfLocateLogEvent*>(&aSourceLogEvent));
	iLocalApp = event->LocalApp();
	delete iPosInfo;
	iPosInfo = CopyPosInfoToEvent(event->PositionInfo());
	}

/** implementation of inherited copy object function */
void CLbsSelfLocateLogEvent::DoCopyL(CLbsLogEvent*& aTargetLogEvent) const
	{
	aTargetLogEvent = CLbsSelfLocateLogEvent::NewL(iLocalApp, iPosInfo);
	}

CLbsSelfLocateLogEvent::CLbsSelfLocateLogEvent()
	{
	SetEventType(TUid::Uid(KLogLbsSelfLocateEventType));
	iPosInfo = NULL;
	}

TInt CLbsSelfLocateLogEvent::DataSize() const
	{
	return sizeof(TUid) + ((iPosInfo == NULL) ? sizeof(TInt8) : iPosInfoWriteSize);
	}

