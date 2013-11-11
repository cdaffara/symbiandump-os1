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
// Definition of assistance data channel component.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include <e32base.h>
#include <e32property.h>
#include <lbs/lbsassistancedatabase.h>
#include <lbs/lbsassistancedatabuilderset.h>

#include <lbs/lbslocerrors.h>
#include <lbserrors.h>
#include "lbsassistancedatacacheapi.h"
#include "agpschannel.h"
#include "lbsdevloggermacros.h"
#include "lbsnrhngmsgs.h"
const TLbsNetSessionIdInt KDummySessionId(TUid::Uid(0xDEADBEEF), 0xDEADBEEF);

//
// CAgpsChannel
//

CAgpsChannel::CAgpsChannel(MAgpsObserver& aObserver) : 
	CActive(EPriorityStandard),
	iSessionCompleteMsgBuffer(KDummySessionId, KErrNotFound),
	iSessionCompleteMsgValid(EFalse),
	iObserver(aObserver)
	{
	}
	
CAgpsChannel::~CAgpsChannel()
	{
	Cancel();
	iAssistanceDataMsgBuffer.Close();
	iAssistanceDataCache.Close();
	iAGPSChannel.Close();
	}

CAgpsChannel* CAgpsChannel::NewL(MAgpsObserver& aObserver)
	{
	CAgpsChannel* self = new (ELeave) CAgpsChannel(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
		
void CAgpsChannel::ConstructL()
	{
	CActiveScheduler::Add(this);
	iAssistanceDataMsgBuffer.OpenL();
	iAssistanceDataCache.OpenL();
	iAGPSChannel.OpenL(RLbsNetChannel::EChannelNG2AGPS, *this);
	}
	
void CAgpsChannel::RunL()
	{
	LBSLOG(ELogP1, "CAgpsChannel::RunL():");
	__ASSERT_DEBUG(iStatus.Int() == KErrNone, User::Panic(KLbsNRHFault, iStatus.Int()));

	// Session Complete has higher priority
	if (iSessionCompleteMsgValid)
		{
		iAGPSChannel.SendMessage(iSessionCompleteMsgBuffer, iStatus);
		iSessionCompleteMsgValid = EFalse;
		SetActive();
		}
	else if (!iAssistanceDataMsgBuffer.IsEmpty())
		{
		TLbsNetAssistanceDataResponseMsg msg(0,0); //Empty msg
		iAssistanceDataMsgBuffer.Read(msg);
		iAGPSChannel.SendMessage(msg, iStatus);
		SetActive();
		}
	}
	
void CAgpsChannel::DoCancel()
	{
	iAGPSChannel.CancelSendMessageNotification();
	}
	
TInt CAgpsChannel::RunError(TInt aError)
	{
	return aError;
	}

void CAgpsChannel::SendAssistanceDataResponse(TInt aError, 
											  TLbsAsistanceDataGroupInt aMask, 
											  const RLbsAssistanceDataBuilderSet& aData)
	{
	LBSLOG(ELogP1, "CAgpsChannel::SendAssistanceDataResponse() CAgpsChannel::SendAssistanceDataResponse:");
	TInt err = KErrNone;
	if (aError == KErrNone)
		{
		TRAP(err, SetAssistanceDataCacheL(aData));
		__ASSERT_DEBUG(err == KErrNone, User::Panic(KLbsNRHFault, err));
		}
	else if(aError == KPositionAssistanceDataReset)
		{
		err = iAssistanceDataCache.ResetAssistanceData(aMask);
		__ASSERT_DEBUG(err == KErrNone, User::Panic(KLbsNRHFault, err));
		}
	
	// Do not send if writing the cache failed
	if (err == KErrNone)
		{
		TLbsNetAssistanceDataResponseMsg msg(aMask, aError);
		if (!IsActive())
			{
			LBSLOG(ELogP2, "CAgpsChannel::SendAssistanceDataResponse() iAGPSChannel.SendMessage()");
			iAGPSChannel.SendMessage(msg, iStatus);
			SetActive();
			}
		else
			{
			LBSLOG(ELogP2, "CAgpsChannel::SendAssistanceDataResponse() iAssistanceDataMsgBuffer.Write()");
			iAssistanceDataMsgBuffer.Write(msg);
			}
		}
	}
	
void CAgpsChannel::SendSessionComplete(TInt aReason, const TLbsNetSessionIdInt& aSessionId)
	{
	TLbsNetSessionCompleteAgpsMsg msg(aSessionId, aReason);
	if (!IsActive())
		{
		iAGPSChannel.SendMessage(msg, iStatus);
		SetActive();
		}
	else
		{
		iSessionCompleteMsgBuffer = msg;
		iSessionCompleteMsgValid = ETrue;
		}
	}
	

void CAgpsChannel::SetAssistanceDataCacheL(const RLbsAssistanceDataBuilderSet& aData)
	{
	RUEPositioningGpsAlmanacBuilder* almanacPtr;
	RUEPositioningGpsIonosphericModelBuilder* ionosphericPtr;
	RUEPositioningGpsNavigationModelBuilder* navigationPtr;
	RUEPositioningGpsReferenceTimeBuilder* referenceTimePtr;
	RUEPositioningGpsUtcModelBuilder*  utcPtr;
	RUEPositioningGpsAcquisitionAssistanceBuilder* acquisitionPtr;
	RBadSatListBuilder* badSatPtr;
	RReferenceLocationBuilder* referenceLocationPtr;

	RLbsAssistanceDataBuilderSet& data = const_cast<RLbsAssistanceDataBuilderSet&>(aData);
	
	// Check each item in turn and publish if it is valid
	// Publish the latest assistance data into the relevant properties
	if (KErrNone == data.GetDataBuilder(almanacPtr))
		{
		if (almanacPtr->IsDataAvailable())
			{
			User::LeaveIfError(iAssistanceDataCache.SetAssistDataItem(EAssistanceDataAlmanac, 
									   *almanacPtr));
			}
		}
	
	if (KErrNone == data.GetDataBuilder(acquisitionPtr))
		{
		if (acquisitionPtr->IsDataAvailable())
			{
			User::LeaveIfError(iAssistanceDataCache.SetAssistDataItem(EAssistanceDataAquisitionAssistance, 
									   *acquisitionPtr));
			}
		}
		
	if (KErrNone == data.GetDataBuilder(badSatPtr))
		{
		if (badSatPtr->IsDataAvailable())
			{
			User::LeaveIfError(iAssistanceDataCache.SetAssistDataItem(EAssistanceDataBadSatList, 
									   *badSatPtr));
			}
		}
		
	if (KErrNone == data.GetDataBuilder(navigationPtr))
		{
		if (navigationPtr->IsDataAvailable())
			{
			User::LeaveIfError(iAssistanceDataCache.SetAssistDataItem(EAssistanceDataNavigationModel, 
									   *navigationPtr));
			}
		}

	if (KErrNone == data.GetDataBuilder(referenceTimePtr))
		{
		if (referenceTimePtr->IsDataAvailable())
			{
			User::LeaveIfError(iAssistanceDataCache.SetAssistDataItem(EAssistanceDataReferenceTime, 
									   *referenceTimePtr));
			}
		}

	if (KErrNone == data.GetDataBuilder(ionosphericPtr))
		{
		if (ionosphericPtr->IsDataAvailable())
			{
			User::LeaveIfError(iAssistanceDataCache.SetAssistDataItem(EAssistanceDataIonosphericModel, 
									   *ionosphericPtr));
			}
		}

	if (KErrNone == data.GetDataBuilder(referenceLocationPtr))
		{
		if (referenceLocationPtr->IsDataAvailable())
			{
			User::LeaveIfError(iAssistanceDataCache.SetAssistDataItem(EAssistanceDataReferenceLocation, 
									   *referenceLocationPtr));
			}
		}

	if (KErrNone == data.GetDataBuilder(utcPtr))
		{
		if (utcPtr->IsDataAvailable())
			{
			User::LeaveIfError(iAssistanceDataCache.SetAssistDataItem(EAssistanceDataPositioningGpsUtcModel, 
									   *utcPtr));
			}
		}
	}
	
void CAgpsChannel::ProcessNetChannelMessage(RLbsNetChannel::TLbsNetChannelId aChannelId, const TLbsNetInternalMsgBase& aMessage)
	{
	__ASSERT_DEBUG(aChannelId == RLbsNetChannel::EChannelNG2AGPS, User::Panic(KLbsNGFault, ENGUnexpectedNetChannelId));
	(void) aChannelId;
	
	const TLbsNetAssistanceDataRequestMsg& msgAssReq    = static_cast<const TLbsNetAssistanceDataRequestMsg&>(aMessage);
	const TLbsNetSelfLocationRequestMsg&   msgLocReq    = static_cast<const TLbsNetSelfLocationRequestMsg&>(aMessage);
	const TLbsNetSelfLocationCancelMsg&    msgLocCancel = static_cast<const TLbsNetSelfLocationCancelMsg&>(aMessage);
	const TLbsNetSystemStatusAdviceMsg&	   msgSysStat   = static_cast<const TLbsNetSystemStatusAdviceMsg&>(aMessage);
												
	switch (aMessage.Type())
		{
		case TLbsNetInternalMsgBase::EAssistanceDataRequest:
			iObserver.OnAssistanceDataRequest(msgAssReq.DataRequestMask());
			break;
			
		case TLbsNetInternalMsgBase::ESelfLocationRequest:
			iObserver.OnSelfLocationRequest(msgLocReq.SessionId(), msgLocReq.Options());
			break;
			
		case TLbsNetInternalMsgBase::ESelfLocationCancel:
			iObserver.OnSelfLocationCancel(msgLocCancel.SessionId(), msgLocCancel.Reason());
			break;
			
		case TLbsNetInternalMsgBase::ESystemStatusAdvice:
			iObserver.OnSystemStatusAdvice(msgSysStat.Tracking());
			break;
			
		default:
			__ASSERT_DEBUG(EFalse, User::Panic(KLbsNGFault, ENGUnexpectedMsgType));
			break;
		}
	}

//
// RAssistanceDataMsgBuffer
//
RAssistanceDataMsgBuffer::TBufferItem::TBufferItem():
	iValid(EFalse),
	iReason(KErrNone)
	{
	}
	
/**
*/	
RAssistanceDataMsgBuffer::RAssistanceDataMsgBuffer():
	iBuffer(KAssistanceDataListCount),
	iEmpty(ETrue),
	iErrGeneric()
	{
	}
	
/**
*/	
void RAssistanceDataMsgBuffer::OpenL()
	{
	TBufferItem emptyItem;
	
	for (TInt i = 0; i < KAssistanceDataListCount; i++)
		{
		iBuffer.AppendL(emptyItem);
		}
	}
/**
*/
void RAssistanceDataMsgBuffer::Close()
	{
	iBuffer.Close();
	}
/**
*/	
void RAssistanceDataMsgBuffer::Write(const TLbsNetAssistanceDataResponseMsg& aMsg)
	{
	LBSLOG(ELogP1, "RAssistanceDataMsgBuffer::Write():");
	TInt reason = aMsg.Reason();
	TLbsAsistanceDataGroup mask = aMsg.DataResponseMask();
	__ASSERT_DEBUG((mask != 0) || (mask==0 && reason<0), User::Panic(KLbsNGFault, KErrArgument));
	if (EAssistanceDataNone == mask)
		{
		// assume something error code comes in
		iErrGeneric.iValid = ETrue;
		iErrGeneric.iReason = reason;
		}	
	else
		{
		for (TInt i = 0; i < KAssistanceDataListCount; i++)
			{
			if (mask & 0x01 == 0x01)
				{
				iBuffer[i].iValid  = ETrue;
				iBuffer[i].iReason = reason;
				}
			
			mask = mask >> 1;
			}
		}
	iEmpty = EFalse;
	}
	
/**
*/	
void RAssistanceDataMsgBuffer::Read(TLbsNetAssistanceDataResponseMsg& aMsg)
	{
	LBSLOG(ELogP1, "RAssistanceDataMsgBuffer::Read()"); 
	__ASSERT_DEBUG((iBuffer.Count() != 0), User::Panic(KLbsNGFault, KErrUnderflow));

	TInt reason = KErrNone;
	TInt valid = EFalse;
	TInt mask = 0;
	TInt i; //first loop
	TInt j; //second loop
	// Check error item
	if (iErrGeneric.iValid)	
		{
		mask = 0;
		reason = iErrGeneric.iReason;
		iErrGeneric.iValid = EFalse;

		iEmpty = ETrue;
		for (i = 0; i < KAssistanceDataListCount; i++)
			{
			if (iBuffer[i].iValid) 
				{
				iEmpty = EFalse;
				break;
				}
			}
		}
	else
		{
		// Find first valid item. Save the reason code
		for (i = 0; i < KAssistanceDataListCount; i++)
			{
			if (iBuffer[i].iValid) 
				{
				reason = iBuffer[i].iReason;
				break;
				}
			}

		// Find other valid items with the same reason code
		for (j = i; j < KAssistanceDataListCount; j++)
			{
			if (iBuffer[j].iValid && (iBuffer[j].iReason == reason))
				{
				mask = mask | (0x00000001 << j);
				iBuffer[j].iValid = EFalse;
				}
			valid = valid || iBuffer[j].iValid;
			}
		iEmpty = !valid;
		}
	
	LBSLOG2(ELogP2, "RAssistanceDataMsgBuffer::Read() iEmpty=%d", iEmpty);
	LBSLOG2(ELogP2, "RAssistanceDataMsgBuffer::Read() mask=%d", mask);
	LBSLOG2(ELogP2, "RAssistanceDataMsgBuffer::Read() reason=%d", reason);

	TLbsNetAssistanceDataResponseMsg msg(mask, reason);
	aMsg = msg;
	}
	
/**
*/	
TBool RAssistanceDataMsgBuffer::IsEmpty() const
	{
	return iEmpty;
	}
