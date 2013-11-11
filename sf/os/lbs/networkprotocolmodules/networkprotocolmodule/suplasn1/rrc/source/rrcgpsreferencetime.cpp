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
// Implementation of encoding and decoding GPS Reference Time
// 
//

#include "rrcgpsreferencetime.h"
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsassistancereferencetime.h>


const TInt KMaxSat=16;

CRrcGpsTowAssistContent* CRrcGpsTowAssistContent::NewL()
	{
	CRrcGpsTowAssistContent* self = CRrcGpsTowAssistContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsTowAssistContent* CRrcGpsTowAssistContent::NewLC()
	{
	CRrcGpsTowAssistContent* self = new (ELeave) CRrcGpsTowAssistContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsTowAssistContent::~CRrcGpsTowAssistContent()
	{
	}

CRrcGpsTowAssistContent::CRrcGpsTowAssistContent()
	{
	}




CRrcGpsTowAssist* CRrcGpsTowAssist::NewL(CRrcGpsTowAssistContent* aTowAssistContent)
	{
	CRrcGpsTowAssist* self = CRrcGpsTowAssist::NewLC(aTowAssistContent);
	CleanupStack::Pop(self);
	return self;					
	}
CRrcGpsTowAssist* CRrcGpsTowAssist::NewLC(CRrcGpsTowAssistContent* aTowAssistContent)
	{
	CRrcGpsTowAssist* self = new (ELeave) CRrcGpsTowAssist(aTowAssistContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsTowAssist::~CRrcGpsTowAssist()
	{
	}

CRrcGpsTowAssist::CRrcGpsTowAssist(CRrcGpsTowAssistContent* aTowAssistContent)
:iTowAssistContent(aTowAssistContent)
	{
	}

void CRrcGpsTowAssist::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcGpsTowAssist::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//GPS-TOW-Assist ::=					SEQUENCE {
	//	satID								SatID,
	//	tlm-Message							BIT STRING (SIZE (14)),
	//	tlm-Reserved						BIT STRING (SIZE (2)),
	//	alert								BOOLEAN,
	//	antiSpoof							BOOLEAN
	// SatID ::=							INTEGER (0..63)

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" Bit
	// Decode SatID
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						63,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iTowAssistContent->iSatID);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	// Decode tlm-Message
	RBuf8 stringData;
	TInt stringLen(0);
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						14,
						14,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);

	iTowAssistContent->iTlmMessage=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	// Decode tlm-Reserved
	CAsn1PerBitString::DecodeL(aBufferToDecode,
						startPos,
						KAsn1LengthNonVariable,
						KAsn1PerMaxConstrained,
						2,
						2,	
						KAsn1PerNonExtensible,
						isExtended,
						bitsDecoded, 
						stringData,
						stringLen);

	iTowAssistContent->iTlmReserved=stringData;
	stringData.Close();
	aDecodedBits+=bitsDecoded;	
	startPos+=bitsDecoded;
	
	//Decode alert
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iTowAssistContent->iAlert);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	//Decode antiSpoof
	CAsn1PerBoolean::DecodeL(aBufferToDecode,
							KAsn1PerNoPadding, 
							startPos, 
							bitsDecoded, 
							iTowAssistContent->iAntiSpoof);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	}







