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
// rrcgpsalmanac - GPS Alanmanac encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/
#ifndef __RRCGPSALMANAC_H__
#define __RRCGPSALMANAC_H__

#include <e32base.h>
#include "asn1per.h"

class RLbsAssistanceDataBuilderSet;
class RAlmanacSatInfoBuilder;
class RUEPositioningGpsAlmanacBuilder;

class CRrcAlmanacSatInfoContent : public CBase
	{
public:
	static CRrcAlmanacSatInfoContent* NewL();
	static CRrcAlmanacSatInfoContent* NewLC();
	~CRrcAlmanacSatInfoContent();
	
public:
	TUint8 iDataID;						// INTEGER (0..3),
	TUint8 iSatID;						// SatID,
	TBuf8<2> iE;						// BIT STRING (SIZE (16)),
	TBuf8<1> iToa;						// BIT STRING (SIZE (8)),
	TBuf8<2> iDeltaI;					// BIT STRING (SIZE (16)),
	TBuf8<2> iOmegaDot;					// BIT STRING (SIZE (16)),
	TBuf8<1> iSatHealth;				// BIT STRING (SIZE (8)),
	TBuf8<3> iASqrt;					// BIT STRING (SIZE (24)),
	TBuf8<3> iOmega0;					// BIT STRING (SIZE (24)),
	TBuf8<3> iM0;						// BIT STRING (SIZE (24)),
	TBuf8<3> iOmega;					// BIT STRING (SIZE (24)),
	TBuf8<2> iAf0;						// BIT STRING (SIZE (11)),
	TBuf8<2> iAf1;						// BIT STRING (SIZE (11))
	};


class CRrcAlmanacSatInfo : public CBase
	{
public:
	static CRrcAlmanacSatInfo* NewL(CRrcAlmanacSatInfoContent* aAlmanacSatInfoContent);
	static CRrcAlmanacSatInfo* NewLC(CRrcAlmanacSatInfoContent* aAlmanacSatInfoContent);
	CRrcAlmanacSatInfo(CRrcAlmanacSatInfoContent* aAlmanacSatInfoContent);
	~CRrcAlmanacSatInfo();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL(RAlmanacSatInfoBuilder* aAlmanacSatInfoBuilder);
private:
	CRrcAlmanacSatInfoContent* iAlmanacSatInfoContent;
	};


class CRrcAlmanacSatInfoListContent : public CBase
	{
public:
	static CRrcAlmanacSatInfoListContent* NewL();
	static CRrcAlmanacSatInfoListContent* NewLC();
	~CRrcAlmanacSatInfoListContent();

public:
	RPointerArray<CRrcAlmanacSatInfoContent> iAlmanacSatInfoList;
	};

class CRrcAlmanacSatInfoList : public CBase
	{
public:
	static CRrcAlmanacSatInfoList* NewL(CRrcAlmanacSatInfoListContent* aAlmanacSatInfoListContent);
	static CRrcAlmanacSatInfoList* NewLC(CRrcAlmanacSatInfoListContent* aAlmanacSatInfoListContent);
	CRrcAlmanacSatInfoList(CRrcAlmanacSatInfoListContent* aAlmanacSatInfoListContent);
	~CRrcAlmanacSatInfoList();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL(RUEPositioningGpsAlmanacBuilder* aGpsAlmanacBuilder);
private:
	
	CRrcAlmanacSatInfoListContent* iAlmanacSatInfoListContent;
	RPointerArray<CRrcAlmanacSatInfo> iAlmanacSatInfoList;
	};


class CRrcGpsAlmanacContent : public CBase
	{
public:
	static CRrcGpsAlmanacContent* NewL();
	static CRrcGpsAlmanacContent* NewLC();
	~CRrcGpsAlmanacContent();

public:
	TBool iSvGlobalHealthPresent;
	TBuf8<1> iWna;
	CRrcAlmanacSatInfoListContent* iAlmanacSatInfoListContent;
	TBuf8<46>SvGlobalHealth;

	};

class CRrcGpsAlmanac : public CBase
	{
public:	
	static CRrcGpsAlmanac* NewL(CRrcGpsAlmanacContent* aGpsAlmanacContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	static CRrcGpsAlmanac* NewLC(CRrcGpsAlmanacContent* aGpsAlmanacContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	~CRrcGpsAlmanac();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL();
	
private:
	CRrcGpsAlmanac(CRrcGpsAlmanacContent* aGpsAlmanacContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	void ConstructL();
	
	
private:
	CRrcGpsAlmanacContent* iGpsAlmanacContent;
	CRrcAlmanacSatInfoList* iAlmanacSatInfoList;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};

#endif // __RRCGPSALMANAC_H__
