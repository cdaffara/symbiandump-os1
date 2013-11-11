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

#include "tzorder.h"
#include "twindow.h"

_LIT(KExecutableFileName,"glplanets.exe");
_LIT(KTestStep0004,"GRAPHICS-UI-BENCH-S60-0004");

_LIT(KRed, "red");
_LIT(KGreen, "green");
_LIT(KBlue, "blue");
_LIT(KParentQueueName, "zorder");

const TPtrC childQueueNames[KNumberOfProcesses] =
    {
    KRed(),
    KGreen(),
    KBlue()
    };

const TInt KMessageSlots = 4;

const TInt KBringToFront = 1;
const TInt KTerminate = 2;

const TInt KIterations = 10;

CTZOrder::CTZOrder()
	{
	SetTestStepName(KTZOrder);
	}

CTZOrder::~CTZOrder()
	{
	for (TInt p = 0; p < KNumberOfProcesses; p++)
		{
		iChildMsgQueues[p].Close();
		}
	iParentMsgQueue.Close();
	}

TVerdict CTZOrder::doTestStepPreambleL()
	{
	CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	TESTNOERRORL(iParentMsgQueue.CreateGlobal(KParentQueueName, KMessageSlots));
	for (TInt p = 0; p < KNumberOfProcesses; p++)
		{
		TESTNOERRORL(iChildMsgQueues[p].CreateGlobal(childQueueNames[p], KMessageSlots));
		RProcess process;
		// The child queue name is passed as argument, parent queue name is fixed
		TBuf<32> commandLine;
		commandLine.Append(childQueueNames[p]);
		TESTNOERRORL(process.Create(KExecutableFileName, commandLine));
		TRequestStatus status;
		process.Rendezvous(status); // Must be done before the process is started
		process.Resume();
		User::WaitForRequest(status); // Wait for process to be ready
		TESTNOERRORL(status.Int());
		process.Close();
		}
	return TestStepResult();
	}

TVerdict CTZOrder::doTestStepL()
    {
    SetTestStepID(KTestStep0004);
    zOrderSwitchingL();
    return TestStepResult();
    }

/**
@SYMTestCaseID
GRAPHICS-UI-BENCH-S60-0004

@SYMTestCaseDesc
Tests how long it takes for application's ordinal to change to the front and notification for 'KNumberOfProcesses'

@SYMTestActions
Change application ordinal to the front and measure average time it takes until it responds 

@SYMTestExpectedResults
Test should pass and display the average framerate for the whole test.
*/
void CTZOrder::zOrderSwitchingL()
	{
	iProfiler->InitResults();
    for (TInt t = 0; t < KIterations; t++)
		{
		for (TInt p = 0; p < KNumberOfProcesses; p++)
			{
			iChildMsgQueues[p].SendBlocking(KBringToFront);
			TInt ticks = 0;
			iParentMsgQueue.ReceiveBlocking(ticks);
			}
		}

	// tell all the processes to commit suicide
    TInt response;
	for (TInt p = 0; p < KNumberOfProcesses; p++)
		{
		iChildMsgQueues[p].SendBlocking(KTerminate);
		iParentMsgQueue.ReceiveBlocking(response);
		}
	
	iProfiler->MarkResultSetL();
	TSize screenSize = CTWindow::GetDisplaySizeInPixels();
	iProfiler->ResultsAnalysisZorderSwitchingRate(KTestStep0004, 0, 0, 0, KIterations, screenSize.iWidth * screenSize.iHeight);
	}
