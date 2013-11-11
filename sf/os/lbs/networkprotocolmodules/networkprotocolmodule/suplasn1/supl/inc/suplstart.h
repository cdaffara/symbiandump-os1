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
// SUPL START message - Client interface for SUPL START encoding & decoding.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __SUPLSTART_H__
#define __SUPLSTART_H__

#include <e32base.h>
#include <lbs/test/suplmessage.h>

class CAsn1PerEncoding;
class CSuplFrequencyInfo;
class CSuplFrequencyInfoContent;

enum TSuplLocationStatus
	{
	EStale = 0,
	ECurrent = 1,
	EUnknown = 2,
	};

enum TCellInformationChoice
	{
	EGsmCellInformation = 0,
	EWcdmaCellInformation = 1,
	ECdmaCellInformation = 2
	};

enum TFrequencyInfoChoice
	{
	EFddFrequency,
	ETddFrequency
	};

enum TModeSpecificInfoChoice
	{
	EFddMode = 0,
	ETddMode = 1
	};

enum TSuplPrefMethod
	{
	EAGpsSETassistedPreferred=0,
	EAGpsSETBasedPreferred=1,
	ENoPreference=3
	};

/**
 * The class holds Pos protocol data element of the SUPL START Messages,
 */
class CSuplPosProtocolContent : public CBase
	{
public:
	IMPORT_C static CSuplPosProtocolContent* NewL();
	IMPORT_C static CSuplPosProtocolContent* NewLC();
	IMPORT_C ~CSuplPosProtocolContent();
	TBool operator==(const CSuplPosProtocolContent& other) const;		
	
public:
	CSuplPosProtocolContent();
	TBool iTia801;
	TBool iRrlp;
	TBool iRrc;
	};
	
/**
 * The class encodes & decodes the SUPL START Messages Pos Protocol.
 */	

class CSuplPosProtocol : public CBase
	{
public:
	static CSuplPosProtocol* NewL(CSuplPosProtocolContent* aPosProtocolContent);
	static CSuplPosProtocol* NewLC(CSuplPosProtocolContent* aPosProtocolContent);
	~CSuplPosProtocol();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplPosProtocol(CSuplPosProtocolContent* aPosProtocolContent);
	CSuplPosProtocolContent* iPosProtocolContent;	
	};


/**
 * The class holds PosTechnology data element of the SUPL START Messages,
 */
class CSuplPosTechnologyContent : public CBase
	{
public:
	IMPORT_C static CSuplPosTechnologyContent* NewL();
	IMPORT_C static CSuplPosTechnologyContent* NewLC();
	IMPORT_C ~CSuplPosTechnologyContent();
	TBool operator==(const CSuplPosTechnologyContent& other) const;	
	
private:
	CSuplPosTechnologyContent();
	void ConstructL();	
	
public:
	TBool iAgpsSETassisted;
	TBool iAgpsSETBased;
	TBool iAutonomousGPS;
	TBool iAFLT;
	TBool iECID;
	TBool iEOTD;
	TBool iOTDOA;
	};

/**
 * The class encodes & decodes the SUPL START Messages Pos Technology.
 */	

class CSuplPosTechnology : public CBase
	{
public:
	static CSuplPosTechnology* NewL(CSuplPosTechnologyContent* aPosTechnologyContent);
	static CSuplPosTechnology* NewLC(CSuplPosTechnologyContent* aPosTechnologyContent);
	~CSuplPosTechnology();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplPosTechnology(CSuplPosTechnologyContent* aPosTechnologyContent);
	void ConstructL();
	CSuplPosTechnologyContent* iPosTechnologyContent;	
	};

/**
 * The class holds SetCapabilities data element of the SUPL START Messages,
 */
