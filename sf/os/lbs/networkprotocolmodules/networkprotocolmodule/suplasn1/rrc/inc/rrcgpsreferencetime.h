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
// rrcgpsreferencetime - GPS reference time encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/
#ifndef __RRCGPSREFERENCETIME_H__
#define __RRCGPSREFERENCETIME_H__

#include <e32base.h>
#include "asn1per.h"

class RLbsAssistanceDataBuilderSet;
class RUEPositioningGpsReferenceTimeBuilder;
class RUEPositioningGpsAcquisitionAssistanceBuilder;
class RUtranGpsReferenceTimeBuilder;

enum TRrcUtranGpsDriftRate
	{
	ERrcUtranGPSDrift0, 
	ERrcUtranGPSDrift1, 
	ERrcUtranGPSDrift2,
	ERrcUtranGPSDrift5, 
	ERrcUtranGPSDrift10, 
	ERrcUtranGPSDrift15,
	ERrcUtranGPSDrift25, 
	ERrcUtranGPSDrift50, 
	ERrcUtranGPSDriftM1,
	ERrcUtranGPSDriftM2, 
	ERrcUtranGPSDriftM5, 
	ERrcUtranGPSDriftM10,
	ERrcUtranGPSDriftM15, 
	ERrcUtranGPSDriftM25, 
	ERrcUtranGPSDriftM50
	};

enum TRrcSfnTowUncertainty
	{
	ERrcLessThan10,
	ERrcMoreThan10
	};

enum TRrcModeSpecificInfo
	{
	ERrcFddMode,
	ERrcTddMode
	};




class CRrcGpsTowAssistContent : public CBase
	{
public:
	static CRrcGpsTowAssistContent* NewL();
	static CRrcGpsTowAssistContent* NewLC();
	~CRrcGpsTowAssistContent();
	
private:
	CRrcGpsTowAssistContent();
public:
	TUint8 iSatID;				//	SatID,
	TBuf8<2> iTlmMessage;		//	BIT STRING (SIZE (14)),
	TBuf8<1> iTlmReserved;		//	BIT STRING (SIZE (2)),
	TBool iAlert;				//	BOOLEAN,
	TBool iAntiSpoof;			//	BOOLEAN

	};


class CRrcGpsTowAssist : public CBase
	{
public:
	static CRrcGpsTowAssist* NewL(CRrcGpsTowAssistContent* aTowAssistContent);
	static CRrcGpsTowAssist* NewLC(CRrcGpsTowAssistContent* aTowAssistContent);
	~CRrcGpsTowAssist();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBits);
	
private:
	CRrcGpsTowAssist(CRrcGpsTowAssistContent* aTowAssistContent);
	CRrcGpsTowAssistContent* iTowAssistContent;
	};


class CRrcGpsTowAssistListContent : public CBase
	{
public:
	static CRrcGpsTowAssistListContent* NewL();
	static CRrcGpsTowAssistListContent* NewLC();
	~CRrcGpsTowAssistListContent();
	
private:
	void ConstructL();
public:
	CRrcGpsTowAssistListContent();
	RPointerArray<CRrcGpsTowAssistContent> iGpsTowAssistArrayContent;

	};

class CRrcGpsTowAssistList : public CBase
	{
public:
	static CRrcGpsTowAssistList* NewL(CRrcGpsTowAssistListContent* aTowAssistListContent);
	static CRrcGpsTowAssistList* NewLC(CRrcGpsTowAssistListContent* aTowAssistListContent);
	~CRrcGpsTowAssistList();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBits);
	void WriteLBSAssistanceDataL(RUEPositioningGpsReferenceTimeBuilder* aGpsReferenceTimeBuilder);
	
private:
	CRrcGpsTowAssistList(CRrcGpsTowAssistListContent* aTowAssistListContent);
	void ConstructL();
private:
	CRrcGpsTowAssistListContent* iGpsTowAssistArrayContent;
	RPointerArray<CRrcGpsTowAssist> iGpsTowAssistArray;
	};




