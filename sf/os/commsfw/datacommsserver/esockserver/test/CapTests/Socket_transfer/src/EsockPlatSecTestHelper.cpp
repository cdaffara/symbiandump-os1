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
// Helper process for the ESOCK PlatSec test.
// Processes messages from the host process (Esock PlatSec test itself). Used for testing ESOCK API policing in the case
// of interprocess communications.
// The first command line argument passed to the process is used as the name of the message queue.
// 
//

/**
 @file
 @internalAll PlatSec test code for the ESOCK
*/

#include <in_sock.h>
#include <nifman.h>
#include <bacline.h>
#include "EsockPlatSecTestHelper.h"


//------------------------------------------------------------------

/**
@param      aMsgQueueName   name of the global message queue to be opened
@return		pointer to the created object
*/
CIPMsgProcessor* CIPMsgProcessor::NewL(const TDesC& aMsgQueueName)
{
    CIPMsgProcessor *self = new(ELeave) CIPMsgProcessor;
    CleanupStack::PushL(self);
    self->ConstructL(aMsgQueueName);
    CleanupStack::Pop();

    return self;
}

/**
Process message from the host process

@param  aMsg message to process.
@return standard error code.
*/
TInt CIPMsgProcessor::ProcessMessage(const TIPMessage& aMsg)
{
    TInt nRes = KErrNone;

    switch(aMsg.iMsgCode)
    {

    //-- just a test message
    case TIPMessage::ETest:
        nRes = KErrNone;
        TRAP(nRes, ProcessTestMessageL(aMsg));
    break;


    //-- pull the socket to this process using RSocket::Transfer()
    case TIPMessage::EPullSocket:

        nRes = KErrNone;
        TRAP(nRes, TestTransferSocketL(aMsg.iBuf));

    break;

    //-- try to clone RConnection from the host process
    case TIPMessage::ECloneConnection:
        nRes = KErrNone;
        TRAP(nRes, TestConnectionCloningL(aMsg.iBuf));
    break;

	//-- try to set SubConnection parameters
	case TIPMessage::ESetParameters:
	    nRes = KErrNone;
        TRAP(nRes, TestSetParametersL(static_cast<RSubConnection::TSubConnType>(aMsg.iIntParam1),aMsg.iBuf));
    break;

    default:
        nRes = KErrNotSupported;
    break;
    }

    return nRes;
}


/**
Process test message

@param aMsg - message to process.
@leave
*/
void CIPMsgProcessor::ProcessTestMessageL(const TIPMessage& aMsg)
{
    (void)aMsg;

    RProcess process;

    //-- fing out our security attributes
    TSecureId sid1 = process.SecureId();
    TVendorId vid1 = process.VendorId();

    //-- find out our capabilities. Note that some capabilities chack can be selectively disabled in epoc.ini
    //-- it looks like the capability is present.
    TUint uCapSet = 0x00;
    for(TInt i=0; i<ECapability_Limit; i++)
    {
        if(process.HasCapability((TCapability)i))
            uCapSet |= (0x0001 << i);
    }

    Log(_L("Test message, SID:%x, VID: %x, Caps: %x"),sid1.iId, vid1.iId, uCapSet);
}



/**
Test RSocket::Transfer().
Tries to transfer the socket from the "donor", host process with PlatSec check.
This process by default has NetworkControl, NetworkServices capabilities and SID 0x102032B9

@param  aSocketName name of the socket being transferred.
@leave

*/
void CIPMsgProcessor::TestTransferSocketL(const TDesC& aSocketName)
{
    TName logBuf;

    logBuf.Copy(_L("CIPMsgProcessor::TestTransferSocketL(), socket name: "));
    logBuf.Append(aSocketName);
    Log(logBuf);

    TInt nRes = KErrNone;

    //-- open socket server
    Log(_L("Connecting to the socket server..."));
    RSocketServ ss;
	nRes = ss.Connect();
    TestL(nRes);
    CleanupClosePushL(ss);

    //-- try to "pull" the socket from the donor process and see what will happen
    Log(_L("Trying to pull the socket..."));
    RSocket sockTrasf;
    nRes = sockTrasf.Transfer(ss, aSocketName);
    TestL(nRes);
    CleanupClosePushL(ss);

    Log(_L("Ok."));

    CleanupStack::PopAndDestroy(2); //ss, sockTrasf
}

/**
Testing RConnection::Open() - cloning a connection from another (host process)
@param  aSocketName name of the connection being cloned
@leave
*/
void  CIPMsgProcessor::TestConnectionCloningL(const TDesC& aConnName)
{
    TName logBuf;

    logBuf.Copy(_L("CIPMsgProcessor::TestConnectionCloningL(), conn name: "));
    logBuf.Append(aConnName);
    Log(logBuf);

    TInt nRes = KErrNone;

    //-- connect to the socket server
    Log(_L("Connecting to the socket server..."));
    RSocketServ ss;
	nRes = ss.Connect();
    TestL(nRes);
    CleanupClosePushL(ss);

    //-- try to clone the connection, opening it by name, obtained from the host process
    Log(_L("Cloning the connection..."));
    TName  connName;
    connName.Copy(aConnName);

    RConnection conn;
    nRes = conn.Open(ss, connName);
    TestL(nRes);
    CleanupClosePushL(conn);

    TUint nVal;
    nRes = conn.EnumerateConnections(nVal);
    TestL(nRes);

    Log(_L("Ok."));

    CleanupStack::PopAndDestroy(2); //ss, conn

}

