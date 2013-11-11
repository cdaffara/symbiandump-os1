// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include "Avctp.h"
#include "avctputils.h"

// KLogComponent defined locally below to prevent multiple declarations

/**
Set iIsClearToSend to EFalse to signify the lower protocol sap is blocked
from writing data
@internalComponent
*/
inline void CAvctpTransport::SetSendBlocked(TInt aChannel)
    {
    iIsClearToSend[aChannel] = EFalse;
    }

/** 
Set iIsClearToSend to ETrue to signify the lower protocol sap is no longer
blocked from writing data
@internalComponent
*/
inline void CAvctpTransport::SetClearToSend(TInt aChannel)
    {
    iIsClearToSend[aChannel] = ETrue;
    }
 
/** 
@return whether or not the muxer is able to send data via it's lower
protocol sap.
@internalComponent
*/
inline TBool CAvctpTransport::IsClearToSend(TInt aChannel) const
    {
    return iIsClearToSend[aChannel];
    }
    
inline const TBTDevAddr& CAvctpTransport::DevAddr() const
	{
	return iRemoteAddr;
	}
	
/**
Start the asynchronous callback to continue processing later

  @internalComponent
*/
inline void CAvctpTransport::StartSecondChannelNewDataAsyncCallBack()
	{
	iNewDataAsyncCallBack->CallBack();
	}

/**
Cancel the asynchronous callback

  @internalComponent
*/
inline void CAvctpTransport::CancelSecondChannelNewDataAsyncCallBack()
	{
	iNewDataAsyncCallBack->Cancel();
	}
	
/**
  @internalComponent
*/	
inline CAvctpPacketMgr& CAvctpTransport::PacketMgr()
	{
	__ASSERT_ALWAYS(iPacketMgr, Panic(SymbianAvctp::ENullPacketMgr));
	return *iPacketMgr;
	}