CRrcGpsTowAssistListContent* CRrcGpsTowAssistListContent::NewL()
	{
	CRrcGpsTowAssistListContent* self = CRrcGpsTowAssistListContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsTowAssistListContent* CRrcGpsTowAssistListContent::NewLC()
	{
	CRrcGpsTowAssistListContent* self = new (ELeave) CRrcGpsTowAssistListContent();
	CleanupStack::PushL(self);
	//self->ConstructL();
	return self;
	}

CRrcGpsTowAssistListContent::~CRrcGpsTowAssistListContent()
	{
	iGpsTowAssistArrayContent.ResetAndDestroy();
	}

CRrcGpsTowAssistListContent::CRrcGpsTowAssistListContent()
	{
	}


void CRrcGpsTowAssistListContent::ConstructL()
	{
	}


CRrcGpsTowAssistList* CRrcGpsTowAssistList::NewL(CRrcGpsTowAssistListContent* aTowAssistListContent)
	{
	CRrcGpsTowAssistList* self = CRrcGpsTowAssistList::NewLC(aTowAssistListContent);
	CleanupStack::Pop(self);
	return self;					
	}
CRrcGpsTowAssistList* CRrcGpsTowAssistList::NewLC(CRrcGpsTowAssistListContent* aTowAssistListContent)
	{
	CRrcGpsTowAssistList* self = new (ELeave) CRrcGpsTowAssistList(aTowAssistListContent);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcGpsTowAssistList::~CRrcGpsTowAssistList()
	{
	iGpsTowAssistArray.ResetAndDestroy();
	}

CRrcGpsTowAssistList::CRrcGpsTowAssistList(CRrcGpsTowAssistListContent* aTowAssistListContent)
:iGpsTowAssistArrayContent(aTowAssistListContent)
	{
	}

void CRrcGpsTowAssistList::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcGpsTowAssistList::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//GPS-TOW-AssistList ::=				SEQUENCE (SIZE (1..maxSat)) OF
	//										GPS-TOW-Assist
											
	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	TInt size;
	// Decode Squence of GPS-TOW-Assist
	CAsn1PerSequenceof::DecodeL(aBufferToDecode,
						startPos,
						KAsn1PerMinConstrained,
						1,
						KAsn1PerMaxConstrained,
						KMaxSat,
						KAsn1PerNonExtensible,
						bitsDecoded,
						size,
						isExtended);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	
	
	// Decode each GPS-TOW-Assist in the list
	for (TInt i=0;i<size;i++)
		{
		CRrcGpsTowAssistContent* content=CRrcGpsTowAssistContent::NewLC();
		iGpsTowAssistArrayContent->iGpsTowAssistArrayContent.AppendL(content);
		CleanupStack::Pop(content);
		CRrcGpsTowAssist* towAssist=CRrcGpsTowAssist::NewLC(content);
		iGpsTowAssistArray.AppendL(towAssist);
		CleanupStack::Pop(towAssist);
		towAssist->DecodeFromL(aBufferToDecode, startPos,bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;
		}
	}

void CRrcGpsTowAssistList::ConstructL()
	{
	}

void CRrcGpsTowAssistList::WriteLBSAssistanceDataL(RUEPositioningGpsReferenceTimeBuilder* aGpsReferenceTimeBuilder)
	{
	
	RUtranGpsReferenceTimeBuilder utranGpsReferenceTimeBuilder;
	aGpsReferenceTimeBuilder->GetFieldBuilder(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime, utranGpsReferenceTimeBuilder);


	RGpsTowAssistArrayBuilder towAssistArrayBuilder;
	aGpsReferenceTimeBuilder->GetArrayBuilder(TUEPositioningGpsReferenceTime::EGpsTowAssistArray, towAssistArrayBuilder);

	TInt count=iGpsTowAssistArrayContent->iGpsTowAssistArrayContent.Count();
	for (TInt i=0;i<count;i++)
		{
		RGpsTowAssistBuilder towAssistBuilder;	
		towAssistArrayBuilder.GetFieldBuilder(i, towAssistBuilder);
		towAssistBuilder.SetField(TGpsTowAssist::ESatID, (TUint)iGpsTowAssistArrayContent->iGpsTowAssistArrayContent[i]->iSatID);
		TUint tlmMessage=iGpsTowAssistArrayContent->iGpsTowAssistArrayContent[i]->iTlmMessage[0]*256+iGpsTowAssistArrayContent->iGpsTowAssistArrayContent[i]->iTlmMessage[1];
		towAssistBuilder.SetField(TGpsTowAssist::ETlmMessage, tlmMessage);
		
		//TODO: Bool is not working now, so comment it in when it is fixed.
		//towAssistBuilder.SetField(TGpsTowAssist::EAlert, (TBool)iGpsTowAssistArrayContent->iGpsTowAssistArrayContent[i]->iAlert);
		//towAssistBuilder.SetField(TGpsTowAssist::EAntiSpoof, (TBool)iGpsTowAssistArrayContent->iGpsTowAssistArrayContent[i]->iAntiSpoof);
		}
	}



CRrcUtranGpsTimingOfCellContent* CRrcUtranGpsTimingOfCellContent::NewL()
	{
	CRrcUtranGpsTimingOfCellContent* self = CRrcUtranGpsTimingOfCellContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcUtranGpsTimingOfCellContent* CRrcUtranGpsTimingOfCellContent::NewLC()
	{
	CRrcUtranGpsTimingOfCellContent* self = new (ELeave) CRrcUtranGpsTimingOfCellContent();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcUtranGpsTimingOfCellContent::~CRrcUtranGpsTimingOfCellContent()
	{
	}

CRrcUtranGpsTimingOfCellContent::CRrcUtranGpsTimingOfCellContent()
	{
	}


void CRrcUtranGpsTimingOfCellContent::ConstructL()
	{
	}





CRrcUtranGpsTimingOfCell* CRrcUtranGpsTimingOfCell::NewL(CRrcUtranGpsTimingOfCellContent* aUtranGpsTimingOfCellContent)
	{
	CRrcUtranGpsTimingOfCell* self = CRrcUtranGpsTimingOfCell::NewLC(aUtranGpsTimingOfCellContent);
	CleanupStack::Pop(self);
	return self;					
	}
CRrcUtranGpsTimingOfCell* CRrcUtranGpsTimingOfCell::NewLC(CRrcUtranGpsTimingOfCellContent* aUtranGpsTimingOfCellContent)
	{
	CRrcUtranGpsTimingOfCell* self = new (ELeave) CRrcUtranGpsTimingOfCell(aUtranGpsTimingOfCellContent);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcUtranGpsTimingOfCell::~CRrcUtranGpsTimingOfCell()
	{
	}

CRrcUtranGpsTimingOfCell::CRrcUtranGpsTimingOfCell(CRrcUtranGpsTimingOfCellContent* aUtranGpsTimingOfCellContent)
:iUtranGpsTimingOfCellContent(aUtranGpsTimingOfCellContent)
	{
	}

void CRrcUtranGpsTimingOfCell::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcUtranGpsTimingOfCell::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// 	utran-GPSTimingOfCell			SEQUENCE {
		// 		ms-part 						INTEGER (0..1023),
		// 		ls-part							INTEGER (0..4294967295)
		// 	},

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" Bit
	// Decode ms-part
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						1023,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iUtranGpsTimingOfCellContent->iMsPart);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode ls-part
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						4294967295LL,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iUtranGpsTimingOfCellContent->iLsPart);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	
	}


void CRrcUtranGpsTimingOfCell::ConstructL()
	{
	
	}









CRrcFddModeSpecificInfoContent* CRrcFddModeSpecificInfoContent::NewL()
	{
	CRrcFddModeSpecificInfoContent* self = CRrcFddModeSpecificInfoContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcFddModeSpecificInfoContent* CRrcFddModeSpecificInfoContent::NewLC()
	{
	CRrcFddModeSpecificInfoContent* self = new (ELeave) CRrcFddModeSpecificInfoContent();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcFddModeSpecificInfoContent::~CRrcFddModeSpecificInfoContent()
	{
	}

CRrcFddModeSpecificInfoContent::CRrcFddModeSpecificInfoContent()
	{
	}


void CRrcFddModeSpecificInfoContent::ConstructL()
	{
	}




CRrcFddModeSpecificInfo* CRrcFddModeSpecificInfo::NewL(CRrcFddModeSpecificInfoContent* aFddModeSpecificInfoContent)
	{
	CRrcFddModeSpecificInfo* self = CRrcFddModeSpecificInfo::NewLC(aFddModeSpecificInfoContent);
	CleanupStack::Pop(self);
	return self;					
	}
CRrcFddModeSpecificInfo* CRrcFddModeSpecificInfo::NewLC(CRrcFddModeSpecificInfoContent* aFddModeSpecificInfoContent)
	{
	CRrcFddModeSpecificInfo* self = new (ELeave) CRrcFddModeSpecificInfo(aFddModeSpecificInfoContent);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcFddModeSpecificInfo::~CRrcFddModeSpecificInfo()
	{
	}

CRrcFddModeSpecificInfo::CRrcFddModeSpecificInfo(CRrcFddModeSpecificInfoContent* aFddModeSpecificInfoContent)
:iFddModeSpecificInfoContent(aFddModeSpecificInfoContent)
	{
	}

void CRrcFddModeSpecificInfo::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcFddModeSpecificInfo::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		//		fdd								SEQUENCE {
		//			referenceIdentity				PrimaryCPICH-Info
		//		},
		
		//		PrimaryCPICH-Info ::=				SEQUENCE {
		//		primaryScramblingCode				PrimaryScramblingCode
		//					}
		//		PrimaryScramblingCode ::=			INTEGER (0..511)

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// NO "E" BIT in fdd
	// Decode PrimaryCPICH-Info
	// NO "E" BIT in PrimaryCPICH-Info	
	// Decode PrimaryScramblingCode

	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						511,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iFddModeSpecificInfoContent->iReferenceIdentity);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	}

void CRrcFddModeSpecificInfo::ConstructL()
	{
	}







CRrcTddModeSpecificInfoContent* CRrcTddModeSpecificInfoContent::NewL()
	{
	CRrcTddModeSpecificInfoContent* self = CRrcTddModeSpecificInfoContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcTddModeSpecificInfoContent* CRrcTddModeSpecificInfoContent::NewLC()
	{
	CRrcTddModeSpecificInfoContent* self = new (ELeave) CRrcTddModeSpecificInfoContent();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcTddModeSpecificInfoContent::~CRrcTddModeSpecificInfoContent()
	{
	}

CRrcTddModeSpecificInfoContent::CRrcTddModeSpecificInfoContent()
	{
	}


void CRrcTddModeSpecificInfoContent::ConstructL()
	{
	}



CRrcTddModeSpecificInfo* CRrcTddModeSpecificInfo::NewL(CRrcTddModeSpecificInfoContent* aTddModeSpecificInfoContent)
	{
	CRrcTddModeSpecificInfo* self = CRrcTddModeSpecificInfo::NewLC(aTddModeSpecificInfoContent);
	CleanupStack::Pop(self);
	return self;					
	}
CRrcTddModeSpecificInfo* CRrcTddModeSpecificInfo::NewLC(CRrcTddModeSpecificInfoContent* aTddModeSpecificInfoContent)
	{
	CRrcTddModeSpecificInfo* self = new (ELeave) CRrcTddModeSpecificInfo(aTddModeSpecificInfoContent);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcTddModeSpecificInfo::~CRrcTddModeSpecificInfo()
	{
	}

CRrcTddModeSpecificInfo::CRrcTddModeSpecificInfo(CRrcTddModeSpecificInfoContent* aTddModeSpecificInfoContent)
:iTddModeSpecificInfoContent(aTddModeSpecificInfoContent)
	{
	}

void CRrcTddModeSpecificInfo::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcTddModeSpecificInfo::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		//		tdd								SEQUENCE {
		//			referenceIdentity				CellParametersID
		//			}
		//		CellParametersID ::=				INTEGER (0..127)

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" bit
	
	// Decode CellParametersID
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						127,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iTddModeSpecificInfoContent->iReferenceIdentity);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;	
	}


void CRrcTddModeSpecificInfo::ConstructL()
	{
	}









CRrcModeSpecificInfoContent* CRrcModeSpecificInfoContent::NewL()
	{
	CRrcModeSpecificInfoContent* self = CRrcModeSpecificInfoContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcModeSpecificInfoContent* CRrcModeSpecificInfoContent::NewLC()
	{
	CRrcModeSpecificInfoContent* self = new (ELeave) CRrcModeSpecificInfoContent();
	CleanupStack::PushL(self);
	//self->ConstructL();
	return self;
	}

CRrcModeSpecificInfoContent::~CRrcModeSpecificInfoContent()
	{
	delete iFddModeSpecificInfoContent;
	delete iTddModeSpecificInfoContent;
	}

CRrcModeSpecificInfoContent::CRrcModeSpecificInfoContent()
	{
	}


void CRrcModeSpecificInfoContent::ConstructL()
	{
	}


CRrcModeSpecificInfo* CRrcModeSpecificInfo::NewL(CRrcModeSpecificInfoContent* aModeSpecificInfoContent)
	{
	CRrcModeSpecificInfo* self = CRrcModeSpecificInfo::NewLC(aModeSpecificInfoContent);
	CleanupStack::Pop(self);
	return self;					
	}
CRrcModeSpecificInfo* CRrcModeSpecificInfo::NewLC(CRrcModeSpecificInfoContent* aModeSpecificInfoContent)
	{
	CRrcModeSpecificInfo* self = new (ELeave) CRrcModeSpecificInfo(aModeSpecificInfoContent);
	CleanupStack::PushL(self);
	return self;
	}

CRrcModeSpecificInfo::~CRrcModeSpecificInfo()
	{
	delete iFddModeSpecificInfo;
	delete iTddModeSpecificInfo;
	}

CRrcModeSpecificInfo::CRrcModeSpecificInfo(CRrcModeSpecificInfoContent* aModeSpecificInfoContent)
:iModeSpecificInfoContent(aModeSpecificInfoContent)
	{
	}

void CRrcModeSpecificInfo::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcModeSpecificInfo::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		//	modeSpecificInfo				CHOICE {
		//		fdd								SEQUENCE {
		//			referenceIdentity				PrimaryCPICH-Info
		//		},
		//		tdd								SEQUENCE {
		//			referenceIdentity				CellParametersID
		//		}
		//	}

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// Decode Mode Specific Info Type
	TInt modeSpecificType(0);
	CAsn1PerChoice::DecodeL(aBufferToDecode,
							startPos,
							1,
							KAsn1PerNonExtensible,
							bitsDecoded,
							isExtended,
							modeSpecificType	
							);
	iModeSpecificInfoContent->iModeSpecificInfoType=(TRrcModeSpecificInfo)modeSpecificType;
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	// Decode Mode Specific Info 	
	if (iModeSpecificInfoContent->iModeSpecificInfoType == ERrcFddMode)
		{
		iModeSpecificInfoContent->iFddModeSpecificInfoContent=CRrcFddModeSpecificInfoContent::NewL();
		iFddModeSpecificInfo=CRrcFddModeSpecificInfo::NewL(iModeSpecificInfoContent->iFddModeSpecificInfoContent);
		iFddModeSpecificInfo->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		}
	else
		{
		iModeSpecificInfoContent->iTddModeSpecificInfoContent=CRrcTddModeSpecificInfoContent::NewL();
		iTddModeSpecificInfo=CRrcTddModeSpecificInfo::NewL(iModeSpecificInfoContent->iTddModeSpecificInfoContent);
		iTddModeSpecificInfo->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		}
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	}

void CRrcModeSpecificInfo::ConstructL()
	{
	}

void CRrcModeSpecificInfo::WriteLBSAssistanceDataL(RUtranGpsReferenceTimeBuilder* aUtranGpsReferenceTimeBuilder)
	{

	if (iModeSpecificInfoContent->iModeSpecificInfoType==ERrcFddMode)
		{
		RPrimaryCpichInfoBuilder primaryCpichInfoBuilder;
		aUtranGpsReferenceTimeBuilder->GetFieldBuilder(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity, primaryCpichInfoBuilder);		
		primaryCpichInfoBuilder.SetField(TPrimaryCpichInfo::EPrimaryScramblingCode, (TUint)iModeSpecificInfoContent->iFddModeSpecificInfoContent->iReferenceIdentity);
		}
	else
		{
		aUtranGpsReferenceTimeBuilder->SetField(TUtranGpsReferenceTime::EModeSpecificInfo_Tdd_ReferenceIdentity, (TUint)iModeSpecificInfoContent->iTddModeSpecificInfoContent->iReferenceIdentity);			
		}
	}



CRrcUtranGpsReferenceTimeContent* CRrcUtranGpsReferenceTimeContent::NewL()
	{
	CRrcUtranGpsReferenceTimeContent* self = CRrcUtranGpsReferenceTimeContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcUtranGpsReferenceTimeContent* CRrcUtranGpsReferenceTimeContent::NewLC()
	{
	CRrcUtranGpsReferenceTimeContent* self = new (ELeave) CRrcUtranGpsReferenceTimeContent();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcUtranGpsReferenceTimeContent::~CRrcUtranGpsReferenceTimeContent()
	{
	delete iUtranGpsTimingOfCellContent;
	delete iModeSpecificInfoContent;
	}

CRrcUtranGpsReferenceTimeContent::CRrcUtranGpsReferenceTimeContent()
	{
	}

void CRrcUtranGpsReferenceTimeContent::ConstructL()
	{
	}




CRrcUtranGpsReferenceTime* CRrcUtranGpsReferenceTime::NewL(CRrcUtranGpsReferenceTimeContent* aUtranGpsReferenceTimeContent)
	{
	CRrcUtranGpsReferenceTime* self = CRrcUtranGpsReferenceTime::NewLC(aUtranGpsReferenceTimeContent);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcUtranGpsReferenceTime* CRrcUtranGpsReferenceTime::NewLC(CRrcUtranGpsReferenceTimeContent* aUtranGpsReferenceTimeContent)
	{
	CRrcUtranGpsReferenceTime* self = new (ELeave) CRrcUtranGpsReferenceTime(aUtranGpsReferenceTimeContent);
	CleanupStack::PushL(self);
	//self->ConstructL();
	return self;
	}

CRrcUtranGpsReferenceTime::~CRrcUtranGpsReferenceTime()
	{
	delete iUtranGpsTimingOfCell;
	delete iModeSpecificInfo;
	}

CRrcUtranGpsReferenceTime::CRrcUtranGpsReferenceTime(CRrcUtranGpsReferenceTimeContent* aUtranGpsReferenceTimeContent)
:iUtranGpsReferenceTimeContent(aUtranGpsReferenceTimeContent)
	{
	}

void CRrcUtranGpsReferenceTime::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcUtranGpsReferenceTime::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		// UTRAN-GPSReferenceTime ::=				SEQUENCE {
		// 	-- For utran-GPSTimingOfCell values above 2322431999999 are not 
		// 	-- used in this version of the specification
		// 	-- Actual value utran-GPSTimingOfCell = (ms-part * 4294967296) + ls-part
		// 	utran-GPSTimingOfCell			SEQUENCE {
		// 		ms-part 						INTEGER (0..1023),
		// 		ls-part							INTEGER (0..4294967295)
		// 	},
		// 	modeSpecificInfo				CHOICE {
		// 		fdd								SEQUENCE {
		// 			referenceIdentity				PrimaryCPICH-Info
		// 		},
		// 		tdd								SEQUENCE {
		// 			referenceIdentity				CellParametersID
		// 		}
		// 	} 				OPTIONAL,
		// 	sfn									INTEGER (0..4095)
		// }

	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT
	// Decode option bit map	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iUtranGpsReferenceTimeContent->iModeSpecificInfoPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode utran-GPSTimingOfCell
	iUtranGpsReferenceTimeContent->iUtranGpsTimingOfCellContent=CRrcUtranGpsTimingOfCellContent::NewL();
	iUtranGpsTimingOfCell=CRrcUtranGpsTimingOfCell::NewL(iUtranGpsReferenceTimeContent->iUtranGpsTimingOfCellContent);
	iUtranGpsTimingOfCell->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	// Decode modeSpecificInfo
	if (iUtranGpsReferenceTimeContent->iModeSpecificInfoPresent)
		{
		iUtranGpsReferenceTimeContent->iModeSpecificInfoContent=CRrcModeSpecificInfoContent::NewL();
		iModeSpecificInfo=CRrcModeSpecificInfo::NewL(iUtranGpsReferenceTimeContent->iModeSpecificInfoContent);
		iModeSpecificInfo->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}

	// Decode sfn
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						4095,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iUtranGpsReferenceTimeContent->iSfn);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	}

void CRrcUtranGpsReferenceTime::ConstructL()
	{
	}

void CRrcUtranGpsReferenceTime::WriteLBSAssistanceDataL(RUEPositioningGpsReferenceTimeBuilder* aGpsReferenceTimeBuilder)
	{
	
	RUtranGpsReferenceTimeBuilder utranGpsReferenceTimeBuilder;
	aGpsReferenceTimeBuilder->GetFieldBuilder(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime, utranGpsReferenceTimeBuilder);
	
	utranGpsReferenceTimeBuilder.SetField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_MsPart, TUint(iUtranGpsReferenceTimeContent->iUtranGpsTimingOfCellContent->iMsPart));
	utranGpsReferenceTimeBuilder.SetField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_LsPart, TUint(iUtranGpsReferenceTimeContent->iUtranGpsTimingOfCellContent->iLsPart));
	
	if (iUtranGpsReferenceTimeContent->iModeSpecificInfoPresent)
		{
		iModeSpecificInfo->WriteLBSAssistanceDataL(&utranGpsReferenceTimeBuilder);			
		}
	
	utranGpsReferenceTimeBuilder.SetField(TUtranGpsReferenceTime::ESfn, TUint(iUtranGpsReferenceTimeContent->iSfn));
			
	}

void CRrcUtranGpsReferenceTime::WriteLBSAssistanceDataL(RUEPositioningGpsAcquisitionAssistanceBuilder* aGpsAcquisitionAssistanceBuilder)
	{
	RUtranGpsReferenceTimeBuilder utranGpsReferenceTimeBuilder;
	aGpsAcquisitionAssistanceBuilder->GetFieldBuilder(TUEPositioningGpsAcquisitionAssistance::EUtranGpsReferenceTime, utranGpsReferenceTimeBuilder);
	utranGpsReferenceTimeBuilder.SetField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_MsPart, TUint(iUtranGpsReferenceTimeContent->iUtranGpsTimingOfCellContent->iMsPart));
	utranGpsReferenceTimeBuilder.SetField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_LsPart, TUint(iUtranGpsReferenceTimeContent->iUtranGpsTimingOfCellContent->iLsPart));
	
	if (iUtranGpsReferenceTimeContent->iModeSpecificInfoPresent)
		{
		iModeSpecificInfo->WriteLBSAssistanceDataL(&utranGpsReferenceTimeBuilder);			
		}

	utranGpsReferenceTimeBuilder.SetField(TUtranGpsReferenceTime::ESfn, TUint(iUtranGpsReferenceTimeContent->iSfn));		
	}


CRrcGpsReferenceTimeContent* CRrcGpsReferenceTimeContent::NewL()
	{
	CRrcGpsReferenceTimeContent* self = CRrcGpsReferenceTimeContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsReferenceTimeContent* CRrcGpsReferenceTimeContent::NewLC()
	{
	CRrcGpsReferenceTimeContent* self = new (ELeave) CRrcGpsReferenceTimeContent();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcGpsReferenceTimeContent::~CRrcGpsReferenceTimeContent()
	{
	delete iUtranGpsReferenceTimeContent;
	delete iTowAssistListContent;
	}

CRrcGpsReferenceTimeContent::CRrcGpsReferenceTimeContent()
	{
	}

void CRrcGpsReferenceTimeContent::ConstructL()
	{
	}



CRrcGpsReferenceTime* CRrcGpsReferenceTime::NewL(CRrcGpsReferenceTimeContent* aGpsReferenceTimeContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsReferenceTime* self = CRrcGpsReferenceTime::NewLC(aGpsReferenceTimeContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsReferenceTime* CRrcGpsReferenceTime::NewLC(CRrcGpsReferenceTimeContent* aGpsReferenceTimeContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsReferenceTime* self = new (ELeave) CRrcGpsReferenceTime(aGpsReferenceTimeContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CRrcGpsReferenceTime::~CRrcGpsReferenceTime()
	{
	delete iUtranGpsReferenceTime;
	delete iTowAssistList;
	}

CRrcGpsReferenceTime::CRrcGpsReferenceTime(CRrcGpsReferenceTimeContent* aGpsReferenceTimeContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
:iGpsReferenceTimeContent(aGpsReferenceTimeContent), iLbsAssistanceDataBuilderSet(aLbsAssistanceDataBuilderSet)
	{
	}

void CRrcGpsReferenceTime::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcGpsReferenceTime::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
	//UE-Positioning-GPS-ReferenceTime ::=			SEQUENCE {
	//	gps-Week							INTEGER (0..1023),
	//	gps-tow-1msec						GPS-TOW-1msec,	utran-GPSReferenceTime				UTRAN-GPSReferenceTime				OPTIONAL,
	//	sfn-tow-Uncertainty					SFN-TOW-Uncertainty					OPTIONAL,
	//	utran-GPS-DriftRate					UTRAN-GPS-DriftRate					OPTIONAL,
	//	gps-TOW-AssistList					GPS-TOW-AssistList					OPTIONAL
	//}

	//GPS-TOW-1msec ::=					INTEGER (0..604799999)
	//SFN-TOW-Uncertainty ::=				ENUMERATED {
	//									lessThan10,
	//									moreThan10 }	

	//UTRAN-GPS-DriftRate ::=				ENUMERATED {
	//								utran-GPSDrift0, utran-GPSDrift1, utran-GPSDrift2,
	//								utran-GPSDrift5, utran-GPSDrift10, utran-GPSDrift15,
	//								utran-GPSDrift25, utran-GPSDrift50, utran-GPSDrift-1,
	//								utran-GPSDrift-2, utran-GPSDrift-5, utran-GPSDrift-10,
	//								utran-GPSDrift-15, utran-GPSDrift-25, utran-GPSDrift-50}


	aDecodedBits=0;
	TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT
	
	// Decode Option Bit for GpsReferenceTime
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsReferenceTimeContent->iUtranGpsReferenceTimePresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode Option Bit for SfnTowUncertainty
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsReferenceTimeContent->iSfnTowUncertaintyPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode Option Bit for UtranGpsDriftRate
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsReferenceTimeContent->iUtranGpsDriftRatePresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode Option Bit for TowAssistList
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsReferenceTimeContent->iGpsTowAssistListPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode gps-Week
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						1023,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iGpsReferenceTimeContent->iWeek);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode GPS-TOW-1msec
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						604799999,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iGpsReferenceTimeContent->iTow1msec);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode Utran Reference Time
	if (iGpsReferenceTimeContent->iUtranGpsReferenceTimePresent)
		{
		iGpsReferenceTimeContent->iUtranGpsReferenceTimeContent=CRrcUtranGpsReferenceTimeContent::NewL();		
		iUtranGpsReferenceTime=CRrcUtranGpsReferenceTime::NewL(iGpsReferenceTimeContent->iUtranGpsReferenceTimeContent);
		iUtranGpsReferenceTime->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}

	if(iGpsReferenceTimeContent->iSfnTowUncertaintyPresent)
		{
		TInt chioceValue;
		CAsn1PerEnum::DecodeL(aBufferToDecode,
							startPos,
							1,
							KAsn1PerNonExtensible,
							KAsn1PerNoPadding,
							bitsDecoded,
							isExtended,
							chioceValue
							);
							
		iGpsReferenceTimeContent->iTowUncertainty=(TRrcSfnTowUncertainty)chioceValue;
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}

	if (iGpsReferenceTimeContent->iUtranGpsDriftRatePresent)
		{
		TInt chioceValue;
		CAsn1PerEnum::DecodeL(aBufferToDecode,
							startPos,
							14,
							KAsn1PerNonExtensible,
							KAsn1PerNoPadding,
							bitsDecoded,
							isExtended,
							chioceValue
							);
							
		iGpsReferenceTimeContent->iUtranGpsDriftRate=(TRrcUtranGpsDriftRate)chioceValue;
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}

	// Decode Tow Assist List
	if (iGpsReferenceTimeContent->iGpsTowAssistListPresent)
		{
		iGpsReferenceTimeContent->iTowAssistListContent=CRrcGpsTowAssistListContent::NewL();
		iTowAssistList=CRrcGpsTowAssistList::NewL(iGpsReferenceTimeContent->iTowAssistListContent);
		iTowAssistList->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}
	}


void CRrcGpsReferenceTime::ConstructL()
	{
	}

void CRrcGpsReferenceTime::WriteLBSAssistanceDataL()
	{
	//Fetch a GpsReferenceTime data item.
	RUEPositioningGpsReferenceTimeBuilder* gpsReferenceTimeBuilder;
	iLbsAssistanceDataBuilderSet->GetDataBuilder(gpsReferenceTimeBuilder);

	// Populate the data item
	gpsReferenceTimeBuilder->SetField(TUEPositioningGpsReferenceTime::EGpsWeek, (TUint)iGpsReferenceTimeContent->iWeek);
	gpsReferenceTimeBuilder->SetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec, (TUint)iGpsReferenceTimeContent->iTow1msec);
	
	if (iGpsReferenceTimeContent->iSfnTowUncertaintyPresent)
		{
		gpsReferenceTimeBuilder->SetField(TUEPositioningGpsReferenceTime::ESfnTowUncertainty, (TSfnTowUncertainty)iGpsReferenceTimeContent->iTowUncertainty);			
		}
	
	if (iGpsReferenceTimeContent->iUtranGpsDriftRatePresent)
		{
		gpsReferenceTimeBuilder->SetField(TUEPositioningGpsReferenceTime::EUtranGpsDriftRate, (TUtranGpsDriftRate)iGpsReferenceTimeContent->iUtranGpsDriftRate);
		}
			
	if (iGpsReferenceTimeContent->iGpsTowAssistListPresent)
		{
		iTowAssistList->WriteLBSAssistanceDataL(gpsReferenceTimeBuilder);	
		}
	
	if (iGpsReferenceTimeContent->iUtranGpsReferenceTimePresent)
		{
		iUtranGpsReferenceTime->WriteLBSAssistanceDataL(gpsReferenceTimeBuilder);	
		}
	
	TTime timeNow;
	timeNow.UniversalTime();
	gpsReferenceTimeBuilder->SetTimeStamp(timeNow);
	}
	

