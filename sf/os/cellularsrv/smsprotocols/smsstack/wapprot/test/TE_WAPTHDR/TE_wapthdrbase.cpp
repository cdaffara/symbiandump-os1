// Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// @file
// 
//

#include "TE_wapthdrbase.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TE_wapthdrbaseTraces.h"
#endif

// WapProt code needs it
GLDEF_C void Panic(TWAPSMSPanic aPanic)
{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TE_WAPTHDRBASE_PANIC_1, "WAPUPANIC %d", aPanic);
	_LIT(KWapProtPanic, "WAPPROT");
	User::Panic(KWapProtPanic, aPanic);
}

// implementation of helper functions

void CWapThdrTestStep::_Print(TPtrC8 aStr)
/**
 *  print an 8bit descriptor to the console
 */
{
    TBuf<256> PrintBuffer;
    PrintBuffer.Copy(aStr);
    INFO_PRINTF1(PrintBuffer);
}


void CWapThdrTestStep::_PrintSegments(CArrayPtrFlat<HBufC8>& aSegmentArray)
/**
 *  print an array of segments to the console
 */
{
    TBuf<16>  NumStr;
    TInt Count = aSegmentArray.Count();
    for (TInt i=0; i<Count; i++)
    {
        NumStr.Num(i);
        INFO_PRINTF1(NumStr);
        _Print(_L8(": "));
        NumStr.Num((*aSegmentArray[i]).Length());
        INFO_PRINTF1(NumStr);
        _Print(_L8(": "));
        _Print(*aSegmentArray[i]);
        _Print(_L8("\n"));
    }
}


void CWapThdrTestStep::_PrintHeaderL(const TWapTextMessage& aHdr)
/**
 *  print a WAP text message header to the console
 */
{
    TInt      Value=0;
    TBuf<16>  NumStr;
    TBuf8<160> OtherHeader;
    TBuf8<160> Data;
    TBool      IsLong = EFalse;

    _Print(_L8("\n"));
    OtherHeader.Zero();
    Data.Zero();

    _Print(_L8("DestPort: SrcPort: RefNumber: TotalSegments: SegmentNumber\n"));

    Value=aHdr.DestinationPort(&IsLong);
    NumStr.Num(Value);
    INFO_PRINTF1(NumStr);
    _Print(_L8(": "));

    Value=aHdr.SourcePort(&IsLong);
    NumStr.Num(Value);
    INFO_PRINTF1(NumStr);
    _Print(_L8(": "));

    Value=aHdr.ReferenceNumber();
    NumStr.Num(Value);
    INFO_PRINTF1(NumStr);
    _Print(_L8(": "));

    Value=aHdr.TotalSegments();
    NumStr.Num(Value);
    INFO_PRINTF1(NumStr);
    _Print(_L8(": "));

    Value=aHdr.SegmentNumber();
    NumStr.Num(Value);
    INFO_PRINTF1(NumStr);
    _Print(_L8("\n"));

    aHdr.OtherHeader(OtherHeader);
    _Print(_L8("OtherHeader: "));
    _Print(OtherHeader);
    _Print(_L8("\n"));

    aHdr.UserData(Data);
    _Print(_L8("Data: "));
    _Print(Data);
    _Print(_L8("\n"));

// assert destination port
    if (IsLong)
        TESTL((aHdr.DestinationPort()>=0 && aHdr.DestinationPort()<=65535));
    else
        TESTL((aHdr.DestinationPort()>=0 && aHdr.DestinationPort()<=255));
    // assert source port
    if (IsLong)
        TESTL((aHdr.SourcePort()>=0 && aHdr.SourcePort()<=65535));
    else
        TESTL((aHdr.SourcePort()>=0 && aHdr.SourcePort()<=255));

    // assert reference number
    if (IsLong)
        TESTL((aHdr.ReferenceNumber()>=0 && aHdr.ReferenceNumber()<=65535));
    else
        TESTL((aHdr.ReferenceNumber()>=0 && aHdr.ReferenceNumber()<=255));

    TESTL((aHdr.TotalSegments()>=1 && aHdr.TotalSegments()<=255));
    TESTL((aHdr.SegmentNumber()>=1 && aHdr.SegmentNumber()<=255));
}


TVerdict CWapThdrTestStep::doTestStepPreambleL()
{
	__UHEAP_MARK;

	TInt ret = iFs.Connect();
    TESTL(ret == KErrNone);

	return TestStepResult() ;
}

TVerdict CWapThdrTestStep::doTestStepPostambleL()
{
	iFs.Close();

	__UHEAP_MARKEND;

	return TestStepResult() ;
}
