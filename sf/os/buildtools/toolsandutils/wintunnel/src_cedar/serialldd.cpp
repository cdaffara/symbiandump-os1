// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// wins/specific/serialldd.cpp
// 
//

#include "winscomm.h"
#include <kernel/kern_priv.h>
#include <e32hal.h>
#include <e32uid.h>

_LIT(KLddName,"Comm");


enum TPanic
	{
	ESetConfigWhileRequestPending,
	ESetSignalsSetAndClear,
	EResetBuffers,
	ESetReceiveBufferLength,
	};


inline TUint32 SafeSwap(TUint32 aNewValue, TUint32& aWord)
	{ return (TUint32)NKern::SafeSwap((TAny*)aNewValue, (TAny*&)aWord); }

DECLARE_STANDARD_LDD()
	{
	return new DDeviceComm;
	}


DDeviceComm::DDeviceComm()
	{
	iParseMask = KDeviceAllowAll;
	iUnitsMask = 0xffffffff; // Leave units decision to the PDD
	iVersion = TVersion(KCommsMajorVersionNumber,KCommsMinorVersionNumber,KCommsBuildVersionNumber);
	}

TInt DDeviceComm::Install()
	{
	return(SetName(&KLddName));
	}

void DDeviceComm::GetCaps(TDes8& aDes) const
	{
	TPckgBuf<TCapsDevCommV01> b;
	b().version = TVersion(KCommsMajorVersionNumber,KCommsMinorVersionNumber,KCommsBuildVersionNumber);
	Kern::InfoCopy(aDes,b);
	}

TInt DDeviceComm::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel = new DChannelComm;
	return aChannel?KErrNone:KErrNoMemory;
	}


DChannelComm::DChannelComm()
	:
		iRxCompleteDfc(DChannelComm::CompleteRxDfc,this,2),
		iTxCompleteDfc(DChannelComm::CompleteTxDfc,this,2),
		iRxDataAvailableDfc(DChannelComm::RxDataAvailableDfc,this,2),
		iSigNotifyDfc(DChannelComm::SignalNotifyDfc,this,2),
//		iTurnaroundMinMilliSeconds(0),
//		iTurnaroundTimerRunning(EFalse),
//		iTurnaroundTransmitDelayed(EFalse),
		iTurnaroundTimer(DChannelComm::TurnaroundStartDfc, this),
		iTurnaroundDfc(DChannelComm::TurnaroundTimeout, this, 2)
//		iTurnaroundTxDesPtr(0),
//		iTurnaroundTxDesLength(0)
	{
	iConfig.iRate = EBps9600;
	iConfig.iDataBits = EData8;
	iConfig.iStopBits = EStop1;
	iConfig.iParity = EParityNone;
	iConfig.iHandshake = KConfigObeyCTS;
	iConfig.iParityError = KConfigParityErrorFail;
	iConfig.iFifo = EFifoEnable;
	iConfig.iTerminatorCount = 0;
	iConfig.iXonChar = 0x11;
	iConfig.iXoffChar = 0x13;
	iConfig.iSIREnable = ESIRDisable;

	iTxError = KErrNone;
	iRxError = KErrNone;
	iRxDAError = KErrNone;
	iSignalError = KErrNone;
	iClientDestPtr = 0;
	iClientSignalResultPtr = 0;
	iClient = &Kern::CurrentThread();
	iClient->Open();
	}


DChannelComm::~DChannelComm()
	{
	Kern::SafeClose((DObject*&)iClient, NULL);
	}

void DChannelComm::Shutdown()
	{
	// clean-up...
	if (iStatus == EActive)
		Stop(EStopPwrDown);			// stop PDD

	Complete(EAll, KErrAbort);

	iRxCompleteDfc.Cancel();
	iTxCompleteDfc.Cancel();
	iTurnaroundTimer.Cancel();
	iTurnaroundDfc.Cancel();
	iSigNotifyDfc.Cancel();
	iRxDataAvailableDfc.Cancel();
	}

TInt DChannelComm::TurnaroundSet(TUint aNewTurnaroundMilliSeconds)
	{
	TInt r = KErrNone;
	(void)TurnaroundStopTimer();
	iTurnaroundMinMilliSeconds = aNewTurnaroundMilliSeconds;
	return r;
	}

