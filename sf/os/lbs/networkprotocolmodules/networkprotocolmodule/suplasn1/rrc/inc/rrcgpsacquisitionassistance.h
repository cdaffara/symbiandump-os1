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
// rrcgpsacquisitionassistance - GPS acquisition assistance encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/
#ifndef __RRCGPSACQUISITIONASSISTANCE_H__
#define __RRCGPSACQUISITIONASSISTANCE_H__

#include <e32base.h>
#include "rrcgpsreferencetime.h"

class RLbsAssistanceDataBuilderSet;
class RUEPositioningGpsAcquisitionAssistanceBuilder;
class RAcquisitionSatInfoBuilder;

enum TRrcDopplerUncertainty
	{
	ERrcHz12_5, 
	ERrcHz25, 
	ERrcHz50, 
	ERrcHz100, 
	ERrcHz200,
	ERrcSpare3, 
	ERrcSpare2, 
	ERrcSpare1
	};

enum TRrcCodePhaseSearchWindow
	{
	ERrcW1023, 
	ERrcW1, 
	ERrcW2, 
	ERrcW3, 
	ERrcW4, 
	ERrcW6, 
	ERrcW8,
	ERrcW12, 
	ERrcW16, 
	ERrcW24, 
	ERrcW32, 
	ERrcW48, 
	ERrcW64,
	ERrcW96, 
	ERrcW128, 
	ERrcW192
	};






class CRrcExtraDopplerInfoContent : public CBase
	{
public:
	static CRrcExtraDopplerInfoContent* NewL();
	static CRrcExtraDopplerInfoContent* NewLC();
	~CRrcExtraDopplerInfoContent();

private:
	CRrcExtraDopplerInfoContent();
	void ConstructL();
		
public:
	TInt8 iDoppler1stOrder;
	TRrcDopplerUncertainty iDopplerUncertainty;
	};


class CRrcExtraDopplerInfo : public CBase
	{
public:
	static CRrcExtraDopplerInfo* NewL(CRrcExtraDopplerInfoContent* aExtraDopplerInfoContent);
	static CRrcExtraDopplerInfo* NewLC(CRrcExtraDopplerInfoContent* aExtraDopplerInfoContent);
	~CRrcExtraDopplerInfo();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL(RAcquisitionSatInfoBuilder* aAcquisitionSatInfoBuilder);
	
private:
	CRrcExtraDopplerInfo(CRrcExtraDopplerInfoContent* aExtraDopplerInfoContent);
	void ConstructL();
	
private: 
	CRrcExtraDopplerInfoContent* iExtraDopplerInfoContent;
	};
	





class CRrcAzimuthAndElevationContent : public CBase
	{
public:
	static CRrcAzimuthAndElevationContent* NewL();
	static CRrcAzimuthAndElevationContent* NewLC();
	~CRrcAzimuthAndElevationContent();

private:
	CRrcAzimuthAndElevationContent();
	void ConstructL();	
	
	
public:
	TUint8 iAzimuth;	// INTEGER (0..31),
	TUint8 iElevation;	// INTEGER (0..7)
	};


class CRrcAzimuthAndElevation : public CBase
	{
public:
	static CRrcAzimuthAndElevation* NewL(CRrcAzimuthAndElevationContent* aAzimuthAndElevationContent);
	static CRrcAzimuthAndElevation* NewLC(CRrcAzimuthAndElevationContent* aAzimuthAndElevationContent);
	~CRrcAzimuthAndElevation();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL(RAcquisitionSatInfoBuilder* aAcquisitionSatInfoBuilder);

	
private:
	CRrcAzimuthAndElevation(CRrcAzimuthAndElevationContent* aAzimuthAndElevationContent);
	void ConstructL();

private:
	CRrcAzimuthAndElevationContent* iAzimuthAndElevationContent;
	};







class CRrcAcquisitionSatInfoContent : public CBase
	{
public:
	static CRrcAcquisitionSatInfoContent* NewL();
	static CRrcAcquisitionSatInfoContent* NewLC();
	~CRrcAcquisitionSatInfoContent();

private:
	CRrcAcquisitionSatInfoContent();
	void ConstructL();	

public:
	TBool iExtraDopplerInfoPresent;
	TBool iAzimuthAndElevationPresent;
	TUint8 iSatID;							
	TInt iDoppler0thOrder;			//INTEGER (-2048..2047),
	CRrcExtraDopplerInfoContent* iExtraDopplerInfoContent;	//ExtraDopplerInfo					OPTIONAL,
	TUint16 iCodePhase;					//INTEGER (0..1022),
	TUint8 iIntegerCodePhase;			//INTEGER (0..19),
	TUint8 iGpsBitNumber;				//INTEGER (0..3),
	TRrcCodePhaseSearchWindow iCodePhaseSearchWindow;	//CodePhaseSearchWindow,
	CRrcAzimuthAndElevationContent*	iAzimuthAndElevationContent;	//AzimuthAndElevation				OPTIONAL
	};


