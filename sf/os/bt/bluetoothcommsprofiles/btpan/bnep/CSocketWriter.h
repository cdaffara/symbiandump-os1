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

/**
 @file
 @internalComponent
*/

#ifndef __CSOCKETWRITER_H
#define __CSOCKETWRITER_H 

#include <e32base.h>
#include <e32std.h>
#include <e32def.h>
#include <es_mbuf.h>
#include <comms-infras/eintsock.h>

class CBnepLink;


const TUint8 KMaxSocketWriterQueueSize = 5;

/**
   @file
   @internalComponent
   Active object to handle socket writes.
*/
NONSHARABLE_CLASS(CSocketWriter) : public CActive  
    {

public:
    ~CSocketWriter();
    static CSocketWriter* NewL (RInternalSocket& aSocket, CBnepLink& aOwner);
    TInt Write (RMBufChain& aChain);

private:
    CSocketWriter (RInternalSocket& aSocket, CBnepLink& aOwner);
    void ConstructL ();
    void RunL ();
    void DoCancel ();

private: 

    RInternalSocket& iSocket;
   	CBnepLink& iBnepLink;
	RMBufPktQ iQueue;
    TUint8 iQueueSize;
    RMBufChain iPendingWriteData;
    };
#endif
