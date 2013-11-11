// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// EPOC includes
#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>

// Test system includes
#include <testframework.h>

#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>

#include <mmf/common/mmfcontroller.h>

#include <videorecorder.h>

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"

// Specific includes for these test steps
#include "TestStepVidRecorder.h"

// --------------------------------------------

/**
 *
 * Static constructor for CTestStepVidRecorderNew.
 *
 *
 * @return	"CTestStepVidRecorderNew*"
 *			The constructed CTestStepVidRecorderNew
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderNew* CTestStepVidRecorderNew::NewL()
	{
	CTestStepVidRecorderNew* self = new(ELeave) CTestStepVidRecorderNew;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderNew::CTestStepVidRecorderNew() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0200-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderNew::~CTestStepVidRecorderNew()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderNew::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderNew::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderNew::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderNew::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}



/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderNew::DoTestStepL()
	{
	__MM_HEAP_MARK;
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0200-CP"));
	INFO_PRINTF1(_L("this test is iniatilising a CVideoRecorderUtility object with NewL()"));

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				iTestStepResult = EFail;

	delete iRecorder;
	iRecorder = NULL;

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

