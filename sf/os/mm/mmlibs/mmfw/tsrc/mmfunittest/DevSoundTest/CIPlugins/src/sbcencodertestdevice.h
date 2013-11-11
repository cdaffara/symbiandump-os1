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

#ifndef SBCENCODERTESTDEVICE_H
#define SBCENCODERTESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>


/**
*
* CSbcEncoderTestDevice
*
* Test hw device used by the
* TSU_MMF_DEVSOUND_CIU_SUITE unit test harness.
*/
class CSbcEncoderTestDevice : public CMMFSwCodecWrapper, public MSbcEncoderIntfc
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CSbcEncoderTestDevice();
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

	// from MSbcEncoderIntfc
	virtual TInt GetSupportedSamplingFrequencies ( RArray<TUint>& aSamplingFrequencies );
	virtual TInt GetSupportedChannelModes ( RArray<TSbcChannelMode>& aChannelModes );
	virtual TInt GetSupportedNumOfSubbands ( RArray<TUint>& aNumOfSubbands ); 
	virtual TInt GetSupportedAllocationMethods ( RArray<TSbcAllocationMethod>& aAllocationMethods );
	virtual TInt GetSupportedNumOfBlocks ( RArray<TUint>& aNumOfBlocks );
	
	virtual TInt GetSupportedBitpoolRange (TUint& aMinSupportedBitpoolSize, TUint& aMaxSupportedBitpoolSize);
	virtual void SetSamplingFrequency ( TUint aSamplingFrequency );
	virtual void SetChannelMode (TSbcChannelMode aChannelMode );
	virtual void SetNumOfSubbands ( TUint aNumOfSubbands );
	virtual void SetNumOfBlocks ( TUint aNumOfBlocks );
	virtual void SetAllocationMethod (TSbcAllocationMethod aAllocationMethod );
	virtual void SetBitpoolSize (TUint aBitpoolSize );
	virtual TInt ApplyConfig();
	virtual TInt GetSamplingFrequency ( TUint& aSamplingFrequency );
	virtual TInt GetChannelMode (TSbcChannelMode& aChannelMode );
	virtual TInt GetNumOfSubbands ( TUint& aNumOfSubbands );
	virtual TInt GetNumOfBlocks ( TUint& aNumOfBlocks );
	virtual TInt GetAllocationMethod (TSbcAllocationMethod& aAllocationMethod );
	virtual TInt GetBitpoolSize (TUint& aBitpoolSize );
	
private:
	CSbcEncoderTestDevice();
	void ConstructL();
private:
	TUint iSamplingFrequency;
	TSbcChannelMode iChannelMode;
	TUint iNumOfSubbands;
	TUint iNumOfBlocks;
	TSbcAllocationMethod iAllocMethod;
	TUint iBitpoolSize;
	TBool iApplyConfig;
	};

#endif 
