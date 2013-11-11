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

#include "TE_wapdgrmbase.h"
#include "smsstackutils.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "TE_wapdgrmbaseTraces.h"
#endif

// WapProt code needs it
GLDEF_C void Panic(TWAPSMSPanic aPanic)
{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, TE_WAPDGRMBASE_PANIC_1, "WAPUPANIC %d", aPanic);
	_LIT(KWapProtPanic, "WAPPROT");
	User::Panic(KWapProtPanic, aPanic);
}

// implementation of helper functions

void CWapDgrmTestStep::_Print(TInt aNum)
/**
 *  print a number to the console
 */
{
	TBuf<16>  NumStr;
	// Ordinal
	NumStr.Num(aNum);
	INFO_PRINTF1(NumStr);
}

void CWapDgrmTestStep::_Print(const TPtrC8 aStr)
/**
 *  print an 8-bit descriptor to the console
 */
{
	TBuf<1024> PrintBuffer;
	PrintBuffer.Copy(aStr);
	INFO_PRINTF1(PrintBuffer);
}

void CWapDgrmTestStep::_Print(const TDesC& aStr)
/**
 *  print a descriptor to the console
 */
{
	INFO_PRINTF1(aStr);
}

void CWapDgrmTestStep::_PrintBool(TBool aTruth)
/**
 *  print a boolean number to the console
 */
{
	_Print((aTruth ? _L8("true") : _L8("false")));
}

void CWapDgrmTestStep::_Print(CWapDatagram& aWap)
/**
 *  print a WAP datagram to the console
 */
{
	TBool                IsComplete = EFalse;
	const TPtrC&         ToAddress = aWap.ToAddress();
	const TPtrC&         FromAddress = aWap.FromAddress();
	TInt                 ToPort = 0;
	TInt                 FromPort = 0;
	TInt                 Is16BitPorts = 0;
	TBool                IsPorts = ETrue;
	TSmsDataCodingScheme::TSmsAlphabet Alphabet;

	IsComplete = aWap.IsComplete();
	Alphabet = aWap.Alphabet();

	_Print(_L8("CWapDatagram:: IsComplete: FromAddress: ToAddress: Alphabet\n"));
	_PrintBool(IsComplete);
	_Print(_L8(": "));
	_Print(FromAddress);
	_Print(_L8(": "));
	_Print(ToAddress);
	_Print(_L8(": "));
	_Print(Alphabet);
	_Print(_L8("\n"));

	aWap.Ports(FromPort,ToPort,&Is16BitPorts);
	_Print(_L8("IsPorts: FromPort: ToPort: Is16BitPorts\n"));
	_PrintBool(IsPorts);
	_Print(_L8(": "));
	_Print(FromPort);
	_Print(_L8(": "));
	_Print(ToPort);
	_Print(_L8(": "));
	_PrintBool(Is16BitPorts);
	_Print(_L8("\n"));

   	if (aWap.IsComplete())
   		{
   		// allocate buffer for data
   		HBufC8* DataBuffer = HBufC8::New(aWap.WapDatagramLength());
   		TPtr8 DataBufferPtr(DataBuffer->Des());
 		RMBufChain dgramChain;
 		aWap.WapDatagram(dgramChain, aWap.WapDatagramLength());
 		dgramChain.CopyOut(DataBufferPtr);
   		// Length
   		_Print(DataBuffer->Length());
   		_Print(_L8(": <"));

   		// Data
   		_Print(*DataBuffer);
   		_Print(_L8(">\n"));

   		delete DataBuffer;
   		}
}

