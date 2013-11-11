/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "lbsagpsngmsgs.h"

//
// AssistanceData Request Message
//
EXPORT_C TLbsNetAssistanceDataRequestMsg::TLbsNetAssistanceDataRequestMsg(TLbsAsistanceDataGroupInt aDataRequestMask):
	TLbsNetInternalMsgBase(EAssistanceDataRequest,
						   sizeof(TLbsNetAssistanceDataRequestMsg))
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetAssistanceDataRequestMsg) <= KMaximumMessageSizeBytes);
	
	iDataRequestMask = aDataRequestMask;
	}
									   	
EXPORT_C TLbsAsistanceDataGroupInt TLbsNetAssistanceDataRequestMsg::DataRequestMask() const
	{
	return iDataRequestMask;
	}
	
//
// AssistanceData Response Message
//
EXPORT_C TLbsNetAssistanceDataResponseMsg::TLbsNetAssistanceDataResponseMsg(TLbsAsistanceDataGroupInt aDataResponseMask,
																			TInt aReason):
	TLbsNetInternalMsgBase(EAssistanceDataResponse,
						   sizeof(TLbsNetAssistanceDataResponseMsg))
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetAssistanceDataResponseMsg) <= KMaximumMessageSizeBytes);
	
	iDataResponseMask = aDataResponseMask;
	iReason = aReason;
	}
									   
	
EXPORT_C TLbsAsistanceDataGroupInt TLbsNetAssistanceDataResponseMsg::DataResponseMask() const
	{
	return iDataResponseMask;
	}
	
EXPORT_C TInt TLbsNetAssistanceDataResponseMsg::Reason() const
	{
	return iReason;
	}

//
// Self Location Request Message
//
EXPORT_C TLbsNetSelfLocationRequestMsg::TLbsNetSelfLocationRequestMsg(const TLbsNetSessionIdInt& aSessionId, 
										   							  const TLbsNetPosRequestOptionsAssistanceInt& aOptions):
	TLbsNetInternalMsgBase(ESelfLocationRequest, 
						   sizeof(TLbsNetSelfLocationRequestMsg),
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetSelfLocationRequestMsg) <= KMaximumMessageSizeBytes);
	
	iOptions = aOptions;
	}
									   
EXPORT_C const TLbsNetPosRequestOptionsAssistanceInt& TLbsNetSelfLocationRequestMsg::Options() const
	{
	return iOptions;
	}

//
// Self Location Cancel Message
//
EXPORT_C TLbsNetSelfLocationCancelMsg::TLbsNetSelfLocationCancelMsg(const TLbsNetSessionIdInt& aSessionId,
										  							TInt aReason):
	TLbsNetInternalMsgBase(ESelfLocationCancel, 
						   sizeof(TLbsNetSelfLocationCancelMsg),
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetSelfLocationCancelMsg) <= KMaximumMessageSizeBytes);
	
	iReason = aReason;
	}
									   
EXPORT_C TInt TLbsNetSelfLocationCancelMsg::Reason() const
	{
	return iReason;
	}
	
//
// System Status Advice Message
//
EXPORT_C TLbsNetSystemStatusAdviceMsg::TLbsNetSystemStatusAdviceMsg(TBool aTracking):
	TLbsNetInternalMsgBase(ESystemStatusAdvice,
						   sizeof(TLbsNetSystemStatusAdviceMsg))
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetSystemStatusAdviceMsg) <= KMaximumMessageSizeBytes);
	
	iTracking = aTracking;
	}

EXPORT_C TBool TLbsNetSystemStatusAdviceMsg::Tracking() const
	{
	return iTracking;
	}

//
// Session Complete Message
//

EXPORT_C TLbsNetSessionCompleteAgpsMsg::TLbsNetSessionCompleteAgpsMsg(const TLbsNetSessionIdInt& aSessionId,
															  TInt aReason) :
	TLbsNetInternalMsgBase(ESessionComplete,
						   sizeof(TLbsNetSessionCompleteAgpsMsg), 
						   aSessionId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsNetSessionCompleteAgpsMsg) <= KMaximumMessageSizeBytes);

	iReason = aReason;
	}
	
EXPORT_C TInt TLbsNetSessionCompleteAgpsMsg::Reason() const
	{
	return iReason;
	}