TBool DChannelComm::TurnaroundStopTimer()
// Stop the timer and DFC
	{
	TInt irq = 0;
	irq = NKern::DisableInterrupts(1);
	TBool result = iTurnaroundTimerRunning;
	if(result)
		{
		iTurnaroundTimerRunning = EFalse;
		iTurnaroundTimer.Cancel();
		iTurnaroundDfc.Cancel();
		}
	NKern::RestoreInterrupts(irq);
	return result;
	}

TInt DChannelComm::TurnaroundClear()
// Clear any old timer
// Called for any change: from T > 0 to T == 0 or (T = t1 > 0) to (T = t2 > 0)
// POLICY: If a write has already been delayed, it will be started immediately (next reschedule)
	{
	TInt r = KErrNone;

	if(TurnaroundStopTimer())
		{
		// if a write is waiting, start a DFC to run it
		TurnaroundStartDfcImplementation(EFalse);
		}

	iTurnaroundMinMilliSeconds = 0;
	return r;
	}

void DChannelComm::TurnaroundStartDfc(TAny* aSelf)
	{
	DChannelComm* self = (DChannelComm*)aSelf;
	self->TurnaroundStartDfcImplementation(ETrue);
	}

void DChannelComm::TurnaroundStartDfcImplementation(TBool inIsr)
	{
	TInt irq = 0;
	if(!inIsr)
		{
		irq = NKern::DisableInterrupts(1);
		}
	iTurnaroundTimerRunning = EFalse;
	if(iTurnaroundTransmitDelayed)
		{
		if(inIsr)
			iTurnaroundDfc.Add();
		else
			{
			NKern::RestoreInterrupts(irq);
			iTurnaroundDfc.Enque();
			}
		return;
		}
	if(!inIsr)
		{
		NKern::RestoreInterrupts(irq);
		}
	}

void DChannelComm::TurnaroundTimeout(TAny* aSelf)
	{
	DChannelComm* self = (DChannelComm*)aSelf;
	self->TurnaroundTimeoutImplementation();
	}

void DChannelComm::TurnaroundTimeoutImplementation(void)
	{
	TInt irq = NKern::DisableInterrupts(1);
	if(iTurnaroundTransmitDelayed)
		{
		iTurnaroundTransmitDelayed = EFalse;	// protected -> prevent reentrant ISR
		NKern::RestoreInterrupts(irq);
		if (iStatus==EClosed)
			{
			iTurnaroundTxDesPtr = 0;
			iTurnaroundTxDesLength = 0;
			Complete(ETx,KErrNotReady);
			return;
			}

		// fail signals checked in the PDD
		InitiateWrite(iTurnaroundTxDesPtr, iTurnaroundTxDesLength);
		iTurnaroundTxDesPtr = 0;
		iTurnaroundTxDesLength = 0;
		}
	else 
		NKern::RestoreInterrupts(irq);
	}

TInt DChannelComm::DoCreate(TInt aUnit, const TDesC8* /*aInfo*/, const TVersion &aVer)
	{
	if(!Kern::CurrentThreadHasCapability(ECapabilityCommDD,__PLATSEC_DIAGNOSTIC_STRING("Checked by ECOMM.LDD (Comm Driver)")))
   		return KErrPermissionDenied;		
	if (!Kern::QueryVersionSupported(TVersion(KCommsMajorVersionNumber,KCommsMinorVersionNumber,KCommsBuildVersionNumber),aVer))
		return KErrNotSupported;

	// set up the correct DFC queue
	SetDfcQ(((DComm*)iPdd)->DfcQ(aUnit));
	iRxCompleteDfc.SetDfcQ(iDfcQ);
	iTxCompleteDfc.SetDfcQ(iDfcQ);
	iRxDataAvailableDfc.SetDfcQ(iDfcQ);
	iSigNotifyDfc.SetDfcQ(iDfcQ);
	iTurnaroundDfc.SetDfcQ(iDfcQ);
	iMsgQ.Receive();

	((DComm *)iPdd)->iLdd = this;

	//setup the initial port configuration
	PddConfigure(iConfig);
	
	return KErrNone;
	}




