/*
* Copyright (c) 2004 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "TSU_MMF_DEVSOUND_TestDevice.h"
#include "TSU_MMF_DEVSOUND_TESTDEVICE_ImplementationUID.hrh"
#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>

// __________________________________________________________________________
// Implementation

CMMFHwDevice* CMMF_TSU_DEVSOUND_TestDevice::NewL()
	{
	CMMF_TSU_DEVSOUND_TestDevice* self=new(ELeave) CMMF_TSU_DEVSOUND_TestDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CMMF_TSU_DEVSOUND_TestDevice::~CMMF_TSU_DEVSOUND_TestDevice()
	{
	delete iDummyInterface;
	}

CMMF_TSU_DEVSOUND_TestDevice::CMMF_TSU_DEVSOUND_TestDevice()
	{
	}

/**
*
*
*/
void CMMF_TSU_DEVSOUND_TestDevice::ConstructL()
	{
	}
	

TInt CMMF_TSU_DEVSOUND_TestDevice::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/) 
	{
	return 0;
	}
	

TInt CMMF_TSU_DEVSOUND_TestDevice::Stop()
	{
	return 0;
	}
	

TInt CMMF_TSU_DEVSOUND_TestDevice::Pause()
	{
	return 0;
	}

		
TInt CMMF_TSU_DEVSOUND_TestDevice::Init(THwDeviceInitParams& /*aDevInfo*/)
	{
	return 0;
	}
	
	
TAny* CMMF_TSU_DEVSOUND_TestDevice::CustomInterface(TUid aInterfaceId)
	{	
	if(aInterfaceId == KUidHwDeviceSetupInterface)
		{
		// For MDF		
		MMdfHwDeviceSetup* setup = static_cast<MMdfHwDeviceSetup*>(this);
		return setup;
		}
	
	TAny* ret = NULL;
	if (!iDummyInterface)
		{
		iDummyInterface = new TDummyCustomInterface();
		if (!iDummyInterface)
			{
			ret = NULL;
			}			
		else
			{
			ret = static_cast<TAny*>(iDummyInterface);
			}			
		}
	else
		{
		ret = static_cast<TAny*>(iDummyInterface);
		}
	return ret;
	}

	
TInt CMMF_TSU_DEVSOUND_TestDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return 0;
	}
	
	
TInt CMMF_TSU_DEVSOUND_TestDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return 0;
	}
	
	
TInt CMMF_TSU_DEVSOUND_TestDevice::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return 0;
	}
	
	
TInt CMMF_TSU_DEVSOUND_TestDevice::StopAndDeleteCodec()
	{
	return 0;
	}
	
	
TInt CMMF_TSU_DEVSOUND_TestDevice::DeleteCodec()
	{
	return 0;
	}

// From MMdfHwDeviceSetup
void CMMF_TSU_DEVSOUND_TestDevice::SetDataTypesL(TFourCC /*aSrcType*/, TFourCC /*aDestType*/)
	{
	}


/////////
CTestAudioHwDevice* CTestAudioHwDevice::NewL()
	{
	CTestAudioHwDevice* self = new(ELeave)CTestAudioHwDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestAudioHwDevice::ConstructL()
	{
	iCodec = new(ELeave)CMMFTestCodec();
	}

CMMFSwCodec& CTestAudioHwDevice::Codec()
	{
	return *iCodec;
	}

TInt CTestAudioHwDevice::ThisHwBufferFilled(CMMFBuffer& /*aFillBufferPtr*/)
	{
	return KErrAbort;
	}

TInt CTestAudioHwDevice::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	return KErrAbort;
	}

CMMFSwCodec::TCodecProcessResult CMMFTestCodec::ProcessL(const CMMFBuffer& /*aSource*/, CMMFBuffer& /*aDest*/)
	{
	return CMMFSwCodec::TCodecProcessResult();
	}
TUint CMMFTestCodec::SourceBufferSize()
	{
	return KSourceBufferSize;
	}

TUint CMMFTestCodec::SinkBufferSize()
	{
	return KSinkBufferSize;
	}
///////////////
CTestAudioHwDevice2* CTestAudioHwDevice2::NewL()
	{
	CTestAudioHwDevice2* self = new(ELeave)CTestAudioHwDevice2();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CTestAudioHwDevice2::ConstructL()
	{
	iCodec = new(ELeave)CMMFTestCodec();
	}
	
CMMFSwCodec& CTestAudioHwDevice2::Codec()
	{
	return *iCodec;
	}

TInt CTestAudioHwDevice2::SetConfig(TTaskConfig& /*aConfig*/)
	{
	return KErrAbort;
	}
//////////////////////////////
CTestAudioHwDevice3* CTestAudioHwDevice3::NewL()
	{
	CTestAudioHwDevice3* self = new(ELeave)CTestAudioHwDevice3();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestAudioHwDevice3::ConstructL()
	{
	iCodec = new(ELeave)CMMFTestCodec();
	}

CMMFSwCodec& CTestAudioHwDevice3::Codec()
	{
	return *iCodec;
	}

TInt CTestAudioHwDevice3::Start(TDeviceFunc /*aFuncCmd*/, TDeviceFlow /*aFlowCmd*/)
	{
	return KErrAbort;
	}

/////////////////////////////////
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KMmfUidTestHwDevice,	CMMF_TSU_DEVSOUND_TestDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x102834C3,	CTestAudioHwDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x10286461,	CTestAudioHwDevice::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x102834C4,	CTestAudioHwDevice2::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x10286462,	CTestAudioHwDevice2::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x102834C5,	CTestAudioHwDevice3::NewL),
	IMPLEMENTATION_PROXY_ENTRY(0x10286463,	CTestAudioHwDevice3::NewL)
	};





//
// ImplementationGroupProxy
//
////////////////////////////////////////////////////////////////////////////////

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}