/**
Testing RSubConnection::SetParameters()
@param  aSocketName name of the connection
@leave
*/
void CIPMsgProcessor::TestSetParametersL(RSubConnection::TSubConnType aConnectionType, const TDesC& aConnName)
{
    TName logBuf;

    logBuf.Copy(_L("CIPMsgProcessor::TestSetParametersL(), conn name: "));
    logBuf.Append(aConnName);
    Log(logBuf);

    TInt nRes = KErrNone;

    //-- connect to the socket server
    Log(_L("Connecting to the socket server..."));
    RSocketServ ss;
	nRes = ss.Connect();
    TestL(nRes);
    CleanupClosePushL(ss);

    //-- try to clone the connection, opening it by name, obtained from the host process
    Log(_L("Cloning the connection..."));
    TName  connName;
    connName.Copy(aConnName);

    RConnection conn;
    nRes = conn.Open(ss, connName);
    TestL(nRes);
    CleanupClosePushL(conn);

    Log(_L("EnumerateConnections..."));
    TUint nVal;
    nRes = conn.EnumerateConnections(nVal);
    TestL(nRes);

    //-- try to get a sub-connection,
    Log(_L("Getting a sub-connection..."));
    RSubConnection subconn;
    nRes = subconn.Open(ss, aConnectionType, conn);
    TestL(nRes);
    CleanupClosePushL(subconn);

    //-- try to set some paramters
    Log(_L("Trying to set sub-connection parameters..."));
    RSubConParameterBundle subconParams;
    CleanupClosePushL(subconParams);

	nRes = subconn.SetParameters(subconParams);
    TestL(nRes);

    Log(_L("Ok."));

    CleanupStack::PopAndDestroy(4); //ss, conn. subconn, rsubconnParams
}

/**
Testing RSubConnection::SetParameters()
@param  aSocketName name of the connection
@leave
*/
void CIPMsgProcessor::TestWaitForSubConnectionL(const TDesC& aConnName)
{
    TName logBuf;

    logBuf.Copy(_L("CIPMsgProcessor::TestWaitForIncoming(), conn name: "));
    logBuf.Append(aConnName);
    Log(logBuf);

    TInt nRes = KErrNone;

    //-- connect to the socket server
    Log(_L("Connecting to the socket server..."));
    RSocketServ ss;
	nRes = ss.Connect();
    TestL(nRes);
    CleanupClosePushL(ss);

    //-- try to clone the connection, opening it by name, obtained from the host process
    Log(_L("Cloning the connection..."));
    TName  connName;
    connName.Copy(aConnName);

    RConnection conn;
    nRes = conn.Open(ss, connName);
    TestL(nRes);
    CleanupClosePushL(conn);

    Log(_L("EnumerateConnections..."));
    TUint nVal;
    nRes = conn.EnumerateConnections(nVal);
    TestL(nRes);

    //-- try to waiting for a sub-connection,
    Log(_L("Waiting for sub-connection..."));
    RSubConnection subconn;
    TRequestStatus status;
    conn.WaitForIncoming(subconn, status);

    //-- try to cancel waiting for a sub-connection
    Log(_L("Cancel waiting for sub-connection..."));
    conn.CancelWaitForIncoming();

    Log(_L("Ok."));

    CleanupStack::PopAndDestroy(4); //ss, conn. subconn, rsubconnParams
}

//------------------------------------------------------------------

//
// Main
//
GLDEF_C TInt E32Main()
    {

	CTrapCleanup* cleanup = CTrapCleanup::New();
    CIPMsgProcessor *pMessageProcessor=NULL;
    CCommandLineArguments *pCommandLineArgs=NULL;

	__UHEAP_MARK;

		TRAPD(nErr, pCommandLineArgs  = CCommandLineArguments::NewL());
		TInt count = pCommandLineArgs->Count();

		if(2<=count)
		{
			//-- create a message handler, the object that will handle control messages from
			//-- the parent process
			TRAP(nErr, pMessageProcessor = CIPMsgProcessor::NewL(pCommandLineArgs->Arg(1)));

			//-- signal parent process that initialization is done.
			RProcess().Rendezvous(nErr);

			if(nErr == KErrNone)
			{//-- initialization was OK, start messages processing
			  nErr = pMessageProcessor->StartMessageLoop();
			}
		}

		delete pMessageProcessor;
		delete pCommandLineArgs;


	__UHEAP_MARKEND;

	delete cleanup;
	return nErr;
    }




















