// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test step definitions for the PhoneBookStore functional unit.
// 
//

/**
 @internalTechnology
*/
 
#include <etelmm.h>

#include "cetelsessionmgr.h"
#include "cctsyintegrationtestphonebookstore.h" 

const TInt KPBDataClientBuf10 = 500;
const TInt KPBDataClientBuf50 = 2500;
const TInt KPBDataClientBuf100 = 5000;

//15 names and numbers
_LIT16(KAnnText,"Ann");
_LIT16(KMattSText,"MattS");
_LIT16(KAndyText,"Andy");
_LIT16(KMattMText,"MattM");
_LIT16(KPaulText,"Paul");
_LIT16(KMumAndDadText,"Mum and Dad");
_LIT16(KHenrikText,"Henrik");
_LIT16(KHenrikSVText,"HenrikSV");
_LIT16(KMikaelText,"Mikael");
_LIT16(KIngeText,"Inge");
_LIT16(KAndersText,"Anders");
_LIT16(KTextTooLong,"ABCDEFGHIJKLMNOPQRSTUVWXYZABCD");

_LIT16(KAnnPhoneNum,"01632960000");
_LIT16(KMattSPhoneNum,"01632960000");
_LIT16(KAndyPhoneNum,"01632960000");
_LIT16(KMattMPhoneNum,"01632960000");
_LIT16(KPaulPhoneNum,"01632960000");
_LIT16(KMumAndDadPhoneNum,"01632960000");
_LIT16(KHenrikPhoneNum,"01632960000");
_LIT16(KHenrikSVPhoneNum,"01632960000");
_LIT16(KMikaelPhoneNum,"01632960000");
_LIT16(KIngePhoneNum,"01632960000");
_LIT16(KAndersPhoneNum,"01632960000");
_LIT16(KNumberTooLong,"0171123456789012345678901234567331111114011111111150111111116011111111701111111180111");// needs to be over 80 since the  phone can take that many digits

	
CCTSYIntegrationTestPhoneBookStoreBase::CCTSYIntegrationTestPhoneBookStoreBase(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestSuiteStepBase(aEtelSessionMgr), iSimTsyTestHelper(*this), iPhoneTsyTestHelper(*this), iNetworkTsyTestHelper(*this)
/**
 * Constructor
 */
	{
	}

CCTSYIntegrationTestPhoneBookStoreBase::~CCTSYIntegrationTestPhoneBookStoreBase()
/*
 * Destructor
 */
	{
	}

TBool CCTSYIntegrationTestPhoneBookStoreBase::EncodeTLVFormat(CPhoneBookBuffer* aPbBuffer
		, TBuf<KBufSize>& aNumber, TUint8 aNumberType, TBuf<KBufSize>& aName)
/**
 * member function for encoding in ASN.1 TLV(Tag-Length-Value) format taking four arguments and returning TBool value
 * 
 * @param aPbBuffer a CPhoneBookBuffer pointer to hold encoded binary stream
 * @param aNumber a reference to TBuf<KBufSize> holding phone number to be encoded
 * @param aNumberType a TUint8 argument holding type phone number to be encoded
 * @param aName a reference to TBuf<KBufSize> holding the name to be encoded
 *
 * @return ETrue if success, EFalse otherwise.
 */
	{	
	TBool functionExecutionStatus = ETrue; //Success
    TInt ret = aPbBuffer->AddNewEntryTag();
	if(ret != KErrNone)
		{	
		DEBUG_PRINTF1(_L("CPhoneBookBuffer::AddNewEntryTag failed"));
		functionExecutionStatus = EFalse;
		}
	ret = aPbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBNumber
			, aNumber);
	if(ret != KErrNone)
		{		
		DEBUG_PRINTF1(_L("CPhoneBookBuffer::PutTagAndValue failed for number"));
		functionExecutionStatus = EFalse;
		}
	// convert number type into TLV format and append it to allocated buffer
	ret=aPbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBTonNpi, aNumberType);
	if(ret != KErrNone)
		{	
		DEBUG_PRINTF1(_L("CPhoneBookBuffer::PutTagAndValue failed for number type"));
		functionExecutionStatus = EFalse;
		}
	// convert text into TLV format and append it to allocated buffer
	ret=aPbBuffer->PutTagAndValue(RMobilePhoneBookStore::ETagPBText, aName);
	if(ret != KErrNone)
		{	
		DEBUG_PRINTF1(_L("CPhoneBookBuffer::PutTagAndValue failed for name"));
		functionExecutionStatus = EFalse;
		}
	return functionExecutionStatus;
	}

TBool CCTSYIntegrationTestPhoneBookStoreBase::DecodeTLVFormat
		(CPhoneBookBuffer* aPbBuffer, TInt aBufLen, TBuf<KBufSize>& aNumber
				, TBuf<KBufSize>& aName, TInt aIndex)
/**
 * member function for decoding a buffer in ASN.1 TLV(Tag-Length-Value) format taking five arguments and returning TBool value
 * 
 * @param aPbBuffer a CPhoneBookBuffer pointer to holding encoded binary stream to be decoded
 * @param aBufLen a TInt argument holding the length of the buffer to be decoded
 * @param aNumber a reference to TBuf<KBufSize> to hold decoded phone number
 * @param aName a reference to TBuf<KBufSize> to hold decoded name
 * @param aIndex a TInt argument holding the index of the record to be decoded
 *
 * @return ETrue if success, EFalse otherwise.
 */
	{
	TBool functionExecutionStatus = EFalse; //Failure
	
	TUint8 tagValue(0);
	CPhoneBookBuffer::TPhBkTagType dataType;
	TPtrC field(NULL,0);
	
	aPbBuffer->StartRead();       // Start reading received phonebook data
	TInt offset = 0;
	TInt pbRecordIndex = 0;
	while(offset < aBufLen)
		{
		TInt ret = aPbBuffer->GetTagAndType(tagValue, dataType);
		if( ret == KErrNotFound ) // Reached the end of phonebook data
			{		
			break;		
			}
			
		switch(dataType)
			{
			case CPhoneBookBuffer::EPhBkTypeInt8:
				{
				TUint8 int8(0);
				offset = offset + 1; // add tag-field only in case that is beginning of new field
				if(tagValue == RMobilePhoneBookStore::ETagPBTonNpi)
					{
					aPbBuffer->GetValue(int8); //
					offset = offset + 1; // 1-byte integer
					}
				}
			break;

			case CPhoneBookBuffer::EPhBkTypeInt16:
				{
				TUint16 int16(0);
				aPbBuffer->GetValue(int16);
				offset = offset + 3; // 2-byte integer + tag-field
				}
			break;

			case CPhoneBookBuffer::EPhBkTypeInt32:
				{
				TUint32 int32(0);
				aPbBuffer->GetValue(int32);
				offset = offset + 5; // 4-byte integer + tag-field
				}
			break;

			case CPhoneBookBuffer::EPhBkTypeDes16:
				{				
				aPbBuffer->GetValue(field);
				offset = offset + field.Length() + 3; // add desc length + length-field + tag-field
				if(tagValue == RMobilePhoneBookStore::ETagPBText)
					{
					aName = field;
					}
				else if(tagValue == RMobilePhoneBookStore::ETagPBNumber)
					{
					aNumber = field;	
					pbRecordIndex++;
					if(pbRecordIndex == aIndex)
						{
						functionExecutionStatus = ETrue;
						return functionExecutionStatus;  //completed reading the required record
						}
					}
				}
				break;
			default:
				break;
			} // end switch aDataType
		} // end while
	
	return functionExecutionStatus;
	}

CCTSYIntegrationTestPhoneBookStore0001Base::CCTSYIntegrationTestPhoneBookStore0001Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0001Base::~CCTSYIntegrationTestPhoneBookStore0001Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0001Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId
						,RMobilePhoneBookStore::TMobilePhoneBookLocation aExpectedPhoneBookLocation,
						TUint32 aExpectedCaps)