void CWapDgrmTestStep::_Print(CSmsMessage& aSms)
/**
 *  print a SMS message to the console
 */
{
	TSmsUserDataSettings UserDataSettings;
	TBool                IsComplete = EFalse;
	const TPtrC&         Address = aSms.ToFromAddress();
	TInt                 ToPort = 0;
	TInt                 FromPort = 0;
	TInt                 Is16BitPorts = 0;
	TBool                IsPorts = EFalse;
	TSmsDataCodingScheme::TSmsAlphabet Alphabet;

	// Print CSmsMessage settings
	CSmsPDU& Pdu = aSms.SmsPDU();

	IsComplete = aSms.IsComplete();
	aSms.UserDataSettings(UserDataSettings);
	Alphabet = UserDataSettings.Alphabet();

	_Print(_L8("CSmsMessage:: IsComplete: ToFromAddress: Alphabet\n"));
	_PrintBool(IsComplete);
	_Print(_L8(": "));
	_Print(Address);
	_Print(_L8(": "));
	_Print(Alphabet);
	_Print(_L8("\n"));

	IsPorts = Pdu.ApplicationPortAddressing(ToPort,FromPort,&Is16BitPorts);
	_Print(_L8("IsPorts: FromPort: ToPort: Is16BitPorts\n"));
	_PrintBool(IsPorts);
	_Print(_L8(": "));
	_Print(FromPort);
	_Print(_L8(": "));
	_Print(ToPort);
	_Print(_L8(": "));
	_PrintBool(Is16BitPorts);
	_Print(_L8("\n"));
}
void CWapDgrmTestStep::_PrintL(CSmsBufferBase& aSmsBuffer)
/**
 *  print a SMS buffer to the console
 */
{
	HBufC16* WideBuffer = NULL;
	TInt Length = 0;

	// copy the data to the descriptor from aSmsBuffer
	Length = aSmsBuffer.Length();
	WideBuffer = HBufC16::NewL(Length);
	TPtr16 WideChars = WideBuffer->Des();
	aSmsBuffer.Extract(WideChars,0,Length);

	// Length
	_Print(WideChars.Length());
	_Print(_L8(": <"));

	// Data
	_Print(WideChars);
	_Print(_L8(">\n"));

	delete WideBuffer;
}

void CWapDgrmTestStep::_PrintL(CArrayPtr<CSmsMessage>& aSmsArray)
/**
 *  print an array of SMS messages to the console
 */
{
	TInt Count = aSmsArray.Count();
	TInt j = 1;

	for (TInt i=0;i<Count;i++,j++)
	{
		// Ordinal
		_Print(i);
		_Print(_L8(": \n"));

		CSmsMessage* Sms = aSmsArray.At(i);
		_Print(*Sms);

		// Print data
		CSmsBufferBase& SmsBuffer = Sms->Buffer();
		_PrintL(SmsBuffer);
	}
}

void CWapDgrmTestStep::_FlushReserveArrayL(CWapReassemblyStore* aWapStore)
/**
 *  Method should be called only once during a test program run !
 */
{
	TInt Count = iReserveArray->Count();

	// In reverse oder
	for (TInt i=Count-1;i>=0;i--)
	{
		CWapDatagram* Wap=iReserveArray->At(i);
		if (!Wap->IsComplete())
		{
			TInt Index = 0;
			TBool IsComplete = EFalse;
			IsComplete = aWapStore->AddMessageL(Index,*Wap);
			if (IsComplete)
			{
				aWapStore->GetDatagramL(Index,*Wap);
				aWapStore->BeginTransactionLC();
				aWapStore->DeleteEntryL(Index);
				aWapStore->CommitTransactionL();
				_Print(*Wap);
			}
		}
		else
			_Print(*Wap);
	}

	iReserveArray->ResetAndDestroy();
	delete iReserveArray;
	iReserveArray = NULL;
}

void CWapDgrmTestStep::_Parse2L(CArrayPtrFlat<CSmsMessage>& aSmsArray,
						CWapReassemblyStore* aWapStore)
