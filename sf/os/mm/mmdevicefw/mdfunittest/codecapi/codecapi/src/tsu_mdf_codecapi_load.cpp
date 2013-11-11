// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsu_mdf_codecapi_load.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>

// 3 load tests :-
// create the adapter
// then call the custom interface
// then load the codec (by calling SetDataTypes)

//
// RTestStepCodecApiLoadAdapter

/**
 * Constructor
 */
RTestStepCodecApiLoadAdapter::RTestStepCodecApiLoadAdapter()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0061-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadAdapter::DoTestStepL()
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
			verdict = EPass;
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

//
// RTestStepCodecApiLoadCustomInterface

/**
 * Constructor
 */
RTestStepCodecApiLoadCustomInterface::RTestStepCodecApiLoadCustomInterface()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0062-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCustomInterface::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test loading a CustomInterface"));
	
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
				verdict = EPass;
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

//
// RTestStepCodecApiLoadCodec

/**
 * Constructor
 */
RTestStepCodecApiLoadCodec::RTestStepCodecApiLoadCodec()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0065-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCodec::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test loading a codec"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					verdict = EPass;
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

//
// RTestStepCodecApiLoadInitCodec

/**
 * Constructor
 */
RTestStepCodecApiLoadInitCodec::RTestStepCodecApiLoadInitCodec()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0066-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadInitCodec::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test loading and initialising a codec"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
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
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

//
// Stop tests

//
// RTestStepCodecApiLoadStop

/**
 * Constructor
 */