/**
 * TestDesc: Get phone store information.
 * TestExpectedResults: Pass - RMobilePhoneBookStore::TMobilePhoneBookInfoV1 returned with correct information.
 * Reason for test: Verify phone store information returned is correct.
 */
	{

	//
	// SET UP
	//

	// Open the phone book subsession  
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//	

	// Get the phone book store information with RMobilePhoneStore::GetInfo 
	TExtEtelRequestStatus phoneStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneStoreInfoStatus, phoneBookInfoPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoStatus
		, ETimeShort), KErrNone, 	
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	ASSERT_EQUALS(phoneStoreInfoStatus.Int(), KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	// Check RMobilePhoneBookStore::TMobilePhoneBookInfoV1 (or derived class) returned contains:
	// iType == RMobilePhoneStore::EPhoneBookStore 
	// iTotalEntries >= -1 
	// iUsedEntries >= -1 
	// iMaxNumLength >= -1 
	// iMaxTextLength >= -1 
	ASSERT_EQUALS(phoneBookInfo.iType, RMobilePhoneStore::EPhoneBookStore,
		_L("RMobilePhoneBookStore::GetInfo did not return RMobilePhoneStore::EPhoneBookStore."));
	ASSERT_TRUE((phoneBookInfo.iTotalEntries >= -1),
		_L("RMobilePhoneBookStore::GetInfo returned incorrect TotalEntries."));
	ASSERT_TRUE((phoneBookInfo.iUsedEntries >= -1),
		_L("RMobilePhoneBookStore::GetInfo returned incorrect UsedEntries."));
	ASSERT_TRUE((phoneBookInfo.iMaxNumLength >= -1),
		_L("RMobilePhoneBookStore::GetInfo returned incorrect MaxNumLength."));
	ASSERT_TRUE((phoneBookInfo.iMaxTextLength >= -1),
		_L("RMobilePhoneBookStore::GetInfo returned incorrect MaxTextLength."));

	// If we are testing the BDN phonebook 
	// iLocation == RMobilePhoneBookStore::ELocationUnknown 
	// otherwise check 
	// iLocation == RMobilePhoneBookStore::ELocationIccMemory 	
	ASSERT_EQUALS(phoneBookInfo.iLocation
		, aExpectedPhoneBookLocation,
		_L("RMobilePhoneBookStore::GetInfo returned incorrect Location information."));

	// Check iCaps for the following depending on which phone book is currently being tested:
	// ADN phone book: iCaps == RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEvent 
	// FDN phone book: iCaps == RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEvent | RMobilePhoneBookStore::KCapsRestrictedWriteAccess 
	// VMBX (voice mail box) phone book: iCaps == RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess | RMobilePhoneStore::KCapsDeleteAll | RMobilePhoneStore::KCapsNotifyEvent 
	// SDN phone book: iCaps == RMobilePhoneStore::KCapsReadAccess 
	// BDN phone book: iCaps == 0 
	// MBDN (mailbox dialling numbers) phone book: iCaps == RMobilePhoneStore::KCapsReadAccess | RMobilePhoneStore::KCapsWriteAccess 
	ASSERT_BITS_SET(phoneBookInfo.iCaps, aExpectedCaps, KNoUnwantedBits, 
		_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
	
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::PopAndDestroy(&phoneStoreInfoStatus);
	
	}


CCTSYIntegrationTestPhoneBookStore0002Base::CCTSYIntegrationTestPhoneBookStore0002Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0002Base::~CCTSYIntegrationTestPhoneBookStore0002Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0002Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Read single and multiple phonebook entries.
 * TestExpectedResults: Pass - Single and multiple entries read successfully.
 * Reason for test: Verify single and multiple entries can be read and entry information is correct.
 */
	{

	//
	// SET UP
	//

	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess | KCapsDeleteAll 
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL
		(KMainServer, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);

	
	TExtEtelRequestStatus phoneStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
							| RMobilePhoneStore::KCapsReadAccess
							| RMobilePhoneStore::KCapsDeleteAll;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
		_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
		
	// Delete all entries from the phone book with RMobilePhoneBookStore::DeleteAll 
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
		, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
		_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	CHECK_TRUE_L((deleteStoreStatus.Int()==KErrNone 
		|| deleteStoreStatus.Int() == KErrNotFound),
		_L("RMobilePhoneBookStore::DeleteAll returned with an error."))

	// Write entries into the first 10 slots of the phone book. 
	const TUint32 count = 10;	
	TFixedArray<TBuf<KBufSize>,count> names;
	TFixedArray<TBuf<KBufSize>,count> numbers;	
	names[0].Copy(KAnnText);
	numbers[0].Copy(KAnnPhoneNum);
	names[1].Copy(KMattSText);
	numbers[1].Copy(KMattSPhoneNum);
	names[2].Copy(KAndyText);
	numbers[2].Copy(KAndyPhoneNum);
	names[3].Copy(KMattMText);
	numbers[3].Copy(KMattMPhoneNum);
	names[4].Copy(KPaulText);
	numbers[4].Copy(KPaulPhoneNum);
	names[5].Copy(KMumAndDadText);
	numbers[5].Copy(KMumAndDadPhoneNum);
	names[6].Copy(KHenrikText);
	numbers[6].Copy(KHenrikPhoneNum);
	names[7].Copy(KHenrikSVText);
	numbers[7].Copy(KHenrikSVPhoneNum);
	names[8].Copy(KMikaelText);
	numbers[8].Copy(KMikaelPhoneNum);
	names[9].Copy(KIngeText);
	numbers[9].Copy(KIngePhoneNum);	
	TBuf8<KPBDataClientBuf10> pbData;  // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	for(TInt i=1;i<=count;++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		pbData.FillZ();
		pbData.Zero();
		TBool ret = EncodeTLVFormat(pbBuffer, numbers[i-1], (TUint8)RMobilePhone::EUnknownNumber
				, names[i-1]);
		CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));	
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore
								, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, i);		
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
		CleanupStack::PopAndDestroy(&phoneBookWriteStatus);
		}

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Read single entry from each of filled slots ===

	for(TInt i = 1; i <= count; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		pbData.FillZ();
		pbData.Zero();		
		TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
			, EMobilePhoneBookStoreRead);	
		CleanupStack::PushL(phoneBookReadStatus);
		
		// For each of the filled phone book slots, read a single entry from the phone book using RMobilePhoneBookStore::Read and setting aNumSlots = 1		
		phoneBookStore.Read(phoneBookReadStatus, i, 1, pbData);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
		ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

		// Check that the aPBData TLV descriptor returned contains correct entry information.
		TBuf<KBufSize> readNumber;
		TBuf<KBufSize> readName;		
		TInt bufLen = pbData.Length(); 

		// decode PB data received in TLV format		
		TBool ret = DecodeTLVFormat(pbBuffer, bufLen, readNumber, readName, 1);
		ASSERT_TRUE(ret, _L("DecodeTLVFormat failed"))				
		ASSERT_EQUALS_DES16(numbers[i-1], readNumber,
			_L("RMobilePhoneBookStore::Read completed with incorrect number"));
		ASSERT_EQUALS_DES16(names[i-1], readName,
			_L("RMobilePhoneBookStore::Read completed with incorrect name"));		
		CleanupStack::PopAndDestroy(&phoneBookReadStatus);
		}
	// ===  Read five entries where all requested slots are filled ===
	// Read 5 entries from the phone book using RMobilePhoneBookStore::Read with aNumSlots = 5 and using aIndex = 1, aIndex = 5 
	// Read from phone book starting from aIndex=1 reading 5 slots

	pbData.FillZ();
	pbData.Zero();	
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
			, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);
	phoneBookStore.Read(phoneBookReadStatus, 1, 5, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Read returned with an error"));
	TInt bufLen = pbData.Length(); 
	// Check that the aPBData TLV descriptor returned contains 5 entries with correct entry information.
	for(TInt i = 1; i <= 5; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		TBuf<KBufSize> readNumber;
		TBuf<KBufSize> readName;		
		// decode PB data received in TLV format		
		TBool ret = DecodeTLVFormat(pbBuffer, bufLen, readNumber, readName, i);
		ASSERT_TRUE(ret, _L("DecodeTLVFormat failed"));

		ASSERT_EQUALS_DES16(numbers[i-1], readNumber,
			_L("RMobilePhoneBookStore::Read completed with incorrect number"));
		ASSERT_EQUALS_DES16(names[i-1], readName,
			_L("RMobilePhoneBookStore::Read completed with incorrect name"));
		}

	// Read from phone book starting from aIndex=5 reading 5 slots
	pbData.FillZ();
	pbData.Zero();	
	phoneBookStore.Read(phoneBookReadStatus, 5, 5, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Read returned with an error"));
	bufLen = pbData.Length();
	for(TInt i = 1; i <= 5; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		TBuf<KBufSize> readNumber;
		TBuf<KBufSize> readName;		
		// decode PB data received in TLV format		
		TBool ret = DecodeTLVFormat(pbBuffer, bufLen, readNumber, readName, i);
		ASSERT_TRUE(ret,_L("DecodeTLVFormat failed"));
		ASSERT_EQUALS_DES16(numbers[(i+4)-1], readNumber,
			_L("RMobilePhoneBookStore::Read completed with incorrect number"));
		ASSERT_EQUALS_DES16(names[(i+4)-1], readName,
			_L("RMobilePhoneBookStore::Read completed with incorrect name"));		
		}	

	// ===  Read five entries where some of requested slots are filled ===
	// Read 5 entries from the phone book using RMobilePhoneBookStore::Read with aNumSlots = 5 and using aIndex = 7 
	pbData.FillZ();
	pbData.Zero();	
	phoneBookStore.Read(phoneBookReadStatus, 7, 5, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Read returned with an error"));
	bufLen = pbData.Length();
	// Check that the aPBData TLV descriptor returned contains 4 entries with correct entry information.
	for(TInt i = 1; i <= 5; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		TBuf<KBufSize> readNumber;
		TBuf<KBufSize> readName;		
		// decode PB data received in TLV format		
		TBool ret = DecodeTLVFormat(pbBuffer, bufLen, readNumber, readName, i);
		
		// For first 4 entries, check that received entry is same as that written 
		if(i < 5)
			{
			ASSERT_TRUE(ret,_L("DecodeTLVFormat failed"));
			ASSERT_EQUALS_DES16(numbers[(i+6)-1], readNumber,
				_L("RMobilePhoneBookStore::Read completed with incorrect number"));
			ASSERT_EQUALS_DES16(names[(i+6)-1], readName,
				_L("RMobilePhoneBookStore::Read completed with incorrect name"));
			}
		else
			{
			ASSERT_TRUE(!ret,_L("test found more entries than was expected"));
			}
		}
	
	//
	// TEST END
	//

    StartCleanup();
    
	// 1 phoneStoreInfoStatus
	// 2 deleteStoreStatus
	// 3 pbBuffer
	// 4 phoneBookReadStatus	
	CleanupStack::PopAndDestroy(4,&phoneStoreInfoStatus);	
	
	}


CCTSYIntegrationTestPhoneBookStore0003Base::CCTSYIntegrationTestPhoneBookStore0003Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0003Base::~CCTSYIntegrationTestPhoneBookStore0003Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0003Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Read single and multiple phonebook entries which are not present.
 * TestExpectedResults: Pass - KErrNotFound returned.
 * Reason for test: Verify KErrNotFound is returned.
 */
	{

	//
	// SET UP
	//
	// Get the phone store information with Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess | KCapsDeleteAll 
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
							| RMobilePhoneStore::KCapsReadAccess
							| RMobilePhoneStore::KCapsDeleteAll;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
		_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
		
	// Delete all entries from the phone book with RMobilePhoneBookStore::DeleteAll 
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
		, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
		_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	CHECK_TRUE_L((deleteStoreStatus.Int()==KErrNone 
		|| deleteStoreStatus.Int() == KErrNotFound),
		_L("RMobilePhoneBookStore::DeleteAll returned with an error."));

	// Get the phone store information and check iUsedEntries = 0, store the value of iTotalEntries. 
	phoneBookStore.GetInfo(phoneStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	CHECK_EQUALS_L(phoneBookInfo.iUsedEntries, 0, 
		_L("RMobilePhoneBookStore::GetInfo returned iUsedEntries not equal to zero"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
		
	// ===  Read single entry from first slot ===
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	// Read a single entry from the phone book using aNumSlots=1 and aIndex = 1 
	pbData.FillZ();
	pbData.Zero();	
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
			, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);

	phoneBookStore.Read(phoneBookReadStatus, 1, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrNotFound is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNotFound, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	// ===  Read single entry from middle of phone book ===

	// Read a single entry from the phone book using aNumSlots=1 and aIndex = 10 
	phoneBookStore.Read(phoneBookReadStatus, 10, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrNotFound is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNotFound, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	// ===  Read single entry from end of phone book ===

	// Read a single entry from the phone book using aNumSlots=1 and aIndex = iTotalEntries 
	phoneBookStore.Read(phoneBookReadStatus, phoneBookInfo.iTotalEntries, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrNotFound is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNotFound, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	// ===  Read multiple entries starting from first index ===

	// Read 5 entries from the phone book using aNumSlots = 5 and aIndex = 1. 
	phoneBookStore.Read(phoneBookReadStatus, 1, 5, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrNotFound is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNotFound, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	// ===  Read multiple entries starting from middle of phone book ===

	// Read 5 entries from the phone book using aNumSlots = 5 and aIndex = 10. 
	phoneBookStore.Read(phoneBookReadStatus, 10, 5, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrNotFound is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNotFound, 
			_L("RMobilePhoneBookStore::Read returned with an error"));
			
	// ===  Read multiple entries up to the end of the phone book ===

	// Read 5 entries from the phone book using aNumSlots = 5 and aIndex = iTotalEntries-5. 
	phoneBookStore.Read(phoneBookReadStatus, phoneBookInfo.iTotalEntries-5, 5, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrNotFound is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNotFound, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	// ===  Read multiple entries going over the end of the phone book ===

	// Read 5 entries from the phone book using aNumSlots = 5 and aIndex = iTotalEntries-2. 
	phoneBookStore.Read(phoneBookReadStatus, phoneBookInfo.iTotalEntries-2, 5, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrNotFound or KErrArgument is returned
	ASSERT_TRUE( ( phoneBookReadStatus.Int() == KErrArgument
					|| phoneBookReadStatus.Int() == KErrNotFound ), 
			_L("RMobilePhoneBookStore::Read returned with an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	//1 phoneStoreInfoStatus
	//2 deleteStoreStatus
	//3 pbBuffer
	//4 phoneBookReadStatus
	CleanupStack::PopAndDestroy(4,&phoneStoreInfoStatus);
	
	}


CCTSYIntegrationTestPhoneBookStore0004Base::CCTSYIntegrationTestPhoneBookStore0004Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0004Base::~CCTSYIntegrationTestPhoneBookStore0004Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0004Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
  *TestDesc: Read phonebook entry using invalid index.
 * TestExpectedResults: Pass - KErrArgument returned.
 * Reason for test: Verify KErrArgument returned.
 */
	{

	//
	// SET UP
	//
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess;
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10040);
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
		_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Read an entry with an invalid slot number ===
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	// Read a single entry from the phone book using aNumSlots=1 and aIndex = -1 
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
			, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);

	phoneBookStore.Read(phoneBookReadStatus, -1, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrArgument is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrArgument, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	// Read a single entry from the phone book using aNumSlots=1 and aIndex = -10 
	phoneBookStore.Read(phoneBookReadStatus, -10, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrArgument is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrArgument, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	// Read a single entry from the phone book using aNumSlots=10 and aIndex = -50 
	phoneBookStore.Read(phoneBookReadStatus, -50, 10, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrArgument is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrArgument, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	// ===  Read an entry with an invalid value for aNumSlots ===

	// Read a single entry from the phone book using aNumSlots=0 and aIndex = 1 
	phoneBookStore.Read(phoneBookReadStatus, 1, 0, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrArgument is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrArgument, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	// Read a single entry from the phone book using aNumSlots=-1 and aIndex = 1 
	phoneBookStore.Read(phoneBookReadStatus, 1, -1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrArgument is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrArgument, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	// Read a single entry from the phone book using aNumSlots=-10 and aIndex = 10 
	phoneBookStore.Read(phoneBookReadStatus, 10, -10, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrArgument is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrArgument, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	//
	// TEST END
	//

    StartCleanup();
	
	//1 phoneStoreInfoStatus
	//2 pbBuffer
	//3 phoneBookReadStatus
	CleanupStack::PopAndDestroy(3,&phoneStoreInfoStatus);
	
	}


CCTSYIntegrationTestPhoneBookStore0005Base::CCTSYIntegrationTestPhoneBookStore0005Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0005Base::~CCTSYIntegrationTestPhoneBookStore0005Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0005Base::DoTestProcedureL(TFixedArray<TPhoneBookStoreId,2>& aPhoneBookTypeArray)
/**
 * TestDesc: Read and write multiple phonebooks simultaneously.
 * TestExpectedResults: Pass - Numbers read successfully.
 * Reason for test: Verify it is possible to post read/write requests simultaneously to different phone books.
 */
	{

	//
	// SET UP
	//

	const TInt noOfPhoneBooks = 2;
	//	Not supported - KMeDialledPhoneBook, KMeMissedPhoneBook, KMeReceivedPhoneBook, KCombinedAdnPhoneBook, KTaAdnPhoneBook, KIccFdnPhoneBook, KIccSdnPhoneBook, KIccBdnPhoneBook,KIccLndPhoneBook, KIccVoiceMailBox,	KIccLndPhoneBook
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	RMobilePhoneBookStore *phoneBookStore[noOfPhoneBooks];	
	for(TInt i = 0; i< noOfPhoneBooks; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		RMobilePhoneBookStore& phoneBookStr = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer, KMainPhone, aPhoneBookTypeArray[i]);		
		iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookTypeArray[i]);		
		phoneBookStore[i] = &phoneBookStr;		
		TExtEtelRequestStatus phoneStoreInfoStatus(phoneBookStr
				, EMobilePhoneStoreGetInfo);
		CleanupStack::PushL(phoneStoreInfoStatus);
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
		RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
		phoneBookStr.GetInfo(phoneStoreInfoStatus, phoneBookInfoPckg);	
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneStoreInfoStatus, ETimeShort), KErrNone, 
			_L("RMobilePhoneBookStore::GetInfo timed-out"));
		CHECK_EQUALS_L(phoneStoreInfoStatus.Int(),  KErrNone,
			 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
		 // Ensure that SIM used is not full.
		if (phoneBookInfo.iUsedEntries == phoneBookInfo.iTotalEntries)    
			{
			// delete some entries
			TInt index = 1;
			
			TExtEtelRequestStatus deleteStoreStatus(phoneBookStr,EMobilePhoneStoreDelete);
			CleanupStack::PushL(deleteStoreStatus);
			phoneBookStr.Delete(deleteStoreStatus,index);
			CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
					_L("RMobilePhoneBookStore::Delete timed-out"));
			CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
							|| deleteStoreStatus.Int() == KErrNone ),		
						 _L("RMobilePhoneBookStore::Delete returned with an error."));
			index = 2;
			phoneBookStr.Delete(deleteStoreStatus,index);
			CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
					_L("RMobilePhoneBookStore::Delete timed-out"));
			CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
							|| deleteStoreStatus.Int() == KErrNone ),		
						 _L("RMobilePhoneBookStore::Delete returned with an error."));
			//deleteStoreStatus
			CleanupStack::PopAndDestroy(&deleteStoreStatus);

			}
		phoneBookStr.GetInfo(phoneStoreInfoStatus, phoneBookInfoPckg);	
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneStoreInfoStatus, ETimeShort), KErrNone, 
			_L("RMobilePhoneBookStore::GetInfo timed-out"));
		CHECK_EQUALS_L(phoneStoreInfoStatus.Int(),  KErrNone,
			 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
		CHECK_TRUE_L(phoneBookInfo.iUsedEntries < phoneBookInfo.iTotalEntries,
			 _L("RMobilePhoneBookStore::GetInfo phonebook is full"));

		// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess 
		TUint32 expectedCaps =  RMobilePhoneStore::KCapsWriteAccess
				| RMobilePhoneStore::KCapsReadAccess;
		CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));


		//1 phoneStoreInfoStatus
		//2 deleteStoreStatus
		CleanupStack::PopAndDestroy(&phoneStoreInfoStatus);
		}
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	TFixedArray<TBuf8<KPBDataClientBuf10>,2> pbData; // Client reserved space for phonebook data
	TFixedArray<CPhoneBookBuffer*,2> pbBuffer;
	pbBuffer[0] = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer[0]);
	pbBuffer[0]->Set(&pbData[0]); // Set it to point to the Client buffer
	pbBuffer[1] = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer[1]);
	pbBuffer[1]->Set(&pbData[1]); // Set it to point to the Client buffer

	// Get all the subsessions for all supported phone books. ADN, SDN etc. 
	_LIT(KTel,"123456%03d");
	_LIT(KText,"TelNo %03d");	
	TFixedArray<TBuf<KBufSize>,noOfPhoneBooks> names;
	TFixedArray<TBuf<KBufSize>,noOfPhoneBooks> numbers;
	TInt index[noOfPhoneBooks];
	TFixedArray<TExtEtelRequestStatus*,noOfPhoneBooks> phoneBookWriteStatus;
	TExtEtelRequestStatus pbWriteStatus1(*phoneBookStore[0], EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(pbWriteStatus1);
	phoneBookWriteStatus[0] = &pbWriteStatus1;
	TExtEtelRequestStatus pbWriteStatus2(*phoneBookStore[1], EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(pbWriteStatus2);
	phoneBookWriteStatus[1] = &pbWriteStatus2;

	
	// Write a single entry to each phone book using RMobilePhoneBookStore::Write 
	for(TInt i = 0; i< noOfPhoneBooks; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		numbers[i].Format(KTel,i);
		names[i].Format(KText,i);
		TBool ret = EncodeTLVFormat(pbBuffer[i], numbers[i], (TUint8)RMobilePhone::EUnknownNumber
				, names[i]);
		CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
		index[i]=-1;
		phoneBookStore[i]->Write(*phoneBookWriteStatus[i], pbData[i], index[i]);

		}
	
	for(TInt i = 0; i< noOfPhoneBooks; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		// Wait for each request to complete for each of the TExtEtelRequestStatus's used in the write operation.
		ASSERT_EQUALS(WaitForRequestWithTimeOut(*phoneBookWriteStatus[i]
  				, ETimeMedium), KErrNone,
  				_L("RMobilePhoneBookStore::Write timed-out"));
		// Check KErrNone is returned for each phone book
  		ASSERT_EQUALS(phoneBookWriteStatus[i]->Int(), KErrNone, 
  					_L("RMobilePhoneBookStore::Write returned with an error"));
		}

	// Read the written entry from each phone book using RMobilePhoneBookStore::Read 	
	TFixedArray<TExtEtelRequestStatus*,noOfPhoneBooks> phoneBookReadStatus;
	TExtEtelRequestStatus pbReadStatus1(*phoneBookStore[0], EMobilePhoneBookStoreRead);
	CleanupStack::PushL(pbReadStatus1);
	phoneBookReadStatus[0] = &pbReadStatus1;
	TExtEtelRequestStatus pbReadStatus2(*phoneBookStore[1], EMobilePhoneBookStoreRead);
	CleanupStack::PushL(pbReadStatus2);
	phoneBookReadStatus[1] = &pbReadStatus2;
	
	for(TInt i = 0; i < noOfPhoneBooks; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		pbData[i].FillZ();
		pbData[i].Zero();
		phoneBookStore[i]->Read(*phoneBookReadStatus[i], index[i], 1, pbData[i]);
		}
	
	for(TInt i = 0; i< noOfPhoneBooks; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		// Wait for each request to complete for each of the TExtEtelRequestStatus's used in the read operation. 	
		ASSERT_EQUALS(WaitForRequestWithTimeOut(*phoneBookReadStatus[i]
            				, ETimeMedium), KErrNone,
            				_L("RMobilePhoneBookStore::Read timed-out"));
		// Check KErrNone is returned for each phone book		
        ASSERT_EQUALS(phoneBookReadStatus[i]->Int(), KErrNone, 
        		_L("RMobilePhoneBookStore::Read returned with an error"));		
		}

	//
	// TEST END
	//

    StartCleanup();

	// 1 pbBuffer[0]
	// 2 pbBuffer[1]
    // 3 pbWriteStatus1
    // 4 pbWriteStatus2
    // 5 pbReadStatus1
    // 6 pbReadStatus2
    for(TInt i = 0; i< noOfPhoneBooks; ++i)
		{
		phoneBookWriteStatus[i] = NULL;
		phoneBookReadStatus[i] = NULL;
		}
    CleanupStack::PopAndDestroy(6,pbBuffer[0]);	
	}



CCTSYIntegrationTestPhoneBookStore0006Base::CCTSYIntegrationTestPhoneBookStore0006Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0006Base::~CCTSYIntegrationTestPhoneBookStore0006Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0006Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * @SYMTestCaseDesc: Read with no SIM present.
 * @SYMTestExpectedResults: Pass - Read request returns KErrEtelInitialisationFailure due to lack of SIM.
 * Reason for test: Verfiy KErrEtelInitialisationFailure is returned.
 */
	{

	//
	// SET UP
	//

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Remove SIM from phone. 
	TBuf<70> prompt(_L("Please ensure that you have removed the SIM before running this test"));
	DisplayUserInteractionPromptL(prompt); // 30 seconds
	
	// Get the phone book subsession. 
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);

	// Read an entry from the phone book. 
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer

	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
			, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);
	phoneBookStore.Read(phoneBookReadStatus, 1, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrEtelInitialisationFailure is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrEtelInitialisationFailure, 
			_L("RMobilePhoneBookStore::Read returned with an error"));

	//
	// TEST END
	//

    StartCleanup();
	
	//1 pbBuffer
	//2 phoneBookReadStatus
	CleanupStack::PopAndDestroy(2,pbBuffer);	
	
	}


CCTSYIntegrationTestPhoneBookStore0007Base::CCTSYIntegrationTestPhoneBookStore0007Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0007Base::~CCTSYIntegrationTestPhoneBookStore0007Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0007Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Cancel reading multiple entries.
 * TestExpectedResults: Pass - Some entries read by the time the request is cancelled.
 * Reason for test: Verify some entries but not all entries have been read before cancellation.
 */
	{

	//
	// SET UP
	//

	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
		_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
		
	// deleting all entries from store
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
		, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
		_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	CHECK_TRUE_L((deleteStoreStatus.Int()==KErrNone 
		|| deleteStoreStatus.Int() == KErrNotFound),
		_L("RMobilePhoneBookStore::DeleteAll returned with an error."));

	// Write 100 entries to the phone book to a known slots. 
	TBuf8<KPBDataClientBuf100> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	
	const TInt count = phoneBookInfo.iTotalEntries / 2; // for FDN
	_LIT(KTel,"123456%03d");
	_LIT(KText,"TelNo %03d");
	CArrayFixFlat<TBuf<KBufSize> > *names;
	names = new ( ELeave ) CArrayFixFlat<TBuf<KBufSize> >( count );
	CleanupStack::PushL(names);
	CArrayFixFlat<TBuf<KBufSize> > *numbers;
	numbers = new ( ELeave ) CArrayFixFlat<TBuf<KBufSize> >( count );
	CleanupStack::PushL(numbers);

	for(TInt i = 1; i<= count; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		TBuf<KBufSize> tempNum;
		TBuf<KBufSize> tempName;
		tempNum.Format(KTel,i);
		numbers->AppendL(tempNum);		
		tempName.Format(KText,i);
		names->AppendL(tempName);
		TBool ret = EncodeTLVFormat(pbBuffer, numbers->At(i-1), (TUint8)RMobilePhone::EUnknownNumber
				, names->At(i-1));
		CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, i);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
		pbData.FillZ();
		pbData.Zero();
		CleanupStack::PopAndDestroy(&phoneBookWriteStatus);		
		}

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Read 100 entries from the phone book using RMobilePhoneBookStore::Read 
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore, EMobilePhoneBookStoreRead);
	CleanupStack::PushL(phoneBookReadStatus);
	phoneBookStore.Read(phoneBookReadStatus, 1, count, pbData);
	
	// Wait for 5 seconds. 
	User::After(5*KOneSecond);
	
	// Cancel the RMobilePhoneBookStore::Read request. 
	phoneBookStore.CancelAsyncRequest(EMobilePhoneBookStoreRead);
	
	// Check KErrCancel returned for the request.			
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrCancel, 
					_L("RMobilePhoneBookStore::Read Cancel returned incorrect error"));

	// Check aEntry descriptor passed to RMobilePhoneBookStore::Read contains phone book entry information and contains >0 entries but <100 entries.
	TInt bufLen = pbData.Length(); 

	for(TInt i=1; i<= count; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		TBuf<KBufSize> readNumber;
		TBuf<KBufSize> readName;		
	
		// decode PB data received in TLV format
		TBool ret = DecodeTLVFormat(pbBuffer, bufLen, readNumber, readName, i);
		// if decode failed check that number of entries read is between 1 - 99
		if(!ret)
			{
			ASSERT_TRUE((i >= 0 && i < count ), _L("RMobilePhoneBookStore::Read Cancel returned entries out of range 0-100"));
			break;
			}
		ASSERT_TRUE(ret, _L("DecodeTLVFormat failed"));
		ASSERT_EQUALS_DES16(numbers->At(i-1), readNumber,
			_L("RMobilePhoneBookStore::Read completed with incorrect number"));
		ASSERT_EQUALS_DES16(names->At(i-1), readName,
			_L("RMobilePhoneBookStore::Read completed with incorrect name"));
		}
	
	//
	// TEST END
	//

    StartCleanup();
	
	//1 phoneStoreInfoStatus
	//2 deleteStoreStatus
    //3 pbBuffer
    //4 names
    //5 numbers
    //6 phoneBookReadStatus
    CleanupStack::PopAndDestroy(6,&phoneStoreInfoStatus);
	
	}



CCTSYIntegrationTestPhoneBookStore0008Base::CCTSYIntegrationTestPhoneBookStore0008Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0008Base::~CCTSYIntegrationTestPhoneBookStore0008Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0008Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Write national, international numbers to the SIM phone book.
 * TestExpectedResults: Pass - Valid numbers written correctly
 * Reason for test: Verify entries are written correctly and can be read.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL
							(KMainServer, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	 // Ensure that SIM used is not full.
	if (phoneBookInfo.iUsedEntries == phoneBookInfo.iTotalEntries)    
		{
		// delete some entries
		TInt index = 1;
		
		TExtEtelRequestStatus deleteStoreStatus(phoneBookStore,EMobilePhoneStoreDelete);
		CleanupStack::PushL(deleteStoreStatus);
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		index = 2;
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		}
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	CHECK_TRUE_L(phoneBookInfo.iUsedEntries < phoneBookInfo.iTotalEntries,
		 _L("RMobilePhoneBookStore::GetInfo phonebook is full"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess | KCapsNotifyEvent 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
						  |	RMobilePhoneStore::KCapsReadAccess
						  | RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
		_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	TInt const count = 2;
	
	TFixedArray<TBuf<KBufSize>,count> names;
	TFixedArray<TBuf<KBufSize>,count> numbers;	
	names[0].Copy(KAnnText);
	numbers[0].Copy(KAnnPhoneNum);
	names[1].Copy(_L("IntrntnlName"));
	numbers[1].Copy(_L("+441632960000"));
	
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	
	
	// ===  Find out how many entries are used in the phone book ===

	for(TInt i = 0; i < count; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		
		// Get phone store information using RMobilePhone::GetPhoneStoreInfo 
		// Store the iUsedEntries returned. 
		TInt mobileGetPhoneStoreUsedEntries = 0;

		// Get phone book store information using RMobilePhoneStore::GetInfo 
		// Store the iUsedEntries returned. 
		TInt mobileGetInfoUsedEntries = 0;
		RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
		
		ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
				phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
				_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	
	
		// ===  Write a valid entry ===
	
		// Write a valid entry with a national number to the phone book specifying a specific index to store the entry which does not contain an entry. 
		// write national number
		TInt index = -1;		

		// post notification for RMobilePhoneBookStore::Write		 
		TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
		CleanupStack::PushL(notifyStoreEventStatus);
		TUint32 event=0;
		TInt notifyIndex=0;	
		phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);
		pbData.FillZ();
		pbData.Zero();
		TBool ret = EncodeTLVFormat(pbBuffer, numbers[i], (TUint8)RMobilePhone::EUnknownNumber
				, names[i]);
		ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));	
		
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		ASSERT_EQUALS(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
	
		// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreEntryAdded
		iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,RMobilePhoneStore::KStoreEntryAdded,notifyIndex,KErrNone);
		
		// ===  Read the written entry ===
	
		// Read the entry from the phone book. 
		pbData.FillZ();
		pbData.Zero();		
		TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
			, EMobilePhoneBookStoreRead);	
		CleanupStack::PushL(phoneBookReadStatus);
		
		// For each of the filled phone book slots, read a single entry from the phone book using RMobilePhoneBookStore::Read and setting aNumSlots = 1		
		phoneBookStore.Read(phoneBookReadStatus, index, 1, pbData);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
		ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Read returned with an error"));
	
		// Check that the entry information is the same as that written.
		TBuf<KBufSize> readNumber;
		TBuf<KBufSize> readName;		
		TInt bufLen = pbData.Length(); 
	
		// decode PB data received in TLV format		
		ret = DecodeTLVFormat(pbBuffer, bufLen, readNumber, readName, 1);
		ASSERT_TRUE(ret, _L("DecodeTLVFormat failed"));
		ASSERT_EQUALS_DES16(numbers[i], readNumber,
			_L("RMobilePhoneBookStore::Read completed with incorrect number"));
		ASSERT_EQUALS_DES16(names[i], readName,
			_L("RMobilePhoneBookStore::Read completed with incorrect name"));
	
		// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===
		// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored.
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.
		ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, phoneBookInfoPckg,
							mobilePhone,mobileGetInfoUsedEntries+1,  mobileGetPhoneStoreUsedEntries+1),
					KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
		
		//1 notifyStoreEventStatus
		//2 phoneBookWriteStatus
		//3 phoneBookReadStatus
		CleanupStack::PopAndDestroy(3,&notifyStoreEventStatus);				
		}
	//
	// TEST END
	//

    StartCleanup();
	
	//1 phoneBookStoreInfoStatus
	//2 pbBuffer
	CleanupStack::PopAndDestroy(2,&phoneBookStoreInfoStatus);
	
	
	}



CCTSYIntegrationTestPhoneBookStore0009Base::CCTSYIntegrationTestPhoneBookStore0009Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0009Base::~CCTSYIntegrationTestPhoneBookStore0009Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0009Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Write invalid entries to SIM phone book (e.g. name too long)
 * TestExpectedResults: Pass - KErrGsm0707TextStringTooLong returned if the name or number is too long. KErrArgument if a bad descriptor is supplied.
 * Reason for test: Verify error is returned on attempt to write an invalid entry and number of used entries in phone book is unchanged.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
						  |	RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
		_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));

	// Ensure there are at least 3 free slots in the phone book 
	if (phoneBookInfo.iUsedEntries + 3 >= phoneBookInfo.iTotalEntries)    
		{
		// delete some entries
		TInt index = 1;
		
		TExtEtelRequestStatus deleteStoreStatus(phoneBookStore,EMobilePhoneStoreDelete);
		CleanupStack::PushL(deleteStoreStatus);
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		index = 2;
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		index = 3;
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		CleanupStack::PopAndDestroy(&deleteStoreStatus);
		}
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	if(phoneBookInfo.iTotalEntries != -1)
		{
		CHECK_TRUE_L(phoneBookInfo.iUsedEntries+3 < phoneBookInfo.iTotalEntries,
			_L("RMobilePhoneBookStore::GetInfo phonebook is full"));
		}
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	ASSERT_EQUALS(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Store iUsedEntries, iMaxNumLength and iMaxTextLength returned for future use 
	TInt mobilePhoneBookStoreUsedEntries = phoneBookInfo.iUsedEntries;

	// ===  Attempt to write a name that is too long ===

	TInt index = -1;	

	// Write an entry to the SIM phone book which has a name of length > iMaxTextLength 				 
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	
	TBuf<KBufSize> names(KTextTooLong);
	TBuf<KBufSize> numbers(KAnnPhoneNum);	
	TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
			
	// Check KErrGsm0707TextStringTooLong returned
	ASSERT_EQUALS(phoneBookWriteStatus.Int(), KErrGsm0707TextStringTooLong, 
		_L("RMobilePhoneBookStore::Write returned with an error"));

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	ASSERT_EQUALS(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Check that iUsedEntries is same as that stored earlier
	ASSERT_EQUALS(phoneBookInfo.iUsedEntries,mobilePhoneBookStoreUsedEntries,
		 _L("RMobilePhoneBookStore::GetInfo phone book contains more than number of entries written."));

	// ===  Attempt to write a number that is too long ===

	// Write an entry to the SIM phone book which has a number of length > iMaxNumLength 
	names.Copy(KAnnText);
	numbers.Copy(KNumberTooLong);	
	pbData.FillZ();
	pbData.Zero();
	ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));

		// code block below to be deleted after the defect is fixed
		{
		ERR_PRINTF1(_L("<font color=Cyan>$CTSYProblem: RMobilePhoneBookStore::Write check for MaxNumLength not present in ctsy, forcing failure.<font color=Black>"));
		ASSERT_TRUE(EFalse, _L("force failure for RMobilePhoneBookStore::Write"));
		}
	// Check KErrGsm0707TextStringTooLong returned
	ASSERT_EQUALS(phoneBookWriteStatus.Int(), KErrGsm0707TextStringTooLong, 
		_L("RMobilePhoneBookStore::Write returned with an error"));

	// Check that iUsedEntries is same as that stored earlier
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	ASSERT_EQUALS(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	ASSERT_EQUALS(phoneBookInfo.iUsedEntries,mobilePhoneBookStoreUsedEntries,
	 _L("RMobilePhoneBookStore::GetInfo phone book contains more than number of entries written."));

	// ===  Attempt to write a corrupt data entry ===

	// Write an entry to the SIM phone book with aPBData = random data (a descriptor containing no known tags from RMobilePhoneBookStore::TMobilePBFieldTags) 
	pbData.FillZ();
	pbData.Zero();
	pbData = _L8("randomText");
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeLong), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	
	// Check KErrArgument returned
	ASSERT_EQUALS(phoneBookWriteStatus.Int(), KErrArgument, 
		_L("RMobilePhoneBookStore::Write returned with an error"));

	// Check that iUsedEntries is same as that stored earlier
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	ASSERT_EQUALS(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	ASSERT_EQUALS(phoneBookInfo.iUsedEntries,mobilePhoneBookStoreUsedEntries,
	 _L("RMobilePhoneBookStore::GetInfo phone book contains more than number of entries written."));

	
	//
	// TEST END
	//

    StartCleanup();
	
	//1 phoneBookStoreInfoStatus
	//2 pbBuffer
	//3 phoneBookWriteStatus
	CleanupStack::PopAndDestroy(3,&phoneBookStoreInfoStatus);
	
	}



