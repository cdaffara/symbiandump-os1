// memsamplerdd.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <fshell/extrabtrace.h>
#include <platform.h>
#include <kern_priv.h>
#include "memsamplerdd.h"
#include <fshell/common.mmh>

const TInt KMinRate = 10;
const TInt KMaxRate = 10000;
const TInt KMajorVersionNumber = 1;
const TInt KMinorVersionNumber = 0;
const TInt KBuildVersionNumber = 0;
const TInt KArrayGranularity = 32;
const TInt KDfcThreadPriority = 26;
_LIT(KDfcThreadName, "MemSamplerDD");

class DDeviceMemSampler : public DLogicalDevice
	{
public:
	DDeviceMemSampler();
	virtual TInt Install();
	virtual void GetCaps(TDes8& aDes) const;
	virtual TInt Create(DLogicalChannelBase*& aChannel);
	};

class TChunkInfo
	{
public:
	TChunkInfo(DChunk& aChunk);
	TBool operator==(const TChunkInfo& aChunkInfo);
public:
	DChunk* iAddress;
	TInt iSize;
	TInt iHighWaterMark;
	TBool iSeen;
	};

TChunkInfo::TChunkInfo(DChunk& aChunk)
	: iAddress(&aChunk), iSize(aChunk.Size()), iHighWaterMark(aChunk.Size()), iSeen(ETrue)
	{
	}

TBool TChunkInfo::operator==(const TChunkInfo& aChunkInfo)
	{
	return (iSize == aChunkInfo.iSize);
	}

class DMemSamplerChannel : public DLogicalChannel
	{
public:
	DMemSamplerChannel();
	~DMemSamplerChannel();
protected:
	virtual TInt DoCreate(TInt aUnit, const TDesC8* anInfo, const TVersion& aVer);
	virtual void HandleMsg(TMessageBase* aMsg);
private:
	TInt StartSampling(TInt aRate);
	TInt StopSampling();
	TInt ResetSampling();
	TInt ReadChunks();
	void TraceNewChunk(const TChunkInfo& aChunkInfo) const;
	void TraceChangedChunk(const TChunkInfo& aChunkInfo) const;
	void TraceDeletedChunk(const TChunkInfo& aChunkInfo) const;
	void TraceNewSample() const;
#if FSHELL_PLATFORM_SYMTB >= 92
	inline TBool Running()
		{return iTimer.IsPending();}
#else
	inline TBool Running()
		{return iTimer.iState!=NTimer::EIdle;}
#endif
private:
	static void Sample(TAny*);
	static void DoSample(TAny*);
	static void ExitDfcThread(TAny*);
private:
	RArray<TChunkInfo> iChunkInfo;
	NTimer iTimer;
	TInt iPeriod;
	TInt iPeriodNumber;
	TDfcQue iPrivateDfcQ;
	TDfc iDoSampleDfc;
	TDfc iExitDfc;
	DThread* iClient;
	mutable TBool iNewSampleTraced;
	};

DECLARE_STANDARD_LDD()
	{
	return new DDeviceMemSampler;
	}

DDeviceMemSampler::DDeviceMemSampler()
	{
	iVersion=TVersion(KMajorVersionNumber, KMinorVersionNumber, KBuildVersionNumber);
	}

TInt DDeviceMemSampler::Install()
	{
	TInt r = SetName(&KMemSamplerName);
	return r;
	}

void DDeviceMemSampler::GetCaps(TDes8&) const
	{
	}

TInt DDeviceMemSampler::Create(DLogicalChannelBase*& aChannel)
	{
	aChannel = new DMemSamplerChannel;
	return aChannel ? KErrNone : KErrNoMemory;
	}

DMemSamplerChannel::DMemSamplerChannel()
	: iChunkInfo(KArrayGranularity, _FOFF(TChunkInfo, iAddress)), iTimer(Sample, this), iPeriodNumber(-1), iDoSampleDfc(DoSample, this, 1), iExitDfc(ExitDfcThread, this, 0), iNewSampleTraced(EFalse)
	{
	}

DMemSamplerChannel::~DMemSamplerChannel()
	{
	Kern::SafeClose((DObject*&)iClient, NULL);
	iExitDfc.Enque(); 
	}

void DMemSamplerChannel::ExitDfcThread(TAny*)
	{
	Kern::Exit(KErrNone);
	} 

TInt DMemSamplerChannel::DoCreate(TInt /*aUnit*/, const TDesC8* /*anInfo*/, const TVersion& aVer)
	{
	if (!Kern::QueryVersionSupported(TVersion(1,0,0),aVer))
		{
		return KErrNotSupported;
		}
	iClient=&Kern::CurrentThread();
	iClient->Open();
	TInt err = Kern::DfcQInit(&iPrivateDfcQ, KDfcThreadPriority, &KDfcThreadName);
	if (err == KErrNone)
		{
    	SetDfcQ(&iPrivateDfcQ);
		iDoSampleDfc.SetDfcQ(&iPrivateDfcQ);
		iExitDfc.SetDfcQ(&iPrivateDfcQ);
    	iMsgQ.Receive();		
		}
	
	return err;
	}

TInt DMemSamplerChannel::StartSampling(TInt aRate)
	{
	DEBUG_MEMSAMPLER(Kern::Printf("START");)
	ReadChunks();
	aRate = Min(KMaxRate, Max(KMinRate, aRate));
	iPeriod = NKern::TimerTicks(aRate);
	if (!Running())
		{
		iTimer.OneShot(iPeriod, EFalse);
		}
	DEBUG_MEMSAMPLER(Kern::Printf("START end");)
	return KErrNone;
	}

