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

#include "tsu_mdf_codecapi_load2.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>

//
// RTestStepCodecApiLoadPUDirect
// loads a PU directly

/**
 * Constructor
 */
RTestStepCodecApiLoadPUDirect::RTestStepCodecApiLoadPUDirect()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0011-HP");	
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiLoadPUDirect::InitializeComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiLoadPUDirect::ExecuteComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadPUDirect::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test loading a PU direct"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
	
		iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(KUidPuLoader, iPuLoaderDtorKey));
	
		// load by FourCC's
		TRAP(err, iPu = iPuLoader->LoadProcessingUnitL(*this, KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16 ));
	
		if (err == KErrNone)
			{
			INFO_PRINTF1(_L("PU loaded"));
			
			iPuLoader->UnloadProcessingUnit(iPu);
			INFO_PRINTF1(_L("PU unloaded"));
			verdict = EPass;
			}
		else
			{
			INFO_PRINTF2(_L("PU failed to load, error: %d"), err);			
			}	

		delete iPuLoader;
		REComSession::DestroyedImplementation(iPuLoaderDtorKey);
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
// RTestStepCodecApiLoadPUDirectByUid
// loads a PU directly by UID

/**
 * Constructor
 */
RTestStepCodecApiLoadPUDirectByUid::RTestStepCodecApiLoadPUDirectByUid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0010-HP");	
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiLoadPUDirectByUid::InitializeComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiLoadPUDirectByUid::ExecuteComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadPUDirectByUid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test loading a PU direct by UID"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
	
		iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(KUidPuLoader, iPuLoaderDtorKey));
	
		// load by implementation uid
		TRAP(err, iPu = iPuLoader->LoadProcessingUnitL(*this, KUidPcmPu));
	
		if (err == KErrNone)
			{
			INFO_PRINTF1(_L("PU loaded"));
		
			iPuLoader->UnloadProcessingUnit(iPu);
			INFO_PRINTF1(_L("PU unloaded"));
			verdict = EPass;
			}
		else
			{
			INFO_PRINTF2(_L("PU failed to load, error: %d"), err);			
			}	
		delete iPuLoader;
		REComSession::DestroyedImplementation(iPuLoaderDtorKey);
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
// RTestStepCodecApiTunnelSetup
// loads a PU and attempts to setup a tunnel between ports

/**
 * Constructor
 */
RTestStepCodecApiTunnelSetup::RTestStepCodecApiTunnelSetup()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0020-HP");	
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiTunnelSetup::InitializeComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiTunnelSetup::ExecuteComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}

// NB for this to work we need a PU which supports tunnelling.
// The PCM processing units can tunnel their output port only;
// CPcmInputPort::MipTunnelRequest is not supported.
// Hence load the audio device PU for its input port.
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiTunnelSetup::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test loading a PU direct and setting up a tunnel"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(KUidPuLoader, iPuLoaderDtorKey));
	
		// load by implementation uid
		TRAP(err, iPcmPu = iPuLoader->LoadProcessingUnitL(*this, KUidPcmPu) );
		TRAP(err, iSrcSinkPu = iPuLoader->LoadProcessingUnitL(*this, KUidSourceSinkPu) );
	
		if (err == KErrNone)
			{
			INFO_PRINTF1(_L("PUs loaded"));
		
			// Get ports
			err = iSrcSinkPu->GetInputPorts(iInputPortsArray);
			err += iPcmPu->GetOutputPorts(iOutputPortsArray);
	
			// const MMdfInputPort& inputPort = static_cast<MMdfInputPort&>(*iInputPortsArray[0]);
			// const MMdfOutputPort& outputPort = static_cast<MMdfOutputPort&>(*iOutputPortsArray[0]);	
			err = iPuLoader->TunnelSetup(*iOutputPortsArray[0], *iInputPortsArray[0]);
			if (err == KErrNone)
				{
				INFO_PRINTF1(_L("Tunnel setup"));
				verdict = EPass;
				}
			else 
				{
				INFO_PRINTF2(_L("Tunnel setup failed, error %d"), err);
				}
			
			iInputPortsArray.Close();
			iOutputPortsArray.Close();
	
			iPuLoader->UnloadProcessingUnit(iPcmPu);
			iPuLoader->UnloadProcessingUnit(iSrcSinkPu);
			INFO_PRINTF1(_L("PUs unloaded"));
			}
        delete iPuLoader;
		REComSession::DestroyedImplementation(iPuLoaderDtorKey);
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
// RTestStepCodecApiLoadAdapterDirect
// loads a codec using CMdfHwDeviceAdapter direct