CCTSYIntegrationTestPhoneBookStore0010Base::CCTSYIntegrationTestPhoneBookStore0010Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0010Base::~CCTSYIntegrationTestPhoneBookStore0010Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0010Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Write a long (40 digit) number to the SIM phone book to USIM which supports extension fields. 
 * TestExpectedResults: Pass - A 40 digit number can be stored and retrieved.
 * Reason for test: Verify entry is written to the phone book.
 */
	{

	//
	// SET UP
	//
	
	// Insert USIM which supports extension fields, boot phone, load TSY etc. 
	TBuf<80> prompt(_L("Please insert USIM which supports extension fields, boot phone, load TSY etc"));
	DisplayUserInteractionPromptL(prompt); // 30 seconds

	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure SIM phone book is not full. 
	if (phoneBookInfo.iUsedEntries == phoneBookInfo.iTotalEntries)    
		{
		// delete some entries
		TInt index = 1;
		
		TExtEtelRequestStatus deleteStoreStatus(phoneBookStore,EMobilePhoneStoreDelete);
		CleanupStack::PushL(deleteStoreStatus);
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		index = 2;
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		CleanupStack::PopAndDestroy(&deleteStoreStatus);
		}
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	CHECK_TRUE_L(phoneBookInfo.iUsedEntries < phoneBookInfo.iTotalEntries,
				_L("RMobilePhoneBookStore::GetInfo phonebook is full"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
						  |	RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
		_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));

	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	ASSERT_EQUALS(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Store iUsedEntries returned for future use 
	TInt mobilePhoneBookStoreUsedEntries = phoneBookInfo.iUsedEntries;

	// ===  Write entry to phone book into next free slot ===

	// post notification using RMobilePhoneBookStore::NotifyStoreEvent for RMobilePhoneBookStore::Write	
	TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 event=0;
	TInt notifyIndex=0;	
	phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);	
				 
	 // Write a 40 digit long number to the SIM phone book supplying aEntry.index = -1 
	 TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	
	_LIT16(KLongPhoneNum,"1234567890123456789012345678901234567890");	
	TBuf<KBufSize> names(KAndersText);
	TBuf<KBufSize> numbers(KLongPhoneNum);	
	TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	TInt index = -1;
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	ASSERT_EQUALS(phoneBookWriteStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Write returned with an error"));

	// Check RMobilePhoneStore::NotifyStoreEvent returns KStoreEntryAdded
	iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,RMobilePhoneStore::KStoreEntryAdded,notifyIndex,KErrNone);

	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored.
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	ASSERT_EQUALS(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	ASSERT_EQUALS(phoneBookInfo.iUsedEntries,mobilePhoneBookStoreUsedEntries+1,
	 _L("RMobilePhoneBookStore::GetInfo phone book contains more than number of entries written."));

	// ===  Read entry ===

	// Read the written entry from the SIM. 
	pbData.FillZ();
	pbData.Zero();
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore, EMobilePhoneBookStoreRead);
	CleanupStack::PushL(phoneBookReadStatus);
	phoneBookStore.Read(phoneBookReadStatus, index, 1, pbData);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus, ETimeMedium),KErrNone, 
					_L("RMobilePhoneBookStore::Read did not complete"));
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNone, 
					_L("RMobilePhoneBookStore::Read Cancel returned incorrect error"));
	
	// Check entry information is correct.
	TInt bufLen = pbData.Length(); 

	TBuf<KBufSize> readNumber;
	TBuf<KBufSize> readName;		
	// decode PB data received in TLV format		
	ret = DecodeTLVFormat(pbBuffer, bufLen, readNumber, readName, 1);
	ASSERT_TRUE(ret, _L("DecodeTLVFormat failed"));				
	ASSERT_EQUALS_DES16(numbers, readNumber,
		_L("RMobilePhoneBookStore::Read completed with incorrect number"));	
	ASSERT_EQUALS_DES16(names, readName,
		_L("RMobilePhoneBookStore::Read completed with incorrect name"));		

	//
	// TEST END
	//

    StartCleanup();
	
	//1 phoneBookStoreInfoStatus
	//2 pbBuffer
	//3 notifyStoreEventStatus
	//4 phoneBookWriteStatus
	//5 phoneBookReadStatus
	CleanupStack::PopAndDestroy(5,&phoneBookStoreInfoStatus);
	
	}



CCTSYIntegrationTestPhoneBookStore0011Base::CCTSYIntegrationTestPhoneBookStore0011Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0011Base::~CCTSYIntegrationTestPhoneBookStore0011Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0011Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Write an entry to phonebook at index of -1 (first free slot).
 * TestExpectedResults: Pass - Entry written.
 * Reason for test: Verify entry is written correctly and correct slot number is returned.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure SIM phone book is not full. 
	if (phoneBookInfo.iUsedEntries == phoneBookInfo.iTotalEntries)    
		{
		// delete some entries
		TInt index = 1;
		
		TExtEtelRequestStatus deleteStoreStatus(phoneBookStore,EMobilePhoneStoreDelete);
		CleanupStack::PushL(deleteStoreStatus);
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		index = 2;
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		CleanupStack::PopAndDestroy(&deleteStoreStatus);
		}
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	CHECK_TRUE_L(phoneBookInfo.iUsedEntries < phoneBookInfo.iTotalEntries,
				_L("RMobilePhoneBookStore::GetInfo phonebook is full"));
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess | KCapsNotifyEvent 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
						|	RMobilePhoneStore::KCapsReadAccess
						| 	RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===
	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	// Store iUsedEntries returned for future use 
	TInt mobileGetInfoUsedEntries = 0;

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
	// Store iUsedEntries returned for future use 
	TInt mobileGetPhoneStoreUsedEntries = 0;
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	
	
	// ===  Write an entry to the first free slot ===	
	TInt index = -1;
	
	// post notification using RMobilePhoneBookStore::NotifyStoreEvent for RMobilePhoneBookStore::Write				 
	TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 event=0;
	TInt notifyIndex=0;	
	phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);
	
	// Write a valid entry to the phone book using RMobilePhoneBookStore::Write supplying aEntry.index = -1 
	TBuf<KBufSize> names(KAndersText);
	TBuf<KBufSize> numbers(KAndersPhoneNum);	
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	

	TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));
	
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	ASSERT_EQUALS(phoneBookWriteStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Write returned with an error"));

	// Store the aEntry.iIndex returned for future use 
	TInt returnIndex = index;
	
	// Check RMobilePhoneStore::NotifyStoreEvent returns RMobilePhoneBookStore::KStoreEntryAdded
	iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,RMobilePhoneStore::KStoreEntryAdded,notifyIndex,KErrNone);
	
	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===
	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored.
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries+1,  mobileGetPhoneStoreUsedEntries+1),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	// ===  Read the written entry ===

	// Read the written entry at index aEntry.iIndex returned from the write 
	pbData.FillZ();
	pbData.Zero();		
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
		, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);	
	phoneBookStore.Read(phoneBookReadStatus, returnIndex, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
		, ETimeMedium), KErrNone,
		_L("RMobilePhoneBookStore::Read timed-out"));
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Read returned with an error"));
		
	// Check RMobilePhoneBookStore::Read returns correct entry information 
	TBuf<KBufSize> readNumber;
	TBuf<KBufSize> readName;		
	TInt bufLen = pbData.Length(); 
	
	// decode PB data received in TLV format		
	ret = DecodeTLVFormat(pbBuffer, bufLen, readNumber, readName, 1);
	ASSERT_TRUE(ret, _L("DecodeTLVFormat failed"));
	ASSERT_EQUALS_DES16(numbers, readNumber,
		_L("RMobilePhoneBookStore::Read completed with incorrect number"));
	ASSERT_EQUALS_DES16(names, readName,
		_L("RMobilePhoneBookStore::Read completed with incorrect name"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 pbBuffer	
    //3 notifyStoreEventStatus
	//4 phoneBookWriteStatus
	//5 phoneBookReadStatus    
	CleanupStack::PopAndDestroy(5,&phoneBookStoreInfoStatus);
		
	}



CCTSYIntegrationTestPhoneBookStore0012Base::CCTSYIntegrationTestPhoneBookStore0012Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0012Base::~CCTSYIntegrationTestPhoneBookStore0012Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0012Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Write an entry to phonebook supplying an invalid index.
 * TestExpectedResults: Pass - KErrArgument returned when writing to an invalid index.
 * Reason for test: Verify error is returned on attempt to write to an invalid index and number of used entries in the phone book is unchanged.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure phone book is not full. 
	if (phoneBookInfo.iUsedEntries == phoneBookInfo.iTotalEntries)    
		{
		// delete some entries
		TInt index = 1;
		
		TExtEtelRequestStatus deleteStoreStatus(phoneBookStore,EMobilePhoneStoreDelete);
		CleanupStack::PushL(deleteStoreStatus);
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		index = 2;
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));			 
		CleanupStack::PopAndDestroy(&deleteStoreStatus);
		}
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	CHECK_TRUE_L(phoneBookInfo.iUsedEntries < phoneBookInfo.iTotalEntries,
			_L("RMobilePhoneBookStore::GetInfo phonebook is full"));
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
						|	RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
				_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	TBuf<KBufSize> names(KAndersText);
	TBuf<KBufSize> numbers(KAndersPhoneNum);
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	

	// Repeat steps 1 to 3 for aEntry.index = -2, -10 
	const TInt indexCount = 3;
	TFixedArray<TInt,indexCount> indexList;
	indexList[0] = 0;
	indexList[1] = -2;
	indexList[2] = -10;
	
	for ( TInt i = 0; i < indexCount; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		
		// ===  1. Find out how many entries are used in the phone book ===
		// ===  Find out how many entries are used in the phone book ===

		// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
		// Store iUsedEntries returned for future use 
		TInt mobileGetInfoUsedEntries = 0;

		// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
		// Store iUsedEntries returned for future use 
		TInt mobileGetPhoneStoreUsedEntries = 0;
		
		RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
		ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
				phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
				_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	
		
		// ===  2. Attempt to write an entry to invalid index ===
	
		// Write an entry using RMobilePhoneBookStore::Write 
		TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
				, names);
		ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));
		
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, indexList[i]);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
	
		// Check KErrArgument is returned
		ASSERT_EQUALS(phoneBookWriteStatus.Int(), KErrArgument, 
				_L("RMobilePhoneBookStore::Write returned with an error"));
		
		// ===  3. Get the number of used entries and check it is same as before the attempted write ===
		// Check RMobilePhoneStore::GetInfo returns same iUsedEntries as before
		// Check RMobilePhone::GetPhoneStoreInfo returns same iUsedEntries as before
		ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
				phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,  mobileGetPhoneStoreUsedEntries),
				KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

		//1 phoneBookWriteStatus
		CleanupStack::PopAndDestroy(&phoneBookWriteStatus);
		}


	// ===  Attempt to write an entry to an index that is higher than the max slot number ===

	// Write an entry using RMobilePhoneBookStore::Write and aEntry.index = 10000 
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	TInt index = 10000;
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));

	// Check error returned is not KErrNone
	ASSERT_TRUE(phoneBookWriteStatus.Int() != KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));	

	
	//
	// TEST END
	//

    StartCleanup();
	// 1 phoneBookStoreInfoStatus
    // 2 pbBuffer
    // 3 phoneBookWriteStatus
	CleanupStack::PopAndDestroy(3,&phoneBookStoreInfoStatus);	
	
	}



CCTSYIntegrationTestPhoneBookStore0013Base::CCTSYIntegrationTestPhoneBookStore0013Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	}

CCTSYIntegrationTestPhoneBookStore0013Base::~CCTSYIntegrationTestPhoneBookStore0013Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0013Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Write an entry which makes the phonebook full.
 * TestExpectedResults: Pass - Entry written. RMobilePhoneStore::KStoreFull event occurs.
 * Reason for test: Verify entry written and store event completes indicating store is now full.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess | KCapsNotifyEvent 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
						|	RMobilePhoneStore::KCapsReadAccess
						| 	RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
	
	//delete all entries before filling phone
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
			, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
		_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	CHECK_TRUE_L((deleteStoreStatus.Int()==KErrNone 
		|| deleteStoreStatus.Int() == KErrNotFound),
		_L("RMobilePhoneBookStore::DeleteAll returned with an error."));
	
	// Fill phone book until there is only one slot free. 
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	
	const TUint32 count = phoneBookInfo.iTotalEntries - 1;
	_LIT(KTel,"123456%03d");
	_LIT(KText,"TelNo %03d");	
	TBuf<KBufSize> name;
	TBuf<KBufSize> number;
	
	for(TInt i = 1; i<= count; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		number.Format(KTel,i);
		name.Format(KText,i);
		TBool ret = EncodeTLVFormat(pbBuffer, number, (TUint8)RMobilePhone::EUnknownNumber
				, name);
		CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, i);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
		pbData.FillZ();
		pbData.Zero();
		number.FillZ();
		number.Zero();
		name.FillZ();
		name.Zero();		
		CleanupStack::PopAndDestroy(&phoneBookWriteStatus);		
		}
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===
	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	// Store iUsedEntries returned for future use 
	TInt mobileGetInfoUsedEntries = 0;

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
	// Store iUsedEntries returned for future use 
	TInt mobileGetPhoneStoreUsedEntries = 0;
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	
	
	// ===  Write an entry into the last free slot in the phone book ===

	// post notification using RMobilePhoneBookStore::NotifyStoreEvent for RMobilePhoneBookStore::Write	
	TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 event=0;
	TInt notifyIndex=0;	
	phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);
	
	// Write an entry using RMobilePhoneBookStore::Write and aEntry.index = -1 
	TBuf<KBufSize> names1(KAndersText);
	TBuf<KBufSize> numbers1(KAndersPhoneNum);	
	TInt index = -1;	
	TBool ret = EncodeTLVFormat(pbBuffer, numbers1, (TUint8)RMobilePhone::EUnknownNumber
			, names1);
	ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Write returned with an error"));
	
	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreFull | KStoreEntryAdded
	TUint32 expectedEvent = RMobilePhoneStore::KStoreFull | RMobilePhoneStore::KStoreEntryAdded;
	iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,expectedEvent,notifyIndex,KErrNone);

	// ===  Get the number of used entries and check it is 1 higher than before writing the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 higher than that stored.
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 higher than that stored.	
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries+1,  mobileGetPhoneStoreUsedEntries+1),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// ===  Read the written entry ===

	// Read the written entry with RMobilePhoneBookStore::Read 
	pbData.FillZ();
	pbData.Zero();		
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
		, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);	
	phoneBookStore.Read(phoneBookReadStatus, index, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
		, ETimeMedium), KErrNone,
		_L("RMobilePhoneBookStore::Read timed-out"));
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Read returned with an error"));
	
	// Check entry details are correct
	TBuf<KBufSize> readNumber;
	TBuf<KBufSize> readName;		
	TInt bufLen = pbData.Length(); 
	
	// decode PB data received in TLV format		
	ret = DecodeTLVFormat(pbBuffer, bufLen, readNumber, readName, 1);
	ASSERT_TRUE(ret, _L("DecodeTLVFormat failed"));
	ASSERT_EQUALS_DES16(numbers1, readNumber,
		_L("RMobilePhoneBookStore::Read completed with incorrect number"));
	ASSERT_EQUALS_DES16(names1, readName,
		_L("RMobilePhoneBookStore::Read completed with incorrect name"));

	
	//
	// TEST END
	//

    StartCleanup();
	
	// 1 phoneBookStoreInfoStatus
    // 2 deleteStoreStatus
    // 3 pbBuffer
    // 4 notifyStoreEventStatus
    // 5 phoneBookWriteStatus
    // 6 phoneBookReadStatus
    CleanupStack::PopAndDestroy(6, &phoneBookStoreInfoStatus);
	
	}



CCTSYIntegrationTestPhoneBookStore0014Base::CCTSYIntegrationTestPhoneBookStore0014Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0014Base::~CCTSYIntegrationTestPhoneBookStore0014Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0014Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Write an entry to phonebook when SIM is full.
 * TestExpectedResults: Pass - Error is returned on attempt to write.
 * Reason for test: Verify error is returned and number of used entries is unchanged.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	//delete all entries before filling phone
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
			, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
		_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	CHECK_TRUE_L((deleteStoreStatus.Int()==KErrNone 
		|| deleteStoreStatus.Int() == KErrNotFound),
		_L("RMobilePhoneBookStore::DeleteAll returned with an error."));

	// Fill SIM phone book. 
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	
	const TUint32 maxPhonebookEntries  = phoneBookInfo.iTotalEntries;
	
	_LIT(KTel,"123456%03d");
	_LIT(KText,"TelNo %03d");	
	TBuf<KBufSize> names;
	TBuf<KBufSize> numbers;
	
	for(TInt i = 1; i<= maxPhonebookEntries ; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		numbers.Format(KTel,i);
		names.Format(KText,i);
		TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
				, names);
		CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, i);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
		pbData.FillZ();
		pbData.Zero();
		CleanupStack::PopAndDestroy(&phoneBookWriteStatus);		
		}

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===
	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	// Store iUsedEntries returned for future use 
	TInt mobileGetInfoUsedEntries = 0;

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
	// Store iUsedEntries returned for future use 
	TInt mobileGetPhoneStoreUsedEntries = 0;
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	
	
	// ===  Attempt to write an entry to the phone book ===

	// Write an entry using RMobilePhoneBookStore::Write and aEntry.index = -1 
	TInt index = -1;
	TBuf<KBufSize> names1(KAndersText);
	TBuf<KBufSize> numbers1(KAndersPhoneNum);
	
	TBool ret = EncodeTLVFormat(pbBuffer, numbers1, (TUint8)RMobilePhone::EUnknownNumber
			, names1);
	ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));
	
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	
	// Check error != KErrNone is returned.
	ASSERT_TRUE(phoneBookWriteStatus.Int() != KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
	
	// ===  Get the number of used entries and check it is the same as before attempted write ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is same as before
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is same as before
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,  mobileGetPhoneStoreUsedEntries),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	//
	// TEST END
	//

    StartCleanup();
	
	// 1 phoneBookStoreInfoStatus
    // 2 deleteStoreStatus
    // 3 pbBuffer
    // 4 phoneBookWriteStatus
    CleanupStack::PopAndDestroy(4, &phoneBookStoreInfoStatus);
	
	
	}



