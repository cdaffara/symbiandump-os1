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

#include <lbs/lbsnetworklocatelogevent.h>
#include <logfilterandeventconstants.hrh>

/** Set local application TUid */
EXPORT_C void CLbsNetworkLocateLogEvent::SetLocalApp(TUid aUid)
	{
	iLocalApp = aUid;
	}

/** get local application TUid value */
EXPORT_C TUid CLbsNetworkLocateLogEvent::LocalApp() const
	{
	return iLocalApp;
	}

/**
 Create network locate data logging event object
 @param aUid, TUid of the local application
 @param aPosInfo, TPositionInfoBase pointer of the TPositionInfo data
 @return CLbsNetworkLocateLogEvent object
*/
EXPORT_C CLbsNetworkLocateLogEvent* CLbsNetworkLocateLogEvent::NewL(TUid aUid, TPositionInfoBase* aPosInfo)
	{
	CLbsNetworkLocateLogEvent* self = new(ELeave) CLbsNetworkLocateLogEvent;
	CleanupStack::PushL(self);
	self->ConstructL(aUid, aPosInfo);
	CleanupStack::Pop(self);	
	return self;
	}
	
void CLbsNetworkLocateLogEvent::ConstructL(TUid aUid, TPositionInfoBase* aPosInfo)
	{
	BaseConstructL();
	iLocalApp = aUid;
	iPosInfo = CopyPosInfoToEvent(aPosInfo);
	}

CLbsNetworkLocateLogEvent::~CLbsNetworkLocateLogEvent()
	{

	}

/** implementation of inherited externalization function */
void CLbsNetworkLocateLogEvent::DoExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt32L(iLocalApp.iUid);
	WritePosInfo(aWriteStream, iPosInfo);
	}

/** implementation of inherited internalization function */
void CLbsNetworkLocateLogEvent::DoInternalizeL(RReadStream& aReadStream)
	{
	iLocalApp = TUid::Uid(aReadStream.ReadInt32L());
	//pos info
	ReadPosInfo(aReadStream, iPosInfo);
	}

/** implementation of inherited update object function */
void CLbsNetworkLocateLogEvent::DoUpdateL(const CLbsLogEvent& aSourceLogEvent)
	{
	__ASSERT_DEBUG(aSourceLogEvent.EventType() == TUid::Uid(KLogLbsNetworkLocateEventType), User::Panic(KLbsLoggerPanic, KErrArgument));
	const CLbsNetworkLocateLogEvent* event = (reinterpret_cast<const CLbsNetworkLocateLogEvent*>(&aSourceLogEvent));
	iLocalApp = event->LocalApp();
	delete iPosInfo;
	iPosInfo = CopyPosInfoToEvent(event->PositionInfo());
	}

/** implementation of inherited copy object function */
void CLbsNetworkLocateLogEvent::DoCopyL(CLbsLogEvent*& aTargetLogEvent) const
	{
	aTargetLogEvent = CLbsNetworkLocateLogEvent::NewL(iLocalApp, iPosInfo);
	}

CLbsNetworkLocateLogEvent::CLbsNetworkLocateLogEvent()
	{
	SetEventType(TUid::Uid(KLogLbsNetworkLocateEventType));
	iPosInfo = NULL;
	}

TInt CLbsNetworkLocateLogEvent::DataSize() const
	{
	return sizeof(TUid) + ((iPosInfo == NULL) ? sizeof(TInt8) : iPosInfoWriteSize);
	}

