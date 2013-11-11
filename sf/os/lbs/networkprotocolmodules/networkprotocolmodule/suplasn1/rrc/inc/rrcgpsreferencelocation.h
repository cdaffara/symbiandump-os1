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
// rrcgpsreferencelocation - GPS reference location encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/
#ifndef __RRCGPSREFERENCELOCATION_H__
#define __RRCGPSREFERENCELOCATION_H__

#include <e32base.h>
#include "asn1per.h"

class RLbsAssistanceDataBuilderSet;
class RReferenceLocationBuilder;

enum TRrcLatitudeSign
	{
	ELatitudeSignNorth,
	ELatitudeSignSouth	
	};

enum TRrcAltitudeDirection
	{
	EAltitudeDirectionHeight,
	EAltitudeDirectionDepth
	};



class CRrcEllipsoidPointAltitudeEllipsoideContent : public CBase
	{
public:	
public:
	static CRrcEllipsoidPointAltitudeEllipsoideContent* NewL();
	static CRrcEllipsoidPointAltitudeEllipsoideContent* NewLC();
	~CRrcEllipsoidPointAltitudeEllipsoideContent();

private:
	CRrcEllipsoidPointAltitudeEllipsoideContent();
	void ConstructL();
	
public:
	TRrcLatitudeSign iLatitudeSign;
	TUint32 ilatitude;		//	INTEGER (0..8388607),
	TInt ilongitude;		//	INTEGER (-8388608..8388607),
	TRrcAltitudeDirection iAltitudeDirection;	
	TUint16 iAltitude;		//	INTEGER (0..32767),
	TUint8 iUncertaintySemiMajor;	// INTEGER (0..127),
	TUint8 iUncertaintySemiMinor;	// INTEGER (0..127),
	TUint8 iOrientationMajorAxis;	// INTEGER (0..89),
	TUint8 iUncertaintyAltitude;	// INTEGER (0..127),
	TUint8 iConfidence;				// INTEGER (0..100)
	};


class CRrcEllipsoidPointAltitudeEllipsoide : public CBase
	{
public:
	static CRrcEllipsoidPointAltitudeEllipsoide* NewL(CRrcEllipsoidPointAltitudeEllipsoideContent* aEllipsoidPointAltitudeEllipsoideContent);
	static CRrcEllipsoidPointAltitudeEllipsoide* NewLC(CRrcEllipsoidPointAltitudeEllipsoideContent* aEllipsoidPointAltitudeEllipsoideContent);
	~CRrcEllipsoidPointAltitudeEllipsoide();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL(RReferenceLocationBuilder* aReferenceLocationBuilder);
	
private:
	CRrcEllipsoidPointAltitudeEllipsoide(CRrcEllipsoidPointAltitudeEllipsoideContent* aEllipsoidPointAltitudeEllipsoideContent);
	void ConstructL();
	CRrcEllipsoidPointAltitudeEllipsoideContent* iEllipsoidPointAltitudeEllipsoideContent;
	};





class CRrcReferenceLocationContent : public CBase
	{
public:
	static CRrcReferenceLocationContent* NewL();
	static CRrcReferenceLocationContent* NewLC();
	~CRrcReferenceLocationContent();

private:
	CRrcReferenceLocationContent();


public:
	CRrcEllipsoidPointAltitudeEllipsoideContent* iEllipsoidPointAltitudeEllipsoideContent;
	};

class CRrcReferenceLocation : public CBase
	{
public:
	static CRrcReferenceLocation* NewL(CRrcReferenceLocationContent* aReferenceLocationContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	static CRrcReferenceLocation* NewLC(CRrcReferenceLocationContent* aReferenceLocationContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	~CRrcReferenceLocation();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	void WriteLBSAssistanceDataL();
	
private:
	CRrcReferenceLocation(CRrcReferenceLocationContent* aReferenceLocationContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet);
	void ConstructL();
	CRrcReferenceLocationContent* iReferenceLocationContent;
	CRrcEllipsoidPointAltitudeEllipsoide* iEllipsoidPointAltitudeEllipsoide;
	RLbsAssistanceDataBuilderSet* iLbsAssistanceDataBuilderSet;
	};







#endif // __RRCGPSREFERENCELOCATION_H__