class CRrcUtranGpsTimingOfCellContent : public CBase
	{
public:
	static CRrcUtranGpsTimingOfCellContent* NewL();
	static CRrcUtranGpsTimingOfCellContent* NewLC();
	~CRrcUtranGpsTimingOfCellContent();
	
private:
	CRrcUtranGpsTimingOfCellContent();
	void ConstructL();	
public:

	TUint16 iMsPart;
	TUint32 iLsPart;
	};



class CRrcUtranGpsTimingOfCell : public CBase
	{
public:
	static CRrcUtranGpsTimingOfCell* NewL(CRrcUtranGpsTimingOfCellContent* aUtranGpsTimingOfCellContent);
	static CRrcUtranGpsTimingOfCell* NewLC(CRrcUtranGpsTimingOfCellContent* aUtranGpsTimingOfCellContent);
	~CRrcUtranGpsTimingOfCell();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBits);

private:
	CRrcUtranGpsTimingOfCell(CRrcUtranGpsTimingOfCellContent* aUtranGpsTimingOfCellContent);
	void ConstructL();
	CRrcUtranGpsTimingOfCellContent* iUtranGpsTimingOfCellContent;
	};

class CRrcFddModeSpecificInfoContent : public CBase
	{
public:
	static CRrcFddModeSpecificInfoContent* NewL();
	static CRrcFddModeSpecificInfoContent* NewLC();
	~CRrcFddModeSpecificInfoContent();
	
private:
	CRrcFddModeSpecificInfoContent();
	void ConstructL();
	
public:
	TUint16 iReferenceIdentity;
	};


class CRrcFddModeSpecificInfo : public CBase
	{
public:
	static CRrcFddModeSpecificInfo* NewL(CRrcFddModeSpecificInfoContent* aFddModeSpecificInfoContent);
	static CRrcFddModeSpecificInfo* NewLC(CRrcFddModeSpecificInfoContent* aFddModeSpecificInfoContent);
	~CRrcFddModeSpecificInfo();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBits);

private:
	CRrcFddModeSpecificInfo(CRrcFddModeSpecificInfoContent* aFddModeSpecificInfoContent);
	void ConstructL();
	CRrcFddModeSpecificInfoContent* iFddModeSpecificInfoContent;
	};

class CRrcTddModeSpecificInfoContent : public CBase
	{
public:
	static CRrcTddModeSpecificInfoContent* NewL();
	static CRrcTddModeSpecificInfoContent* NewLC();
	~CRrcTddModeSpecificInfoContent();
private:
	CRrcTddModeSpecificInfoContent();
	void ConstructL();	
public:
	TUint8 iReferenceIdentity;
	};


class CRrcTddModeSpecificInfo : public CBase
	{
public:
	static CRrcTddModeSpecificInfo* NewL(CRrcTddModeSpecificInfoContent* aTddModeSpecificInfoContent);
	static CRrcTddModeSpecificInfo* NewLC(CRrcTddModeSpecificInfoContent* aTddModeSpecificInfoContent);
	~CRrcTddModeSpecificInfo();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBits);
private:
	void ConstructL();
	CRrcTddModeSpecificInfo(CRrcTddModeSpecificInfoContent* aTddModeSpecificInfoContent);
	CRrcTddModeSpecificInfoContent* iTddModeSpecificInfoContent;
	};



class CRrcModeSpecificInfoContent : public CBase
	{
public:
	static CRrcModeSpecificInfoContent* NewL();
	static CRrcModeSpecificInfoContent* NewLC();
	~CRrcModeSpecificInfoContent();

private:
	CRrcModeSpecificInfoContent();
	void ConstructL();

public:
	TRrcModeSpecificInfo iModeSpecificInfoType;
	CRrcFddModeSpecificInfoContent* iFddModeSpecificInfoContent;
	CRrcTddModeSpecificInfoContent* iTddModeSpecificInfoContent;
	};

class CRrcModeSpecificInfo : public CBase
	{
public:
	static CRrcModeSpecificInfo* NewL(CRrcModeSpecificInfoContent* aModeSpecificInfoContent);
	static CRrcModeSpecificInfo* NewLC(CRrcModeSpecificInfoContent* aModeSpecificInfoContent);
	~CRrcModeSpecificInfo();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBits);
	void WriteLBSAssistanceDataL(RUtranGpsReferenceTimeBuilder* aUtranGpsReferenceTimeBuilder);
	