class CSuplSetCapabilitiesContent:public CBase
	{
public:
	IMPORT_C static CSuplSetCapabilitiesContent* NewL();	
	IMPORT_C static CSuplSetCapabilitiesContent* NewLC();
	IMPORT_C ~CSuplSetCapabilitiesContent();
	TBool operator==(const CSuplSetCapabilitiesContent& other) const;
	
private:	
	void ConstructL();
	
public:
	CSuplPosTechnologyContent* iPosTechnologyContent;
	TSuplPrefMethod iPrefMethod;
	CSuplPosProtocolContent* iPosProtocolContent;
	};
	
/**
 * The class decodes the SUPL START Messages Set Capabilities.
 */	
class CSuplSetCapabilities:public CBase
	{
public:
	static CSuplSetCapabilities* NewL(CSuplSetCapabilitiesContent* aSetCapabilitiesContent);
	static CSuplSetCapabilities* NewLC(CSuplSetCapabilitiesContent* aSetCapabilitiesContent);
	~CSuplSetCapabilities();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplSetCapabilities(CSuplSetCapabilitiesContent* aSetCapabilitiesContent);
	void ConstructL();
	CSuplSetCapabilitiesContent* iSetCapabilitiesContent;
	CSuplPosTechnology* iPosTechnology;
	CSuplPosProtocol* iPosProtocol;
	};


class CSuplGsmCellInformationContent : public CBase
	{
public:
	IMPORT_C static CSuplGsmCellInformationContent* NewL();	
	IMPORT_C static CSuplGsmCellInformationContent* NewLC();
	IMPORT_C ~CSuplGsmCellInformationContent();
	TBool operator==(const CSuplGsmCellInformationContent& other) const;	
	
private:
	CSuplGsmCellInformationContent();	
	void ConstructL();

public:	
	TBool iNMRPresent;
	TBool iTAPresent;
	TUint16 iRefMCC;  //INTEGER(0..999), -- Mobile Country Code
	TUint16 iRefMNC;  //INTEGER(0..999), -- Mobile Network Code
	TUint16 iRefLAC;  //INTEGER(0..65535), -- Location area code
	TUint16 iRefCI;   //INTEGER(0..65535), -- Cell identity
	//CSuplNMRContent* iNMRContent;    //NMR OPTIONAL,
	TUint8 iTA;       //INTEGER(0..255) OPTIONAL, --Timing Advance
		
	};

class CSuplGsmCellInformation : public CBase
	{
public:	
	static CSuplGsmCellInformation* NewL(CSuplGsmCellInformationContent* aGsmCellInformationContent);
	static CSuplGsmCellInformation* NewLC(CSuplGsmCellInformationContent* aGsmCellInformationContent);
	~CSuplGsmCellInformation();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplGsmCellInformation(CSuplGsmCellInformationContent* aGsmCellInformationContent);
	void ConstructL();
	CSuplGsmCellInformationContent* iGsmCellInformationContent;
	//CSuplNMR* iNMR;
	};
	
class CSuplCdmaCellInformationContent : public CBase
	{
		
public:
	IMPORT_C static CSuplCdmaCellInformationContent* NewL();	
	IMPORT_C static CSuplCdmaCellInformationContent* NewLC();
	IMPORT_C ~CSuplCdmaCellInformationContent();
	TBool operator==(const CSuplCdmaCellInformationContent& other) const;	
	
private:
	CSuplCdmaCellInformationContent();
	void ConstructL();

public:
	TUint16 iRefNID;	// INTEGER(0..65535), -- Network Id
	TUint16 iRefSID;	// INTEGER(0..32767), -- System Id
	TUint16 iRefBASEID; // INTEGER(0..65535), -- Base Station Id
	TUint32 iRefBASELAT;// INTEGER(0..4194303), -- Base Station Latitude
	TUint32 iReBASELONG;// INTEGER(0..8388607), -- Base Station Longitude
	TUint16 iRefREFPN;	// INTEGER(0..511), -- Base Station PN Code
	TUint16 iRefWeekNumber;	// INTEGER(0..65535), -- GPS Week Number
	TUint32 iRefSeconds;	// INTEGER(0..4194303)-- GPS Seconds -- ,
	};
	
