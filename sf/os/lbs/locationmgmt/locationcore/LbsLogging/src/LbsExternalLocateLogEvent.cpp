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

#include <lbs/lbsexternallocatelogevent.h>
#include <logfilterandeventconstants.hrh>

/** set the request originator uid */
EXPORT_C void CLbsExternalLocateLogEvent::SetRequestOriginator(TUid aRequestor)
	{
	iRequestOriginator = aRequestor;
	}

/** get the request originator uid */
EXPORT_C TUid CLbsExternalLocateLogEvent::RequestOriginator() const
	{
	return iRequestOriginator;
	}

/** get the request privacy paramters */
EXPORT_C TLbsLoggingPrivacyRequestParams CLbsExternalLocateLogEvent::PrivacyRequestParams() const
	{
	return iPrivacyReqInfo;
	}

/** get the response privacy paramters */
EXPORT_C TLbsLoggingPrivacyResponseParams CLbsExternalLocateLogEvent::PrivacyResponseParams() const
	{
	return iPrivacyResInfo;
	}

/** return the reference position */
EXPORT_C TPositionInfoBase* CLbsExternalLocateLogEvent::ReferenceLocation() const
	{
	return iRefPosInfo;
	}

/**
 Create external locate data logging event object
 @param aRequestOriginator, uid of the originator 
 @param aRefPosInfo, TPositionInfoBase* of the reference position data
 @param aPosInfo,  TPositionInfoBase* of the GPS position data
 @return CLbsExternalLocateLogEvent object
*/
EXPORT_C CLbsExternalLocateLogEvent* CLbsExternalLocateLogEvent::NewL(TUid aRequestOriginator,TPositionInfoBase* aRefPosInfo, TPositionInfoBase* aPosInfo)
	{
	CLbsExternalLocateLogEvent* self = new(ELeave) CLbsExternalLocateLogEvent;
	CleanupStack::PushL(self);
	self->ConstructL(aRequestOriginator, aRefPosInfo, aPosInfo);
	CleanupStack::Pop(self);		
	return self;
	}

/** construction function */
void CLbsExternalLocateLogEvent::ConstructL(TUid aRequestOriginator,TPositionInfoBase* aRefPosInfo, TPositionInfoBase* aPosInfo)
	{
	BaseConstructL();
	iRefPosInfo = CopyPosInfoToEvent(aRefPosInfo);
	iPosInfo = CopyPosInfoToEvent(aPosInfo);
	
	iRequestOriginator = aRequestOriginator;
	}
	
CLbsExternalLocateLogEvent::~CLbsExternalLocateLogEvent()
	{
	if(iRefPosInfo != NULL)
		{
		delete iRefPosInfo;
		}
	}

/** implementation of inherited externalization function */
void CLbsExternalLocateLogEvent::DoExternalizeL(RWriteStream& aWriteStream) const
	{
	aWriteStream.WriteInt32L(iRequestOriginator.iUid);
	// write privacy request
	aWriteStream.WriteInt32L(iPrivacyReqInfo.iSessionId.SessionOwner().iUid);
	aWriteStream.WriteUint32L(iPrivacyReqInfo.iSessionId.SessionNum());
	aWriteStream.WriteInt32L(static_cast<TInt>(iPrivacyReqInfo.iRequestPrivacy.RequestAction()));
	aWriteStream.WriteInt32L(static_cast<TInt>(iPrivacyReqInfo.iRequestPrivacy.RequestAdvice()));	
	TBuf8<128> rxBuf;
	iPrivacyReqInfo.iRequestInfo.GetRequesterId(rxBuf);
	TInt len = rxBuf.Length();
	aWriteStream.WriteInt32L(len);
	aWriteStream.WriteL(rxBuf, len);
	rxBuf.Zero();
	iPrivacyReqInfo.iRequestInfo.GetClientName(rxBuf);
	len = rxBuf.Length();
	aWriteStream.WriteInt32L(len);	
	aWriteStream.WriteL(rxBuf, len);
	rxBuf.Zero();
	iPrivacyReqInfo.iRequestInfo.GetClientExternalId(rxBuf);
	len = rxBuf.Length();
	aWriteStream.WriteInt32L(len);
	aWriteStream.WriteL(rxBuf, len);
	
	// response	
	aWriteStream.WriteInt32L(iPrivacyResInfo.iSessionId.SessionOwner().iUid);
	aWriteStream.WriteUint32L(iPrivacyResInfo.iSessionId.SessionNum());
	aWriteStream.WriteInt32L(static_cast<TInt>(iPrivacyResInfo.iResult));

	// write reference pos info
	WritePosInfo(aWriteStream, iRefPosInfo);
	// write pos info
	WritePosInfo(aWriteStream, iPosInfo);
	}

