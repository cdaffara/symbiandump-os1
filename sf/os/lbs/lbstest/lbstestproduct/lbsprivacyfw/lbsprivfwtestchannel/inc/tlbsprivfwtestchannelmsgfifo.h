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
 @internalTechnology
 @test
*/

#ifndef LBS_PRIVFW_TESTCHANNEL_MSGFIFO_H
#define LBS_PRIVFW_TESTCHANNEL_MSGFIFO_H

#include <e32cmn.h>
#include <lbs/test/tlbsprivfwtestchannelmsgtypes.h>

//
// A helper class used to queue messages prior sending over RLbsTestChannel
//
class RLbsTestChannelMsgFifo
	{
public:
	IMPORT_C RLbsTestChannelMsgFifo(TInt aMaxCount);
	IMPORT_C void OpenL();
	IMPORT_C void Close();
	
	IMPORT_C const TLbsTestChannelMsgBase& Read();
	IMPORT_C void  Write(const TLbsTestChannelMsgBase& aMsg);
	IMPORT_C TBool FindAndRemove(TLbsTestChannelMsgBase::TLbsTestChannelMsgType aMsgType);
	IMPORT_C TBool IsEmpty();

private:
	TInt FindEmptyBuffer();

private:	
	NONSHARABLE_CLASS(TBufferItem)
		{
	public:
		TBufferItem();
		TBool iEmpty;
		TLbsTestChannelMsgBase& iMsg;
		TUint8 iMsgBuffer[KMaximumMessageSizeBytes];
		};
	
private:		
	TInt iMaxCount;
	/** The queue. Keeps buffers (iBuffers) numbers */
	RArray<TInt> iQ;
	/** Pre-allocated buffers */
	RPointerArray<TBufferItem> iBuffers;
	};

#endif // LBS_PRIVFW_TESTCHANNEL_MSGFIFO_H
