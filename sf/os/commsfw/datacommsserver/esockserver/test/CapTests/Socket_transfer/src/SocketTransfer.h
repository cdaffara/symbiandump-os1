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
 @internalAll PlatSec test code for the ESOCK
*/

#ifndef     __SOCKET_TRANSFER_TEST_H__
#define     __SOCKET_TRANSFER_TEST_H__

#include <in_sock.h>

#include <test/testexecutestepbase.h>
#include "TE_EsockPlatSecStep.h"
#include "SocketTransfer.h"

//---------------------------------------------------------------------------------------------------------

_LIT(KTestStep_SocketTransfer,"SocketTransfer");

/**
	RSocket::Transfer() policing test step
*/
class CSocketTransferStep : public CEsockPlatSecTestStep
    {
    public:
        CSocketTransferStep(CEsockPlatSecTestServer *apTestServer = NULL);
       ~CSocketTransferStep();
        
        virtual TVerdict doTestStepL();	
    
    protected:    
        TInt CloseOpenSocket(RSocket& aSock);
        
    protected:
        RSocketServ     iSockServ;
        TName           iSocketName;
    
    };





#endif // __SOCKET_TRANSFER_TEST_H__






