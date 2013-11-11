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

#include <e32base.h>
#include "EsockPlatSecTest.h"

/**
Construct the CSyncMessageSender object.
@param  aHelperProcess  reference to the helper object that receivbes and processes the messages
@param  apLogger        optional. pointer to the TEF logger to log messages from the helper process

@return reference to the constructed object.
*/
CSyncMessageSender* CSyncMessageSender::NewL(const TDesC &aMsgQueueName, const RProcess& aHelperProcess, CTestExecuteLogger *apLogger/*=NULL*/)
{
    CSyncMessageSender *self = new(ELeave) CSyncMessageSender;
    CleanupStack::PushL(self);
    self->ConstructL(aMsgQueueName,aHelperProcess,apLogger);
    CleanupStack::Pop();

    return self;
}

void CSyncMessageSender::ConstructL(const TDesC &aMsgQueueName, const RProcess& aHelperProcess, CTestExecuteLogger *apLogger/*=NULL*/)
{
    //-- create a message queue for 1 element only - anyway, message processing is synchronous
    User::LeaveIfError(iMsgQueue.CreateGlobal(aMsgQueueName, 1));

    //-- create a message queue for log messages from the helper process
    User::LeaveIfError(iLogMsgQueue.CreateGlobal(KTestLogMsgQueue, KLogQueueSlots));

    iHelperProcess = aHelperProcess;
    ipLogger = apLogger;
}

CSyncMessageSender::~CSyncMessageSender(void)
{
    iMsgQueue.Close();
    iLogMsgQueue.Close();
}

/**
Synchronously send the message to the helper object. Posts the message and waits its completion.

@param  aMsg message to send
@return error code, result of the message processing.
*/
TInt CSyncMessageSender::SendMessageSync(const TIPMessage& aMsg)
{

    iMsgQueue.SendBlocking(aMsg);
    iHelperProcess.Rendezvous(iMsgRqStat);

    //-- wait for the message to be completed in the helper process, optionally processing logs from the helper.
    return WaitForMsgComplete();
}

/**
Wait for message completion by the helper process

@param  aProcessLog if ETrue, log messages from the helper process will be logged by TEF
@return             error code, result of the message processing.
*/
TInt CSyncMessageSender::WaitForMsgComplete(TBool aProcessLog/*=ETrue*/)
{
    if( !aProcessLog )
    {//-- don't need to process messages from the helper process
        User::WaitForRequest(iMsgRqStat);
        return iMsgRqStat.Int();
    }

    //-- wait for message completion processing logs from the helper process
    TName logBuf;

    for(;;)
    {
        iLogMsgQueue.NotifyDataAvailable(iLogRqStat);
        User::WaitForRequest(iMsgRqStat, iLogRqStat);

        if(iLogRqStat.Int() == KRequestPending)
        {//-- iMsgRqStat has been completed, the pessage has been processed
         //-- cancel waiting for the log messages.
            iLogMsgQueue.CancelDataAvailable();
            User::WaitForRequest(iLogRqStat);
            break;
        }
        else
        {//-- log message has been posted to the queue, pull all log messages up and process them
            TIPLogMsg   logMsg;
            TInt        nRes;

            for(;;)
            {
                nRes = iLogMsgQueue.Receive(logMsg);
                if(nRes != KErrNone)
                    break;

                if(ipLogger)
                {//-- logger is available, log the message.
                    logBuf.Copy(_L("\t~helper: "));
                    logBuf.Append(logMsg.iLogMsg);
                    ipLogger->Write(logBuf);
                }
            }//for(;;)
        }
     }//for(;;)


    return iMsgRqStat.Int();
}


/**
cancel the message.
*/
void CSyncMessageSender::CancelSendMessage(void)
{
    TRequestStatus *pRqStat = &iMsgRqStat;
    User::RequestComplete(pRqStat, KErrCancel);
}


//---------------------------------------------------------------------------------------------------

/**
@return		pointer to the created object
*/
CSyncMessageHandler* CSyncMessageHandler::NewL(const TDesC& aMsgQueueName)
{
    CSyncMessageHandler *self = new(ELeave) CSyncMessageHandler;
    CleanupStack::PushL(self);
    self->ConstructL(aMsgQueueName);
    CleanupStack::Pop();

    return self;
}

void CSyncMessageHandler::ConstructL(const TDesC& aMsgQueueName)
{
    iThisProcess = RProcess();
    //-- open a global messages queue that will control this message handler
    User::LeaveIfError(iMsgQueue.OpenGlobal(aMsgQueueName));

    //-- open a global message queue for logging.
    User::LeaveIfError(iLogMsgQueue.OpenGlobal(KTestLogMsgQueue));
}

CSyncMessageHandler::~CSyncMessageHandler(void)
{
    iMsgQueue.Close();
    iLogMsgQueue.Close();
}

/**
Start the message loop.
Wait here for the message from the host process to come, process the message and signal the result to the host process
The message loop finishes on receiving EShutDownProcess message. Usually it leads to this process exit

@return result code of the EShutDownProcess message processor.
*/
TInt   CSyncMessageHandler::StartMessageLoop(void)
{

    TInt nRes = KErrNone;
    TIPMessage msg;

    for(;;)
    {

        //-- wait for the message from the host process
        iMsgQueue.ReceiveBlocking(msg);

        //-- call the message processor function
        nRes = ProcessMessage(msg);

        if(msg.iMsgCode == TIPMessage::EShutDownProcess) //-- request to exit the process
        {
            nRes = KErrNone;
            break;
       	}

        //-- signal the parent process that the message has been completed and pass there
        //-- completion code
        iThisProcess.Rendezvous(nRes);

    } //for(;;)

    return nRes;
}


/**
Default message processor. Does nothing. To be overloaded by a derived class that wishes to to something
useful.

@param  aMsg message from the host process
@return KErrNone
*/
TInt  CSyncMessageHandler::ProcessMessage(const TIPMessage& aMsg)
{
    (void)aMsg;
    return KErrNone;
}


/**
Test the parameter. if it is not KErrNone, leave.

@param  aResult a code to test.
@leave  if aResult is not equal KErrNone
*/
void CSyncMessageHandler::TestL(TInt aResult)
{
    if(aResult != KErrNone)
    {
        Log(_L("CSyncMessageHandler::TestL() failed, code: %d"),aResult);
        User::LeaveIfError(aResult);
    }
}

/**
Places the log message to the log message queue, and the host process can pull them up and place to the test step log.
@see CSyncMessageSender::SendMessageSync
@param aText pointer descriptor to the string.
*/
void CSyncMessageHandler::Log(TPtrC aText, ...)
{
    //-- format the message
    VA_LIST list;
	VA_START(list, aText);

	TIPLogMsg logmsg;
	logmsg.iLogMsg.FormatList(aText, list);

    //-- send the message through the log message queue. Don't care about result code...
    (void) iLogMsgQueue.Send(logmsg);
}







