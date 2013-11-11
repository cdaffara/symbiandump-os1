// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ConnCloning.cpp
// 
//

/**
 @file
 @internalAll PlatSec test code for the ESOCK
*/

#include <nifman.h>
#include "SubConnection.h"

#include <test/testexecutelog.h>


//---------------------------------------------------------------------------


CSubConnectionStepBase::CSubConnectionStepBase(CEsockPlatSecTestServer *apTestServer /*= NULL*/)
                     :CEsockPlatSecTestStep(apTestServer), iProcHelper(Logger())
{
    ipMsgSender = NULL;
}


/**
Close, Open and start connection. Also places connection name to the iConnName

@param  aConn RConnection object to work with.
*/
void CSubConnectionStepBase::CloseOpenStartConnectionL(RConnection &aConn)
{
    INFO_PRINTF1(_L("Creating and starting a connection..."));

    //-- close connection
    aConn.Close();

    //-- open connection
    TInt nRes = aConn.Open(iSockServ);
    if(nRes != KErrNone)
    {
        ERR_PRINTF2(_L("Can not open the connection! code:%d..."), nRes);
        TESTL(EFalse);
    }

    //-- start the connection
    nRes = aConn.Start();
    if(nRes != KErrNone)
    {
        ERR_PRINTF2(_L("Can not start the connection! code:%d..."), nRes);
        TESTL(EFalse);
    }

	//-- get connection name
	nRes = aConn.Name(iConnName);
    if(nRes != KErrNone)
    {
        ERR_PRINTF2(_L("Can not get connection name! code:%d..."), nRes);
        TESTL(EFalse);
    }

}


/**
Connect to the socket server
*/
void CSubConnectionStepBase::SocketServerConnectL(void)
{
    //-- connect to the Socket Server
    INFO_PRINTF1(_L("Connecting to the socket server..."));
    TESTL( iSockServ.Connect() == KErrNone );
}

/**
start the helper child process with given security attributes.

@param  aCapsSet    capabilities set that the helper process will be assigned

*/
void CSubConnectionStepBase::HelperProcessStartL(const TCapabilitySet& aCapsSet)
{
    TInt nRes = KErrNone;

    INFO_PRINTF1(_L("Starting the helper process..."));

    iProcHelper.SetName(KESockTestHelperProcess);
    iProcHelper.SetCaps(aCapsSet);

    //-- create the helper process
    nRes = iProcHelper.Create(TestStepName());
    if(nRes != KErrNone)
    {//-- something went wrong with the helper process creation
        ERR_PRINTF2(_L("Error creating helper process, code:%d..."), nRes);
        TESTL(EFalse);
    }

    //-- create messages sender
    ipMsgSender = CSyncMessageSender::NewL(TestStepName(), iProcHelper.Handle(), &Logger());
    if(!ipMsgSender)
    {
        ERR_PRINTF1(_L("Unable to create messages sender object!..."));
        TESTL(EFalse);
    }

    //-- resume the helper process and wait for Rendezvous
    nRes = iProcHelper.Start();
    if(nRes != KErrNone)
    {//-- something went wrong with the helper process
        ERR_PRINTF2(_L("Error in the helper process, code:%d..."), nRes);
        TESTL(EFalse);
    }

    //-- send the test message to the helper process, just for fun.
    TIPMessage  msg;

    msg.iMsgCode = TIPMessage::ETest;
    nRes = ipMsgSender->SendMessageSync(msg);
    TESTL(nRes == KErrNone);

}

/**
shutdown the helper process
*/
void CSubConnectionStepBase::HelperProcessStop(void)
{
    INFO_PRINTF1(_L("Shutting down the helper process..."));

    if(!ipMsgSender)
        return;

    TIPMessage  msg;

    msg.iMsgCode = TIPMessage::EShutDownProcess;
    ipMsgSender->SendMessageSync(msg);

    delete ipMsgSender;
    ipMsgSender = NULL;
}


CSubConnectionStepBase::~CSubConnectionStepBase()
{
	HelperProcessStop();
    iSockServ.Close();
}

//---------------------------------------------------------------------------

