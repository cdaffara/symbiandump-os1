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
 @test
 @internalComponent - Internal Symbian test code 
*/


#include "tzordermultiplesurfaces.h"
#include "twindow.h"


_LIT(KTestStep0005, "GRAPHICS-UI-BENCH-S60-0005");

_LIT(KOpenGLExecutableFileName, "glplanets.exe");
_LIT(KMultiExecutableFileName, "multiplesurfaces.exe");
const TPtrC childExecutableNames[KNumberOfMultiSurfacesProcesses] =
    {
    KOpenGLExecutableFileName(),
    KMultiExecutableFileName()
    };

_LIT(KOpenGL, "opengl");
_LIT(KMutipleSurfaces, "multi");
_LIT(KParentQueueName, "zorder");
const TPtrC childQueueNames[KNumberOfMultiSurfacesProcesses] =
    {
    KOpenGL(),
    KMutipleSurfaces()
    };
    
const TInt KMessageSlots = 4;

const TInt KBringToFront = 1;
const TInt KTerminate = 2;

const TInt KIterations = 10;


CTZOrderMultipleSurfaces::CTZOrderMultipleSurfaces()
	{
	SetTestStepName(KTZOrderMultipleSurfaces);
	}

CTZOrderMultipleSurfaces::~CTZOrderMultipleSurfaces()
	{
	for (TInt p = 0; p < KNumberOfMultiSurfacesProcesses; p++)
		{
		iChildMsgQueues[p].Close();
		}
	iParentMsgQueue.Close();
	}

TVerdict CTZOrderMultipleSurfaces::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	TESTNOERRORL(iParentMsgQueue.CreateGlobal(KParentQueueName, KMessageSlots));
	for (TInt p = 0; p < KNumberOfMultiSurfacesProcesses; ++p)
		{
		TESTNOERRORL(iChildMsgQueues[p].CreateGlobal(childQueueNames[p], KMessageSlots));
		TBuf<32> commandLine;
		commandLine.Append(childQueueNames[p]);
		RProcess process;
		TESTNOERRORL(process.Create(childExecutableNames[p], commandLine));TRequestStatus status;
        process.Rendezvous(status); // Must be done before the process is started
        process.Resume();
        User::WaitForRequest(status); // Wait for process to be ready
        TESTNOERRORL(status.Int());
        process.Close();
		}	
	return TestStepResult();
	}

TVerdict CTZOrderMultipleSurfaces::doTestStepL()
    {
    SetTestStepID(KTestStep0005);
    zOrderMultipleSurfacesSwitchingL();
    return TestStepResult();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0005

@SYMTestCaseDesc
Tests how long it takes for application's ordinal to change to the front and notification for 'KNumberOfMultiSurfacesProcesses'

@SYMTestActions
Change application ordinal to the front and measure average time it takes until it responds 

@SYMTestExpectedResults
Test should pass and display the average framerate for the whole test.
*/
void CTZOrderMultipleSurfaces::zOrderMultipleSurfacesSwitchingL()
    {    
    iProfiler->InitResults();
    for (TInt t = 0; t < KIterations; ++t)
        {
        for (TInt p = 0; p < KNumberOfMultiSurfacesProcesses; ++p)
            {
            iChildMsgQueues[p].SendBlocking(KBringToFront);
            TInt ticks = 0;
            iParentMsgQueue.ReceiveBlocking(ticks);
            }
        }

    TInt response;
    for (TInt p = 0; p < KNumberOfMultiSurfacesProcesses; ++p)
        {
        iChildMsgQueues[p].SendBlocking(KTerminate);
        iParentMsgQueue.ReceiveBlocking(response);
        }
    
    iProfiler->MarkResultSetL();
    TSize screenSize = CTWindow::GetDisplaySizeInPixels();
    iProfiler->ResultsAnalysisZorderSwitchingRate(KTestStep0005, 0, 0, 0, KIterations, screenSize.iWidth * screenSize.iHeight);
    }