RTestStepCodecApiLoadStop::RTestStepCodecApiLoadStop()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0120-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadStop::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test Stopping a codec after load"));
	
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
			// Stop
			err = hwDevice->Stop();
			if(err == KErrNone) 
				{
				verdict = EPass;
				}
			else 
				{
				INFO_PRINTF2(_L("Stop() failed, error %d"), err);
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

//
// RTestStepCodecApiLoadCIStop

/**
 * Constructor
 */
RTestStepCodecApiLoadCIStop::RTestStepCodecApiLoadCIStop()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0121-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCIStop::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test stopping a codec after load / CI"));
	
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
				// stop
				err = hwDevice->Stop();
				if(err == KErrNone) 
					{
					verdict = EPass;
					}
				else 
					{
					INFO_PRINTF2(_L("Stop() failed, error %d"), err);
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

//
// RTestStepCodecApiLoadCISetupStop

/**
 * Constructor
 */
RTestStepCodecApiLoadCISetupStop::RTestStepCodecApiLoadCISetupStop()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0122-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCISetupStop::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test stopping a codec after load / CI / setup"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					// stop
					err = hwDevice->Stop();
					if(err == KErrNone) 
						{
						verdict = EPass;
						}
					else 
						{
						INFO_PRINTF2(_L("Stop() failed, error %d"), err);
						}
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

//
// RTestStepCodecApiLoadCISetupInitStop

/**
 * Constructor
 */
RTestStepCodecApiLoadCISetupInitStop::RTestStepCodecApiLoadCISetupInitStop()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0123-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCISetupInitStop::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test stopping a codec after load / CI / setup / init"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					err = hwDevice->Init(initParams);
					if (err == KErrNone)
						{
						// stop
						err = hwDevice->Stop();
						if(err == KErrNone) 
							{
							verdict = EPass;
							}
						else 
							{
							INFO_PRINTF2(_L("Stop() failed, error %d"), err);
							}
						}
					else
						{
						INFO_PRINTF2(_L("Hw Device initialisation failed, error %d"), err);
						}
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

//
// pause tests

//
// RTestStepCodecApiLoadPause

/**
 * Constructor
 */
RTestStepCodecApiLoadPause::RTestStepCodecApiLoadPause()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0130-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadPause::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test pausing a codec after load"));
	
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
			// Pause
			err = hwDevice->Pause();
			if(err == KErrNone) 
				{
				verdict = EPass;
				}
			else 
				{
				INFO_PRINTF2(_L("Pause() failed, error %d"), err);
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

//
// RTestStepCodecApiLoadCIPause

/**
 * Constructor
 */
RTestStepCodecApiLoadCIPause::RTestStepCodecApiLoadCIPause()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0131-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCIPause::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test pausing a codec after load / CI"));
	
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
				// Pause
				err = hwDevice->Pause();
				if(err == KErrNone) 
					{
					verdict = EPass;
					}
				else 
					{
					INFO_PRINTF2(_L("Pause() failed, error %d"), err);
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

//
// RTestStepCodecApiLoadCISetupPause

/**
 * Constructor
 */
RTestStepCodecApiLoadCISetupPause::RTestStepCodecApiLoadCISetupPause()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0132-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCISetupPause::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test pausing a codec after load / CI / setup"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					// Pause
					err = hwDevice->Pause();
					if(err == KErrNone) 
						{
						verdict = EPass;
						}
					else 
						{
						INFO_PRINTF2(_L("Pause() failed, error %d"), err);
						}
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

//
// RTestStepCodecApiLoadCISetupInitPause

/**
 * Constructor
 */
RTestStepCodecApiLoadCISetupInitPause::RTestStepCodecApiLoadCISetupInitPause()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0133-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCISetupInitPause::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test pausing a codec after load / CI / setup / init"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					err = hwDevice->Init(initParams);
					if (err == KErrNone)
						{
						// Pause
						err = hwDevice->Pause();
						if(err == KErrNone) 
							{
							verdict = EPass;
							}
						else 
							{
							INFO_PRINTF2(_L("Pause() failed, error %d"), err);
							}
						}
					else
						{
						INFO_PRINTF2(_L("Hw Device initialisation failed, error %d"), err);
						}
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

//
// stop & delete codec tests

//
// RTestStepCodecApiLoadSDC

/**
 * Constructor
 */
RTestStepCodecApiLoadSDC::RTestStepCodecApiLoadSDC()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0140-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadSDC::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test StopAndDeleteCodec after load"));
	
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
			// StopAndDeleteCodec
			err = hwDevice->StopAndDeleteCodec();
			if(err == KErrNone) 
				{
				verdict = EPass;
				}
			else 
				{
				INFO_PRINTF2(_L("StopAndDeleteCodec() failed, error %d"), err);
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

//
// RTestStepCodecApiLoadCISDC

/**
 * Constructor
 */
RTestStepCodecApiLoadCISDC::RTestStepCodecApiLoadCISDC()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0141-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCISDC::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test StopAndDeleteCodec after load / CI"));
	
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
				// StopAndDeleteCodec
				err = hwDevice->StopAndDeleteCodec();
				if(err == KErrNone) 
					{
					verdict = EPass;
					}
				else 
					{
					INFO_PRINTF2(_L("StopAndDeleteCodec() failed, error %d"), err);
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

//
// RTestStepCodecApiLoadCISetupSDC

/**
 * Constructor
 */
RTestStepCodecApiLoadCISetupSDC::RTestStepCodecApiLoadCISetupSDC()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0142-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCISetupSDC::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test StopAndDeleteCodec after load / CI / setup"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					// StopAndDeleteCodec
					err = hwDevice->StopAndDeleteCodec();
					if(err == KErrNone) 
						{
						verdict = EPass;
						}
					else 
						{
						INFO_PRINTF2(_L("StopAndDeleteCodec() failed, error %d"), err);
						}
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

//
// RTestStepCodecApiLoadCISetupInitSDC

/**
 * Constructor
 */
RTestStepCodecApiLoadCISetupInitSDC::RTestStepCodecApiLoadCISetupInitSDC()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0143-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCISetupInitSDC::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test StopAndDeleteCodec after load / CI / setup / init"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					err = hwDevice->Init(initParams);
					if (err == KErrNone)
						{
						// StopAndDeleteCodec
						err = hwDevice->StopAndDeleteCodec();
						if(err == KErrNone) 
							{
							verdict = EPass;
							}
						else 
							{
							INFO_PRINTF2(_L("StopAndDeleteCodec() failed, error %d"), err);
							}
						}
					else
						{
						INFO_PRINTF2(_L("Hw Device initialisation failed, error %d"), err);
						}
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

//
// delete codec tests

//
// RTestStepCodecApiLoadDeleteCodec

/**
 * Constructor
 */
RTestStepCodecApiLoadDeleteCodec::RTestStepCodecApiLoadDeleteCodec()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0150-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadDeleteCodec::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test DeleteCodec after load"));
	
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
			// DeleteCodec
			err = hwDevice->DeleteCodec();
			if(err == KErrNone) 
				{
				verdict = EPass;
				}
			else 
				{
				INFO_PRINTF2(_L("DeleteCodec() failed, error %d"), err);
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

//
// RTestStepCodecApiLoadCIDeleteCodec

/**
 * Constructor
 */
RTestStepCodecApiLoadCIDeleteCodec::RTestStepCodecApiLoadCIDeleteCodec()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0151-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCIDeleteCodec::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test DeleteCodec after load / CI"));
	
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
				// DeleteCodec
				err = hwDevice->DeleteCodec();
				if(err == KErrNone) 
					{
					verdict = EPass;
					}
				else 
					{
					INFO_PRINTF2(_L("DeleteCodec() failed, error %d"), err);
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

//
// RTestStepCodecApiLoadCISetupDeleteCodec

/**
 * Constructor
 */
RTestStepCodecApiLoadCISetupDeleteCodec::RTestStepCodecApiLoadCISetupDeleteCodec()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0152-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCISetupDeleteCodec::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test DeleteCodec after load / CI / setup"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					// DeleteCodec
					err = hwDevice->DeleteCodec();
					if(err == KErrNone) 
						{
						verdict = EPass;
						}
					else 
						{
						INFO_PRINTF2(_L("DeleteCodec() failed, error %d"), err);
						}
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

//
// RTestStepCodecApiLoadCISetupInitDeleteCodec

/**
 * Constructor
 */
RTestStepCodecApiLoadCISetupInitDeleteCodec::RTestStepCodecApiLoadCISetupInitDeleteCodec()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0153-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCISetupInitDeleteCodec::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test DeleteCodec after load / CI / setup / init"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					err = hwDevice->Init(initParams);
					if (err == KErrNone)
						{
						// DeleteCodec
						err = hwDevice->DeleteCodec();
						if(err == KErrNone) 
							{
							verdict = EPass;
							}
						else 
							{
							INFO_PRINTF2(_L("DeleteCodec() failed, error %d"), err);
							}
						}
					else
						{
						INFO_PRINTF2(_L("Hw Device initialisation failed, error %d"), err);
						}
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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
	
//
// negative testing

//
// RTestStepCodecApiLoadAdapterInvalid

/**
 * Constructor
 */
RTestStepCodecApiLoadAdapterInvalid::RTestStepCodecApiLoadAdapterInvalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1061-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadAdapterInvalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test loading a CMMFHwDevice with invalid UID"));
	
	TInt err = OMX_Init();
	if (err == KErrNone)
		{
		CMMFHwDevice* hwDevice = NULL;
		TRAP(err, hwDevice = CMMFHwDevice::NewL(TUid::Uid(KUidHwDeviceInvalidAudioAdapter)));
		
		if (err != KErrNotFound)
			{
			ERR_PRINTF2(_L("Load attempt gave unexpected error %d"), err);
			}
		else 
			{
			INFO_PRINTF1(_L("Load attempt failed with KErrNotFound"));
			verdict = EPass;
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

//
// RTestStepCodecApiLoadCustomInterfaceInvalid

/**
 * Constructor
 */
RTestStepCodecApiLoadCustomInterfaceInvalid::RTestStepCodecApiLoadCustomInterfaceInvalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1062-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCustomInterfaceInvalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test loading a CustomInterface with invalid UID"));
	
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
			TAny* setup = hwDevice->CustomInterface(KUidHwInvalidInterface);
			if (setup == NULL)
				{
				INFO_PRINTF1(_L("Failed to create custom interface with KErrNotFound"));
				verdict = EPass;
				}
			else
				{
				INFO_PRINTF1(_L("CustomInterface() succeeded unexpectedly"));
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

//
// RTestStepCodecApiLoadCodecInvalid

/**
 * Constructor
 */
RTestStepCodecApiLoadCodecInvalid::RTestStepCodecApiLoadCodecInvalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1065-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadCodecInvalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test loading a codec with invalid FourCC codes"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestGarbage1, KMMFFourCCCodeTestGarbage2));
				if (err == KErrNotFound)
					{
					INFO_PRINTF1(_L("SetDataTypesL() left with KErrNotFound"));
					verdict = EPass;
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() had unexpected error %d"), err);
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

//
// RTestStepCodecApiLoadInitCodecInvalid

/**
 * Constructor
 */
RTestStepCodecApiLoadInitCodecInvalid::RTestStepCodecApiLoadInitCodecInvalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1066-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadInitCodecInvalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test initialising a codec without creating a Custom Interface"));
	
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
			
			// do not create a Custom Interface 
			err = hwDevice->Init(initParams);
			
			// we expect a panic HwDeviceAdapter:0
			
			// if we get here, then we have failed - the test should have panicked
			ERR_PRINTF1(_L("Failed - test did not panic"));
			
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

//
// RTestStepCodecApiLoadInitCodec2Invalid

/**
 * Constructor
 */
RTestStepCodecApiLoadInitCodec2Invalid::RTestStepCodecApiLoadInitCodec2Invalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1067-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadInitCodec2Invalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test initialising a device without loading a codec"));
	
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
				// don't call SetDataTypesL
				err = hwDevice->Init(initParams);
				
				// we expect a panic HwDeviceAdapter:0
			
				// if we get here, then we have failed - the test should have panicked
				ERR_PRINTF1(_L("Failed - test did not panic"));
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

//
// RTestStepCodecApiLoadInitCodec3Invalid

/**
 * Constructor
 */
RTestStepCodecApiLoadInitCodec3Invalid::RTestStepCodecApiLoadInitCodec3Invalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1068-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadInitCodec3Invalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test initialising a codec without setting observer"));
	
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
			// set the observer to null
			initParams.iHwDeviceObserver = NULL;
			MMdfHwDeviceSetup* setup = (MMdfHwDeviceSetup*)(hwDevice->CustomInterface(KUidHwDeviceSetupInterface));
			if (setup != NULL)
				{
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					err = hwDevice->Init(initParams);
					if (err == KErrArgument)
						{
						INFO_PRINTF1(_L("Hw Device initialisation failed with KErrArgument"));
						verdict = EPass;
						}
					else
						{
						INFO_PRINTF2(_L("Hw Device initialisation gave unexpected error %d"), err);
						}
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

//
// RTestStepCodecApiLoadNoOpenMax

/**
 * Constructor
 */
RTestStepCodecApiLoadNoOpenMax::RTestStepCodecApiLoadNoOpenMax()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1069-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadNoOpenMax::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test loading an OpenMax PU without initialising OpenMax"));
	
	CMMFHwDevice* hwDevice = NULL;
	TRAPD(err, hwDevice = CMMFHwDevice::NewL(TUid::Uid(KUidHwDeviceAudioAdapter)));
	
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
			TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
			if (err == KErrNone)
				{
				err = hwDevice->Init(initParams);
				
				// we should have panicked with OmxCore:-8	
				ERR_PRINTF1(_L("Failed - test did not panic"));
				}
			}
		}
	delete hwDevice;

	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiLoadStartInvalid

/**
 * Constructor
 */
RTestStepCodecApiLoadStartInvalid::RTestStepCodecApiLoadStartInvalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1100-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadStartInvalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test loading and starting a codec with invalid params"));
	
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
				TRAP(err, setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16));
				if (err == KErrNone)
					{
					err = hwDevice->Init(initParams);
					if (err == KErrNone)
						{
						// try and start with EDevNullFunc
						err = hwDevice->Start(EDevNullFunc, EDevInFlow);
						if (err == KErrNotSupported)
							{
							INFO_PRINTF1(_L("Start() failed with expected KErrNotSupported"));
							verdict = EPass;
							}
						else 
							{
							ERR_PRINTF2(_L("Start() gave unexpected error %d"), err);
							}
						}
					else
						{
						INFO_PRINTF2(_L("Hw Device initialisation failed, error %d"), err);
						}
					}
				else
					{
					INFO_PRINTF2(_L("SetDataTypesL() left, error %d"), err);
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

// end

