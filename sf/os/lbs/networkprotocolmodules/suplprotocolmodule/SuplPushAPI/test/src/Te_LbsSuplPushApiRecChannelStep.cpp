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
// These are the unit tests for the CLbsSuplPushRecChannel class
// 
//

#include <e32property.h>

#include <lbs/lbssuplpush.h>
#include <centralrepository.h>

#include "lbssuplpushprops.h"
#include "lbsrootcenrepdefs.h"

#include "Te_LbsSuplPushApiReceiverStep.h"
#include "Te_LbsSuplPushApiRecChannelStep.h"
#include "Te_LbsSuplErrorPattern.h"
#include "Te_LbsSuplPushRProperty.h"
#include "Te_LbsSuplCommon.h"

/** The test message example 1 */
_LIT8(KTestMessage0, "0000");
const TInt KLengthMessage0 = 4;
/** The test message example 2 */
_LIT8(KTestMessage1, "11111111111111111111111");
const TInt KLengthMessage1 = 23;

/**
The stub observer passed in on the creaton of the CLbsSuplPush class.

@see CLbsSuplPush
@see MLbsSuplPushObserver
*/
class TLbsSuplPushObserver : public MLbsSuplPushObserver
	{
public:
	virtual void OnSuplInitComplete(TLbsSuplPushChannel aChannel, TLbsSuplPushRequestId aReqId,
			TInt aError, TInt aReserved);
	};

/**
The stub override of the base class pure virtual function.

@see MLbsSuplPushObserver::OnSuplInitComplete
*/
void TLbsSuplPushObserver::OnSuplInitComplete(TLbsSuplPushChannel /*aChannel*/, TLbsSuplPushRequestId /*aReqId*/,
			TInt /*aError*/, TInt /*aReserved*/)	
	{
	//Intentionally left blank
	}


/**
 * Constructor. It must	call the SetTestStepName as the controlling
 * framework uses the test step name immediately following construction to set
 * up the step's unique logging ID. 
 */
CTe_LbsSuplPushApiRecChannelStep::CTe_LbsSuplPushApiRecChannelStep()
	{
	SetTestStepName(KLbsSuplPushApiRecChannelStep);
	}


/**
 * Destructor.
 */
CTe_LbsSuplPushApiRecChannelStep::~CTe_LbsSuplPushApiRecChannelStep()
	{
	delete iScheduler;
	}

