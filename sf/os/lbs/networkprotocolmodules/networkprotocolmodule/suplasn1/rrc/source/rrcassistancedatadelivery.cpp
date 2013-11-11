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
// Implementation of encoding and decoding Assistance data delivery
// 
//

#include "rrcassistancedatadelivery.h"
#include "asn1per.h"
#include "rrcgpsreferencetime.h"
#include "rrcgpsreferencelocation.h"
#include "rrcgpsutcmodel.h"
#include "rrcgpsnavigationmodel.h"
#include "rrcgpsionosphericmodel.h"
#include "rrcgpsbadsatlist.h"
#include "rrcgpsalmanac.h"
#include "rrcgpsacquisitionassistance.h"


CRrcGpsAssistanceDataContent* CRrcGpsAssistanceDataContent::NewL()
	{
	CRrcGpsAssistanceDataContent* self = CRrcGpsAssistanceDataContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsAssistanceDataContent* CRrcGpsAssistanceDataContent::NewLC()
	{
	CRrcGpsAssistanceDataContent* self = new (ELeave) CRrcGpsAssistanceDataContent();
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsAssistanceDataContent::~CRrcGpsAssistanceDataContent()
	{	
	delete iRrcGpsReferenceTimeContent;
	delete iRrcReferenceLocationContent;
	delete iRrcGpsNavigationModelContent;
	delete iRrcGpsIonosphericModelContent;
	delete iRrcGpsUTCModelContent;
	delete iRrcGpsAlmanacContent;
	delete iRrcGpsAcquisitionAssistanceContent;
	delete iRrcGpsBadSatListContent;	
	}

CRrcGpsAssistanceDataContent::CRrcGpsAssistanceDataContent()
	{
	}




CRrcGpsAssistanceData* CRrcGpsAssistanceData::NewL(CRrcGpsAssistanceDataContent* aGpsAssistanceDataContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsAssistanceData* self = CRrcGpsAssistanceData::NewLC(aGpsAssistanceDataContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::Pop(self);
	return self;					
	}

CRrcGpsAssistanceData* CRrcGpsAssistanceData::NewLC(CRrcGpsAssistanceDataContent* aGpsAssistanceDataContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CRrcGpsAssistanceData* self = new (ELeave) CRrcGpsAssistanceData(aGpsAssistanceDataContent, aLbsAssistanceDataBuilderSet);
	CleanupStack::PushL(self);
	return self;
	}

CRrcGpsAssistanceData::~CRrcGpsAssistanceData()
	{
	delete iRrcGpsReferenceTime;
	delete iRrcReferenceLocation;
	delete iRrcGpsNavigationModel;
	delete iRrcGpsIonosphericModel;
	delete iRrcGpsUTCModel;
	delete iRrcGpsAlmanac;
	delete iRrcGpsAcquisitionAssistance;
	delete iRrcGpsBadSatList;
	}

CRrcGpsAssistanceData::CRrcGpsAssistanceData(CRrcGpsAssistanceDataContent* aGpsAssistanceDataContent, RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
:iGpsAssistanceDataContent(aGpsAssistanceDataContent), iLbsAssistanceDataBuilderSet(aLbsAssistanceDataBuilderSet)
	{
	}

void CRrcGpsAssistanceData::EncodeL(CAsn1PerEncoding& /*aPerEncoding*/)
	{
	User::Leave(KErrNotSupported);
	}

void CRrcGpsAssistanceData::DecodeFromL(const TDesC8& aBufferToDecode, const TInt aStartBitPos, TInt& aDecodedBits)
	{
		//	UE-Positioning-GPS-AssistanceData ::=			SEQUENCE {
		//		ue-positioning-GPS-ReferenceTime				UE-Positioning-GPS-ReferenceTime				OPTIONAL,
		//		ue-positioning-GPS-ReferenceLocation			ReferenceLocation				OPTIONAL,
		//		ue-positioning-GPS-DGPS-Corrections				UE-Positioning-GPS-DGPS-Corrections				OPTIONAL,
		//		ue-positioning-GPS-NavigationModel				UE-Positioning-GPS-NavigationModel				OPTIONAL,
		//		ue-positioning-GPS-IonosphericModel				UE-Positioning-GPS-IonosphericModel				OPTIONAL,
		//		ue-positioning-GPS-UTC-Model					UE-Positioning-GPS-UTC-Model					OPTIONAL,
		//		ue-positioning-GPS-Almanac						UE-Positioning-GPS-Almanac						OPTIONAL,
		//		ue-positioning-GPS-AcquisitionAssistance		UE-Positioning-GPS-AcquisitionAssistance		OPTIONAL,
		//		ue-positioning-GPS-Real-timeIntegrity			BadSatList							OPTIONAL,
		//		-- dummy is not used in this version of the specification, it should
		//		-- not be sent and if received it should be ignored.
		//		dummy			UE-Positioning-GPS-ReferenceCellInfo		OPTIONAL
		//	}

	aDecodedBits=0;
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	// No "E" BIT
	
	// Decode option bit for GpsReferenceTime.	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAssistanceDataContent->iRrcGpsReferenceTimePresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
		
	// Decode option bit for ReferenceLocation.	
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAssistanceDataContent->iRrcReferenceLocationPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	// Decode option bit for GpsDgpsCorrections.
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAssistanceDataContent->iRrcGpsDgpsCorrectionsPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode option bit for GpsNavigationModel.
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAssistanceDataContent->iRrcGpsNavigationModelPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;
	
	// Decode option bit for IonosphericModel.
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAssistanceDataContent->iRrcGpsIonosphericModelPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode option bit for UTCModel.
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAssistanceDataContent->iRrcGpsUTCModelPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode option bit for Almanac.
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAssistanceDataContent->iRrcGpsAlmanacPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode option bit for AcquisitionAssistance.
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAssistanceDataContent->iRrcGpsAcquisitionAssistancePresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode option bit for GpsBadSatList.
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAssistanceDataContent->iRrcGpsBadSatListPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;

	// Decode option bit for Dummy.
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iGpsAssistanceDataContent->iDummyPresent);
	aDecodedBits+=bitsDecoded;
	startPos+=bitsDecoded;


	if (iGpsAssistanceDataContent->iRrcGpsDgpsCorrectionsPresent || iGpsAssistanceDataContent->iDummyPresent)
		{
		User::Leave(KErrNotSupported);	
		}

	// Decode GPS Reference Time
	if (iGpsAssistanceDataContent->iRrcGpsReferenceTimePresent)
		{
		//Create GpsReferenceTime instance if needed
		iGpsAssistanceDataContent->iRrcGpsReferenceTimeContent=CRrcGpsReferenceTimeContent::NewL();
		iRrcGpsReferenceTime=CRrcGpsReferenceTime::NewL(iGpsAssistanceDataContent->iRrcGpsReferenceTimeContent, iLbsAssistanceDataBuilderSet);
		iRrcGpsReferenceTime->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;		
		}
	
	// Decode GPS Reference Location		
	if (iGpsAssistanceDataContent->iRrcReferenceLocationPresent)
		{
		//Create ReferenceLocation instance if needed
		iGpsAssistanceDataContent->iRrcReferenceLocationContent=CRrcReferenceLocationContent::NewL();
		iRrcReferenceLocation=CRrcReferenceLocation::NewL(iGpsAssistanceDataContent->iRrcReferenceLocationContent, iLbsAssistanceDataBuilderSet);
		iRrcReferenceLocation->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}

	// Decode GPS Navigation Model	
	
	if (iGpsAssistanceDataContent->iRrcGpsNavigationModelPresent)
		{
		//Create ReferenceLocation instance if needed	
		iGpsAssistanceDataContent->iRrcGpsNavigationModelContent=CRrcGpsNavigationModelContent::NewL();
		iRrcGpsNavigationModel=CRrcGpsNavigationModel::NewL(iGpsAssistanceDataContent->iRrcGpsNavigationModelContent, iLbsAssistanceDataBuilderSet);
		iRrcGpsNavigationModel->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;		
		}
	
	// Decode GPS Ionospheric Model	
	if (iGpsAssistanceDataContent->iRrcGpsIonosphericModelPresent)
		{
		iGpsAssistanceDataContent->iRrcGpsIonosphericModelContent=CRrcGpsIonosphericModelContent::NewL();
		iRrcGpsIonosphericModel=CRrcGpsIonosphericModel::NewL(iGpsAssistanceDataContent->iRrcGpsIonosphericModelContent, iLbsAssistanceDataBuilderSet);
		iRrcGpsIonosphericModel->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}
	
	// Decode GPS UTC Model
	if (iGpsAssistanceDataContent->iRrcGpsUTCModelPresent)
		{
		iGpsAssistanceDataContent->iRrcGpsUTCModelContent=CRrcGpsUTCModelContent::NewL();
		iRrcGpsUTCModel=CRrcGpsUTCModel::NewL(iGpsAssistanceDataContent->iRrcGpsUTCModelContent, iLbsAssistanceDataBuilderSet);
		iRrcGpsUTCModel->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}
		
	// Decode GPS Almanac	
	if (iGpsAssistanceDataContent->iRrcGpsAlmanacPresent)
		{
		iGpsAssistanceDataContent->iRrcGpsAlmanacContent=CRrcGpsAlmanacContent::NewL();
		iRrcGpsAlmanac=CRrcGpsAlmanac::NewL(iGpsAssistanceDataContent->iRrcGpsAlmanacContent, iLbsAssistanceDataBuilderSet);
		iRrcGpsAlmanac->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}

	// Decode GPS Acquisition Assistance
	if (iGpsAssistanceDataContent->iRrcGpsAcquisitionAssistancePresent)
		{
		iGpsAssistanceDataContent->iRrcGpsAcquisitionAssistanceContent=CRrcGpsAcquisitionAssistanceContent::NewL();
		iRrcGpsAcquisitionAssistance=CRrcGpsAcquisitionAssistance::NewL(iGpsAssistanceDataContent->iRrcGpsAcquisitionAssistanceContent, iLbsAssistanceDataBuilderSet);
		iRrcGpsAcquisitionAssistance->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}

	// Decode GPS Bad Sat list
	if (iGpsAssistanceDataContent->iRrcGpsBadSatListPresent)
		{
		iGpsAssistanceDataContent->iRrcGpsBadSatListContent=CRrcGpsBadSatListContent::NewL();
		iRrcGpsBadSatList=CRrcGpsBadSatList::NewL(iGpsAssistanceDataContent->iRrcGpsBadSatListContent, iLbsAssistanceDataBuilderSet);
		iRrcGpsBadSatList->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		aDecodedBits+=bitsDecoded;
		startPos+=bitsDecoded;			
		}
	}

void CRrcGpsAssistanceData::WriteLBSAssistanceDataL()
	{
	if (iGpsAssistanceDataContent->iRrcGpsReferenceTimePresent)
		{
		iRrcGpsReferenceTime->WriteLBSAssistanceDataL();			
		}
	
	if (iGpsAssistanceDataContent->iRrcReferenceLocationPresent)
		{
		iRrcReferenceLocation->WriteLBSAssistanceDataL();			
		}

	
	if (iGpsAssistanceDataContent->iRrcGpsNavigationModelPresent)
		{
		iRrcGpsNavigationModel->WriteLBSAssistanceDataL();			
		}

	
	if (iGpsAssistanceDataContent->iRrcGpsIonosphericModelPresent)
		{
		iRrcGpsIonosphericModel->WriteLBSAssistanceDataL();	
		}
	
	if (iGpsAssistanceDataContent->iRrcGpsUTCModelPresent)
		{
		iRrcGpsUTCModel->WriteLBSAssistanceDataL();	
		}
	
	if (iGpsAssistanceDataContent->iRrcGpsAlmanacPresent)
		{
		iRrcGpsAlmanac->WriteLBSAssistanceDataL();			
		}

	if (iGpsAssistanceDataContent->iRrcGpsAcquisitionAssistancePresent)
		{
		iRrcGpsAcquisitionAssistance->WriteLBSAssistanceDataL();		
		}	
	
	if (iGpsAssistanceDataContent->iRrcGpsBadSatListPresent)
		{
		iRrcGpsBadSatList->WriteLBSAssistanceDataL();		
		}
	
	}
	
CAssistanceDataDeliveryR3IEsContent* CAssistanceDataDeliveryR3IEsContent::NewL()
	{
	CAssistanceDataDeliveryR3IEsContent* self = CAssistanceDataDeliveryR3IEsContent::NewLC();
	CleanupStack::Pop(self);
	return self;					
	}

CAssistanceDataDeliveryR3IEsContent* CAssistanceDataDeliveryR3IEsContent::NewLC()
	{
	CAssistanceDataDeliveryR3IEsContent* self = new (ELeave) CAssistanceDataDeliveryR3IEsContent();
	CleanupStack::PushL(self);
	return self;
	}

CAssistanceDataDeliveryR3IEsContent::~CAssistanceDataDeliveryR3IEsContent()
	{
	delete iRrcGpsAssistanceDataContent;
	}

	
CAssistanceDataDeliveryR3IEsContent::CAssistanceDataDeliveryR3IEsContent()
	{
		
	}


EXPORT_C CAssistanceDataDeliveryR3IEs* CAssistanceDataDeliveryR3IEs::NewL(RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CAssistanceDataDeliveryR3IEs* self = CAssistanceDataDeliveryR3IEs::NewLC(aLbsAssistanceDataBuilderSet);
	CleanupStack::Pop(self);
	return self;					
	}

EXPORT_C CAssistanceDataDeliveryR3IEs* CAssistanceDataDeliveryR3IEs::NewLC(RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
	{
	CAssistanceDataDeliveryR3IEs* self = new (ELeave) CAssistanceDataDeliveryR3IEs(aLbsAssistanceDataBuilderSet);
	CleanupStack::PushL(self);
	return self;
	}

EXPORT_C CAssistanceDataDeliveryR3IEs::~CAssistanceDataDeliveryR3IEs()
	{
	delete iRrcGpsAssistanceData;
	delete iAssistanceDataDeliveryR3IEsContent;
	}

CAssistanceDataDeliveryR3IEs::CAssistanceDataDeliveryR3IEs(RLbsAssistanceDataBuilderSet* aLbsAssistanceDataBuilderSet)
:iLbsAssistanceDataBuilderSet(aLbsAssistanceDataBuilderSet)
	{
	}

EXPORT_C void CAssistanceDataDeliveryR3IEs::EncodeL(RBuf8&)
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C void CAssistanceDataDeliveryR3IEs::DecodeL(const TDesC8& aBufferToDecode, const TInt aStartBitPos)
	{
	// AssistanceDataDelivery-r3-IEs ::= SEQUENCE {
	//	-- User equipment IEs
	//	rrc-TransactionIdentifier		RRC-TransactionIdentifier,
	//	-- Measurement Information Elements
	//	ue-positioning-GPS-AssistanceData			UE-Positioning-GPS-AssistanceData				OPTIONAL,
	//	ue-positioning-OTDOA-AssistanceData-UEB		UE-Positioning-OTDOA-AssistanceData-UEB			OPTIONAL
	//}

	// RRC-TransactionIdentifier ::=		INTEGER (0..3)

	// TBool isExtended(EFalse);
	TInt startPos=aStartBitPos;
	TInt bitsDecoded=0;

	//Create the Assistance data content
	iAssistanceDataDeliveryR3IEsContent=CAssistanceDataDeliveryR3IEsContent::NewL();
	
	// No "E" bit	
	//Decode option bit map		
	
	/* SUPL-issue. Uncomment this code after SiRF have fixed their server's issue.
	   				This DecodeL method should be used to decode the full Assistance Data Delivery sequence.
	   				With this section commented out, is only used to decode the ue-positioning-GPS-AssistanceData.
	   				
	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iAssistanceDataDeliveryR3IEsContent->iRrcGpsAssistanceDataPresent);
	startPos+=bitsDecoded;

	CAsn1PerOption::DecodeL(aBufferToDecode, 
							startPos, 
							bitsDecoded, 
							iAssistanceDataDeliveryR3IEsContent->iRrcOTDOAAssistanceDataUEBPresent);
	startPos+=bitsDecoded;

	// Decode TransactionIdentifier
	CAsn1PerInt::DecodeL(aBufferToDecode, 
						startPos,
						KAsn1PerMinConstrained,
						0,
						KAsn1PerMaxConstrained,
						3,
						KAsn1PerNonExtensible,						
						bitsDecoded,
						isExtended,
						iAssistanceDataDeliveryR3IEsContent->iRrcTransactionIdentifier);					
	startPos+=bitsDecoded;

	// Decode GPS AssistanceData	
	if (iAssistanceDataDeliveryR3IEsContent->iRrcGpsAssistanceDataPresent)
		{
		
*/
		//create the assistance data instance
		iAssistanceDataDeliveryR3IEsContent->iRrcGpsAssistanceDataContent=CRrcGpsAssistanceDataContent::NewL();
		iRrcGpsAssistanceData=CRrcGpsAssistanceData::NewL(iAssistanceDataDeliveryR3IEsContent->iRrcGpsAssistanceDataContent, iLbsAssistanceDataBuilderSet);
		iRrcGpsAssistanceData->DecodeFromL(aBufferToDecode, startPos, bitsDecoded);
		startPos+=bitsDecoded;
		
		// Call RrcGpsAssistanceData to write the data to store	
		iRrcGpsAssistanceData->WriteLBSAssistanceDataL();					
//		}
		
	// Decode UE-Positioning-OTDOA-AssistanceData-UEB
	if (iAssistanceDataDeliveryR3IEsContent->iRrcOTDOAAssistanceDataUEBPresent)
		{
		User::Leave(KErrNotSupported);	
		}
		
	}
















































