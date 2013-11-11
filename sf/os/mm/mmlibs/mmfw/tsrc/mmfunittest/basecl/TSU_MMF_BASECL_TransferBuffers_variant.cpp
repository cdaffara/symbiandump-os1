// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Test system includes
#include "TSU_MMF_BASECL_Buffers.h"
#include "TSU_MMF_BASECLSuite.h"

//Tests 20-99 Reserved for future non 0 copy buffer types eg Bitmap, YUV, Audio
//These are the EKA2 variants for CMMFTransferBuffer


CTestStep_MMF_BASECL_TransferBuffer::CTestStep_MMF_BASECL_TransferBuffer() : iTestData(0,0,0)
	{
	}


CTestStep_MMF_BASECL_TransferBuffer::~CTestStep_MMF_BASECL_TransferBuffer()
	{
	}

/**
 * Pre-amble for transfer buffer test step
 * Sets __MM_HEAP_MARK for memory leak detection
 * Connects to the transfer buffer tester server
 * Uses the transfer buffer tester server to create a buffer in the server process
 * and map it into the client transfer window
 */
TVerdict CTestStep_MMF_BASECL_TransferBuffer::DoTestStepPreambleL()

 {
	 __MM_HEAP_MARK;
	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	TVerdict verdict = EPass;
	 return verdict;
	 }
 

/**
 * Post-amble for transfer buffer test step
 * closes transfer buffer tester server
 * Sets __MM_HEAP_MARKEND for memory leak detection
 */
TVerdict CTestStep_MMF_BASECL_TransferBuffer::DoTestStepPostambleL()

	{
	 __MM_HEAP_MARK;

	TVerdict verdict = EPass;
	__MM_HEAP_MARKEND;

	return verdict;
	}



CTestStep_MMF_BASECL_U_0100::CTestStep_MMF_BASECL_U_0100()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0100-LP");
	 }
 
/** Basic sanity check to verify transfer buffers are working on Target 
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0100::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	 INFO_PRINTF1(_L("Transfer Window not supported on this platform!"));	//let testers know WHY...

	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0101::CTestStep_MMF_BASECL_U_0101()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0101-HP");
	 }
 
/** Verify CMMFTransferBuffer creation and deletion using supplied handles
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0101::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0102::CTestStep_MMF_BASECL_U_0102()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0102-HP");
	 }
 
/** Verify CMMFTransferBuffer creation and deletion specifying the buffer data length
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0102::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0103::CTestStep_MMF_BASECL_U_0103()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0103-HP");
	 }
 
/** Verify CMMFTransferBuffer buffer data is identical the data in the server process
 * with test data added server side and verified client side
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0103::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0104::CTestStep_MMF_BASECL_U_0104()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0104-HP");
	 }
 
/** Verify CMMFTransferBuffer mapping data out the buffer
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0104::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0105::CTestStep_MMF_BASECL_U_0105()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0105-HP");
	 }
 
/** Verify CMMFTransferBuffer Updating transfer window
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0105::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0106::CTestStep_MMF_BASECL_U_0106()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0106-HP");
	 }
 
/** Verify CMMFTransferBuffer length
 * This test works by appending data to the data mapped into this process 
 * from the server - the test then verifeid that the CMMFTransferBuffer 
 * BufferSize() procedure picks up the new length set on the descriptor
 * and that the testdata copied from the server side matches that of the mapped
 * in transfer buffer along with the appended data
 * also write data first then map in buffer?
 * with test data added client side and verified server side
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0106::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


 CTestStep_MMF_BASECL_U_0107::CTestStep_MMF_BASECL_U_0107()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0107-HP");
	 }
 
/** Verify CMMFTransferBuffer status checking
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0107::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0108::CTestStep_MMF_BASECL_U_0108()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0108-HP");
	 }
 
/** Verify CMMFTransferBuffer time to play
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0108::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
	
	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }



CTestStep_MMF_BASECL_U_0109::CTestStep_MMF_BASECL_U_0109()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0109-HP");
	 }
 
/** Verify CMMFTransferBuffer frame number
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0109::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0110::CTestStep_MMF_BASECL_U_0110()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0110-HP");
	 }
 
/** Verify CMMFTransferBuffer positioning
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0110::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0111::CTestStep_MMF_BASECL_U_0111()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0111-LP");
	 }
 
/** Verify CMMFTransferBuffer Set Request Size
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0111::DoTestStepL( void )

	 {
	 TVerdict verdict = EPass;
 
	 //This test has moved to 0120 !!
		
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0112::CTestStep_MMF_BASECL_U_0112()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0112-HP");
	 }
 
/** Verify CMMFTransferBuffer Transfer window
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0112::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0113::CTestStep_MMF_BASECL_U_0113()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0113-HP");
	 }
 
/** Verify CMMFTransferBuffer with null RTransferwindow handle
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0113::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0114::CTestStep_MMF_BASECL_U_0114()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0114-HP");
	 }
 
/** Verify CMMFTransferBuffer but set the buffer length to be bigger than mapped in transfer buffer length
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0114::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0115::CTestStep_MMF_BASECL_U_0115()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0115-HP");
	 }
 
/** Verify CMMFTransferBuffer creation where no transfer buffer has been mapped in to the 
 * supplied RTransferWindow
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0115::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0116::CTestStep_MMF_BASECL_U_0116()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0116-HP");
	 }
 
/** Verify CMMFTransferBuffer updating the transfer window
 * but with a different transfer window to the prvious window
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0116::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0117::CTestStep_MMF_BASECL_U_0117()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0117-HP");
	 }
 
/** Verify CMMFTransferBuffer updating the transfer window with a null transfer window
 * but with a different transfer window to the prvious window
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0117::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;
 
	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0118::CTestStep_MMF_BASECL_U_0118()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0118-HP");
	 }
 
/** Verify CMMFTransferBuffer updating the transfer window 
 * with the buffer length bigger than mapped in transfer buffer length
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0118::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0119::CTestStep_MMF_BASECL_U_0119()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0119-HP");
	 }
 
/** Verify CMMFTransferBuffer create with valid RTransferWindow - then 
 * try to map in a new transfer window which does not have a valid buffer mapped in
 * with the buffer length bigger than mapped in transfer buffer length
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0119::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0120::CTestStep_MMF_BASECL_U_0120()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0120-HP");
	 }
 
/** Verify CMMFTransferBuffer RequestSize/SetRequestSize
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0120::DoTestStepL( void )
	 {
	 TVerdict verdict = EPass;

	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	 return verdict;
	 }


CTestStep_MMF_BASECL_U_0121::CTestStep_MMF_BASECL_U_0121()
/** Constructor
 */
	 {
	 iTestStepName = _L("MM-MMF-BASECL-U-0121-HP");
	 }
 
/** Verify CMMFTransferBuffer OOM
 * Use case: N/A
 * @test Req. under test REQ172.7.25
 */
TVerdict CTestStep_MMF_BASECL_U_0121::DoTestStepL( void )
	{
	TVerdict verdict = EPass;
 
	INFO_PRINTF1(_L("No Transfer Buffers on EKA2 - Skipping test!"));
	return verdict;
	}

	 
