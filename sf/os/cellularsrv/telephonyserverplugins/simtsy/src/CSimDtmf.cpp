// Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the functionality required to provide DMTF transmission services.
// 
//

/**
 @file
*/



#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CSimDtmfTraces.h"
#endif

#include <testconfigfileparser.h>
#include "CSimDtmf.h"
#include "CSimPhone.h"
#include "CSimVoiceCall.h"

const TInt KPauseDuration=2;		//< The duration of a "pause" DTMF character.
const TInt KDtmfToneDuration=3;		//< The duration of a standard DTMF character (tone or "pause").

CSimDtmf* CSimDtmf::NewL(CSimPhone* aPhone)
/**
 * Standard two-phase constructor.
 * @param aPhone		The parent phone object.
 * @return CSimDtmf		The new signal strength class.
 */
	{
	CSimDtmf* self=new(ELeave) CSimDtmf(aPhone);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CSimDtmf::CSimDtmf(CSimPhone* aPhone)
		: iPhone(aPhone)
/**
 * Trivial first phase construction.
 * @param aPhone	The parent phone object.
 */
	{
	}

void CSimDtmf::ConstructL()
/**
 * Second phase construction.  Create instances of the necessary heap-based
 * objects and read in the DTMF information from the configuration file.
 *
 * Entries in the configuration file will take the following format:
 * "XXX= <Yyy>, <Zzz>"
 * A number of these entries may be included to create a signal strength profile
 * for the duration of the test.
 */
	{
	iTimer=CSimTimer::NewL(iPhone);
	}

CSimDtmf::~CSimDtmf()
/**
 * Standard destructor.  Destroy the heap-based object owned by this object.
 */
	{
	delete iDtmfData;
	delete iTimer;
	}

TInt CSimDtmf::GetDtmfCaps(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Retrieve DMTF capability information.  This function completes the
 * client's request synchronously.
 *
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the
 *						capability data to be returned.
 * @return TInt			Standard error value.
 */
	{
	TPckg<TUint32>* dtmfCapsPckg=(TPckg<TUint32>*)aPckg1;
	TUint32& dtmfCaps=(*dtmfCapsPckg)();

	dtmfCaps= RMobilePhone::KCapsSendDTMFString |
			  RMobilePhone::KCapsSendDTMFSingleTone;
	iPhone->ReqCompleted(aReqHandle,KErrNone);
	return KErrNone;
	}

TInt CSimDtmf::NotifyDtmfCapsChange(TTsyReqHandle /*aReqHandle*/,TDes8* /*aPckg1*/)
/**
 * Register a client's interest in being notified when the DMTF Caps change.
 * Since the capabilities remain static, this request is not completed.
 *
 * @param aReqHandle	The handle associated with this request.
 * @param aPckg1		The first parameter package.  This is populated with the changed
 *						capability information.
 * @return TInt			Standard error value.
 */
	{
	return KErrNone;
	}

void CSimDtmf::NotifyDtmfCapsChangeCancel(TTsyReqHandle aReqHandle)
/**
 * Cancel a previous request to be notified of a change in DTMF capabilities.
 */
	{
	iPhone->ReqCompleted(aReqHandle,KErrCancel);
	}

TInt CSimDtmf::SendDTMFTones(TTsyReqHandle aReqHandle,TDes* aPckg1)
/**
 * Send a series of DMTF tones.
 *
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the
 *						DTMF tone data to be sent.
 * @return TInt			Standard error value.
 */
	{
	TInt ret=SetDtmfCall();
	if(ret!=KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle,KErrEtelCallNotActive);
		return KErrNone;
		}

	if(aPckg1->Length()==0)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNone);
		return KErrNone;
		}

	iDtmfData=HBufC::New(aPckg1->Length());
	if(iDtmfData==NULL)
		{
		iPhone->ReqCompleted(aReqHandle,KErrNoMemory);
		return KErrNone;
		}
	iDtmfData->Des().Copy(*aPckg1);

	iDtmfString=ETrue;
	iDtmfStringIndex=0;
	iDtmfStringReqPending=aReqHandle;
	ret=ActionEvent(EEventStartDtmfString,(*iDtmfData)[iDtmfStringIndex]);
	if(ret!=KErrNone)
		{
		CompleteDtmfStringTx(ret);
		}
	return KErrNone;
	}

