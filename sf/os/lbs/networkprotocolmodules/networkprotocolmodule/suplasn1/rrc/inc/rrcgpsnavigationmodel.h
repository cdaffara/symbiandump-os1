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
// rrcgpsnavigationmodel - GPS reference navigation model encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __RRCGPSNAVIGATIONMODEL_H__
#define __RRCGPSNAVIGATIONMODEL_H__

#include <e32base.h>
#include "asn1per.h"

class RLbsAssistanceDataBuilderSet;
class RUEPositioningGpsNavigationModelBuilder;
class RNavigationModelSatInfoBuilder;

/*

NavigationModelSatInfo ::=			SEQUENCE {
	satID								SatID,
	satelliteStatus						SatelliteStatus,
	ephemerisParameter					EphemerisParameter		OPTIONAL
}

SatelliteStatus ::=					ENUMERATED {
										ns-NN-U,
										es-SN,
										es-NN-U,
										rev2,
										rev }
										



EphemerisParameter ::=						SEQUENCE {
	codeOnL2							BIT STRING (SIZE (2)),
	uraIndex							BIT STRING (SIZE (4)),
	satHealth							BIT STRING (SIZE (6)),
	iodc								BIT STRING (SIZE (10)),
	l2Pflag								BIT STRING (SIZE (1)),
	sf1Revd								SubFrame1Reserved,
	t-GD								BIT STRING (SIZE (8)),
	t-oc								BIT STRING (SIZE (16)),
	af2									BIT STRING (SIZE (8)),
	af1									BIT STRING (SIZE (16)),
	af0									BIT STRING (SIZE (22)),
	c-rs								BIT STRING (SIZE (16)),
	delta-n								BIT STRING (SIZE (16)),
	m0									BIT STRING (SIZE (32)),
	c-uc								BIT STRING (SIZE (16)),
	e									BIT STRING (SIZE (32)),
	c-us								BIT STRING (SIZE (16)),
	a-Sqrt								BIT STRING (SIZE (32)),
	t-oe								BIT STRING (SIZE (16)),
	fitInterval							BIT STRING (SIZE (1)),
	aodo								BIT STRING (SIZE (5)),
	c-ic								BIT STRING (SIZE (16)),
	omega0								BIT STRING (SIZE (32)),
	c-is								BIT STRING (SIZE (16)),
	i0									BIT STRING (SIZE (32)),
	c-rc								BIT STRING (SIZE (16)),
	omega								BIT STRING (SIZE (32)),
	omegaDot							BIT STRING (SIZE (24)),
	iDot								BIT STRING (SIZE (14))
}

*/										
										
enum TRrcSatelliteStatus
	{
	ERrcNsNNU,
	ERrcEsSN,
	ERrcEsNNU,
	ERrcRev2,
	ERrcRev
	};										


class CRrcSf1RevdContent : public CBase
	{
public:
	static CRrcSf1RevdContent* NewL();
	static CRrcSf1RevdContent* NewLC();
	~CRrcSf1RevdContent();

	
public:
 	TBuf8<3> iReserved1;					// BIT STRING (SIZE (23)),
	TBuf8<3> iReserved2;					// BIT STRING (SIZE (24)),
	TBuf8<3> iReserved3;					// BIT STRING (SIZE (24)),
	TBuf8<2> iReserved4;					// BIT STRING (SIZE (16))
	};
	
class CRrcSf1Revd : public CBase
	{
public:
	static CRrcSf1Revd* NewL(CRrcSf1RevdContent* aSf1RevdContent);
	static CRrcSf1Revd* NewLC(CRrcSf1RevdContent* aSf1RevdContent);
	~CRrcSf1Revd();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
	
private:
	CRrcSf1Revd(CRrcSf1RevdContent* aSf1RevdContent);
	void ConstructL();	
private:	
	CRrcSf1RevdContent* iSf1RevdContent;
	};	
	

class CRrcEphemerisParameterContent : public CBase
	{
public:
	static CRrcEphemerisParameterContent* NewL();
	static CRrcEphemerisParameterContent* NewLC();
	~CRrcEphemerisParameterContent();
public:

	TBuf8<1> iCodeOnL2;
	
	TBuf8<1> iUraIndex;					// BIT STRING (SIZE (4)),
	TBuf8<1> iSatHealth;				// BIT STRING (SIZE (6)),
	TBuf8<2> iIodc;						// BIT STRING (SIZE (10)),
	TBuf8<1> iL2Pflag;					// BIT STRING (SIZE (1)),
	CRrcSf1RevdContent* iSf1RevdContent;// SubFrame1Reserved,
	TBuf8<1> iTGD;						// BIT STRING (SIZE (8)),
	TBuf8<2> iToc;						// BIT STRING (SIZE (16)),
	TBuf8<1> iAf2;						// BIT STRING (SIZE (8)),
	TBuf8<2> iAf1;						// BIT STRING (SIZE (16)),
	TBuf8<3> iAf0;						// BIT STRING (SIZE (22)),
	TBuf8<2> iCrs;						// BIT STRING (SIZE (16)),
	TBuf8<2> iDeltaN;					// BIT STRING (SIZE (16)),
	TBuf8<4> iM0;						// BIT STRING (SIZE (32)),
	TBuf8<2> iCuc;						// BIT STRING (SIZE (16)),
	TBuf8<4> iE;						// BIT STRING (SIZE (32)),
	TBuf8<2> iCus;						// BIT STRING (SIZE (16)),
	TBuf8<4> iASqrt;					// BIT STRING (SIZE (32)),
	TBuf8<2> iToe;						// BIT STRING (SIZE (16)),
	TBuf8<1> iFitInterval;				// BIT STRING (SIZE (1)),
	TBuf8<1> iAodo;						// BIT STRING (SIZE (5)),
	TBuf8<2> iCic;						// BIT STRING (SIZE (16)),
	TBuf8<4> iOmega0;					// BIT STRING (SIZE (32)),
	TBuf8<2> iCis;						// BIT STRING (SIZE (16)),
	TBuf8<4> iI0;						// BIT STRING (SIZE (32)),
	TBuf8<2> iCrc;						// BIT STRING (SIZE (16)),
	TBuf8<4> iOmega;					// BIT STRING (SIZE (32)),
	TBuf8<3> iCmegaDot;					// BIT STRING (SIZE (24)),
	TBuf8<2> iDot;						// BIT STRING (SIZE (14))
		
	};