void DChannelComm::Start()
	{
	if (iStatus != EClosed)
		{
		PddStart();
		iStatus = EActive;
		}
	}




void DChannelComm::HandleMsg(TMessageBase* aMsg)
	{
	TThreadMessage& m = *(TThreadMessage*)aMsg;
	TInt id = m.iValue;
	if (id == (TInt)ECloseMsg)
		{
		Shutdown();
		iStatus = EClosed;
		m.Complete(KErrNone, EFalse);
		return;
		}
	else if (id == KMaxTInt)
		{
		// DoCancel
		DoCancel(m.Int0());
		m.Complete(KErrNone, ETrue);
		return;
		}

	if (id < 0)
		{
		// DoRequest
		TRequestStatus* pS = (TRequestStatus*)m.Ptr0();
		TInt r = DoRequest(~id, pS, m.Ptr1(), m.Ptr2());
		if (r != KErrNone)
			Kern::RequestComplete(iClient, pS, r);
		m.Complete(KErrNone, ETrue);
		}
	else
		{
		// DoControl
		TInt r = DoControl(id, m.Ptr0(), m.Ptr1());
		m.Complete(r, ETrue);
		}
	}


TInt DChannelComm::DoRequest(TInt aReqNo, TRequestStatus* aStatus, TAny* a1, TAny* a2)
	{

	//
	// First check if we have started
	//
	if (iStatus == EOpen)
		{
		Start();
		}

	// Now we can dispatch the request
	TInt r = KErrNone;
	TInt len = 0;
	switch (aReqNo)
		{
		case RBusDevComm::ERequestRead:
			if (a2)
				//get the size of the client data 
				r = Kern::ThreadRawRead(iClient, a2, &len, sizeof(len));
			if (r == KErrNone)
				{
				if (a1)	//doing a read
					{
					iRxStatus = aStatus;
					//start the read
					InitiateRead(a1,len);
					}
				else	//notify read data availiable
					{
					iRxDAStatus = aStatus;
					NotifyReadDataAvailable();
					}
				}
			break;
		
		case RBusDevComm::ERequestWrite:
			{
			if (iStatus == EClosed)
				return KErrNotReady;
			if (!a1)
				a1 = (TAny*)1;
			r = Kern::ThreadRawRead(iClient, a2, &len, sizeof(len));	//get the length of the data to write
			if (r == KErrNone)
				{
				iTxStatus = aStatus;
				TInt irq = NKern::DisableInterrupts(1);
				if(iTurnaroundTimerRunning)
					{
					iTurnaroundTransmitDelayed = ETrue;
					iTurnaroundTxDesPtr = a1;
					iTurnaroundTxDesLength = len;
					NKern::RestoreInterrupts(irq);
					}
				else
					{
					NKern::RestoreInterrupts(irq);
					InitiateWrite(a1, len); //a1 is ptr to data to write (on client side)
					}
				}
			break;
			}

		case RBusDevComm::ERequestNotifySignalChange:
			{
			//a1 has place to put the result
			//a2 has the signal mask
			if (!a1)
				{
				r = KErrArgument;
				break;
				}
			
			//start the signal request
			TInt mask = 0;
			r = Kern::ThreadRawRead(iClient, a2, &mask, sizeof(mask));	//get the signal mask
			if (r == KErrNone)
				{
				iSignalStatus = aStatus;
				InitiateNotifySignals(a1, mask);
				}
			break;
			}

		
		case RBusDevComm::ERequestBreak:
			Complete(aReqNo, KErrNotSupported);
			break;

		default:
			r = KErrNotSupported;
			break;

		}
	return r;
	}

TInt DChannelComm::SetConfig(TCommConfigV01& c)
	{
	iConfig = c;
	PddConfigure(iConfig);
	return KErrNone;
	}

