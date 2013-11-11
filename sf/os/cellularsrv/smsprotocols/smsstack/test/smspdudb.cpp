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


#include "smspdudb.h"
#include <testconfigfileparser.h>
#include "Gsmumsg.h"
#include "gsmubuf.h"

const TInt KSmsPduDbMaxReadWriteChunk = 0x200;

EXPORT_C CSmsPduDatabase* CSmsPduDatabase::NewL(RFs& aFs)
/**
 *  CSmsPduDatabase default factory function
 *  
 *  Calls the other overload of NewL with the default section, filename and component.
 */
	{
	return NewL(aFs, KSmsPduDbDefaultSection, KSmsPduDbDefaultFilename, KSmsPduDbDefaultComponent);
	}

EXPORT_C CSmsPduDatabase* CSmsPduDatabase::NewL(RFs& aFs, const TDesC8& aSection, const TDesC& aFileName, const TDesC& aComponent)
	{
	CSmsPduDatabase* self = new (ELeave) CSmsPduDatabase(aFs);
	CleanupStack::PushL(self);

	//Construct iConfigFile and set iSection to aSection
	CTestConfig*  testConfig = CTestConfig::NewLC(aFs, aComponent, aFileName);
	CleanupStack::Pop(testConfig);

	self->iConfigFile = testConfig;
	self->SetSectionL(aSection);

	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSmsPduDatabase* CSmsPduDatabase::NewL(RFs& aFs, const CTestConfigSection& aSection)
/**
 *  Constructs a CSmsPduDatabase and sets iSection to aSection.
 *  @note iConfigFile is not constructed and should remain NULL
 */
	{
	CSmsPduDatabase* self = new (ELeave) CSmsPduDatabase(aFs);
	self->SetSection(aSection);
	return self;
	}

EXPORT_C CSmsPduDatabase::~CSmsPduDatabase()
	{
	delete iConfigFile;
	// @note Don't delete iSection because it is either owned by iConfigFile or by another config file
	}

TPtrC8 CSmsPduDatabase::GetTypeL(CSmsPDU::TSmsPDUType aType) const
	{
	TPtrC8 type;

	switch (aType)
		{
		case CSmsPDU::ESmsSubmit:

			type.Set(KSmsPduDbSubmit);
			break;

		case CSmsPDU::ESmsDeliver:

			type.Set(KSmsPduDbDeliver);
			break;

		case CSmsPDU::ESmsCommand:

			type.Set(KSmsPduDbCommand);
			break;

		case CSmsPDU::ESmsStatusReport:

			type.Set(KSmsPduDbStatusReport);
			break;

		case CSmsPDU::ESmsSubmitReport:

			type.Set(KSmsPduDbSubmitReport);
			break;

		case CSmsPDU::ESmsDeliverReport:

			type.Set(KSmsPduDbDeliverReport);
			break;

		default:

			User::Leave(KErrNotSupported);
			break;
		}

	return type;
	}

TPtrC8 CSmsPduDatabase::GetConcatTypeL(CSmsPDU::TSmsPDUType aType) const
	{
	TPtrC8 type;

	switch (aType)
		{
		case CSmsPDU::ESmsSubmit:

			type.Set(KSmsPduDbSubmitConcat);
			break;

		case CSmsPDU::ESmsDeliver:

			type.Set(KSmsPduDbDeliverConcat);
			break;

		default:

			User::Leave(KErrNotSupported);
			break;
		}

	return type;
	}


EXPORT_C CSmsPduDbPdu* CSmsPduDatabase::GetPduLC(CSmsPDU::TSmsPDUType aType, TInt aId) const
	{
	const TPtrC8 type(GetTypeL(aType));
	RSmsPduDbIdArray array(type);
	CleanupClosePushL(array);
	GetIdsL(array, aId);

	const CTestConfigItem& item = *array[0].iItem;

	CSmsPduDbPdu* pdu = CSmsPduDbPdu::NewL(iFs, item, aType);
	CleanupStack::PopAndDestroy(&array);
	CleanupStack::PushL(pdu);
	return pdu;
	}

EXPORT_C void CSmsPduDatabase::GetPduL(RPointerArray<CSmsPduDbPdu>& aArray, CSmsPDU::TSmsPDUType aType) const
	{
	const TPtrC8 type(GetTypeL(aType));
	RSmsPduDbIdArray array(type);
	CleanupClosePushL(array);

	GetIdsL(array);
	const TInt count = array.Count();

	for (TInt i = 0; i < count; i++)
		{
		CSmsPduDbPdu* pdu = CSmsPduDbPdu::NewL(iFs, *array[i].iItem, aType);
		CleanupStack::PushL(pdu);
		User::LeaveIfError(aArray.Append(pdu));
		CleanupStack::Pop(pdu);
		}

	CleanupStack::PopAndDestroy(&array);
	}

EXPORT_C CSmsPduDbConcatSegment* CSmsPduDatabase::GetConcatSegmentLC(CSmsPDU::TSmsPDUType aType, TInt aId, TInt aSegment) const
	{
	const TPtrC8 type(GetConcatTypeL(aType));
	RSmsPduDbIdArray array(type, ETrue);
	CleanupClosePushL(array);
	GetIdsL(array, aId);

	CSmsPduDbConcatSegment* concat = GetConcatSegmentL(array, aType, aId, aSegment);

	CleanupStack::PopAndDestroy(&array);
	CleanupStack::PushL(concat);
	return concat;
	}

CSmsPduDbConcatSegment* CSmsPduDatabase::GetConcatSegmentL(const RSmsPduDbIdArray& aIdArray, CSmsPDU::TSmsPDUType aType, TInt aId, TInt aSegment) const
	{
	const TInt count = aIdArray.Count();
	CSmsPduDbConcatSegment* concat = NULL;

	for (TInt i = 0; i < count; i++) //order important
		{
		const TSmsPduDbId& id = aIdArray[i];

		if (id.iId == aId)
			{
			const TDesC8& val = id.iItem->Value();
			TInt segment = KErrNotFound;
			const TInt err = CTestConfig::GetElement(val, KSmsPduDbDelimiter, CSmsPduDbConcatSegment::EConcatSegment, segment);

			if (err == KErrNone && segment == aSegment)
				{
				concat = CSmsPduDbConcatSegment::NewL(iFs, *id.iItem, aType);
				break;
				}
			}
		}

	if (concat == NULL)
		User::Leave(KErrNotFound);

	return concat;
	}

EXPORT_C CSmsPduDbConcat* CSmsPduDatabase::GetConcatLC(CSmsPDU::TSmsPDUType aType, TInt aId) const
	{
	const TPtrC8 type(GetConcatTypeL(aType));
	RSmsPduDbIdArray array(type, ETrue);
	CleanupClosePushL(array);
	GetIdsL(array, aId);

	CSmsPduDbConcat* concat = new (ELeave) CSmsPduDbConcat();
	CleanupStack::PushL(concat);

	const TInt count = array.Count();

	for (TInt i=0; i<count; i++)
		{
		const TSmsPduDbId& id = array[i];
		CSmsPduDbConcatSegment* concatSegment = CSmsPduDbConcatSegment::NewL(iFs, *id.iItem, aType);
		CleanupStack::PushL(concatSegment);
		User::LeaveIfError(concat->iSegments.Append(concatSegment));
		}

	concat->DecodeL(iFs);

	CleanupStack::PopAndDestroy(&array);
	return concat;
	}

EXPORT_C void CSmsPduDatabase::GetConcatL(RPointerArray<CSmsPduDbConcat>& aArray, CSmsPDU::TSmsPDUType aType) const
	{
	const TPtrC8 type(GetConcatTypeL(aType));
	RSmsPduDbIdArray array(type, ETrue);
	CleanupClosePushL(array);
	GetIdsL(array);

	const TInt count = array.Count();
	TInt lastId = KErrNotFound;
	CSmsPduDbConcat* concat = NULL;

	for (TInt i=0; i<count; i++)
		{
		const CTestConfigItem& item = *array[i].iItem;
		const TInt id = array[i].iId;

		if (id != lastId || concat == NULL)
			{
			if (concat != NULL)
				concat->DecodeL(iFs);

			concat = new (ELeave) CSmsPduDbConcat();
			CleanupStack::PushL(concat);
			User::LeaveIfError(aArray.Append(concat));
			CleanupStack::Pop(concat);
			lastId = id;
			}

		CSmsPduDbConcatSegment* segment = CSmsPduDbConcatSegment::NewL(iFs, item, aType);
		CleanupStack::PushL(segment);
		User::LeaveIfError(concat->iSegments.Append(segment));
		CleanupStack::Pop(segment);
		}

	if (concat != NULL)
		concat->DecodeL(iFs);

	CleanupStack::PopAndDestroy(&array);
	}

EXPORT_C CSmsPduDbMessage* CSmsPduDatabase::GetMessageLC(CSmsPDU::TSmsPDUType aType, TInt aId) const
	{
	RSmsPduDbIdArray array(KSmsPduDbMessage);
	CleanupClosePushL(array);
	GetIdsL(array, aId);

	const CTestConfigItem& item = *array[0].iItem;
	CSmsPduDbMessage* pdu = CSmsPduDbMessage::NewL(iFs, item, aType);
	CleanupStack::PushL(pdu);


	CleanupStack::PopAndDestroy(&array);
	return pdu;
	}

EXPORT_C void CSmsPduDatabase::GetMessageL(RPointerArray<CSmsPduDbMessage>& aArray, CSmsPDU::TSmsPDUType aType) const
	{
	RSmsPduDbIdArray array(KSmsPduDbMessage);
	CleanupClosePushL(array);
	GetIdsL(array);

	const TInt count = array.Count();

	for (TInt i=0; i<count; i++)
		{
		const CTestConfigItem& item = *array[i].iItem;
		CSmsPduDbMessage* pdu = CSmsPduDbMessage::NewL(iFs, item, aType);
		CleanupStack::PushL(pdu);
		User::LeaveIfError(aArray.Append(pdu));
		CleanupStack::Pop(pdu);
		}

	CleanupStack::PopAndDestroy(&array);
	}

CSmsPduDatabase::CSmsPduDatabase(RFs& aFs)
: iFs(aFs)
	{
	}

void CSmsPduDatabase::GetIdsL(RSmsPduDbIdArray& aIds) const
	{
	RPointerArray<const CTestConfigItem> array;
	CleanupClosePushL(array);

	iSection->ItemsL(array, aIds.Type());

	const TInt count = array.Count();

	for (TInt i=0; i<count; i++)
		{
		TSmsPduDbId item(array[i]);
		const TInt err = CTestConfig::GetElement(item.iItem->Value(), KSmsPduDbDelimiter, CSmsPduDbBase::ESmsPduDbId, item.iId);
		if (err == KErrNone)
			{
			aIds.InsertL(item);
			}
		}

	CleanupStack::PopAndDestroy(&array);
	}

void CSmsPduDatabase::GetIdsL(RSmsPduDbIdArray& aIds, TInt aId) const
	{
	RSmsPduDbIdArray tempArray(aIds.Type(), ETrue);
	CleanupClosePushL(tempArray);

	GetIdsL(tempArray);

	TInt find = KErrNotFound;

	while ((find = tempArray.Find(aId)) != KErrNotFound)
		{
		User::LeaveIfError(aIds.Append(tempArray[find]));
		tempArray.Remove(find);
		}

	CleanupStack::PopAndDestroy(&tempArray);
	if (aIds.Count() == 0)
		User::Leave(KErrNotFound);
	}

void CSmsPduDatabase::ReadFileL(const TDesC& aInputFileName, const TDesC& aInputComponent, CBufFlat& aData) const
	{
	TParse parse;
	User::LeaveIfError(CTestConfig::ResolveFile(iFs, aInputComponent, aInputFileName, parse));

	RFile file;
	User::LeaveIfError(file.Open(iFs, parse.FullName(), EFileRead));
	CleanupClosePushL(file);

	TInt size(0);
	User::LeaveIfError(file.Size(size));

	for (TInt i = 0; i < size; i += KSmsPduDbMaxReadWriteChunk)
		{
		const TInt readSize = Min(KSmsPduDbMaxReadWriteChunk, size - i);
		TBuf8<KSmsPduDbMaxReadWriteChunk> read;
		User::LeaveIfError(file.Read(read, readSize));
		const TInt pos = aData.Size();
		aData.InsertL(pos, read);
		}

	CleanupStack::PopAndDestroy(&file);
	}

void CSmsPduDatabase::WriteFileL(const TDesC& aOutputFileName, const CBufFlat& aData) const
	{
	RFile file;
	User::LeaveIfError(file.Replace(iFs, aOutputFileName, EFileWrite));
	CleanupClosePushL(file);

	const TInt size = aData.Size();

	for (TInt i = 0; i < size; i += KSmsPduDbMaxReadWriteChunk)
		{
		const TInt readSize = Min(KSmsPduDbMaxReadWriteChunk, size - i);
		TBuf8<KSmsPduDbMaxReadWriteChunk> read;
		aData.Read(i, read, readSize);
		User::LeaveIfError(file.Write(read));
		}

	User::LeaveIfError(file.Flush());

	CleanupStack::PopAndDestroy(&file);
	}

EXPORT_C void CSmsPduDatabase::RewriteFileL(const TDesC& aInputFileName, const TDesC& aInputComponent, const TDesC& aOutputFileName) const
	{
	CBufFlat* buffer = CBufFlat::NewL(0x100); //< TODO Remove this magic number
	CleanupStack::PushL(buffer);

	ReadFileL(aInputFileName, aInputComponent, *buffer);

	ParseFileL(*buffer);

	WriteFileL(aOutputFileName, *buffer);

	CleanupStack::PopAndDestroy(buffer);
	}

void CSmsPduDatabase::ParseFileL(CBufFlat& aData) const
	{
	TPtrC8 ptr(aData.Ptr(0));
	TLex8 lex(ptr);
	lex.Mark();

	while (!lex.Eos())
		{
		if (lex.Peek() == KSmsPduDbTagStart)
			{
			ParseTagL(aData, ptr, lex);
			}
		else
			{
			lex.Inc();
			}
		}
	}

void CSmsPduDatabase::ParseTagL(CBufFlat& aData, TPtrC8& aPtr, TLex8& aLex) const
	{
	const TInt startingPos = aLex.Offset();

	const TPtrC8 remainder(aLex.Remainder());
	const TInt locate = remainder.Locate(KSmsPduDbTagEnd);
	TInt err = locate;

	if (locate != KErrNotFound)
		{
		aLex.Mark();
		aLex.Inc(locate+1);

		const TPtrC8 marked(aLex.MarkedToken());
		CSmsPduDbPdu* pdu = NULL;

		TRAP(err, pdu = PduFactoryL(marked));

		if (err == KErrNone)
			{
			CleanupStack::PushL(pdu);
			TBuf8<RMobileSmsMessaging::KGsmTpduSize*2> hexPdu;
			pdu->GetHexPdu(hexPdu);
			aData.Delete(startingPos, marked.Length());
			aData.InsertL(startingPos, hexPdu);
			aPtr.Set(aData.Ptr(0));
			aLex = aPtr;
			aLex.Inc(startingPos + pdu->iPdu.Length());
			CleanupStack::PopAndDestroy(pdu);
			}
		}

	if (err != KErrNone)
		{
		aLex.Inc();
		}
	}

TBool CSmsPduDatabase::IsPdu(const TDesC8& aTag, CSmsPDU::TSmsPDUType& aType) const
	{
	TBool ret = EFalse;

	if (aTag.CompareF(KSmsPduDbSubmit) == KErrNone)
		{
		ret = ETrue;
		aType = CSmsPDU::ESmsSubmit;
		}
	else if (aTag.CompareF(KSmsPduDbDeliver) == KErrNone)
		{
		ret = ETrue;
		aType = CSmsPDU::ESmsDeliver;
		}
	else if (aTag.CompareF(KSmsPduDbStatusReport) == KErrNone)
		{
		ret = ETrue;
		aType = CSmsPDU::ESmsStatusReport;
		}
	else if (aTag.CompareF(KSmsPduDbCommand) == KErrNone)
		{
		ret = ETrue;
		aType = CSmsPDU::ESmsCommand;
		}
	else if (aTag.CompareF(KSmsPduDbSubmitReport) == KErrNone)
		{
		ret = ETrue;
		aType = CSmsPDU::ESmsSubmitReport;
		}
	else if (aTag.CompareF(KSmsPduDbDeliverReport) == KErrNone)
		{
		ret = ETrue;
		aType = CSmsPDU::ESmsDeliverReport;
		}

	return ret;
	}

TBool CSmsPduDatabase::IsConcatSegment(const TDesC8& aTag, CSmsPDU::TSmsPDUType& aType) const
	{
	TBool ret = EFalse;

	if (aTag.CompareF(KSmsPduDbSubmitConcat) == KErrNone)
		{
		ret = ETrue;
		aType = CSmsPDU::ESmsSubmit;
		}
	else if (aTag.CompareF(KSmsPduDbDeliverConcat) == KErrNone)
		{
		ret = ETrue;
		aType = CSmsPDU::ESmsDeliver;
		}

	return ret;
	}

EXPORT_C CSmsPduDbPdu* CSmsPduDatabase::PduFactoryL(const CTestConfigItem& aItem, CSmsPDU::TSmsPDUType aTypeForMessageTags) const
	{
	const TDesC8& type = aItem.Item();
	CSmsPduDbPdu* pdu = NULL;
	CSmsPDU::TSmsPDUType pduType;

	if (IsPdu(type, pduType))
		{
		pdu = CSmsPduDbPdu::NewL(iFs, aItem, pduType);
		}
	else if (IsConcatSegment(type, pduType))
		{
		pdu = CSmsPduDbConcatSegment::NewL(iFs, aItem, pduType);
		}
	else if (IsMessage(type))
		{
		pdu = CSmsPduDbMessage::NewL(iFs, aItem, aTypeForMessageTags);
		}
	else
		{
		//Attempt to parse aItem.Value() using the other overload of PduFactoryL()
		pdu = PduFactoryL(aItem.Value(), aTypeForMessageTags);
		}

	__ASSERT_DEBUG(pdu != NULL, PduDbPanic(EPduDbPanicPduNotConstructed));
	return pdu;
	}

EXPORT_C CSmsPduDbPdu* CSmsPduDatabase::PduFactoryL(const TDesC8& aTag, CSmsPDU::TSmsPDUType aTypeForMessageTags) const
/**
 *  Parses tags of the form <MESSAGE_TYPE; ID; {SEGMENT}>, e.g. <SUBMIT,21> or <MESSAGE,2> or <SUBMITCONCAT; 32; 2>
 *  If the MESSAGE_TYPE and ID combination if found in the PDU database then a CSmsPduDbPdu-derived object is created.
 *  SEGMENT is only compulsory if MESSAGE_TYPE is a concatenated message.
 *  
 *  @param aTag Tag of the form <MESSAGE_TYPE; ID; {SEGMENT}>. MESSAGE_TYPE and ID are compulsory and must be delimited by a semicolon (;)
 *  @param aTypeForMessageTags PDU Type used when creating a CSmsPduDbMessage. This occurs if MESSAGE_TYPE == "message"
 */
	{
	TPtrC8 tag(CTestConfig::Trim(aTag));
	const TInt len = tag.Length();

	if (len < 4)
		User::Leave(KErrBadName);

	TInt midStart = 0;
	TInt midLen = len;
	if (tag[0] == KSmsPduDbTagStart)
		{
		midStart++;
		midLen--;
		}

	if (tag[len-1] == KSmsPduDbTagEnd)
		midLen--;

	tag.Set(tag.Mid(midStart, midLen));

	TPtrC8 type;
	TInt id(0);

	User::LeaveIfError(CTestConfig::GetElement(tag, KSmsPduDbTagDelimitier, ETagType, type));
	User::LeaveIfError(CTestConfig::GetElement(tag, KSmsPduDbTagDelimitier, ETagId, id));

	CSmsPduDbPdu* pdu = NULL;
	CSmsPDU::TSmsPDUType pduType;

	if (IsPdu(type, pduType))
		{
		pdu = GetPduLC(pduType, id);
		}
	else if (IsConcatSegment(type, pduType))
		{
		TInt segment;
		User::LeaveIfError(CTestConfig::GetElement(tag, KSmsPduDbTagDelimitier, ETagSegment, segment));
		pdu = GetConcatSegmentLC(pduType, id, segment);
		}
	else if (IsMessage(type))
		{
		pdu = GetMessageLC(aTypeForMessageTags, id);
		}
	else
		{
		User::Leave(KErrBadName);
		}

	__ASSERT_DEBUG(pdu != NULL, PduDbPanic(EPduDbPanicPduNotConstructed));
	CleanupStack::Pop(pdu);
	return pdu;
	}

EXPORT_C CSmsPduDbPdu* CSmsPduDatabase::PduFactory(const TDesC8& aTag, CSmsPDU::TSmsPDUType aTypeForMessageTags) const
	{
	CSmsPduDbPdu* pdu = NULL;
	TRAPD(err, (pdu = PduFactoryL(aTag, aTypeForMessageTags)));
	return ((err == KErrNone) ? pdu : NULL);
	}

EXPORT_C CSmsPduDbPdu* CSmsPduDatabase::PduFactory(const CTestConfigItem& aItem, CSmsPDU::TSmsPDUType aTypeForMessageTags) const
	{
	CSmsPduDbPdu* pdu = NULL;
	TRAPD(err, (pdu = PduFactoryL(aItem, aTypeForMessageTags)));
	return ((err == KErrNone) ? pdu : NULL);
	}

TInt CSmsPduDatabase::RSmsPduDbIdArray::Compare(const TSmsPduDbId& aLeft, const TSmsPduDbId& aRight)
	{
	return aLeft.iId - aRight.iId;
	}

TInt CSmsPduDatabase::RSmsPduDbIdArray::Find(TInt aId) const
	{
	TLinearOrder<TSmsPduDbId> order(Compare);
	const TSmsPduDbId tempId(NULL, aId);
	return FindInOrder(tempId, order);
	}

void CSmsPduDatabase::RSmsPduDbIdArray::InsertL(const TSmsPduDbId& aId)
	{
	TLinearOrder<TSmsPduDbId> order(Compare);

	if (iAllowDuplicates)
		User::LeaveIfError(InsertInOrderAllowRepeats(aId, order));
	else
		User::LeaveIfError(InsertInOrder(aId, order));
	}
