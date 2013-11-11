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
 @released
*/

#include <lbs/lbstransmitlocationlogevent.h>
#include <logfilterandeventconstants.hrh>

/** set 3rd party application uid*/
EXPORT_C void CLbsTransmitLocationLogEvent::SetX3PApp(TUid aUid)
	{
	iX3PApp = aUid;
	}

/** get 3rd party application uid*/
EXPORT_C TUid CLbsTransmitLocationLogEvent::X3PApp() const
	{
	return iX3PApp;
	}

/** set 3rd party destination descriptor */
EXPORT_C void CLbsTransmitLocationLogEvent::SetDestination(TDesC& aDestination)
	{
	iDestination = aDestination;
	}

/** get 3rd party destination descriptor*/
EXPORT_C TDesC& CLbsTransmitLocationLogEvent::Destination() const
	{
	return (TDesC&)iDestination;
	}

/**
 Create transmit locate data logging event object
 @param aUid, TUid of the local application
 @param aPosInfo, TPositionInfoBase pointer of the TPositionInfo data
 @param aDestination, descriptor of the trasmitted destination
 @return CLbsTransmitLocationLogEvent object
*/
EXPORT_C CLbsTransmitLocationLogEvent* CLbsTransmitLocationLogEvent::NewL(TUid aUid, TPositionInfoBase* aPosInfo, TDesC& aDestination)
	{
	CLbsTransmitLocationLogEvent* self = new(ELeave) CLbsTransmitLocationLogEvent;
	CleanupStack::PushL(self);
	self->ConstructL(aUid, aPosInfo, aDestination);
	CleanupStack::Pop(self);		
	return self;
	}

/** construction function */
void CLbsTransmitLocationLogEvent::ConstructL(TUid aUid, TPositionInfoBase* aPosInfo, TDesC& aDestination)
	{
	BaseConstructL();
	iX3PApp = aUid;
	iPosInfo = CopyPosInfoToEvent(aPosInfo);
	iDestination = aDestination;
	}
	
CLbsTransmitLocationLogEvent::~CLbsTransmitLocationLogEvent()
	{
	}

/** implementation of inherited externalization function */
void CLbsTransmitLocationLogEvent::DoExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt32L(iX3PApp.iUid);
	aWriteStream.WriteInt32L(iDestination.Length());
	aWriteStream.WriteL(iDestination.Ptr(), iDestination.Length());
	WritePosInfo(aWriteStream, iPosInfo);
	}

/** implementation of inherited internalization function */
void CLbsTransmitLocationLogEvent::DoInternalizeL(RReadStream& aReadStream)
	{
	iX3PApp = TUid::Uid(aReadStream.ReadInt32L());
	TInt32 len = aReadStream.ReadInt32L();
	aReadStream.ReadL(iDestination, len);
	ReadPosInfo(aReadStream, iPosInfo);
	}

/** implementation of inherited update object function */
void CLbsTransmitLocationLogEvent::DoUpdateL(const CLbsLogEvent& aSourceLogEvent)
	{
	__ASSERT_DEBUG(aSourceLogEvent.EventType() == TUid::Uid(KLogLbsTransmitLocationEventType), User::Panic(KLbsLoggerPanic, KErrArgument));
	
	const CLbsTransmitLocationLogEvent* event = (reinterpret_cast<const CLbsTransmitLocationLogEvent*>(&aSourceLogEvent));
	iX3PApp = event->X3PApp();
	iDestination = event->Destination();
	delete iPosInfo;
	iPosInfo = CopyPosInfoToEvent(event->PositionInfo());
	}

/** implementation of inherited copy object function */
void CLbsTransmitLocationLogEvent::DoCopyL(CLbsLogEvent*& aTargetLogEvent) const
	{
	aTargetLogEvent = CLbsTransmitLocationLogEvent::NewL(iX3PApp, iPosInfo, (TDesC&)iDestination);
	}

CLbsTransmitLocationLogEvent::CLbsTransmitLocationLogEvent()
	{
	SetEventType(TUid::Uid(KLogLbsTransmitLocationEventType));
	iPosInfo = NULL;
	}

TInt CLbsTransmitLocationLogEvent::DataSize() const
	{
	return sizeof(TUid) + sizeof(TInt32) + iDestination.Size() + ((iPosInfo == NULL) ? sizeof(TInt8) : iPosInfoWriteSize);
	}


