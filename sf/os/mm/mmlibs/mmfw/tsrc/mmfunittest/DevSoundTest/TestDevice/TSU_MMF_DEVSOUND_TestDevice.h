/*
* Copyright (c) 2004 - 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#ifndef TSU_MMF_DEVSOUND_TESTDEVICE_H
#define TSU_MMF_DEVSOUND_TESTDEVICE_H

#include <mmf/server/mmfswcodecwrapper.h>
#include <mmf/server/mmfhwdevice.h>
#include <mmf/server/mmfhwdevicesetup.h>


class TDummyCustomInterface
	{
public:
	TDummyCustomInterface() {};
	};

/**
*
* CMMF_TSU_DEVSOUND_TestDevice
*
* Test hw device used by the
* TSU_MMF_DEVSOUND_SUITE unit test harness
* This is used to create an asymetry between the play
* and record hardware devices - 
* used to detect defects such as DEF45329
*
*/
class CMMF_TSU_DEVSOUND_TestDevice : public CMMFHwDevice, MMdfHwDeviceSetup
	{
public:
	static CMMFHwDevice* NewL();
	virtual ~CMMF_TSU_DEVSOUND_TestDevice();
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

	// From MMdfHwDeviceSetup
	virtual void SetDataTypesL(TFourCC aSrcType, TFourCC aDestType);
	
private:
	CMMF_TSU_DEVSOUND_TestDevice();
	void ConstructL();
private:
	TDummyCustomInterface* iDummyInterface;
	};

class CTestAudioHwDevice : public CMMFSwCodecWrapper
	{
public:
	static CTestAudioHwDevice* NewL();
	virtual CMMFSwCodec& Codec();
	//override required functions from CMMFSwCodecWrapper
	virtual TInt ThisHwBufferFilled(CMMFBuffer& aFillBufferPtr);
	virtual TInt ThisHwBufferEmptied(CMMFBuffer& aEmptyBufferPtr);
private:
	void ConstructL();
	};

class CTestAudioHwDevice2 : public CMMFSwCodecWrapper
	{
public:
	static CTestAudioHwDevice2* NewL();
	virtual CMMFSwCodec& Codec();
	//override required functions from CMMFSwCodecWrapper
	virtual TInt SetConfig(TTaskConfig& aConfig);
private:
	void ConstructL();
	};

class CTestAudioHwDevice3 : public CMMFSwCodecWrapper
	{
public:
	static CTestAudioHwDevice3* NewL();
	virtual CMMFSwCodec& Codec();
	//override required functions from CMMFSwCodecWrapper
	virtual TInt Start(TDeviceFunc aFuncCmd, TDeviceFlow aFlowCmd);
private:
	void ConstructL();
	};

class CMMFTestCodec : public CMMFSwCodec
	{
#define KSourceBufferSize 0x400
#define KSinkBufferSize 0x400	
public:
	virtual TCodecProcessResult ProcessL(const CMMFBuffer& aSource, CMMFBuffer& aDest);
	virtual TUint SourceBufferSize();
	virtual TUint SinkBufferSize();
	virtual TBool IsNullCodec() {return ETrue;}
	};


#endif // TSU_MMF_DEVSOUND_TESTDEVICE_H
