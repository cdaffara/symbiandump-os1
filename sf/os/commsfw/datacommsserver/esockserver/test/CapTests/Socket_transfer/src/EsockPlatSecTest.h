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
// Common header file for the ESOCK PlatSec tests
// 
//

/**
 @file
 @internalAll PlatSec test code for the ESOCK
*/


#ifndef     __ESOCK_PLATSEC_TEST_H__
#define     __ESOCK_PLATSEC_TEST_H__

#include <e32property.h>
#include <e32msgqueue.h>
#include <test/testexecutestepbase.h>

_LIT(KSetCapProcName, "SetCap.exe");                        //< SetCap tool process name
_LIT(KESockTestHelperProcess,"EsockPlatSecTestHelper.exe"); //< name of the helper process module
_LIT(KTestMsgQueue,"EsockPlatSecTestMsgQueue");             //< name of the global message queue that is used for communication between processes
_LIT(KTestLogMsgQueue,"EsockPlatSecTest_Log_MsgQueue");     //< name of the global message queue that is used for sending log messages from the helper process to the host

const TUint KHelperProcDefaultSID = 0x102032B9; //< Helper process SID by default

/**
Message that is being sent between host and helper processes
*/
struct TIPMessage
{
    TInt        iMsgCode;   //< Message code, see TRSTMsgCode
    TInt        iIntParam1; //< just integer parameter 1
    TInt        iIntParam2; //< just integer parameter 2
    TBuf<32>    iBuf;       //< string parameter

    TIPMessage()
    {
        iMsgCode = ETest;
        iIntParam1 = 0;
        iIntParam2 = 0;
    }


    /** message codes definition*/
    enum TIPMsgCode
    {
         ETest,             //< no meaning, no parameters used
         EPullSocket,       //< Transfer socket from "donor" host process to another one. iBuf must contain socket name
         ECloneConnection,  //< cloning connection in another process. iBuf must contain connection name.
         ESetParameters,	//< sub connection set parameters. iBuf must contain connection name, iIntParam1 connection type.
         EWaitForConnection,//< wait for connect, iBuf must contain connection name,
         EShutDownProcess   //< this message is sent to finish helper process, no parameters used
    };

};

/***/
const TInt KLogMsglen = 124;    //< maximal log message length. Beware about 4 byte alignment issue for the message queue
const TInt KLogQueueSlots = 10; //< number of slots in a log message queue

class TIPLogMsg
{
public:

    TBuf<KLogMsglen> iLogMsg;
};


/**
This class represents synchronous message sender. @see CSyncMessageHandler
It creates a global message queue (another process must open it) and places there message of type TIPMessage
After that it waits for Rendezvous() from another process.
*/
class CSyncMessageSender : public CBase
{
public:

    static  CSyncMessageSender* NewL(const TDesC &aMsgQueueName, const RProcess& aHelperProcess, CTestExecuteLogger *apLogger=NULL);
    void    ConstructL(const TDesC &aMsgQueueName, const RProcess& aHelperProcess, CTestExecuteLogger *apLogger=NULL);
    virtual ~CSyncMessageSender(void);

    TInt    SendMessageSync(const TIPMessage& aMsg);
    void    CancelSendMessage(void);

protected:
    CSyncMessageSender(void) : CBase() {}

    TInt WaitForMsgComplete(TBool aProcessLog=ETrue);

protected:
    RMsgQueue<TIPMessage>   iMsgQueue;      //< handle to a global message queue used by this and helper proceses
    TRequestStatus          iMsgRqStat;

    RMsgQueue<TIPLogMsg>    iLogMsgQueue;   //< message queue for log messages from the helper process
    TRequestStatus          iLogRqStat;

    RProcess                iHelperProcess; //< handle of the helper process
    CTestExecuteLogger     *ipLogger;      //< pointer to the TEF logger that can log messages from the helper process
};



/**
This class represents synchronous message handler. @see CSyncMessageSender
Messages come from another process and are processed synchronously by this or derived class.
default ProcessMessage() here does nothing and is supposed to be overloaded in derived class to do something useful.

The host process places a message of class "TIPMessage" into the global Message Queue with specified name
and waits for the Rendezvous() call from this process.

This process starts the message waiting loop (StartMessageLoop() call), opens the MessageQueue by name and waits for the message to come.
After processing the message it calls the Rendezvous() signaling the host process that its message has been processed and reporting the result
code in TRequestStatus.
*/
class CSyncMessageHandler : public CBase
{
public:
    static  CSyncMessageHandler* NewL(const TDesC& aMsgQueueName);
    void    ConstructL(const TDesC& aMsgQueueName);
    virtual ~CSyncMessageHandler(void);

	TInt   StartMessageLoop(void);

protected:
    CSyncMessageHandler(void) : CBase() {};

    virtual TInt  ProcessMessage(const TIPMessage& aMsg);
    void    TestL(TInt aResult);
    void    Log(TPtrC aText, ...);

protected:
    RMsgQueue<TIPMessage>   iMsgQueue;    //< handle to a global message queue, created by the host process
    RMsgQueue<TIPLogMsg>    iLogMsgQueue;   //< message queue for log messages from the helper process

    RProcess                iThisProcess; //< represents this process
};




#endif //__ESOCK_PLATSEC_TEST_H__






