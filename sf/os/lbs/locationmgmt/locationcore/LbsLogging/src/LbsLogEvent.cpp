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

#include <lbs/lbslogevent.h>
#include <lbs/lbsselflocatelogevent.h>
#include <lbs/lbsexternallocatelogevent.h>
#include <lbs/lbstransmitlocationlogevent.h>
#include <lbs/lbsnetworklocatelogevent.h>
#include <lbs/lbsassistancedatalogevent.h>
#include <s32mem.h>
#include <logfilterandeventconstants.hrh>

EXPORT_C CLbsLogEvent* CLbsLogEvent::NewL(const CLogEvent& aLogEvent)
/**
 This method should be used to retrieve an existing logged event from Log Engine,
 CLogEvent parameter should be a valid logging event object. This function will
 internalise this logged event into the approprite LBS Location Requestion Logging class.
 
 This method should NOT be used within LBS Sub-system, if otherwise, 
 it should be trapped.
 
 @param aLogEvent The event that was logged

 @return A pointer to a new instance of the class
*/
	{
	if(&aLogEvent == NULL)
		{
		User::Leave(KErrArgument);
		}
		
	CLbsLogEvent* self = NULL;
	if(aLogEvent.EventType() == TUid::Uid(KLogLbsSelfLocateEventType))
		{
		self = CLbsSelfLocateLogEvent::NewL(TUid::Uid(0), NULL);
		}
	else if(aLogEvent.EventType() == TUid::Uid(KLogLbsExternalLocateEventType))
		{
		self = CLbsExternalLocateLogEvent::NewL(TUid::Uid(0), NULL, NULL);
		}
	else if(aLogEvent.EventType() == TUid::Uid(KLogLbsTransmitLocationEventType))
		{
		TBuf<2> dest(_L(""));
		self = CLbsTransmitLocationLogEvent::NewL(TUid::Uid(0), NULL, dest);
		}
	else if(aLogEvent.EventType() == TUid::Uid(KLogLbsNetworkLocateEventType))
		{
		self = CLbsNetworkLocateLogEvent::NewL(TUid::Uid(0), NULL);
		}
	else if(aLogEvent.EventType() == TUid::Uid(KLogLbsAssistanceDataEventType))
		{
		self = CLbsAssistanceDataLogEvent::NewL(0, 0);
		}
	else
		{
		User::Leave(KErrUnknown);
		}
	self->InternalizeL(aLogEvent);
	return self;
	}

void CLbsLogEvent::UpdateRootEvent(CLogEvent* aTargetEvent)
/**
 Update the CLogEvent object by the property within this CLbsLogEvent
 @param aTargetEvent The class to be updated
*/
	{
	TBuf<32> buf;
	if(iDirection)
		{
		aTargetEvent->SetDirection(KLbsLogMOLR);
		}
	else
		{
		aTargetEvent->SetDirection(KLbsLogMTLR);
		}
	if(iCostInformationAvailable)
		{
		buf.Num(iCostInformation);
		aTargetEvent->SetNumber(buf);
		buf.Zero();
		}
	TInt requestOutcome(static_cast<TInt>(iRequestOutcome));
	buf.Num(requestOutcome);
	aTargetEvent->SetStatus(buf);
	buf.Zero();
	
	HBufC8* buffer = HBufC8::NewLC(DataSize());
	RDesWriteStream stream;
	TPtr8 pBuffer(buffer->Des());
	stream.Open(pBuffer);
	CleanupClosePushL(stream);
	
	DoExternalizeL(stream);
	stream.CommitL();
	TInt ret = 0;
	TRAP(ret, aTargetEvent->SetDataL(buffer->Des()));
	
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buffer);	
	}
	
void CLbsLogEvent::InternalizeL(const CLogEvent& aLogEvent)
/**
 Internalises into this class from CLogEvent
 
 @param aLogEvent The class to internalise from
*/
	{
	RDesReadStream stream(aLogEvent.Data());
	CleanupClosePushL(stream);
	
	if(aLogEvent.Direction().Compare(KLbsLogMOLR))
		{
		iDirection = ETrue;
		}
	if(aLogEvent.Direction().Compare(KLbsLogMTLR))
		{
		iDirection = EFalse;
		}
	TLex tex(aLogEvent.Number());
	tex.Val(iCostInformation);
	if(iCostInformation != 0)
		iCostInformationAvailable = ETrue;
	
	tex.Assign(aLogEvent.Status());
	TInt requestOutcome;
	tex.Val(requestOutcome);
	iRequestOutcome = static_cast<TLbsRequestOutcome>(requestOutcome);
	DoInternalizeL(stream);
	CleanupStack::PopAndDestroy(&stream);
	}

void CLbsLogEvent::BaseConstructL()
/**
 The 2nd phase constructor for the base class, gets called from derived classes
 This function will produce an unique queue ID within the queue for each event, it will be reset once the
 log engine is reset.
*/
	{
	}