CSubConnectionStep1::CSubConnectionStep1(CEsockPlatSecTestServer *apTestServer /*= NULL*/)
                  :CSubConnectionStepBase(apTestServer)
{
    _LIT(KTestStep_Report,"EsockPlatSecStep_SubConnection1");
    SetTestStepName(KTestStep_Report);
}


TVerdict CSubConnectionStep1::doTestStepL()
{
    TInt nRes = KErrNone;

    SocketServerConnectL(); //-- connect to the Socket Server

    //########################### create the helper process with given security attributes

    //-- assign security attributes to the helper process
    //-- don't grant helper process any capabilities
    TCapabilitySet capsSet;
    capsSet.SetEmpty(); // we need it, because the constructor does nothing.
    capsSet.AddCapability(ECapabilityNetworkControl);

    //-- start the helper process
    HelperProcessStartL(capsSet);

    //############################  testing, actually

    TIPMessage  msg;
    RConnection connParent;

    CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    _LIT_SECURITY_POLICY_C1(KProcPolicy1,ECapability_None);

    //-- 1. testing EAttachToDefault with the correct capabilities set
    INFO_PRINTF1(_L("EAttachToDefault with correct capabilities..."));

	CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    TSecurityPolicyBuf secPolBuf;
    secPolBuf().Set(KProcPolicy1().Package());
    nRes = connParent.Control(KCOLConnection, KCoEnableCloneOpen,secPolBuf);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::ESetParameters;
    msg.iIntParam1 = RSubConnection::EAttachToDefault;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg);
    TESTL(nRes != KErrPermissionDenied);

    INFO_PRINTF1(_L("WaitForIncoming with correct capabilities..."));
	CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    msg.iMsgCode = TIPMessage::EWaitForConnection;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg);
	TESTL(nRes != KErrPermissionDenied);

	//----------------------------------

    INFO_PRINTF1(_L("Closing the connection..."));
    connParent.Close();

    //############################  shutdown the helper process
    HelperProcessStop();

    return EPass;
}



//---------------------------------------------------------------------------

CSubConnectionStep2::CSubConnectionStep2(CEsockPlatSecTestServer *apTestServer /*= NULL*/)
                  :CSubConnectionStepBase(apTestServer)
{
    _LIT(KTestStep_Report,"EsockPlatSecStep_SubConnection2");
    SetTestStepName(KTestStep_Report);
}

TVerdict CSubConnectionStep2::doTestStepL()
{
    TInt nRes = KErrNone;

    SocketServerConnectL(); //-- connect to the Socket Server

    //########################### create the helper process with given security attributes

    //-- assign security attributes to the helper process
    //-- don't grant helper process any capabilities
    TCapabilitySet capsSet;
    capsSet.SetEmpty(); // we need it, because the constructor does nothing.

    //-- start the helper process
    HelperProcessStartL(capsSet);

    //############################  testing, actually
    //--- 1. Helper Process now has only ECapabilityNetworkControl.
    //-- Connnection clonning will require ECapabilityNetworkServices ECapabilityNetworkControl so that expected result will be KErrPermissionDenied

    TIPMessage  msg;
    RConnection connParent;

    CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    _LIT_SECURITY_POLICY_C1(KProcPolicy1,ECapability_None);
    TSecurityPolicyBuf secPolBuf;
    secPolBuf().Set(KProcPolicy1().Package());

    //-- 1. testing EAttachToDefault with the incorrect capabilities set
    INFO_PRINTF1(_L("EAttachToDefault with incorrect capabilities..."));

	CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.
    nRes = connParent.Control(KCOLConnection, KCoEnableCloneOpen,secPolBuf);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::ESetParameters;
    msg.iIntParam1 = RSubConnection::EAttachToDefault;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg);
	TESTL(nRes == KErrPermissionDenied);

    //-- 2. testing ECreate with the correct capabilities set
    INFO_PRINTF1(_L("ECreateNew with correct capabilities..."));
	CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    nRes = connParent.Control(KCOLConnection, KCoEnableCloneOpen,secPolBuf);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::ESetParameters;
    msg.iIntParam1 = RSubConnection::ECreateNew;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg);
	TESTL(nRes != KErrPermissionDenied);

	//----------------------------------

    INFO_PRINTF1(_L("Closing the connection..."));
    connParent.Close();

    //############################  shutdown the helper process
    HelperProcessStop();


    return EPass;
}







