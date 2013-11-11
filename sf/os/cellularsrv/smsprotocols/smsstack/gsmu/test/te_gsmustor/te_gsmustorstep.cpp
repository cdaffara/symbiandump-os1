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
// Example CTestStep derived implementation
// 
//

/**
 @file
*/

#include <s32std.h>
#include "gsmumsg.h"
#include "gsmubuf.h"
#include "gsmuset.h"
#include "te_gsmustorStep.h"
#include <test/testexecutelog.h>


//    	The INFO_PRINTF3 jhave been left commented after the ranbdom call
//      This is because the generatged strings break the log files
//		ie code like this
//		TInt tmp = (TInt)Math::Random();
//		//INFO_PRINTF3(_L("%d %d"), i, tmp);
//


//
// Conversion strings for testing
//

_LIT(KTestGsmuStoreName,"C:\\System\\Data\\testgsmustor.dat");
const TUid KTestGsmuStoreUid={0x1010089e};


const TUint KInterval = 50;

void
CTgsmustorStepBase::ExpectFileToBeL(TBool aOpen, TBool aInTransaction=EFalse, CSARStore* aStore=NULL)
{
	TBool answer;
	TInt ret = iFs.IsFileOpen(KTestGsmuStoreName, answer);
	TEST(ret == KErrNone);
	TEST((answer && aOpen) || (!answer && !aOpen));

	if (aOpen && aInTransaction && aStore != NULL)
	{
		TRAP(ret, BeginTransactionL(*aStore));
		TEST(ret == KErrAccessDenied);
	}
}

void CTgsmustorStepBase::BeginTransactionL(CSARStore& aStore)
{
	aStore.BeginTransactionLC();
	CleanupStack::PopAndDestroy();
}



TVerdict CTgsmustorStepBase::doTestStepPreambleL()
{
	__UHEAP_MARK;

	TInt ret=iFs.Connect();
	TEST(ret == KErrNone);

	return TestStepResult();
}

TVerdict CTgsmustorStepBase::doTestStepPostambleL()
{
	iFs.Close();

	__UHEAP_MARKEND;

	return TestStepResult();
}


// not used
#if 0

void CTgsmustorStepBase::DumpSAREntriesL(const TDesC& aName, TUid aUid)
	{
	CTestSARStore* sar = CTestSARStore::NewL(iFs);
	sar->OpenL(aName, aUid);
	const CArrayFix<TSAREntry>& sarArray = sar->Entries();
	INFO_PRINTF2(_L("%S:"),aName );
	INFO_PRINTF2(_L("  TSAREntry array: %d entries"), sarArray.Count());
	sar->Close();
	}

void CTgsmustorStepBase::TestDumpSARStoreEntriesL(void)
/**
 *  @test dump the SAR entries store by the SmsStack
 */
	{
	// also defined in wappstor.cpp
	_LIT(KStoreName,"C:\\Private\\101F7989\\sms\\wapreast.dat");
	const TUid KWapReassemblyStoreUid={0x100008CB};

	DumpSAREntriesL(KReassemblyStoreName, KReassemblyStoreUid);
	DumpSAREntriesL(KSegmentationStoreName, KSegmentationStoreUid);
	DumpSAREntriesL(KStoreName, KWapReassemblyStoreUid);
	}



LOCAL_C void BeginTransactionL(CSARStore& aStore)
	{
	aStore.BeginTransactionLC();
	CleanupStack::PopAndDestroy();
	}
#endif

//
// implementation of CTestSARStore
//
CTestSARStore* CTestSARStore::NewL(RFs& aFs)
	{
	return new (ELeave) CTestSARStore(aFs);
	}

CTestSARStore::~CTestSARStore()
/**
 *  D'tor
 */
	{
	}

CTestSARStore::CTestSARStore(RFs& aFs)
/**
 *  C'tor
 */
	:CSARStore(aFs)
	{
	}

void CTestSARStore::AddEntryL(const TSAREntry& aEntry)
	{
	CSARStore::AddEntryL(aEntry);
	}

void CTestSARStore::ChangeEntryL(TInt aIndex,const TSAREntry& aNewEntry)
	{
	CSARStore::ChangeEntryL(aIndex, aNewEntry);
	}

void CTestSARStore::SetExtraStreamIdL(TStreamId aExtraStreamId)
	{
	CSARStore::SetExtraStreamIdL(aExtraStreamId);
	}