class CSuplCdmaCellInformation : public CBase
	{
public:	
	static CSuplCdmaCellInformation* NewL(CSuplCdmaCellInformationContent* aCdmaCellInformationContent);
	static CSuplCdmaCellInformation* NewLC(CSuplCdmaCellInformationContent* aCdmaCellInformationContent);
	~CSuplCdmaCellInformation();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplCdmaCellInformation(CSuplCdmaCellInformationContent* aCdmaCellInformationContent);
	void ConstructL();
	CSuplCdmaCellInformationContent* iCdmaCellInformationContent;	
	};
	
/**
 * The class holds WcdmaCellInformation of the Supl Location Messages,
 */
class CSuplWcdmaCellInformationContent : public CBase
	{
public:
	IMPORT_C static CSuplWcdmaCellInformationContent* NewL();	
	IMPORT_C static CSuplWcdmaCellInformationContent* NewLC();
	IMPORT_C ~CSuplWcdmaCellInformationContent();
	TBool operator==(const CSuplWcdmaCellInformationContent& other) const;	
	
private:	
	void ConstructL();
	
public:
	TBool iFrequencyInfoPresent; // EFalse
	TBool iPrimaryScramblingCodePresent; // EFalse
	TBool iMeasuredResultsListPresent; // EFalse
	TUint16 iRefMCC;	//INTEGER(0..999), -- Mobile Country Code
	TUint16 iRefMNC;	//INTEGER(0..999), -- Mobile Network Code
	TUint32 iRefUC;		//INTEGER(0..268435455), -- Cell identity
	//CSuplFrequencyInfoContent* iFrequencyInfoContent;          //FrequencyInfo OPTIONAL,
	//TUint16 iPrimaryScramblingCode;				//INTEGER(0..511) OPTIONAL,
	//CSuplMeasuredResultsListContent* iMeasuredResultsListContent;    //MeasuredResultsList OPTIONAL,
	};

/**
 * The class encodes and decodes the SUPL START Messages location Id,
 */
class CSuplWcdmaCellInformation : public CBase
	{
public:	
	static CSuplWcdmaCellInformation* NewL(CSuplWcdmaCellInformationContent* aWcdmaCellInformationContent);
	static CSuplWcdmaCellInformation* NewLC(CSuplWcdmaCellInformationContent* aWcdmaCellInformationContent);
	~CSuplWcdmaCellInformation();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplWcdmaCellInformation(CSuplWcdmaCellInformationContent* aWcdmaCellInformationContent);
	void ConstructL();
	//CSuplFrequencyInfo* iFrequencyInfo;
	//CSuplMeasuredResultsList* iMeasuredResultsList;
	CSuplWcdmaCellInformationContent* iWcdmaCellInformationContent;
	};
	

/**
 * The class holds data element of cell info,
 */
class CSuplCellInfoContent : public CBase
	{
public:	
	IMPORT_C static CSuplCellInfoContent* NewL();
	IMPORT_C static CSuplCellInfoContent* NewLC();
	IMPORT_C ~CSuplCellInfoContent();
	TBool operator==(const CSuplCellInfoContent& other) const;
	
private:
	void ConstructL();
	CSuplCellInfoContent();
	
public:
	TCellInformationChoice iCellInformation;
	CSuplGsmCellInformationContent* iGsmCellInformationContent;
	CSuplWcdmaCellInformationContent* iWcdmaCellInformationContent;
	CSuplCdmaCellInformationContent* iCdmaCellInformationContent;
	};


/**
 * The class decodes the cell info,
 */
class CSuplCellInfo : public CBase
	{
public:	
	static CSuplCellInfo* NewL(CSuplCellInfoContent* aCellInfoContent);
	static CSuplCellInfo* NewLC(CSuplCellInfoContent* aCellInfoContent);
	~CSuplCellInfo();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplCellInfo(CSuplCellInfoContent* aCellInfoContent);
	void ConstructL();
	CSuplCellInfoContent* iCellInfoContent;
	CSuplGsmCellInformation* iGsmCellInformation;
	CSuplWcdmaCellInformation* iWcdmaCellInformation;
	CSuplCdmaCellInformation* iCdmaCellInformation;		
	};