EXPORT_C CLogEvent* CLbsLogEvent::ExternalizeL() const
/**
 Externalises this object into a CLogEvent object so 
 it can be passed on to the LogEng.
 
 @return A pointer to a newly created CLogEvent instance 
 which can be passed to LogEng
*/
	{
	CLogEvent* event = CLogEvent::NewL();
	CleanupStack::PushL(event);
	event->SetEventType(iEventType);
	TBuf<32> buf;
	if(iDirection)
		{
		event->SetDirection(KLbsLogMOLR);
		}
	else
		{
		event->SetDirection(KLbsLogMTLR);
		}
	if(iCostInformationAvailable)
		{
		buf.Num(iCostInformation);
		event->SetNumber(buf);
		buf.Zero();
		}
	TInt requestOutcome(static_cast<TInt>(iRequestOutcome));
	buf.Num(requestOutcome);
	event->SetStatus(buf);
	buf.Zero();
	
	HBufC8* buffer = HBufC8::NewLC(DataSize());
	RDesWriteStream stream;
	TPtr8 pBuffer(buffer->Des());
	stream.Open(pBuffer);
	CleanupClosePushL(stream);
	
	DoExternalizeL(stream);
	stream.CommitL();
	TInt ret = 0;
	TRAP(ret, event->SetDataL(buffer->Des()));
	
	CleanupStack::PopAndDestroy(&stream);
	CleanupStack::PopAndDestroy(buffer);	
	CleanupStack::Pop(event);
	return event;
	}

void CLbsLogEvent::UpdateL(const CLbsLogEvent& aLogEvent)
/**
 Updates this class with information provided
 
 @param aLogEvent The information that was provided
 
 @leave KErrArgument If what is passed in has a different type than type of 
 			this instance
*/
	{
	if(aLogEvent.EventType() != EventType())
		{
		User::Leave(KErrArgument);
		}
	iRequestOutcome = aLogEvent.RequestOutcome();
	iDirection = aLogEvent.Direction();
	iCostInformationAvailable = aLogEvent.IsCostInformationAvailable();
	iCostInformation = aLogEvent.CostInformation();
	DoUpdateL(aLogEvent);
	}

CLbsLogEvent* CLbsLogEvent::CopyL() const
/**
 Creates a new instance of CLbsLogEvent which will the a copy of the current one
 
 @return A pointer to the newly created instance
*/
	{
	CLbsLogEvent* newEvent = NULL;
	// coverity[leave_without_push]
	// Heap allocation happens in DoCopyL, which leaves if it fails. No leaks occur.
	DoCopyL(newEvent);
	newEvent->SetRequestOutcome(iRequestOutcome);
	newEvent->SetCostInformation(iCostInformation);
	newEvent->SetDirection(iDirection);
	return newEvent;
	}

TPositionInfo* CLbsLogEvent::CopyPosInfoToEvent(const TPositionInfoBase* aSrcosInfo)
/**
 Copy PositionInfo class for different supported types
 @param aSrcosInfo provided source TPositionInfoBase* pointer
 @return TPositionInfoBase* pointer to the copied position class
*/
	{
	TPositionInfo* posInfo = NULL;
	if (aSrcosInfo)
		{
		posInfo = new TPositionInfo();
		if(posInfo == NULL)
			{
			// If allocation fails we cannot do anything else
			return posInfo;
			}
		TPositionInfoBase* posInfoBase = static_cast<TPositionInfoBase*>(posInfo);

		TUint8* baseToAddr = reinterpret_cast<TUint8*>(posInfoBase)+sizeof(TPositionClassTypeBase);
		const TUint8* const baseFromAddr = reinterpret_cast<const TUint8*>(aSrcosInfo)+sizeof(TPositionClassTypeBase);
		TUint32 dataLength = sizeof(TPositionInfo) - sizeof(TPositionClassTypeBase); 
		Mem::Copy(baseToAddr, baseFromAddr, dataLength);
		}
	return posInfo;
	}

/**
 Copy PositionInfo class for different supported types
 @param aPosInfo provided source TPositionInfoBase* pointer
*/
EXPORT_C void CLbsLogEvent::SetPositionInfo(TPositionInfoBase* aPosInfo)
	{
	if(aPosInfo != NULL)
		{
		if(iPosInfo != NULL)
			{
			delete iPosInfo;
			iPosInfo = NULL;
			}
		iPosInfo = CopyPosInfoToEvent(aPosInfo);
		}	
	}
	
CLbsLogEvent::CLbsLogEvent() :
	iPosInfoWriteSize(sizeof(TInt8) + sizeof(TInt32) + 
					  (sizeof(TUint32) * 2) + sizeof(TInt64) + 
					  (sizeof(TReal32) * 3) + (sizeof(TReal64) * 2))
/**
 C++ constructor
*/
	{
	iRequestOutcome = ELbsRequestOutcomeInconclusive;
	iCostInformationAvailable = EFalse;
	iDirection = ETrue;
	iPosInfo = NULL;
	}