TVerdict CTestGsmuStorFileOpenClose::doTestStepL()
{
	// always delete the file before the test

	iFs.Delete(KTestGsmuStoreName);

	CTestSARStore* sar = CTestSARStore::NewL(iFs);
	sar->OpenL(KTestGsmuStoreName, KTestGsmuStoreUid);
	ExpectFileToBeL(ETrue); // file SHOULD be open

	//
	// Open/Close the file and check status after
	//
	sar->Close();
	ExpectFileToBeL(EFalse); // file SHOULD be closed

	//
	// do it twice just in case
	//
	sar->DoOpenL();
	ExpectFileToBeL(ETrue, EFalse, sar); // file SHOULD be open
	sar->Close();
	ExpectFileToBeL(EFalse); // file SHOULD be closed


	TRAPD(err, BeginTransactionL(*sar));
	TEST(err == KErrAccessDenied);

	sar->DoOpenL();
	ExpectFileToBeL(ETrue, EFalse, sar); // file SHOULD be open
	sar->BeginTransactionLC();
	sar->CommitTransactionL();

	sar->Close();
	ExpectFileToBeL(EFalse); // file SHOULD be closed

	delete sar;

	return EPass;

}


TVerdict CTestGsmuStorFileOpenAfterAccess::doTestStepL()
{
	const TStreamId dummyId = 1;

	TInt ret;

	// always delete the file before the test
	iFs.Delete(KTestGsmuStoreName);

	CTestSARStore* sar = CTestSARStore::NewL(iFs);
	sar->OpenL(KTestGsmuStoreName, KTestGsmuStoreUid);

	//
	// === methods that opens/closed the filestore automatically ===
	//

	// test PurgeL - the file *should* be closed after
	ExpectFileToBeL(ETrue); // file SHOULD be open
	TRAP(ret, sar->PurgeL(1234, ETrue) );
	ExpectFileToBeL(ETrue); // file SHOULD be open

	// test PurgeL - the file *should* be closed after
	ExpectFileToBeL(ETrue); // file SHOULD be open
	sar->BeginTransactionLC();
	ExpectFileToBeL(ETrue, ETrue, sar); // file SHOULD be open
	TRAP(ret, sar->CompactL() );
	ExpectFileToBeL(ETrue); // file SHOULD be open
	sar->CommitTransactionL();
	ExpectFileToBeL(ETrue); // file SHOULD be open

	//
	// === methods that don't opens/closed the filestore ===
	//

	// test AddEntryL - the file *should* be closed after
	TSAREntry entry;
	entry.SetDataStreamId(dummyId);
	ExpectFileToBeL(ETrue); // file SHOULD be open
	sar->BeginTransactionLC();
	ExpectFileToBeL(ETrue, ETrue, sar); // file SHOULD be open
	TRAP(ret, sar->AddEntryL(entry));
	ExpectFileToBeL(ETrue); // file SHOULD be open
	sar->CommitTransactionL();
	ExpectFileToBeL(ETrue); // file SHOULD be open

	// test ChangeEntryL - the file *should* be open after
	TSAREntry entry2;
	entry2.SetDataStreamId(dummyId);
	ExpectFileToBeL(ETrue); // file SHOULD be open
	sar->BeginTransactionLC();
	ExpectFileToBeL(ETrue, ETrue, sar); // file SHOULD be open
	TRAP(ret, sar->ChangeEntryL(0, entry2) );
	ExpectFileToBeL(ETrue); // file SHOULD be open
	sar->CommitTransactionL();
	ExpectFileToBeL(ETrue); // file SHOULD be open

	// test DeleteEntryL - the file *should* be closed after
	// note: Delete must be tested after Add
	sar->BeginTransactionLC();
	ExpectFileToBeL(ETrue, ETrue, sar); // file SHOULD be open
	TRAP(ret, sar->DeleteEntryL(0));
	ExpectFileToBeL(ETrue); // file SHOULD be open
	sar->CommitTransactionL();
	ExpectFileToBeL(ETrue); // file SHOULD be open


	// test SetExtraStreamIdL - the file *should* be open after
	TStreamId sid = 1;
	sar->BeginTransactionLC();
	ExpectFileToBeL(ETrue, ETrue, sar); // file SHOULD be open
	TRAP(ret, sar->SetExtraStreamIdL(sid) );
	(void)sid;
	ExpectFileToBeL(ETrue); // file SHOULD be open
	sar->CommitTransactionL();
	ExpectFileToBeL(ETrue); // file SHOULD be open

	sar->Close();
	ExpectFileToBeL(EFalse); // file SHOULD be closed

	delete sar;

	return EPass;
}


