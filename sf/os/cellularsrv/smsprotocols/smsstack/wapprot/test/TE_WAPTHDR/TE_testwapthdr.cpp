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
// @file
// 
//

#include "TE_testwapthdr.h"

#include "TE_wapthdrbase.h"
#include "smsstackutils.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TE_testwapthdrTraces.h"
#endif

const TInt KSmsLogBufferSize = 1280;

TVerdict CTestParseWapTextMessage::doTestStepL()
/*
 * implementation of Parse Wap Text Message test case
 */
{
    INFO_PRINTF1(_L("TE_WAPTHDR Parse Wap Text Message TEST - Starting ..."));
    INFO_PRINTF1(_L(""));

    const TUint KStdDelimiter=',';
    TWapTextMessage* wapHdr = 0;
    TPtrC8           wapBuf8;
    TBool            isWapHdr = EFalse;
    TInt             count = 0;

    CTestConfig* configFile;
    const CTestConfigSection* cfgFile;
    configFile=CTestConfig::NewLC(iFs,KConfigFileDir,KConfigFilename);

    // end of construction

    TESTL(((cfgFile = configFile->Section(KParsingWapHeader))!=NULL));
    TESTL(count=cfgFile->ItemCount(KWapHeader));

    const CTestConfigItem* item=NULL;
    TInt i, ret=KErrNone;
    for(i=0;i<count;i++)
    {
        item=cfgFile->Item(KWapHeader,i);
        if(!item)
            break;
        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,wapBuf8, EFalse);
        if(ret!=KErrNone)
            continue;
        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,isWapHdr);
        if(ret!=KErrNone)
            continue;
        //
        // wapprot function call has to be here
        //
        TBuf<256>       buf;
        buf.Copy( wapBuf8 );
        INFO_PRINTF2(_L("[test%02d]"), i+1 );
        INFO_PRINTF2(_L("%S"), &buf );

        wapHdr = new (ELeave) TWapTextMessage(wapBuf8);
        CleanupStack::PushL(wapHdr);
        TESTL((isWapHdr == wapHdr->Parse()));
        if( isWapHdr )
        {
            CArrayPtrFlat<HBufC8>* segmentArray = new (ELeave) CArrayPtrFlat<HBufC8> (8);
            CleanupStack::PushL(segmentArray);
            CleanupResetAndDestroyPushL(*segmentArray);            
            
            _PrintHeaderL(*wapHdr);
            wapHdr->EncodeSegmentsL(*segmentArray);
            _PrintSegments(*segmentArray);
            
            CleanupStack::PopAndDestroy(2, segmentArray);  // segmentArray elements (Reset and Destroy), segmentArray            
        }
        CleanupStack::PopAndDestroy(wapHdr);
    }
    // end of the parsing

    // beginning of the destruction
    CleanupStack::PopAndDestroy(configFile);

    return TestStepResult();
}

TVerdict CTestEncodeWapTextMessage::doTestStepL()
/*
 * implementation of Encode Wap Text Message test case
 */
{
    INFO_PRINTF1(_L("TE_WAPTHDR Encode Wap Text Message TEST - Starting ..."));
    INFO_PRINTF1(_L(""));

    const TUint KStdDelimiter=',';
    TPtrC8           result8;
    TPtrC8           wapBuf8;
    TPtrC8           wapOther8;
    TInt             destport, is16destport, srcport, is16srcport, refnum;
    TInt             count = 0;

    CTestConfig* configFile;
    const CTestConfigSection* cfgFile;
    configFile=CTestConfig::NewLC(iFs,KConfigFileDir,KConfigFilename);

    // end of construction

    TESTL(((cfgFile = configFile->Section(KEncodingWapHeader))!=NULL));
    TESTL(count=cfgFile->ItemCount(KResult));

    const CTestConfigItem* item=NULL;
    TInt i, ret=KErrNone;
    for(i=0;i<count;i++)
    {
        item=cfgFile->Item(KResult,i);
        if(!item)
            break;

        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,result8, EFalse);
        if(ret!=KErrNone)
            continue;

        item=cfgFile->Item(KPort,i);
        if(!item)
            break;
        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,destport);
        if(ret!=KErrNone)
            continue;
        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,1,is16destport);
        if(ret!=KErrNone)
            continue;
        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,2,srcport);
        if(ret!=KErrNone)
            continue;
        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,3,is16srcport);
        if(ret!=KErrNone)
            continue;

        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,wapBuf8, EFalse);
        if(ret!=KErrNone)
            continue;

        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,wapOther8, EFalse);
        if(ret!=KErrNone)
            continue;

        ret=CTestConfig::GetElement(item->Value(),KStdDelimiter,0,refnum);
        if(ret!=KErrNone)
            continue;

        //
        // wapprot function call has to be here
        //
        TBuf<256>       buf;
        buf.Copy( wapBuf8 );
        INFO_PRINTF2(_L("[test%02d]"), i+1 );
        INFO_PRINTF2(_L("%S"), &buf );

        CArrayPtrFlat<HBufC8>* segmentArray = new (ELeave) CArrayPtrFlat<HBufC8> (8);
        CleanupStack::PushL(segmentArray);
        CleanupResetAndDestroyPushL(*segmentArray);                    
        
        TWapTextMessage* wapHdr = new (ELeave) TWapTextMessage(KNullDesC8);
        CleanupStack::PushL(wapHdr);

        wapHdr->SetDestinationPort(destport, is16destport);
        wapHdr->SetSourcePort(srcport, is16srcport);
        wapHdr->SetReferenceNumber(refnum);
        wapHdr->SetUserData(wapBuf8);
        wapHdr->SetOtherHeader(wapOther8);
        wapHdr->EncodeSegmentsL(*segmentArray);
        _PrintSegments(*segmentArray);
  
        CleanupStack::PopAndDestroy(3, segmentArray);  // wapHdr, segmentArray elements (Reset and Destroy), segmentArray                    
    }
    // end of the parsing

    // beginning of the destruction
    CleanupStack::PopAndDestroy(configFile);//configFile

    return TestStepResult();
}

TVerdict CTestLoggerOverflow::doTestStepL()
/*
 * implementation of Logger Overflow test case
 */
	{
	INFO_PRINTF1(_L("TE_WAPTHDR Logger Overflow TEST - Starting ..."));
	INFO_PRINTF1(_L(""));

	TBuf8<KSmsLogBufferSize*2> buf;
	buf.SetMax();
	buf.Fill('A');

	// Send a buffer too long for the logger to handle, therefore causing an overflow.
	// Function will panic if overflow handler is not implemented correctly.
	INFO_PRINTF1(_L("Testing LOGSMSIF2..."));
	OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CTESTLOGGEROVERFLOW_DOTESTSTEPL_1, "%s",buf);

	return TestStepResult();
	}

