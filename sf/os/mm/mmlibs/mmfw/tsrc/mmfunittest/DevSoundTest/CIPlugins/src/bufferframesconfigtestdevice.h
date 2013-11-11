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
//

#ifndef BUFFERFRAMESCONFIGTESTDEVICE_H
#define BUFFERFRAMESCONFIGTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


/**
*
* CBufferFramesConfigTestDevice
*
* Test hw device used by the
* TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CBufferFramesConfigTestDevice : public CMMFSwCodecWrapper, public MMMFBufferFramesConfig
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CBufferFramesConfigTestDevice();
	virtual TInt Start(TDeviceFunc aFuncCmd, TDeviceFlow aFlowCmd);
	virtual TInt Stop();
	virtual TInt Pause();
	virtual TInt Init(THwDeviceInitParams& aDevInfo);
	virtual TAny* CustomInterface(TUid aInterfaceId);
	virtual TInt ThisHwBufferFilled(CMMFBuffer& aFillBufferPtr);
	virtual TInt ThisHwBufferEmptied(CMMFBuffer& aEmptyBufferPtr);
	virtual TInt SetConfig(TTaskConfig& aConfig);
	virtual TInt StopAndDeleteCodec();
	virtual TInt DeleteCodec();

	// from CMMFSwCodecWrapper
	virtual CMMFSwCodec& Codec();

	// from MMMFBufferFramesConfig
	virtual TInt MmbfcSetNumberOfFramesPerInputBuffer(TInt aFrameCount, TInt aSamplesPerFrame);
	virtual TInt MmbfcSetNumberOfFramesPerOutputBuffer(TInt aFrameCount, TInt aSamplesPerFrame);
	
private:
	CBufferFramesConfigTestDevice();
	void ConstructL();
	void DoWriteToFileL(TInt aFrameCount, TInt aSamplesPerFrame);
	};

#endif 