CCTSYIntegrationTestPhoneBookStore0015Base::CCTSYIntegrationTestPhoneBookStore0015Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0015Base::~CCTSYIntegrationTestPhoneBookStore0015Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0015Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Edit an entry in the SIM phone book.
 * TestExpectedResults: Pass - Phone book entry modified and KStoreEntryChanged event completed.
 * Reason for test: Verify new entry information is written to the phone book and correct store event completes.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsReadAccess | KCapsNotifyEvent 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
						|  RMobilePhoneStore::KCapsReadAccess
						|  RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
						_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
	
	// delete entry before writing to known slot	
	TInt index = 5;
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.Delete(deleteStoreStatus,index);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
			_L("RMobilePhoneBookStore::Delete timed-out"));
	CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
					|| deleteStoreStatus.Int() == KErrNone ),		
				 _L("RMobilePhoneBookStore::Delete returned with an error."));
	
	// Write an entry to a known slot in the phone book. 
	TBuf<KBufSize> names(KAndersText);
	TBuf<KBufSize> numbers(KAndersPhoneNum);
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	

	TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Write returned with an error"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===
	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	// Store iUsedEntries returned for future use 
	TInt mobileGetInfoUsedEntries = 0;

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
	// Store iUsedEntries returned for future use 
	TInt mobileGetPhoneStoreUsedEntries = 0;
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	

	// ===  Write an entry to a slot in the phone book which already contains data ===

	// post notification using RMobilePhoneBookStore::NotifyStoreEvent for RMobilePhoneBookStore::Write		
	TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 event=0;
	TInt notifyIndex=0;	
	phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);
	
	// Write an entry with a different name and number to specific slot 
	names.Copy(KAnnText);
	numbers.Copy(KAnnPhoneNum);
	pbData.FillZ();
	pbData.Zero();
	ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeLong), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	ASSERT_EQUALS(phoneBookWriteStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Write returned with an error"));
	
	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEntryChanged
	iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,RMobilePhoneStore::KStoreEntryChanged,notifyIndex,KErrNone);

	// ===  Get the number of used entries and check it is same as before writing the entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is same as before
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is same as before
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,  mobileGetPhoneStoreUsedEntries),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// ===  Read the written entry ===

	// Read the written entry with RMobilePhoneBookStore::Read 
	pbData.FillZ();
	pbData.Zero();		
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
		, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);	
	phoneBookStore.Read(phoneBookReadStatus, index, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
		, ETimeMedium), KErrNone,
		_L("RMobilePhoneBookStore::Read timed-out"));
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Read returned with an error"));
		
	// Check entry details are correct
	TBuf<KBufSize> readNumber;
	TBuf<KBufSize> readName;		
	TInt bufLen = pbData.Length(); 
	
	// decode PB data received in TLV format		
	ret = DecodeTLVFormat(pbBuffer, bufLen, readNumber, readName, 1);
	ASSERT_TRUE(ret, _L("DecodeTLVFormat failed"));
	ASSERT_EQUALS_DES16(numbers, readNumber,
		_L("RMobilePhoneBookStore::Read completed with incorrect number"));
	ASSERT_EQUALS_DES16(names, readName,
		_L("RMobilePhoneBookStore::Read completed with incorrect name"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 deleteStoreStatus
    //3 pbBuffer
    //4 notifyStoreEventStatus
    //5 phoneBookWriteStatus
	//6 phoneBookReadStatus
	CleanupStack::PopAndDestroy(6,&phoneBookStoreInfoStatus);
	
	}



CCTSYIntegrationTestPhoneBookStore0016Base::CCTSYIntegrationTestPhoneBookStore0016Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0016Base::~CCTSYIntegrationTestPhoneBookStore0016Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0016Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Delete an entry from the SIM phone book.
 * TestExpectedResults: Pass - Deletion is successful.
 * Reason for test: Verify deletion is successful and deleted entry cannot be read.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Ensure phone has KCapsReadAccess and KCapsWriteAccess capabilities. 
	TUint32 expectedCaps =  RMobilePhoneStore::KCapsReadAccess
							| RMobilePhoneStore::KCapsWriteAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
	
	//delete all entries before filling phonebook
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
			, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
		_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	CHECK_TRUE_L((deleteStoreStatus.Int()==KErrNone 
		|| deleteStoreStatus.Int() == KErrNotFound),
		_L("RMobilePhoneBookStore::DeleteAll returned with an error."));

	// Fill the first 250 slots in the phone book. 
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	
	const TUint32 count = phoneBookInfo.iTotalEntries;
	
	_LIT(KTel,"123456%03d");
	_LIT(KText,"TelNo %03d");	
	TBuf<KBufSize> names;
	TBuf<KBufSize> numbers;
	
	for(TInt i = 1; i<= count; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		numbers.Format(KTel,i);
		names.Format(KText,i);
		TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
				, names);
		CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, i);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
		pbData.FillZ();
		pbData.Zero();
		CleanupStack::PopAndDestroy(&phoneBookWriteStatus);		
		}

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//

	// ===  Find out how many entries are used in the phone book ===

	// Repeat whole test with aIndex = midEntry and aIndex = lastEntry 
	const TInt indexCount = 3;
	TFixedArray<TInt,indexCount> indexList;
	indexList[0] = 1;
	indexList[1] =  phoneBookInfo.iTotalEntries/2; 
	indexList[2] = phoneBookInfo.iTotalEntries;
 
	for( TInt i = 0; i < indexCount ; ++i )
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);

		// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
		// Store iUsedEntries returned for future use 
		TInt mobileGetInfoUsedEntries = 0;

		// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
		// Store iUsedEntries returned for future use 
		TInt mobileGetPhoneStoreUsedEntries = 0;
		
		RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
		ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
				phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
				_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	
		
		// ===  Delete an entry ===
		// post notification using RMobilePhoneBookStore::NotifyStoreEvent for RMobilePhoneBookStore::Delete				 
		TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
		CleanupStack::PushL(notifyStoreEventStatus);
		TUint32 event=0;
		TInt notifyIndex=0;	
		phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);
		
		// Delete entry from the phone book specifying aIndex = 1 
		TInt index =indexList[i];
		TExtEtelRequestStatus deleteStoreStatus1(phoneBookStore,EMobilePhoneStoreDelete);
		CleanupStack::PushL(deleteStoreStatus1);
		phoneBookStore.Delete(deleteStoreStatus1,index);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus1, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		ASSERT_EQUALS(deleteStoreStatus1.Int(), KErrNone,
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		
		// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEntryDeleted
		iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,RMobilePhoneStore::KStoreEntryDeleted,notifyIndex,KErrNone);	
	
		// ===  Get the number of used entries and check it is 1 lower than before deleting the new entry ===
	
		// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 lower  than that stored.
		// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 lower than that stored.
		ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
				phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries-1,  mobileGetPhoneStoreUsedEntries-1),
				KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

		// ===  Attempt to read the deleted entry.  ===
	
		// Read a single entry stored in the slot used in delete 
		pbData.FillZ();
		pbData.Zero();		
		TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
			, EMobilePhoneBookStoreRead);	
		CleanupStack::PushL(phoneBookReadStatus);	
		phoneBookStore.Read(phoneBookReadStatus, index, 1, pbData);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
		
		// Check KErrNotFound is returned.
		ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNotFound, 
			_L("RMobilePhoneBookStore::Read returned with an error"));
	
	
		// ===  Attempt to delete the already deleted entry again. ===
	
		// Delete the already deleted entry 
		phoneBookStore.Delete(deleteStoreStatus1,index);
		ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus1, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
	
		// Check KErrNotFound is returned
		ASSERT_EQUALS(deleteStoreStatus1.Int(), KErrNotFound,
					 _L("RMobilePhoneBookStore::Delete returned with an error."));

		//1 notifyStoreEventStatus
		//2 deleteStoreStatus1
		//3 phoneBookReadStatus
		
		CleanupStack::PopAndDestroy(3, &notifyStoreEventStatus);

		}

	
	//
	// TEST END
	//

    StartCleanup();
	
	//1 phoneBookStoreInfoStatus
    //2 deleteStoreStatus
    //3 pbBuffer
    CleanupStack::PopAndDestroy(3, &phoneBookStoreInfoStatus);
	
	}



CCTSYIntegrationTestPhoneBookStore0017Base::CCTSYIntegrationTestPhoneBookStore0017Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0017Base::~CCTSYIntegrationTestPhoneBookStore0017Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0017Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Delete an entry from the SIM phone book supplying an invalid index.
 * TestExpectedResults: Pass - KErrArgument returned.
 * Reason for test: Verify KErrArgument returned.
 */
	{

	//
	// SET UP
	//

	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	// Store iUsedEntries returned for future use 
	TInt mobileGetInfoUsedEntries = 0;

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
	// Store iUsedEntries returned for future use 
	TInt mobileGetPhoneStoreUsedEntries = 0;
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	
	
	// ===  Attempt to delete with invalid index ===
	
	// Delete an entry from the phone book using RMobilePhoneStore::Delete with aIndex = -1 
	TInt index = -1;
	
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.Delete(deleteStoreStatus,index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
			_L("RMobilePhoneBookStore::Delete timed-out"));

	// Check KErrArgument is returned
	ASSERT_EQUALS( deleteStoreStatus.Int(), KErrArgument,		
				 _L("RMobilePhoneBookStore::Delete returned with an error."));

	// ===  Get the number of used entries and check it is the same as before attempted delete ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is same as before
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is same as before
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,  mobileGetPhoneStoreUsedEntries),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// ===  Attempt to delete with invalid index ===

	// Delete an entry from the phone book using RMobilePhoneStore::Delete with aIndex = -12 
	index = -12;	
	phoneBookStore.Delete(deleteStoreStatus,index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
			_L("RMobilePhoneBookStore::Delete timed-out"));

	// Check KErrArgument is returned
	ASSERT_EQUALS( deleteStoreStatus.Int(), KErrArgument,		
				 _L("RMobilePhoneBookStore::Delete returned with an error."));
	

	// ===  Get the number of used entries and check it is the same as before attempted delete ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is same as before
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is same as before
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
			phoneBookInfoPckg,mobilePhone, mobileGetInfoUsedEntries,  mobileGetPhoneStoreUsedEntries),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
	//2 deleteStoreStatus
    CleanupStack::PopAndDestroy(2,&phoneBookStoreInfoStatus);
	
	
	}



CCTSYIntegrationTestPhoneBookStore0018Base::CCTSYIntegrationTestPhoneBookStore0018Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0018Base::~CCTSYIntegrationTestPhoneBookStore0018Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0018Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Delete all entries from the SIM phone book.
 * TestExpectedResults: Pass - Deletion is successful.
 * Reason for test: Verify deletion is successful and there are no entries in the phone book after deletion.
 */
	{

	//
	// SET UP
	//

	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsDeleteAll 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsDeleteAll;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
	
	// Ensure there are at least 10 entries in the SIM phone book. 
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	
	const TUint32 count = 10;
	
	_LIT(KTel,"123456%03d");
	_LIT(KText,"TelNo %03d");	
	TFixedArray<TBuf<KBufSize>,count> names;
	TFixedArray<TBuf<KBufSize>,count> numbers;
	
	for(TInt i = 1; i<= count; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		numbers[i-1].Format(KTel,i);
		names[i-1].Format(KText,i);
		TBool ret = EncodeTLVFormat(pbBuffer, numbers[i-1], (TUint8)RMobilePhone::EUnknownNumber
				, names[i-1]);
		CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, i);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
		pbData.FillZ();
		pbData.Zero();
		CleanupStack::PopAndDestroy(&phoneBookWriteStatus);		
		}

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Delete all the entries in one go ===
	
	// post notification using RMobilePhoneBookStore::NotifyStoreEvent for RMobilePhoneBookStore::DeleteAll 
	TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 event=0;
	TInt notifyIndex=0;	
	phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);
	
	// Delete all the entries from the SIM phone book. 
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
			, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong),KErrNone, 
			_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	ASSERT_EQUALS(deleteStoreStatus.Int(), KErrNone,
				 _L("RMobilePhoneBookStore::DeleteAll returned with an error."));
	
	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEmpty
	iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,RMobilePhoneStore::KStoreEmpty,notifyIndex,KErrNone);	

	// ===  Get the number of used entries and check it is 0 ===
	// Check RMobilePhoneStore::GetInfo returns iUsedEntries == 0
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries == 0	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
			phoneBookInfoPckg,mobilePhone, 0,  0),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 pbBuffer
    //3 notifyStoreEventStatus
    //4 deleteStoreStatus
	CleanupStack::PopAndDestroy(4,&phoneBookStoreInfoStatus);
	
	}



CCTSYIntegrationTestPhoneBookStore0019Base::CCTSYIntegrationTestPhoneBookStore0019Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0019Base::~CCTSYIntegrationTestPhoneBookStore0019Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0019Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Fill and delete all entries in the phone book.
 * TestExpectedResults: Pass - All entries deleted successfully.
 * Reason for test: Verify entries can be written to the phone book until it is full and then all entries can be deleted from a full phone book.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsDeleteAll 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
							| RMobilePhoneStore::KCapsDeleteAll;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
	
	// Ensure phone book is empty
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
			, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
		_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	CHECK_TRUE_L((deleteStoreStatus.Int()==KErrNone 
		|| deleteStoreStatus.Int() == KErrNotFound),
		_L("RMobilePhoneBookStore::DeleteAll returned with an error."));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Write entries to the phone book until it is full 
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	
	const TUint32 maxPhonebookEntries  = phoneBookInfo.iTotalEntries;
	
	_LIT(KTel,"123456%03d");
	_LIT(KText,"TelNo %03d");	
	TBuf<KBufSize> names;
	TBuf<KBufSize> numbers;
	
	for(TInt i = 1; i<= maxPhonebookEntries ; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		// post notification for RMobilePhoneBookStore::Write	
		TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
		CleanupStack::PushL(notifyStoreEventStatus);
		TUint32 event=0;
		TInt notifyIndex=0;	
		phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);
		
		numbers.Format(KTel,i);
		names.Format(KText,i);
		TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
				, names);
		ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, i);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));

		// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEntryAdded with each write
		iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,RMobilePhoneStore::KStoreEntryAdded,notifyIndex,KErrNone);

		pbData.FillZ();
		pbData.Zero();
		// 1 notifyStoreEventStatus
		// 2 phoneBookWriteStatus
		CleanupStack::PopAndDestroy(2,&notifyStoreEventStatus);		
		}

	// ===  Check that the phone book is full ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	ASSERT_EQUALS(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Check that iTotalEntries == iUsedEntries
	ASSERT_EQUALS(phoneBookInfo.iUsedEntries, phoneBookInfo.iTotalEntries, 
			_L("RMobilePhoneBookStore::GetPhoneStoreInfo return incorrect information"));

	// ===  Delete all entries from the phone book ===
	// post notification using RMobilePhoneBookStore::NotifyStoreEvent for RMobilePhoneBookStore::DeleteAll	
	TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 event=0;
	TInt notifyIndex=0;	
	phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);

	// Delete all entries from the full phone book 
	phoneBookStore.DeleteAll(deleteStoreStatus);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
		_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	ASSERT_EQUALS(deleteStoreStatus.Int(),KErrNone ,
		_L("RMobilePhoneBookStore::DeleteAll returned with an error."));
	
	// Check RMobilePhoneStore::NotifyStoreEvent complete with KStoreEmpty
	iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,RMobilePhoneStore::KStoreEmpty,notifyIndex,KErrNone);

	// ===  Check phone book is empty ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	// Check iUsedEntries == 0 
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	// Check iUsedEntries == 0
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
			phoneBookInfoPckg,mobilePhone, 0,  0),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));
	
	//
	// TEST END
	//

    StartCleanup();
    
    //1 phoneBookStoreInfoStatus
    //2 deleteStoreStatus
	//3 pbBuffer
	//4 notifyStoreEventStatus
	CleanupStack::PopAndDestroy(4,&phoneBookStoreInfoStatus);
	
	}



CCTSYIntegrationTestPhoneBookStore0020Base::CCTSYIntegrationTestPhoneBookStore0020Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0020Base::~CCTSYIntegrationTestPhoneBookStore0020Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0020Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Cancel deletion of all entries from the SIM phone book.
 * TestExpectedResults: Pass - Deletion is cancelled.
 * Reason for test: Verify some entries have been deleted before request is cancelled.
 */
	{
	
	//
	// SET UP
	//

	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsDeleteAll 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsDeleteAll;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
	
	// Ensure there are 100 entries in the SIM phone book. 
	TBuf8<KPBDataClientBuf100> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	
	TUint32 count = phoneBookInfo.iTotalEntries/2;

	_LIT(KTel,"123456%03d");
	_LIT(KText,"TelNo %03d");	
	TBuf<KBufSize> names;
	TBuf<KBufSize> numbers;
	
	for(TInt i = 1; i<= count; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		numbers.Format(KTel,i);
		names.Format(KText,i);
		TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
				, names);
		CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, i);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
		pbData.FillZ();
		pbData.Zero();
		CleanupStack::PopAndDestroy(&phoneBookWriteStatus);		
		}

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	// Store iUsedEntries returned for future use 
	TInt mobileGetInfoUsedEntries = 0;

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
	// Store iUsedEntries returned for future use 
	TInt mobileGetPhoneStoreUsedEntries = 0;
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	
	
	// Delete all the entries from the SIM phone book. 
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
			, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);	

	// Cancel deletion after 1s. 
	User::After(KOneSecond);
	phoneBookStore.CancelAsyncRequest(EMobilePhoneStoreDeleteAll);
	
	// Check TRequestStatus used for deletion is KErrCancel | KErrNone
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
			_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	ASSERT_TRUE( ( (deleteStoreStatus.Int() == KErrCancel) ||
					(deleteStoreStatus.Int() == KErrNone)),
			_L("RMobilePhoneBookStore::DeleteAll returned with an error."));

	// ===  Check that the phone book is full ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	ASSERT_EQUALS(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Check that iUsedEntries has decreased but is not 0
	// or condition for KErrNone case, since cancel is not supported
	ASSERT_TRUE( ( ( phoneBookInfo.iUsedEntries < mobileGetInfoUsedEntries )
			|| ( phoneBookInfo.iUsedEntries != 0 ) ),
			 _L("RMobilePhoneBookStore::GetInfo returned incorrect information."))	
	
	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo 
	TExtEtelRequestStatus phoneStoreInfoStatus(mobilePhone, EMobilePhoneGetPhoneStoreInfo);	
	CleanupStack::PushL(phoneStoreInfoStatus);
	RMobilePhoneStore::TMobilePhoneStoreInfoV1 phoneStoreInfo;
	RMobilePhoneStore::TMobilePhoneStoreInfoV1Pckg phoneStoreInfoPckg(phoneStoreInfo);
	/* mobilePhone.GetPhoneStoreInfo(phoneStoreInfoStatus,phoneStoreInfoPckg,aStoreName);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneStoreInfoStatus, ETimeShort), 
			KErrNone, _L("RMobilePhone::GetPhoneStoreInfo timed-out"));
	ASSERT_EQUALS(phoneStoreInfoStatus.Int(), 
			KErrNone, _L("RMobilePhone::GetPhoneStoreInfo returned with an error"));

	// Check that iUsedEntries has decreased but is not 0
	ASSERT_TRUE( ( (phoneStoreInfo.iUsedEntries < mobilePhoneStoreUsedEntries)
			&& (phoneStoreInfo.iUsedEntries != 0) ), _L("RMobilePhoneBookStore::GetPhoneStoreInfo return incorrect information"))	*/

	
	//
	// TEST END
	//

    StartCleanup();
	//1 phoneBookStoreInfoStatus
	//2 pbBuffer
	//3 deleteStoreStatus
	//4 phoneStoreInfoStatus
	CleanupStack::PopAndDestroy(4,&phoneBookStoreInfoStatus);
	
	}



CCTSYIntegrationTestPhoneBookStore0021Base::CCTSYIntegrationTestPhoneBookStore0021Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0021Base::~CCTSYIntegrationTestPhoneBookStore0021Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0021Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestCaseDesc: Delete an entry from a full phone book.
 * TestExpectedResults: Pass - RMobilePhoneStore::KStoreHasSpace completed.
 * Reason for test: Verify entry deleted successfully and phone book event completes.
 */
	{

	//
	// SET UP
	//

	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));

	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsNotifyEvent 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
				| 	RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
	
	//delete all entries before filling phonebook
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
			, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
		_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	CHECK_TRUE_L((deleteStoreStatus.Int()==KErrNone 
		|| deleteStoreStatus.Int() == KErrNotFound),
		_L("RMobilePhoneBookStore::DeleteAll returned with an error."));
	
	// Ensure phone book is full 
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	
	const TUint32 maxPhonebookEntries  = phoneBookInfo.iTotalEntries;
	
	_LIT(KTel,"123456%03d");
	_LIT(KText,"TelNo %03d");	
	TBuf<KBufSize> names;
	TBuf<KBufSize> numbers;
	
	for(TInt i = 1; i<= maxPhonebookEntries ; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		numbers.Format(KTel,i);
		names.Format(KText,i);
		TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
				, names);
		CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, i);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
		pbData.FillZ();
		pbData.Zero();
		CleanupStack::PopAndDestroy(&phoneBookWriteStatus);		
		}

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===
	// Get the phone book information using RMobilePhoneStore::GetInfo 
	// Store iUsedEntries returned for future use 
	TInt mobileGetInfoUsedEntries = 0;

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
	// Store iUsedEntries returned for future use 
	TInt mobileGetPhoneStoreUsedEntries = 0;
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	

	// post notification using RMobilePhoneBookStore::NotifyStoreEvent for RMobilePhoneBookStore::Delete				 
	TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 event=0;
	TInt notifyIndex=0;	
	phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);
	
	// Delete an entry from the phone book. 
	TInt index =1;
	TExtEtelRequestStatus deleteStoreStatus1(phoneBookStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStoreStatus1);
	phoneBookStore.Delete(deleteStoreStatus1,index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus1, ETimeMedium),KErrNone, 
			_L("RMobilePhoneBookStore::Delete timed-out"));
	ASSERT_EQUALS(deleteStoreStatus1.Int(), KErrNone,
				 _L("RMobilePhoneBookStore::Delete returned with an error."));
	
	// Check RMobilePhoneStore::NotifyStoreEvent completes with RMobilePhoneStore::KStoreHasSpace | RMobilePhoneStore::KStoreEntryDeleted
	TUint32 expectedEvent = RMobilePhoneStore::KStoreHasSpace
		| RMobilePhoneStore::KStoreEntryDeleted;
	iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,expectedEvent,notifyIndex,KErrNone);	
	
	// ===  Get the number of used entries and check it is one lower than before ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries is one lower than before
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries is one lower than before
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
			phoneBookInfoPckg,mobilePhone, mobileGetInfoUsedEntries-1, mobileGetPhoneStoreUsedEntries-1),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 deleteStoreStatus
    //3 pbBuffer
    //4 notifyStoreEventStatus
    //5 deleteStoreStatus1
    CleanupStack::Pop(5,&phoneBookStoreInfoStatus);	
	
	}



