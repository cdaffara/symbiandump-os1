// sampler.cpp
// 
// Copyright (c) 1999 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <platform.h>

#include "sampler.h"
//#include <nkern.h>
//#include <kernel.h>
#include <kern_priv.h>
_LIT(KLddName,"topsampler");

const TInt KMinRate=10;
const TInt KMaxRate=1000;
const TInt KRawBufSize = 20000; // Enough for 20 seconds of samples - since max expected refresh rate in UI is 10s we can do everything nice and simply with one buffer

class DDeviceSampler : public DLogicalDevice
	{
public:
	DDeviceSampler();
	virtual TInt Install();
	virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	};

class DProfile : public DLogicalChannel
	{
public:
	DProfile();
	~DProfile();
protected:
	virtual TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);
	virtual void HandleMsg(TMessageBase* aMsg);
private:
	TInt StartSampling(TInt aRate);
	TInt StopSampling();
	TInt Reset();
	//TInt GetErrors(TDes8* aDes);
	TInt ProcessReadRequest();
	void Complete(TInt aResult);
	inline TBool Running()
		{return iTimer.iState!=NTimer::EIdle;}
private:
	static void Sample(TAny*);
	void DoSample();

	
private:
	TUint32 iStartTime;
	TInt iRepeat;
	TInt iPeriod;
	DThread* iClient;
	TRequestStatus* iReqStatus;
	TDes8* iClientDes;		// client des pointer
	TUint32 iBuf1[KRawBufSize]; // First word is the current position in the buffer
	TUint32 iBuf2[KRawBufSize]; // Have two buffers that we can switch between, this avoids us having to stop the sampler timer, or disable interrupts or whatever, when we want to drain the buffer
	TUint32* iCurrentBuf; // Pointer to either iBuf1 or iBuf2

	struct TReport
		{
		TUint iRawBufferErrCounter;
		TUint iCodeSegErrCounter;
		TInt  iReportMask; 
		} iReport;

	NTimer iTimer;
	};

DECLARE_STANDARD_LDD()
	{
	return new DDeviceSampler;
	}

DDeviceSampler::DDeviceSampler()
//
// Constructor
//
	{
	//iParseMask=0;
	//iUnitsMask=0;
	iVersion=TVersion(1,0,0);
	}

TInt DDeviceSampler::Install()
//
// Install the device driver.
//
	{
	TInt r=SetName(&KLddName);
	return r;
	}

void DDeviceSampler::GetCaps(TDes8& /*aDes*/) const
//
// Return the capabilities.
//
	{
	}

TInt DDeviceSampler::Create(DLogicalChannelBase*& aChannel)
//
// Create a channel on the device.
//
	{
	aChannel=new DProfile;
	return aChannel?KErrNone:KErrNoMemory;
	}

DProfile::DProfile()
	:	iTimer(Sample,this)
//
// Constructor
//
	{
	iCurrentBuf = iBuf1;
	iCurrentBuf[0] = 1;
	}

DProfile::~DProfile()
//
// Destructor
//
	{
	iTimer.Cancel();
	Kern::SafeClose((DObject*&)iClient, NULL);
	}

TInt DProfile::DoCreate(TInt /*aUnit*/, const TDesC8* /*anInfo*/, const TVersion& aVer)
//
// Create the channel from the passed info.
//
	{
	if (!Kern::QueryVersionSupported(TVersion(1,0,1),aVer))
		return KErrNotSupported;

	iClient=&Kern::CurrentThread();
	iClient->Open();
	//Kern::SetThreadPriority(24);
	SetDfcQ(Kern::DfcQue0());
	iMsgQ.Receive();
	return KErrNone;
	}

void DProfile::Complete(TInt aResult)
//Completes user request
	{
	DEBUG_PROFILER(Kern::Printf("C");)
	Kern::RequestComplete(iClient,iReqStatus,aResult);
	}

TInt DProfile::StartSampling(TInt aRate)
	{
	DEBUG_PROFILER(Kern::Printf("START");)
	//Activate timer
	aRate=Min(KMaxRate, Max(KMinRate, aRate));
	iPeriod=1000/aRate;

	if (!Running())
		{
		iCurrentBuf = iBuf1;
		iCurrentBuf[0] = 1;
		iTimer.OneShot(iPeriod);
		}
	
	DEBUG_PROFILER(Kern::Printf("START end");)
	return KErrNone;
	}

