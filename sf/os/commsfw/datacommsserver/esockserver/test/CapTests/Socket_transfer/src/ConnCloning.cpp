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
//

/**
 @file
 @internalAll PlatSec test code for the ESOCK
*/

#include <nifman.h>
#include "ConnCloning.h"

#include <test/testexecutelog.h>


//---------------------------------------------------------------------------


CConnCloningStepBase::CConnCloningStepBase(CEsockPlatSecTestServer *apTestServer /*= NULL*/)
                     :CEsockPlatSecTestStep(apTestServer), iProcHelper(Logger())
{
    ipMsgSender = NULL;
}


/**
Close, Open and start connection. Also places connection name to the iConnName

@param  aConn RConnection object to work with.
*/
void CConnCloningStepBase::CloseOpenStartConnectionL(RConnection &aConn)
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
void CConnCloningStepBase::SocketServerConnectL(void)
{
    //-- connect to the Socket Server
    INFO_PRINTF1(_L("Connecting to the socket server..."));
    TESTL( iSockServ.Connect() == KErrNone );
}

/**
start the helper child process with given security attributes.

@param  aCapsSet    capabilities set that the helper process will be assigned

*/
void CConnCloningStepBase::HelperProcessStartL(const TCapabilitySet& aCapsSet)
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
void CConnCloningStepBase::HelperProcessStop(void)
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


CConnCloningStepBase::~CConnCloningStepBase()
{
    HelperProcessStop();
    iSockServ.Close();
}

//---------------------------------------------------------------------------

CConnCloningStep1::CConnCloningStep1(CEsockPlatSecTestServer *apTestServer /*= NULL*/)
                  :CConnCloningStepBase(apTestServer)
{
    _LIT(KTestStep_Report,"EsockPlatSecStep_ConnClone1");
    SetTestStepName(KTestStep_Report);
}


TVerdict CConnCloningStep1::doTestStepL()
{
    TInt nRes = KErrNone;

    SocketServerConnectL(); //-- connect to the Socket Server

    //########################### create the helper process with given security attributes

    //-- assign security attributes to the helper process
    //-- grant helper process NetworkServices and NetworkControl capabilities
    TCapabilitySet capsSet;
    capsSet.SetEmpty(); // we need it, because the constructor does nothing.
    capsSet.AddCapability(ECapabilityNetworkServices);
    capsSet.AddCapability(ECapabilityNetworkControl);

    //-- start the helper process
    HelperProcessStartL(capsSet);

    //############################  testing, actually

    TIPMessage  msg;
    RConnection connParent;

    CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    //-- 1. try to clone the connection without enabling this first.
    INFO_PRINTF1(_L("Try to clone the connection without enabling this operation..."));
    msg.iMsgCode = TIPMessage::ECloneConnection;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg); //shall be KErrPermissionDenied
    TESTL(nRes == KErrPermissionDenied);

    _LIT_SECURITY_POLICY_C2(KProcPolicy1, ECapabilityNetworkServices, ECapabilityNetworkControl);

    //-- 2. Enable cloning and cancelling it
    INFO_PRINTF1(_L("Testing disabling connection cloning..."));

    CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    TSecurityPolicyBuf secPolBuf;
    secPolBuf().Set(KProcPolicy1().Package());

    nRes = connParent.Control(KCOLConnection, KCoEnableCloneOpen, secPolBuf);
    TESTL(nRes == KErrNone);

    //-- 2.1 cancel connection cloning.
    nRes = connParent.Control(KCOLConnection, KCoDisableCloneOpen, secPolBuf);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::ECloneConnection;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg); //shall be KErrPermissionDenied
    TESTL(nRes == KErrPermissionDenied);



    //-- 3. testing with a wrong SID
    _LIT_SECURE_ID(KPullingProcessWrongSid,0x1234567); //-- definetely wrong SID
    _LIT_SECURITY_POLICY_S0(KProcPolicyWrongSid, KPullingProcessWrongSid);

    INFO_PRINTF1(_L("Try to clone connection from the process with a wrong SID..."));

    CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    secPolBuf().Set(KProcPolicyWrongSid().Package());
    nRes = connParent.Control(KCOLConnection, KCoEnableCloneOpen,secPolBuf);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::ECloneConnection;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg); //shall be KErrPermissionDenied

    TESTL(nRes == KErrPermissionDenied);



    //-- 4. testing with correct SID
    INFO_PRINTF1(_L("Cloning the connection in the process with correct SID..."));

    _LIT_SECURE_ID(KPullingProcessSid,KHelperProcDefaultSID); //-- default SID for the pulling process, supposed to be correct
    _LIT_SECURITY_POLICY_S0(KProcPolicySid, KPullingProcessSid);

	CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    secPolBuf().Set(KProcPolicySid().Package());
    nRes = connParent.Control(KCOLConnection, KCoEnableCloneOpen,secPolBuf);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::ECloneConnection;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg);
    TESTL(nRes == KErrNone);



    //-- 5. testing with wrong capabilities set.
    INFO_PRINTF1(_L("Try clone the connection in the process with a wrong capabilities set..."));

	//-- helper process doesn't have ECapabilityProtServ
	_LIT_SECURITY_POLICY_C3(KProcWrongPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl, ECapabilityProtServ );

	CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    secPolBuf().Set(KProcWrongPolicy().Package());
    nRes = connParent.Control(KCOLConnection, KCoEnableCloneOpen,secPolBuf);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::ECloneConnection;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg); //shall be KErrPermissionDenied

    TESTL(nRes == KErrPermissionDenied);


    //-- 6. testing with the correct capabilities set
    INFO_PRINTF1(_L("Cloning connection in the process with correct capabilities..."));

	CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    secPolBuf().Set(KProcPolicy1().Package());
    nRes = connParent.Control(KCOLConnection, KCoEnableCloneOpen,secPolBuf);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::ECloneConnection;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg);
    TESTL(nRes == KErrNone);


	//----------------------------------

    INFO_PRINTF1(_L("Closing the connection..."));
    connParent.Close();

    //############################  shutdown the helper process
    HelperProcessStop();


    return EPass;
}