/**
 * Constructor
 */
RTestStepCodecApiLoadAdapterDirect::RTestStepCodecApiLoadAdapterDirect()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0060-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadAdapterDirect::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	
	INFO_PRINTF1(_L("Test loading a CMdfHwDeviceAdapter direct"));
	
	TInt err = OMX_Init();
	if (err == KErrNone)
		{
		CMMFHwDevice* hwDevice = NULL;
		
		hwDevice = static_cast<CMMFHwDevice*>
		(REComSession::CreateImplementationL(TUid::Uid(KUidHwDeviceAudioAdapter), iHwDeviceAdapterDtorKey));

		if (hwDevice == NULL)
			{
			INFO_PRINTF1(_L("Could not create Hw Device"));
			}
		else 
			{
			THwDeviceInitParams initParams;
			initParams.iHwDeviceObserver = this;
			MMdfHwDeviceSetup* setup = (MMdfHwDeviceSetup*)(hwDevice->CustomInterface(KUidHwDeviceSetupInterface));
			if (setup != NULL)
				{
				setup->SetDataTypesL(KMMFFourCCCodeTestPCMU8,KMMFFourCCCodePCM16);
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
		REComSession::DestroyedImplementation(iHwDeviceAdapterDtorKey);
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
// negative tests

//
// RTestStepCodecApiLoadPUDirectInvalid
// attempt to load a PU with invalid FourCC codes

/**
 * Constructor
 */
RTestStepCodecApiLoadPUDirectInvalid::RTestStepCodecApiLoadPUDirectInvalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1011-HP");	
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiLoadPUDirectInvalid::InitializeComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiLoadPUDirectInvalid::ExecuteComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadPUDirectInvalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test loading a PU direct with invalid FourCCs"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
	
		iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(KUidPuLoader, iPuLoaderDtorKey));
	
		// load by FourCC's
		TRAP(err, iPu = iPuLoader->LoadProcessingUnitL(*this, KMMFFourCCCodeTestGarbage1,KMMFFourCCCodeTestGarbage2 ));
	
		if (err != KErrNotFound)
			{
			INFO_PRINTF2(_L("PU load failed as expected, error %d"), err);
			if(err == KErrNone) 
				{
				iPuLoader->UnloadProcessingUnit(iPu);
				INFO_PRINTF1(_L("PU unloaded"));
				}
			}
		else 
			{
			INFO_PRINTF2(_L("PU load failed as expected, error %d"), err);
			verdict = EPass;
			}
        delete iPuLoader;
		REComSession::DestroyedImplementation(iPuLoaderDtorKey);
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
// RTestStepCodecApiLoadPUDirectByUidInvalid
// attempt to load a PU directly by invalid UID

/**
 * Constructor
 */
RTestStepCodecApiLoadPUDirectByUidInvalid::RTestStepCodecApiLoadPUDirectByUidInvalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1010-HP");	
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiLoadPUDirectByUidInvalid::InitializeComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiLoadPUDirectByUidInvalid::ExecuteComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadPUDirectByUidInvalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test loading a PU direct with invalid UID"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
	
		iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(KUidPuLoader, iPuLoaderDtorKey));
	
		// load by implementation uid
		TRAP(err, iPu = iPuLoader->LoadProcessingUnitL(*this, KUidGarbagePu));
	
		if (err != KErrNotFound)
			{
			INFO_PRINTF2(_L("PU load failed as expected, error %d"), err);
			if(err == KErrNone) 
				{
				iPuLoader->UnloadProcessingUnit(iPu);
				INFO_PRINTF1(_L("PU unloaded"));
				}
			}
		else 
			{
			INFO_PRINTF2(_L("PU load failed as expected, error %d"), err);
			verdict = EPass;
			}
        delete iPuLoader;
		REComSession::DestroyedImplementation(iPuLoaderDtorKey);
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
// RTestStepCodecApiTunnelSetupInvalid
// loads a PU and attempts to setup a tunnel between ports
// use two PCM PUs, which do not support tunnelling between them.

/**
 * Constructor
 */
RTestStepCodecApiTunnelSetupInvalid::RTestStepCodecApiTunnelSetupInvalid()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1020-HP");	
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiTunnelSetupInvalid::InitializeComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiTunnelSetupInvalid::ExecuteComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiTunnelSetupInvalid::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test setting up an invalid tunnel"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
	
		iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(KUidPuLoader, iPuLoaderDtorKey));
	
		// load by implementation uid
		TRAP(err, iPcmPu = iPuLoader->LoadProcessingUnitL(*this, KUidPcmPu) );
		TRAP(err, iSrcSinkPu = iPuLoader->LoadProcessingUnitL(*this, KUidPcmPu) );
	
		if (err == KErrNone)
			{
			INFO_PRINTF1(_L("PUs loaded"));
		
			// Get ports
			err = iSrcSinkPu->GetInputPorts(iInputPortsArray);
			err += iPcmPu->GetOutputPorts(iOutputPortsArray);
	
		//	const MMdfInputPort& inputPort = static_cast<MMdfInputPort&>(*iInputPortsArray[0]);
		//	const MMdfOutputPort& outputPort = static_cast<MMdfOutputPort&>(*iOutputPortsArray[0]);	
			err = iPuLoader->TunnelSetup(*iOutputPortsArray[0], *iInputPortsArray[0]);
			if (err != KErrNotSupported)
				{
				INFO_PRINTF2(_L("Tunnel setup returned error %d"), err);
				}
			else 
				{
				INFO_PRINTF2(_L("Tunnel setup failed as expected, error %d"), err);
				verdict = EPass;
				}
			
			iInputPortsArray.Close();
			iOutputPortsArray.Close();
	
			iPuLoader->UnloadProcessingUnit(iPcmPu);
			iPuLoader->UnloadProcessingUnit(iSrcSinkPu);
			INFO_PRINTF1(_L("PUs unloaded"));
			}
        delete iPuLoader;
		REComSession::DestroyedImplementation(iPuLoaderDtorKey);
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
// RTestStepCodecApiLoadPUMemLeak
// PDEF111147

/**
 * Constructor
 */
RTestStepCodecApiLoadPUMemLeak::RTestStepCodecApiLoadPUMemLeak()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-1110-HP");	
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiLoadPUMemLeak::InitializeComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiLoadPUMemLeak::ExecuteComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiLoadPUMemLeak::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test loading a PU direct"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
	
		iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(KUidPuLoader, iPuLoaderDtorKey));
	
		// load by FourCC's
		TRAP(err, iPu = iPuLoader->LoadProcessingUnitL(*this, KMMFFourCCCodeTestPCM8, KMMFFourCCCodePCM16));
	
		if (err == KErrNone)
			{
			INFO_PRINTF1(_L("PU loaded"));
			
			iPuLoader->UnloadProcessingUnit(iPu);
			INFO_PRINTF1(_L("PU unloaded"));
			verdict = EPass;
			}
		else
			{
			INFO_PRINTF2(_L("PU failed to load, error: %d"), err);			
			}	

		REComSession::DestroyedImplementation(iPuLoaderDtorKey);
		delete iPuLoader;
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
