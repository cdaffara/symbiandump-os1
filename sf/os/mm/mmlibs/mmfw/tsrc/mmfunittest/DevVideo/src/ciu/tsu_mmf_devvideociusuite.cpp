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

#include <e32base.h>
#include <ecom/ecom.h>
// Test system includes
#include <testframework.h>
#include "tsu_mmf_devvideociu.h"
#include "tsu_mmf_devvideociusuite.h"
#include "testdevvideociuplay.h"
#include "testdevvideociurecord.h"
		    

/**
 * NewTestSuite is exported at ordinal 1
 * this provides the interface to allow schedule test
 * to create instances of this test suite
 */
EXPORT_C CTestSuite* NewTestSuiteL()
    {
	CTestMmfDevVideoCiuSuite* result = new (ELeave) CTestMmfDevVideoCiuSuite;
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/**
 * destructor
 */
CTestMmfDevVideoCiuSuite::~CTestMmfDevVideoCiuSuite()
	{
	}

/**
 * make a version string available for test system
 * @return version string
 */
TPtrC CTestMmfDevVideoCiuSuite::GetVersion()
 	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
 * Constructor for test suite
 * this creates all the test steps and 
 * stores them inside CTestMmfDevVideoCiuSuite 
 */
void CTestMmfDevVideoCiuSuite::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("MMF_DEVVIDEOCI");

// POSITIVE TEST
	// DevVideoPlay
	AddTestStepL( CTestDevVideoCiuPlayCreateVideoMediaProtected::NewL( _L("MM-MMF-DEVVIDEOCI-U-0001-HP")));
	AddTestStepL( CTestDevVideoCiuPlaySetVideoMediaProtected::NewL( _L("MM-MMF-DEVVIDEOCI-U-0002-CP")));

	AddTestStepL( CTestDevVideoCiuPlayCreateColourSpace::NewL( _L("MM-MMF-DEVVIDEOCI-U-0003-HP")));
	AddTestStepL( CTestDevVideoCiuPlayGetColourSpace::NewL( _L("MM-MMF-DEVVIDEOCI-U-0004-CP")));

	AddTestStepL( CTestDevVideoCiuPlayCreatePolicyExtension::NewL( _L("MM-MMF-DEVVIDEOCI-U-0005-CP")));
	AddTestStepL( CTestDevVideoCiuPlaySetClientPolicyExtension::NewL( _L("MM-MMF-DEVVIDEOCI-U-0006-CP")));
	AddTestStepL( CTestDevVideoCiuPlaySetPriorityPolicyExtension::NewL( _L("MM-MMF-DEVVIDEOCI-U-0007-CP")));
	AddTestStepL( CTestDevVideoCiuPlayClientNotifierRevoked::NewL( _L("MM-MMF-DEVVIDEOCI-U-0008-CP")));
	AddTestStepL( CTestDevVideoCiuPlayClientNotifierResource::NewL( _L("MM-MMF-DEVVIDEOCI-U-0009-CP")));

    
	AddTestStepL( CTestDevVideoCiuRecCreateVideoInputSnapshotPreProc::NewL( _L("MM-MMF-DEVVIDEOCI-U-0101-CP")));
	AddTestStepL( CTestDevVideoCiuRecSetObserverVideoInputSnapshotPreProc::NewL( _L("MM-MMF-DEVVIDEOCI-U-0102-CP")));
	AddTestStepL( CTestDevVideoCiuRecGetSnapshotVideoInputSnapshotPreProc::NewL( _L("MM-MMF-DEVVIDEOCI-U-0103-CP")));
	AddTestStepL( CTestDevVideoCiuRecCancelSnapshotVideoInputSnapshotPreProc::NewL( _L("MM-MMF-DEVVIDEOCI-U-0104-CP")));

	AddTestStepL( CTestDevVideoCiuRecSnapshotCompleteVideoInputSnapshotPreProc::NewL( _L("MM-MMF-DEVVIDEOCI-U-0105-CP")));

	AddTestStepL( CTestDevVideoCiuRecCreateVideoBufferManagement::NewL( _L("MM-MMF-DEVVIDEOCI-U-0106-CP")));
	AddTestStepL( CTestDevVideoCiuRecSetObserverVideoBufferManagement::NewL( _L("MM-MMF-DEVVIDEOCI-U-0107-CP")));
	AddTestStepL( CTestDevVideoCiuRecEnableVideoBufferManagement::NewL( _L("MM-MMF-DEVVIDEOCI-U-0108-CP")));
	AddTestStepL( CTestDevVideoCiuRecSetOptionsVideoBufferManagement::NewL( _L("MM-MMF-DEVVIDEOCI-U-0109-CP")));
	AddTestStepL( CTestDevVideoCiuRecGetOptionsVideoBufferManagement::NewL( _L("MM-MMF-DEVVIDEOCI-U-0110-CP")));
	AddTestStepL( CTestDevVideoCiuRecGetBufferVideoBufferManagement::NewL( _L("MM-MMF-DEVVIDEOCI-U-0111-CP")));
	AddTestStepL( CTestDevVideoCiuRecReleaseBufferVideoBufferManagement::NewL( _L("MM-MMF-DEVVIDEOCI-U-0112-CP")));
	AddTestStepL( CTestDevVideoCiuRecNewBuffersVideo::NewL( _L("MM-MMF-DEVVIDEOCI-U-0113-CP")));
	AddTestStepL( CTestDevVideoCiuRecReleaseBuffersVideo::NewL( _L("MM-MMF-DEVVIDEOCI-U-0114-CP")));

	AddTestStepL( CTestDevVideoCiuPlayGetAndSetLogicalWindow::NewL(_L("MM-MMF-DEVVIDEOCI-U-0115-CP")));
	AddTestStepL( CTestDevVideoCiuPlayGetAndSetContentOffset::NewL(_L("MM-MMF-DEVVIDEOCI-U-0116-CP")));
	AddTestStepL( CTestDevVideoCiuPlayGetAndSetContentAlignment::NewL(_L("MM-MMF-DEVVIDEOCI-U-0117-CP")));

	AddTestStepL( CTestDevVideoCiuPlayGetAndSetResourceObserver::NewL(_L("MM-MMF-DEVVIDEOCI-U-0118-CP")));
	AddTestStepL( CTestDevVideoCiuPlayResourceLost::NewL(_L("MM-MMF-DEVVIDEOCI-U-0119-CP")));
	AddTestStepL( CTestDevVideoCiuPlayResourceRestored::NewL(_L("MM-MMF-DEVVIDEOCI-U-0120-CP")));
	AddTestStepL( CTestDevVideoCiuPlayGetAndSetResourcePriority::NewL(_L("MM-MMF-DEVVIDEOCI-U-0121-CP")));
	AddTestStepL( CTestDevVideoCiuPlaySetAndGetPropertiesObserver::NewL(_L("MM-MMF-DEVVIDEOCI-U-0122-CP")));
	AddTestStepL( CTestDevVideoCiuPlayUpdateProperties::NewL(_L("MM-MMF-DEVVIDEOCI-U-0123-CP")));	

#ifdef SYMBIAN_BUILD_GCE
	// DevVideoPlay Surface support
	AddTestStepL( CTestDevVideoCiuPlayGetAndSetUseSurface::NewL(_L("MM-MMF-DEVVIDEOCI-U-0225-CP")));
	AddTestStepL( CTestDevVideoCiuPlayGetAndSetSurfaceParameters::NewL(_L("MM-MMF-DEVVIDEOCI-U-0227-CP")));
	AddTestStepL( CTestDevVideoCiuPlayGetAndSetSurfaceRemoved::NewL(_L("MM-MMF-DEVVIDEOCI-U-0229-CP")));

	AddTestStepL( CTestDevVideoCiuPlaySetAndGetSurfaceSupportObserver::NewL(_L("MM-MMF-DEVVIDEOCI-U-0230-CP")));
	AddTestStepL( CTestDevVideoCiuPlaySurfaceCreated::NewL(_L("MM-MMF-DEVVIDEOCI-U-0231-CP")));
	AddTestStepL( CTestDevVideoCiuPlaySurfaceParametersChanged::NewL(_L("MM-MMF-DEVVIDEOCI-U-0232-CP")));
	AddTestStepL( CTestDevVideoCiuRemoveSurface::NewL(_L("MM-MMF-DEVVIDEOCI-U-0234-CP")));
	
	// DevVideoPlay Client Buffer Support
	AddTestStepL( CTestDevVideoCiuPlayUseClientBuffers::NewL(_L("MM-MMF-DEVVIDEOCI-U-0240-CP")));
	AddTestStepL( CTestDevVideoCiuPlaySupplyBuffer::NewL(_L("MM-MMF-DEVVIDEOCI-U-0241-CP")));
	AddTestStepL( CTestDevVideoCiuPlayReleaseBuffers::NewL(_L("MM-MMF-DEVVIDEOCI-U-0242-CP")));



#endif // SYMBIAN_BUILD_GCE

	// DevVideoPlay play rate control
	AddTestStepL( CTestDevVideoCiuPlayGetAndSetPlayRateCapabilities::NewL(_L("MM-MMF-DEVVIDEOCI-U-0300-CP")));
	AddTestStepL( CTestDevVideoCiuPlaySetAndGetPlayRate::NewL(_L("MM-MMF-DEVVIDEOCI-U-0301-CP")));
	AddTestStepL( CTestDevVideoCiuPlayGetAndSetStepFrame::NewL(_L("MM-MMF-DEVVIDEOCI-U-0302-CP")));
	AddTestStepL( CTestDevVideoCiuPlaySetAndGetPlayRateObserver::NewL(_L("MM-MMF-DEVVIDEOCI-U-0303-CP")));
	AddTestStepL( CTestDevVideoCiuPlayStepFrameComplete::NewL(_L("MM-MMF-DEVVIDEOCI-U-0304-CP")));
	AddTestStepL( CTestDevVideoCiuPlayKeyFrameModeRequest::NewL(_L("MM-MMF-DEVVIDEOCI-U-0305-CP")));

#ifdef SYMBIAN_BUILD_GCE

	//SurfaceHandleControl
	AddTestStepL( CTestDevVideoCiuPlaySurfaceHandleControl::NewL(_L("MM-MMF-DEVVIDEOCI-U-0311-CP")));
	AddTestStepL( CTestDevVideoCiuPlaySurfaceRedrawBuffer::NewL(_L("MM-MMF-DEVVIDEOCI-U-0312-CP")));

#endif // SYMBIAN_BUILD_GCE

	// MMmfVideoHwDeviceTunnel
	AddTestStepL(CTestDevVideoCiuHwDeviceTunnel::NewL(_L("MM-MMF-DEVVIDEOCI-U-0320-CP")));

	// DevVideoRecord
	AddTestStepL( CTestDevVideoCiuRecCreateVideoH324AnnexKModeEncoder::NewL(_L("MM-MMF-DEVVIDEOCI-U-0124-CP")));
	AddTestStepL( CTestDevVideoCiuRecSetVideoH324AnnexKModeEncoder::NewL(_L("MM-MMF-DEVVIDEOCI-U-0125-CP")));

// NEGATIVE TEST
	// DevVideoPlay
	AddTestStepL( CTestDevVideoCiuPlayCreateVideoMediaProtectedDecoder::NewL( _L("MM-MMF-DEVVIDEOCI-U-1001-LP")));
	AddTestStepL( CTestDevVideoCiuRecCreateVideoMediaProtectedPreProc::NewL( _L("MM-MMF-DEVVIDEOCI-U-1002-LP")));
	AddTestStepL( CTestDevVideoCiuRecCreateVideoMediaProtectedEncoder::NewL( _L("MM-MMF-DEVVIDEOCI-U-1003-LP")));
	}