CCTSYIntegrationTestPhoneBookStore0022Base::CCTSYIntegrationTestPhoneBookStore0022Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0022Base::~CCTSYIntegrationTestPhoneBookStore0022Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0022Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId, const TDesC& aStoreName)
/**
 * TestDesc: Delete a long (>20 digit) number from the SIM phone book.
 * TestExpectedResults: Pass - Number is deleted.
 * Reason for test: Verify entry deleted successfully.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess | KCapsNotifyEvent 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess
							| RMobilePhoneStore::KCapsNotifyEvent;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));	

	// delete entry before writing to known slot	
	TInt index = 10;;	
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.Delete(deleteStoreStatus,index);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
			_L("RMobilePhoneBookStore::Delete timed-out"));
	CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
					|| deleteStoreStatus.Int() == KErrNone ),		
				 _L("RMobilePhoneBookStore::Delete returned with an error."));

	
	// Write a number longer than 20 digits to the SIM phone book. 
	TBuf<KBufSize> names(KAndersText);
	_LIT16(KLongNumber, "123456789012345678901234567890");
	TBuf<KBufSize> numbers(KLongNumber);
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	

	TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Write returned with an error"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// ===  Find out how many entries are used in the phone book ===

	// Get the phone book information using RMobilePhoneStore::GetInfo using a RMobilePhoneBookStore::TMobilePhoneBookInfoV1  
	// Store iUsedEntries returned for future use 
	TInt mobileGetInfoUsedEntries = 0;

	// Get the phone book information using RMobilePhone::GetPhoneStoreInfo
	// Store iUsedEntries returned for future use 
	TInt mobileGetPhoneStoreUsedEntries = 0;
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	ASSERT_EQUALS(iSimTsyTestHelper.GetNumberOfUsedEntriesInStoreL(phoneBookStore, aStoreName,
			phoneBookInfoPckg,mobilePhone,mobileGetInfoUsedEntries,	mobileGetPhoneStoreUsedEntries),KErrNone, 
			_L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));	
	
	// post notification using RMobilePhoneBookStore::NotifyStoreEvent for RMobilePhoneBookStore::Delete				 
	TExtEtelRequestStatus notifyStoreEventStatus(phoneBookStore,EMobilePhoneStoreNotifyStoreEvent);
	CleanupStack::PushL(notifyStoreEventStatus);
	TUint32 event=0;
	TInt notifyIndex=0;	
	phoneBookStore.NotifyStoreEvent(notifyStoreEventStatus, event, notifyIndex);
	
	// Delete the written entry. 
	TExtEtelRequestStatus deleteStoreStatus1(phoneBookStore,EMobilePhoneStoreDelete);
	CleanupStack::PushL(deleteStoreStatus1);
	phoneBookStore.Delete(deleteStoreStatus1,index);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus1, ETimeMedium),KErrNone, 
			_L("RMobilePhoneBookStore::Delete timed-out"));
	ASSERT_EQUALS(deleteStoreStatus1.Int(), KErrNone,
				 _L("RMobilePhoneBookStore::Delete returned with an error."));
	
	// Check RMobilePhoneStore::NotifyStoreEvent completes with KStoreEntryDeleted
	iSimTsyTestHelper.WaitForMobilePhoneStoreNotifyStoreEvent(phoneBookStore,notifyStoreEventStatus,event,RMobilePhoneStore::KStoreEntryDeleted,notifyIndex,KErrNone);

	// ===  Get the number of used entries and check it is 1 lower than before deleting the new entry ===

	// Check RMobilePhoneStore::GetInfo returns iUsedEntries which is 1 lower  than that stored.
	// Check RMobilePhone::GetPhoneStoreInfo returns iUsedEntries which is 1 lower than that stored.
	ASSERT_EQUALS(iSimTsyTestHelper.AssertNumberOfUsedEntriesCorrectL(phoneBookStore,aStoreName, 
			phoneBookInfoPckg,mobilePhone, mobileGetInfoUsedEntries-1, mobileGetPhoneStoreUsedEntries-1),
			KErrNone, _L("RMobilePhoneStore::GetInfo and RMobilePhone::GetPhoneStoreInfo both returned an error"));

	// ===  Attempt to read deleted entry ===

	// Attempt to read the entry in the slot which was just deleted 
	pbData.FillZ();
	pbData.Zero();		
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
		, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);	
	phoneBookStore.Read(phoneBookReadStatus, index, 1, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
		, ETimeMedium), KErrNone,
		_L("RMobilePhoneBookStore::Read timed-out"));

	// Check KErrNotFound returned
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNotFound, 
			_L("RMobilePhoneBookStore::Read returned with an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 deleteStoreStatus
    //3 pbBuffer
    //4 phoneBookWriteStatus
    //5 notifyStoreEventStatus
    //6 deleteStoreStatus1
    //7 phoneBookReadStatus    
    CleanupStack::Pop(7,&phoneBookStoreInfoStatus);	
	
	}



CCTSYIntegrationTestPhoneBookStore0023Base::CCTSYIntegrationTestPhoneBookStore0023Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0023Base::~CCTSYIntegrationTestPhoneBookStore0023Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0023Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Read a single phone book entry into a buffer that is too small.
 * TestExpectedResults: Pass - KErrGeneral returned when the descriptor is too small.
 * Reason for test: Verify KErrGeneral returned
 */
	{

	//
	// SET UP
	//

	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));	
		
	// Ensure there are entries in the phone book in known slots 
	TInt index = -1;;	
	TBuf<KBufSize> names(KAndersText);
	TBuf<KBufSize> numbers(KAndersPhoneNum);
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	

	TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Write returned with an error"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	// Attempt to read a single entry from the phone book using a 10 byte descriptor for aPBData 	
	TBuf8<10> pbData1;
	pbBuffer->Set(&pbData1);
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
			, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);
	phoneBookStore.Read(phoneBookReadStatus, index, 1, pbData1);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrGeneral is returned
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrGeneral, 
			_L("RMobilePhoneBookStore::Read returned with an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 pbBuffer
    //3 phoneBookWriteStatus
    //4 phoneBookReadStatus    
    CleanupStack::Pop(4,&phoneBookStoreInfoStatus);	
	
	
	}



CCTSYIntegrationTestPhoneBookStore0024Base::CCTSYIntegrationTestPhoneBookStore0024Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0024Base::~CCTSYIntegrationTestPhoneBookStore0024Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0024Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Read multiple phone book entries when phone book is empty.
 * TestExpectedResults: Pass - Number of entries TSY tried to read and number of entries actually read correct.
 * Reason for test: Verify KErrNotFound is returned.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10040);
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));	
	
	// Delete all entries from the phone book 
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
			, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.DeleteAll(deleteStoreStatus);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeVeryLong), KErrNone, 
		_L("RMobilePhoneBookStore::DeleteAll timed-out"));
	CHECK_TRUE_L((deleteStoreStatus.Int()==KErrNone 
		|| deleteStoreStatus.Int() == KErrNotFound),
		_L("RMobilePhoneBookStore::DeleteAll returned with an error."));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Read multiple entries from the phone book. 
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
			, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);	
	phoneBookStore.Read(phoneBookReadStatus, 1, 10, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
		// Check KErrNotFound is returned.
		ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrNotFound, 
				_L("RMobilePhoneBookStore::Read returned with an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 deleteStoreStatus
    //3 pbBuffer
    //4 phoneBookReadStatus    
    CleanupStack::Pop(4,&phoneBookStoreInfoStatus);	
	
	}



CCTSYIntegrationTestPhoneBookStore0025Base::CCTSYIntegrationTestPhoneBookStore0025Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0025Base::~CCTSYIntegrationTestPhoneBookStore0025Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0025Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Read multiple phone book entries into a buffer that is too small.
 * TestExpectedResults: Pass - KErrGeneral returned when the descriptor is too small.
 * Reason for test: Verify KErrGeneral returned
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess  
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));	
	
	// Write valid entries into the first 50 locations in the phone book. 
	TBuf8<KPBDataClientBuf50> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer
	
	TInt count = phoneBookInfo.iTotalEntries/3;
	_LIT(KTel,"123456%03d");
	_LIT(KText,"TelNo %03d");	
	TBuf<KBufSize> names;
	TBuf<KBufSize> numbers;
	
	for(TInt i = 1; i<= count; ++i)
		{
		DEBUG_PRINTF2(_L("loop iteration : %d"),i);
		numbers.Format(KTel,i);
		names.Format(KText,i);
		TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
				, names);
		CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
		TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
		CleanupStack::PushL(phoneBookWriteStatus);
		phoneBookStore.Write(phoneBookWriteStatus, pbData, i);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
				, ETimeMedium), KErrNone,
				_L("RMobilePhoneBookStore::Write timed-out"));
		CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
			_L("RMobilePhoneBookStore::Write returned with an error"));
		pbData.FillZ();
		pbData.Zero();
		CleanupStack::PopAndDestroy(&phoneBookWriteStatus);		
		}

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Read 50 entries from the phone book using aIndex = 1, aNumSlots = 50 and aPBData = a 10 byte descriptor 
	TBuf8<10> pbData1;
	pbBuffer->Set(&pbData1);
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
			, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);	
	phoneBookStore.Read(phoneBookReadStatus, 1, count, pbData);
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookReadStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Read timed-out"));
	
	// Check KErrGeneral is returned.
	ASSERT_EQUALS(phoneBookReadStatus.Int(), KErrGeneral, 
			_L("RMobilePhoneBookStore::Read returned with an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 pbBuffer
    //3 phoneBookReadStatus    
    CleanupStack::Pop(3,&phoneBookStoreInfoStatus);	
	
	}



CCTSYIntegrationTestPhoneBookStore0026Base::CCTSYIntegrationTestPhoneBookStore0026Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0026Base::~CCTSYIntegrationTestPhoneBookStore0026Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0026Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestCaseDesc: Cancel reading a single entry from the phone book.
 * TestExpectedResults: Pass - KErrCancel returned.
 * Reason for test: Verify KErrCancel returned.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsReadAccess  
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));	

	// Write an entry to a known location in the phone book
	TInt index = 10;
	TBuf<KBufSize> names(KAndersText);
	TBuf<KBufSize> numbers(KAndersPhoneNum);
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	

	TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Write returned with an error"));
	
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Read an entry that exists from the phone book 
	pbData.FillZ();
	pbData.Zero();		
	TExtEtelRequestStatus phoneBookReadStatus(phoneBookStore
		, EMobilePhoneBookStoreRead);	
	CleanupStack::PushL(phoneBookReadStatus);
	
	phoneBookStore.Read(phoneBookReadStatus, index, 1, pbData);
	
	// Cancel read entry by calling CancelAsyncRequest( EMobilePhoneBookStoreRead )  
	phoneBookStore.CancelAsyncRequest(EMobilePhoneBookStoreRead);
	
	// Check KErrCancel or KErrNone is returned
	ASSERT_TRUE(phoneBookReadStatus.Int()==KErrCancel || phoneBookReadStatus.Int()==KErrNone, 
			_L("RMobilePhoneBookStore::Read returned with an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 pbBuffer
    //3 phoneBookWriteStatus
    //4 phoneBookReadStatus    
    CleanupStack::Pop(4,&phoneBookStoreInfoStatus);	
	
	}



CCTSYIntegrationTestPhoneBookStore0027Base::CCTSYIntegrationTestPhoneBookStore0027Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0027Base::~CCTSYIntegrationTestPhoneBookStore0027Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0027Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Cancel writing an entry to the phone book.
 * TestExpectedResults: Pass - KErrCancel or KErrNone returned.
 * Reason for test: Verify KErrCancel returned.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));	

	// Ensure phone book is not full 
	if (phoneBookInfo.iUsedEntries == phoneBookInfo.iTotalEntries)    
		{
		// delete some entries
		TInt index = 1;
		
		TExtEtelRequestStatus deleteStoreStatus(phoneBookStore,EMobilePhoneStoreDelete);
		CleanupStack::PushL(deleteStoreStatus);
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		index = 2;
		phoneBookStore.Delete(deleteStoreStatus,index);
		CHECK_EQUALS_L(WaitForRequestWithTimeOut(deleteStoreStatus, ETimeMedium),KErrNone, 
				_L("RMobilePhoneBookStore::Delete timed-out"));
		CHECK_TRUE_L( ( deleteStoreStatus.Int() == KErrNotFound 
						|| deleteStoreStatus.Int() == KErrNone ),		
					 _L("RMobilePhoneBookStore::Delete returned with an error."));
		CleanupStack::PopAndDestroy(&deleteStoreStatus);
		}
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	CHECK_TRUE_L(phoneBookInfo.iUsedEntries < phoneBookInfo.iTotalEntries,
				_L("RMobilePhoneBookStore::GetInfo phonebook is full"));

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	TInt index = -1;
	// Write an entry to the phone book in the next available slot 
	TBuf<KBufSize> names(KAndersText);
	TBuf<KBufSize> numbers(KAndersPhoneNum);
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	

	TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	ASSERT_TRUE(ret,_L("EncodeTLVFormat failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);

	// Cancel write entry by calling CancelAsyncRequest( EMobilePhoneBookStoreWrite );  
	phoneBookStore.CancelAsyncRequest(EMobilePhoneBookStoreWrite);
	
	// Check KErrCancel or KErrNone returned
	ASSERT_EQUALS(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	ASSERT_TRUE( ( (phoneBookWriteStatus.Int() == KErrCancel) ||
					(phoneBookWriteStatus.Int() == KErrNone) ), 
			_L("RMobilePhoneBookStore::Write did not return KErrCancel or KErrNone"));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 pbBuffer
    //3 phoneBookWriteStatus    
    CleanupStack::Pop(3,&phoneBookStoreInfoStatus);
    
	}



CCTSYIntegrationTestPhoneBookStore0028Base::CCTSYIntegrationTestPhoneBookStore0028Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	
	}

CCTSYIntegrationTestPhoneBookStore0028Base::~CCTSYIntegrationTestPhoneBookStore0028Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0028Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Cancel deleting an entry from the phone book.
 * TestExpectedResults: Pass - KErrCancel or KErrNone returned.
 * Reason for test: Verify KErrCancel returned.
 */
	{

	//
	// SET UP
	//
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer
			, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);
	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore
		, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);	
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);	
	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookStoreInfoStatus, ETimeShort), KErrNone, 
		_L("RMobilePhoneBookStore::GetInfo timed-out"));
	CHECK_EQUALS_L(phoneBookStoreInfoStatus.Int(),  KErrNone,
		 _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	// Ensure RMobilePhoneStore::GetInfo returns iCaps in set of KCapsWriteAccess 
	TUint32 expectedCaps = RMobilePhoneStore::KCapsWriteAccess;
	CHECK_BITS_SET_L(phoneBookInfo.iCaps, expectedCaps, KNoUnwantedBits,
			_L("RMobilePhoneBookStore::GetInfo returned incorrect caps"));
	
	// Write an entry to the phone book at a known location 
	TInt index = 10;
	TBuf<KBufSize> names(KAndersText);
	TBuf<KBufSize> numbers(KAndersPhoneNum);
	TBuf8<KPBDataClientBuf10> pbData; // Client reserved space for phonebook data
	CPhoneBookBuffer* pbBuffer = new(ELeave) CPhoneBookBuffer(); // used to write phonebook data
	CleanupStack::PushL(pbBuffer);
	pbBuffer->Set(&pbData); // Set it to point to the Client buffer	

	TBool ret = EncodeTLVFormat(pbBuffer, numbers, (TUint8)RMobilePhone::EUnknownNumber
			, names);
	CHECK_TRUE_L(ret,_L("EncodeTLVFormat failed"));
	TExtEtelRequestStatus phoneBookWriteStatus(phoneBookStore, EMobilePhoneBookStoreWrite);
	CleanupStack::PushL(phoneBookWriteStatus);
	phoneBookStore.Write(phoneBookWriteStatus, pbData, index);

	CHECK_EQUALS_L(WaitForRequestWithTimeOut(phoneBookWriteStatus
			, ETimeLong), KErrNone,
			_L("RMobilePhoneBookStore::Write timed-out"));
	CHECK_EQUALS_L(phoneBookWriteStatus.Int(), KErrNone, 
		_L("RMobilePhoneBookStore::Write returned with an error"));
		
	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Delete an entry that exists 
	TExtEtelRequestStatus deleteStoreStatus(phoneBookStore
			, EMobilePhoneStoreDeleteAll);
	CleanupStack::PushL(deleteStoreStatus);
	phoneBookStore.Delete(deleteStoreStatus,index);	

	// Cancel delete entry by calling CancelAsyncRequest( EMobilePhoneStoreDelete ) 
	phoneBookStore.CancelAsyncRequest(EMobilePhoneStoreDelete);

	// Check KErrCancel or KErrNone is returned
	ASSERT_EQUALS(WaitForRequestWithTimeOut(deleteStoreStatus
			, ETimeMedium), KErrNone,
			_L("RMobilePhoneBookStore::Delete timed-out"));
	ASSERT_TRUE( (deleteStoreStatus.Int() == KErrNone) || 
				(deleteStoreStatus.Int() == KErrCancel),
			 _L("RMobilePhoneBookStore::Delete returned with an error."));
	
	//
	// TEST END
	//

    StartCleanup();
	
    //1 phoneBookStoreInfoStatus
    //2 pbBuffer
    //3 phoneBookWriteStatus    
    //4 deleteStoreStatus
    CleanupStack::Pop(4,&phoneBookStoreInfoStatus);
	
	
	}


CCTSYIntegrationTestPhoneBookStore0029Base::CCTSYIntegrationTestPhoneBookStore0029Base(CEtelSessionMgr& aEtelSessionMgr)
	: CCTSYIntegrationTestPhoneBookStoreBase(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	}

CCTSYIntegrationTestPhoneBookStore0029Base::~CCTSYIntegrationTestPhoneBookStore0029Base()
/**
 * Destructor.
 */
	{
	}

void CCTSYIntegrationTestPhoneBookStore0029Base::DoTestProcedureL(TPhoneBookStoreId aPhoneBookId)
/**
 * TestDesc: Cancel getting phone store information.
 * TestExpectedResults: Pass - KErrCancel or KErrNone returned.
 * Reason for test: Verify KErrCancel returned.
 */
	{

	//
	// SET UP
	//

	//
	// SET UP END
	//
	
	StartTest();
	
	//
	// TEST START
	//
	
	
	// Get the phone store information using RMobilePhoneStore::GetInfo 
	RMobilePhoneBookStore& phoneBookStore = iEtelSessionMgr.GetPhoneBookStoreL(KMainServer, KMainPhone, aPhoneBookId);
	RMmCustomAPI& mmCustomAPI = iEtelSessionMgr.GetCustomApiL(KMainServer, KMainPhone,KMainCustomApi);
	iPhoneTsyTestHelper.WaitForPhoneBookStoreCacheReadyL(mmCustomAPI,aPhoneBookId);

	TExtEtelRequestStatus phoneBookStoreInfoStatus(phoneBookStore, EMobilePhoneStoreGetInfo);
	CleanupStack::PushL(phoneBookStoreInfoStatus);
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1 phoneBookInfo;
	RMobilePhoneBookStore::TMobilePhoneBookInfoV1Pckg phoneBookInfoPckg(phoneBookInfo);
	phoneBookStore.GetInfo(phoneBookStoreInfoStatus, phoneBookInfoPckg);
	
	// Cancel by calling CancelAsyncRequest( EMobilePhoneStoreGetInfo ) 
	phoneBookStore.CancelAsyncRequest(EMobilePhoneStoreGetInfo);
	
	// Check request is cancelled with KErrCancel | KErrNone
	ASSERT_TRUE(( phoneBookStoreInfoStatus.Int() == KErrCancel
			|| phoneBookStoreInfoStatus.Int() == KErrNone), _L("RMobilePhoneBookStore::GetInfo returned with an error"));
	
	//
	// TEST END
	//

    StartCleanup();
	
	CleanupStack::Pop(&phoneBookStoreInfoStatus);
	
	
	}

//
// Definitions of actual test cases for each phone book are below.
//


CCTSYIntegrationTestPhoneBookStore0001ADN::CCTSYIntegrationTestPhoneBookStore0001ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0001Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0001ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0001ADN::~CCTSYIntegrationTestPhoneBookStore0001ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0001ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0001
 * @SYMFssID BA/CTSY/PBSTR-ADN-0001
 * @SYMTestCaseDesc Get phone store information. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - RMobilePhoneBookStore::TMobilePhoneBookInfoV1 returned with correct information.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone store information returned is correct.
 *
 * @return - TVerdict code
 */
	{
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess 
							| RMobilePhoneStore::KCapsWriteAccess 
							| RMobilePhoneStore::KCapsDeleteAll 
							| RMobilePhoneStore::KCapsNotifyEvent;		
	DoTestProcedureL(KIccAdnPhoneBook,RMobilePhoneBookStore::ELocationIccMemory, expectedCaps);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0001ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0001ADN");
	}



CCTSYIntegrationTestPhoneBookStore0001FDN::CCTSYIntegrationTestPhoneBookStore0001FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0001Base(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneBookStore0001FDN::GetTestStepName());
	}

CCTSYIntegrationTestPhoneBookStore0001FDN::~CCTSYIntegrationTestPhoneBookStore0001FDN()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneBookStore0001FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0001
 * @SYMFssID BA/CTSY/PBSTR-FDN-0001
 * @SYMTestCaseDesc Get phone store information. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - RMobilePhoneBookStore::TMobilePhoneBookInfoV1 returned with correct information.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone store information returned is correct.
 *
 * @return - TVerdict code
 */
	{
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess 
							| RMobilePhoneStore::KCapsWriteAccess 
							| RMobilePhoneStore::KCapsDeleteAll 
							| RMobilePhoneStore::KCapsNotifyEvent 
							| RMobilePhoneBookStore::KCapsRestrictedWriteAccess;		
	DoTestProcedureL(KIccFdnPhoneBook,RMobilePhoneBookStore::ELocationIccMemory, expectedCaps);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0001FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0001FDN");
	}

CCTSYIntegrationTestPhoneBookStore0001BDN::CCTSYIntegrationTestPhoneBookStore0001BDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0001Base(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneBookStore0001BDN::GetTestStepName());
	}

CCTSYIntegrationTestPhoneBookStore0001BDN::~CCTSYIntegrationTestPhoneBookStore0001BDN()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneBookStore0001BDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-BDN-0001
 * @SYMFssID BA/CTSY/PBSTR-BDN-0001
 * @SYMTestCaseDesc Get phone store information. (BDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - RMobilePhoneBookStore::TMobilePhoneBookInfoV1 returned with correct information.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone store information returned is correct.
 *
 * @return - TVerdict code
 */
	{
	// test causing the board to reboot
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10007);
	TUint32 expectedCaps = 0;		
	User::Leave(KErrGeneral); // Test leaving to prevent crashing the hardware. Remove this line when defect is fixed.
	DoTestProcedureL(KIccBdnPhoneBook,RMobilePhoneBookStore::ELocationUnknown, expectedCaps);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0001BDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0001BDN");
	}


CCTSYIntegrationTestPhoneBookStore0001SDN::CCTSYIntegrationTestPhoneBookStore0001SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0001Base(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneBookStore0001SDN::GetTestStepName());
	}

CCTSYIntegrationTestPhoneBookStore0001SDN::~CCTSYIntegrationTestPhoneBookStore0001SDN()
/**
 * Destructor.
 */
	{
	}

TVerdict CCTSYIntegrationTestPhoneBookStore0001SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0001
 * @SYMFssID BA/CTSY/PBSTR-SDN-0001
 * @SYMTestCaseDesc Get phone store information. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - RMobilePhoneBookStore::TMobilePhoneBookInfoV1 returned with correct information.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone store information returned is correct.
 *
 * @return - TVerdict code
 */
	{
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess;
	DoTestProcedureL(KIccSdnPhoneBook,RMobilePhoneBookStore::ELocationIccMemory, expectedCaps);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0001SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0001SDN");
	}



CCTSYIntegrationTestPhoneBookStore0001VMBX::CCTSYIntegrationTestPhoneBookStore0001VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0001Base(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneBookStore0001VMBX::GetTestStepName());
	}

CCTSYIntegrationTestPhoneBookStore0001VMBX::~CCTSYIntegrationTestPhoneBookStore0001VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0001VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0001
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0001
 * @SYMTestCaseDesc Get phone store information. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - RMobilePhoneBookStore::TMobilePhoneBookInfoV1 returned with correct information.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone store information returned is correct.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess 
							| RMobilePhoneStore::KCapsWriteAccess 
							| RMobilePhoneStore::KCapsDeleteAll 
							| RMobilePhoneStore::KCapsNotifyEvent;		
	DoTestProcedureL(KIccVoiceMailBox,RMobilePhoneBookStore::ELocationIccMemory, expectedCaps);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0001VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0001VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0001MBDN::CCTSYIntegrationTestPhoneBookStore0001MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0001Base(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneBookStore0001MBDN::GetTestStepName());
	}

