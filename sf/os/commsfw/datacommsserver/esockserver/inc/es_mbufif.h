// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines support for the v1.5 PRT interface. Formerly this was implemented as a separate
// provider class but in Jetstream this can all be done in CServProviderBase.
// 
//

/**
 @file
 @internalComponent  
*/

#ifndef __ES_MBUFIF_H
#define __ES_MBUFIF_H

#include <es_mbuf.h>

namespace ESock
{
			
class MSessionDataNotify;
NONSHARABLE_CLASS(CWaitForMBufs) : public CActive
    {
public:
	enum TAllocationSignal { ECanSend, ENewData } ;
	// construct/destruct
	static CWaitForMBufs* NewL(MSessionDataNotify* aSocket);
	~CWaitForMBufs();

	// requests
	void AllocWaitAndSignalNewData(TUint aSize);
	void AllocWaitAndSignalCanSend(TUint aSize);

	// cancel the requests
	void CancelNewData();
	void CancelCanSend();

private:
	// construct/destruct
	CWaitForMBufs(MSessionDataNotify* aSocket);
	void ConstructL();

	// from CActive
    void RunL();
	void DoCancel();

    MSessionDataNotify* iSocket;
	TBool iSignalCanSend;
	TBool iSignalNewData;
	RMBufAsyncRequest iRequest;
	RMBufChain iMBufChain;
    };
} // namespace ESock

#endif