/** implementation of inherited internalization function */
void CLbsExternalLocateLogEvent::DoInternalizeL(RReadStream& aReadStream)
	{
	iRequestOriginator = TUid::Uid(aReadStream.ReadInt32L());
	
	// read privacy
	iPrivacyReqInfo.iSessionId.SetSessionOwner(TUid::Uid(aReadStream.ReadInt32L()));
	iPrivacyReqInfo.iSessionId.SetSessionNum(aReadStream.ReadUint32L());
	
	iPrivacyReqInfo.iRequestPrivacy.SetRequestAction(static_cast<TLbsNetPosRequestPrivacy::TLbsRequestAction>(aReadStream.ReadInt32L()));
	iPrivacyReqInfo.iRequestPrivacy.SetRequestAdvice(static_cast<TLbsNetPosRequestPrivacy::TLbsRequestAdvice>(aReadStream.ReadInt32L()));
	
	TBuf8<128> rxBuf;
	TInt len = aReadStream.ReadInt32L();
	aReadStream.ReadL(rxBuf, len);
	iPrivacyReqInfo.iRequestInfo.SetRequesterId(rxBuf);
	rxBuf.Zero();
	len = aReadStream.ReadInt32L();
	aReadStream.ReadL(rxBuf, len);
	iPrivacyReqInfo.iRequestInfo.SetClientName(rxBuf);
	rxBuf.Zero();
	len = aReadStream.ReadInt32L();
	aReadStream.ReadL(rxBuf, len);	
	iPrivacyReqInfo.iRequestInfo.SetClientExternalId(rxBuf);
	
	// response	
	iPrivacyResInfo.iSessionId.SetSessionOwner(TUid::Uid(aReadStream.ReadInt32L()));
	iPrivacyResInfo.iSessionId.SetSessionNum(aReadStream.ReadUint32L());
	iPrivacyResInfo.iResult = (static_cast<CLbsNetworkProtocolBase::TLbsPrivacyResponse>(aReadStream.ReadInt32L()));

	// ref position info
	ReadPosInfo(aReadStream, iRefPosInfo);
	// pos Info
	ReadPosInfo(aReadStream, iPosInfo);
	}

/** implementation of inherited update object function */
void CLbsExternalLocateLogEvent::DoUpdateL(const CLbsLogEvent& aSourceLogEvent)
	{
	__ASSERT_DEBUG(aSourceLogEvent.EventType() == TUid::Uid(KLogLbsExternalLocateEventType), User::Panic(KLbsLoggerPanic, KErrArgument));
	
	const CLbsExternalLocateLogEvent* event = (reinterpret_cast<const CLbsExternalLocateLogEvent*>(&aSourceLogEvent));
	iRequestOriginator = event->RequestOriginator();
	CopyPrivacyRequestParams(iPrivacyReqInfo, event->PrivacyRequestParams());
	CopyPrivacyResponseParams(iPrivacyResInfo, event->PrivacyResponseParams());
	delete iPosInfo;
	iPosInfo = CopyPosInfoToEvent(event->PositionInfo());
	delete iRefPosInfo;
	iRefPosInfo = CopyPosInfoToEvent(event->ReferenceLocation());
	}

/** implementation of inherited copy object function */
void CLbsExternalLocateLogEvent::DoCopyL(CLbsLogEvent*& aTargetLogEvent) const
	{
	aTargetLogEvent = CLbsExternalLocateLogEvent::NewL(iRequestOriginator, iRefPosInfo, NULL);
	CLbsExternalLocateLogEvent* event = reinterpret_cast<CLbsExternalLocateLogEvent*>(aTargetLogEvent);
	event->SetPrivacyRequestParams(iPrivacyReqInfo);
	event->SetPrivacyResponseParams(iPrivacyResInfo);
	event->SetPositionInfo(iPosInfo);
	}

EXPORT_C void CLbsExternalLocateLogEvent::SetPrivacyRequestParams(const TLbsLoggingPrivacyRequestParams& aParams)
	{
	CopyPrivacyRequestParams(iPrivacyReqInfo, aParams);
	}

