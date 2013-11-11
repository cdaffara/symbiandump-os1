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

/**
 @file
 @internalComponent
*/

#include "tsu_mdf_codecapi_pu.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mdf/mdfprocessingunit.h>
#include <mdf/mdfpuconfig.h>

//
// RTestStepCodecApiPuCreate
// loads a PU directly

/**
 * Constructor
 */
RTestStepCodecApiPuCreate::RTestStepCodecApiPuCreate()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0200-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuCreate::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test creating a PU"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		
		TRAP(err, iPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		if (err == KErrNone && iPu != NULL)
			{
			INFO_PRINTF1(_L("PU constructed"));
			err = iPu->Create(*this);
			if(err == KErrNone) 
				{
				INFO_PRINTF1(_L("PU created"));
				verdict = EPass;
			
			// NB : currently if we try and delete direct without calling Create() 
			// it will KERN-EXEC:3
			// as it is not initialised.. think this is a problem with 
			// destructors in the PCM PU itself which needs to be fixed
				}
			else 
				{
				ERR_PRINTF2(_L("PU create failed, err %d"), err);
				}
			delete iPu;
			INFO_PRINTF1(_L("PU destroyed"));
			}
		else 
			{
			ERR_PRINTF3(_L("PU construct failed, err %d, iPu 0x%08x"), err, iPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuCustomInterface
// attempts to get a PU's custom interface, but will return KErrNotSupported
// as the Video PU does not support the KMmfVideoRecordSettingsCustomInterface
// Custom Interface.

/**
 * Constructor
 */
RTestStepCodecApiPuCustomInterface::RTestStepCodecApiPuCustomInterface()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0260-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuCustomInterface::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test getting a PU's custom interface"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		// NB test this with the video encoder PU for now,
		// as we know it does not have a custom interface.		
		TRAP(err, iPu = CMdfProcessingUnit::NewL(KUidVideoTestEncoderPu));
		if (err == KErrNone && iPu != NULL)
			{
			INFO_PRINTF1(_L("PU constructed"));
			err = iPu->Create(*this);
			if(err == KErrNone) 
				{
				INFO_PRINTF1(_L("PU created"));				
				INFO_PRINTF1(_L("Attempt to call CreateCustomInterface(KMmfVideoRecordSettingsCustomInterface)"));
				err = iPu->CreateCustomInterface(KMmfVideoRecordSettingsCustomInterface);				
				if(err != KErrNotSupported)
					{					
					ERR_PRINTF2(_L("CreateCustomInterface() failed, err %d"), err);
					}
				else
					{
					INFO_PRINTF1(_L("CreateCustomInterface() failed with expected KErrNotSupported"));
					INFO_PRINTF1(_L("Attempt to call CustomInterface(KMmfVideoRecordSettingsCustomInterface)"));
					TAny* ciPtr = iPu->CustomInterface(KMmfVideoRecordSettingsCustomInterface);
					if(ciPtr == NULL)
						{
						INFO_PRINTF1(_L("CustomInterface(KMmfVideoRecordSettingsCustomInterface) returned the expected NULL"));
						verdict = EPass;
						}
					else 
						{
					 	ERR_PRINTF1(_L("CustomInterface(KMmfVideoRecordSettingsCustomInterface) returned incorrect NON-NULL"));
						}
					}
				}
			else 
				{
				ERR_PRINTF2(_L("PU create failed, err %d"), err);
				}
			delete iPu;
			INFO_PRINTF1(_L("PU destroyed"));
			}
		else 
			{
			ERR_PRINTF3(_L("PU construct failed, err %d, iPu 0x%08x"), err, iPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuGetPorts
// gets a PU's ports

/**
 * Constructor
 */
RTestStepCodecApiPuGetPorts::RTestStepCodecApiPuGetPorts()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0201-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuGetPorts::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test getting a PU's ports"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAP(err, iPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		if (err == KErrNone && iPu != NULL)
			{
			INFO_PRINTF1(_L("PU constructed"));
			err = iPu->Create(*this);
			if(err == KErrNone) 
				{
				INFO_PRINTF1(_L("PU created"));
				
				// get the ports
				TInt err1 = iPu->GetInputPorts(iInputPortsArray);
				TInt err2 = iPu->GetOutputPorts(iOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* inputPort = iInputPortsArray[0];
					MMdfOutputPort* outputPort = iOutputPortsArray[0];
					
					if(inputPort && outputPort) 
						{
						INFO_PRINTF1(_L("Got PU ports"));
						verdict = EPass;
						}
					else 
						{
						if(!inputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, input port is null"));
							}
						if(!outputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, output port is null"));
							}
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF2(_L("PU create failed, err %d"), err);
				}

			// cleanup
			iInputPortsArray.Close();
			iOutputPortsArray.Close();
			delete iPu;
			INFO_PRINTF1(_L("PU destroyed"));
			}
		else 
			{
			ERR_PRINTF3(_L("PU construct failed, err %d, iPu 0x%08x"), err, iPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuConfig
// configures a PU

/**
 * Constructor
 */
RTestStepCodecApiPuConfig::RTestStepCodecApiPuConfig()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0202-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuConfig::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test getting a PU's ports"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAP(err, iPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		if (err == KErrNone && iPu != NULL)
			{
			INFO_PRINTF1(_L("PU constructed"));
			err = iPu->Create(*this);
			if(err == KErrNone) 
				{
				INFO_PRINTF1(_L("PU created"));
				
				// get the ports
				TInt err1 = iPu->GetInputPorts(iInputPortsArray);
				TInt err2 = iPu->GetOutputPorts(iOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* inputPort = iInputPortsArray[0];
					MMdfOutputPort* outputPort = iOutputPortsArray[0];
					
					if(inputPort && outputPort) 
						{
						INFO_PRINTF1(_L("Got PU ports"));
						
						// now configure the PU and its ports
						TTaskConfig defConfig;
						defConfig.iRate = 44100;
						defConfig.iStereoMode = 2;
						defConfig.iUid = KUidTTaskConfig;

						TPuTaskConfig config(defConfig);
						TInt err1 = inputPort->MipConfigure(config);
						TInt err2 = outputPort->MopConfigure(config);
						TInt err3 = iPu->Configure(config);
						
						if(!err1 && !err2 && !err3) 
							{
							INFO_PRINTF1(_L("PU and ports configured ok"));
							verdict = EPass;
							}
						else 
							{
							ERR_PRINTF4(_L("Configure errors inputPort %d outputPort %d PU %d"), err1, err2, err3);
							}
						}
					else 
						{
						if(!inputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, input port is null"));
							}
						if(!outputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, output port is null"));
							}
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF2(_L("PU create failed, err %d"), err);
				}

			// cleanup
			iInputPortsArray.Close();
			iOutputPortsArray.Close();
			delete iPu;
			INFO_PRINTF1(_L("PU destroyed"));
			}
		else 
			{
			ERR_PRINTF3(_L("PU construct failed, err %d, iPu 0x%08x"), err, iPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuCreateBuffers
// creates buffers

/**
 * Constructor
 */
RTestStepCodecApiPuCreateBuffers::RTestStepCodecApiPuCreateBuffers()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0210-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuCreateBuffers::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test CreateBuffers"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAP(err, iPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		if (err == KErrNone && iPu != NULL)
			{
			INFO_PRINTF1(_L("PU constructed"));
			err = iPu->Create(*this);
			if(err == KErrNone) 
				{
				INFO_PRINTF1(_L("PU created"));
				
				// get the ports
				TInt err1 = iPu->GetInputPorts(iInputPortsArray);
				TInt err2 = iPu->GetOutputPorts(iOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* inputPort = iInputPortsArray[0];
					MMdfOutputPort* outputPort = iOutputPortsArray[0];
					
					if(inputPort && outputPort) 
						{
						INFO_PRINTF1(_L("Got PU ports"));
						
						// create buffers
						
						// now configure the PU and its ports
						TInt bufSize = 1024;
						
						CMMFBuffer* buf1 = inputPort->MipCreateBuffer(bufSize);
						CMMFBuffer* buf2 = outputPort->MopCreateBuffer(bufSize);
						
						if(buf1 && buf2) 
							{
							INFO_PRINTF1(_L("Buffers created ok"));
							
							// destroy the buffers
							
							// *** NB specification of 
							// FreeBuffer() in Omx Component has changed
							
							err1 = inputPort->MipFreeBuffer(buf1);
							err2 = outputPort->MopFreeBuffer(buf2);

							if(!err1 && !err2) 
								{
								INFO_PRINTF1(_L("Buffers destroyed ok"));
								verdict = EPass;
								}
							else 
								{
								ERR_PRINTF3(_L("Destroy buffers failed, err1 %d, err2 %d"), err1, err2);
								}
							}
						else 
							{
							if(!buf1) 
								{
								ERR_PRINTF1(_L("Create buffers failed, input port buffer is null"));
								}
							if(!buf2) 
								{
								ERR_PRINTF1(_L("Create buffers failed, output port buffer is null"));
								}
							}
						}
					else 
						{
						if(!inputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, input port is null"));
							}
						if(!outputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, output port is null"));
							}
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF2(_L("PU create failed, err %d"), err);
				}

			// cleanup
			iInputPortsArray.Close();
			iOutputPortsArray.Close();
			delete iPu;
			INFO_PRINTF1(_L("PU destroyed"));
			}
		else 
			{
			ERR_PRINTF3(_L("PU construct failed, err %d, iPu 0x%08x"), err, iPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuUseBuffers
// uses buffers

/**
 * Constructor
 */
RTestStepCodecApiPuUseBuffers::RTestStepCodecApiPuUseBuffers()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0211-HP");	
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuUseBuffers::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test UseBuffers"));

	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAP(err, iPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		if (err == KErrNone && iPu != NULL)
			{
			INFO_PRINTF1(_L("PU constructed"));
			err = iPu->Create(*this);
			if(err == KErrNone) 
				{
				INFO_PRINTF1(_L("PU created"));
				
				// get the ports
				TInt err1 = iPu->GetInputPorts(iInputPortsArray);
				TInt err2 = iPu->GetOutputPorts(iOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* inputPort = iInputPortsArray[0];
					MMdfOutputPort* outputPort = iOutputPortsArray[0];
					
					if(inputPort && outputPort) 
						{
						INFO_PRINTF1(_L("Got PU ports"));
						
						// use buffers
						
						// now configure the PU and its ports
						TInt bufSize = 1024;
						
						CMMFBuffer* buf1 = CMMFDataBuffer::NewL(bufSize);
						CMMFBuffer* buf2 = CMMFDataBuffer::NewL(bufSize);
						
						err1 = inputPort->MipUseBuffer(*buf1);
						err2 = outputPort->MopUseBuffer(*buf2);
						
						if(!err1 && !err2) 
							{
							INFO_PRINTF1(_L("Buffers used ok"));
							
							// destroy the buffers
							
							// *** NB specification of 
							// FreeBuffer() in Omx Component has changed

							err1 = inputPort->MipFreeBuffer(buf1);
							err2 = outputPort->MopFreeBuffer(buf2);

							delete buf1;
							delete buf2;
							
							if(!err1 && !err2) 
								{
								INFO_PRINTF1(_L("Buffers destroyed ok"));
								verdict = EPass;
								}
							else 
								{
								ERR_PRINTF3(_L("Destroy buffers failed, err1 %d, err2 %d"), err1, err2);
								}
							}
						else
							{
							ERR_PRINTF3(_L("Use buffers failed, err1 %d, err2 %d"), err1, err2);
							}
							
						// NB don't delete buffers at this point. FreeBuffer() has already done it
						}
					else 
						{
						if(!inputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, input port is null"));
							}
						if(!outputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, output port is null"));
							}
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF2(_L("PU create failed, err %d"), err);
				}

			// cleanup
			iInputPortsArray.Close();
			iOutputPortsArray.Close();
			delete iPu;
			INFO_PRINTF1(_L("PU destroyed"));
			}
		else 
			{
			ERR_PRINTF3(_L("PU construct failed, err %d, iPu 0x%08x"), err, iPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuInit
// initializes a PU directly

/**
 * Constructor
 */
RTestStepCodecApiPuInit::RTestStepCodecApiPuInit()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0220-HP");	
	}
	
/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiPuInit::InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
	{
	if(aPu != iPu) 
		{
		ERR_PRINTF1(_L("Error: InitializeComplete called back on wrong PU"));
		}
	else if(aErrorCode != KErrNone) 
		{
		ERR_PRINTF2(_L("Error: InitializeComplete called back with error %d"), aErrorCode);
		}
	else 
		{
		INFO_PRINTF1(_L("InitializeComplete called back ok"));
		iInitComplete = ETrue;
		}	
	CActiveScheduler::Stop();		
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuInit::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test initializing a PU"));
	iInitComplete = EFalse;
	
	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAP(err, iPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		if (err == KErrNone && iPu != NULL)
			{
			INFO_PRINTF1(_L("PU constructed"));
			err = iPu->Create(*this);
			if(err == KErrNone) 
				{
				INFO_PRINTF1(_L("PU created"));
				
				// get the ports
				TInt err1 = iPu->GetInputPorts(iInputPortsArray);
				TInt err2 = iPu->GetOutputPorts(iOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* inputPort = iInputPortsArray[0];
					MMdfOutputPort* outputPort = iOutputPortsArray[0];
					
					if(inputPort && outputPort) 
						{
						INFO_PRINTF1(_L("Got PU ports"));
						
						// create buffers
						
						// now configure the PU and its ports
						TInt bufSize = 1024;
						
						CMMFBuffer* buf1 = inputPort->MipCreateBuffer(bufSize);
						CMMFBuffer* buf2 = outputPort->MopCreateBuffer(bufSize);
						
						if(buf1 && buf2) 
							{
							INFO_PRINTF1(_L("Buffers created ok"));
							
							// this has to go in an active object
							INFO_PRINTF1(_L("Calling Initialize()..."));
							iPu->Initialize();
							
							// wait for the callback or timeout
							StartActive(5);
							
							if(iInitComplete) 
								{
								INFO_PRINTF1(_L("Initialize callback ok"));
								verdict = EPass;
								}
							else 
								{
								ERR_PRINTF1(_L("Initialize callback did not occur"));
								}

							// destroy the buffers
							
							// *** NB specification of 
							// FreeBuffer() in Omx Component has changed
							err1 = inputPort->MipFreeBuffer(buf1);
							err2 = outputPort->MopFreeBuffer(buf2);

							if(!err1 && !err2) 
								{
								INFO_PRINTF1(_L("Buffers destroyed ok"));
								}
							else 
								{
								ERR_PRINTF3(_L("Destroy buffers failed, err1 %d, err2 %d"), err1, err2);
								}
							}
						else 
							{
							if(!buf1) 
								{
								ERR_PRINTF1(_L("Create buffers failed, input port buffer is null"));
								}
							if(!buf2) 
								{
								ERR_PRINTF1(_L("Create buffers failed, output port buffer is null"));
								}
							}
						}
					else 
						{
						if(!inputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, input port is null"));
							}
						if(!outputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, output port is null"));
							}
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF2(_L("PU create failed, err %d"), err);
				}

			// cleanup
			iInputPortsArray.Close();
			iOutputPortsArray.Close();
			delete iPu;
			INFO_PRINTF1(_L("PU destroyed"));
			}
		else 
			{
			ERR_PRINTF3(_L("PU construct failed, err %d, iPu 0x%08x"), err, iPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuExecute
// executes a PU directly

/**
 * Constructor
 */
RTestStepCodecApiPuExecute::RTestStepCodecApiPuExecute()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0230-HP");
	}
	
/**
 * @see MMdfInputPortObserver
 */
void RTestStepCodecApiPuExecute::MipoWriteDataComplete(const MMdfInputPort* aInputPort, CMMFBuffer* /*aBuffer*/, TInt aErrorCode)
	{
	iWriteDataComplete = ETrue;
	if(aInputPort != iInputPortsArray[0]) 
		{
		ERR_PRINTF1(_L("Error: MipoWriteDataComplete called back on wrong PU"));
		}
	else if(aErrorCode != KErrNone) 
		{
		ERR_PRINTF2(_L("Error: MipoWriteDataComplete called back with error %d"), aErrorCode);
		}
	else 
		{
		INFO_PRINTF1(_L("MipoWriteDataComplete called back ok"));
		iWriteDataOk = ETrue;
		}
	
	// make sure we have both callbacks before we stop
	if(iReadDataComplete && iWriteDataComplete) 
		{
		CActiveScheduler::Stop();		
		}
	}

/**
 * @see MMdfInputPortObserver
 */
void RTestStepCodecApiPuExecute::MipoDisconnectTunnelComplete(const MMdfInputPort* /*aInputPort*/, TInt /*aErrorCode*/)
	{
	INFO_PRINTF1(_L("MipoDisconnectTunnelComplete called back"));
	}

/**
 * @see MMdfInputPortObserver
 */
void RTestStepCodecApiPuExecute::MipoRestartTunnelComplete(const MMdfInputPort* /*aInputPort*/, TInt /*aErrorCode*/)
	{
	INFO_PRINTF1(_L("MipoRestartTunnelComplete called back"));
	}

/**
 * @see MMdfOutputPortObserver
 */
void RTestStepCodecApiPuExecute::MopoReadDataComplete(const MMdfOutputPort* aOutputPort, CMMFBuffer* /*aBuffer*/, TInt aErrorCode)
	{
	iReadDataComplete = ETrue;
	if(aOutputPort != iOutputPortsArray[0]) 
		{
		ERR_PRINTF1(_L("Error: MopoReadDataComplete called back on wrong port"));
		}
	else if(aErrorCode != KErrNone) 
		{
		ERR_PRINTF2(_L("Error: MopoReadDataComplete called back with error %d"), aErrorCode);
		}
	else 
		{
		INFO_PRINTF1(_L("MopoReadDataComplete called back ok"));
		iReadDataOk = ETrue;
		}	
	// make sure we have both callbacks before we stop
	if(iReadDataComplete && iWriteDataComplete) 
		{
		CActiveScheduler::Stop();		
		}
	}

/**
 * @see MMdfOutputPortObserver
 */
void RTestStepCodecApiPuExecute::MopoDisconnectTunnelComplete(const MMdfOutputPort* /*aOutputPort*/, TInt /*aErrorCode*/)
	{
	INFO_PRINTF1(_L("MopoDisconnectTunnelComplete called back"));
	}

/**
 * @see MMdfOutputPortObserver
 */
void RTestStepCodecApiPuExecute::MopoRestartTunnelComplete(const MMdfOutputPort* /*aOutputPort*/, TInt /*aErrorCode*/)
	{
	INFO_PRINTF1(_L("MopoRestartTunnelComplete called back"));
	}

/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiPuExecute::InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
	{
	if(aPu != iPu) 
		{
		ERR_PRINTF1(_L("Error: InitializeComplete called back on wrong PU"));
		}
	else if(aErrorCode != KErrNone) 
		{
		ERR_PRINTF2(_L("Error: InitializeComplete called back with error %d"), aErrorCode);
		}
	else 
		{
		INFO_PRINTF1(_L("InitializeComplete called back ok"));
		iInitComplete = ETrue;
		}	
	CActiveScheduler::Stop();		
	}

/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiPuExecute::ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
	{
	if(aPu != iPu) 
		{
		ERR_PRINTF1(_L("Error: ExecuteComplete called back on wrong PU"));
		}
	else if(aErrorCode != KErrNone) 
		{
		ERR_PRINTF2(_L("Error: ExecuteComplete called back with error %d"), aErrorCode);
		}
	else 
		{
		INFO_PRINTF1(_L("ExecuteComplete called back ok"));
		iExecuteComplete = ETrue;
		}	
	// the PUs are now synchronous so don't call CActiveScheduler::Stop();		
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuExecute::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test executing a PU"));
	iExecuteComplete = EFalse;
	iWriteDataComplete = EFalse;
	iReadDataComplete = EFalse;
	iWriteDataOk = EFalse;
	iReadDataOk = EFalse;
	iSourceFile = NULL;
	
	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAP(err, iPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		if (err == KErrNone && iPu != NULL)
			{
			INFO_PRINTF1(_L("PU constructed"));
			err = iPu->Create(*this);
			if(err == KErrNone) 
				{
				INFO_PRINTF1(_L("PU created"));
				
				// get the ports
				TInt err1 = iPu->GetInputPorts(iInputPortsArray);
				TInt err2 = iPu->GetOutputPorts(iOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* inputPort = iInputPortsArray[0];
					MMdfOutputPort* outputPort = iOutputPortsArray[0];
					
					if(inputPort && outputPort) 
						{
						INFO_PRINTF1(_L("Got PU ports"));
						
						// open a file, read it to buffer
						// NB this will allocate iSourceFile
						err = ReadTestFileInBuffer();
						if(err == KErrNone) 
							{
							// create buffers
						
							// now configure the PU and its ports
							TInt bufSize1 = 1024;
							TInt bufSize2 = 2048;
						
							// we're converting 8 bit to 16 bit so make the output buffer
							// twice the size of the input buffer, to ensure we get both
							// callbacks. if they were both the same size, then we'd never
							// get a callback on the input buffer (it'd never get emptied)
							CMMFBuffer* buf1 = inputPort->MipCreateBuffer(bufSize1);
							CMMFBuffer* buf2 = outputPort->MopCreateBuffer(bufSize2);
							
							if(buf1 && buf2) 
								{
								INFO_PRINTF1(_L("Buffers created ok"));
							
								// *** set port observers
								inputPort->MipSetObserver(*this);
								outputPort->MopSetObserver(*this);
								
								// initialize
								INFO_PRINTF1(_L("Calling Initialize()..."));
								iPu->Initialize();
								CActiveScheduler::Start();
									
								// now we need to run the PU.
								// start executing, pass a buffer to the input port,
								// read a buffer from the output port.
								iPu->Execute();
		
								// fill the input buffer from our file buffer
								
								CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(buf1);
							    TUint8* data_pointer = const_cast<TUint8*>(dataBuffer->Data().Ptr());
								TInt size = dataBuffer->Data().MaxLength();			
								Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Ptr(), size);
								dataBuffer->Data().SetLength(size);
						
								outputPort->MopReadData(*buf2);
								inputPort->MipWriteData(*buf1);
								StartActive(5);
								
								// if we don't get both callbacks we will time out
								if(iReadDataOk && iWriteDataOk) 
									{
									// stop the PU. NB it's synchronous now
									// check the state is EProcessingUnitIdle
									iPu->Stop();
									
									if(iPu->State() == EProcessingUnitIdle) 
										{
										// success
										verdict = EPass;
										}
									else 
										{
										ERR_PRINTF1(_L("PU state after Stop() was not EProcessingUnitIdle"));
										}
									}
								else 
									{
									ERR_PRINTF1(_L("Read / write callback did not occur"));
									}

								// destroy the buffers
							
								// *** NB specification of 
								// FreeBuffer() in Omx Component has changed
								err1 = inputPort->MipFreeBuffer(buf1);
								err2 = outputPort->MopFreeBuffer(buf2);	

								if(!err1 && !err2) 
									{
									INFO_PRINTF1(_L("Buffers destroyed ok"));
									}
								else 
									{
									ERR_PRINTF3(_L("Destroy buffers failed, err1 %d, err2 %d"), err1, err2);
									}
								}
							else 
								{
								if(!buf1) 
									{
									ERR_PRINTF1(_L("Create buffers failed, input port buffer is null"));
									}
								if(!buf2) 
									{
									ERR_PRINTF1(_L("Create buffers failed, output port buffer is null"));
									}
								}
							}
						else 
							{
							ERR_PRINTF1(_L("Open source file failed"));
							verdict = EInconclusive;
							}
						}
					else 
						{
						if(!inputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, input port is null"));
							}
						if(!outputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, output port is null"));
							}
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF2(_L("PU create failed, err %d"), err);
				}

			// cleanup
			iInputPortsArray.Close();
			iOutputPortsArray.Close();
			
			delete iSourceFile;
			delete iPu;
			INFO_PRINTF1(_L("PU destroyed"));
			}
		else 
			{
			ERR_PRINTF3(_L("PU construct failed, err %d, iPu 0x%08x"), err, iPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

/**
 * ReadTestFileInBuffer
 * reads file into buffer referenced by iSourceFile
 */
TInt RTestStepCodecApiPuExecute::ReadTestFileInBuffer()
	{
	
	RFs fs;
	fs.Connect();
	
	RFile file;
	TInt err = file.Open(fs, KTestPlayFile, EFileRead);
	if (err == KErrNone)
		{
		TInt size; 
		err = file.Size(size);
		if (err == KErrNone)
			{			
	 		iSourceFile = HBufC8::NewMax(size);
	 		if(!iSourceFile)
	 			{
	 			return KErrNoMemory;
	 			}
	 		TPtr8 ptr = iSourceFile->Des();
    		file.Read(ptr,size);
    		file.Close();
			}
		fs.Close();
		}
    return err;
	}

//
// RTestStepCodecApiPuExecuteTunnel
// execute with tunnelling

/**
 * Constructor
 */
RTestStepCodecApiPuExecuteTunnel::RTestStepCodecApiPuExecuteTunnel()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0231-HP");
	}
	
/**
 * @see MMdfInputPortObserver
 */
void RTestStepCodecApiPuExecuteTunnel::MipoWriteDataComplete(const MMdfInputPort* aInputPort, CMMFBuffer* /*aBuffer*/, TInt aErrorCode)
	{
	// we expect one callback
	iWriteDataNumCallbacks++;
	if(iWriteDataNumCallbacks == 1) 
	{
		iWriteDataComplete = ETrue;
	}
	if(aInputPort != iPcmInputPortsArray[0]) 
		{
		ERR_PRINTF1(_L("Error: MipoWriteDataComplete called back on wrong input port"));
		}
	else if(aErrorCode != KErrNone) 
		{
		ERR_PRINTF2(_L("Error: MipoWriteDataComplete called back with error %d"), aErrorCode);
		}
	else 
		{
		// NB we expect to get a Write callback on EACH PU
		// (we've called two Reads, one of which is tunnelled and so will end in
		// a WriteDataComplete)
		if(aInputPort == iPcmInputPortsArray[0]) 
			{
			INFO_PRINTF1(_L("MipoWriteDataComplete called back ok on PCM input port"));
			iWriteDataPcmOk = ETrue;
			}
		}
	if(iWriteDataPcmOk) 
		{
		iWriteDataOk = ETrue;
		}
	
	// make sure we have both callbacks before we stop
	if(iWriteDataComplete) 
		{
		CActiveScheduler::Stop();		
		}
	}

/**
 * @see MMdfOutputPortObserver
 */
void RTestStepCodecApiPuExecuteTunnel::MopoReadDataComplete(const MMdfOutputPort* /*aOutputPort*/, CMMFBuffer* /*aBuffer*/, TInt /*aErrorCode*/)
	{
	// nothing to do here!
	}

/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiPuExecuteTunnel::InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
	{
	if(aPu != iPcmPu && aPu != iSrcSinkPu) 
		{
		ERR_PRINTF1(_L("Error: InitializeComplete called back on wrong PU"));
		}
	else if(aErrorCode != KErrNone) 
		{
		ERR_PRINTF2(_L("Error: InitializeComplete called back with error %d"), aErrorCode);
		}
	else 
		{
		INFO_PRINTF1(_L("InitializeComplete called back ok"));
		iInitComplete = ETrue;
		}	
	// NB the SrcSinkPu is SYNCHRONOUS
	if(aPu != iSrcSinkPu)
		{
		CActiveScheduler::Stop();		
		}
	}

/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepCodecApiPuExecuteTunnel::ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
	{
	if(aPu != iPcmPu && aPu != iSrcSinkPu) 
		{
		ERR_PRINTF1(_L("Error: ExecuteComplete called back on wrong PU"));
		}
	else if(aErrorCode != KErrNone) 
		{
		ERR_PRINTF2(_L("Error: ExecuteComplete called back with error %d"), aErrorCode);
		}
	else 
		{
		INFO_PRINTF1(_L("ExecuteComplete called back ok"));
		iExecuteComplete = ETrue;
		}	
		
	// the PUs are now synchronous so don't call CActiveScheduler::Stop();		
	}

/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuExecuteTunnel::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test executing tunnelled PU's"));
	iExecuteComplete = EFalse;
	iWriteDataComplete = EFalse;
	iReadDataComplete = EFalse;
	iWriteDataOk = EFalse;
	iReadDataOk = EFalse;
	iSourceFile = NULL;
	
	iWriteDataPcmOk = EFalse;
	iWriteDataSrcSinkOk = EFalse;
	iWriteDataNumCallbacks = 0;
	
	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAPD(err1, iPcmPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		TRAPD(err2, iSrcSinkPu = CMdfProcessingUnit::NewL(KUidSourceSinkPu));
		if (err1 == KErrNone && err2 == KErrNone && iPcmPu != NULL && iSrcSinkPu != NULL)
			{
			INFO_PRINTF1(_L("PUs constructed"));
			err1 = iPcmPu->Create(*this);
			err2 = iSrcSinkPu->Create(*this);
			if(err1 == KErrNone && err2 == KErrNone) 
				{
				INFO_PRINTF1(_L("PUs created"));
				
				// get the ports
				err1 = iSrcSinkPu->GetInputPorts(iSrcSinkInputPortsArray);
				err1 += iSrcSinkPu->GetOutputPorts(iSrcSinkOutputPortsArray);
				err2 = iPcmPu->GetInputPorts(iPcmInputPortsArray);
				err2 += iPcmPu->GetOutputPorts(iPcmOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* pcmInputPort = iPcmInputPortsArray[0];
					MMdfOutputPort* pcmOutputPort = iPcmOutputPortsArray[0];
					MMdfInputPort* srcSinkInputPort = iSrcSinkInputPortsArray[0];
					
					if(pcmInputPort && pcmInputPort && srcSinkInputPort) 
						{
						INFO_PRINTF1(_L("Got PUs' ports"));
						
						// tunnel the ports
						// PCM output port -> SrcSink input port
						TTunnelFlags tunnelFlags = EBufferReadOnly;
						TSupplierType supplierType = EBufferSupplyUnspecified;
												
						err = pcmOutputPort->MopTunnelRequest(*srcSinkInputPort, tunnelFlags, supplierType);
						if (err == KErrNone)
							{
							err = srcSinkInputPort->MipTunnelRequest(*pcmOutputPort,tunnelFlags,supplierType);
							}
						
						if(err == KErrNone) 
							{
							if (pcmOutputPort->MopIsTunnelled())
								{
								INFO_PRINTF1(_L("Tunnel setup ok for the PCM output port"));
								}
							
							if (srcSinkInputPort->MipIsTunnelled())
								{
								INFO_PRINTF1(_L("Tunnel setup ok for the Sink input port"));
								}
						
							// open a file, read it to buffer
							// NB this will allocate iSourceFile
							err = ReadTestFileInBuffer();
							if(err == KErrNone) 
								{
								
								// create buffers for PCM input port, srcSink output port
						
								// now configure the PU and its ports
								TInt bufSize1 = 1024;
								TInt bufSize2 = 2048;
						
								// the SrcSinkPu can't create buffers, so we will create them instead
								CMMFBuffer* buf1 = CMMFDataBuffer::NewL(bufSize1);
								CMMFBuffer* buf2 = CMMFDataBuffer::NewL(bufSize2);
						
								err1 = pcmInputPort->MipUseBuffer(*buf1);
								err1 += pcmOutputPort->MopUseBuffer(*buf2);
								// it's tunnelled
								err2 = srcSinkInputPort->MipUseBuffer(*buf2);
				
						
								if(!err1 && !err2) 
									{
									INFO_PRINTF1(_L("Buffers used ok"));
							
									// *** set port observers
									pcmInputPort->MipSetObserver(*this);
									pcmOutputPort->MopSetObserver(*this);
									srcSinkInputPort->MipSetObserver(*this);
								
									// initialize
									INFO_PRINTF1(_L("Calling Initialize()..."));
									
									// NB the SrcSinkPu is SYNCHRONOUS
									iPcmPu->Initialize();
									iSrcSinkPu->Initialize();
									
									CActiveScheduler::Start();

									// now we need to run the PUs.		
									// start executing, pass a buffer to the input port,
									// read a buffer from the output port.
									iPcmPu->Execute();
									iSrcSinkPu->Execute();
		
									// fill the input buffer from our file buffer
							
									CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(buf1);
								    TUint8* data_pointer = const_cast<TUint8*>(dataBuffer->Data().Ptr());
									TInt size = dataBuffer->Data().MaxLength();			
									Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Ptr(), size);
									dataBuffer->Data().SetLength(size);
					
									pcmInputPort->MipWriteData(*buf1);
									pcmOutputPort->MopReadData(*buf2);
								
									StartActive(95);
							
									// if we don't get both callbacks we will time out
									if( iWriteDataOk) 
										{
										// stop the PU. NB it's synchronous now
										// check the state is EProcessingUnitIdle
										iSrcSinkPu->Stop();
										iPcmPu->Stop();
										
										if(iPcmPu->State() == EProcessingUnitIdle) 
											{
											// success
											verdict = EPass;
											}
										else 
											{
											ERR_PRINTF1(_L("PCM PU state after Stop() was not EProcessingUnitIdle"));
											}
										}
									else 
										{
										ERR_PRINTF1(_L("Read / write callback did not occur"));
										}		

									// destroy the buffers
									
									// *** NB specification of 
									// FreeBuffer() in Omx Component has changed
									err1 = pcmInputPort->MipFreeBuffer(buf1);
									err2 = pcmOutputPort->MopFreeBuffer(buf2);
								
									delete buf1;
									delete buf2;

									if(!err1 && !err2) 
										{
										INFO_PRINTF1(_L("Buffers destroyed ok"));
										}
									else 
										{
										ERR_PRINTF3(_L("Destroy buffers failed, err1 %d, err2 %d"), err1, err2);
										}
									}
								else 
									{
									ERR_PRINTF3(_L("Use buffers failed, err1 %d, err2 %d"), err1, err2);
									}
								}
							else 
								{
								ERR_PRINTF1(_L("Open source file failed"));
								verdict = EInconclusive;
								}
							}
						else
							{
							ERR_PRINTF2(_L("Couldn't setup tunnel, error %d"), err);						
							}
						}
					else 
						{
						ERR_PRINTF1(_L("PU get ports failed, one or more ports is null"));
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF3(_L("PU create failed, err1 %d, err2 %d"), err1, err2);
				}

			// cleanup
			iPcmInputPortsArray.Close();
			iPcmOutputPortsArray.Close();
			iSrcSinkInputPortsArray.Close();
			iSrcSinkOutputPortsArray.Close();
			
			delete iSourceFile;
			delete iSrcSinkPu;
			delete iPcmPu;
			INFO_PRINTF1(_L("PUs destroyed"));
			}
		else 
			{
			ERR_PRINTF5(_L("PU construct failed, err1 %d, err2 %d, iPcmPu 0x%08x, iSrcSinkPu 0x%08x"), err1, err2, iPcmPu, iSrcSinkPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuPauseRestart
// pause and restart a PU

/**
 * Constructor
 */
RTestStepCodecApiPuPauseRestart::RTestStepCodecApiPuPauseRestart()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0240-HP");
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuPauseRestart::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test pause / restart a PU"));
	iExecuteComplete = EFalse;
	iWriteDataComplete = EFalse;
	iReadDataComplete = EFalse;
	iWriteDataOk = EFalse;
	iReadDataOk = EFalse;
	iSourceFile = NULL;
	
	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAP(err, iPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		if (err == KErrNone && iPu != NULL)
			{
			INFO_PRINTF1(_L("PU constructed"));
			err = iPu->Create(*this);
			if(err == KErrNone) 
				{
				INFO_PRINTF1(_L("PU created"));
				
				// get the ports
				TInt err1 = iPu->GetInputPorts(iInputPortsArray);
				TInt err2 = iPu->GetOutputPorts(iOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* inputPort = iInputPortsArray[0];
					MMdfOutputPort* outputPort = iOutputPortsArray[0];
					
					if(inputPort && outputPort) 
						{
						INFO_PRINTF1(_L("Got PU ports"));
						
						// open a file, read it to buffer
						// NB this will allocate iSourceFile
						err = ReadTestFileInBuffer();
						if(err == KErrNone) 
							{
							// create buffers
						
							// now configure the PU and its ports
							TInt bufSize1 = 1024;
							TInt bufSize2 = 2048;
						
							// we're converting 8 bit to 16 bit so make the output buffer
							// twice the size of the input buffer, to ensure we get both
							// callbacks. if they were both the same size, then we'd never
							// get a callback on the input buffer (it'd never get emptied)
							CMMFBuffer* buf1 = inputPort->MipCreateBuffer(bufSize1);
							CMMFBuffer* buf2 = outputPort->MopCreateBuffer(bufSize2);
						
							if(buf1 && buf2) 
								{
								INFO_PRINTF1(_L("Buffers created ok"));
							
								// *** set port observers
								inputPort->MipSetObserver(*this);
								outputPort->MopSetObserver(*this);
							
								// initialize
								INFO_PRINTF1(_L("Calling Initialize()..."));
								iPu->Initialize();
								CActiveScheduler::Start();
							
								// now we need to run the PU.
								// start executing, pass a buffer to the input port,
								// read a buffer from the output port.
								iPu->Execute();
	
								// fill the input buffer from our file buffer
							
								CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(buf1);
							    TUint8* data_pointer = const_cast<TUint8*>(dataBuffer->Data().Ptr());
								TInt size = dataBuffer->Data().MaxLength();			
								Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Ptr(), size);
								dataBuffer->Data().SetLength(size);
					
								outputPort->MopReadData(*buf2);
								inputPort->MipWriteData(*buf1);
							
								// NB: the TestFramework timeout will cause the PU to
								// remain active and therefore subsequent tests will fail.
								// Therefore use a timeout timer to ensure that cleanup is
								// done properly
								StartActive(5);
							
								// if we don't get both callbacks we will time out.
								if(iReadDataOk && iWriteDataOk) 
									{
									// stop the PU, wait for callback
									err = iPu->Pause();
								
									if(err == KErrNone) 
										{
										// try and restart it
										INFO_PRINTF1(_L("Paused ok, restarting..."));
										iPu->Execute();
									
										// send / receive buffers again
										iExecuteComplete = EFalse;
										iWriteDataComplete = EFalse;
										iReadDataComplete = EFalse;
										iWriteDataOk = EFalse;
										iReadDataOk = EFalse;
									
										Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Ptr(), size);
										dataBuffer->Data().SetLength(size);
					
										outputPort->MopReadData(*buf2);
										inputPort->MipWriteData(*buf1);
										StartActive(5);
							
										// if we don't get both callbacks we will time out
										if(iReadDataOk && iWriteDataOk) 
											{
											INFO_PRINTF1(_L("Success"));
											verdict = EPass;
											}
										else 
											{
											ERR_PRINTF1(_L("Read / write callback did not occur"));
											}
										}
									else 
										{
										ERR_PRINTF2(_L("Pause returned error %d"), err);
										}
									}
								else 
									{
									ERR_PRINTF1(_L("Read / write callback did not occur"));
									}

								// destroy the buffers
								
								// *** NB specification of 
								// FreeBuffer() in Omx Component has changed
								err1 = inputPort->MipFreeBuffer(buf1);
								err2 = outputPort->MopFreeBuffer(buf2);	

								if(!err1 && !err2) 
									{
									INFO_PRINTF1(_L("Buffers destroyed ok"));
									}
								else 
									{
									ERR_PRINTF3(_L("Destroy buffers failed, err1 %d, err2 %d"), err1, err2);
									}
								}
							else 
								{
								if(!buf1) 
									{
									ERR_PRINTF1(_L("Create buffers failed, input port buffer is null"));
									}
								if(!buf2) 
									{
									ERR_PRINTF1(_L("Create buffers failed, output port buffer is null"));
									}
								}
							}
						else 
							{
							ERR_PRINTF1(_L("Open source file failed"));
							verdict = EInconclusive;
							}
						}
					else 
						{
						if(!inputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, input port is null"));
							}
						if(!outputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, output port is null"));
							}
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF2(_L("PU create failed, err %d"), err);
				}

			// cleanup
			iInputPortsArray.Close();
			iOutputPortsArray.Close();
			
			delete iSourceFile;
			delete iPu;
			INFO_PRINTF1(_L("PU destroyed"));
			}
		else 
			{
			ERR_PRINTF3(_L("PU construct failed, err %d, iPu 0x%08x"), err, iPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuStopRestart
// stop and restart a PU

/**
 * Constructor
 */
RTestStepCodecApiPuStopRestart::RTestStepCodecApiPuStopRestart()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0250-HP");
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuStopRestart::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test stop / restart a PU"));
	iExecuteComplete = EFalse;
	iWriteDataComplete = EFalse;
	iReadDataComplete = EFalse;
	iWriteDataOk = EFalse;
	iReadDataOk = EFalse;
	iSourceFile = NULL;
	
	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAP(err, iPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		if (err == KErrNone && iPu != NULL)
			{
			INFO_PRINTF1(_L("PU constructed"));
			err = iPu->Create(*this);
			if(err == KErrNone) 
				{
				INFO_PRINTF1(_L("PU created"));
				
				// get the ports
				TInt err1 = iPu->GetInputPorts(iInputPortsArray);
				TInt err2 = iPu->GetOutputPorts(iOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* inputPort = iInputPortsArray[0];
					MMdfOutputPort* outputPort = iOutputPortsArray[0];
					
					if(inputPort && outputPort) 
						{
						INFO_PRINTF1(_L("Got PU ports"));
						
						// open a file, read it to buffer
						// NB this will allocate iSourceFile
						err = ReadTestFileInBuffer();
						if(err == KErrNone) 
							{
							
							// create buffers
						
							// now configure the PU and its ports
							TInt bufSize1 = 1024;
							TInt bufSize2 = 2048;
						
							// we're converting 8 bit to 16 bit so make the output buffer
							// twice the size of the input buffer, to ensure we get both
							// callbacks. if they were both the same size, then we'd never
							// get a callback on the input buffer (it'd never get emptied)
							CMMFBuffer* buf1 = inputPort->MipCreateBuffer(bufSize1);
							CMMFBuffer* buf2 = outputPort->MopCreateBuffer(bufSize2);
						
							if(buf1 && buf2) 
								{
								INFO_PRINTF1(_L("Buffers created ok"));
								
								// *** set port observers
								inputPort->MipSetObserver(*this);
								outputPort->MopSetObserver(*this);
								
								// initialize
								INFO_PRINTF1(_L("Calling Initialize()..."));
								iPu->Initialize();
								CActiveScheduler::Start();
								
								// now we need to run the PU.
								// start executing, pass a buffer to the input port,
								// read a buffer from the output port.
								iPu->Execute();
		
								// fill the input buffer from our file buffer
								
								CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(buf1);
							    TUint8* data_pointer = const_cast<TUint8*>(dataBuffer->Data().Ptr());
								TInt size = dataBuffer->Data().MaxLength();			
								Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Ptr(), size);
								dataBuffer->Data().SetLength(size);
						
								outputPort->MopReadData(*buf2);
								inputPort->MipWriteData(*buf1);		
	
								// NB: the TestFramework timeout will cause the PU to
								// remain active and therefore subsequent tests will fail.
								// Therefore use a timeout timer to ensure that cleanup is
								// done properly
								StartActive(5);
														
								// if we don't get both callbacks we will time out
								if(iReadDataOk && iWriteDataOk) 
									{
									// stop the PU
									iPu->Stop();
									
									if(iPu->State() == EProcessingUnitIdle) 
										{
										// try and restart it
										INFO_PRINTF1(_L("Stopped ok, restarting..."));
										iPu->Execute();
										
										// send / receive buffers again
										iExecuteComplete = EFalse;
										iWriteDataComplete = EFalse;
										iReadDataComplete = EFalse;
										iWriteDataOk = EFalse;
										iReadDataOk = EFalse;
										
										Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Ptr(), size);
										dataBuffer->Data().SetLength(size);
						
										outputPort->MopReadData(*buf2);
										inputPort->MipWriteData(*buf1);
										StartActive(5);
								
										// if we don't get both callbacks we will time out
										if(iReadDataOk && iWriteDataOk) 
											{
											INFO_PRINTF1(_L("Success"));
											verdict = EPass;
											}
										else 
											{
											ERR_PRINTF1(_L("Read / write callback did not occur"));
											}
										}
									else 
										{
										ERR_PRINTF1(_L("PU state after Stop() was not EProcessingUnitIdle"));
										}
									}
								else 
									{
									ERR_PRINTF1(_L("Read / write callback did not occur"));
									}			

								// destroy the buffers
								
								// *** NB specification of 
								// FreeBuffer() in Omx Component has changed
								err1 = inputPort->MipFreeBuffer(buf1);
								err2 = outputPort->MopFreeBuffer(buf2);

								if(!err1 && !err2) 
									{
									INFO_PRINTF1(_L("Buffers destroyed ok"));
									}
								else 
									{
									ERR_PRINTF3(_L("Destroy buffers failed, err1 %d, err2 %d"), err1, err2);
									}
								}
							else 
								{
								if(!buf1) 
									{
									ERR_PRINTF1(_L("Create buffers failed, input port buffer is null"));
									}
								if(!buf2) 
									{
									ERR_PRINTF1(_L("Create buffers failed, output port buffer is null"));
									}
								}
							}
						else 
							{
							ERR_PRINTF1(_L("Open source file failed"));
							verdict = EInconclusive;
							}
						}
					else 
						{
						if(!inputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, input port is null"));
							}
						if(!outputPort) 
							{
							ERR_PRINTF1(_L("PU get ports failed, output port is null"));
							}
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF2(_L("PU create failed, err %d"), err);
				}

			// cleanup
			iInputPortsArray.Close();
			iOutputPortsArray.Close();
			
			delete iSourceFile;
			delete iPu;
			INFO_PRINTF1(_L("PU destroyed"));
			}
		else 
			{
			ERR_PRINTF3(_L("PU construct failed, err %d, iPu 0x%08x"), err, iPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuPauseRestartTunnel
// pause and restart, with tunnelling

/**
 * Constructor
 */
RTestStepCodecApiPuPauseRestartTunnel::RTestStepCodecApiPuPauseRestartTunnel()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0241-HP");
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuPauseRestartTunnel::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test pause / restart tunnelled PU's"));
	iExecuteComplete = EFalse;
	iWriteDataComplete = EFalse;
	iReadDataComplete = EFalse;
	iWriteDataOk = EFalse;
	iReadDataOk = EFalse;
	iSourceFile = NULL;
	
	iWriteDataPcmOk = EFalse;
	iWriteDataSrcSinkOk = EFalse;
	iWriteDataNumCallbacks = 0;
	
	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAPD(err1, iPcmPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		TRAPD(err2, iSrcSinkPu = CMdfProcessingUnit::NewL(KUidSourceSinkPu));
		if (err1 == KErrNone && err2 == KErrNone && iPcmPu != NULL && iSrcSinkPu != NULL)
			{
			INFO_PRINTF1(_L("PUs constructed"));
			err1 = iPcmPu->Create(*this);
			err2 = iSrcSinkPu->Create(*this);
			if(err1 == KErrNone && err2 == KErrNone) 
				{
				INFO_PRINTF1(_L("PUs created"));
				
				// get the ports
				err1 = iSrcSinkPu->GetInputPorts(iSrcSinkInputPortsArray);
				err1 += iSrcSinkPu->GetOutputPorts(iSrcSinkOutputPortsArray);
				err2 = iPcmPu->GetInputPorts(iPcmInputPortsArray);
				err2 += iPcmPu->GetOutputPorts(iPcmOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* pcmInputPort = iPcmInputPortsArray[0];
					MMdfOutputPort* pcmOutputPort = iPcmOutputPortsArray[0];
					MMdfInputPort* srcSinkInputPort = iSrcSinkInputPortsArray[0];
			
					
					if(pcmInputPort && pcmInputPort && srcSinkInputPort) 
						{
						INFO_PRINTF1(_L("Got PUs' ports"));
						
						// tunnel the ports
						// PCM output port -> SrcSink input port
						TTunnelFlags tunnelFlags = EBufferReadOnly;
						TSupplierType supplierType = EBufferSupplyUnspecified;

						err = pcmOutputPort->MopTunnelRequest(*srcSinkInputPort, tunnelFlags, supplierType);
						if (err == KErrNone)
							{
							err = srcSinkInputPort->MipTunnelRequest(*pcmOutputPort,tunnelFlags,supplierType);
							}
						
						if(err == KErrNone) 
							{
							if (pcmOutputPort->MopIsTunnelled())
								{
								INFO_PRINTF1(_L("Tunnel setup ok for the PCM output port"));
								}
							
							if (srcSinkInputPort->MipIsTunnelled())
								{
								INFO_PRINTF1(_L("Tunnel setup ok for the Sink input port"));
								}
							}
						if(err == KErrNone) 
							{
							INFO_PRINTF1(_L("Tunnel setup ok"));
							
							// open a file, read it to buffer
							// NB this will allocate iSourceFile
							err = ReadTestFileInBuffer();
							if(err == KErrNone) 
								{
								// create buffers for PCM input port, srcSink output port
							
								// now configure the PU and its ports
								TInt bufSize1 = 1024;
								TInt bufSize2 = 2048;
						
								// the SrcSinkPu can't create buffers, so we will create them instead
								CMMFBuffer* buf1 = CMMFDataBuffer::NewL(bufSize1);
								CMMFBuffer* buf2 = CMMFDataBuffer::NewL(bufSize2);
						
								err1 = pcmInputPort->MipUseBuffer(*buf1);
								err1 += pcmOutputPort->MopUseBuffer(*buf2);
								// it's tunnelled
								err2 = srcSinkInputPort->MipUseBuffer(*buf2);
			
								if(!err1 && !err2) 
									{
									INFO_PRINTF1(_L("Buffers used ok"));
							
									// *** set port observers
									pcmInputPort->MipSetObserver(*this);
									pcmOutputPort->MopSetObserver(*this);
									srcSinkInputPort->MipSetObserver(*this);
									// initialize
									INFO_PRINTF1(_L("Calling Initialize()..."));
								
									// NB the SrcSinkPu is SYNCHRONOUS
									iSrcSinkPu->Initialize();
									iPcmPu->Initialize();
									CActiveScheduler::Start();

									// now we need to run the PUs.
									// start executing, pass a buffer to the input port,
									// read a buffer from the output port.
									iPcmPu->Execute();
									iSrcSinkPu->Execute();
		
									// fill the input buffer from our file buffer
							
									CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(buf1);
								    TUint8* data_pointer = const_cast<TUint8*>(dataBuffer->Data().Ptr());
									TInt size = dataBuffer->Data().MaxLength();			
									Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Ptr(), size);
									dataBuffer->Data().SetLength(size);
					
									pcmOutputPort->MopReadData(*buf2);
									pcmInputPort->MipWriteData(*buf1);
									StartActive(5);
							
									// if we don't get both callbacks we will time out
									if(iWriteDataOk) 
										{
										// pause the PUs
										iSrcSinkPu->Pause();
										err = iPcmPu->Pause();
									
										if(err == KErrNone) 
											{
											// try and restart them
											INFO_PRINTF1(_L("Pause ok, restarting..."));
											iSrcSinkPu->Execute();
											iPcmPu->Execute();
									
											// send / receive buffers again
											iExecuteComplete = EFalse;
											iWriteDataComplete = EFalse;
											iReadDataComplete = EFalse;
											iWriteDataOk = EFalse;
											iReadDataOk = EFalse;
										
											iWriteDataPcmOk = EFalse;
											iWriteDataSrcSinkOk = EFalse;
											iWriteDataNumCallbacks = 0;
	
											Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Ptr(), size);
											dataBuffer->Data().SetLength(size);
						
											pcmOutputPort->MopReadData(*buf2);
											pcmInputPort->MipWriteData(*buf1);
											StartActive(5);
							
											// if we don't get both callbacks we will time out
											if(iWriteDataOk) 
												{
												// stop the PUs
												iSrcSinkPu->Stop();
												iPcmPu->Stop();
										
												if(iPcmPu->State() == EProcessingUnitIdle) 
													{
													INFO_PRINTF1(_L("Success"));
													verdict = EPass;
													}
												else 
													{
													ERR_PRINTF1(_L("PCM PU state after Stop() was not EProcessingUnitIdle"));
													}
												}
											else 
												{
												ERR_PRINTF1(_L("Read / write callback did not occur"));
												}
											}
										else 
											{
											ERR_PRINTF2(_L("PU Pause returned error %d"), err);
											}
										}
									else 
										{
										ERR_PRINTF1(_L("Read / write callback did not occur"));
										}			

									// destroy the buffers
								
									// *** NB specification of 
									// FreeBuffer() in Omx Component has changed
									err1 = pcmInputPort->MipFreeBuffer(buf1);
									err2 = pcmOutputPort->MopFreeBuffer(buf2);
									
									delete buf1;
									delete buf2;

									if(!err1 && !err2) 
										{
										INFO_PRINTF1(_L("Buffers destroyed ok"));
										}
									else 
										{
										ERR_PRINTF3(_L("Destroy buffers failed, err1 %d, err2 %d"), err1, err2);
										}
									}
								else 
									{
									ERR_PRINTF3(_L("Use buffers failed, err1 %d, err2 %d"), err1, err2);
									}
								}
							else 
								{
								ERR_PRINTF1(_L("Open source file failed"));
								verdict = EInconclusive;
								}
							}
						else
							{
							ERR_PRINTF2(_L("Couldn't setup tunnel, error %d"), err);						
							}
						}
					else 
						{
						ERR_PRINTF1(_L("PU get ports failed, one or more ports is null"));
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF3(_L("PU create failed, err1 %d, err2 %d"), err1, err2);
				}

			// cleanup
			iPcmInputPortsArray.Close();
			iPcmOutputPortsArray.Close();
			iSrcSinkInputPortsArray.Close();
			iSrcSinkOutputPortsArray.Close();
			
			delete iSourceFile;
			delete iSrcSinkPu;
			delete iPcmPu;
			INFO_PRINTF1(_L("PUs destroyed"));
			}
		else 
			{
			ERR_PRINTF5(_L("PU construct failed, err1 %d, err2 %d, iPcmPu 0x%08x, iSrcSinkPu 0x%08x"), err1, err2, iPcmPu, iSrcSinkPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

//
// RTestStepCodecApiPuStopRestartTunnel
// stop and restart, with tunnelling

/**
 * Constructor
 */
RTestStepCodecApiPuStopRestartTunnel::RTestStepCodecApiPuStopRestartTunnel()
	{
	iTestStepName = _L("MM-MDF-CODECAPI-U-0251-HP");
	}
	
/**
 * Do the test step
 */
TVerdict RTestStepCodecApiPuStopRestartTunnel::DoTestStepL()
	{
	TVerdict verdict = EFail;
__UHEAP_MARK;	

	INFO_PRINTF1(_L("Test stop / restart tunnelled PU's"));
	iExecuteComplete = EFalse;
	iWriteDataComplete = EFalse;
	iReadDataComplete = EFalse;
	iWriteDataOk = EFalse;
	iReadDataOk = EFalse;
	iSourceFile = NULL;
	
	iWriteDataPcmOk = EFalse;
	iWriteDataSrcSinkOk = EFalse;
	iWriteDataNumCallbacks = 0;
	
	TInt err = OMX_Init();
	if(err == KErrNone) 
		{
		TRAPD(err1, iPcmPu = CMdfProcessingUnit::NewL(KUidPcmPu));
		TRAPD(err2, iSrcSinkPu = CMdfProcessingUnit::NewL(KUidSourceSinkPu));
		if (err1 == KErrNone && err2 == KErrNone && iPcmPu != NULL && iSrcSinkPu != NULL)
			{
			INFO_PRINTF1(_L("PUs constructed"));
			err1 = iPcmPu->Create(*this);
			err2 = iSrcSinkPu->Create(*this);
			if(err1 == KErrNone && err2 == KErrNone) 
				{
				INFO_PRINTF1(_L("PUs created"));
				
				// get the ports
				err1 = iSrcSinkPu->GetInputPorts(iSrcSinkInputPortsArray);
				err1 += iSrcSinkPu->GetOutputPorts(iSrcSinkOutputPortsArray);
				err2 = iPcmPu->GetInputPorts(iPcmInputPortsArray);
				err2 += iPcmPu->GetOutputPorts(iPcmOutputPortsArray);
				
				if(!err1 && !err2) 
					{
					MMdfInputPort* pcmInputPort = iPcmInputPortsArray[0];
					MMdfOutputPort* pcmOutputPort = iPcmOutputPortsArray[0];
					MMdfInputPort* srcSinkInputPort = iSrcSinkInputPortsArray[0];
					MMdfOutputPort* srcSinkOutputPort = iSrcSinkOutputPortsArray[0];
					
					if(pcmInputPort && pcmInputPort && srcSinkInputPort && srcSinkOutputPort) 
						{
						INFO_PRINTF1(_L("Got PUs' ports"));
						
						// tunnel the ports
						// PCM output port -> SrcSink input port
						TTunnelFlags tunnelFlags = EBufferReadOnly;
						TSupplierType supplierType = EBufferSupplyUnspecified;
						
						err = pcmOutputPort->MopTunnelRequest(*srcSinkInputPort, tunnelFlags, supplierType);
						if (err == KErrNone)
							{
							err = srcSinkInputPort->MipTunnelRequest(*pcmOutputPort,tunnelFlags,supplierType);
							}
						
						if(err == KErrNone) 
							{
							if (pcmOutputPort->MopIsTunnelled())
								{
								INFO_PRINTF1(_L("Tunnel setup ok for the PCM output port"));
								}
							
							if (srcSinkInputPort->MipIsTunnelled())
								{
								INFO_PRINTF1(_L("Tunnel setup ok for the Sink input port"));
								}
							}
						if(err == KErrNone) 
							{
							INFO_PRINTF1(_L("Tunnel setup ok"));
							
							// open a file, read it to buffer
							// NB this will allocate iSourceFile
							err = ReadTestFileInBuffer();
							if(err == KErrNone) 
								{
								// create buffers for PCM input port, srcSink output port
						
								// now configure the PU and its ports
								TInt bufSize1 = 1024;
								TInt bufSize2 = 2048;
						
								// the SrcSinkPu can't create buffers, so we will create them instead
								CMMFBuffer* buf1 = CMMFDataBuffer::NewL(bufSize1);
								CMMFBuffer* buf2 = CMMFDataBuffer::NewL(bufSize2);
						
								err1 = pcmInputPort->MipUseBuffer(*buf1);
								err1 += pcmOutputPort->MopUseBuffer(*buf2);
								// it's tunnelled
								err2 = srcSinkInputPort->MipUseBuffer(*buf2);
						
								if(!err1 && !err2) 
									{
									INFO_PRINTF1(_L("Buffers used ok"));
							
									// *** set port observers
									pcmInputPort->MipSetObserver(*this);
									pcmOutputPort->MopSetObserver(*this);
									srcSinkInputPort->MipSetObserver(*this);
							
									// initialize
									INFO_PRINTF1(_L("Calling Initialize()..."));
								
									// NB the SrcSinkPu is SYNCHRONOUS
									iSrcSinkPu->Initialize();
									iPcmPu->Initialize();
									CActiveScheduler::Start();

									// now we need to run the PUs.
									// start executing, pass a buffer to the input port,
									// read a buffer from the output port.
									iPcmPu->Execute();
									iSrcSinkPu->Execute();
		
									// fill the input buffer from our file buffer
							
									CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(buf1);
								    TUint8* data_pointer = const_cast<TUint8*>(dataBuffer->Data().Ptr());
									TInt size = dataBuffer->Data().MaxLength();			
									Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Ptr(), size);
									dataBuffer->Data().SetLength(size);
					
									pcmOutputPort->MopReadData(*buf2);
									pcmInputPort->MipWriteData(*buf1);
									StartActive(5);
							
									// if we don't get both callbacks we will time out
									if(iWriteDataOk) 
										{
										// pause the PUs
										iSrcSinkPu->Stop();
										iPcmPu->Stop();
								
										if(iPcmPu->State() == EProcessingUnitIdle) 
											{
											// try and restart them
											INFO_PRINTF1(_L("Stop ok, restarting..."));
											iSrcSinkPu->Execute();
											iPcmPu->Execute();
									
											// send / receive buffers again
											iExecuteComplete = EFalse;
											iWriteDataComplete = EFalse;
											iReadDataComplete = EFalse;
											iWriteDataOk = EFalse;
											iReadDataOk = EFalse;
									
											iWriteDataPcmOk = EFalse;
											iWriteDataSrcSinkOk = EFalse;
											iWriteDataNumCallbacks = 0;

											Mem::Copy((TAny*)data_pointer, (TAny*)iSourceFile->Ptr(), size);
											dataBuffer->Data().SetLength(size);
					
											pcmOutputPort->MopReadData(*buf2);
											pcmInputPort->MipWriteData(*buf1);
											StartActive(5);
							
											// if we don't get both callbacks we will time out
											if(iWriteDataOk) 
												{
												// stop the PUs a second time
												iSrcSinkPu->Stop();
												iPcmPu->Stop();
										
												if(iPcmPu->State() == EProcessingUnitIdle) 
													{
													INFO_PRINTF1(_L("Success"));
													verdict = EPass;
													}
												else 
													{
													ERR_PRINTF1(_L("PCM PU state after second Stop() was not EProcessingUnitIdle"));
													}
												}
											else 
												{
												ERR_PRINTF1(_L("Write callback did not occur"));
												}
											}
										else 
											{
											ERR_PRINTF1(_L("PCM PU state after first Stop() was not EProcessingUnitIdle"));
											}
										}
									else 
										{
										ERR_PRINTF1(_L("Read / write callback did not occur"));
										}		

									// destroy the buffers
								
									// *** NB specification of 
									// FreeBuffer() in Omx Component has changed
									err1 = pcmInputPort->MipFreeBuffer(buf1);
									err2 = pcmOutputPort->MopFreeBuffer(buf2);

									delete buf1;
									delete buf2;
								
									if(!err1 && !err2) 
										{
										INFO_PRINTF1(_L("Buffers destroyed ok"));
										}
									else 
										{
										ERR_PRINTF3(_L("Destroy buffers failed, err1 %d, err2 %d"), err1, err2);
										}
									}
								else 
									{
									ERR_PRINTF3(_L("Use buffers failed, err1 %d, err2 %d"), err1, err2);
									}
								}
							else 
								{
								ERR_PRINTF1(_L("Open source file failed"));
								verdict = EInconclusive;
								}
							}
						else
							{
							ERR_PRINTF2(_L("Couldn't setup tunnel, error %d"), err);						
							}
						}
					else 
						{
						ERR_PRINTF1(_L("PU get ports failed, one or more ports is null"));
						}
					}
				else 
					{
					ERR_PRINTF3(_L("PU get ports failed, err1 %d, err2 %d"), err1, err2);
					}
				}
			else 
				{
				ERR_PRINTF3(_L("PU create failed, err1 %d, err2 %d"), err1, err2);
				}

			// cleanup
			iPcmInputPortsArray.Close();
			iPcmOutputPortsArray.Close();
			iSrcSinkInputPortsArray.Close();
			iSrcSinkOutputPortsArray.Close();
			
			delete iSourceFile;
			delete iSrcSinkPu;
			delete iPcmPu;
			INFO_PRINTF1(_L("PUs destroyed"));
			}
		else 
			{
			ERR_PRINTF5(_L("PU construct failed, err1 %d, err2 %d, iPcmPu 0x%08x, iSrcSinkPu 0x%08x"), err1, err2, iPcmPu, iSrcSinkPu);
			
			}

		OMX_Deinit();
		}
	else
		{
		INFO_PRINTF2(_L("Failed to initialise OpenMAX core, error: %d"), err);		
		}
		// must do this as CMdfProcessingUnit loads ECom plugins
	REComSession::FinalClose();
__UHEAP_MARKEND;	
	return verdict;
	}

// end
