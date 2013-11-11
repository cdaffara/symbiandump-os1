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

#include "tsu_mdf_omxvorbiscodecs_omx.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mdf/mdfpuconfig.h>
#include "PU/decoder/vorbisdecoderprocessingunit.h"
#include "PU/vorbisprocessingunituids.hrh"
// for the bitrate custom interface
#include <mmf/server/devsoundstandardcustominterfaces.h>

#include <e32debug.h>

//
// RTestStepOmxVorbisCodecOmx
// general tests relating to OpenMax layer

/**
 * Constructor
 */
RTestStepOmxVorbisCodecOmx::RTestStepOmxVorbisCodecOmx()
 	{
 	iTestStepName = _L("MM-MDF-OMXVORBISCODECS-U-0020-HP");
	iHeapSize = KTestHeapSize;
 	}

/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepOmxVorbisCodecOmx::InitializeComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}

/**
 * @see MMdfProcessingUnitObserver
 */
void RTestStepOmxVorbisCodecOmx::ExecuteComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */)
	{
	}

/**
 * Do the test step
 */
TVerdict RTestStepOmxVorbisCodecOmx::DoTestStepL()
	{
	// *** TO DO
	// (Mip/Mop)Configure / GetConfig() are largely stubbed for both PU and
	// input / output ports in the Vorbis PU.
	// This ought to be fixed.
	
	iVerdict = EPass;

	CMdfProcessingUnit* theDecPu=NULL;
	CMdfProcessingUnit* theEncPu=NULL;

	_LIT8(KTestGarbage, "I am Garbage");
	
	const TDesC8& testGarbageDes = KTestGarbage;
	const MMdfProcessingUnitObserver* garbageObs = NULL;
	
	// NB we *deliberately* use an unsafe cast
	garbageObs = (const MMdfProcessingUnitObserver*)&testGarbageDes;
	  
	TInt err = OMX_Init();
	if(err != KErrNone) 
		{
		ERR_PRINTF1(_L("Error - couldn't initialise OpenMax"));
		return EFail;	
		}

	TRAPD(err1, theDecPu = CMdfProcessingUnit::NewL(TUid::Uid(KUidVorbisDecoderPU) ));
	if(err1 == KErrNone) 
		{
		err1 = theDecPu->Create(*garbageObs);
		}
		
	TRAPD(err2, theEncPu = CMdfProcessingUnit::NewL(TUid::Uid(KUidVorbisEncoderPU) ));
	if(err2 == KErrNone) 
		{
		err2 = theEncPu->Create(*garbageObs);
		}
		
	if(err1 || err2)
		{
		ERR_PRINTF1(_L("Error - couldn't initialise PUs (1)"));
		delete theDecPu;
		delete theEncPu;
		OMX_Deinit();
		REComSession::FinalClose();
		return EFail;	
		}
		
	// re-initialise using ourselves as observer	
	delete theDecPu;
	delete theEncPu;
	TRAP(err1, theDecPu = CMdfProcessingUnit::NewL(TUid::Uid(KUidVorbisDecoderPU) ));
	if(err1 == KErrNone) 
		{
		err1 = theDecPu->Create(*this);
		}
		
	TRAP(err2, theEncPu = CMdfProcessingUnit::NewL(TUid::Uid(KUidVorbisEncoderPU) ));
	if(err2 == KErrNone) 
		{
		err2 = theEncPu->Create(*this);
		}
		
	if(err1 || err2)
		{
		ERR_PRINTF1(_L("Error - couldn't initialise PUs (2)"));
		delete theDecPu;
		delete theEncPu;
		OMX_Deinit();
		REComSession::FinalClose();
		return EFail;	
		}
		
	RPointerArray<MMdfInputPort> ipa;
	RPointerArray<MMdfOutputPort> opa;

	// decoder
	if(theDecPu)
		{
		MMdfInputPort* inputPort = NULL;
		MMdfOutputPort* outputPort = NULL;

		err1 = theDecPu->GetInputPorts(ipa);
		if(err1 == KErrNone && ipa.Count()) 
			{
			inputPort = ipa[0];
			}
		err2 = theDecPu->GetOutputPorts(opa);
		if(err2 == KErrNone && opa.Count()) 
			{
			outputPort = opa[0];
			}

		if(!inputPort || !outputPort) 
			{
			ERR_PRINTF1(_L("Error - couldn't get input/output ports"));
			iVerdict = EFail;
			}
		else 
			{
			// set port observers
			inputPort->MipSetObserver(*this);
			outputPort->MopSetObserver(*this);
			
			// configure
			TTaskConfig config;
			config.iRate = 0;
			config.iStereoMode = 0;
			TPuTaskConfig puConfig2(config);
			err = theDecPu->Configure(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't configure PU"));			
				iVerdict = EFail;
				}
			err = theDecPu->GetConfig(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't get PU configuration"));			
				iVerdict = EFail;
				}

			// create / free buffers
			CMMFBuffer* inputBuffer = NULL;
			CMMFBuffer* outputBuffer = NULL;
		
			// try and create oversized buffers
			inputBuffer = inputPort->MipCreateBuffer(20480000);
			if(inputBuffer) 
				{
				ERR_PRINTF1(_L("Error - oversize buffer creation was non-null"));			
				iVerdict = EFail;
				}
			outputBuffer = outputPort->MopCreateBuffer(0);
			if(outputBuffer) 
				{
				ERR_PRINTF1(_L("Error - zero buffer creation was non-null"));			
				iVerdict = EFail;
				}
		
			TUint32 inputBufSize = 2048;
			inputBufSize = inputPort->MipBufferSize();	
			inputBuffer = inputPort->MipCreateBuffer(inputBufSize);
			outputBuffer = outputPort->MopCreateBuffer(inputBufSize);
			if(!inputBuffer || !outputBuffer) 
				{
				ERR_PRINTF1(_L("Error - failed to create input or output buffer"));			
				iVerdict = EFail;
				}

			// direct port config
			err = inputPort->MipConfigure(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't configure input port"));			
				iVerdict = EFail;
				}
			err = inputPort->MipGetConfig(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't get input port configuration"));			
				iVerdict = EFail;
				}
			err = outputPort->MopConfigure(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't configure output port"));			
				iVerdict = EFail;
				}
			err = outputPort->MopGetConfig(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't get output port configuration"));			
				iVerdict = EFail;
				}	

			// --- get state
			theDecPu->State();
	
			// try and initialize twice; try and execute twice
			
			theDecPu->Initialize();
			theDecPu->Initialize();

			theDecPu->Execute();
			theDecPu->Execute();

			// try and action the ports when we're not running
			// NB we should give some delay here so we don't free them before
			// they are actioned
			theDecPu->Pause();
			inputPort->MipWriteData(*inputBuffer);
			outputPort->MopReadData(*outputBuffer);
			User::After(2000000L);
		
			theDecPu->Stop();
			inputPort->MipWriteData(*inputBuffer);
			outputPort->MopReadData(*outputBuffer);
			User::After(2000000L);
	
			err = inputPort->MipFreeBuffer(inputBuffer);
			err += outputPort->MopFreeBuffer(outputBuffer);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't free input/output buffer"));			
				iVerdict = EFail;
				}	

			opa.Close();
			ipa.Close();
			}
		}	

	// encoder
	if(theEncPu)
		{
		MMdfInputPort* inputPort = NULL;
		MMdfOutputPort* outputPort = NULL;

		err1 = theEncPu->GetInputPorts(ipa);
		if(err1 == KErrNone && ipa.Count()) 
			{
			inputPort = ipa[0];
			}
		err2 = theEncPu->GetOutputPorts(opa);
		if(err2 == KErrNone && opa.Count()) 
			{
			outputPort = opa[0];
			}

		if(!inputPort || !outputPort) 
			{
			ERR_PRINTF1(_L("Error - couldn't get input/output ports"));
			iVerdict = EFail;
			}
		else 
			{

			// configure
			TTaskConfig config;
			config.iRate = 0;
			config.iStereoMode = 0;
			TPuTaskConfig puConfig2(config);
			err = theEncPu->Configure(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't configure PU"));			
				iVerdict = EFail;
				}
			err = theEncPu->GetConfig(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't get PU configuration"));			
				iVerdict = EFail;
				}

			// create / free buffers
			CMMFBuffer* inputBuffer = NULL;
			CMMFBuffer* outputBuffer = NULL;
		
			// try and create oversized buffers
			inputBuffer = inputPort->MipCreateBuffer(20480000);
			if(inputBuffer) 
				{
				ERR_PRINTF1(_L("Error - oversize buffer creation was non-null"));			
				iVerdict = EFail;
				}
			outputBuffer = outputPort->MopCreateBuffer(0);
			if(outputBuffer) 
				{
				ERR_PRINTF1(_L("Error - zero buffer creation was non-null"));			
				iVerdict = EFail;
				}
		
			TUint32 inputBufSize = 2048;
			inputBufSize = inputPort->MipBufferSize();	
			inputBuffer = inputPort->MipCreateBuffer(inputBufSize);
			outputBuffer = outputPort->MopCreateBuffer(inputBufSize);
			if(!inputBuffer || !outputBuffer) 
				{
				ERR_PRINTF1(_L("Error - failed to create input or output buffer"));			
				iVerdict = EFail;
				}

			// direct port config
			err = inputPort->MipConfigure(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't configure input port"));			
				iVerdict = EFail;
				}
			err = inputPort->MipGetConfig(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't get input port configuration"));			
				iVerdict = EFail;
				}
			err = outputPort->MopConfigure(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't configure output port"));			
				iVerdict = EFail;
				}
			err = outputPort->MopGetConfig(puConfig2);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't get output port configuration"));			
				iVerdict = EFail;
				}	

			// --- get state
			theEncPu->State();
	
			// try and initialize twice; try and execute twice
			
			theEncPu->Initialize();
			theEncPu->Initialize();

			theEncPu->Execute();
			theEncPu->Execute();

			// try and action the ports when we're not running
			// NB we should give some delay here so we don't free them before
			// they are actioned
	
			// *** there is a problem with this at the moment - 
			// VorbisEncoder will kern-exec:3 when passed garbage data
			// directly. To be resolved before this test goes back in.
			/*	
			CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(inputBuffer);
		    TUint8* data_pointer = const_cast<TUint8*>(dataBuffer->Data().Ptr());
			memset(data_pointer, 0, dataBuffer->Data().MaxLength());
			dataBuffer = static_cast<CMMFDataBuffer*>(outputBuffer);
		    data_pointer = const_cast<TUint8*>(dataBuffer->Data().Ptr());
			memset(data_pointer, 0, dataBuffer->Data().MaxLength());
		
			theEncPu->Pause();
			inputPort->MipWriteData(*inputBuffer);
			outputPort->MopReadData(*outputBuffer);
			User::After(2000000L);
			*/
	
			err = inputPort->MipFreeBuffer(inputBuffer);
			err += outputPort->MopFreeBuffer(outputBuffer);
			if(err) 
				{
				ERR_PRINTF1(_L("Error - couldn't free input/output buffer"));			
				iVerdict = EFail;
				}	

			opa.Close();
			ipa.Close();
			}
		}	
		
	delete theDecPu;
	delete theEncPu;
	OMX_Deinit();
	REComSession::FinalClose();
	return iVerdict;
	}	

// end
