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


#ifndef     __CONN_CLONING_TEST_H__
#define     __CONN_CLONING_TEST_H__


#include <test/testexecutestepbase.h>
#include "TE_EsockPlatSecStep.h"
#include "SocketTransfer.h"


//---------------------------------------------------------------------------------------------------------

/**
	base class for RConnection cloning policing test steps
*/
class CConnCloningStepBase : public CEsockPlatSecTestStep
{
    public:
       ~CConnCloningStepBase();
        
    protected:    
        
        CConnCloningStepBase(CEsockPlatSecTestServer *apTestServer = NULL);

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
	RConnection cloning policing test step 1
*/
class CConnCloningStep1 : public CConnCloningStepBase
    {
    public:
        CConnCloningStep1(CEsockPlatSecTestServer *apTestServer = NULL);
       ~CConnCloningStep1() {}
        
        virtual TVerdict doTestStepL();	
    };

_LIT(KTestStep_ConnCloning1,"ConnCloning1");


/**
	RConnection cloning policing test step 2
*/
class CConnCloningStep2 : public CConnCloningStepBase
    {
    public:
        CConnCloningStep2(CEsockPlatSecTestServer *apTestServer = NULL);
       ~CConnCloningStep2() {}
        
        virtual TVerdict doTestStepL();	
    };

_LIT(KTestStep_ConnCloning2,"ConnCloning2");


#endif // __CONN_CLONING_TEST_H__