class CRrcAcquisitionSatInfo : public CBase
	{
public:
	static CRrcAcquisitionSatInfo* NewL(CRrcAcquisitionSatInfoContent* aAcquisitionSatInfoContent);
	static CRrcAcquisitionSatInfo* NewLC(CRrcAcquisitionSatInfoContent* aAcquisitionSatInfoContent);
	~CRrcAcquisitionSatInfo();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL(RAcquisitionSatInfoBuilder* aAcquisitionSatInfoBuilder);
private:
	CRrcAcquisitionSatInfo(CRrcAcquisitionSatInfoContent* aAcquisitionSatInfoContent);
	void ConstructL();
	
private:
	CRrcAcquisitionSatInfoContent* iAcquisitionSatInfoContent;
	CRrcExtraDopplerInfo* iExtraDopplerInfo;
	CRrcAzimuthAndElevation* iAzimuthAndElevation;
	};







class CRrcAcquisitionSatInfoListContent : public CBase
	{
public:
	static CRrcAcquisitionSatInfoListContent* NewL();
	static CRrcAcquisitionSatInfoListContent* NewLC();
	~CRrcAcquisitionSatInfoListContent();

private:
	CRrcAcquisitionSatInfoListContent();
	void ConstructL();	
	
public:
	RPointerArray<CRrcAcquisitionSatInfoContent> iAcquisitionSatInfoContentList;
	};


class CRrcAcquisitionSatInfoList : public CBase
	{
public:
	static CRrcAcquisitionSatInfoList* NewL(CRrcAcquisitionSatInfoListContent* aAcquisitionSatInfoListContent);
	static CRrcAcquisitionSatInfoList* NewLC(CRrcAcquisitionSatInfoListContent* aAcquisitionSatInfoListContent);
	~CRrcAcquisitionSatInfoList();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL(RUEPositioningGpsAcquisitionAssistanceBuilder* aGpsAcquisitionAssistanceBuilder);
	
private:
	CRrcAcquisitionSatInfoList(CRrcAcquisitionSatInfoListContent* aAcquisitionSatInfoListContent);
	void ConstructL();

private:
	CRrcAcquisitionSatInfoListContent* iAcquisitionSatInfoListContent;
	RPointerArray<CRrcAcquisitionSatInfo> iAcquisitionSatInfoList;
	};







class CRrcGpsAcquisitionAssistanceContent : public CBase
	{
public:
	static CRrcGpsAcquisitionAssistanceContent* NewL();
	static CRrcGpsAcquisitionAssistanceContent* NewLC();
	~CRrcGpsAcquisitionAssistanceContent();

private:
	CRrcGpsAcquisitionAssistanceContent();
	void ConstructL();	
public:
	TBool iUtranGpsReferenceTimePresent;
	TUint32 iGpsReferenceTime;
	CRrcUtranGpsReferenceTimeContent* iUtranGpsReferenceTimeContent;
	CRrcAcquisitionSatInfoListContent* iAcquisitionSatInfoListContent;
	};


class CRrcGpsAcquisitionAssistance : public CBase
	{
public:
	static CRrcGpsAcquisitionAssistance* NewL(CRrcGpsAcquisitionAssistanceContent* aGpsAcquisitionAssistanceContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	static CRrcGpsAcquisitionAssistance* NewLC(CRrcGpsAcquisitionAssistanceContent* aGpsAcquisitionAssistanceContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	~CRrcGpsAcquisitionAssistance();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL();
	
private:
	CRrcGpsAcquisitionAssistance(CRrcGpsAcquisitionAssistanceContent* aGpsAcquisitionAssistanceContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	void ConstructL();

private:
	CRrcGpsAcquisitionAssistanceContent* iAcquisitionAssistanceContent;
	CRrcUtranGpsReferenceTime* iUtranGpsReferenceTime;
	CRrcAcquisitionSatInfoList* iAcquisitionSatInfoList;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};

#endif // __RRCGPSACQUISITIONASSISTANCE_H__