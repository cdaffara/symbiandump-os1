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
#include <lbs/test/tlbsprivfwtestchannelmsgtypes.h>
#include "tlbsprivfwtestchannelmsgfifo.h"

/**
*/
RLbsTestChannelMsgFifo::TBufferItem::TBufferItem():
	iEmpty(ETrue),
	iMsg(reinterpret_cast<TLbsTestChannelMsgBase&>(iMsgBuffer))
	{
	}
	
/**
*/
EXPORT_C RLbsTestChannelMsgFifo::RLbsTestChannelMsgFifo(TInt aMaxCount):
	iMaxCount(aMaxCount)
	{
	}
	
/**
*/
EXPORT_C void RLbsTestChannelMsgFifo::OpenL()
	{
	iQ.ReserveL(iMaxCount);
	iBuffers.ReserveL(iMaxCount);
	
	TBufferItem* bufPtr;
	for (TInt i = 0; i < iMaxCount; i++)
		{
		bufPtr = new(ELeave) TBufferItem;
		iBuffers.AppendL(bufPtr);
		}
	}

/**
*/
EXPORT_C void RLbsTestChannelMsgFifo::Close()
	{
	for (TInt i = 0; i < iMaxCount; i++)
		{
		delete iBuffers[i];
		}
	
	iQ.Close();
	iBuffers.Close();
	}
	
/**
*/
EXPORT_C void RLbsTestChannelMsgFifo::Write(const TLbsTestChannelMsgBase& aMsg)
	{
	TInt index = FindEmptyBuffer();
	//TBD panic if not found

	iBuffers[index]->iEmpty = EFalse;
	Mem::Copy(iBuffers[index]->iMsgBuffer, &aMsg, aMsg.Size());
	
	TInt err = iQ.Append(index);
	}

/**
*/
EXPORT_C const TLbsTestChannelMsgBase& RLbsTestChannelMsgFifo::Read()
	{
	TInt index = iQ[0];
	iQ.Remove(0);
	iBuffers[index]->iEmpty = ETrue;
	
	return iBuffers[index]->iMsg;
	}

/**
*/
EXPORT_C TBool RLbsTestChannelMsgFifo::IsEmpty()
	{
	return (iQ.Count() == 0);
	}

/**
*/
EXPORT_C TBool RLbsTestChannelMsgFifo::FindAndRemove(TLbsTestChannelMsgBase::TLbsTestChannelMsgType aMsgType)
	{
	TInt count = iQ.Count();
	TInt index;
	TInt ret = EFalse;
	
	for (TInt i = 0; i < count; i++)
		{
		index = iQ[i];
		if (iBuffers[index]->iMsg.Type() == aMsgType)
			{
			iQ.Remove(i);
			iBuffers[index]->iEmpty = ETrue;
			ret = ETrue;
			break;
			}
		}
	
	return ret;
	}
	
/**
*/
TInt RLbsTestChannelMsgFifo::FindEmptyBuffer()
	{
	TInt ret = KErrNotFound;
	
	for (TInt i = 0; i < iMaxCount; i++)
		{
		if (iBuffers[i]->iEmpty != EFalse)
			{
			ret = i;
			break;
			}
		}
		
	return ret;
	}
