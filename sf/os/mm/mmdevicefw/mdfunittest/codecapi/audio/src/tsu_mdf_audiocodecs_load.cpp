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

#include "tsu_mdf_audiocodecs_load.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>

// loads an audio PCM codec
RTestStepAudioCodecLoad::RTestStepAudioCodecLoad()
	{
	iTestStepName = _L("MM-MDF-AUDIOCODECS-0001");	
	}
	
TVerdict RTestStepAudioCodecLoad::DoTestStepL()
	{
	TVerdict verdict = EFail;
	__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test loading a CMMFHwDevice"));
	
	TInt err = OMX_Init();
	if (err == KErrNone)
		{
		CMMFHwDevice* hwDevice = NULL;
		TRAP(err, hwDevice = CMMFHwDevice::NewL(TUid::Uid(KUidHwDeviceAudioAdapter)));
		
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("Could not create Hw Device, error %d"), err);
			}
		else 
			{
			THwDeviceInitParams initParams;
			initParams.iHwDeviceObserver = this;
			MMdfHwDeviceSetup* setup = (MMdfHwDeviceSetup*)(hwDevice->CustomInterface(KUidHwDeviceSetupInterface));
			if (setup != NULL)
				{
				TRAPD(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNotFound)
					{
					delete hwDevice;
					OMX_Deinit();
					REComSession::FinalClose();
					return EFail;
					}
				
				err = hwDevice->Init(initParams);
				if (err == KErrNone)
					{
					verdict = EPass;
					}
				else
					{
					INFO_PRINTF2(_L("Hw Device initialisation failed, error %d"), err);
					}
				}
			else
				{
				INFO_PRINTF2(_L("Failed to create custom interface, error %d"), KErrNotFound);
				}		
			}
		delete hwDevice;
		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
	REComSession::FinalClose();
	__UHEAP_MARKEND;	
	return verdict;
	}

