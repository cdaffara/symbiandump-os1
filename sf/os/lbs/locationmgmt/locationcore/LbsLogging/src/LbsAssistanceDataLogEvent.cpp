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

#include <lbs/lbsassistancedatalogevent.h>
#include <logfilterandeventconstants.hrh>

/**
 Set requested assistance data mask
*/
EXPORT_C void CLbsAssistanceDataLogEvent::SetRequestAssistanceDataType(const TUint32 aMask)
	{
	iRequestAssistanceDataType = aMask;
	}

/**
 Get requested assistance data mask
*/
EXPORT_C TUint32 CLbsAssistanceDataLogEvent::RequestAssistanceDataType() const
	{
	return iRequestAssistanceDataType;
	}
	
/**
 Set received assistance data mask
*/
EXPORT_C void CLbsAssistanceDataLogEvent::SetReceiveAssistanceDataType(const TUint32 aMask)
	{
	iReceiveAssistanceDataType = aMask;
	}

/**
 Get received assistance data mask
*/
EXPORT_C TUint32 CLbsAssistanceDataLogEvent::ReceiveAssistanceDataType() const
	{
	return iReceiveAssistanceDataType;
	}

/**
 Create assistance data logging event object
 @param aRequestType, requested assistance data type
 @param aReceiveType, received assistance data type
 @return CLbsAssistanceDataLogEvent object
*/
EXPORT_C CLbsAssistanceDataLogEvent* CLbsAssistanceDataLogEvent::NewL(const TUint32 aRequestType, const TUint32 aReceiveType)
	{
	CLbsAssistanceDataLogEvent* self = new(ELeave) CLbsAssistanceDataLogEvent;
	CleanupStack::PushL(self);
	self->ConstructL(aRequestType, aReceiveType);
	CleanupStack::Pop(self);
	return self;
	}

/** construction function */
void CLbsAssistanceDataLogEvent::ConstructL(const TUint32 aRequestType, const TUint32 aReceiveType)
	{
	BaseConstructL();
	iRequestAssistanceDataType = aRequestType;
	iReceiveAssistanceDataType = aReceiveType;
	}
	
CLbsAssistanceDataLogEvent::~CLbsAssistanceDataLogEvent()
	{
	}

/** implementation of inherited externalization function */
void CLbsAssistanceDataLogEvent::DoExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteUint32L(iRequestAssistanceDataType);
	aWriteStream.WriteUint32L(iReceiveAssistanceDataType);
	}

/** implementation of inherited internalization function */
void CLbsAssistanceDataLogEvent::DoInternalizeL(RReadStream& aReadStream)
	{
	iRequestAssistanceDataType = aReadStream.ReadUint32L();
	
	iReceiveAssistanceDataType = aReadStream.ReadUint32L();
	}

/** implementation of inherited update object function */
void CLbsAssistanceDataLogEvent::DoUpdateL(const CLbsLogEvent& aSourceLogEvent)
	{
	__ASSERT_DEBUG(aSourceLogEvent.EventType() == TUid::Uid(KLogLbsAssistanceDataEventType), User::Panic(KLbsLoggerPanic, KErrArgument));
	
	const CLbsAssistanceDataLogEvent* event = (reinterpret_cast<const CLbsAssistanceDataLogEvent*>(&aSourceLogEvent));
	iRequestAssistanceDataType = event->RequestAssistanceDataType();
	iReceiveAssistanceDataType = event->ReceiveAssistanceDataType();
	}

/** implementation of inherited copy object function */
void CLbsAssistanceDataLogEvent::DoCopyL(CLbsLogEvent*& aTargetLogEvent) const
	{
	aTargetLogEvent = CLbsAssistanceDataLogEvent::NewL(iRequestAssistanceDataType, iReceiveAssistanceDataType);
	}

CLbsAssistanceDataLogEvent::CLbsAssistanceDataLogEvent()
	{
	SetEventType(TUid::Uid(KLogLbsAssistanceDataEventType));
	}

TInt CLbsAssistanceDataLogEvent::DataSize() const
	{
	return (sizeof(TUint) * 2);
	}