/**
 *  This method tests mainly CWapDatagram::DecodeConcatenatedMessagesL
 */
{
    TInt Count = aSmsArray.Count();

    // In reverse order
    // Note ! You can test additional features by changing value of
    // i or a global variable (insertSms). For example:
    // incrementing i by 1: a duplicate test
    // decrementing i by 1: not all datagrams are pushed to the store
    // ncreasing insertSms causes datagrams to be appended to the reserve array
    // Remember to set flush the reserve array on a later phase
    // by setting iIsFlushReserveArray true


    for (TInt i=Count-1;i>=iInsertSms;i--)
    {
        CWapDatagram* Wap=CWapDatagram::NewL(*(aSmsArray.At(i)));
        if (!Wap->IsComplete())
        {
            TInt Index = 0;
            TBool IsComplete = EFalse;
            IsComplete = aWapStore->AddMessageL(Index,*Wap);
            if (IsComplete)
            {
                aWapStore->GetDatagramL(Index,*Wap);
				aWapStore->BeginTransactionLC();
                aWapStore->DeleteEntryL(Index);
				aWapStore->CommitTransactionL();
                _Print(*Wap);
            }
        }
        else
            _Print(*Wap);

        delete Wap;
    }

	// append rest of the datagrams to the ReserveArray
	if (iInsertSms>0 && Count>1)
	{
		// reserve order here too
        for (TInt i=iInsertSms-1;i>=0;i--)
        {
            CWapDatagram* Wap=CWapDatagram::NewL(*(aSmsArray.At(i)));
            iReserveArray->AppendL(Wap);
        }
	}

    if (iIsFlushReserveArray)
    {
        _FlushReserveArrayL(aWapStore);
        iInsertSms = 0;
        iIsFlushReserveArray = EFalse;
    }
}

void CWapDgrmTestStep::ConstructWap(CWapDatagram& aWap,
						TDesC& aFromAddress, TDesC& aToAddress,
						TInt aFromPort, TInt aToPort,
						TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
						TBool aIs16BitPorts = EFalse)
/**
 *  create a WAP datagram, fill in data like to/from adress, ports, etc...
 */
{
	static TInt Ref = 0;
	Ref++;
	aWap.SetFromAddress(aFromAddress);
	aWap.SetToAddress(aToAddress);
	aWap.SetPorts(aFromPort,aToPort,aIs16BitPorts);
	aWap.SetAlphabet(aAlphabet);
	aWap.SetConcatenatedMessageReference(Ref);
}

void CWapDgrmTestStep::DoTestWapProtDatagramL(const TDesC8& aBuffer,
                        TDesC& aToAddress, TInt aToPort, TDesC& aFromAddress,
                        TInt aFromPort, TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
                        TBool aIs16BitPorts)
/**
 *  @test a short 7/8-bit datagram with explicit 16-bit port number
 */
    {
        INFO_PRINTF1(_L("a 7/8-bit datagram with explicit 16-bit port number"));

        CArrayPtrFlat<CSmsMessage>* SmsArray = 0;
        CWapDatagram* Wap = NULL;
        RFs FileServerSession;
        CWapReassemblyStore* wapStore = NULL;

        if(iIsRunParsing)
        {
            TInt ret = FileServerSession.Connect();
            TESTL(ret == KErrNone);
            wapStore = CWapReassemblyStore::NewL(FileServerSession);
            CleanupStack::PushL(wapStore);
        }

        _Print((aBuffer).Length());_Print(_L8(": <"));_Print(aBuffer);_Print(_L8(">\n"));
        Wap=CWapDatagram::NewL(aBuffer);
        CleanupStack::PushL(Wap);        
        
        SmsArray = new (ELeave) CArrayPtrFlat<CSmsMessage>(8);
        CleanupStack::PushL(SmsArray);
        CleanupResetAndDestroyPushL(*SmsArray);        
        
        ConstructWap(*Wap, aFromAddress, aToAddress, aFromPort, aToPort, aAlphabet, aIs16BitPorts);

        Wap->EncodeConcatenatedMessagesL(iFs, *SmsArray);
        if (iIsPrintEncoded)
            _PrintL(*SmsArray);

        if (iIsRunParsing)
            _Parse2L(*SmsArray, wapStore);
        
        CleanupStack::PopAndDestroy(3, Wap);  // SmsArray elements (Reset and Destroy), SmsArray, Wap       

        if (iIsRunParsing)
        {
            CleanupStack::PopAndDestroy(wapStore);
            FileServerSession.Close();
        }
    }