CLbsLogEvent::~CLbsLogEvent()
/**
 C++ destructor
*/
	{
	if(iPosInfo != NULL)
		{
		delete iPosInfo;
		}
	}
	
/** set the direction for logging event (incoming or outgoing calls) */
EXPORT_C void CLbsLogEvent::SetDirection(TBool aDirection)
	{
	iDirection = aDirection;
	}

/** get the direction for logging event (incoming or outgoing calls) */
EXPORT_C TBool CLbsLogEvent::Direction() const
	{
	return iDirection;
	}
	
/** set the logging event type */
EXPORT_C void CLbsLogEvent::SetEventType(TUid aEventType)
	{
	iEventType = aEventType;
	}

/** get the logging event type */
EXPORT_C TUid CLbsLogEvent::EventType() const
	{
	return iEventType;
	}

/** set the lbs service request outcome */
EXPORT_C void CLbsLogEvent::SetRequestOutcome(TLbsRequestOutcome aRequestOutcome)
	{
	iRequestOutcome = aRequestOutcome;
	}

/** get the lbs service request outcome */
EXPORT_C TLbsRequestOutcome CLbsLogEvent::RequestOutcome() const
	{
	return iRequestOutcome;
	}

/** set the lbs service cost flag */
EXPORT_C TBool CLbsLogEvent::IsCostInformationAvailable() const
	{
	return iCostInformationAvailable;
	}

/** set the lbs service cost info */
EXPORT_C void CLbsLogEvent::SetCostInformation(TUint aCostInformation)
	{
	if(aCostInformation == 0)
		{
		iCostInformationAvailable = EFalse;
		iCostInformation = aCostInformation;
		}
	else
		{
		iCostInformationAvailable = ETrue;
		iCostInformation = aCostInformation;
		}
	}

/** get the lbs service cost info */
EXPORT_C TUint CLbsLogEvent::CostInformation() const
	{
	return iCostInformation;
	}

/** get the TPositonInfo data */
EXPORT_C TPositionInfoBase* CLbsLogEvent::PositionInfo() const
	{
	return iPosInfo;
	}
	
void CLbsLogEvent::WritePosInfo(RWriteStream& aWriteStream, const TPositionInfo* aPosInfo) const
	{
	if (aPosInfo != NULL)
		{
		aWriteStream.WriteInt8L((TInt8)1);
		
		// TPositionInfoBase
		aWriteStream.WriteInt32L(aPosInfo->ModuleId().iUid);
		aWriteStream.WriteUint32L(aPosInfo->PositionMode());
		aWriteStream.WriteUint32L(aPosInfo->PositionModeReason());
		// TPositionInfo.iPosition
		TPosition pos;
		aPosInfo->GetPosition(pos);
		// TPositionInfo.TPosition.iTime
		TInt64 time = pos.Time().Int64();
		TUint8* timeAddr = reinterpret_cast<TUint8*>(&time);
		aWriteStream.WriteL(timeAddr, sizeof(TInt64));
		// Accuracry from TLocality
		aWriteStream.WriteReal32L(pos.HorizontalAccuracy());
		aWriteStream.WriteReal32L(pos.VerticalAccuracy());
		// POsition from TCoord
		aWriteStream.WriteReal64L(pos.Latitude());
		aWriteStream.WriteReal64L(pos.Longitude());
		aWriteStream.WriteReal32L(pos.Altitude());
		}
	else
		{
		aWriteStream.WriteInt8L((TInt8)0);
		}
	
	
	}
	
	
void CLbsLogEvent::ReadPosInfo(RReadStream& aReadStream, TPositionInfo*& aPosInfo)
	{
	if (aPosInfo != NULL)
		{
		delete aPosInfo;
		aPosInfo = NULL;
		}


	TInt8 written = aReadStream.ReadInt8L();
	if (written != 0)
		{
		aPosInfo = new TPositionInfo();
		aPosInfo->SetModuleId(TUid::Uid(aReadStream.ReadInt32L()));
		aPosInfo->SetPositionMode(aReadStream.ReadUint32L());
		aPosInfo->SetPositionModeReason(aReadStream.ReadUint32L());
		// Time
		TPosition pos;
		TInt64 time;
		TUint8* timeAddr = reinterpret_cast<TUint8*>(&time);
		aReadStream.ReadL(timeAddr, sizeof(TInt64));
		pos.SetTime(time);
		// Accuracy
		TReal32 horAcc = aReadStream.ReadReal32L(); 
		TReal32 verAcc = aReadStream.ReadReal32L();
		pos.SetAccuracy(horAcc, verAcc);
		// Position
		TReal64 lat = aReadStream.ReadReal64L();
		TReal64 lon = aReadStream.ReadReal64L();
		TReal32 alt = aReadStream.ReadReal32L();
		pos.SetCoordinate(lat, lon, alt);
//		pos.SetCoordinate(aReadStream.ReadReal64L(), aReadStream.ReadReal64L(), aReadStream.ReadReal32L());
		
		aPosInfo->SetPosition(pos);
		}
	}

