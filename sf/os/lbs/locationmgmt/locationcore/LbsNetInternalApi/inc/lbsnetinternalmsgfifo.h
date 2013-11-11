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
 @released
*/

#ifndef LBSNETINTERNALMSGFIFO_H
#define LBSNETINTERNALMSGFIFO_H

#include <e32cmn.h>
#include "lbsnetinternalmsgtypes.h"

//
// A helper class used to queue messages prior sending over RLbsNetChannel
//
class RLbsNetMsgFifo
	{
public:
	IMPORT_C RLbsNetMsgFifo(TInt aMaxCount);
	IMPORT_C void OpenL();
	IMPORT_C void Close();
	
	IMPORT_C const TLbsNetInternalMsgBase& Read();
	IMPORT_C void  Write(const TLbsNetInternalMsgBase& aMsg);
	IMPORT_C TBool FindAndRemove(TLbsNetInternalMsgBase::TLbsNetInternalMsgType aMsgType);
	IMPORT_C TBool IsEmpty();

private:
	TInt FindEmptyBuffer();

private:	
	NONSHARABLE_CLASS(TBufferItem)
		{
	public:
		TBufferItem();
		TBool iEmpty;
		TLbsNetInternalMsgBase& iMsg;
		TUint8 iMsgBuffer[KMaximumMessageSizeBytes];
		};
	
private:		
	TInt iMaxCount;
	/** The queue. Keeps buffers (iBuffers) numbers */
	RArray<TInt> iQ;
	/** Pre-allocated buffers */
	RPointerArray<TBufferItem> iBuffers;
	};

#endif // LBSNETINTERNALMSGFIFO_H