TVerdict CTestGsmuStorTSAREntry::doTestStepL()
{
/**
 *  @test test TSAREntry classes
 *  
 *  this is testing the TSAREntry classes and the classes derived from it.
 *  it is basically testing getters and setters.
 */
	//
	// test generic base class
	//
	TSAREntry entry1;
	TSmsReassemblyEntry entry2;
	TSmsSegmentationEntry entry3;
	TWapReassemblyEntry entry4;

	TestSAREntryL(entry1);
	TestSAREntryL(entry2);
	TestSAREntryL(entry3);
	TestSAREntryL(entry4);

	//
	// Sms Reassembly
	//
	TSmsReassemblyEntry smsReassEntry;
	TestSmsReassemblyEntryL(smsReassEntry);

	//
	// Sms Segmentation
	//
	TSmsSegmentationEntry smsSegEntry;
	TestSmsSegmentationEntryL(smsSegEntry);

	//
	// Wap Reassembly
	//
	TWapReassemblyEntry wapReassEntry;
	TestWapReassemblyEntryL(wapReassEntry);

	return EPass;
}

void CTestGsmuStorTSAREntry::TestSAREntryL(TSAREntry& aEntry)
/**
 *  test getters and setters on the TSAREntry class
 */
{
	TUint i;

	INFO_PRINTF1(_L("TSAREntry::Reference"));
	INFO_PRINTF1(_L("TSAREntry::SetReference"));
	for(i=0;i<KInterval;i++)
	{
		TInt tmp = (TInt)Math::Random();
//		INFO_PRINTF3(_L("%d %d"), i, tmp);
		aEntry.SetReference(tmp);
		TInt ref = aEntry.Reference();
		TEST(tmp == ref);
	}

	INFO_PRINTF1(_L("TSAREntry::Total"));
	INFO_PRINTF1(_L("TSAREntry::SetTotal"));
	for(i=0;i<KInterval;i++)
	{
		TInt tmp = (TInt)Math::Random();
//		INFO_PRINTF3(_L("%d %d"), i, tmp);
		aEntry.SetTotal(tmp);
		TInt ref = aEntry.Total();
		TEST(tmp == ref);
	}

	INFO_PRINTF1(_L("TSAREntry::Count"));
	INFO_PRINTF1(_L("TSAREntry::SetCount"));
	for(i=0;i<KInterval;i++)
	{
		TInt tmp = (TInt)Math::Random();
//		INFO_PRINTF3(_L("%d %d"), i, tmp);
		aEntry.SetCount(tmp);
		TInt ref = aEntry.Count();
		TEST(tmp == ref);
	}

	INFO_PRINTF1(_L("TSAREntry::IsComplete"));
	for(i=0;i<KInterval;i++)
	{
		TInt tmp = (TInt)Math::Random();
//		INFO_PRINTF3(_L("%d %d"), i, tmp);

		// should return ETrue
		aEntry.SetTotal(tmp);
		aEntry.SetCount(tmp);
		TEST(aEntry.IsComplete());

		// should return EFalse
		aEntry.SetCount(tmp+1);
		TEST(aEntry.IsComplete() == EFalse);
	}

	INFO_PRINTF1(_L("TSAREntry::LogServerId"));
	INFO_PRINTF1(_L("TSAREntry::SetLogServerId"));
	for(i=0;i<KInterval;i++)
	{
		TInt tmp = (TInt)Math::Random();
//		INFO_PRINTF3(_L("%d %d"), i, tmp);
		aEntry.SetLogServerId(tmp);
		TInt ref = aEntry.LogServerId();
		TEST(tmp == ref);
	}

	INFO_PRINTF1(_L("TSAREntry::Description1"));
	INFO_PRINTF1(_L("TSAREntry::SetDescription1"));
	for(i=0;i<KInterval;i++)
	{
		const TInt KTopLimit = TSAREntry::ESmsSAREntryDescriptionLength;

		TBuf<KTopLimit> buf;
		for(TInt j=0;j<KTopLimit;j++)
			buf.AppendFormat(_L("%c"), Math::Random() % 128);

//		INFO_PRINTF3(_L("%d %S"), i, &buf);
		aEntry.SetDescription1(buf);
		TPtrC buf2 = aEntry.Description1();
		TEST(buf == buf2);
	}

	INFO_PRINTF1(_L("TSAREntry::Description2"));
	INFO_PRINTF1(_L("TSAREntry::SetDescription2"));
	for(i=0;i<KInterval;i++)
		{
		const TInt KTopLimit = TSAREntry::ESmsSAREntryDescriptionLength;

		TBuf<KTopLimit> buf;
		for(TInt j=0;j<KTopLimit;j++)
			buf.AppendFormat(_L("%c"), Math::Random() % 128);

//		INFO_PRINTF3(_L("%d %S"), i, &buf);
		aEntry.SetDescription2(buf);
		TPtrC buf2 = aEntry.Description2();
		TEST(buf == buf2);
		}

	INFO_PRINTF1(_L("TSAREntry::Time"));
	INFO_PRINTF1(_L("TSAREntry::SetTime"));
	for(i=0;i<KInterval;i++)
		{
		const TUint high = (TUint)Math::Random();
		const TUint low = (TUint)Math::Random();
		const TTime tmp_time(MAKE_TINT64(high, low));

//		INFO_PRINTF3(_L("%d %d"), i, tmp_time.Int64() );
		aEntry.SetTime(tmp_time);
		TTime tmp2_time = aEntry.Time();
		TEST(tmp_time == tmp2_time);
		}

	INFO_PRINTF1(_L("TSAREntry::DataStreamId"));
	INFO_PRINTF1(_L("TSAREntry::SetDataStreamId"));
	for(i=0;i<KInterval;i++)
		{
		const TStreamId id(Math::Random()%KMaxStreamIdValue);
//		INFO_PRINTF3(_L("%d %d"), i, id.Value() );
		aEntry.SetDataStreamId(id);
		TStreamId id2 = aEntry.DataStreamId();
		TEST(id == id2);
		}

}



