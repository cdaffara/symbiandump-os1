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
// SUPL POS INIT - Client interface for SUPL POS INIT encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __SUPLPOSINIT_H__
#define __SUPLPOSINIT_H__

#include <e32base.h>
#include <lbs/test/suplmessage.h>
#include <lbs/test/suplstart.h>
#include <lbs/test/suplpos.h>

class CSuplPositionContent;
class CSuplPositionEstimateContent;
class CSuplPositionEstimate;

enum TSuplLatitudSign
	{
	ENorth=0,
	ESouth=1,
	};

class CSuplPosition : public CBase
	{
public:	
	static CSuplPosition* NewL(CSuplPositionContent* aPositionContent);
	static CSuplPosition* NewLC(CSuplPositionContent* aPositionContent);
	~CSuplPosition();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	
private:
	CSuplPosition(CSuplPositionContent* aPositionContent);
	void ConstructL();
	CSuplPositionContent* iPositionContent;
	CSuplPositionEstimate* iPositionEstimate;
	};

class CSuplPositionContent : public CBase
	{
public:	
	IMPORT_C static CSuplPositionContent* NewL();
	IMPORT_C static CSuplPositionContent* NewLC();
	IMPORT_C ~CSuplPositionContent();
	TBool operator==(const CSuplPositionContent& other) const;
public:
	TBool iVelocityPresent;
	TTime iUtcTimeStamp;	// UTCTime Per encoding- shall include seconds and shall use UTC time.
	CSuplPositionEstimateContent* iPositionEstimateContent;	
	
private:
	CSuplPositionContent();
	void ConstructL();
	};

class CSuplPositionEstimate
	{
public:
	static CSuplPositionEstimate* NewL(CSuplPositionEstimateContent* aPositionEstimateContent);
	static CSuplPositionEstimate* NewLC(CSuplPositionEstimateContent* aPositionEstimateContent);
	~CSuplPositionEstimate();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);

private:
	CSuplPositionEstimate(CSuplPositionEstimateContent* aPositionEstimateContent);
	void ConstructL();
	CSuplPositionEstimateContent* iPositionEstimateContent;
	};

class CSuplPositionEstimateContent
	{
public:
	IMPORT_C static CSuplPositionEstimateContent* NewL();
	IMPORT_C static CSuplPositionEstimateContent* NewLC();
	IMPORT_C ~CSuplPositionEstimateContent();
	TBool operator==(const CSuplPositionEstimateContent& other) const;
public:
	TBool iUncertainty;
	TBool iConfidence;
	TBool iAltitudInfo;
	TSuplLatitudSign iLatitudeSign;	// ENUMERATED (North or South) 
	TUint32 iLatitude;  				// INTEGER (0..8388607),
	TInt32 iLongitude;				// INTEGER (-8388607...8388607)
private:
	CSuplPositionEstimateContent();
	void ConstructL();
	};

class CSuplRequestedAssistDataContent : public CBase
	{
public:	
	IMPORT_C static CSuplRequestedAssistDataContent* NewL();
	IMPORT_C static CSuplRequestedAssistDataContent* NewLC();
	IMPORT_C ~CSuplRequestedAssistDataContent();
	TBool operator==(const CSuplRequestedAssistDataContent& other) const;
	
private:
	void ConstructL();	
	
public:
	TBool iNavigationModelDataPresent;
	TBool iAlmanacRequested;
	TBool iUtcModelRequested;
	TBool iIonosphericModelRequested;
	TBool iDgpsCorrectionsRequested;
	TBool iReferenceLocationRequested;
	TBool iReferenceTimeRequested;
	TBool iAcquisitionAssistanceRequested;
	TBool iRealTimeIntegrityRequested;
	TBool iNavigationModelRequested;
	//CSuplNavigationModelDataContent* iNavigationModelDataConent;			
	};
	
class CSuplRequestedAssistData : public CBase
	{
public:

	static CSuplRequestedAssistData* NewL(CSuplRequestedAssistDataContent* aRequestedAssistDataContent);
	static CSuplRequestedAssistData* NewLC(CSuplRequestedAssistDataContent* aRequestedAssistDataContent);
	~CSuplRequestedAssistData();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);

private:
	CSuplRequestedAssistData(CSuplRequestedAssistDataContent* aRequestedAssistDataContent);
	void ConstructL();
	CSuplRequestedAssistDataContent* iRequestedAssistDataContent;
	//CSuplNavigationModelData* iNavigationModelData;		
	};	


class CSuplPosInitContent : public CBase
	{
public:
	IMPORT_C static CSuplPosInitContent* NewL();
	IMPORT_C static CSuplPosInitContent* NewLC();
	IMPORT_C~CSuplPosInitContent();
	TBool operator==(const CSuplPosInitContent& other) const;
	
private:
	CSuplPosInitContent();
	
public:	

	TBool iRequestedAssistDataPresent; //ETrue
	TBool iPositionPresent; //EFalse
	TBool iSuplPosPresent; //EFalse
	TBool iVerPresent; //EFalse
	CSuplSetCapabilitiesContent* iSetCapContent;
	CSuplRequestedAssistDataContent* iRequestedAssistDataContent;
	CSuplLocationIdContent* iLocationIdContent;
	CSuplPositionContent* iPositionContent;
	//CSuplPosContent* iSuplPosContent;
	//TBuf8<8> iVer;
	};


/**
 * The class defines the SUPL POS INIT Message, 
 * which is the initial message from SLP to SET
 */
class CSuplPosInit : public CBase
	{
public:
	static CSuplPosInit* NewL(CSuplPosInitContent* aSuplPosInitContent);
	static CSuplPosInit* NewLC(CSuplPosInitContent* aSuplPosInitContent);
	~CSuplPosInit();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplPosInit(CSuplPosInitContent* aSuplPosInitContent);
	void ConstructL();
	CSuplPosInitContent* iSuplPosInitContent;
	CSuplSetCapabilities* iSetCap;
	CSuplRequestedAssistData* iRequestedAssistData;
	CSuplLocationId* iLocationId;
	CSuplPosition* iPosition;
	//CSuplPos * iSuplPos;
	};

#endif // __SUPLPOSINIT_H__