TVerdict CWapDgrmTestStep::doTestStepPreambleL()
	{
	const TInt KMBuf_MaxAvail = 393216;
	const TInt KMBuf_MBufSize = 128;
	const TInt KMBuf_MBufSizeBig = 1600;
	const TInt KMBuf_InitialAllocation = 128;
	const TInt KMBuf_MinGrowth = 64;
	const TInt KMBuf_GrowthThreshold = 40;

	__UHEAP_MARK;

#ifdef __CFLOG_ACTIVE
	__CFLOG_CREATEL;
	__CFLOG_OPEN;
#endif

	iIsFlushReserveArray = EFalse;
	iReserveArray = NULL;
	iInsertSms = 0;
	iIsRunParsing = ETrue;
	iIsPrintEncoded = ETrue;

    // Create and install the active scheduler
    iScheduler=new (ELeave) CActiveScheduler;
    CActiveScheduler::Install(iScheduler);

	// The WAP protocol unpacking code uses MBufs, so we need a local MBufManager
#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
    // Initialize the commsbuf pond
    RArray<TCommsBufPoolCreateInfo> createInfos;
    TCommsBufPoolCreateInfo createInfo;
    createInfo.iBufSize = KMBuf_MBufSize;
    createInfo.iInitialBufs = KMBuf_InitialAllocation;
    createInfo.iGrowByBufs = KMBuf_MinGrowth;
    createInfo.iMinFreeBufs = KMBuf_GrowthThreshold;
    createInfo.iCeiling = (KMBuf_MaxAvail/2) / KMBuf_MBufSize;
    createInfos.AppendL(createInfo);
    TCommsBufPoolCreateInfo createInfo2;
    createInfo2.iBufSize = KMBuf_MBufSizeBig;
    createInfo2.iInitialBufs = KMBuf_InitialAllocation;
    createInfo2.iGrowByBufs = KMBuf_MinGrowth;
    createInfo2.iMinFreeBufs = KMBuf_GrowthThreshold;
    createInfo2.iCeiling = (KMBuf_MaxAvail/2) / KMBuf_MBufSize;    
    createInfos.AppendL(createInfo2);
	User::LeaveIfError(iCommsBufPond.Open(createInfos));
	createInfos.Close();
#else
	MMBufSizeAllocator*  mBufSizeAllocator;

	iBufMgr = CMBufManager::NewL(KMBuf_MaxAvail, mBufSizeAllocator);
	if (iBufMgr != NULL  &&  mBufSizeAllocator != NULL)
		{
		mBufSizeAllocator->AddL(KMBuf_MBufSize,    KMBuf_InitialAllocation, KMBuf_MinGrowth, KMBuf_GrowthThreshold);
		mBufSizeAllocator->AddL(KMBuf_MBufSizeBig, KMBuf_InitialAllocation, KMBuf_MinGrowth, KMBuf_GrowthThreshold);
		}
	iBufMgr->SetContext();
#endif

	iWapDgrmTable[0] = &KWapDgrm1;
	iWapDgrmTable[1] = &KWapDgrm2;
	iWapDgrmTable[2] = &KWapDgrm3;
	iWapDgrmTable[3] = &KWapDgrm4;
	iWapDgrmTable[4] = &KWapDgrm5;
	iWapDgrmTable[5] = &KWapDgrm6;
	iWapDgrmTable[6] = NULL;

	TInt ret = iFs.Connect();
    TESTL(ret == KErrNone);

	return TestStepResult() ;
	}

TVerdict CWapDgrmTestStep::doTestStepPostambleL()
	{
    iFs.Close();

#ifdef SYMBIAN_ZERO_COPY_NETWORKING	
	iCommsBufPond.Close();
#else
   	delete iBufMgr;
   	iBufMgr = NULL;
#endif 	// SYMBIAN_ZERO_COPY_NETWORKING	

	delete iScheduler;
	iScheduler = NULL;

#ifdef __CFLOG_ACTIVE
	__CFLOG_CLOSE;
	__CFLOG_DELETE;
#endif

	__UHEAP_MARKEND;

	return TestStepResult() ;
	}