CCTSYIntegrationTestPhoneBookStore0001MBDN::~CCTSYIntegrationTestPhoneBookStore0001MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0001MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0001
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0001
 * @SYMTestCaseDesc Get phone store information. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions 
 * @SYMTestExpectedResults Pass - RMobilePhoneBookStore::TMobilePhoneBookInfoV1 returned with correct information.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify phone store information returned is correct.
 *
 * @return - TVerdict code
 */
	{
    ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10040);
	
	TUint32 expectedCaps = RMobilePhoneStore::KCapsReadAccess 
							| RMobilePhoneStore::KCapsWriteAccess;		
	DoTestProcedureL(KIccMbdnPhoneBook,RMobilePhoneBookStore::ELocationIccMemory, expectedCaps);

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0001MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0001MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0002ADN::CCTSYIntegrationTestPhoneBookStore0002ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0002Base(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneBookStore0002ADN::GetTestStepName());
	}

CCTSYIntegrationTestPhoneBookStore0002ADN::~CCTSYIntegrationTestPhoneBookStore0002ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0002ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0002
 * @SYMFssID BA/CTSY/PBSTR-ADN-0002
 * @SYMTestCaseDesc Read single and multiple phonebook entries. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Single and multiple entries read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify single and multiple entries can be read and entry information is correct.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0002ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0002ADN");
	}



CCTSYIntegrationTestPhoneBookStore0002FDN::CCTSYIntegrationTestPhoneBookStore0002FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0002Base(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneBookStore0002FDN::GetTestStepName());
	}

CCTSYIntegrationTestPhoneBookStore0002FDN::~CCTSYIntegrationTestPhoneBookStore0002FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0002FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0002
 * @SYMFssID BA/CTSY/PBSTR-FDN-0002
 * @SYMTestCaseDesc Read single and multiple phonebook entries. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Single and multiple entries read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify single and multiple entries can be read and entry information is correct.
 *
 * @return - TVerdict code
 */
	{
	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0002FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0002FDN");
	}



CCTSYIntegrationTestPhoneBookStore0002SDN::CCTSYIntegrationTestPhoneBookStore0002SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0002Base(aEtelSessionMgr)
/**
 * Constructor.
 */
	{
	SetTestStepName(CCTSYIntegrationTestPhoneBookStore0002SDN::GetTestStepName());
	}

CCTSYIntegrationTestPhoneBookStore0002SDN::~CCTSYIntegrationTestPhoneBookStore0002SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0002SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0002
 * @SYMFssID BA/CTSY/PBSTR-SDN-0002
 * @SYMTestCaseDesc Read single and multiple phonebook entries. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Single and multiple entries read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify single and multiple entries can be read and entry information is correct.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0002SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0002SDN");
	}



CCTSYIntegrationTestPhoneBookStore0002VMBX::CCTSYIntegrationTestPhoneBookStore0002VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0002Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0002VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0002VMBX::~CCTSYIntegrationTestPhoneBookStore0002VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0002VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0002
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0002
 * @SYMTestCaseDesc Read single and multiple phonebook entries. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Single and multiple entries read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify single and multiple entries can be read and entry information is correct.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0002VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0002VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0002MBDN::CCTSYIntegrationTestPhoneBookStore0002MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0002Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0002MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0002MBDN::~CCTSYIntegrationTestPhoneBookStore0002MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0002MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0002
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0002
 * @SYMTestCaseDesc Read single and multiple phonebook entries. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Single and multiple entries read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify single and multiple entries can be read and entry information is correct.
 *
 * @return - TVerdict code
 */
	{

	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0002MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0002MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0003ADN::CCTSYIntegrationTestPhoneBookStore0003ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0003Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0003ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0003ADN::~CCTSYIntegrationTestPhoneBookStore0003ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0003ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0003
 * @SYMFssID BA/CTSY/PBSTR-ADN-0003
 * @SYMTestCaseDesc Read single and multiple phonebook entries which are not present. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrNotFound returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0003ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0003ADN");
	}



CCTSYIntegrationTestPhoneBookStore0003FDN::CCTSYIntegrationTestPhoneBookStore0003FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0003Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0003FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0003FDN::~CCTSYIntegrationTestPhoneBookStore0003FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0003FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0003
 * @SYMFssID BA/CTSY/PBSTR-FDN-0003
 * @SYMTestCaseDesc Read single and multiple phonebook entries which are not present. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrNotFound returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0003FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0003FDN");
	}



CCTSYIntegrationTestPhoneBookStore0003SDN::CCTSYIntegrationTestPhoneBookStore0003SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0003Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0003SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0003SDN::~CCTSYIntegrationTestPhoneBookStore0003SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0003SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0003
 * @SYMFssID BA/CTSY/PBSTR-SDN-0003
 * @SYMTestCaseDesc Read single and multiple phonebook entries which are not present. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrNotFound returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{

	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0003SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0003SDN");
	}



CCTSYIntegrationTestPhoneBookStore0003VMBX::CCTSYIntegrationTestPhoneBookStore0003VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0003Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0003VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0003VMBX::~CCTSYIntegrationTestPhoneBookStore0003VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0003VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0003
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0003
 * @SYMTestCaseDesc Read single and multiple phonebook entries which are not present. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrNotFound returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0003VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0003VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0003MBDN::CCTSYIntegrationTestPhoneBookStore0003MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0003Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0003MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0003MBDN::~CCTSYIntegrationTestPhoneBookStore0003MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0003MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0003
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0003
 * @SYMTestCaseDesc Read single and multiple phonebook entries which are not present. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrNotFound returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{

	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0003MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0003MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0004ADN::CCTSYIntegrationTestPhoneBookStore0004ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0004Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0004ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0004ADN::~CCTSYIntegrationTestPhoneBookStore0004ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0004ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0004
 * @SYMFssID BA/CTSY/PBSTR-ADN-0004
 * @SYMTestCaseDesc Read phonebook entry using invalid index. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0004ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0004ADN");
	}



CCTSYIntegrationTestPhoneBookStore0004FDN::CCTSYIntegrationTestPhoneBookStore0004FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0004Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0004FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0004FDN::~CCTSYIntegrationTestPhoneBookStore0004FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0004FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0004
 * @SYMFssID BA/CTSY/PBSTR-FDN-0004
 * @SYMTestCaseDesc Read phonebook entry using invalid index. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0004FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0004FDN");
	}



CCTSYIntegrationTestPhoneBookStore0004SDN::CCTSYIntegrationTestPhoneBookStore0004SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0004Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0004SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0004SDN::~CCTSYIntegrationTestPhoneBookStore0004SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0004SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0004
 * @SYMFssID BA/CTSY/PBSTR-SDN-0004
 * @SYMTestCaseDesc Read phonebook entry using invalid index. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccSdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0004SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0004SDN");
	}



CCTSYIntegrationTestPhoneBookStore0004VMBX::CCTSYIntegrationTestPhoneBookStore0004VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0004Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0004VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0004VMBX::~CCTSYIntegrationTestPhoneBookStore0004VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0004VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0004
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0004
 * @SYMTestCaseDesc Read phonebook entry using invalid index. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0004VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0004VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0004MBDN::CCTSYIntegrationTestPhoneBookStore0004MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0004Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0004MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0004MBDN::~CCTSYIntegrationTestPhoneBookStore0004MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0004MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0004
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0004
 * @SYMTestCaseDesc Read phonebook entry using invalid index. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument returned.
 *
 * @return - TVerdict code
 */
	{

	DoTestProcedureL(KIccMbdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0004MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0004MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0005ADN::CCTSYIntegrationTestPhoneBookStore0005ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0005Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0005ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0005ADN::~CCTSYIntegrationTestPhoneBookStore0005ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0005ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0005
 * @SYMFssID BA/CTSY/PBSTR-ADN-0005
 * @SYMTestCaseDesc Read and write multiple phonebooks simultaneously. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Numbers read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify it is possible to post read/write requests simultaneously to different phone books.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	TFixedArray<TPhoneBookStoreId,2> phoneBookTypeArray;
	phoneBookTypeArray[0] = KIccAdnPhoneBook;
	phoneBookTypeArray[1] = KIccFdnPhoneBook;
	DoTestProcedureL(phoneBookTypeArray);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0005ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0005ADN");
	}



CCTSYIntegrationTestPhoneBookStore0005FDN::CCTSYIntegrationTestPhoneBookStore0005FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0005Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0005FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0005FDN::~CCTSYIntegrationTestPhoneBookStore0005FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0005FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0005
 * @SYMFssID BA/CTSY/PBSTR-FDN-0005
 * @SYMTestCaseDesc Read and write multiple phonebooks simultaneously. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Numbers read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify it is possible to post read/write requests simultaneously to different phone books.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	TFixedArray<TPhoneBookStoreId,2> phoneBookTypeArray;
	phoneBookTypeArray[0] = KIccFdnPhoneBook;
	phoneBookTypeArray[1] = KIccVoiceMailBox;
	DoTestProcedureL(phoneBookTypeArray);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0005FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0005FDN");
	}



CCTSYIntegrationTestPhoneBookStore0005SDN::CCTSYIntegrationTestPhoneBookStore0005SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0005Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0005SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0005SDN::~CCTSYIntegrationTestPhoneBookStore0005SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0005SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0005
 * @SYMFssID BA/CTSY/PBSTR-SDN-0005
 * @SYMTestCaseDesc Read and write multiple phonebooks simultaneously. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Numbers read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify it is possible to post read/write requests simultaneously to different phone books.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0005SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0005SDN");
	}



CCTSYIntegrationTestPhoneBookStore0005VMBX::CCTSYIntegrationTestPhoneBookStore0005VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0005Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0005VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0005VMBX::~CCTSYIntegrationTestPhoneBookStore0005VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0005VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0005
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0005
 * @SYMTestCaseDesc Read and write multiple phonebooks simultaneously. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Numbers read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify it is possible to post read/write requests simultaneously to different phone books.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	TFixedArray<TPhoneBookStoreId,2> phoneBookTypeArray;
	phoneBookTypeArray[0] = KIccVoiceMailBox;
	phoneBookTypeArray[1] = KIccAdnPhoneBook;
	DoTestProcedureL(phoneBookTypeArray);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0005VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0005VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0005MBDN::CCTSYIntegrationTestPhoneBookStore0005MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0005Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0005MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0005MBDN::~CCTSYIntegrationTestPhoneBookStore0005MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0005MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0005
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0005
 * @SYMTestCaseDesc Read and write multiple phonebooks simultaneously. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Numbers read successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify it is possible to post read/write requests simultaneously to different phone books.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10009);
	TFixedArray<TPhoneBookStoreId,2> phoneBookTypeArray;
	phoneBookTypeArray[0] = KIccMbdnPhoneBook;
	phoneBookTypeArray[1] = KIccAdnPhoneBook;
	DoTestProcedureL(phoneBookTypeArray);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0005MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0005MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0006ADN::CCTSYIntegrationTestPhoneBookStore0006ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0006Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0006ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0006ADN::~CCTSYIntegrationTestPhoneBookStore0006ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0006ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0006
 * @SYMFssID BA/CTSY/PBSTR-ADN-0006
 * @SYMTestCaseDesc Read with no SIM present. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Read request returns KErrEtelInitialisationFailure due to lack of SIM.

 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verfiy KErrEtelInitialisationFailure is returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10044</font>"));
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0006ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0006ADN");
	}



CCTSYIntegrationTestPhoneBookStore0006FDN::CCTSYIntegrationTestPhoneBookStore0006FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0006Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0006FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0006FDN::~CCTSYIntegrationTestPhoneBookStore0006FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0006FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0006
 * @SYMFssID BA/CTSY/PBSTR-FDN-0006
 * @SYMTestCaseDesc Read with no SIM present. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Read request returns KErrEtelInitialisationFailure due to lack of SIM.

 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verfiy KErrEtelInitialisationFailure is returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10044</font>"));
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0006FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0006FDN");
	}



CCTSYIntegrationTestPhoneBookStore0006SDN::CCTSYIntegrationTestPhoneBookStore0006SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0006Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0006SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0006SDN::~CCTSYIntegrationTestPhoneBookStore0006SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0006SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0006
 * @SYMFssID BA/CTSY/PBSTR-SDN-0006
 * @SYMTestCaseDesc Read with no SIM present. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Read request returns KErrEtelInitialisationFailure due to lack of SIM.

 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verfiy KErrEtelInitialisationFailure is returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10044</font>"));
	DoTestProcedureL(KIccSdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0006SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0006SDN");
	}



CCTSYIntegrationTestPhoneBookStore0006VMBX::CCTSYIntegrationTestPhoneBookStore0006VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0006Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0006VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0006VMBX::~CCTSYIntegrationTestPhoneBookStore0006VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0006VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0006
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0006
 * @SYMTestCaseDesc Read with no SIM present. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Read request returns KErrEtelInitialisationFailure due to lack of SIM.

 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verfiy KErrEtelInitialisationFailure is returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10044</font>"));
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0006VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0006VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0006MBDN::CCTSYIntegrationTestPhoneBookStore0006MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0006Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0006MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0006MBDN::~CCTSYIntegrationTestPhoneBookStore0006MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0006MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0006
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0006
 * @SYMTestCaseDesc Read with no SIM present. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Read request returns KErrEtelInitialisationFailure due to lack of SIM.

 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verfiy KErrEtelInitialisationFailure is returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10044</font>"));
	DoTestProcedureL(KIccMbdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0006MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0006MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0007ADN::CCTSYIntegrationTestPhoneBookStore0007ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0007Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0007ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0007ADN::~CCTSYIntegrationTestPhoneBookStore0007ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0007ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0007
 * @SYMFssID BA/CTSY/PBSTR-ADN-0007
 * @SYMTestCaseDesc Cancel reading multiple entries. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Some entries read by the time the request is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify some entries but not all entries have been read before cancellation.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10043</font>"));
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0007ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0007ADN");
	}



CCTSYIntegrationTestPhoneBookStore0007FDN::CCTSYIntegrationTestPhoneBookStore0007FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0007Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0007FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0007FDN::~CCTSYIntegrationTestPhoneBookStore0007FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0007FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0007
 * @SYMFssID BA/CTSY/PBSTR-FDN-0007
 * @SYMTestCaseDesc Cancel reading multiple entries. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Some entries read by the time the request is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify some entries but not all entries have been read before cancellation.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10043</font>"));
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0007FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0007FDN");
	}



CCTSYIntegrationTestPhoneBookStore0007SDN::CCTSYIntegrationTestPhoneBookStore0007SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0007Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0007SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0007SDN::~CCTSYIntegrationTestPhoneBookStore0007SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0007SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0007
 * @SYMFssID BA/CTSY/PBSTR-SDN-0007
 * @SYMTestCaseDesc Cancel reading multiple entries. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Some entries read by the time the request is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify some entries but not all entries have been read before cancellation.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0007SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0007SDN");
	}



CCTSYIntegrationTestPhoneBookStore0007VMBX::CCTSYIntegrationTestPhoneBookStore0007VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0007Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0007VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0007VMBX::~CCTSYIntegrationTestPhoneBookStore0007VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0007VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0007
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0007
 * @SYMTestCaseDesc Cancel reading multiple entries. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Some entries read by the time the request is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify some entries but not all entries have been read before cancellation.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10006</font>"));
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0007VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0007VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0007MBDN::CCTSYIntegrationTestPhoneBookStore0007MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0007Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0007MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0007MBDN::~CCTSYIntegrationTestPhoneBookStore0007MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0007MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0007
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0007
 * @SYMTestCaseDesc Cancel reading multiple entries. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Some entries read by the time the request is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify some entries but not all entries have been read before cancellation.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0007MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0007MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0008ADN::CCTSYIntegrationTestPhoneBookStore0008ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0008Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0008ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0008ADN::~CCTSYIntegrationTestPhoneBookStore0008ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0008ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0008
 * @SYMFssID BA/CTSY/PBSTR-ADN-0008
 * @SYMTestCaseDesc Write national, international numbers to the SIM phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read, RMobilePhoneBookStore::Write, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Valid numbers written correctly
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries are written correctly and can be read.
 *
 * @return - TVerdict code
 */
	{ 
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0008ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0008ADN");
	}



CCTSYIntegrationTestPhoneBookStore0008FDN::CCTSYIntegrationTestPhoneBookStore0008FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0008Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0008FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0008FDN::~CCTSYIntegrationTestPhoneBookStore0008FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0008FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0008
 * @SYMFssID BA/CTSY/PBSTR-FDN-0008
 * @SYMTestCaseDesc Write national, international numbers to the SIM phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read, RMobilePhoneBookStore::Write, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Valid numbers written correctly
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries are written correctly and can be read.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0008FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0008FDN");
	}



CCTSYIntegrationTestPhoneBookStore0008SDN::CCTSYIntegrationTestPhoneBookStore0008SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0008Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0008SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0008SDN::~CCTSYIntegrationTestPhoneBookStore0008SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0008SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0008
 * @SYMFssID BA/CTSY/PBSTR-SDN-0008
 * @SYMTestCaseDesc Write national, international numbers to the SIM phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read, RMobilePhoneBookStore::Write, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Valid numbers written correctly
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries are written correctly and can be read.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0008SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0008SDN");
	}



CCTSYIntegrationTestPhoneBookStore0008VMBX::CCTSYIntegrationTestPhoneBookStore0008VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0008Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0008VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0008VMBX::~CCTSYIntegrationTestPhoneBookStore0008VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0008VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0008
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0008
 * @SYMTestCaseDesc Write national, international numbers to the SIM phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read, RMobilePhoneBookStore::Write, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Valid numbers written correctly
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries are written correctly and can be read.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10006</font>"));
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0008VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0008VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0008MBDN::CCTSYIntegrationTestPhoneBookStore0008MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0008Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0008MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0008MBDN::~CCTSYIntegrationTestPhoneBookStore0008MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0008MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0008
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0008
 * @SYMTestCaseDesc Write national, international numbers to the SIM phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read, RMobilePhoneBookStore::Write, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Valid numbers written correctly
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries are written correctly and can be read.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0008MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0008MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0009ADN::CCTSYIntegrationTestPhoneBookStore0009ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0009Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0009ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0009ADN::~CCTSYIntegrationTestPhoneBookStore0009ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0009ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0009
 * @SYMFssID BA/CTSY/PBSTR-ADN-0009
 * @SYMTestCaseDesc Write invalid entries to SIM phone book (e.g. name too long) (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrGsm0707TextStringTooLong returned if the name or number is too long. KErrArgument if a bad descriptor is supplied.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write an invalid entry and number of used entries in phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10008);
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0009ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0009ADN");
	}



CCTSYIntegrationTestPhoneBookStore0009FDN::CCTSYIntegrationTestPhoneBookStore0009FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0009Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0009FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0009FDN::~CCTSYIntegrationTestPhoneBookStore0009FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0009FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0009
 * @SYMFssID BA/CTSY/PBSTR-FDN-0009
 * @SYMTestCaseDesc Write invalid entries to SIM phone book (e.g. name too long) (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrGsm0707TextStringTooLong returned if the name or number is too long. KErrArgument if a bad descriptor is supplied.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write an invalid entry and number of used entries in phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10008);
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0009FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0009FDN");
	}



CCTSYIntegrationTestPhoneBookStore0009SDN::CCTSYIntegrationTestPhoneBookStore0009SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0009Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0009SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0009SDN::~CCTSYIntegrationTestPhoneBookStore0009SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0009SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0009
 * @SYMFssID BA/CTSY/PBSTR-SDN-0009
 * @SYMTestCaseDesc Write invalid entries to SIM phone book (e.g. name too long) (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrGsm0707TextStringTooLong returned if the name or number is too long. KErrArgument if a bad descriptor is supplied.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write an invalid entry and number of used entries in phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0009SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0009SDN");
	}