/**
 * The class holds data element of the Supl Location Messages,
 */
	
class CSuplLocationIdContent : public CBase
	{
public:	
	IMPORT_C static CSuplLocationIdContent* NewL();
	IMPORT_C static CSuplLocationIdContent* NewLC();
	IMPORT_C ~CSuplLocationIdContent();
	TBool operator==(const CSuplLocationIdContent& other) const;
	
private:
	CSuplLocationIdContent();
	void ConstructL();
public:
	CSuplCellInfoContent* iCellInfoContent;
	TSuplLocationStatus iLocationStatus;
	};


/**
 * The class decodes the SUPL START Messages location Id,
 */
class CSuplLocationId : public CBase
	{
public:	
	static CSuplLocationId* NewL(CSuplLocationIdContent* aLocationIdContent);
	static CSuplLocationId* NewLC(CSuplLocationIdContent* aLocationIdContent);
	~CSuplLocationId();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplLocationId(CSuplLocationIdContent* aLocationIdContent);
	void ConstructL();
	CSuplLocationIdContent* iLocationIdContent;
	CSuplCellInfo* iCellInfo;
	};

/**
 * The class holds QoP data of the Supl Location Messages,
 */
class CSuplQoPContent : public CBase
	{
public:
	IMPORT_C static CSuplQoPContent* NewL();
	IMPORT_C static CSuplQoPContent* NewLC();
	IMPORT_C ~CSuplQoPContent();
	TBool operator==(const CSuplQoPContent& other) const;

private:
	CSuplQoPContent();
	void ConstructL();
public:
	TBool iVeraccPresent;
	TBool iMaxLocAgePresent;
	TBool iDelayPresent;
	
	TInt iHoracc;
	};

/*
* This class encodes parameter QoP in message SUPL START
*/
class CSuplQoP : public CBase
	{
public:
	static CSuplQoP* NewL(CSuplQoPContent* aQoPContent);
	static CSuplQoP* NewLC(CSuplQoPContent* aQoPContent);
	~CSuplQoP();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBits);
	
private:
	CSuplQoP(CSuplQoPContent* aQoPContent);
	void ConstructL();
	CSuplQoPContent* iQoPContent;
	};

/**
 * The class holds data element of the Supl Start Messages,
 */
class CSuplStartContent : public CBase
	{
public:
	IMPORT_C static CSuplStartContent* NewL();
	IMPORT_C static CSuplStartContent* NewLC();
	IMPORT_C~CSuplStartContent();	
	TBool operator==(const CSuplStartContent& other) const;
	
private:
	CSuplStartContent();
	void ConstructL();
	
public:	
	TBool iQopPresent;
	CSuplSetCapabilitiesContent* iSetCapabilitiesContent;
	CSuplLocationIdContent* iLocationIdContent;
	CSuplQoPContent* iQoPContent;
	};

/**
 * The class encodes and decodes the SUPL START Messages,
 * which is sent from SET to SLP
 */
class CSuplStart : public CBase
	{
public:
	static CSuplStart* NewL(CSuplStartContent* aSuplStartContent);
	static CSuplStart* NewLC(CSuplStartContent* aSuplStartContent);
	~CSuplStart();
	void EncodeL(CAsn1PerEncoding& aPerEncoding);
	void DecodeFromL(const TDesC8& aBufferToDecode, 
					const TInt aStartBitPos,
					TInt& aDecodedBitsLength);
	
private:
	CSuplStart(CSuplStartContent* aSuplStartContent);
	void ConstructL();
	CSuplSetCapabilities* iSetCap;
	CSuplLocationId* iLocationId;
	CSuplQoP* iQoP;
	CSuplStartContent* iSuplStartContent;
	};

#endif // __SUPLSTART_H__