private:
	CRrcModeSpecificInfo(CRrcModeSpecificInfoContent* aModeSpecificInfoContent);
	void ConstructL();
	CRrcModeSpecificInfoContent* iModeSpecificInfoContent;
	CRrcFddModeSpecificInfo* iFddModeSpecificInfo;
	CRrcTddModeSpecificInfo* iTddModeSpecificInfo;
	};

class CRrcUtranGpsReferenceTimeContent : public CBase
	{
public:

	static CRrcUtranGpsReferenceTimeContent* NewL();
	static CRrcUtranGpsReferenceTimeContent* NewLC();
	~CRrcUtranGpsReferenceTimeContent();

private:
	CRrcUtranGpsReferenceTimeContent();
	void ConstructL();
public:
	TBool iModeSpecificInfoPresent;
	CRrcUtranGpsTimingOfCellContent* iUtranGpsTimingOfCellContent;
	CRrcModeSpecificInfoContent* iModeSpecificInfoContent;
	TUint16 iSfn;
	};


class CRrcUtranGpsReferenceTime : public CBase
	{
public:
	static CRrcUtranGpsReferenceTime* NewL(CRrcUtranGpsReferenceTimeContent* aUtranGpsReferenceTimeContent);
	static CRrcUtranGpsReferenceTime* NewLC(CRrcUtranGpsReferenceTimeContent* aUtranGpsReferenceTimeContent);
	~CRrcUtranGpsReferenceTime();

	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBits);
	void WriteLBSAssistanceDataL(RUEPositioningGpsReferenceTimeBuilder* aGpsReferenceTimeBuilder);
	void WriteLBSAssistanceDataL(RUEPositioningGpsAcquisitionAssistanceBuilder* aGpsAcquisitionAssistanceBuilder);	

private:
	CRrcUtranGpsReferenceTime(CRrcUtranGpsReferenceTimeContent* aUtranGpsReferenceTimeContent);
	void ConstructL();
	CRrcUtranGpsReferenceTimeContent* iUtranGpsReferenceTimeContent;
	CRrcUtranGpsTimingOfCell* iUtranGpsTimingOfCell;
	CRrcModeSpecificInfo* iModeSpecificInfo;
	};




class CRrcGpsReferenceTimeContent : CBase
	{
public:
	static CRrcGpsReferenceTimeContent* NewL();
	static CRrcGpsReferenceTimeContent* NewLC();
	~CRrcGpsReferenceTimeContent();
	void WriteLBSAssistanceDataL();
	
private:
	CRrcGpsReferenceTimeContent();
	void ConstructL();
	
public:
	TBool iUtranGpsReferenceTimePresent;
	TBool iSfnTowUncertaintyPresent;
	TBool iUtranGpsDriftRatePresent;
	TBool iGpsTowAssistListPresent;
	TUint16 iWeek;
	TUint32 iTow1msec;
	CRrcUtranGpsReferenceTimeContent* iUtranGpsReferenceTimeContent;	
	TRrcSfnTowUncertainty iTowUncertainty;
	TRrcUtranGpsDriftRate	iUtranGpsDriftRate;
	CRrcGpsTowAssistListContent* iTowAssistListContent;
	
	};

class CRrcGpsReferenceTime : public CBase
	{
public:
	static CRrcGpsReferenceTime* NewL(CRrcGpsReferenceTimeContent* aGpsReferenceTimeContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	static CRrcGpsReferenceTime* NewLC(CRrcGpsReferenceTimeContent* aGpsReferenceTimeContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	~CRrcGpsReferenceTime();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBits);
	void WriteLBSAssistanceDataL();
private:
	CRrcGpsReferenceTime(CRrcGpsReferenceTimeContent* aGpsReferenceTimeContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	void ConstructL();
	CRrcGpsReferenceTimeContent* iGpsReferenceTimeContent;
	CRrcUtranGpsReferenceTime* iUtranGpsReferenceTime;
	CRrcGpsTowAssistList* iTowAssistList;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};


#endif // __RRCGPSREFERENCETIME_H__