CCTSYIntegrationTestPhoneBookStore0009VMBX::CCTSYIntegrationTestPhoneBookStore0009VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0009Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0009VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0009VMBX::~CCTSYIntegrationTestPhoneBookStore0009VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0009VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0009
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0009
 * @SYMTestCaseDesc Write invalid entries to SIM phone book (e.g. name too long) (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrGsm0707TextStringTooLong returned if the name or number is too long. KErrArgument if a bad descriptor is supplied.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write an invalid entry and number of used entries in phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0009VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0009VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0009MBDN::CCTSYIntegrationTestPhoneBookStore0009MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0009Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0009MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0009MBDN::~CCTSYIntegrationTestPhoneBookStore0009MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0009MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0009
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0009
 * @SYMTestCaseDesc Write invalid entries to SIM phone book (e.g. name too long) (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrGsm0707TextStringTooLong returned if the name or number is too long. KErrArgument if a bad descriptor is supplied.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write an invalid entry and number of used entries in phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10009);
	DoTestProcedureL(KIccMbdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0009MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0009MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0010ADN::CCTSYIntegrationTestPhoneBookStore0010ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0010Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0010ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0010ADN::~CCTSYIntegrationTestPhoneBookStore0010ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0010ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0010
 * @SYMFssID BA/CTSY/PBSTR-ADN-0010
 * @SYMTestCaseDesc Write a long (40 digit) number to the SIM phone book to USIM which supports extension fields.  (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - A 40 digit number can be stored and retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry is written to the phone book.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0010ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0010ADN");
	}



CCTSYIntegrationTestPhoneBookStore0010FDN::CCTSYIntegrationTestPhoneBookStore0010FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0010Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0010FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0010FDN::~CCTSYIntegrationTestPhoneBookStore0010FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0010FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0010
 * @SYMFssID BA/CTSY/PBSTR-FDN-0010
 * @SYMTestCaseDesc Write a long (40 digit) number to the SIM phone book to USIM which supports extension fields.  (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - A 40 digit number can be stored and retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry is written to the phone book.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0010FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0010FDN");
	}



CCTSYIntegrationTestPhoneBookStore0010SDN::CCTSYIntegrationTestPhoneBookStore0010SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0010Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0010SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0010SDN::~CCTSYIntegrationTestPhoneBookStore0010SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0010SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0010
 * @SYMFssID BA/CTSY/PBSTR-SDN-0010
 * @SYMTestCaseDesc Write a long (40 digit) number to the SIM phone book to USIM which supports extension fields.  (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - A 40 digit number can be stored and retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry is written to the phone book.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0010SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0010SDN");
	}



CCTSYIntegrationTestPhoneBookStore0010VMBX::CCTSYIntegrationTestPhoneBookStore0010VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0010Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0010VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0010VMBX::~CCTSYIntegrationTestPhoneBookStore0010VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0010VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0010
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0010
 * @SYMTestCaseDesc Write a long (40 digit) number to the SIM phone book to USIM which supports extension fields.  (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - A 40 digit number can be stored and retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry is written to the phone book.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0010VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0010VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0010MBDN::CCTSYIntegrationTestPhoneBookStore0010MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0010Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0010MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0010MBDN::~CCTSYIntegrationTestPhoneBookStore0010MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0010MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0010
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0010
 * @SYMTestCaseDesc Write a long (40 digit) number to the SIM phone book to USIM which supports extension fields.  (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - A 40 digit number can be stored and retrieved.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry is written to the phone book.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0010MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0010MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0011ADN::CCTSYIntegrationTestPhoneBookStore0011ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0011Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0011ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0011ADN::~CCTSYIntegrationTestPhoneBookStore0011ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0011ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0011
 * @SYMFssID BA/CTSY/PBSTR-ADN-0011
 * @SYMTestCaseDesc Write an entry to phonebook at index of -1 (first free slot). (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhoneBookStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Entry written.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry is written correctly and correct slot number is returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0011ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0011ADN");
	}



CCTSYIntegrationTestPhoneBookStore0011FDN::CCTSYIntegrationTestPhoneBookStore0011FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0011Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0011FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0011FDN::~CCTSYIntegrationTestPhoneBookStore0011FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0011FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0011
 * @SYMFssID BA/CTSY/PBSTR-FDN-0011
 * @SYMTestCaseDesc Write an entry to phonebook at index of -1 (first free slot). (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhoneBookStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Entry written.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry is written correctly and correct slot number is returned.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0011FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0011FDN");
	}



CCTSYIntegrationTestPhoneBookStore0011SDN::CCTSYIntegrationTestPhoneBookStore0011SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0011Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0011SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0011SDN::~CCTSYIntegrationTestPhoneBookStore0011SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0011SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0011
 * @SYMFssID BA/CTSY/PBSTR-SDN-0011
 * @SYMTestCaseDesc Write an entry to phonebook at index of -1 (first free slot). (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhoneBookStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Entry written.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry is written correctly and correct slot number is returned.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0011SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0011SDN");
	}



CCTSYIntegrationTestPhoneBookStore0011VMBX::CCTSYIntegrationTestPhoneBookStore0011VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0011Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0011VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0011VMBX::~CCTSYIntegrationTestPhoneBookStore0011VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0011VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0011
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0011
 * @SYMTestCaseDesc Write an entry to phonebook at index of -1 (first free slot). (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhoneBookStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Entry written.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry is written correctly and correct slot number is returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0011VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0011VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0011MBDN::CCTSYIntegrationTestPhoneBookStore0011MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0011Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0011MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0011MBDN::~CCTSYIntegrationTestPhoneBookStore0011MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0011MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0011
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0011
 * @SYMTestCaseDesc Write an entry to phonebook at index of -1 (first free slot). (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhoneBookStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Entry written.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry is written correctly and correct slot number is returned.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0011MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0011MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0012ADN::CCTSYIntegrationTestPhoneBookStore0012ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0012Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0012ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0012ADN::~CCTSYIntegrationTestPhoneBookStore0012ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0012ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0012
 * @SYMFssID BA/CTSY/PBSTR-ADN-0012
 * @SYMTestCaseDesc Write an entry to phonebook supplying an invalid index. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrArgument returned when writing to an invalid index.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write to an invalid index and number of used entries in the phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0012ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0012ADN");
	}



CCTSYIntegrationTestPhoneBookStore0012FDN::CCTSYIntegrationTestPhoneBookStore0012FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0012Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0012FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0012FDN::~CCTSYIntegrationTestPhoneBookStore0012FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0012FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0012
 * @SYMFssID BA/CTSY/PBSTR-FDN-0012
 * @SYMTestCaseDesc Write an entry to phonebook supplying an invalid index. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrArgument returned when writing to an invalid index.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write to an invalid index and number of used entries in the phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook, KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0012FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0012FDN");
	}



CCTSYIntegrationTestPhoneBookStore0012SDN::CCTSYIntegrationTestPhoneBookStore0012SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0012Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0012SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0012SDN::~CCTSYIntegrationTestPhoneBookStore0012SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0012SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0012
 * @SYMFssID BA/CTSY/PBSTR-SDN-0012
 * @SYMTestCaseDesc Write an entry to phonebook supplying an invalid index. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrArgument returned when writing to an invalid index.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write to an invalid index and number of used entries in the phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0012SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0012SDN");
	}



CCTSYIntegrationTestPhoneBookStore0012VMBX::CCTSYIntegrationTestPhoneBookStore0012VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0012Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0012VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0012VMBX::~CCTSYIntegrationTestPhoneBookStore0012VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0012VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0012
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0012
 * @SYMTestCaseDesc Write an entry to phonebook supplying an invalid index. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrArgument returned when writing to an invalid index.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write to an invalid index and number of used entries in the phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0012VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0012VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0012MBDN::CCTSYIntegrationTestPhoneBookStore0012MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0012Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0012MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0012MBDN::~CCTSYIntegrationTestPhoneBookStore0012MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0012MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0012
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0012
 * @SYMTestCaseDesc Write an entry to phonebook supplying an invalid index. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrArgument returned when writing to an invalid index.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify error is returned on attempt to write to an invalid index and number of used entries in the phone book is unchanged.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10009);
	DoTestProcedureL(KIccMbdnPhoneBook,KETelIccMbdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0012MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0012MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0013ADN::CCTSYIntegrationTestPhoneBookStore0013ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0013Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0013ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0013ADN::~CCTSYIntegrationTestPhoneBookStore0013ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0013ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0013
 * @SYMFssID BA/CTSY/PBSTR-ADN-0013
 * @SYMTestCaseDesc Write an entry which makes the phonebook full. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Entry written. RMobilePhoneStore::KStoreFull event occurs.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry written and store event completes indicating store is now full.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10032);
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0013ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0013ADN");
	}



CCTSYIntegrationTestPhoneBookStore0013FDN::CCTSYIntegrationTestPhoneBookStore0013FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0013Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0013FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0013FDN::~CCTSYIntegrationTestPhoneBookStore0013FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0013FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0013
 * @SYMFssID BA/CTSY/PBSTR-FDN-0013
 * @SYMTestCaseDesc Write an entry which makes the phonebook full. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Entry written. RMobilePhoneStore::KStoreFull event occurs.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry written and store event completes indicating store is now full.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10032);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0013FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0013FDN");
	}



CCTSYIntegrationTestPhoneBookStore0013SDN::CCTSYIntegrationTestPhoneBookStore0013SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0013Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0013SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0013SDN::~CCTSYIntegrationTestPhoneBookStore0013SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0013SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0013
 * @SYMFssID BA/CTSY/PBSTR-SDN-0013
 * @SYMTestCaseDesc Write an entry which makes the phonebook full. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Entry written. RMobilePhoneStore::KStoreFull event occurs.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry written and store event completes indicating store is now full.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0013SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0013SDN");
	}



CCTSYIntegrationTestPhoneBookStore0013VMBX::CCTSYIntegrationTestPhoneBookStore0013VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0013Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0013VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0013VMBX::~CCTSYIntegrationTestPhoneBookStore0013VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0013VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0013
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0013
 * @SYMTestCaseDesc Write an entry which makes the phonebook full. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Entry written. RMobilePhoneStore::KStoreFull event occurs.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry written and store event completes indicating store is now full.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0013VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0013VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0013MBDN::CCTSYIntegrationTestPhoneBookStore0013MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0013Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0013MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0013MBDN::~CCTSYIntegrationTestPhoneBookStore0013MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0013MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0013
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0013
 * @SYMTestCaseDesc Write an entry which makes the phonebook full. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Entry written. RMobilePhoneStore::KStoreFull event occurs.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry written and store event completes indicating store is now full.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0013MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0013MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0014ADN::CCTSYIntegrationTestPhoneBookStore0014ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0014Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0014ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0014ADN::~CCTSYIntegrationTestPhoneBookStore0014ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0014ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0014
 * @SYMFssID BA/CTSY/PBSTR-ADN-0014
 * @SYMTestCaseDesc Write an entry to phonebook when SIM is full. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Error is returned on attempt to write.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify error is returned and number of used entries is unchanged.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0014ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0014ADN");
	}



CCTSYIntegrationTestPhoneBookStore0014FDN::CCTSYIntegrationTestPhoneBookStore0014FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0014Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0014FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0014FDN::~CCTSYIntegrationTestPhoneBookStore0014FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0014FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0014
 * @SYMFssID BA/CTSY/PBSTR-FDN-0014
 * @SYMTestCaseDesc Write an entry to phonebook when SIM is full. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Error is returned on attempt to write.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify error is returned and number of used entries is unchanged.
 *
 * @return - TVerdict code
 */
	{	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0014FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0014FDN");
	}



CCTSYIntegrationTestPhoneBookStore0014SDN::CCTSYIntegrationTestPhoneBookStore0014SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0014Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0014SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0014SDN::~CCTSYIntegrationTestPhoneBookStore0014SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0014SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0014
 * @SYMFssID BA/CTSY/PBSTR-SDN-0014
 * @SYMTestCaseDesc Write an entry to phonebook when SIM is full. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Error is returned on attempt to write.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify error is returned and number of used entries is unchanged.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0014SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0014SDN");
	}



CCTSYIntegrationTestPhoneBookStore0014VMBX::CCTSYIntegrationTestPhoneBookStore0014VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0014Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0014VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0014VMBX::~CCTSYIntegrationTestPhoneBookStore0014VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0014VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0014
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0014
 * @SYMTestCaseDesc Write an entry to phonebook when SIM is full. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Error is returned on attempt to write.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify error is returned and number of used entries is unchanged.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0014VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0014VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0014MBDN::CCTSYIntegrationTestPhoneBookStore0014MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0014Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0014MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0014MBDN::~CCTSYIntegrationTestPhoneBookStore0014MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0014MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0014
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0014
 * @SYMTestCaseDesc Write an entry to phonebook when SIM is full. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Error is returned on attempt to write.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify error is returned and number of used entries is unchanged.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0014MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0014MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0015ADN::CCTSYIntegrationTestPhoneBookStore0015ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0015Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0015ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0015ADN::~CCTSYIntegrationTestPhoneBookStore0015ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0015ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0015
 * @SYMFssID BA/CTSY/PBSTR-ADN-0015
 * @SYMTestCaseDesc Edit an entry in the SIM phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneBookStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Phone book entry modified and KStoreEntryChanged event completed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify new entry information is written to the phone book and correct store event completes.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0015ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0015ADN");
	}



CCTSYIntegrationTestPhoneBookStore0015FDN::CCTSYIntegrationTestPhoneBookStore0015FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0015Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0015FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0015FDN::~CCTSYIntegrationTestPhoneBookStore0015FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0015FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0015
 * @SYMFssID BA/CTSY/PBSTR-FDN-0015
 * @SYMTestCaseDesc Edit an entry in the SIM phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneBookStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Phone book entry modified and KStoreEntryChanged event completed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify new entry information is written to the phone book and correct store event completes.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0015FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0015FDN");
	}



CCTSYIntegrationTestPhoneBookStore0015SDN::CCTSYIntegrationTestPhoneBookStore0015SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0015Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0015SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0015SDN::~CCTSYIntegrationTestPhoneBookStore0015SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0015SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0015
 * @SYMFssID BA/CTSY/PBSTR-SDN-0015
 * @SYMTestCaseDesc Edit an entry in the SIM phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneBookStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Phone book entry modified and KStoreEntryChanged event completed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify new entry information is written to the phone book and correct store event completes.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0015SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0015SDN");
	}



CCTSYIntegrationTestPhoneBookStore0015VMBX::CCTSYIntegrationTestPhoneBookStore0015VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0015Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0015VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0015VMBX::~CCTSYIntegrationTestPhoneBookStore0015VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0015VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0015
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0015
 * @SYMTestCaseDesc Edit an entry in the SIM phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneBookStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Phone book entry modified and KStoreEntryChanged event completed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify new entry information is written to the phone book and correct store event completes.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0015VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0015VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0015MBDN::CCTSYIntegrationTestPhoneBookStore0015MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0015Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0015MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0015MBDN::~CCTSYIntegrationTestPhoneBookStore0015MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0015MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0015
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0015
 * @SYMTestCaseDesc Edit an entry in the SIM phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneBookStore::Read, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Phone book entry modified and KStoreEntryChanged event completed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify new entry information is written to the phone book and correct store event completes.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0015MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0015MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0016ADN::CCTSYIntegrationTestPhoneBookStore0016ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0016Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0016ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0016ADN::~CCTSYIntegrationTestPhoneBookStore0016ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0016ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0016
 * @SYMFssID BA/CTSY/PBSTR-ADN-0016
 * @SYMTestCaseDesc Delete an entry from the SIM phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Deletion is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and deleted entry cannot be read.
 *
 * @return - TVerdict code
 */
	{
	// test causing the board to reboot
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10015);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10016);
	User::Leave(KErrGeneral); // Test leaving to prevent crashing the hardware. Remove this line when defect is fixed.	
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0016ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0016ADN");
	}



CCTSYIntegrationTestPhoneBookStore0016FDN::CCTSYIntegrationTestPhoneBookStore0016FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0016Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0016FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0016FDN::~CCTSYIntegrationTestPhoneBookStore0016FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0016FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0016
 * @SYMFssID BA/CTSY/PBSTR-FDN-0016
 * @SYMTestCaseDesc Delete an entry from the SIM phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Deletion is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and deleted entry cannot be read.
 *
 * @return - TVerdict code
 */
	{
	// test causing the board to reboot
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10015);
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10016);
	User::Leave(KErrGeneral); // Test leaving to prevent crashing the hardware. Remove this line when defect is fixed.	
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0016FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0016FDN");
	}



CCTSYIntegrationTestPhoneBookStore0016SDN::CCTSYIntegrationTestPhoneBookStore0016SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0016Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0016SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0016SDN::~CCTSYIntegrationTestPhoneBookStore0016SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0016SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0016
 * @SYMFssID BA/CTSY/PBSTR-SDN-0016
 * @SYMTestCaseDesc Delete an entry from the SIM phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Deletion is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and deleted entry cannot be read.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0016SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0016SDN");
	}



CCTSYIntegrationTestPhoneBookStore0016VMBX::CCTSYIntegrationTestPhoneBookStore0016VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0016Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0016VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0016VMBX::~CCTSYIntegrationTestPhoneBookStore0016VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0016VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0016
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0016
 * @SYMTestCaseDesc Delete an entry from the SIM phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Deletion is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and deleted entry cannot be read.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0016VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0016VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0016MBDN::CCTSYIntegrationTestPhoneBookStore0016MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0016Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0016MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0016MBDN::~CCTSYIntegrationTestPhoneBookStore0016MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0016MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0016
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0016
 * @SYMTestCaseDesc Delete an entry from the SIM phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Deletion is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and deleted entry cannot be read.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0016MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0016MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0017ADN::CCTSYIntegrationTestPhoneBookStore0017ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0017Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0017ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0017ADN::~CCTSYIntegrationTestPhoneBookStore0017ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0017ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0017
 * @SYMFssID BA/CTSY/PBSTR-ADN-0017
 * @SYMTestCaseDesc Delete an entry from the SIM phone book supplying an invalid index. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0017ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0017ADN");
	}



CCTSYIntegrationTestPhoneBookStore0017FDN::CCTSYIntegrationTestPhoneBookStore0017FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0017Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0017FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0017FDN::~CCTSYIntegrationTestPhoneBookStore0017FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0017FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0017
 * @SYMFssID BA/CTSY/PBSTR-FDN-0017
 * @SYMTestCaseDesc Delete an entry from the SIM phone book supplying an invalid index. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0017FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0017FDN");
	}



CCTSYIntegrationTestPhoneBookStore0017SDN::CCTSYIntegrationTestPhoneBookStore0017SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0017Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0017SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0017SDN::~CCTSYIntegrationTestPhoneBookStore0017SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0017SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0017
 * @SYMFssID BA/CTSY/PBSTR-SDN-0017
 * @SYMTestCaseDesc Delete an entry from the SIM phone book supplying an invalid index. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument returned.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0017SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0017SDN");
	}



CCTSYIntegrationTestPhoneBookStore0017VMBX::CCTSYIntegrationTestPhoneBookStore0017VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0017Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0017VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0017VMBX::~CCTSYIntegrationTestPhoneBookStore0017VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0017VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0017
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0017
 * @SYMTestCaseDesc Delete an entry from the SIM phone book supplying an invalid index. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0017VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0017VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0017MBDN::CCTSYIntegrationTestPhoneBookStore0017MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0017Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0017MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0017MBDN::~CCTSYIntegrationTestPhoneBookStore0017MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0017MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0017
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0017
 * @SYMTestCaseDesc Delete an entry from the SIM phone book supplying an invalid index. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - KErrArgument returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrArgument returned.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0017MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0017MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0018ADN::CCTSYIntegrationTestPhoneBookStore0018ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0018Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0018ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0018ADN::~CCTSYIntegrationTestPhoneBookStore0018ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0018ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0018
 * @SYMFssID BA/CTSY/PBSTR-ADN-0018
 * @SYMTestCaseDesc Delete all entries from the SIM phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::DeleteAll, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - Deletion is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and there are no entries in the phone book after deletion.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0018ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0018ADN");
	}



CCTSYIntegrationTestPhoneBookStore0018FDN::CCTSYIntegrationTestPhoneBookStore0018FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0018Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0018FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0018FDN::~CCTSYIntegrationTestPhoneBookStore0018FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0018FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0018
 * @SYMFssID BA/CTSY/PBSTR-FDN-0018
 * @SYMTestCaseDesc Delete all entries from the SIM phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::DeleteAll, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - Deletion is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and there are no entries in the phone book after deletion.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0018FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0018FDN");
	}



CCTSYIntegrationTestPhoneBookStore0018SDN::CCTSYIntegrationTestPhoneBookStore0018SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0018Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0018SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0018SDN::~CCTSYIntegrationTestPhoneBookStore0018SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0018SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0018
 * @SYMFssID BA/CTSY/PBSTR-SDN-0018
 * @SYMTestCaseDesc Delete all entries from the SIM phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::DeleteAll, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - Deletion is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and there are no entries in the phone book after deletion.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0018SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0018SDN");
	}



CCTSYIntegrationTestPhoneBookStore0018VMBX::CCTSYIntegrationTestPhoneBookStore0018VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0018Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0018VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0018VMBX::~CCTSYIntegrationTestPhoneBookStore0018VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0018VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0018
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0018
 * @SYMTestCaseDesc Delete all entries from the SIM phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::DeleteAll, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - Deletion is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and there are no entries in the phone book after deletion.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0018VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0018VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0018MBDN::CCTSYIntegrationTestPhoneBookStore0018MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0018Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0018MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0018MBDN::~CCTSYIntegrationTestPhoneBookStore0018MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0018MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0018
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0018
 * @SYMTestCaseDesc Delete all entries from the SIM phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::DeleteAll, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - Deletion is successful.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify deletion is successful and there are no entries in the phone book after deletion.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0018MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0018MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0019ADN::CCTSYIntegrationTestPhoneBookStore0019ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0019Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0019ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0019ADN::~CCTSYIntegrationTestPhoneBookStore0019ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0019ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0019
 * @SYMFssID BA/CTSY/PBSTR-ADN-0019
 * @SYMTestCaseDesc Fill and delete all entries in the phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::DeleteAll, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - All entries deleted successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries can be written to the phone book until it is full and then all entries can be deleted from a full phone book.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0019ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0019ADN");
	}



CCTSYIntegrationTestPhoneBookStore0019FDN::CCTSYIntegrationTestPhoneBookStore0019FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0019Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0019FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0019FDN::~CCTSYIntegrationTestPhoneBookStore0019FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0019FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0019
 * @SYMFssID BA/CTSY/PBSTR-FDN-0019
 * @SYMTestCaseDesc Fill and delete all entries in the phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::DeleteAll, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - All entries deleted successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries can be written to the phone book until it is full and then all entries can be deleted from a full phone book.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0019FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0019FDN");
	}



CCTSYIntegrationTestPhoneBookStore0019SDN::CCTSYIntegrationTestPhoneBookStore0019SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0019Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0019SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0019SDN::~CCTSYIntegrationTestPhoneBookStore0019SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0019SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0019
 * @SYMFssID BA/CTSY/PBSTR-SDN-0019
 * @SYMTestCaseDesc Fill and delete all entries in the phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::DeleteAll, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - All entries deleted successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries can be written to the phone book until it is full and then all entries can be deleted from a full phone book.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0019SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0019SDN");
	}



CCTSYIntegrationTestPhoneBookStore0019VMBX::CCTSYIntegrationTestPhoneBookStore0019VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0019Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0019VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0019VMBX::~CCTSYIntegrationTestPhoneBookStore0019VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0019VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0019
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0019
 * @SYMTestCaseDesc Fill and delete all entries in the phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::DeleteAll, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - All entries deleted successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries can be written to the phone book until it is full and then all entries can be deleted from a full phone book.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0019VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0019VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0019MBDN::CCTSYIntegrationTestPhoneBookStore0019MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0019Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0019MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0019MBDN::~CCTSYIntegrationTestPhoneBookStore0019MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0019MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0019
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0019
 * @SYMTestCaseDesc Fill and delete all entries in the phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write, RMobilePhoneStore::DeleteAll, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - All entries deleted successfully.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entries can be written to the phone book until it is full and then all entries can be deleted from a full phone book.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0019MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0019MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0020ADN::CCTSYIntegrationTestPhoneBookStore0020ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0020Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0020ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0020ADN::~CCTSYIntegrationTestPhoneBookStore0020ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0020ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0020
 * @SYMFssID BA/CTSY/PBSTR-ADN-0020
 * @SYMTestCaseDesc Cancel deletion of all entries from the SIM phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhoneStore::DeleteAll, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Deletion is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify some entries have been deleted before request is cancelled.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0020ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0020ADN");
	}



CCTSYIntegrationTestPhoneBookStore0020FDN::CCTSYIntegrationTestPhoneBookStore0020FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0020Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0020FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0020FDN::~CCTSYIntegrationTestPhoneBookStore0020FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0020FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0020
 * @SYMFssID BA/CTSY/PBSTR-FDN-0020
 * @SYMTestCaseDesc Cancel deletion of all entries from the SIM phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhoneStore::DeleteAll, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Deletion is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify some entries have been deleted before request is cancelled.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0020FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0020FDN");
	}