class CRrcEphemerisParameter : public CBase
	{
public:
	static CRrcEphemerisParameter* NewL(CRrcEphemerisParameterContent* aEphemerisParameterContent);
	static CRrcEphemerisParameter* NewLC(CRrcEphemerisParameterContent* aEphemerisParameterContent);
	~CRrcEphemerisParameter();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL(RNavigationModelSatInfoBuilder* aNavigationModelSatInfoBuilder);
	
private:
	CRrcEphemerisParameter(CRrcEphemerisParameterContent* aEphemerisParameterContent);
	void ConstructL();
	
private:
	CRrcEphemerisParameterContent* iEphemerisParameterContent;
	CRrcSf1Revd* iSf1Revd;	
	};





class CRrcNavigationModelSatInfoContent : public CBase
	{
public:
	static CRrcNavigationModelSatInfoContent* NewL();
	static CRrcNavigationModelSatInfoContent* NewLC();
	~CRrcNavigationModelSatInfoContent();


public:
	TBool iEphemerisParameterPresent;
	TUint8 iSatId;
	TRrcSatelliteStatus iSatelliteStatus;
	CRrcEphemerisParameterContent* iEphemerisParameterContent;
	};


class CRrcNavigationModelSatInfo : public CBase
	{
public:
	static CRrcNavigationModelSatInfo* NewL(CRrcNavigationModelSatInfoContent* aNavigationModelSatInfoContent);
	static CRrcNavigationModelSatInfo* NewLC(CRrcNavigationModelSatInfoContent* aNavigationModelSatInfoContent);
	~CRrcNavigationModelSatInfo();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL(RNavigationModelSatInfoBuilder* aNavigationModelSatInfoBuilder);
	
private:
	CRrcNavigationModelSatInfo(CRrcNavigationModelSatInfoContent* aNavigationModelSatInfoContent);
	void ConstructL();

	void WriteLBSAssistanceDataL();


private:
	CRrcNavigationModelSatInfoContent* iNavigationModelSatInfoContent;
	CRrcEphemerisParameter* iEphemerisParameter;	
	};










class CRrcNavigationModelSatInfoListContent : public CBase
	{
public:
	static CRrcNavigationModelSatInfoListContent* NewL();
	static CRrcNavigationModelSatInfoListContent* NewLC();
	~CRrcNavigationModelSatInfoListContent();

public:
	RPointerArray<CRrcNavigationModelSatInfoContent> iNavigationModelSatInfoListContent;
	};


class CRrcNavigationModelSatInfoList : public CBase
	{
public:
	static CRrcNavigationModelSatInfoList* NewL(CRrcNavigationModelSatInfoListContent* aRrcNavigationModelSatInfoListContent);
	static CRrcNavigationModelSatInfoList* NewLC(CRrcNavigationModelSatInfoListContent* aRrcNavigationModelSatInfoListContent);
	~CRrcNavigationModelSatInfoList();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL(RUEPositioningGpsNavigationModelBuilder* aGpsNavigationModelBuilder);
	
private:
	CRrcNavigationModelSatInfoList(CRrcNavigationModelSatInfoListContent* aRrcNavigationModelSatInfoListContent);
	void ConstructL();

private:
	RPointerArray<CRrcNavigationModelSatInfo> iNavigationModelSatInfoArray;
	CRrcNavigationModelSatInfoListContent* iNavigationModelSatInfoArrayContent;	
	};






class CRrcGpsNavigationModelContent : public CBase
	{

public:
	static CRrcGpsNavigationModelContent* NewL();
	static CRrcGpsNavigationModelContent* NewLC();
	~CRrcGpsNavigationModelContent();

public:
	CRrcNavigationModelSatInfoListContent* iNavigationModelSatInfoListContent;	
	};


class CRrcGpsNavigationModel : public CBase
	{
public:
	static CRrcGpsNavigationModel* NewL(CRrcGpsNavigationModelContent* aGpsNavigationModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	static CRrcGpsNavigationModel* NewLC(CRrcGpsNavigationModelContent* aGpsNavigationModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	~CRrcGpsNavigationModel();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL();
	
private:
	CRrcGpsNavigationModel(CRrcGpsNavigationModelContent* aGpsNavigationModelContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	void ConstructL();
	
private:
	CRrcGpsNavigationModelContent* iGpsNavigationModelContent;
	CRrcNavigationModelSatInfoList* iNavigationModelSatInfoList;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};


#endif // __RRCGPSNAVIGATIONMODEL_H__