void CTestGsmuStorTSAREntry::TestSmsReassemblyEntryL(TSmsReassemblyEntry& aEntry)
/**
 *  test getters and setters on the TSmsReassemblyEntry class
 */
{
	TUint i;

	INFO_PRINTF1(_L("TSmsReassemblyEntry::iData1"));
	for(i=0;i<KInterval;i++)
		{
		const TInt b7to4 = (TUint)Math::Random() & 0xff;
		const TInt id1   = (TUint)Math::Random() & 0xff;
		const TInt id2   = (TUint)Math::Random() & 0xff;
		INFO_PRINTF5(_L("%d %d %d %d"), i, b7to4, id1, id2 );
		aEntry.SetBits7to4andIdentifiers(b7to4, id1, id2);
		const TInt b7to4_ = aEntry.Bits7to4();
		const TInt id1_   = aEntry.Identifier1();
		const TInt id2_   = aEntry.Identifier2();
		TEST(b7to4 == b7to4_);
		TEST(id1 == id1_);
		TEST(id2 == id2_);
		}

	INFO_PRINTF1(_L("TSmsReassemblyEntry::iData3 (PDU type)"));
	for(i=0;i<KInterval;i++)
		{
		const CSmsPDU::TSmsPDUType type = (CSmsPDU::TSmsPDUType)((Math::Random() % 6) & 0xFF);
		const TBool passed = Math::Random() & 0x01;

		INFO_PRINTF4(_L("%d Type %d PassedToClient %d"), i, type, passed);

		const TBool passedBefore = aEntry.PassedToClient();

		aEntry.SetPduType(type);

		TBool passed_ = aEntry.PassedToClient();
		CSmsPDU::TSmsPDUType type_ = aEntry.PduType();
		TEST(passedBefore && passed_ || !passedBefore && !passed_);
		TEST(type == type_);

		aEntry.SetPassedToClient(passed);

		passed_ = aEntry.PassedToClient();
		type_ = aEntry.PduType();
		TEST(passed && passed_ || !passed && !passed_);
		TEST(type == type_);
		}

	INFO_PRINTF1(_L("TSmsReassemblyEntry::iData4 (Storage)"));
	for(i=0;i<KInterval;i++)
		{
		const CSmsMessage::TMobileSmsStorage stor = (CSmsMessage::TMobileSmsStorage)(Math::Random() & 0xff);
		INFO_PRINTF3(_L("%d %d"), i, stor );
		aEntry.SetStorage(stor);
		const CSmsMessage::TMobileSmsStorage stor_ = aEntry.Storage();
		TEST(stor == stor_);
		}

}