TInt CSimDtmf::StartDTMFTone(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Send a single DMTF tone.
 *
 * @param aReqHandle	The request handle associated with this request.
 * @param aPckg1		The first parameter package.  This will be populated with the
 *						DTMF tone to be sent.
 * @return TInt			Standard error value.
 */
	{
	TInt ret=SetDtmfCall();
	if(ret!=KErrNone)
		{
		iPhone->ReqCompleted(aReqHandle,KErrEtelCallNotActive);
		return KErrNone;
		}

	TPckg<TChar>* tonePckg=(TPckg<TChar>*)aPckg1;
	TChar& tone=(*tonePckg)();

	ret=ActionEvent(EEventStartDtmfTone,tone);
	iPhone->ReqCompleted(aReqHandle,ret);
	return KErrNone;
	}

TInt CSimDtmf::StopDTMFTone(TTsyReqHandle aReqHandle)
/**
 * Stop transmission of a single DMTF tone.
 *
 * @param aReqHandle	The request handle associated with this request.
 * @return TInt			Standard error value.
 */
	{
	__ASSERT_ALWAYS(CheckForActiveVoiceCall(),SimPanic(EIllegalDtmfReq));
	TInt ret=ActionEvent(EEventStopDtmfTone);
	iPhone->ReqCompleted(aReqHandle,ret);
	return KErrNone;
	}

TInt CSimDtmf::NotifyStopInDTMFString(TTsyReqHandle aReqHandle)
/**
 * Register a client's interest in being notified of when a stop tone is reached.
 * @param aReqHandle	The request handle associated with this request.
 * @return TInt			Standard error value.
 */
	{
	__ASSERT_ALWAYS(!iNotifyStopChar,SimPanic(EIllegalDtmfEvent));
	iNotifyStopChar=ETrue;
	iNotifyStopReqHandle=aReqHandle;
	return KErrNone;
	}

void CSimDtmf::NotifyStopInDTMFStringCancel()
/**
 * Cancel a client's interest in being notified of when a stop tone is reach.
 */
	{
	if(iNotifyStopChar)
		{
		iNotifyStopChar=EFalse;
		iPhone->ReqCompleted(iNotifyStopReqHandle,KErrCancel);
		}
	}

TInt CSimDtmf::ContinueDtmfStringSending(TTsyReqHandle aReqHandle,TDes8* aPckg1)
/**
 * Continue transmitting a DTMF String after a wait character has been hit.
 * @param aPckg1		The first request package, which contains an indication of
 *						whether the request should be continued.
 * @param aReqHandle	The request handle associated with this request.
 * @return TInt			Standard error value.
 */
	{
	if(!CheckForActiveVoiceCall())
		{
		iPhone->ReqCompleted(aReqHandle,KErrNotReady);
		return KErrNone;
		}

	TPckg<TBool>* contPckg=(TPckg<TBool>*)aPckg1;
	TBool& cont=(*contPckg)();

	if(cont)
		{
		TInt ret=ActionEvent(EEventContinueDtmf);
		iPhone->ReqCompleted(aReqHandle,ret);
		}
	else
		{
		TInt ret=ActionEvent(EEventTerminateDtmf);
		iPhone->ReqCompleted(aReqHandle,ret);
		}
	return KErrNone;
	}

TInt CSimDtmf::ActionEvent(TEvent aEvent)
/**
 * Shell function for events that don't pass a DTMF tone character.
 * @param aEvent	The event to be actioned.
 * @return TInt		Standard error return.
 */
	{
	const TChar KNullChar(0);
	return ActionEvent(aEvent,KNullChar);
	}

TInt CSimDtmf::ActionEvent(TEvent aEvent,const TChar& aTone)
/**
 * Action a DTMF event.
 * @param aEvent	The event to be actioned.
 * @param aTone		Optionally, a tone associated with the event.
 * @return TInt		Standard error return.
 */
	{
	TInt ret = KErrNone;
	switch(aEvent)
		{
	case EEventStartDtmfString:
		if(iState==ETxTone)
			return KErrInUse;
		ret = ProcessTone(aTone,ETrue);
		return ret;

	case EEventStartDtmfTone:
		if(iState==ETxTone)
			return KErrInUse;
		ret = ProcessTone(aTone,EFalse);
		return ret;

	case EEventTimer:
		__ASSERT_ALWAYS(iState==ETxTone,SimPanic(EIllegalDtmfEvent));
		__ASSERT_ALWAYS(iDtmfString,SimPanic(EIllegalDtmfEvent));
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDTMF_ACTIONEVENT1_1, "Completed sending DTMF Tone");
		iDtmfStringIndex++;
		if(iDtmfStringIndex<iDtmfData->Length())
			ret = ProcessTone((*iDtmfData)[iDtmfStringIndex],ETrue);
		else
			{
			iState=EIdle;
			CompleteDtmfStringTx(KErrNone);
			}
		return ret;

	case EEventStopDtmfTone:
		if(iDtmfString)
			return KErrInUse;
		if(iState!=ETxTone)			// If there's been no StartDtmfTone, then return an error.
			return KErrUnknown;
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDTMF_ACTIONEVENT1_2, "Stopping DTMF Tone");
		iState=EIdle;
		return KErrNone;

	case EEventContinueDtmf:
		if(iState!=EStopped)
			return KErrUnknown;
		__ASSERT_ALWAYS(iDtmfString,SimPanic(EIllegalDtmfEvent));
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDTMF_ACTIONEVENT1_3, "Continuing Transmitting a DTMF string after a wait");
		iDtmfStringIndex++;
		if(iDtmfStringIndex<iDtmfData->Length())
			ret = ProcessTone((*iDtmfData)[iDtmfStringIndex],ETrue);
		else
			{
			iState=EIdle;
			CompleteDtmfStringTx(KErrNone);
			}
		return ret;

	case EEventTerminateDtmf:
		if(iState==EStopped)
			{
			__ASSERT_ALWAYS(iDtmfString,SimPanic(EIllegalDtmfEvent));
			iState=EIdle;
			CompleteDtmfStringTx(KErrAccessDenied);
			return KErrNone;
			}
		else
			return KErrUnknown;
	case EEventCallClosure:
		if(iDtmfString)
			{
			iTimer->Cancel();
			CompleteDtmfStringTx(KErrEtelNoCarrier);
			}
		iState=EIdle;
		return KErrNone;
		}

	SimPanic(EIllegalDtmfEvent);
	return KErrNone;		// Dummy to stop compiler error.
	}

