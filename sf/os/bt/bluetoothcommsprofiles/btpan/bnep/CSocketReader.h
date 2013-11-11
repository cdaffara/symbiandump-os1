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


#ifndef __CSOCKETREADER_H
#define __CSOCKETREADER_H 

#include <e32base.h>
#include <comms-infras/eintsock.h>

class CBnepLink;
class RMBufChain;

/**
@file
@internalComponent
Active objects to handle socket reads.
*/



/**
Encapsulate read access to a socket.
*/
NONSHARABLE_CLASS(CSocketReader) : public CActive  
    {
public:
    ~CSocketReader();
    static CSocketReader* NewL (RInternalSocket& aSocket, CBnepLink& aOwner);
    void Read ();

private:
    CSocketReader (RInternalSocket& aSocket, CBnepLink& aOwner);
    void ConstructL ();
    void DoCancel ();
    void RunL ();

private:    
    RInternalSocket& iSocket;
    CBnepLink& iBnepLink;
    RMBufChain iRecvMsg;
    };


#endif