CCTSYIntegrationTestPhoneBookStore0020SDN::CCTSYIntegrationTestPhoneBookStore0020SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0020Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0020SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0020SDN::~CCTSYIntegrationTestPhoneBookStore0020SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0020SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0020
 * @SYMFssID BA/CTSY/PBSTR-SDN-0020
 * @SYMTestCaseDesc Cancel deletion of all entries from the SIM phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhoneStore::DeleteAll, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Deletion is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify some entries have been deleted before request is cancelled.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0020SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0020SDN");
	}



CCTSYIntegrationTestPhoneBookStore0020VMBX::CCTSYIntegrationTestPhoneBookStore0020VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0020Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0020VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0020VMBX::~CCTSYIntegrationTestPhoneBookStore0020VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0020VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0020
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0020
 * @SYMTestCaseDesc Cancel deletion of all entries from the SIM phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhoneStore::DeleteAll, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Deletion is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify some entries have been deleted before request is cancelled.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0020VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0020VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0020MBDN::CCTSYIntegrationTestPhoneBookStore0020MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0020Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0020MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0020MBDN::~CCTSYIntegrationTestPhoneBookStore0020MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0020MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0020
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0020
 * @SYMTestCaseDesc Cancel deletion of all entries from the SIM phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo, RMobilePhoneStore::DeleteAll, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - Deletion is cancelled.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify some entries have been deleted before request is cancelled.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0020MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0020MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0021ADN::CCTSYIntegrationTestPhoneBookStore0021ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0021Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0021ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0021ADN::~CCTSYIntegrationTestPhoneBookStore0021ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0021ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0021
 * @SYMFssID BA/CTSY/PBSTR-ADN-0021
 * @SYMTestCaseDesc Delete an entry from a full phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - RMobilePhoneStore::KStoreHasSpace completed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry deleted successfully and phone book event completes.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10032);
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0021ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0021ADN");
	}



CCTSYIntegrationTestPhoneBookStore0021FDN::CCTSYIntegrationTestPhoneBookStore0021FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0021Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0021FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0021FDN::~CCTSYIntegrationTestPhoneBookStore0021FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0021FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0021
 * @SYMFssID BA/CTSY/PBSTR-FDN-0021
 * @SYMTestCaseDesc Delete an entry from a full phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - RMobilePhoneStore::KStoreHasSpace completed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry deleted successfully and phone book event completes.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10032);
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0021FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0021FDN");
	}



CCTSYIntegrationTestPhoneBookStore0021SDN::CCTSYIntegrationTestPhoneBookStore0021SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0021Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0021SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0021SDN::~CCTSYIntegrationTestPhoneBookStore0021SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0021SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0021
 * @SYMFssID BA/CTSY/PBSTR-SDN-0021
 * @SYMTestCaseDesc Delete an entry from a full phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - RMobilePhoneStore::KStoreHasSpace completed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry deleted successfully and phone book event completes.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0021SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0021SDN");
	}



CCTSYIntegrationTestPhoneBookStore0021VMBX::CCTSYIntegrationTestPhoneBookStore0021VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0021Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0021VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0021VMBX::~CCTSYIntegrationTestPhoneBookStore0021VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0021VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0021
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0021
 * @SYMTestCaseDesc Delete an entry from a full phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - RMobilePhoneStore::KStoreHasSpace completed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry deleted successfully and phone book event completes.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0021VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0021VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0021MBDN::CCTSYIntegrationTestPhoneBookStore0021MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0021Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0021MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0021MBDN::~CCTSYIntegrationTestPhoneBookStore0021MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0021MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0021
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0021
 * @SYMTestCaseDesc Delete an entry from a full phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneStore::GetInfo, RMobilePhoneStore::NotifyStoreEvent, RMobilePhone::GetPhoneStoreInfo
 * @SYMTestExpectedResults Pass - RMobilePhoneStore::KStoreHasSpace completed.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify entry deleted successfully and phone book event completes.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0021MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0021MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0022ADN::CCTSYIntegrationTestPhoneBookStore0022ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0022Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0022ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0022ADN::~CCTSYIntegrationTestPhoneBookStore0022ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0022ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0022
 * @SYMFssID BA/CTSY/PBSTR-ADN-0022
 * @SYMTestCaseDesc Delete a long (>20 digit) number from the SIM phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Number is deleted.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry deleted successfully.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook,KETelIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0022ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0022ADN");
	}



CCTSYIntegrationTestPhoneBookStore0022FDN::CCTSYIntegrationTestPhoneBookStore0022FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0022Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0022FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0022FDN::~CCTSYIntegrationTestPhoneBookStore0022FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0022FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0022
 * @SYMFssID BA/CTSY/PBSTR-FDN-0022
 * @SYMTestCaseDesc Delete a long (>20 digit) number from the SIM phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Number is deleted.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry deleted successfully.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook,KETelIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0022FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0022FDN");
	}



CCTSYIntegrationTestPhoneBookStore0022SDN::CCTSYIntegrationTestPhoneBookStore0022SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0022Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0022SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0022SDN::~CCTSYIntegrationTestPhoneBookStore0022SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0022SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0022
 * @SYMFssID BA/CTSY/PBSTR-SDN-0022
 * @SYMTestCaseDesc Delete a long (>20 digit) number from the SIM phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Number is deleted.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry deleted successfully.
 *
 * @return - TVerdict code
 */
	{

	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0022SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0022SDN");
	}



CCTSYIntegrationTestPhoneBookStore0022VMBX::CCTSYIntegrationTestPhoneBookStore0022VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0022Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0022VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0022VMBX::~CCTSYIntegrationTestPhoneBookStore0022VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0022VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0022
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0022
 * @SYMTestCaseDesc Delete a long (>20 digit) number from the SIM phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Number is deleted.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry deleted successfully.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox,KETelIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0022VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0022VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0022MBDN::CCTSYIntegrationTestPhoneBookStore0022MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0022Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0022MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0022MBDN::~CCTSYIntegrationTestPhoneBookStore0022MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0022MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0022
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0022
 * @SYMTestCaseDesc Delete a long (>20 digit) number from the SIM phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete, RMobilePhoneBookStore::Read, RMobilePhoneStore::GetInfo, RMobilePhone::GetPhoneStoreInfo, RMobilePhoneStore::NotifyStoreEvent
 * @SYMTestExpectedResults Pass - Number is deleted.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/manual
 *
 * Reason for test: Verify entry deleted successfully.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0022MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0022MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0023ADN::CCTSYIntegrationTestPhoneBookStore0023ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0023Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0023ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0023ADN::~CCTSYIntegrationTestPhoneBookStore0023ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0023ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0023
 * @SYMFssID BA/CTSY/PBSTR-ADN-0023
 * @SYMTestCaseDesc Read a single phone book entry into a buffer that is too small. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - KErrGeneral returned when the descriptor is too small.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrGeneral returned
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10010);
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0023ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0023ADN");
	}



CCTSYIntegrationTestPhoneBookStore0023FDN::CCTSYIntegrationTestPhoneBookStore0023FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0023Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0023FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0023FDN::~CCTSYIntegrationTestPhoneBookStore0023FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0023FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0023
 * @SYMFssID BA/CTSY/PBSTR-FDN-0023
 * @SYMTestCaseDesc Read a single phone book entry into a buffer that is too small. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - KErrGeneral returned when the descriptor is too small.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrGeneral returned
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10010);
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0023FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0023FDN");
	}



CCTSYIntegrationTestPhoneBookStore0023SDN::CCTSYIntegrationTestPhoneBookStore0023SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0023Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0023SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0023SDN::~CCTSYIntegrationTestPhoneBookStore0023SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0023SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0023
 * @SYMFssID BA/CTSY/PBSTR-SDN-0023
 * @SYMTestCaseDesc Read a single phone book entry into a buffer that is too small. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - KErrGeneral returned when the descriptor is too small.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrGeneral returned
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0023SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0023SDN");
	}



CCTSYIntegrationTestPhoneBookStore0023VMBX::CCTSYIntegrationTestPhoneBookStore0023VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0023Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0023VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0023VMBX::~CCTSYIntegrationTestPhoneBookStore0023VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0023VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0023
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0023
 * @SYMTestCaseDesc Read a single phone book entry into a buffer that is too small. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - KErrGeneral returned when the descriptor is too small.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrGeneral returned
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0023VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0023VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0023MBDN::CCTSYIntegrationTestPhoneBookStore0023MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0023Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0023MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0023MBDN::~CCTSYIntegrationTestPhoneBookStore0023MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0023MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0023
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0023
 * @SYMTestCaseDesc Read a single phone book entry into a buffer that is too small. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::Read
 * @SYMTestExpectedResults Pass - KErrGeneral returned when the descriptor is too small.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrGeneral returned
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10010);
	DoTestProcedureL(KIccMbdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0023MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0023MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0024ADN::CCTSYIntegrationTestPhoneBookStore0024ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0024Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0024ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0024ADN::~CCTSYIntegrationTestPhoneBookStore0024ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0024ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0024
 * @SYMFssID BA/CTSY/PBSTR-ADN-0024
 * @SYMTestCaseDesc Read multiple phone book entries when phone book is empty. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Number of entries TSY tried to read and number of entries actually read correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0024ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0024ADN");
	}



CCTSYIntegrationTestPhoneBookStore0024FDN::CCTSYIntegrationTestPhoneBookStore0024FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0024Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0024FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0024FDN::~CCTSYIntegrationTestPhoneBookStore0024FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0024FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0024
 * @SYMFssID BA/CTSY/PBSTR-FDN-0024
 * @SYMTestCaseDesc Read multiple phone book entries when phone book is empty. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Number of entries TSY tried to read and number of entries actually read correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0024FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0024FDN");
	}



CCTSYIntegrationTestPhoneBookStore0024SDN::CCTSYIntegrationTestPhoneBookStore0024SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0024Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0024SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0024SDN::~CCTSYIntegrationTestPhoneBookStore0024SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0024SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0024
 * @SYMFssID BA/CTSY/PBSTR-SDN-0024
 * @SYMTestCaseDesc Read multiple phone book entries when phone book is empty. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Number of entries TSY tried to read and number of entries actually read correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{

	DoTestProcedureL(KIccSdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0024SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0024SDN");
	}



CCTSYIntegrationTestPhoneBookStore0024VMBX::CCTSYIntegrationTestPhoneBookStore0024VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0024Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0024VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0024VMBX::~CCTSYIntegrationTestPhoneBookStore0024VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0024VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0024
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0024
 * @SYMTestCaseDesc Read multiple phone book entries when phone book is empty. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Number of entries TSY tried to read and number of entries actually read correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0024VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0024VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0024MBDN::CCTSYIntegrationTestPhoneBookStore0024MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0024Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0024MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0024MBDN::~CCTSYIntegrationTestPhoneBookStore0024MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0024MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0024
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0024
 * @SYMTestCaseDesc Read multiple phone book entries when phone book is empty. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - Number of entries TSY tried to read and number of entries actually read correct.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrNotFound is returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccMbdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0024MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0024MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0025ADN::CCTSYIntegrationTestPhoneBookStore0025ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0025Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0025ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0025ADN::~CCTSYIntegrationTestPhoneBookStore0025ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0025ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0025
 * @SYMFssID BA/CTSY/PBSTR-ADN-0025
 * @SYMTestCaseDesc Read multiple phone book entries into a buffer that is too small. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrGeneral returned when the descriptor is too small.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrGeneral returned
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10010);
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0025ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0025ADN");
	}



CCTSYIntegrationTestPhoneBookStore0025FDN::CCTSYIntegrationTestPhoneBookStore0025FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0025Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0025FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0025FDN::~CCTSYIntegrationTestPhoneBookStore0025FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0025FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0025
 * @SYMFssID BA/CTSY/PBSTR-FDN-0025
 * @SYMTestCaseDesc Read multiple phone book entries into a buffer that is too small. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrGeneral returned when the descriptor is too small.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrGeneral returned
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10010);
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0025FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0025FDN");
	}



CCTSYIntegrationTestPhoneBookStore0025SDN::CCTSYIntegrationTestPhoneBookStore0025SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0025Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0025SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0025SDN::~CCTSYIntegrationTestPhoneBookStore0025SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0025SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0025
 * @SYMFssID BA/CTSY/PBSTR-SDN-0025
 * @SYMTestCaseDesc Read multiple phone book entries into a buffer that is too small. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrGeneral returned when the descriptor is too small.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrGeneral returned
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0025SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0025SDN");
	}



CCTSYIntegrationTestPhoneBookStore0025VMBX::CCTSYIntegrationTestPhoneBookStore0025VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0025Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0025VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0025VMBX::~CCTSYIntegrationTestPhoneBookStore0025VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0025VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0025
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0025
 * @SYMTestCaseDesc Read multiple phone book entries into a buffer that is too small. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrGeneral returned when the descriptor is too small.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrGeneral returned
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF2(_L("<font color=Orange>$CTSYKnownFailure: defect id = %d</font>"), 10006);
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0025VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0025VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0025MBDN::CCTSYIntegrationTestPhoneBookStore0025MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0025Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0025MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0025MBDN::~CCTSYIntegrationTestPhoneBookStore0025MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0025MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0025
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0025
 * @SYMTestCaseDesc Read multiple phone book entries into a buffer that is too small. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrGeneral returned when the descriptor is too small.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrGeneral returned
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();

	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0025MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0025MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0026ADN::CCTSYIntegrationTestPhoneBookStore0026ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0026Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0026ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0026ADN::~CCTSYIntegrationTestPhoneBookStore0026ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0026ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0026
 * @SYMFssID BA/CTSY/PBSTR-ADN-0026
 * @SYMTestCaseDesc Cancel reading a single entry from the phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0026ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0026ADN");
	}



CCTSYIntegrationTestPhoneBookStore0026FDN::CCTSYIntegrationTestPhoneBookStore0026FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0026Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0026FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0026FDN::~CCTSYIntegrationTestPhoneBookStore0026FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0026FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0026
 * @SYMFssID BA/CTSY/PBSTR-FDN-0026
 * @SYMTestCaseDesc Cancel reading a single entry from the phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0026FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0026FDN");
	}



CCTSYIntegrationTestPhoneBookStore0026SDN::CCTSYIntegrationTestPhoneBookStore0026SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0026Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0026SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0026SDN::~CCTSYIntegrationTestPhoneBookStore0026SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0026SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0026
 * @SYMFssID BA/CTSY/PBSTR-SDN-0026
 * @SYMTestCaseDesc Cancel reading a single entry from the phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0026SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0026SDN");
	}



CCTSYIntegrationTestPhoneBookStore0026VMBX::CCTSYIntegrationTestPhoneBookStore0026VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0026Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0026VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0026VMBX::~CCTSYIntegrationTestPhoneBookStore0026VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0026VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0026
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0026
 * @SYMTestCaseDesc Cancel reading a single entry from the phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10006</font>"));
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0026VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0026VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0026MBDN::CCTSYIntegrationTestPhoneBookStore0026MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0026Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0026MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0026MBDN::~CCTSYIntegrationTestPhoneBookStore0026MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0026MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0026
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0026
 * @SYMTestCaseDesc Cancel reading a single entry from the phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Read
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0026MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0026MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0027ADN::CCTSYIntegrationTestPhoneBookStore0027ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0027Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0027ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0027ADN::~CCTSYIntegrationTestPhoneBookStore0027ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0027ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0027
 * @SYMFssID BA/CTSY/PBSTR-ADN-0027
 * @SYMTestCaseDesc Cancel writing an entry to the phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0027ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0027ADN");
	}



CCTSYIntegrationTestPhoneBookStore0027FDN::CCTSYIntegrationTestPhoneBookStore0027FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0027Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0027FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0027FDN::~CCTSYIntegrationTestPhoneBookStore0027FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0027FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0027
 * @SYMFssID BA/CTSY/PBSTR-FDN-0027
 * @SYMTestCaseDesc Cancel writing an entry to the phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0027FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0027FDN");
	}



CCTSYIntegrationTestPhoneBookStore0027SDN::CCTSYIntegrationTestPhoneBookStore0027SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0027Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0027SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0027SDN::~CCTSYIntegrationTestPhoneBookStore0027SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0027SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0027
 * @SYMFssID BA/CTSY/PBSTR-SDN-0027
 * @SYMTestCaseDesc Cancel writing an entry to the phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0027SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0027SDN");
	}



CCTSYIntegrationTestPhoneBookStore0027VMBX::CCTSYIntegrationTestPhoneBookStore0027VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0027Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0027VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0027VMBX::~CCTSYIntegrationTestPhoneBookStore0027VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0027VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0027
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0027
 * @SYMTestCaseDesc Cancel writing an entry to the phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10006</font>"));
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0027VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0027VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0027MBDN::CCTSYIntegrationTestPhoneBookStore0027MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0027Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0027MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0027MBDN::~CCTSYIntegrationTestPhoneBookStore0027MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0027MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0027
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0027
 * @SYMTestCaseDesc Cancel writing an entry to the phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Write
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0027MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0027MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0028ADN::CCTSYIntegrationTestPhoneBookStore0028ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0028Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0028ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0028ADN::~CCTSYIntegrationTestPhoneBookStore0028ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0028ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0028
 * @SYMFssID BA/CTSY/PBSTR-ADN-0028
 * @SYMTestCaseDesc Cancel deleting an entry from the phone book. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0028ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0028ADN");
	}



CCTSYIntegrationTestPhoneBookStore0028FDN::CCTSYIntegrationTestPhoneBookStore0028FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0028Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0028FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0028FDN::~CCTSYIntegrationTestPhoneBookStore0028FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0028FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0028
 * @SYMFssID BA/CTSY/PBSTR-FDN-0028
 * @SYMTestCaseDesc Cancel deleting an entry from the phone book. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	RMobilePhone& mobilePhone = iEtelSessionMgr.GetPhoneL(KMainServer, KMainPhone);
	iSimTsyTestHelper.VerifyPin2L(mobilePhone,iNetworkTsyTestHelper);
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0028FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0028FDN");
	}



CCTSYIntegrationTestPhoneBookStore0028SDN::CCTSYIntegrationTestPhoneBookStore0028SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0028Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0028SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0028SDN::~CCTSYIntegrationTestPhoneBookStore0028SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0028SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0028
 * @SYMFssID BA/CTSY/PBSTR-SDN-0028
 * @SYMTestCaseDesc Cancel deleting an entry from the phone book. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for SDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0028SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0028SDN");
	}



CCTSYIntegrationTestPhoneBookStore0028VMBX::CCTSYIntegrationTestPhoneBookStore0028VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0028Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0028VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0028VMBX::~CCTSYIntegrationTestPhoneBookStore0028VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0028VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0028
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0028
 * @SYMTestCaseDesc Cancel deleting an entry from the phone book. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	ERR_PRINTF1(_L("<font color=Orange>$CTSYKnownFailure: defect id = 10006</font>"));
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0028VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0028VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0028MBDN::CCTSYIntegrationTestPhoneBookStore0028MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0028Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0028MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0028MBDN::~CCTSYIntegrationTestPhoneBookStore0028MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0028MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0028
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0028
 * @SYMTestCaseDesc Cancel deleting an entry from the phone book. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneBookStore::Delete
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{

	INFO_PRINTF1(_L("<font color=Cyan> Not applicable for MBDN</font>"));	
	StartCleanup();
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0028MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0028MBDN");
	}



CCTSYIntegrationTestPhoneBookStore0029ADN::CCTSYIntegrationTestPhoneBookStore0029ADN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0029Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0029ADN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0029ADN::~CCTSYIntegrationTestPhoneBookStore0029ADN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0029ADN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-ADN-0029
 * @SYMFssID BA/CTSY/PBSTR-ADN-0029
 * @SYMTestCaseDesc Cancel getting phone store information. (ADN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccAdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0029ADN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0029ADN");
	}



CCTSYIntegrationTestPhoneBookStore0029FDN::CCTSYIntegrationTestPhoneBookStore0029FDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0029Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0029FDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0029FDN::~CCTSYIntegrationTestPhoneBookStore0029FDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0029FDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-FDN-0029
 * @SYMFssID BA/CTSY/PBSTR-FDN-0029
 * @SYMTestCaseDesc Cancel getting phone store information. (FDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccFdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0029FDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0029FDN");
	}



CCTSYIntegrationTestPhoneBookStore0029SDN::CCTSYIntegrationTestPhoneBookStore0029SDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0029Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0029SDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0029SDN::~CCTSYIntegrationTestPhoneBookStore0029SDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0029SDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-SDN-0029
 * @SYMFssID BA/CTSY/PBSTR-SDN-0029
 * @SYMTestCaseDesc Cancel getting phone store information. (SDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{

	DoTestProcedureL(KIccSdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0029SDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0029SDN");
	}



CCTSYIntegrationTestPhoneBookStore0029VMBX::CCTSYIntegrationTestPhoneBookStore0029VMBX(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0029Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0029VMBX::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0029VMBX::~CCTSYIntegrationTestPhoneBookStore0029VMBX()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0029VMBX::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-VMBX-0029
 * @SYMFssID BA/CTSY/PBSTR-VMBX-0029
 * @SYMTestCaseDesc Cancel getting phone store information. (VMBX phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccVoiceMailBox);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0029VMBX::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0029VMBX");
	}



CCTSYIntegrationTestPhoneBookStore0029MBDN::CCTSYIntegrationTestPhoneBookStore0029MBDN(CEtelSessionMgr& aEtelSessionMgr)
: CCTSYIntegrationTestPhoneBookStore0029Base(aEtelSessionMgr)
/**
 * Constructor.
 */
{
SetTestStepName(CCTSYIntegrationTestPhoneBookStore0029MBDN::GetTestStepName());
}

CCTSYIntegrationTestPhoneBookStore0029MBDN::~CCTSYIntegrationTestPhoneBookStore0029MBDN()
/**
 * Destructor.
 */
{
}

TVerdict CCTSYIntegrationTestPhoneBookStore0029MBDN::doTestStepL()
/**
 * @SYMTestCaseID BA-CTSY-INT-PBSTR-MBDN-0029
 * @SYMFssID BA/CTSY/PBSTR-MBDN-0029
 * @SYMTestCaseDesc Cancel getting phone store information. (MBDN phone book)
 * @SYMTestPriority High
 * @SYMTestActions RMobilePhoneStore::GetInfo
 * @SYMTestExpectedResults Pass - KErrCancel returned.
 * @SYMTestType CIT
 * @SYMTestCaseDependencies live/automatic
 *
 * Reason for test: Verify KErrCancel returned.
 *
 * @return - TVerdict code
 */
	{
	DoTestProcedureL(KIccMbdnPhoneBook);
	return TestStepResult();
	}

TPtrC CCTSYIntegrationTestPhoneBookStore0029MBDN::GetTestStepName()
/**
 * @return The test step name.
 */
	{
	return _L("CCTSYIntegrationTestPhoneBookStore0029MBDN");
	}