/**
Process a tone, i.e. check the tone's validity and, if necessary, start the timer.

@param aTone The DTMF character to be processed.
@param aStartTimer  A flag indicating whether the timer should be started.
                    The timer is not required for the single DTMF tone transmissions.
@return TInt Standard error return.
*/
TInt CSimDtmf::ProcessTone(const TChar& aTone, TBool aStartTimer)
	{
	const TChar wait('w');
	const TChar pause('p');

	if(aTone==wait)
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDTMF_PROCESSTONE_1, "Starting to perform a DTMF wait; character w");
		iState=EStopped;
		CheckNotification();
		return KErrNone;
		}
	else if(aTone.IsDigit()||(aTone>='A')&&(aTone<='D'))
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDTMF_PROCESSTONE_2, "Starting to send DTMF Tone %u", TUint(aTone));
		iState=ETxTone;
		if(aStartTimer)
			{
			iTimer->Start(KDtmfToneDuration,this);
			}
		return KErrNone;
		}
	else if(aTone==pause)
		{
		if(!aStartTimer)
			{
			return KErrArgument;  // can't tx a single "pause" character
			}
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CSIMDTMF_PROCESSTONE_3, "Starting to perform a DTMF pause; character p");
		iState=ETxTone;
		iTimer->Start(KPauseDuration,this);
		return KErrNone;
		}
	return KErrArgument;			// Illegal DTMF character.
	}

void CSimDtmf::CompleteDtmfStringTx(TInt aStatus)
/**
 * Complete a DTMF string transmission.
 * @param aStatus	Completion value.
 */
	{
	iDtmfString=EFalse;
	delete iDtmfData;
	iDtmfData=NULL;
	iPhone->ReqCompleted(iDtmfStringReqPending,aStatus);
	}

void CSimDtmf::CheckNotification()
/**
 * If pending, complete a DTMF "wait" command notification.
 */
	{
	if(iNotifyStopChar)
		{
		iNotifyStopChar=EFalse;
		iPhone->ReqCompleted(iNotifyStopReqHandle,KErrNone);
		}
	}

TInt CSimDtmf::SetDtmfCall()
/**
 * Set a pointer to this class in the active voice call class, so that the call class
 * can callback if the call becomes inactive.
 * @return TInt		Standard error return.
 */
	{
	CSimVoiceCall* call;
	TInt ret=iPhone->FindActiveVoiceCall(call);
	if(ret!=KErrNone)
		return ret;
	call->SetDtmfSession(this);
	return ret;
	}

TBool CSimDtmf::CheckForActiveVoiceCall()
/**
 * Check the active call DTMF pointer is pointing to this class.
 * @return TBool	ETrue if the call's pointer is correctly set.  EFalse if not.
 */
	{
	CSimVoiceCall* call;
	TInt ret=iPhone->FindActiveVoiceCall(call);
	if(ret!=KErrNone)
		return EFalse;
	if(call->GetDtmfSession()==this)
		return ETrue;
	return EFalse;
	}


void CSimDtmf::TimerCallBack(TInt /*aId*/)
/**
 * The timer callback function.  This function will be called when the timer
 * completes.  It indicates a DTMF tone completion.  So, the action event function
 * is called to advance the class' state.
 *
 * @param aId	This parameter is unused.  It is only required for CSimXxx classes
 *				that have more than one timer instance and need to identify which
 *				timer has expired.
 */
	{
	TInt ret=ActionEvent(EEventTimer);
	__ASSERT_ALWAYS(ret==KErrNone,SimPanic(EIllegalDtmfEvent));
	}

void CSimDtmf::CallClosureCallback()
	{
	TInt ret=ActionEvent(EEventCallClosure);
	__ASSERT_ALWAYS(ret==KErrNone,SimPanic(EIllegalDtmfEvent));
	}

const CTestConfigSection* CSimDtmf::CfgFile()
/**
 * Returns a pointer to the current configuration file section.
 *
 * @return CTestConfigSection	A pointer to the current configuration file section.
 */
	{
	return iPhone->CfgFile();
	}

void CSimDtmf::SendDTMFTonesCancel()
/**
Cancels pending SendDTMFTone
*/
	{
	iState=EIdle;
	if(iTimer->Running())
		iTimer->Cancel();
	CompleteDtmfStringTx(KErrCancel);
	
	}
