// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef     __SUB_CONNECTION_TEST_H__
#define     __SUB_CONNECTION_TEST_H__


#include <test/testexecutestepbase.h>
#include "TE_EsockPlatSecStep.h"
#include "SocketTransfer.h"


//---------------------------------------------------------------------------------------------------------

/**
	base class for RConnection cloning policing test steps
*/
class CSubConnectionStepBase : public CEsockPlatSecTestStep
{
    public:
       ~CSubConnectionStepBase();

    protected:

        CSubConnectionStepBase(CEsockPlatSecTestServer *apTestServer = NULL);

        void SocketServerConnectL(void);

        void CloseOpenStartConnectionL(RConnection &aConn);
        void HelperProcessStartL(const TCapabilitySet& aCapsSet);
        void HelperProcessStop(void);

    protected:
        RSocketServ         iSockServ;
        TName               iConnName;
        CHelperProcess      iProcHelper;
        CSyncMessageSender *ipMsgSender;
};




/**
	RSubConnection SetParameters policing test step 1
*/
class CSubConnectionStep1 : public CSubConnectionStepBase
    {
    public:
        CSubConnectionStep1(CEsockPlatSecTestServer *apTestServer = NULL);
       ~CSubConnectionStep1() {}

        virtual TVerdict doTestStepL();
    };

_LIT(KTestStep_SubConnection1,"SubConnection1");

/**
	RSubConnection SetParameters policing test step 2
*/
class CSubConnectionStep2 : public CSubConnectionStepBase
    {
    public:
        CSubConnectionStep2(CEsockPlatSecTestServer *apTestServer = NULL);
       ~CSubConnectionStep2() {}

        virtual TVerdict doTestStepL();
    };

_LIT(KTestStep_SubConnection2,"SubConnection2");


#endif // __CONN_CLONING_TEST_H__