EXPORT_C void CLbsExternalLocateLogEvent::SetPrivacyResponseParams(const TLbsLoggingPrivacyResponseParams& aParams)
	{
	CopyPrivacyResponseParams(iPrivacyResInfo, aParams);
	}

EXPORT_C void CLbsExternalLocateLogEvent::SetReferenceLocation(TPositionInfoBase* 	aPosInfo)
	{
	if(aPosInfo != NULL)
		{
		if(iRefPosInfo != NULL)
			{
			delete iRefPosInfo;
			iRefPosInfo = NULL;
			}
		iRefPosInfo = CopyPosInfoToEvent(aPosInfo);
		}
	}

void CLbsExternalLocateLogEvent::CopyPrivacyRequestParams(TLbsLoggingPrivacyRequestParams& aTargetPrivacyInfo, const TLbsLoggingPrivacyRequestParams& aSourcePrivacyInfo)
	{
	TBuf8<64> rxBuf;
	TBuf8<64> orBuf;
	aSourcePrivacyInfo.iRequestInfo.GetRequesterId(rxBuf);
	aTargetPrivacyInfo.iRequestInfo.GetRequesterId(orBuf);
	aTargetPrivacyInfo.iRequestInfo.SetRequesterId(rxBuf);
	rxBuf.Zero();
	orBuf.Zero();
	
	aSourcePrivacyInfo.iRequestInfo.GetClientName(rxBuf);
	aTargetPrivacyInfo.iRequestInfo.GetClientName(orBuf);
	aTargetPrivacyInfo.iRequestInfo.SetClientName(rxBuf);
	rxBuf.Zero();
	orBuf.Zero();
	
	aSourcePrivacyInfo.iRequestInfo.GetClientExternalId(rxBuf);
	aTargetPrivacyInfo.iRequestInfo.GetClientExternalId(orBuf);
	aTargetPrivacyInfo.iRequestInfo.SetClientExternalId(rxBuf);
	rxBuf.Zero();
	orBuf.Zero();
	
	aTargetPrivacyInfo.iRequestPrivacy.SetRequestAction(aSourcePrivacyInfo.iRequestPrivacy.RequestAction());
	aTargetPrivacyInfo.iRequestPrivacy.SetRequestAdvice(aSourcePrivacyInfo.iRequestPrivacy.RequestAdvice());
	aTargetPrivacyInfo.iSessionId.SetSessionNum(aSourcePrivacyInfo.iSessionId.SessionNum());
	aTargetPrivacyInfo.iSessionId.SetSessionOwner(aSourcePrivacyInfo.iSessionId.SessionOwner());
	}
	
void CLbsExternalLocateLogEvent::CopyPrivacyResponseParams(TLbsLoggingPrivacyResponseParams& aTargetPrivacyInfo, const TLbsLoggingPrivacyResponseParams& aSourcePrivacyInfo)
	{
	aTargetPrivacyInfo.iResult = aSourcePrivacyInfo.iResult;
	aTargetPrivacyInfo.iSessionId.SetSessionNum(aSourcePrivacyInfo.iSessionId.SessionNum());
	aTargetPrivacyInfo.iSessionId.SetSessionOwner(aSourcePrivacyInfo.iSessionId.SessionOwner());
	}
		
CLbsExternalLocateLogEvent::CLbsExternalLocateLogEvent()
	{
	SetEventType(TUid::Uid(KLogLbsExternalLocateEventType));
	iRefPosInfo = NULL;
	}

TInt CLbsExternalLocateLogEvent::DataSize() const
	{
	TBuf8<128> temp;
	TInt textSize = 0;
	iPrivacyReqInfo.iRequestInfo.GetRequesterId(temp);
	textSize += temp.Length();
	temp.Zero();
	iPrivacyReqInfo.iRequestInfo.GetClientName(temp);
	textSize += temp.Length();
	temp.Zero();
	iPrivacyReqInfo.iRequestInfo.GetClientExternalId(temp);
	textSize += temp.Length();
	temp.Zero();

	return 	sizeof(TInt32) + 
			// request privacy
			(sizeof(TInt32) * 6) +
			sizeof(TUint32) + 
			textSize + 
			// response privacy
			(sizeof(TInt32) * 2) + 
			sizeof(TUint32) + 
			// Pos infos
			((iPosInfo == NULL) ? sizeof(TInt8) : iPosInfoWriteSize) + 
			((iRefPosInfo == NULL) ? sizeof(TInt8) : iPosInfoWriteSize);
	}