/**
Overrides the base class pure virtual function. Contains the initialization code required for 
the test step. 

Creates and installs the CActiveScheduler object. The error pattern is also opened here.
 
@return - The result of the preamble.

@leave If a error happens, it leaves with one of the system error codes.

@see CTestStep::doTestStepPreambleL
*/
TVerdict CTe_LbsSuplPushApiRecChannelStep::doTestStepPreambleL()
	{
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	RTe_LbsSuplErrorPattern::OpenL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/**
Overrides the base class pure virtual function. Contains the test code to run. 

This test step runs various successfull and error scenarios while receiving the message. 
 
@return - The result of the test 

@leave If a error happens, it leaves with one of the system error codes.

@see CTestStep::doTestStepL
*/
TVerdict CTe_LbsSuplPushApiRecChannelStep::doTestStepL()
	{
	__UHEAP_MARK;
	

	// build the test messages
	TBuf8<KLengthMessage0> testMsg0;
	testMsg0.Copy(KTestMessage0);
	testMsg0[0] = (TUint8)KLengthMessage0 >> 8; // Set MSB
	testMsg0[1] = (TUint8)KLengthMessage0;  	 // Set LSB

	TBuf8<KLengthMessage1> testMsg1;
	testMsg1.Copy(KTestMessage1);
	testMsg1[0] = (TUint8)KLengthMessage1 >> 8; // Set MSB
	testMsg1[1] = (TUint8)KLengthMessage1;  	 // Set LSB

	CTe_LbsSuplPushRecChannel::EnableAsserts();
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TInt category;
	CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
	User::LeaveIfError(rep->Get(KSuplPushAPIKey, category));
	CleanupStack::PopAndDestroy(rep);
				
	iChannel = CLbsSuplPushRecChannel::NewL(ELbsSuplPushChannelSMS, iRecObserver, 
			TSecureId(category));
	
	CTe_LbsSuplPushRecChannel::RunL(*iChannel, KErrGeneral);
	CheckNoMessageL();
	//Send the message and check that the channel processes it correctly
	INFO_PRINTF1(_L("--- Send the message and check that the channel processes it correctly"));
	INFO_PRINTF1(_L("--- SendReceiveMessageL(testMsg0);"));
	SendReceiveMessageL(testMsg0);
	
	//set the INIT property to the zero value	
	User::LeaveIfError(RProperty::Set(TSecureId(category), KLbsSuplPushSmsInitKey, KNullDesC8));
	CheckNoMessageL();
	//Send the message and check that the channel processes it correctly
	INFO_PRINTF1(_L("--- Set the INIT property to the zero value"));
	INFO_PRINTF1(_L("--- SendReceiveMessageL(testMsg1);"));
	SendReceiveMessageL(testMsg1);
	
	
	
	//set the INIT property to the corrupted value - negative length
	TPckgBuf<TInt> length = -1;
	User::LeaveIfError(RProperty::Set(TSecureId(category), KLbsSuplPushSmsInitKey, length));
	CTe_LbsSuplPushRecChannel::DisableAsserts();	
	CheckNoMessageL();
	CTe_LbsSuplPushRecChannel::EnableAsserts();
	//Send the message and check that the channel processes it correctly
	INFO_PRINTF1(_L("--- Set the INIT property to the corrupted value - negative length;"));
	INFO_PRINTF1(_L("--- SendReceiveMessageL(testMsg0);"));
	SendReceiveMessageL(testMsg0);
	
	
	
	//set the INIT property to the corrupted value - negative length + some data
	TBuf8<16> data = length;
	data.Append(testMsg0);
	User::LeaveIfError(RProperty::Set(TSecureId(category), KLbsSuplPushSmsInitKey, data));
	CTe_LbsSuplPushRecChannel::DisableAsserts();	
	CheckNoMessageL();
	CTe_LbsSuplPushRecChannel::EnableAsserts();
	//Send the message and check that the channel processes it correctly
	INFO_PRINTF1(_L("--- Set the INIT property to the corrupted value - negative length + some data"));
	INFO_PRINTF1(_L("--- SendReceiveMessageL(testMsg1);"));
	SendReceiveMessageL(testMsg1);
	
	
	
	//set the INIT property to the corrupted value - data length > length
	data.Zero();
	length = 4;
	data.Append(length);
	data.Append(length);
	data.Append(testMsg0);
	User::LeaveIfError(RProperty::Set(TSecureId(category), KLbsSuplPushSmsInitKey, data));
	CTe_LbsSuplPushRecChannel::DisableAsserts();	
	CheckNoMessageL();
	CTe_LbsSuplPushRecChannel::EnableAsserts();
	//Send the message and check that the channel processes it correctly
	INFO_PRINTF1(_L("--- Set the INIT property to the corrupted value - data length > length"));
	INFO_PRINTF1(_L("--- SendReceiveMessageL(testMsg0);"));
	SendReceiveMessageL(testMsg0);
	
	//set the INIT property to the corrupted value - data length < length
	data.Zero();
	length = 4;
	data.Append(length);
	data.Append(length);
	User::LeaveIfError(RProperty::Set(TSecureId(category), KLbsSuplPushSmsInitKey, data));
	CTe_LbsSuplPushRecChannel::DisableAsserts();	
	CheckNoMessageL();
	CTe_LbsSuplPushRecChannel::EnableAsserts();
	//Send the message and check that the channel processes it correctly
	INFO_PRINTF1(_L("--- Set the INIT property to the corrupted value - data length < length"));
	INFO_PRINTF1(_L("--- SendReceiveMessageL(testMsg1);"));
	SendReceiveMessageL(testMsg1);
	
	
	//set the INIT property to the corrupted value - reqId<0
	INFO_PRINTF1(_L("--- Set the INIT property to the corrupted value - reqId<0"));
	INFO_PRINTF1(_L("--- SendMessageL(category, testMsg0, -1, 0);"));
	SendMessageL(category, testMsg0, -1, 0);
	CTe_LbsSuplPushRecChannel::DisableAsserts();	
	CheckNoMessageL();
	CTe_LbsSuplPushRecChannel::EnableAsserts();
	
	//Send the message and check that the channel processes it correctly
	INFO_PRINTF1(_L("--- Send the message and check that the channel processes it correctly"));
	INFO_PRINTF1(_L("--- SendReceiveMessageL(testMsg1);"));
	SendReceiveMessageL(testMsg1);
	
	//Message already sent, ackId = reqId
	INFO_PRINTF1(_L("--- Message already sent, ackId = reqId"));
	INFO_PRINTF1(_L("--- SendMessageL(category, testMsg0, 1, 1);"));	
	SendMessageL(category, testMsg0, 1, 1);
	CTe_LbsSuplPushRecChannel::DisableAsserts();	
	CheckNoMessageL();
	CTe_LbsSuplPushRecChannel::EnableAsserts();
	//Send the message and check that the channel processes it correctly
	INFO_PRINTF1(_L("--- Send the message and check that the channel processes it correctly"));
	INFO_PRINTF1(_L("--- SendReceiveMessageL(testMsg1);"));
	SendReceiveMessageL(testMsg1);
	
	
	//set the ACK property to the corrupted value - ackId<0
	INFO_PRINTF1(_L("--- Set the ACK property to the corrupted value - ackId<0"));
	INFO_PRINTF1(_L("--- SendMessageL(category, testMsg0, 1, -1);"));
	SendMessageL(category, testMsg0, 1, -1);
	CTe_LbsSuplPushRecChannel::DisableAsserts();	
	CheckNoMessageL();
	CTe_LbsSuplPushRecChannel::EnableAsserts();
	//Send the message and check that the channel processes it correctly
	INFO_PRINTF1(_L("--- Send the message and check that the channel processes it correctly"));
	INFO_PRINTF1(_L("--- SendReceiveMessageL(testMsg1);"));
	SendReceiveMessageL(testMsg1);
	
	
	//Test RProperty::Get method errors
	INFO_PRINTF1(_L("--- Test RProperty::Get method errors"));
	INFO_PRINTF1(_L("--- SendMessageL(category, testMsg0, 2, 1);"));
	SendMessageL(category, testMsg0, 2, 1);
	CTe_LbsSuplPushRecChannel::DisableAsserts();	
	TInt count=1;
	do	{
		RTe_LbsSuplErrorPattern::Reset();
		RTe_LbsSuplErrorPattern::AppendNextErrorL(Te_LbsSuplErrorId::ERProperty_Get,
				count, KErrGeneral, EFalse);

		TRAPD(err, CheckNoMessageL());
		if(RTe_LbsSuplErrorPattern::ErrorReached()==1)
			{
			User::LeaveIfError(err);
			}
		else
			{
			if(err!=EFail || !iRecObserver.HasNewMessage() || 
					iRecObserver.Channel()!=ELbsSuplPushChannelSMS || 
					iRecObserver.ReqId()!=2 || iRecObserver.Message()!=testMsg0)
				{
				INFO_PRINTF1(_L("--- Error - Logic check failed"));
				INFO_PRINTF3(_L("---         err was %d, expected %d (EFail)"),iRecObserver.HasNewMessage(),EFail);
				INFO_PRINTF3(_L("---         iRecObserver.HasNewMessage() was %d, expected %d (ETrue)"),iRecObserver.HasNewMessage(),ETrue);
				INFO_PRINTF3(_L("---         iRecObserver.Channel() was %d, expected %d (ELbsSuplPushChannelSMS)"),iRecObserver.Channel(),ELbsSuplPushChannelSMS);
				INFO_PRINTF3(_L("---         iRecObserver.ReqId() was %d, expected %d"),iRecObserver.ReqId(),2);
				if(iRecObserver.Message()!=testMsg0)
					{
					INFO_PRINTF1(_L("---         iRecObserver.Message() was not equal to 'aMessage'"));
					}
				else
					{
					INFO_PRINTF1(_L("---         iRecObserver.Message() was equal to 'aMessage'"));
					}
				User::Leave(EFail);
				}
			
			}
		count++;
		}
	while(RTe_LbsSuplErrorPattern::ErrorReached()==1);	
	
	RTe_LbsSuplErrorPattern::Reset();
	CTe_LbsSuplPushRecChannel::EnableAsserts();	
	
	//Send the message and check that the channel processes it correctly
	SendReceiveMessageL(testMsg0);
	
	
	
	//Test memory allocation errors
	
	User::__DbgSetAllocFail(EFalse ,RAllocator::EFailNext, 1);
	
	TInt* marker = new TInt(0);
	delete marker;
			
	if(!marker) //The OS is in the udeb mode
		{
		marker = new TInt(0);
		delete marker;
		
		for(TInt i=1; marker; i++)
			{
			SendMessageL(category, testMsg0, 2, 1);
			
			User::__DbgSetAllocFail(EFalse ,RAllocator::EFailNext, i);
			
			CTe_LbsSuplPushRecChannel::DisableAsserts();	
			TRAPD(err, CheckNoMessageL());
			CTe_LbsSuplPushRecChannel::EnableAsserts();	
			marker = new TInt(0);
			delete marker;
		
			if(marker) //memory allocation failure occured
				{
				if(err==EFail) //we managed to receive a msg despite the alloc failure
					{
					//Check tah the msg is correct
					if(!iRecObserver.HasNewMessage() || iRecObserver.Channel()!=ELbsSuplPushChannelSMS || 
							iRecObserver.ReqId()!=2 || iRecObserver.Message()!=testMsg0)
						{
						INFO_PRINTF1(_L("--- Error - Logic check failed"));
						INFO_PRINTF3(_L("---         iRecObserver.HasNewMessage() was %d, expected %d (ETrue)"),iRecObserver.HasNewMessage(),ETrue);
						INFO_PRINTF3(_L("---         iRecObserver.Channel() was %d, expected %d (ELbsSuplPushChannelSMS)"),iRecObserver.Channel(),ELbsSuplPushChannelSMS);
						INFO_PRINTF3(_L("---         iRecObserver.ReqId() was %d, expected %d"),iRecObserver.ReqId(),2);
						if(iRecObserver.Message()!=testMsg0)
							{
							INFO_PRINTF1(_L("---         iRecObserver.Message() was not equal to 'aMessage'"));
							}
						else
							{
							INFO_PRINTF1(_L("---         iRecObserver.Message() was equal to 'aMessage'"));
							}
						User::Leave(EFail);
						}
					}
				}
			else
				{
				if(err!=EFail || !iRecObserver.HasNewMessage() || 
					iRecObserver.Channel()!=ELbsSuplPushChannelSMS || 
					iRecObserver.ReqId()!=2 || iRecObserver.Message()!=testMsg0)
					{
					INFO_PRINTF1(_L("--- Error - Logic check failed"));
					INFO_PRINTF3(_L("---         err was %d, expected %d (EFail)"),iRecObserver.HasNewMessage(),EFail);
					INFO_PRINTF3(_L("---         iRecObserver.HasNewMessage() was %d, expected %d (ETrue)"),iRecObserver.HasNewMessage(),ETrue);
					INFO_PRINTF3(_L("---         iRecObserver.Channel() was %d, expected %d (ELbsSuplPushChannelSMS)"),iRecObserver.Channel(),ELbsSuplPushChannelSMS);
					INFO_PRINTF3(_L("---         iRecObserver.ReqId() was %d, expected %d"),iRecObserver.ReqId(),2);
					if(iRecObserver.Message()!=testMsg0)
					{
						INFO_PRINTF1(_L("---         iRecObserver.Message() was not equal to 'aMessage'"));
						}
					else
						{
						INFO_PRINTF1(_L("---         iRecObserver.Message() was equal to 'aMessage'"));
						}
					User::Leave(EFail);
					}
				}
			}
		}
	//Send the message and check that the channel processes it correctly
	SendReceiveMessageL(testMsg0);
	
	
	delete iChannel;
	iChannel = 0;
	__UHEAP_MARKEND;
	return TestStepResult();
	}


/**
Override of the base class virtual function. Contains the cleanup code to release 
the test step resources. 

The error pattern is closed here.
 
@return - The result of the postamble.

@see CTestStep::doTestStepPostambleL
*/
TVerdict CTe_LbsSuplPushApiRecChannelStep::doTestStepPostambleL()
	{
	RTe_LbsSuplErrorPattern::Close();
	return TestStepResult();
	}

/**
Synchronously sends and receives a message.

@param aMessage [In] A message to be sent and received.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushApiRecChannelStep::SendReceiveMessageL(const TDesC8& aMessage)
	{
	TLbsSuplPushObserver suplPushObserver;
	CLbsSuplPush* suplPush = CLbsSuplPush::NewL(ELbsSuplPushChannelSMS, suplPushObserver);
	CleanupStack::PushL(suplPush);
	TLbsSuplPushRequestId reqId;
	

	suplPush->SuplInit(reqId, aMessage, 0);
	iRecObserver.Reset();
	CTe_LbsSuplPushRecChannel::RunL(*iChannel, KErrNone);
	
	
	if(!iRecObserver.HasNewMessage() || iRecObserver.Channel()!=ELbsSuplPushChannelSMS || 
			iRecObserver.ReqId()!=reqId || iRecObserver.Message()!= aMessage)
		{
		INFO_PRINTF1(_L("--- Error - Logic check failed"));
		INFO_PRINTF3(_L("---         iRecObserver.HasNewMessage() was %d, expected %d (ETrue)"),iRecObserver.HasNewMessage(),ETrue);
		INFO_PRINTF3(_L("---         iRecObserver.Channel() was %d, expected %d (ELbsSuplPushChannelSMS)"),iRecObserver.Channel(),ELbsSuplPushChannelSMS);
		INFO_PRINTF3(_L("---         iRecObserver.ReqId() was %d, expected %d"),iRecObserver.ReqId(),reqId);
		if(iRecObserver.Message()!=aMessage)
			{
			INFO_PRINTF1(_L("---         iRecObserver.Message() was not equal to 'aMessage'"));
			}
		else
			{
			INFO_PRINTF1(_L("---         iRecObserver.Message() was equal to 'aMessage'"));
			}
		
		User::Leave(EFail);
		}
	CleanupStack::PopAndDestroy(suplPush);
	}

/**
Checks that there is no new message arrived.

@leave If there is a new message, it leaves with EFail code. If a error happens, it leaves with 
one of the system error codes.
*/
void CTe_LbsSuplPushApiRecChannelStep::CheckNoMessageL()
	{
	iRecObserver.Reset();
	CTe_LbsSuplPushRecChannel::RunL(*iChannel, KErrNone);
	if(iRecObserver.HasNewMessage())
		{
		User::Leave(EFail);
		}
	}

/**
Sets the INIT and ACK properties to the values required by the scenario.

@param aMessage [In] A message to be written to the INIT property.
@param aReqId [In] A request id to be written to the INIT property.
@param aAckId [In] An ack id to be written to the ACK property.

@leave If a error happens, it leaves with one of the system error codes.
*/
void CTe_LbsSuplPushApiRecChannelStep::SendMessageL(TInt aCategory, const TDesC8& aMessage, TInt aReqId, TInt aAckId)
	{
	TBuf8<16> data;
	TPckgBuf<TInt> ival = aMessage.Length();
	data.Append(ival);
	ival() = aReqId;
	data.Append(ival);
	data.Append(aMessage);
	User::LeaveIfError(RProperty::Set(TSecureId(aCategory), KLbsSuplPushSmsInitKey, data));
	User::LeaveIfError(RProperty::Set(TSecureId(aCategory), KLbsSuplPushSmsAckKey, aAckId));
	}


