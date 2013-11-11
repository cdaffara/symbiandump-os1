// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "teventhandlingperf.h"
#include <hal.h>

// Max length of the descriptor used for results of performance tests  
// which are actually done in separate process. And UIBench testcase is 
// used to display them according its format
// 16 integers plus 8 commas i,e 32 * 16 + 16
const TInt KMaxDescLength = 528;

CTEventHandlingPerf::CTEventHandlingPerf()
	{
	SetTestStepName(KEventHandlingPerfName);
	}

CTEventHandlingPerf::~CTEventHandlingPerf()
	{
	}

TVerdict CTEventHandlingPerf::doTestStepL()
	{
/**
@SYMTestCaseID GRAPHICS-UI-BENCH-0172
*/
	SetTestStepID(_L("GRAPHICS-UI-BENCH-0172"));
	TBuf<KMaxDescLength> bufAvg;
	TPtr temp(NULL,0);
	TPtr temp1(NULL,0);
	TPtr temp2(NULL,0);
	// Here check if the HAL configuration are correct if not then finish the test case
	TInt maxPtr;
	TInt ret = HAL::Get(HALData::EPointerMaxPointers, maxPtr);
	if (ret != KErrNone || maxPtr < 2 || maxPtr > 8)
		{
		INFO_PRINTF1(_L("Incorrect HAL configuration. \n"));
		SetTestStepResult(EFail);
	    CloseTMSGraphicsStep();
		RecordTestResultL();
		return TestStepResult();
		}
		
	iProfiler->InitResults();
	TInt minMemSize = 128;
	TInt maxMemSize = KMaxDescLength;
	// Create a shared chunk using the userheap function
	// Allocate some memory in that and then get the offset from it - chunk's base
	_LIT(KPerformanceTimingChunk, "PerformanceTimingChunk");
	RHeap* heap = UserHeap::ChunkHeap(&KPerformanceTimingChunk, minMemSize, maxMemSize, 10);
	CleanupClosePushL(*heap);
	if (heap == NULL)
		{
		User::LeaveNoMemory();
		}
	RChunk chunk;
	User::LeaveIfError(chunk.OpenGlobal(KPerformanceTimingChunk, ETrue));
	CleanupClosePushL(chunk);
	
	TInt memSize = KMaxDescLength;
	TAny* perfDesc = heap->AllocL(memSize);
	if (!perfDesc)
		{
		User::LeaveNoMemory();
		}
	TInt offset = reinterpret_cast<TUint8*>(perfDesc) - chunk.Base() ;

	// Create a process called te_multiptrperf
	// Pass in the offset as descriptor
	_LIT(KMultiPtrEventHandlingPerf,"z:\\sys\\bin\\te_multiptrperf.exe");
	TBuf<128> buf;
	buf.Num(offset);
	RProcess eventHandPerf;
	User::LeaveIfError(eventHandPerf.Create(KMultiPtrEventHandlingPerf, buf));
	TRequestStatus stat;
	eventHandPerf.Logon(stat);
	eventHandPerf.Resume();
	User::WaitForRequest(stat);
	eventHandPerf.Close();
	
	// Once the process finished its execution print the result by 
	// reading the data from chunk's memory
	TPtr8 ptrDes((TUint8*)perfDesc, memSize, memSize);
	TPtr8 ptrDesDisplay((TUint8*)perfDesc, memSize, memSize);

	// If it has failed then just print description written in the same
	// when it is failed description contains '*' at the end
	TInt lastChar = 0;
	lastChar = ptrDes.Locate('*');
	if (KErrNotFound != lastChar)
		{
		SetTestStepResult(EFail);
		TPtr8 temp = ptrDesDisplay.LeftTPtr(lastChar);
		TBuf<128> buf;
		buf.Copy(temp);
		INFO_PRINTF2(_L("%S"), &buf);
		RDebug::RawPrint(temp);
		goto END;
		}
	// If it has skipped then just print skip description from the chunk
	// when it is skipped description contains '#' at the end
    lastChar = ptrDes.Locate('#');
    if (KErrNotFound != lastChar)
        {
        SetTestStepResult(EPass);
        TPtr8 temp = ptrDesDisplay.LeftTPtr(lastChar);
        TBuf<128> buf;
        buf.Copy(temp);
        INFO_PRINTF2(_L("%S"), &buf);
        RDebug::RawPrint(temp);
        goto END;
        }
	
	// If the every thing goes fine the descriptor is displayed as
	// "12345678,123456789,123456789"
	TInt avg4Events, avg8Events, avg16Events, avg32Events;
	bufAvg.Copy(ptrDesDisplay);
	for (TInt i=0; i<4; i++)
		{
		TInt locate = bufAvg.Locate(',');
		if (locate == KErrNotFound)
			{
			SetTestStepResult(EFail);
			goto END;
			}
		TLex lex(bufAvg.Left(locate));
		lex.Val(avg4Events);
	
		locate++;
		temp.Set(bufAvg.MidTPtr(locate));
		locate = temp.Locate(',');
		if (locate == KErrNotFound)
			{
			SetTestStepResult(EFail);
			goto END;
			}
		lex = temp.Left(locate);
		lex.Val(avg8Events);
	
		locate++;
		temp1.Set(temp.MidTPtr(locate));
		locate = temp1.Locate(',');
		if (locate == KErrNotFound)
			{
			SetTestStepResult(EFail);
			goto END;
			}	
		lex = temp.Left(locate);
		lex.Val(avg16Events);
		
		locate++;
		temp2.Set(temp1.MidTPtr(locate));
		locate = temp2.Locate(',');
		if (locate == KErrNotFound)
			{
			SetTestStepResult(EFail);
			goto END;
			}	
		lex = temp2.Left(locate);
		lex.Val(avg32Events);
		
		locate++;
		bufAvg.Copy(temp2.MidTPtr(locate));
		
		switch (i)
			{
			case 0:
				INFO_PRINTF5(_L("TID: Average time for Single pointer(wait after each event) for 4 events:%i  8 events:%i  16 events:%i  32events:%i"), avg4Events, avg8Events, avg16Events, avg32Events);
				break;
			case 1:
				INFO_PRINTF5(_L("TID: Average time for Multi pointer(wait after each event) for 4 events:%i  8 events:%i  16 events:%i  32events:%i"), avg4Events, avg8Events, avg16Events, avg32Events);
				break;
			case 2:
				INFO_PRINTF5(_L("TID: Average time for single pointer for 4 events:%i  8 events:%i  16 events:%i  32events:%i"), avg4Events, avg8Events, avg16Events, avg32Events);
				break;
			case 3:
				INFO_PRINTF5(_L("TID: Average time for Multi pointer for 4 events:%i  8 events:%i  16 events:%i  32events:%i"), avg4Events, avg8Events, avg16Events, avg32Events);
				SetTestStepResult(EPass);
				break;
			default:
				break;
			}
		}
	
	iProfiler->ResultsAnalysis(KEventHandlingPerfName, 0, EColor16MA, EColor16MA, 1);
	
END:	
	// Once the data is printed or displyed delete the memory
	heap->Free(perfDesc);
	
	CleanupStack::PopAndDestroy(2, heap);
	RecordTestResultL();
    CloseTMSGraphicsStep();
	return TestStepResult();
	}

TVerdict CTEventHandlingPerf::doTestStepPreambleL()
	{
	return CTe_graphicsperformanceSuiteStepBase::doTestStepPreambleL();
	}

TVerdict CTEventHandlingPerf::doTestStepPostambleL()
	{
	return CTe_graphicsperformanceSuiteStepBase::doTestStepPostambleL();
	}
