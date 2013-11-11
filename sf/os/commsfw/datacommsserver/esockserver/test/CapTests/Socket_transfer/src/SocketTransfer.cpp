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
// RSocket::Transfer() policing test.
// 
//

/**
 @file
 @internalAll PlatSec test code for the ESOCK
*/

#include "SocketTransfer.h"
#include "EsockPlatSecTest.h"
#include <test/es_dummy.h>
#include <test/testexecutelog.h>



CSocketTransferStep::CSocketTransferStep(CEsockPlatSecTestServer *apTestServer /*= NULL*/)
                    :CEsockPlatSecTestStep(apTestServer)
{
    _LIT(KTestStep_Report,"EsockPlatSecStep_SockTransfer");
    SetTestStepName(KTestStep_Report);
}

CSocketTransferStep::~CSocketTransferStep()
{
}


//---------------------------------------------------------------------------

TVerdict CSocketTransferStep::doTestStepL()
{
    TInt nRes = KErrNone;

    //-- connect to the Socket Server
    INFO_PRINTF1(_L("Connecting to the socket server...\n"));
    nRes = iSockServ.Connect();
    TESTL(nRes == KErrNone);
    CleanupClosePushL(iSockServ);

    //-- start the helper child process
    INFO_PRINTF1(_L("Starting the helper process...\n"));

    CHelperProcess procHelper(Logger());
   // CleanupStack::PushL(&procHelper);

    //-- assign security attributes to the helper process
    TCapabilitySet capsSet;
    capsSet.SetEmpty(); // we need it, because the constructor does nothing.
    capsSet.AddCapability(ECapabilityNetworkServices);
    capsSet.AddCapability(ECapabilityNetworkControl);

    procHelper.SetName(KESockTestHelperProcess);
    procHelper.SetCaps(capsSet);

    //-- create the helper process
    nRes = procHelper.Create(TestStepName());
    if(nRes != KErrNone)
    {//-- something went wrong with the helper process creation
        ERR_PRINTF2(_L("Error creating helper process, code:%d...\n"), nRes);
        TESTL(EFalse);
    }

    //-- create messages sender
    CSyncMessageSender *pMsgSender = CSyncMessageSender::NewL(TestStepName(),procHelper.Handle(), &Logger());
    if(!pMsgSender)
    {
        ERR_PRINTF1(_L("Unable to create messages sender object!...\n"));
        TESTL(EFalse);
    }
    CleanupStack::PushL(pMsgSender);

    //-- resume the helper process and wait for Rendezvous
    nRes = procHelper.Start();
    if(nRes != KErrNone)
    {//-- something went wrong with the helper process
        ERR_PRINTF2(_L("Error in the helper process, code:%d...\n"), nRes);
        TESTL(EFalse);
    }

    //-----------------------------  testing, actually ---------------
    TIPMessage  msg;
    RSocket     sock1;


    //-- 0. send the test message to the helper process, no implications.
    msg.iMsgCode = TIPMessage::ETest;
    nRes = pMsgSender->SendMessageSync(msg);
    TESTL(nRes == KErrNone);

    _LIT_SECURITY_POLICY_C2(KProcPolicy1, ECapabilityNetworkServices, ECapabilityNetworkControl);

    //-- 1. Try to transfer socket without enabling this operation.
    INFO_PRINTF1(_L("Try to transfer socket without enabling this operation..."));
    TESTL(CloseOpenSocket(sock1) == KErrNone);
    msg.iMsgCode = TIPMessage::EPullSocket;
    msg.iBuf.Copy(iSocketName);
    nRes = pMsgSender->SendMessageSync(msg); //shall be KErrPermissionDenied
    TESTL(nRes == KErrPermissionDenied);


    //-- 2. enable transfer and cancel it
    INFO_PRINTF1(_L("Testing disabling socket transfer..."));
    TESTL(CloseOpenSocket(sock1) == KErrNone);
    nRes = sock1.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy1().Package());
    TESTL(nRes == KErrNone);

    //-- 2.1 cancel socket transfer operation
    nRes = sock1.SetOpt(KSODisableTransfer, KSOLSocket);
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::EPullSocket;
    msg.iBuf.Copy(iSocketName);

    nRes = pMsgSender->SendMessageSync(msg); //shall be KErrPermissionDenied
    TESTL(nRes == KErrPermissionDenied);

    //--3. testing with a wrong SID
    INFO_PRINTF1(_L("Try to transfer the socket to the process with a wrong SID..."));
    TESTL(CloseOpenSocket(sock1) == KErrNone);

    _LIT_SECURE_ID(KPullingProcessWrongSid,0x1234567); //-- definetely wrong SID
    _LIT_SECURITY_POLICY_S0(KProcPolicyWrongSid, KPullingProcessWrongSid);
    nRes = sock1.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicyWrongSid().Package());
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::EPullSocket;
    msg.iBuf.Copy(iSocketName);

    nRes = pMsgSender->SendMessageSync(msg);
    TESTL(nRes == KErrPermissionDenied);



    //-- 4. testing with correct SID
    INFO_PRINTF1(_L("Transferring the socket to the process with correct SID..."));
    TESTL(CloseOpenSocket(sock1) == KErrNone);

    _LIT_SECURE_ID(KPullingProcessSid,KHelperProcDefaultSID); //-- default SID for the pulling process, supposed to be correct
    _LIT_SECURITY_POLICY_S0(KProcPolicySid, KPullingProcessSid);
    nRes = sock1.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicySid().Package());
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::EPullSocket;
    msg.iBuf.Copy(iSocketName);

    nRes = pMsgSender->SendMessageSync(msg); //shall be KErrNone
    TESTL(nRes == KErrNone);


    //-- 5. testing with wrong capabilities set.
    INFO_PRINTF1(_L("Try to transfer the socket to the process with a wrong capabilities set..."));
    TESTL(CloseOpenSocket(sock1) == KErrNone);

    //-- helper process doesn't have ECapabilityProtServ
    _LIT_SECURITY_POLICY_C3(KProcWrongPolicy, ECapabilityNetworkServices, ECapabilityNetworkControl, ECapabilityProtServ );
    nRes = sock1.SetOpt(KSOEnableTransfer, KSOLSocket, KProcWrongPolicy().Package());
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::EPullSocket;
    msg.iBuf.Copy(iSocketName);

    nRes = pMsgSender->SendMessageSync(msg);
    TESTL(nRes == KErrPermissionDenied);


    //-- 6. testing with correct capabilities set
    INFO_PRINTF1(_L("Transferiing the socket to the process with correct capabilities..."));
    TESTL(CloseOpenSocket(sock1) == KErrNone);
    nRes = sock1.SetOpt(KSOEnableTransfer, KSOLSocket, KProcPolicy1().Package());
    TESTL(nRes == KErrNone);

    msg.iMsgCode = TIPMessage::EPullSocket;
    msg.iBuf.Copy(iSocketName);

    nRes = pMsgSender->SendMessageSync(msg); //shall be KErrNone
    TESTL(nRes == KErrNone);


    //-- shutdown the helper process
    msg.iMsgCode = TIPMessage::EShutDownProcess;
    nRes = pMsgSender->SendMessageSync(msg);


    CleanupStack::PopAndDestroy(2);	 //socket server, message sender

    return EPass;
}


/**
Closes and opens a socket on a dummy protocol.
If the socket opened successfully, places its name to the iSocketName
*/
TInt CSocketTransferStep::CloseOpenSocket(RSocket& aSock)
{
    aSock.Close();
    TInt nRes = aSock.Open(iSockServ, KDummyOneName);

    if(nRes != KErrNone)
    {//-- something went wrong with the helper process
        ERR_PRINTF2(_L("CSocketTransferStep::CloseOpenSocket(), Error opening socket! code:%d...\n"), nRes);
    }
    else
    {//-- get the socket name if everything is ok
        aSock.Name(iSocketName);
    }

    return nRes;
}







































