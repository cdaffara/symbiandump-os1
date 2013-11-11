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
// RPSCommon.h
// 
//

#include <e32uid.h>
#include "RpsCommon.h"
#include "rpsasciirqstdefs.h"
#include "rpsownnumbers.h"
#include "rpsloggermacros.h"
#include <etelmm.h>


_LIT(KVoiceTagU, "*Voice*");
_LIT(KVoiceTagL, "*voice*");
_LIT(KFaxTagU, "*Fax*");
_LIT(KFaxTagL, "*fax*");
_LIT(KDataTagU, "*Data*");
_LIT(KDataTagL, "*data*");

_LIT(KPlus,"+");
_LIT(KUKCountryCode, "44");


EXPORT_C CRpsMsg* CRpsMsg::NewLC(const TDesC8& aMsgType, const TDesC8& aOpCode, const TUint aMsgNum, const CDesCArrayFlat& aArgArray)
	{
	CRpsMsg* self = new (ELeave) CRpsMsg();
	CleanupStack::PushL(self);
	self->ConstructL(aMsgType, aOpCode, aMsgNum, aArgArray);
	return self;
	}


EXPORT_C CRpsMsg* CRpsMsg::NewL(const TDesC8& aMsgType, const TDesC8& aOpCode, const TUint aMsgNum, const CDesCArrayFlat& aArgArray)
	{
	CRpsMsg* self = CRpsMsg::NewLC(aMsgType, aOpCode, aMsgNum, aArgArray);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CRpsMsg* CRpsMsg::NewLC()
	{
	CRpsMsg* self = new (ELeave) CRpsMsg();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


EXPORT_C CRpsMsg* CRpsMsg::NewL()
	{
	CRpsMsg* self = CRpsMsg::NewLC();
	CleanupStack::Pop();
	return self;
	}
	
EXPORT_C CRpsMsg::~CRpsMsg() 
	{
	delete iMsgDataArray;
	iMsgDataArray = NULL;
	}


void CRpsMsg::ConstructL() 
	{
	iMsgDataArray = new (ELeave)CDesC8ArrayFlat(1);
	}
	
void CRpsMsg::ConstructL(const TDesC8& aMsgType, const TDesC8& aOpCode, const TUint aMsgNum, const CDesCArrayFlat& aArgArray) 
	{
	
	TUint numArgs(aArgArray.Count());
	iMsgDataArray = new (ELeave)CDesC8ArrayFlat(1);
	
	// Calc the total msg length
	TUint msgLen(KRqstLengthNoOpCodeNoArgsNoCommas + aOpCode.Length());
	for(TInt i(0); i < numArgs; ++i)
		{
		msgLen += aArgArray.MdcaPoint(i).Length();
		}
	// add commas to calculation
	msgLen += KRqstMsgElementsNoArgs + numArgs - 1;
	
	// Msg Type
	iMsgDataArray->AppendL(aMsgType);
	iMsgDataArray->AppendL(KComma);
	
	// Msg Len
	TBuf8<KMaxMsgLenChars> mlenString;
	mlenString.NumFixedWidth(msgLen, EDecimal, KMaxMsgLenChars);
	iMsgDataArray->AppendL(mlenString);
	iMsgDataArray->AppendL(KComma);
	
	// Msg Seq Num
	TBuf8<KMaxMsgNumChars> msnString;
	//msnString.Num(NextMsgNum());
	msnString.NumFixedWidth(aMsgNum, EDecimal, KMaxMsgNumChars);
	iMsgDataArray->AppendL(msnString);
	iMsgDataArray->AppendL(KComma);
	
	// OpCode
	iMsgDataArray->AppendL(aOpCode);
	iMsgDataArray->AppendL(KComma);
	
	// Arguments. Convert aArgArray data to 8-bit as we go.
	for(TInt i(0); i < numArgs; ++i)
		{
		TBuf8<KMaxElementSize> buf;
		buf.Copy(aArgArray.MdcaPoint(i));
		iMsgDataArray->AppendL(buf);
		iMsgDataArray->AppendL(KComma);
		}
		
	// EOM
	iMsgDataArray->AppendL(KEOM);
    }


// buf to array
EXPORT_C TInt CRpsMsg::InternalizeL(const TDesC8& aBufIn)
	{
	TInt length(aBufIn.Length());
	if(length <= 0)
		{
		return KErrCorrupt;
		}
		
	if(iMsgDataArray != NULL)
		iMsgDataArray->Reset();
	
	TBuf8<KMaxElementSize> buf;
	
	// Dismantle msg into data elements first 
	TBool stop(EFalse);
	while (stop == EFalse)
		{
		TPtrC8 ptr = aBufIn.Right(length);
		
		TInt offset = ptr.Find(KComma);
		
		if(offset == KErrNotFound)
			{
			if(length > 0)
				{
				buf.Copy(ptr);
				iMsgDataArray->AppendL(buf);
				break;
				}	
			}

		buf.Copy(ptr.Left(offset));
		iMsgDataArray->AppendL(buf);
		length -= ++offset;
		if(length <= 0)
			{
			stop = ETrue;
			}
		}
	
	return KErrNone;
	}
 
// array to buf
// aBufOut is an uninstantiated buffer - caller assumes ownership on return
EXPORT_C void CRpsMsg::ExternalizeL(HBufC8*& aBufOut)
	{
	TUint msgLen(0);
	for(TInt i(0); i < iMsgDataArray->MdcaCount(); ++i)
		{
		msgLen += iMsgDataArray->MdcaPoint(i).Length();
		}
		
	aBufOut = HBufC8::NewL(msgLen);
	TPtr8 ptr = aBufOut->Des();
	
	for(TInt i(0); i < iMsgDataArray->MdcaCount(); ++i)
		{
		ptr.Append(iMsgDataArray->MdcaPoint(i));
		}	
	}
	
EXPORT_C TPtrC8 CRpsMsg::GetRawDataL(TInt aOffset)
	{
	if(aOffset >= iMsgDataArray->Count())
		{
		User::Leave(KErrArgument);
		}
	return iMsgDataArray->MdcaPoint(aOffset);
	}
	
EXPORT_C TPtrC8 CRpsMsg::GetArgL(TInt aArgNo)
	{
	TInt offset = KArg1Offset + aArgNo -1;
	if(offset >= iMsgDataArray->Count())
		{
		User::Leave(KErrArgument);
		}
	return iMsgDataArray->MdcaPoint(offset);
	}
	
EXPORT_C TPtrC8 CRpsMsg::GetOpCode() const
	{
	return iMsgDataArray->MdcaPoint(KOpCodeOffset);	
	}
	
EXPORT_C TInt CRpsMsg::ArraySize() const
	{
	return iMsgDataArray->Count();
	}
	
EXPORT_C TPtrC8 CRpsMsg::GetMsgType() const
	{
	return iMsgDataArray->MdcaPoint(KRqstOffset);
	}
	
EXPORT_C TInt CRpsMsg::GetMsgNum() const
	{
	TInt msgNum;
	TPtrC8 msgNumTxt = iMsgDataArray->MdcaPoint(KMsgNumOffset);
	TLex8 lex(msgNumTxt);
	lex.Val(msgNum);
	return msgNum;
	}

	
EXPORT_C TInt TRPSOwnNumbers::RetrieveOwnNumbersL(RMobilePhone& aPhone)
	{
	
	RDEBUGPRINTLOGGER1(_L("RPS: TRPSOwnNumbers::RetrieveOwnNumbersL"));
	
	RMobileONStore ownNumberStore;
	User::LeaveIfError(ownNumberStore.Open(aPhone));
	CleanupClosePushL(ownNumberStore);
	
	// Get Own Number Information
	RMobileONStore::TMobileONStoreInfoV1 ownNumberInfo;
	RMobileONStore::TMobileONStoreInfoV1Pckg ownNumberInfoPckg(ownNumberInfo);

	TRequestStatus status;
	ownNumberStore.GetInfo(status, ownNumberInfoPckg);
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());

	//
	// Read entire Own Number store entries if supported
	//
	TInt errorReadingWhole = KErrNone;
	TInt errorsExtractANumber(0);

	if(ownNumberInfo.iCaps & (TUint32)RMobilePhoneStore::KCapsWholeStore)
		{
		// Reads phone's entire ON list
		CMobilePhoneONList* onList = NULL;
		
		// Assumes we have a scheduler to work with.
		CActiveScheduler* sched = CActiveScheduler::Current();
		
		CActiveONRetrieverRps* retriever = new(ELeave)CActiveONRetrieverRps(ownNumberStore, onList, errorReadingWhole, sched);
		CleanupStack::PushL(retriever);
		retriever->StartL();
		sched->Start();

		if(!errorReadingWhole)
			{
			// Verify that the name and number are OK.
			TInt numOfEntry = onList->Enumerate();
			for(TInt x = 0; x < numOfEntry; ++x)
				{		
				
				if(ExtractANumber(onList->GetEntryL(x)) != KErrNone)
					++errorsExtractANumber;
				}
			}
		delete onList;

		CleanupStack::PopAndDestroy();

		}

	TInt lenVoiceNum(iOwnVoiceNumber.Length());
	TInt lenFaxNum(iOwnFaxNumber.Length());
	TInt lenDataNum(iOwnDataNumber.Length());

	if(!(lenVoiceNum && lenFaxNum && lenDataNum))
		{
		// Try reading ON store one entry at a time
		for (TInt i = 1; i <= ownNumberInfo.iTotalEntries; i++)
			{
			RMobileONStore::TMobileONEntryV1 ownNumberEntry;
		
			ownNumberEntry.iIndex = i;
			RMobileONStore::TMobileONEntryV1Pckg ownNumberEntryPckg(ownNumberEntry);

			ownNumberStore.Read(status, ownNumberEntryPckg);
			User::WaitForRequest(status);
			
			ExtractANumber(ownNumberEntry);
			}

			// As the last resort, 
			// if the voice number has not been found, assume it is under the Unspecified service tag
			if(iOwnVoiceNumber.Length() == 0)
				{
				RMobileONStore::TMobileONEntryV1 ownNumberEntry;
				ownNumberEntry.iIndex = 1;
				RMobileONStore::TMobileONEntryV1Pckg ownNumberEntryPckg(ownNumberEntry);
				ownNumberStore.Read(status, ownNumberEntryPckg);
				User::WaitForRequest(status);
			
				if (ownNumberEntry.iService == RMobilePhone::EServiceUnspecified)
					{
					TBuf<1> prependstr;
					prependstr.Zero();
					if(ownNumberEntry.iNumber.iTypeOfNumber == RMobilePhone::EInternationalNumber || 
					   ownNumberEntry.iNumber.iTelNumber.Left(2) == KUKCountryCode)
						{
						prependstr = KPlus;
						}
					iOwnVoiceNumber = prependstr;
					iOwnVoiceNumber += ownNumberEntry.iNumber.iTelNumber;
					}
				}
		}

	CleanupStack::PopAndDestroy(); //ownNumberStore
	
	//often fax and data numbers are blank on the SIM so if that is the case then set them to be same as voice
	//Hopefully attempting to dial a data/fax call using the voice number should still work
	if(iOwnDataNumber.Length() == 0)
		iOwnDataNumber = iOwnVoiceNumber;
	
	if(iOwnFaxNumber.Length() == 0)
		iOwnFaxNumber = iOwnVoiceNumber;
	
	RDEBUGPRINTLOGGER2(_L("RPS: iOwnVoiceNumber=%S"), &iOwnVoiceNumber);
	RDEBUGPRINTLOGGER2(_L("RPS: iOwnDataNumber=%S"), &iOwnDataNumber);
	RDEBUGPRINTLOGGER2(_L("RPS: iOwnFaxNumber=%S"), &iOwnFaxNumber);

	return KErrNone;	
	}
	
EXPORT_C TRPSOwnNumbers::TRPSOwnNumbers()
	{
	iOwnVoiceNumber.Zero();
	iOwnDataNumber.Zero();
	iOwnFaxNumber.Zero();
	}

TInt TRPSOwnNumbers::ExtractANumber(const RMobileONStore::TMobileONEntryV1& aEntry)
	{
	TInt ret(KErrNotFound);
	
	TBuf<1> prependstr;
	prependstr.Zero();
	if(aEntry.iNumber.iTypeOfNumber == RMobilePhone::EInternationalNumber || 
	   aEntry.iNumber.iTelNumber.Left(2) == KUKCountryCode)
		{
		prependstr = KPlus;
		}
		
	// Check if entry's Service tag is set and extract number
	switch(aEntry.iService)
		{
		case RMobilePhone::EVoiceService:
			if(iOwnVoiceNumber.Length()==0)
				{					
				 iOwnVoiceNumber = prependstr;
				 iOwnVoiceNumber += aEntry.iNumber.iTelNumber;
			 	}
			ret = KErrNone;
			break;
		case RMobilePhone::ECircuitDataService:
			if(iOwnDataNumber.Length()==0)
				{
				iOwnDataNumber = prependstr;
				iOwnDataNumber += aEntry.iNumber.iTelNumber;
				}
			ret = KErrNone;
			break;
		case RMobilePhone::EFaxService:
			if(iOwnFaxNumber.Length()==0)
				{
				iOwnFaxNumber = prependstr;
				iOwnFaxNumber += aEntry.iNumber.iTelNumber;
				}
			ret = KErrNone;
			break;
			
		default:
			{
			// Else check the text tag, assuming the user has set the text tag according to the RPS
			// user guide
			if(iOwnVoiceNumber.Length()==0)
				{
				if(aEntry.iText.Match(KVoiceTagU)!= KErrNotFound || aEntry.iText.Match(KVoiceTagL)!= KErrNotFound)
					{
					// Assume this is the voice line
					iOwnVoiceNumber = prependstr;
				 	iOwnVoiceNumber += aEntry.iNumber.iTelNumber;
					ret = KErrNone;
					}
				}
				
			if(iOwnFaxNumber.Length()==0)
				{
				if(aEntry.iText.Match(KFaxTagU)!= KErrNotFound || aEntry.iText.Match(KFaxTagL)!= KErrNotFound)
					{
					// Assume this is the fax line
					iOwnFaxNumber = prependstr;
					iOwnFaxNumber += aEntry.iNumber.iTelNumber;		
					ret = KErrNone;
					}
				}
					
			if(iOwnDataNumber.Length()==0)
				{
				if(aEntry.iText.Match(KDataTagU)!= KErrNotFound || aEntry.iText.Match(KDataTagL)!= KErrNotFound)
					{
					// Assume this is the data line
					iOwnDataNumber = prependstr;
					iOwnDataNumber += aEntry.iNumber.iTelNumber;
					ret = KErrNone;
					}
				}					
			}
			break;
		}
	return ret;
	}


//
// CActiveONRetrieverRps Methods
//
CActiveONRetrieverRps::CActiveONRetrieverRps(RMobileONStore& aONStore,
									   		CMobilePhoneONList*& aONList,
									   		TInt& aError, 
									   		CActiveScheduler*& aScheduler
									   		)
	: CActive(EPriorityNormal), iONStore(aONStore), iONList(aONList), iError(aError), iScheduler(aScheduler)
	{
	iScheduler->Add(this);
	}

CActiveONRetrieverRps::~CActiveONRetrieverRps()
	{
	delete iRetriever;
	}

void CActiveONRetrieverRps::StartL()
	{
	// Start retrieving all the own numbers.
	iRetriever = CRetrieveMobilePhoneONList::NewL(iONStore);
	iRetriever->Start(iStatus);
	SetActive();
	}

void CActiveONRetrieverRps::RunL()
	{

	// Get the own numbers into a list.
	if (iStatus == KErrNone)
		iONList = iRetriever->RetrieveListL();

	iError = iStatus.Int();

	delete iRetriever;
	iRetriever = NULL;
	
	iScheduler->Stop();
	}

void CActiveONRetrieverRps::DoCancel()
	{
	iRetriever->Cancel();
	}


	
