// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMFBTSWCODECDATAPATH_H__
#define __MMFBTSWCODECDATAPATH_H__

#include <e32base.h>
#include <mmf/server/mmfdatabuffer.h>

class MMMFHwDeviceObserver;
class CMMFSwCodec;

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
	virtual void BufferFilledL(CMMFDataBuffer& aBuffer) = 0;
	virtual void BufferEmptiedL(const CMMFDataBuffer& aBuffer) = 0;
	virtual void SoundDeviceException(TInt aError) = 0;
	virtual TSwCodecDataPathState State() {return iState;};
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
	TSwCodecDataPathState iState;
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

#endif

