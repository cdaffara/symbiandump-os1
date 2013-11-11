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
// Assistancedatadelivery Message - Client interface for Assistance data delivery encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/
#ifndef __RRCASSISTANCEDATA_H__
#define __RRCASSISTANCEDATA_H__

#include <e32base.h>
#include <lbs/lbsassistancedatabuilderset.h>


class CAsn1PerEncoding;
class CRrcGpsReferenceTime;
class CRrcReferenceLocation;
class CRrcGpsNavigationModel;
class CRrcGpsIonosphericModel;
class CRrcGpsUTCModel;
class CRrcGpsAlmanac;
class CRrcGpsAcquisitionAssistance;
class CRrcGpsBadSatList;
class CRrcGpsReferenceTimeContent;
class CRrcReferenceLocationContent;
class CRrcGpsNavigationModelContent;
class CRrcGpsIonosphericModelContent;
class CRrcGpsUTCModelContent;
class CRrcGpsAlmanacContent;
class CRrcGpsAcquisitionAssistanceContent;
class CRrcGpsBadSatListContent;


class CRrcGpsAssistanceDataContent : public CBase
	{
public:
	static CRrcGpsAssistanceDataContent* NewL();
	static CRrcGpsAssistanceDataContent* NewLC();
	~CRrcGpsAssistanceDataContent();

private:
	CRrcGpsAssistanceDataContent();
	void ConstructL();
	
public:	
	TBool iRrcGpsReferenceTimePresent;
	TBool iRrcReferenceLocationPresent;
	TBool iRrcGpsDgpsCorrectionsPresent; //Should be EFalse
	TBool iRrcGpsNavigationModelPresent;
	TBool iRrcGpsIonosphericModelPresent;
	TBool iRrcGpsUTCModelPresent;
	TBool iRrcGpsAlmanacPresent;
	TBool iRrcGpsAcquisitionAssistancePresent;
	TBool iRrcGpsBadSatListPresent;
	TBool iDummyPresent;
	CRrcGpsReferenceTimeContent* iRrcGpsReferenceTimeContent;
	CRrcReferenceLocationContent* iRrcReferenceLocationContent;
	//CRrcGpsDgpsCorrectionsContent* iRrcGpsDgpsCorrectionsContent;
	CRrcGpsNavigationModelContent* iRrcGpsNavigationModelContent;
	CRrcGpsIonosphericModelContent* iRrcGpsIonosphericModelContent;
	CRrcGpsUTCModelContent* iRrcGpsUTCModelContent;
	CRrcGpsAlmanacContent* iRrcGpsAlmanacContent;
	CRrcGpsAcquisitionAssistanceContent* iRrcGpsAcquisitionAssistanceContent;
	CRrcGpsBadSatListContent* iRrcGpsBadSatListContent;
	};


class CRrcGpsAssistanceData : public CBase
	{
public:
	static CRrcGpsAssistanceData* NewL(CRrcGpsAssistanceDataContent* aGpsAssistanceDataContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	static CRrcGpsAssistanceData* NewLC(CRrcGpsAssistanceDataContent* aGpsAssistanceDataContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	~CRrcGpsAssistanceData();

	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL();

private:
	void ConstructL();
	CRrcGpsAssistanceData(CRrcGpsAssistanceDataContent* aGpsAssistanceDataContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	
	CRrcGpsAssistanceDataContent* iGpsAssistanceDataContent;
	CRrcGpsReferenceTime* iRrcGpsReferenceTime;
	CRrcReferenceLocation* iRrcReferenceLocation;
	//CRrcGpsDgpsCorrections* iRrcGpsDgpsCorrections;
	CRrcGpsNavigationModel* iRrcGpsNavigationModel;
	CRrcGpsIonosphericModel* iRrcGpsIonosphericModel;
	CRrcGpsUTCModel* iRrcGpsUTCModel;
	CRrcGpsAlmanac* iRrcGpsAlmanac;
	CRrcGpsAcquisitionAssistance* iRrcGpsAcquisitionAssistance;
	CRrcGpsBadSatList* iRrcGpsBadSatList;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};

class CAssistanceDataDeliveryR3IEsContent : public CBase
	{
public:
	static CAssistanceDataDeliveryR3IEsContent* NewL();
	static CAssistanceDataDeliveryR3IEsContent* NewLC();
	~CAssistanceDataDeliveryR3IEsContent();

private:
	CAssistanceDataDeliveryR3IEsContent();
	void ConstructL();

public:	
	TUint8 iRrcTransactionIdentifier;
	TBool iRrcGpsAssistanceDataPresent;
	TBool iRrcOTDOAAssistanceDataUEBPresent; // Should be EFalse
	CRrcGpsAssistanceDataContent* iRrcGpsAssistanceDataContent;
	};

class CAssistanceDataDeliveryR3IEs : public CBase
	{
public:	
	IMPORT_C static CAssistanceDataDeliveryR3IEs* NewL(RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	IMPORT_C static CAssistanceDataDeliveryR3IEs* NewLC(RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	IMPORT_C virtual ~CAssistanceDataDeliveryR3IEs();
	IMPORT_C void EncodeL(RBuf8&);
	IMPORT_C void DecodeL(const TDesC8& aBufToDecode, const TInt aStartPos);

private:
	CAssistanceDataDeliveryR3IEs(RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	void ConstructL();
	CRrcGpsAssistanceData* iRrcGpsAssistanceData;
	CAssistanceDataDeliveryR3IEsContent* iAssistanceDataDeliveryR3IEsContent;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};

#endif // __RRCASSISTANCEDATA_H__
