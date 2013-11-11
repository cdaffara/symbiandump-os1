// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFSWCODECDATAPATH_H__
#define __MMFSWCODECDATAPATH_H__

#include <e32base.h>
#include <mmf/server/mmfdatabuffer.h>


#ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
	#include "mdasoundadapter.h"
#else
	#include <mdasound.h>
	_LIT(KPddFileName,"ESDRV.PDD");
	_LIT(KLddFileName,"ESOUND.LDD");
#endif
	
class MMMFHwDeviceObserver;
class CMMFSwCodec;

/*
Interface for setting desired sample rate, channels and for setting Gain/Volume
*/
const TUid KUidSwSetParamInterface = {0x1028707E}; 
class MSwSetParamInterface
	{
public:
	virtual TInt SetSampleRate(TInt aSampleRate)=0;
	virtual TInt SetNumChannels(TInt aNumChannels)=0;
	virtual TInt SetGain(TInt aGain)=0;
	virtual TInt GetBufferSizes(TInt& aMinSize, TInt& aMaxSize)=0;
	};

/*
 * Interface for discovering various parameters
 */
const TUid KUidSwInfoInterface = {0x1028707F}; 
class MSwInfoInterface
    {
public:
    virtual TInt GetSupportedSampleRates(RArray<TInt>& aSupportedSampleRates)=0; 
    };
	
/**
 *  Base class for the datapath internal to the Sw codec wrapper
 *  @internalComponent
 */
class CMMFSwCodecDataPath : public CBase
	{ 
public: 
	enum TSwCodecDataPathState
	{
	EStopped,
	EPlaying,
	EPaused
	};
public:	
	virtual	~CMMFSwCodecDataPath(); 
	virtual TInt SetObserver(MMMFHwDeviceObserver& aHwObserver) = 0;
	virtual TInt AddCodec(CMMFSwCodec& aCodec) = 0;
	virtual TInt Start() = 0;
	virtual void Stop() = 0;
	virtual void Pause() = 0;
	virtual RMdaDevSound& Device() = 0;
	virtual void BufferFilledL(CMMFDataBuffer& aBuffer) = 0;
	virtual void BufferEmptiedL(const CMMFDataBuffer& aBuffer) = 0;
	virtual void SoundDeviceException(TInt aError) = 0;
	virtual TSwCodecDataPathState State() const = 0;
	virtual TAny* CustomInterface(TUid aInterfaceId);
protected:	
	CMMFSwCodecDataPath() {};
	inline void Panic(TInt aPanicCode);
	void ConstructL();

#ifdef __CYCLE_MMF_DATABUFFERS__
	CMMFDataBuffer* CycleAudioBuffer(CMMFDataBuffer* aBuffer);
#endif

#ifdef __USE_MMF_TRANSFERBUFFERS__
	CMMFTransferBuffer* CreateTransferBufferL(TUint aBufferSize, CMMFTransferBuffer* aOldBuffer);
#endif

#ifdef __USE_MMF_PTRBUFFERS__
	CMMFPtrBuffer* CreatePtrBufferL(TUint aBufferSize);
#endif

protected: 
	MMMFHwDeviceObserver* iHwDeviceObserver;
	CMMFSwCodec* iCodec;
#ifdef __USE_MMF_TRANSFERBUFFERS__
	RTransferBuffer* iTransferBuffer;
	RTransferWindow* iTransferWindow;
#endif

#ifdef __USE_MMF_PTRBUFFERS__
	HBufC8* iPtrBufferMemoryBlock;
#endif
	};

/**
 * Internal panic
 * @internalComponent
 */
inline void CMMFSwCodecDataPath::Panic(TInt aPanicCode)
	{
	_LIT(KMMFSwCodecWrapperPanicCategory, "MMFSwCodecWrapper");
	User::Panic(KMMFSwCodecWrapperPanicCategory, aPanicCode);
	}

/**
 * Extended datapath class with state info  
 * @internalComponent 
 */
class CMMFSwCodecDataPathX : public CMMFSwCodecDataPath
    {
protected: 
    CMMFSwCodecDataPathX(): CMMFSwCodecDataPath() {}
    
    // from CMMFSwCodecDataPath
    TSwCodecDataPathState State() const {return iState;}

    TSwCodecDataPathState iState;
    };

#endif

