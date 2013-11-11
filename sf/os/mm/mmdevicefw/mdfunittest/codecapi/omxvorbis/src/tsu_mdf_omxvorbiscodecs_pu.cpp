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

#include "tsu_mdf_omxvorbiscodecs_pu.h"
#include <openmax/il/khronos/v1_x/OMX_Core.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mdf/mdfpuconfig.h>
#include "PU/decoder/vorbisdecoderprocessingunit.h"
#include "PU/vorbisprocessingunituids.hrh"
// for the bitrate custom interface
#include <mmf/server/devsoundstandardcustominterfaces.h>

#include <e32debug.h>

//
// RTestStepOmxVorbisCodecPu
// general tests relating to Processing Unit

/**
 * Constructor
 */
RTestStepOmxVorbisCodecPu::RTestStepOmxVorbisCodecPu()
 	{
 	iTestStepName = _L("MM-MDF-OMXVORBISCODECS-U-0010-HP");
	iHeapSize = KTestHeapSize;
 	}

/**
 * Do the test step
 */
TVerdict RTestStepOmxVorbisCodecPu::DoTestStepL()
	{
	iVerdict = EPass;

	// try and create PUs with invalid observers. it should succeed.
	
	_LIT8(KTestGarbage, "I am Garbage");
	const TDesC8& testGarbageDes = KTestGarbage;
	const MMdfProcessingUnitObserver* garbageObs = NULL;
	
	// NB: we *deliberately* use an unsafe cast
	garbageObs = (const MMdfProcessingUnitObserver*)&testGarbageDes;
	  
	CMdfProcessingUnit* theDecPu = NULL;
	CMdfProcessingUnit* theEncPu = NULL;
	
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
		ERR_PRINTF1(_L("Error - couldn't initialise PUs"));
		delete theDecPu;
		delete theEncPu;
		OMX_Deinit();
		REComSession::FinalClose();
		return EFail;	
		}
		
	// --- test : configure the ports with different params

	RPointerArray<MMdfInputPort> ipa;
	RPointerArray<MMdfOutputPort> opa;
	TTaskConfig config;

	// decoder tests
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
			}
		else 
			{
			config.iRate = 0;
			config.iStereoMode = 0;
	
			TPuTaskConfig puConfig(config);
			err = inputPort->MipConfigure(puConfig);
			if(err != KErrNone)
				{
				ERR_PRINTF1(_L("Error - couldn't configure input port (1)"));
				iVerdict = EFail;
				}
			err = outputPort->MopConfigure(puConfig);
			if(err != KErrNone)
				{
				ERR_PRINTF1(_L("Error - couldn't configure output port (1)"));
				iVerdict = EFail;
				}
		
			config.iStereoMode |= ETaskMono;
			config.iStereoMode |= ETaskInterleaved;
	
			TPuTaskConfig puConfig2(config);
			err = inputPort->MipConfigure(puConfig2);
			if(err != KErrNone)
				{
				ERR_PRINTF1(_L("Error - couldn't configure input port (2)"));
				iVerdict = EFail;
				}
			err = outputPort->MopConfigure(puConfig2);
			if(err != KErrNone)
				{
				ERR_PRINTF1(_L("Error - couldn't configure output port (2)"));
				iVerdict = EFail;
				}
		
			// now cast a different type so that it's not recognised	
			TPuConfig* puConfig3 = (TPuConfig*)&testGarbageDes;
			err = inputPort->MipConfigure(*puConfig3);
			if(err != KErrNotSupported)
				{
				ERR_PRINTF2(_L("Error - configure input port (3) returned %d"), err);
				iVerdict = EFail;
				}
			err = outputPort->MopConfigure(*puConfig3);
			if(err != KErrNotSupported)
				{
				ERR_PRINTF2(_L("Error - configure output port (3) returned %d"), err);
				iVerdict = EFail;
				}
		
			opa.Close();
			ipa.Close();
			}	
		}
		
	// encoder tests
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
			}
		else 
			{
			config.iRate = 0;
			config.iStereoMode = 0;
	
			TPuTaskConfig puConfig(config);
			err = inputPort->MipConfigure(puConfig);
			if(err != KErrNone)
				{
				ERR_PRINTF1(_L("Error - couldn't configure input port (1)"));
				iVerdict = EFail;
				}
			err = outputPort->MopConfigure(puConfig);
			if(err != KErrNone)
				{
				ERR_PRINTF1(_L("Error - couldn't configure output port (1)"));
				iVerdict = EFail;
				}
		
			config.iStereoMode |= ETaskMono;
			config.iStereoMode |= ETaskInterleaved;
	
			TPuTaskConfig puConfig2(config);
			err = inputPort->MipConfigure(puConfig2);
			if(err != KErrNone)
				{
				ERR_PRINTF1(_L("Error - couldn't configure input port (2)"));
				iVerdict = EFail;
				}
			err = outputPort->MopConfigure(puConfig2);
			if(err != KErrNone)
				{
				ERR_PRINTF1(_L("Error - couldn't configure output port (2)"));
				iVerdict = EFail;
				}
		
			// now cast a different type so that it's not recognised	
			TPuConfig* puConfig3 = (TPuConfig*)&testGarbageDes;
			err = inputPort->MipConfigure(*puConfig3);
			if(err != KErrNotSupported)
				{
				ERR_PRINTF2(_L("Error - configure input port (3) returned %d"), err);
				iVerdict = EFail;
				}
			err = outputPort->MopConfigure(*puConfig3);
			if(err != KErrNotSupported)
				{
				ERR_PRINTF2(_L("Error - configure output port (3) returned %d"), err);
				iVerdict = EFail;
				}
		
			opa.Close();
			ipa.Close();
			}	
		}

	// --- test : custom interface on encoder
	
	// invalid uid
	TUid garbageUid = {0x12345678};
	MMMFDevSoundCustomInterfaceBitRate* bitrateCI = (MMMFDevSoundCustomInterfaceBitRate*)theEncPu->CustomInterface(garbageUid);
	if (bitrateCI != NULL)
		{
		ERR_PRINTF2(_L("Error - CustomInterface returned 0x%08x"), bitrateCI);
		iVerdict = EFail;
		}
	
	err = theEncPu->CreateCustomInterface(garbageUid);
	if (err == KErrNone)
		{
		ERR_PRINTF2(_L("Error - CreateCustomInterface returned %d"), err);
		iVerdict = EFail;
		}

	// create
	err = theEncPu->CreateCustomInterface(KUidCustomInterfaceDevSoundBitRate);
	if (err != KErrNone)
		{
		ERR_PRINTF1(_L("Error - couldn't create custom interface"));
		iVerdict = EFail;
		}
	
	// fetch & use
	bitrateCI = (MMMFDevSoundCustomInterfaceBitRate*)theEncPu->CustomInterface(KUidCustomInterfaceDevSoundBitRate);
	if (bitrateCI == NULL)
		{
		ERR_PRINTF1(_L("Error - couldn't get custom interface"));
		iVerdict = EFail;
		}
	else 
		{
		TRAP(err, bitrateCI->SetBitRateL(KTestBitRate));			
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't set bitrate"));
			iVerdict = EFail;
			}
			
		TInt thisBitRate = 0;	
		TRAP(err, thisBitRate = bitrateCI->BitRateL());			
		if (err != KErrNone || thisBitRate > KTestBitRate)
			{
			ERR_PRINTF1(_L("Error - couldn't get bitrate, or bitrate incorrect"));
			iVerdict = EFail;
			}
			
		RArray<TInt> bitRates;
		TRAP(err, bitrateCI->GetSupportedBitRatesL(bitRates));			
		if (err != KErrNone)
			{
			ERR_PRINTF1(_L("Error - couldn't get supported bitrates"));
			iVerdict = EFail;
			}
		TInt count = bitRates.Count();	
		if(count <= 0) 
			{
			ERR_PRINTF1(_L("Error - zero supported bitrates"));
			iVerdict = EFail;
			}
		bitRates.Close();	
		
		}
					
	
	// ---
		
	delete theDecPu;
	delete theEncPu;
	OMX_Deinit();
	
	REComSession::FinalClose();
	return iVerdict;
	}	

// end
