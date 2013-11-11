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


#include <e32cmn.h>
#include <lbs/lbslocclasstypes.h>
#include <lbs/test/tlbsprivfwtestchannelmsgtypes.h>




//
// Base Message Class
//


EXPORT_C TLbsTestChannelMsgBase::TLbsTestChannelMsgType TLbsTestChannelMsgBase::Type() const
	{
	return iType;
	}
	
EXPORT_C TUint TLbsTestChannelMsgBase::RequestId() const
	{
	return iRequestId;
	}

EXPORT_C TInt32 TLbsTestChannelMsgBase::Size() const
	{
	return iSize;
	}

EXPORT_C TLbsTestChannelMsgBase& TLbsTestChannelMsgBase::operator= (const TLbsTestChannelMsgBase& aSrc)
	{
	__ASSERT_ALWAYS(Type() == aSrc.Type(), User::Panic(_L("LbsPrivFwInternal"), 2));
	Mem::Copy(this, &aSrc, aSrc.Size());
	return *this;
	}
	
EXPORT_C TLbsTestChannelMsgBase::TLbsTestChannelMsgBase() :
	iType(EMessageUnknown),
	iSize(sizeof(TLbsTestChannelMsgBase))
	{
	// Must make sure that the data to be stored is not bigger than the P&S buffer.
	__ASSERT_DEBUG(iSize <= KMaximumMessageSizeBytes, User::Panic(_L("LbsPrivFwInternal"), 1));
	}

EXPORT_C TLbsTestChannelMsgBase::TLbsTestChannelMsgBase(const TLbsTestChannelMsgBase& aSrc)
	{
	*this = aSrc;	
	}

TLbsTestChannelMsgBase::TLbsTestChannelMsgBase(TLbsTestChannelMsgType aType,
											   TInt aSize):
	iType(aType),
	iSize(aSize)
	{
	// Must make sure that the data to be stored is not bigger than the P&S buffer.
	__ASSERT_DEBUG(iSize <= KMaximumMessageSizeBytes, User::Panic(_L("LbsNetInternal"), 1));
	}
	
TLbsTestChannelMsgBase::TLbsTestChannelMsgBase(TLbsTestChannelMsgType aType, 
											   TInt aSize,
											   TUint aRequestId) :
	iType(aType),
	iSize(aSize),
	iRequestId(aRequestId)
	{
	// Must make sure that the data to be stored is not bigger than the P&S buffer.
	__ASSERT_DEBUG(iSize <= KMaximumMessageSizeBytes, User::Panic(_L("LbsNetInternal"), 1));
	}

//
// Notify Request Message
//

EXPORT_C TLbsPrivFwNotifyRequestMsg::TLbsPrivFwNotifyRequestMsg(TUint& aRequestId) :
	TLbsTestChannelMsgBase(EMessageNotifyRequest, 
						   sizeof(TLbsPrivFwNotifyRequestMsg), 
						   aRequestId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsPrivFwNotifyRequestMsg) <= KMaximumMessageSizeBytes);
	}

//
// Verify Request Message
//

EXPORT_C TLbsPrivFwVerifyRequestMsg::TLbsPrivFwVerifyRequestMsg(TUint& aRequestId) :
	TLbsTestChannelMsgBase(EMessageVerifyRequest, 
						   sizeof(TLbsPrivFwVerifyRequestMsg), 
						   aRequestId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsPrivFwVerifyRequestMsg) <= KMaximumMessageSizeBytes);
	//iTestRequestInfo = new (ELeave) TLbsTestRequestInfo();
	}

//
// Notify Response Message
//


EXPORT_C TLbsPrivFwNotifyResponseMsg::TLbsPrivFwNotifyResponseMsg(TUint& aRequestId) :
	TLbsTestChannelMsgBase(EMessageNotifyResponse, 
						   sizeof(TLbsPrivFwNotifyResponseMsg), 
						   aRequestId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsPrivFwNotifyResponseMsg) <= KMaximumMessageSizeBytes);
	
	iResponse = EResponseTypeUnknown;
	iResponseDelay = 0;
	}


//
// Verify Response Message
//


EXPORT_C TLbsPrivFwVerifyResponseMsg::TLbsPrivFwVerifyResponseMsg(TUint& aRequestId) :
	TLbsTestChannelMsgBase(EMessageVerifyResponse, 
						   sizeof(TLbsPrivFwVerifyResponseMsg), 
						   aRequestId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsPrivFwVerifyResponseMsg) <= KMaximumMessageSizeBytes);
	
	iResponse = EResponseTypeUnknown;
	iResponseDelay = 0;
	}

//
// Session Complete Message
//

EXPORT_C TLbsPrivFwSessionCompleteMsg::TLbsPrivFwSessionCompleteMsg(TUint& aRequestId) :
	TLbsTestChannelMsgBase(EPrivacyMsgSessionComplete, 
						   sizeof(TLbsPrivFwSessionCompleteMsg), 
						   aRequestId)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsPrivFwSessionCompleteMsg) <= KMaximumMessageSizeBytes);
	
	iCompleteReason = -1;
	}


//
// Controller CloseDown Message
//

EXPORT_C TLbsPrivFwCtrlCloseDownMsg::TLbsPrivFwCtrlCloseDownMsg() :
	TLbsTestChannelMsgBase(EMessageCtrlCloseDown, 
						   sizeof(TLbsPrivFwCtrlCloseDownMsg))
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(TLbsPrivFwCtrlCloseDownMsg) <= KMaximumMessageSizeBytes);
	
	}