//---------------------------------------------------------------------------

CConnCloningStep2::CConnCloningStep2(CEsockPlatSecTestServer *apTestServer /*= NULL*/)
                 :CConnCloningStepBase(apTestServer)
{
    _LIT(KTestStep_Report,"EsockPlatSecStep_ConnClone2");
    SetTestStepName(KTestStep_Report);
}


TVerdict CConnCloningStep2::doTestStepL()
{
    TInt nRes = KErrNone;

    SocketServerConnectL(); //-- connect to the Socket Server

    //########################### create the helper process with given security attributes

    //-- assign security attributes to the helper process
    //-- grant helper process NetworkControl capability
    TCapabilitySet capsSet;
    capsSet.SetEmpty(); // we need it, because the constructor does nothing.
    capsSet.AddCapability(ECapabilityNetworkControl);

    //-- start the helper process
    HelperProcessStartL(capsSet);


    //############################  testing, actually

    //--- 1. Helper Process now has only ECapabilityNetworkControl.
    //-- Connnection clonning will require ECapabilityNetworkServices ECapabilityNetworkControl so that expected result will be KErrPermissionDenied

    TIPMessage  msg;
    RConnection connParent;

    CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    _LIT_SECURITY_POLICY_C2(KProcPolicy1, ECapabilityNetworkServices, ECapabilityNetworkControl);

    TSecurityPolicyBuf secPolBuf;
    secPolBuf().Set(KProcPolicy1().Package());

    nRes = connParent.Control(KCOLConnection, KCoEnableCloneOpen, secPolBuf);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::ECloneConnection;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg);
    TESTL(nRes == KErrPermissionDenied);

    //############################  shutdown the helper process
    HelperProcessStop();

	//----------------------------------

    //-- assign security attributes to the helper process
    //-- grant helper process ECapabilityNetworkServices capability
    capsSet.SetEmpty(); // we need it, because the constructor does nothing.
    capsSet.AddCapability(ECapabilityNetworkServices);

    //-- start the helper process
    HelperProcessStartL(capsSet);

    //############################  testing, actually
    //--- 1. Helper Process now has only ECapabilityNetworkServices.
    //-- Connnection clonning will require ECapabilityNetworkServices ECapabilityNetworkControl so that expected result will be KErrPermissionDenied

    CloseOpenStartConnectionL(connParent); //-- close, open, start connection and get its name.

    secPolBuf().Set(KProcPolicy1().Package());

    nRes = connParent.Control(KCOLConnection, KCoEnableCloneOpen, secPolBuf);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::ECloneConnection;
    msg.iBuf.Copy(iConnName);
    nRes = ipMsgSender->SendMessageSync(msg);
    TESTL(nRes == KErrPermissionDenied);

    //############################  shutdown the helper process
    HelperProcessStop();

    INFO_PRINTF1(_L("Closing the connection..."));
    connParent.Close();

    return EPass;
}