TInt DMemSamplerChannel::StopSampling()
	{
	DEBUG_MEMSAMPLER(Kern::Printf("STOP");)
	if (Running())
		{
		iTimer.Cancel();
		}
	DEBUG_MEMSAMPLER(Kern::Printf("STOP end");)
	return KErrNone;
	}

TInt DMemSamplerChannel::ResetSampling()
	{
	DEBUG_MEMSAMPLER(Kern::Printf("RESET");)
	iChunkInfo.Reset();
	DEBUG_MEMSAMPLER(Kern::Printf("RESET end");)
	return KErrNone;
	}

TInt DMemSamplerChannel::ReadChunks()
	{
	++iPeriodNumber;
	iNewSampleTraced = EFalse;

	for (TInt i = (iChunkInfo.Count() - 1); i >= 0; --i)
		{
		iChunkInfo[i].iSeen = EFalse;
		}

	DObjectCon* const * containers = Kern::Containers();
	DObjectCon& container = *containers[EChunk];
	container.Wait();

	for (TInt i = (container.Count() - 1); i >= 0; --i)
		{
		TChunkInfo thisChunk(*(DChunk*)container[i]);
		TInt pos;
		TInt err = iChunkInfo.FindInUnsignedKeyOrder(thisChunk, pos);
		if (err == KErrNotFound)
			{
			if (pos < iChunkInfo.Count())
				{
				iChunkInfo.Insert(thisChunk, pos);
				}
			else
				{
				TInt err = iChunkInfo.Append(thisChunk);
				if (err)
					{
					container.Signal();
					return err;
					}
				}
			TraceNewChunk(thisChunk);
			}
		else
			{
			if (thisChunk == iChunkInfo[pos])
				{
				// Chunk size hasn't changed - ignore.
				iChunkInfo[pos].iSeen = ETrue;
				}
			else
				{
				TChunkInfo& c = iChunkInfo[pos];
				if (thisChunk.iSize > c.iHighWaterMark)
					{
					c.iHighWaterMark = thisChunk.iSize;
					}
				c.iSize = thisChunk.iSize;
				c.iSeen = ETrue;
				TraceChangedChunk(c);
				}
			}
		}

	container.Signal();

	for (TInt i = (iChunkInfo.Count() - 1); i >= 0; --i)
		{
		const TChunkInfo& chunkInfo = iChunkInfo[i];
		if (!chunkInfo.iSeen)
			{
			TraceDeletedChunk(chunkInfo);
			iChunkInfo.Remove(i);
			}
		}

	return KErrNone;
	}

void DMemSamplerChannel::TraceNewChunk(const TChunkInfo& aChunkInfo) const
	{
	TraceNewSample();
	TFullName nameBuf;
	aChunkInfo.iAddress->FullName(nameBuf);				
	BTraceN(RMemSampler::EBtraceCategory, RMemSampler::ENewChunk, aChunkInfo.iAddress, aChunkInfo.iAddress->MaxSize(), nameBuf.Ptr(), nameBuf.Size());
	TraceChangedChunk(aChunkInfo);
	}

void DMemSamplerChannel::TraceChangedChunk(const TChunkInfo& aChunkInfo) const
	{
	TraceNewSample();
	BTraceContext12(RMemSampler::EBtraceCategory, RMemSampler::EChangedChunk, aChunkInfo.iAddress, aChunkInfo.iSize, aChunkInfo.iHighWaterMark);
	}

void DMemSamplerChannel::TraceDeletedChunk(const TChunkInfo& aChunkInfo) const
	{
	TraceNewSample();
	BTraceContext4(RMemSampler::EBtraceCategory, RMemSampler::EDeletedChunk, aChunkInfo.iAddress);
	}

void DMemSamplerChannel::TraceNewSample() const
	{
	if (!iNewSampleTraced)
		{
		iNewSampleTraced = ETrue;
		BTraceContext8(RMemSampler::EBtraceCategory, RMemSampler::ENewSample, iPeriod, iPeriodNumber);
		}
	}

void DMemSamplerChannel::HandleMsg(TMessageBase* aMsg)
	{
	TInt r=KErrNone;
	TThreadMessage& m=*(TThreadMessage*)aMsg;
	TInt id=m.iValue;
	if (id==(TInt)ECloseMsg)
		{
		DEBUG_MEMSAMPLER(Kern::Printf("CLOSE");)
		iTimer.Cancel();
		m.Complete(KErrNone,EFalse);
		iMsgQ.CompleteAll(KErrServerTerminated);
		DEBUG_MEMSAMPLER(Kern::Printf("CLOSE end");)
		return;
		}
	else
		{
		switch(id)
			{
			case RMemSampler::EControlStartProfile:
				r = StartSampling(m.Int0());
				break;
			case RMemSampler::EControlStopProfile:
				r = StopSampling();
				break;
			case RMemSampler::EControlResetProfile:
				r = ResetSampling();
				break;
			default:
				r = KErrNotSupported;
				break;
			}
		}
	m.Complete(r,ETrue);
	}

void DMemSamplerChannel::Sample(TAny* aPtr)
	{
	DMemSamplerChannel& d = *(DMemSamplerChannel*)aPtr;
	d.iTimer.Again(d.iPeriod);
	d.iDoSampleDfc.Add(); 
	}

void DMemSamplerChannel::DoSample(TAny* aPtr)
	{
	DMemSamplerChannel& d = *(DMemSamplerChannel*)aPtr;
	d.ReadChunks();
	}