TInt DChannelComm::DoControl(TInt aFunction, TAny* a1, TAny* a2)
	{

	TCommConfigV01 c;
	TInt r = KErrNone;

	switch (aFunction)
		{
		case RBusDevComm::EControlConfig:
			{
			//get the current configuration
			TPtrC8 cfg((const TUint8*)&iConfig, sizeof(iConfig));
			r = Kern::ThreadDesWrite(iClient, a1, cfg, 0, KTruncateToMaxLength, iClient);
			break;
			}

		case RBusDevComm::EControlSetConfig:
			{
			if (AreAnyPending())	
				Kern::PanicCurrentThread(_L("D32COMM"), ESetConfigWhileRequestPending);
			else
				{
				memclr(&c, sizeof(c));
				TPtr8 cfg((TUint8*)&c, 0, sizeof(c));
				r = Kern::ThreadDesRead(iClient, a1, cfg, 0, 0);
				if (r == KErrNone)
					r = SetConfig(c);	//set the new configuration
				}
			break;
			}

		case RBusDevComm::EControlCaps:
			{
			//get capabilities
			TCommCaps2 caps;
			PddCaps(caps);	//call ipdd->Caps
			r = Kern::ThreadDesWrite(iClient, a1, caps, 0, KTruncateToMaxLength, iClient);
			break;
			}

		case RBusDevComm::EControlSignals:
			{
			r = Signals();
			break;
			}

		case RBusDevComm::EControlSetSignals:
			{
//			if (((TUint)a1)&((TUint)a2))	//can't set and clear at same time
//				{
//				Kern::PanicCurrentThread(_L("D32COMM"), ESetSignalsSetAndClear);
//				}
//			else
				{

				SetSignals((TUint)a1, (TUint)a2);
				}
			break;
			}

		case RBusDevComm::EControlQueryReceiveBuffer:
			r = RxCount();
			break;

		case RBusDevComm::EControlResetBuffers:
			if (AreAnyPending())
				Kern::PanicCurrentThread(_L("D32COMM"), EResetBuffers);
			else
				ResetBuffers(ETrue);
			break;

		case RBusDevComm::EControlReceiveBufferLength:
			r = RxBufferSize();
			break;

		case RBusDevComm::EControlSetReceiveBufferLength:
			if (AreAnyPending())
				Kern::PanicCurrentThread(_L("D32COMM"), ESetReceiveBufferLength);
			else
				r = SetRxBufferSize((TInt)a1);
			break;

		case RBusDevComm::EControlMinTurnaroundTime:
			r = iTurnaroundMicroSeconds;			// used saved value
			break;

		case RBusDevComm::EControlSetMinTurnaroundTime:
				{
				if ((TInt)a1<0)
					a1=(TAny*)0;
				iTurnaroundMicroSeconds = (TUint)a1;			// save this
				TUint newTurnaroundMilliSeconds = (TUint)a1/1000;	// convert to ms
				if(newTurnaroundMilliSeconds != iTurnaroundMinMilliSeconds)
					{
					// POLICY: if a new turnaround time is set before the previous running timer has expired and a
					// write request has been queued, the transmission goes ahead immediately
					TurnaroundClear();
					if(newTurnaroundMilliSeconds > 0)
						{
						r = TurnaroundSet(newTurnaroundMilliSeconds);
						}
					}
				}
			break;

		default:
			r = KErrNotSupported;
			}
		return(r);
		}


void DChannelComm::SignalNotifyDfc(TAny* aPtr)
	{
	DChannelComm* pC = (DChannelComm*)aPtr;
	pC->DoSignalNotify();
	}

void DChannelComm::RxDataAvailableDfc(TAny* aPtr)
	{
	DChannelComm* pC = (DChannelComm*)aPtr;
	pC->DoRxDataAvailable();
	}

void DChannelComm::DoRxDataAvailable()
	{
	Complete(ERxDA, iRxDAError);
	iRxDAError = KErrNone;
	}

void DChannelComm::DoSignalNotify()
	{
	//copy the data back to the client
	if (iSignalError == KErrNone)
		iSignalError = Kern::ThreadRawWrite(iClient, iClientSignalResultPtr,&iSignalResult, sizeof(iSignalResult), iClient);
	Complete(ESigChg, iSignalError);
	iSignalError = KErrNone;
	}

