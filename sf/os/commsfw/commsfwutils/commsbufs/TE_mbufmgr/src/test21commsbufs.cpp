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
// Contains MBufMgr Test Step 21 for commsbufs method
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
//#include "networking/log.h"
//#include "networking/teststep.h"

//#ifdef SYMBIAN_OLD_EXPORT_LOCATION
//#include "networking/log.h"
//#include "networking/teststep.h"
//#else
//#include <networking/log.h>
//#include <networking/teststep.h>
//#endif

#include "TestStepCTMbufmgr.h"

#include "Test21commsbufs.h"
#include <comms-infras/commsbufpond.h>
// constructor
CTest21CommsBufs::CTest21CommsBufs()
	{
	SetTestStepName( _L("MBufMgrTest21") );	// Store the name of this test case
	}

// destructor
CTest21CommsBufs::~CTest21CommsBufs()
	{
	}

//
enum TVerdict CTest21CommsBufs::doTestStepL(void)
	{
	SetTestStepResult(EFail);
			
	//-------------- substep 1 --------------------
	INFO_PRINTF1(_L("  01 Creating CMBufManager and installing active scheduler:"));
    CleanupStack::PushL( iActSch = new(ELeave) CActiveScheduler );
	CActiveScheduler::Install(iActSch);
	CreateInstanceMBufMgrL(KMBufDefaultHeapSize);
	CleanupClosePushL(iBufPond);
	TCommsBufAllocator allocator = iBufPond.Allocator();
    INFO_PRINTF1(_L("  01 Allocate a RCommsBuf of size 128"));	
	RCommsBuf* buf = RCommsBuf::Alloc(128, allocator);
    INFO_PRINTF1(_L("  01 Check the initial offset and length should be 0 & 128"));
	// Check the initial offset and length. MUST be 0 & 128 as we 
	// allocated a buffer of size 128
	if(buf->Offset() != 0 || buf->Length() != buf->RawSize())
	    {
	    INFO_PRINTF1(_L("  Error: The offset and length checking failed"));
	    return EFail;
	    }
	
	INFO_PRINTF1(_L("  02 Write abcdef"));
	_LIT8(KWriteData, "abcdef");
	buf->Write(KWriteData, 20); // Write abcdef at offset 20
	// Check the offset should be 20
	// Check the length should 6 ie; abcdef
	INFO_PRINTF1(_L("  02 Check the write offset and length should be 20 & 6"));	
    if(buf->Offset() != 20 || buf->Length() != KWriteData().Length())
        {
        INFO_PRINTF1(_L("  Error 02: The offset and length checking failed"));
		return TestStepResult();
        }
	
    TBuf8<10> buffer;
    INFO_PRINTF1(_L("  02 Read the buffer at offset 0"));
    buf->Read(buffer);
    INFO_PRINTF1(_L("  02 check the content of the read buffer"));
    if(buffer.Compare(KWriteData()) != 0)
        {
        INFO_PRINTF1(_L("  Error 02 The read buffer content is not same as the write buffer"));
        return TestStepResult();
        }

    INFO_PRINTF1(_L("  02 Read the buffer at offset 3"));
    buf->Read(buffer, 3);
    INFO_PRINTF1(_L("  02 check the content of the read buffer"));
    TPtrC8 ptr2 = KWriteData().Right(3); // Should be def
    if(buffer.Compare(ptr2) != 0)
        {
        INFO_PRINTF1(_L("  Error 02 The read buffer content is not same as the write buffer"));
        return TestStepResult();       
        }

    INFO_PRINTF1(_L("  03 Allocate a RCommsBuf of size 128"));   
    RCommsBuf* buf2 = RCommsBuf::Alloc(128, allocator);
    INFO_PRINTF1(_L("  03 Write abcdef"));
    buf2->Write(KWriteData, 10); // Write abcdef at offset 20
    // Append ghijklm
    INFO_PRINTF1(_L("  03 Append ghijklm"));
    _LIT8(KAppendData, "ghijklm");
    buf2->Append(KAppendData);
    
    // Compare the data
    INFO_PRINTF1(_L("  03 Compare the contained data"));
    _LIT8(KCompareData, "abcdefghijklm");
    TBuf8<30> buffer2;
    buf2->Read(buffer2);
    if(buffer2.Compare(KCompareData()) != 0)
        {
        INFO_PRINTF1(_L("  Error 03 The read buffer content is not same as the write buffer"));
        return TestStepResult();       
        }
    
    // Prepend 123
    _LIT8(KPrependData, "12345");
    INFO_PRINTF1(_L("  03 Prepend 12345"));
    buf2->Prepend(KPrependData);
    // Now offset would have been 5
    if(buf2->Offset() != 5)
        {
        INFO_PRINTF1(_L("  Error 03 after prepend the offset is not correct"));
        return TestStepResult(); 
        }
    
    // Read the whole data
    buf2->Read(buffer2);
    
    _LIT8(KCompareData2, "12345abcdefghijklm");
    if(buffer2.Compare(KCompareData2()) != 0)
        {
        INFO_PRINTF1(_L("  Error 03 After prepend the read buffer is not correct."));
        return EFail;        
        }
     
    for(TInt i = 0; i < buffer2.Length(); ++i)
         {
         if(buffer2[i] != (*buf2)[i])
             {
             INFO_PRINTF1(_L("  Error 03 Comparison of individual characters failed."));
             return TestStepResult();
             }
         }

    buf->Free();
	buf2->Free();
	CleanupStack::PopAndDestroy(); // pond
	CActiveScheduler::Install(NULL);
	CleanupStack::PopAndDestroy(iActSch);
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}
