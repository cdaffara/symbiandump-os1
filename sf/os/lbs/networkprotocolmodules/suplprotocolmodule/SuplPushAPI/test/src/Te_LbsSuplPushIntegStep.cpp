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
// These are the integration unit tests for the CLbsSuplPush, CLbsSuplPushImpl and related classes
// 
//

#include <e32property.h>

#include "lbssuplpushprops.h"

#include "Te_LbsSuplPushIntegStep.h"

/**
Constructor. It sets the test step name used later by the TEF.
*/
CTe_LbsSuplPushIntegStep::CTe_LbsSuplPushIntegStep()
	{
	SetTestStepName(KLbsSuplPushIntegStep);
	}

/**
Overrides the pure virtual CTe_LbsSuplPushBaseStep::ReallyDoTestStepL. The test step code is 
placed here.

@leave If the test step fails, it leaves with either the CTestStep::EFail code or one of 
the system error codes.

@see CTe_LbsSuplPushBaseStep::ReallyDoTestStepL
*/
void CTe_LbsSuplPushIntegStep::ReallyDoTestStepL()
	{
	
	TVersion version = MLbsSuplPushObserver::Version();
	if(version.iMajor!=1 || version.iMinor!=0 || version.iBuild!=0)
		{
		User::Leave(EFail);
		}
	MLbsSuplPushObserver::ExtendedInterface(0,0,0);
	
	iSuplPush = CLbsSuplPush::NewL(iChannel, *this);
	CleanupStack::PushL(iSuplPush);
	iSuplPushRec = CLbsSuplPushRec::NewL(*this);
	CleanupStack::PushL(iSuplPushRec);
	
	//Send/receive - success
	SendMessageL(iMsgPattern++);
	iError = KErrNone;
	iSchedulerWait->Start();
	
	
	//Several messages - id reaches KMaxTInt and switches to 1
	//Set the BUSY prop with the id near to KMaxTInt
	User::LeaveIfError(RProperty::Set(iPropOwnerSecureId, iBusyPropKey, KMaxTInt-5));
	for(TInt i=0;i<10;i++)
		{
		SendMessageL(iMsgPattern++);
		}
	iError = KErrNone;
	iSchedulerWait->Start();
	
	//Timeout error
	CleanupStack::PopAndDestroy(iSuplPushRec);
	iSuplPushRec = 0;
	SendMessageL(iMsgPattern++);
	iError = KErrTimedOut;
	iSchedulerWait->Start();
	
	//Timeout error with 2 messages
	SendMessageL(iMsgPattern++);
	
	User::After(1000000);
	SendMessageL(iMsgPattern++);
	
	iError = KErrTimedOut;
	iSchedulerWait->Start();

	CleanupStack::PopAndDestroy(iSuplPush);
	iSuplPush = 0;
	}

/**
Prepares a new message and sends it calling the CSuplPushImp::SuplInit method.

@param aNum [In] A number used for filling the message with the data.

@leave If a error happens, it leaves with one of the system error codes. 
*/
void CTe_LbsSuplPushIntegStep::SendMessageL(TUint aNum)
	{
	if(aNum>9)
		{
		aNum = aNum%10;
		}
	iMessage.Zero();
	TInt maxLength = iMessage.MaxLength();
	for(TInt i=0;i<maxLength;i++)
		{
		iMessage.AppendNum(aNum);
		}
	User::LeaveIfError(iSuplPush->SuplInit(iReqId, iMessage,0));
	iMsgCount++;
	}

/**
Receives notification when a SUPL INIT message was sent to the LBS sub-system or failed.
Overrides the pure virtual MLbsSuplPushObserver::OnSuplInitComplete.

The implementation here stops the CActiveSchedulerWait object and checks the test result.

@param aReqId    [In] An Id of the request the call-back is related to.
@param aError    [In] KErrNone if successful, KErrTimeout if it was not possible to deliver
                      the request before the timeout period, KErrArgument if the structure 
                      or content of the SUPL INIT message was incorrect. 
                      Any system wide error code otherwise.

@see MLbsSuplPushObserver
*/
void CTe_LbsSuplPushIntegStep::OnSuplInitComplete(TLbsSuplPushChannel /*aChannel*/, 
		TLbsSuplPushRequestId aReqId, TInt aError, TInt /*aReserved*/)
	{
	iMsgCount--;
	if(iMsgCount==0)
		{
		iSchedulerWait->AsyncStop();
		}
	
	if((iMsgCount==0 && aReqId!=iReqId)|| aError!=iError)
		{
		SetTestStepResult(EFail);
		}
	}


/**
Receives notification about an incoming SUPL INIT message. Overrides the 
MLbsSuplPushRecObserver::OnSuplInit.
	
@param aChannel  [In] The channel the call-back is related to.
@param aReqId    [In] An Id of the request the call-back is related to.
@param aMsg      [In] A buffer containing a SUPL INIT message.
	
@see CLbsSuplPushRec
@see MLbsSuplPushRecObserver
*/
void CTe_LbsSuplPushIntegStep::OnSuplInit(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId, TDesC8& aMsg)
	{
	if(aChannel!=iChannel)
		{
		if(iMsgCount==1 && (aReqId!=iReqId || aMsg.Compare(iMessage)!=0))
			{
			SetTestStepResult(EFail);
			}
		}
	}