void CTestGsmuStorTSAREntry::TestSmsSegmentationEntryL(TSmsSegmentationEntry& aEntry)
/**
 *  test getters and setters on the TSmsSegmentationEntry class
 */
{
	TUint i;

	INFO_PRINTF1(_L("TSmsSegmentationEntry::iData1 (Delivered + Failed)"));
	for(i=0;i<KInterval;i++)
		{
		const TUint16 deli = (TUint16)(Math::Random() & 0xffff);
		const TUint16 fail = (TUint16)(Math::Random() & 0xffff);
//		INFO_PRINTF4(_L("%d %d %d"), i, deli, fail );
		aEntry.SetDeliveredAndFailed(deli, fail);
		const TUint16 deli_ = (TUint16)aEntry.Delivered();
		const TUint16 fail_ = (TUint16)aEntry.Failed();
		TEST(deli == deli_);
		TEST(fail == fail_);
		}

	INFO_PRINTF1(_L("TSmsSegmentationEntry::iData3 (Validity Period)"));
	for(i=0;i<KInterval;i++)
		{
		const TInt val = (TInt)Math::Random();
//		INFO_PRINTF3(_L("%d %d"), i, val );
		aEntry.SetValidityPeriod(val);
		const TInt val_ = aEntry.ValidityPeriod();
		TEST(val == val_);
		}

	INFO_PRINTF1(_L("TSmsSegmentationEntry::iData4 (SR + PDU type + Refs)"));
	for(i=0;i<KInterval;i++)
		{
		const TBool sr = (TBool)(Math::Random() & 0x01);
		const CSmsPDU::TSmsPDUType type = (CSmsPDU::TSmsPDUType)(Math::Random() & 0xff);
		const TInt ref1 = (TInt)(Math::Random() & 0xff);
		const TInt ref2 = (TInt)(Math::Random() & 0xff);
//		INFO_PRINTF6(_L("%d %d %d %d %d"), i, sr, type, ref1, ref2 );
		aEntry.SetPduTypeAndRefs(sr, type, ref1, ref2);
		const TBool sr_ = aEntry.ExpectStatusReport();
		const CSmsPDU::TSmsPDUType type_ = aEntry.PduType();
		const TInt ref1_ = aEntry.Reference1();
		const TInt ref2_ = aEntry.Reference2();
		TEST(sr == sr_);
		TEST(type == type_);
		TEST(ref1 == ref1_);
		TEST(ref2 == ref2_);
		}

}


void CTestGsmuStorTSAREntry::TestWapReassemblyEntryL(TWapReassemblyEntry& aEntry)
/**
 *  test getters and setters on the TWapReassemblyEntry class
 */
	{
	TUint i;

	INFO_PRINTF1(_L("TWapReassemblyEntry::iData1 (to port)"));
	for(i=0;i<KInterval;i++)
		{
		const TInt port = (TInt)(Math::Random());
//		INFO_PRINTF3(_L("%d %d"), i, port );
		aEntry.SetToPort(port);
		const TInt port_ = aEntry.ToPort();
		TEST(port == port_);
		}
	}


TVerdict CTestGsmuStorCSARStoreClass::doTestStepL()
	{
#if 0
	// always delete the file before the test
	iFs.Delete(KTestGsmuStoreName);

	//
	// prepare
	//
	CTestSARStore* sar = CTestSARStore::NewL(iFs);
	sar->OpenL(KTestGsmuStoreName, KTestGsmuStoreUid);

	//
	// testing publics
	//
	const CArrayFix<TSAREntry>& sarArray = sar->Entries();
	TInt nEntries = sarArray.Count();

	TSAREntry dummySarEntry;
//	dummySarEntry.SetDataStreamId(1);

	for(TInt i=0;i<64;i++)
		  {
		  sar->  OpenFileLC();

		  INFO_PRINTF2(_L("entries: %d"), nEntries );
		  // add two entries
		  sar->AddEntryL(dummySarEntry);  //  Adds entry
		  sar->AddEntryL(dummySarEntry);  //  Adds entry
		  nEntries += 2;
		  const CArrayFix<TSAREntry>& sarArrayTmp = sar->Entries();
		  TInt nEntriesCount = sarArrayTmp.Count();
		  TEST(nEntriesCount == nEntries);

#if 0
		  // delete a random entry
		  sar->DeleteEntryL((TInt)((Math::Random() % nEntries) - 1));
		  nEntries -= 1;
		  const CArrayFix<TSAREntry>& sarArrayTmp2 = sar->Entries();
		  TInt nEntriesCount2 = sarArrayTmp2.Count();
		  TEST(nEntriesCount2 == nEntries);
#endif // 0
		  sar->CommitTransactionL();
		  }



	//
	// cleanup
	//
	sar->Close();
#endif // 0

	INFO_PRINTF1(_L("Well done"));
	return TestStepResult();
	}