TInt DProfile::Reset()
	{
//
// Resets the device. It is the first message sent by profiler application.
//	
	if (Running())
		return KErrGeneral;
	
	DEBUG_PROFILER(Kern::Printf("RST %d", aXIPOnly);)

	iTimer.Cancel();
	iPeriod=1;
	iReqStatus=NULL;
	iClientDes=NULL;

	iReport.iRawBufferErrCounter = 0;
	iReport.iCodeSegErrCounter = 0;
	iReport.iReportMask = 0;
	DEBUG_PROFILER(Kern::Printf("RST end");)
	return KErrNone;
	}

TInt DProfile::StopSampling()
//
// Stops sampling
//
	{
	DEBUG_PROFILER(Kern::Printf("STOP");)
	if (Running())
		{
		iTimer.Cancel();
		}
	if (iReqStatus)
		Complete(KErrNone);
	
	DEBUG_PROFILER(Kern::Printf("STOP end");)
	return KErrNone;
	}

TInt DProfile::ProcessReadRequest()
	{
	// Whatever the status of the profiler, return the contents of the current sampling buffer
	DEBUG_PROFILER(Kern::Printf("READ");)
	TInt max=Kern::ThreadGetDesMaxLength(iClient, iClientDes);
	if (max<0)
		return max;
	if (max==0)
		return KErrArgument;

	TUint32* otherBuf = (iCurrentBuf == iBuf1 ? iBuf2 : iBuf1);
	otherBuf[0] = 1; // Reset other buf
	TUint32* bufToWrite = iCurrentBuf;
	iCurrentBuf = otherBuf; // And switch do it, so we can safely mess with bufToWrite without the sampling ISR adding more data under our feet
	TInt numSamples = bufToWrite[0] - 1; // Because bufToWrite[0] is an index and the data actually starts at index 1
	TPtrC8 desToWrite((TUint8*)&bufToWrite[1], (numSamples)*sizeof(TUint32));
	
	TInt r = Kern::ThreadDesWrite(iClient, iClientDes, desToWrite, 0, 0, iClient);
	return r;
	}

void DProfile::HandleMsg(TMessageBase* aMsg)
//
// Client requests
//
	{
	TInt r=KErrNone;
	TThreadMessage& m=*(TThreadMessage*)aMsg;
	//BEGIN TOMSCI commented out this check as it is too strict
	/*
	if (m.Client()!=iClient)
		{
		m.PanicClient(_L("SAMPLER"),EAccessDenied);
		return;
		}
	*/
	//END TOMSCI
	TInt id=m.iValue;
	if (id==(TInt)ECloseMsg)
		{
		DEBUG_PROFILER(Kern::Printf("CLOSE");)
		iTimer.Cancel();
		m.Complete(KErrNone,EFalse);
		iMsgQ.CompleteAll(KErrServerTerminated);
		DEBUG_PROFILER(Kern::Printf("CLOSE end");)
		return;
		}
	else if (id<0)
		{
		if (id!=~RSampler::ERequestRead)
			{
			TRequestStatus* pS=(TRequestStatus*)m.Ptr0();
			Kern::RequestComplete(iClient,pS,KErrNotSupported);
			}
		if (iReqStatus)
			{
			m.PanicClient(_L("SAMPLER"),ERequestAlreadyPending);
			return;
			}
		iReqStatus=(TRequestStatus*)m.Ptr0();
		iClientDes=(TDes8*)m.Ptr1();
		TInt err = ProcessReadRequest();
		Complete(err);
		}
	else if (id==KMaxTInt)
		{
		TInt mask=m.Int0();
		if (mask & (1<<RSampler::ERequestRead))
			{
			Complete(KErrCancel);
			}
		}
	else
		{
		switch(id)
			{
			case RSampler::EControlStartProfile:
				r=StartSampling(m.Int0());
				break;
			case RSampler::EControlStopProfile:
				r=StopSampling();
				break;
			case RSampler::EControlResetProfile:
				r=Reset();
				break;
			//case RSampler::EControlGetErrors:
			//	r=GetErrors((TDes8*)m.Ptr0());
			//	break;
			default:
				r=KErrNotSupported;
				break;
			}
		}
	m.Complete(r,ETrue);
	}

void DProfile::Sample(TAny* aPtr)
	{
	DProfile& d=*(DProfile*)aPtr;
	d.DoSample();
	}

void DProfile::DoSample()
	{
	iTimer.Again(iPeriod);

	TUint32& currentPos = iCurrentBuf[0];
	if (currentPos < (TUint)KRawBufSize) // space in buffer
		{
		DThread* pT=Kern::NThreadToDThread(NKern::CurrentThread());
		if (pT!=NULL)
			{
			iCurrentBuf[currentPos] = pT->iId;
			currentPos++;
			}
		}
	else
		iReport.iRawBufferErrCounter++;
	}