void DChannelComm::CompleteTxDfc(TAny* aPtr)
	{
	DChannelComm* pC = (DChannelComm*)aPtr;
	pC->DoCompleteTx();
	}

void DChannelComm::DoCompleteTx()
	{
	Complete(ETx, iTxError);
	iTxError = KErrNone;
	}

void DChannelComm::CompleteRxDfc(TAny* aPtr)
	{
	DChannelComm* pC = (DChannelComm*)aPtr;
	pC->DoCompleteRx();
	}

void DChannelComm::DoCompleteRx()
	{
	if (iRxError == KErrNone)
		{
		//copy the data back to the client
		iRxError = Kern::ThreadDesWrite(iClient, (TDes8*)iClientDestPtr, *RxBuffer(), 0, KChunkShiftBy0, iClient);
		}
	Complete(ERx, iRxError);
	iRxError = KErrNone;
	TInt irq = NKern::DisableInterrupts(1);
	if(iTurnaroundMinMilliSeconds > 0)
		{
		// POLICY: if timer is running from a previous read, stop it and re-start it
		if(iTurnaroundTimerRunning)
			{
			iTurnaroundTimer.Cancel();
			iTurnaroundDfc.Cancel();
			}
		iTurnaroundTimerRunning = ETrue;
		TInt timeout = NKern::TimerTicks(iTurnaroundMinMilliSeconds);
		iTurnaroundTimer.OneShot(timeout);
		}
	NKern::RestoreInterrupts(irq);
	}

void DChannelComm::DoCancel(TInt aMask)
	{
	if (aMask & RBusDevComm::ERequestReadCancel)
		{
		ReadCancel();
		}

	if (aMask & RBusDevComm::ERequestWriteCancel)
		{
		TInt irq = NKern::DisableInterrupts(1);
		if(iTurnaroundTransmitDelayed)
			{
			iTurnaroundTxDesPtr = 0;
			iTurnaroundTxDesLength = 0;
			iTurnaroundTransmitDelayed = EFalse;
			}
		NKern::RestoreInterrupts(irq);

		WriteCancel();
		}

	if (aMask & RBusDevComm::ERequestNotifySignalChangeCancel)
		{
		SignalChangeCancel();
		Complete(ESigChg,KErrCancel);
		}

	}


void DChannelComm::InitiateWrite(TAny *aTxDes, TInt aLength)
	{

//aTxDes has client side data
//aLength has the len
	
	if (!aTxDes)
		{
		Complete(ETx, KErrArgument);
		return;
		}
	// call the pdd to fill its buffer and write the data
	Write(iClient, aTxDes, aLength);
	}

void DChannelComm::InitiateRead(TAny *aRxDes, TInt aLength)
	{

	// Complete zero-length read immediately.  maybe not

//	if (aLength == 0)
//		{
//		Complete(ERx, KErrNone);
//		return;
//		}
	TInt max=Kern::ThreadGetDesMaxLength(iClient,aRxDes);

	if (max < Abs(aLength) || max < 0)
		Complete(ERx, KErrGeneral);
		// do not start the Turnaround timer (invalid Descriptor this read never starts)
	else
		{
		iClientDestPtr = aRxDes;
		Read(iClient, aRxDes, aLength);
		}
	}

void DChannelComm::InitiateNotifySignals(TAny *aSignalResultPtr, TInt aMask)
	{
	//aMask has the mask of signals we require
	//aSignalResultPtr is a pointer to the clients area for the result
	iClientSignalResultPtr = (TUint*)aSignalResultPtr;
	NotifySignals(iClient, aMask);
	}

void DChannelComm::NotifyReadDataAvailable()
	{
	NotifyDataAvailable();
	}


void DChannelComm::Complete(TInt aMask, TInt aReason)
	{
	if (aMask & ERx)
		Kern::RequestComplete(iClient, iRxStatus, aReason);
	if (aMask & ETx)
		Kern::RequestComplete(iClient, iTxStatus, aReason);
	if (aMask & ESigChg)
		Kern::RequestComplete(iClient, iSignalStatus, aReason);
	if (aMask & ERxDA)
		Kern::RequestComplete(iClient, iRxDAStatus, aReason);
	}


