// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#include "tsuplasn1step.h"
#include <lbs/test/rrcassistancedatadelivery.h>
#include <charconv.h>

/**
   Destructor
 */
CRrcPayloadStep::~CRrcPayloadStep()
	{
	iTheFs.Close();
	}

/**
   Constructor
 */
CRrcPayloadStep::CRrcPayloadStep()
	{

	TInt ret=iTheFs.Connect();
	TEST(ret==KErrNone);
	iTheFs.ShareProtected();
	
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KRrcPayLoadStep);
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CRrcPayloadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CRrcPayloadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}


TVerdict CRrcPayloadStep::doTestStepL()
	{
	__UHEAP_MARK;
	
	//Build the RRC payload Decoder
	RLbsAssistanceDataBuilderSet lbsAssistanceDataBuilderSet;
	lbsAssistanceDataBuilderSet.OpenL();
	CleanupClosePushL(lbsAssistanceDataBuilderSet);
	CAssistanceDataDeliveryR3IEs* rrcPayLoad=CAssistanceDataDeliveryR3IEs::NewLC(&lbsAssistanceDataBuilderSet);
	
	//Read the RRC payload from the file
	TPtrC fileName;
	GetStringFromConfig(ConfigSection(), _L("fileName"), fileName);		
	RBuf8 buffer;
	CleanupClosePushL(buffer);
	RFile file;
	CleanupClosePushL(file);
	User::LeaveIfError(file.Open(iTheFs, fileName, EFileStream | EFileRead));
	TInt size;
	file.Size(size);	
	buffer.ReAllocL(size);
	file.Read(buffer);
	CleanupStack::PopAndDestroy(&file);	
	
	//Decode the rrc message
	TRAPD(err, rrcPayLoad->DecodeL(buffer, 0);)
	User::LeaveIfError(err);
	CleanupStack::PopAndDestroy(&buffer);
	
	//
	//TODO: check if the assistance data is valid in the lbsAssistanceDataBuilderSet
	// against the expect flag in the ini file.
	
	TBool expectPresent;
	GetBoolFromConfig(ConfigSection(), _L("AssistanceDataReferenceTime"), expectPresent);
	GetBoolFromConfig(ConfigSection(), _L("AssistanceDataNavigationModel"), expectPresent);
	GetBoolFromConfig(ConfigSection(), _L("AssistanceDataIonosphericModel"), expectPresent);
	GetBoolFromConfig(ConfigSection(), _L("AssistanceDataPositioningGpsUtcModel"), expectPresent);
	GetBoolFromConfig(ConfigSection(), _L("AssistanceDataBadSatList"), expectPresent);
	GetBoolFromConfig(ConfigSection(), _L("AssistanceDataAlmanac"), expectPresent);
	GetBoolFromConfig(ConfigSection(), _L("AssistanceDataReferenceLocation"), expectPresent);
	GetBoolFromConfig(ConfigSection(), _L("AssistanceDataAquisitionAssistance"), expectPresent);
	
	//	
	
	RUEPositioningGpsReferenceTimeBuilder* ptrToGpsReferenceTimeBuilder = NULL;
	lbsAssistanceDataBuilderSet.GetDataBuilder(ptrToGpsReferenceTimeBuilder);

	RUEPositioningGpsNavigationModelBuilder* ptrToGpsNavigationModelBuilder = NULL;
	lbsAssistanceDataBuilderSet.GetDataBuilder(ptrToGpsNavigationModelBuilder);

	RUEPositioningGpsIonosphericModelBuilder* ptrToGpsIonosphericModelBuilder = NULL;
	lbsAssistanceDataBuilderSet.GetDataBuilder(ptrToGpsIonosphericModelBuilder);

	RUEPositioningGpsUtcModelBuilder* ptrToGpsUtcModelBuilder = NULL;
	lbsAssistanceDataBuilderSet.GetDataBuilder(ptrToGpsUtcModelBuilder);


	RBadSatListBuilder* ptrToBadSatListBuilder = NULL;	
	lbsAssistanceDataBuilderSet.GetDataBuilder(ptrToBadSatListBuilder);


	RUEPositioningGpsAlmanacBuilder* ptrToGpsAlmanacBuilder = NULL;
	lbsAssistanceDataBuilderSet.GetDataBuilder(ptrToGpsAlmanacBuilder);

	// The following assistance data have been tested by using a hacked asn1 spec. I hacked the spec by repalce the 
	// the range that includes minus value to plus value. So only the minus value ranges are not tested.


	RReferenceLocationBuilder* ptrToGpsReferenceLocationBuilder = NULL;
	lbsAssistanceDataBuilderSet.GetDataBuilder(ptrToGpsReferenceLocationBuilder);



	RUEPositioningGpsAcquisitionAssistanceBuilder* ptrToGpsAcquisitionAssistanceBuilder = NULL;
	lbsAssistanceDataBuilderSet.GetDataBuilder(ptrToGpsAcquisitionAssistanceBuilder);


	TRAP(err, CheckDecodedReferenceTimeResultsL(ptrToGpsReferenceTimeBuilder);
				CheckDecodedGpsNavigationModelResultsL(ptrToGpsNavigationModelBuilder);
				CheckDecodedGpsIonosphericModelResultsL(ptrToGpsIonosphericModelBuilder);
				CheckDecodedGpsUtcModelResultsL(ptrToGpsUtcModelBuilder);
				CheckDecodedBadSatListResultsL(ptrToBadSatListBuilder);
				CheckDecodedGpsAlmanacResultsL(ptrToGpsAlmanacBuilder);
				CheckDecodedReferenceLocationResultsL(ptrToGpsReferenceLocationBuilder);
				CheckDecodedGpsAcquisitionAssistanceResultsL(ptrToGpsAcquisitionAssistanceBuilder))

				
	if (err!=KErrNone)
		{
		SetTestStepResult(EFail);	
		}
	
	CleanupStack::PopAndDestroy(rrcPayLoad);
	CleanupStack::PopAndDestroy(&lbsAssistanceDataBuilderSet);
		
	__UHEAP_MARKEND;
	return TestStepResult();
	}
	
void CRrcPayloadStep::CheckDecodedGpsAlmanacResultsL(RUEPositioningGpsAlmanacBuilder* aGpsAlmanacBuilder)
	{
	RUEPositioningGpsAlmanacReader gpsAlmanacReader;
	CleanupClosePushL(gpsAlmanacReader);
	gpsAlmanacReader.OpenL();
	gpsAlmanacReader.DataBuffer() = aGpsAlmanacBuilder->DataBuffer();
	
	TUint readValue;
	TInt expectedValue;	
	gpsAlmanacReader.GetField(TUEPositioningGpsAlmanac::EWnA, readValue);	
	GetIntFromConfig(ConfigSection(), _L("GpsAlmanac_EWnA"), expectedValue);
	TESTL(readValue==expectedValue);
	
	if (gpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::ESvGlobalHealth))
		{
		TBuf8<46> readBuffer;
		TPtrC expectedBuffer;
		gpsAlmanacReader.GetField(TUEPositioningGpsAlmanac::ESvGlobalHealth, readBuffer);
		GetStringFromConfig(ConfigSection(), _L("GpsAlmanac_ESvGlobalHealth"), expectedBuffer);
		HBufC8* covertedBuffer=ConvertDes16toHBufC8LC(expectedBuffer);
		TESTL(!readBuffer.CompareF(*covertedBuffer));
		CleanupStack::PopAndDestroy(covertedBuffer);
		}
	
	if (gpsAlmanacReader.FieldExists(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray))
		{
		RAlmanacSatInfoArrayReader almanacSatInfoArrayReader;
		gpsAlmanacReader.GetArrayReader(TUEPositioningGpsAlmanac::EAlmanacInfoSatArray, almanacSatInfoArrayReader);
		TInt index(0);
		while (almanacSatInfoArrayReader.ElementExists(index))
			{
			RAlmanacSatInfoReader almanacSatInfoReader;
			almanacSatInfoArrayReader.GetFieldReader(index, almanacSatInfoReader);
			TBuf<256> fName;
			
			fName.Format(_L("GpsAlmanac_EDataID_%d"), index);			
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EDataID, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_ESatID_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::ESatID, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_EE_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EE, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_EToa_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EToa, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_EDeltaI_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EDeltaI, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_EOmegaDot_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EOmegaDot, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_ESatHealth_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::ESatHealth, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_EASqrt_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EASqrt, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_EOmega0_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EOmega0, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_EM0_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EM0, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_EOmega_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EOmega, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_EAf0_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EAf0, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsAlmanac_EAf1_%d"), index);
			almanacSatInfoReader.GetField(TAlmanacSatInfo::EAf1, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);			
			index++;
			}			
			
		}
	
	CleanupStack::PopAndDestroy(&gpsAlmanacReader);		
	}
	
HBufC8* CRrcPayloadStep::ConvertDes16toHBufC8LC(TDesC& source)
	{
	HBufC8 *buf = HBufC8::NewL(source.Length());
	CleanupStack::PushL(buf);
	TPtr8 ptr=buf->Des();

	CCnvCharacterSetConverter *converter = CCnvCharacterSetConverter::NewLC();
	converter->PrepareToConvertToOrFromL(KCharacterSetIdentifierAscii, iTheFs);
	converter->ConvertFromUnicode(ptr, source);
	CleanupStack::PopAndDestroy(converter);
	return buf;
	}
	
	
void CRrcPayloadStep::CheckDecodedBadSatListResultsL(RBadSatListBuilder* aBadSatListBuilder)
	{
	RBadSatListReader badSatListReader;
	CleanupClosePushL(badSatListReader);
	badSatListReader.OpenL();
	badSatListReader.DataBuffer() = aBadSatListBuilder->DataBuffer();
	TUint readValue;
	TInt expectedValue;
	
	if (badSatListReader.FieldExists(TBadSatList::EBadSatIdArray))
		{
		RBadSatArrayReader badSatArrayReader;
		badSatListReader.GetArrayReader(TBadSatList::EBadSatIdArray, badSatArrayReader);
		TInt index(0);
		while (badSatArrayReader.ElementExists(index))
			{
			badSatArrayReader.GetElement(index, readValue);
			TBuf<256> fName;
			fName.Format(_L("BadSatList_%d"), index);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);					
			index++;
			}			
		}
	

	
	CleanupStack::PopAndDestroy(&badSatListReader);	
	}

void CRrcPayloadStep::CheckDecodedGpsUtcModelResultsL(RUEPositioningGpsUtcModelBuilder* aGpsUtcModelBuilder)
	{
	RUEPositioningGpsUtcModelReader gpsUtcModelReader;
	CleanupClosePushL(gpsUtcModelReader);
	gpsUtcModelReader.OpenL();
	gpsUtcModelReader.DataBuffer() = aGpsUtcModelBuilder->DataBuffer();
	
	TUint readValue;
	TInt expectedValue;
	
	gpsUtcModelReader.GetField(TUEPositioningGpsUtcModel::EA1, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsUtcModel_EA1"), expectedValue);
	TESTL(readValue==expectedValue);
	
	gpsUtcModelReader.GetField(TUEPositioningGpsUtcModel::EA0, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsUtcModel_EA0"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsUtcModelReader.GetField(TUEPositioningGpsUtcModel::ETot, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsUtcModel_ETot"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsUtcModelReader.GetField(TUEPositioningGpsUtcModel::EWnt, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsUtcModel_EWnt"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsUtcModelReader.GetField(TUEPositioningGpsUtcModel::EDeltatLS, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsUtcModel_EDeltatLS"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsUtcModelReader.GetField(TUEPositioningGpsUtcModel::EWnlsf, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsUtcModel_EWnlsf"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsUtcModelReader.GetField(TUEPositioningGpsUtcModel::EDn, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsUtcModel_EDn"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsUtcModelReader.GetField(TUEPositioningGpsUtcModel::EDeltatLSF, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsUtcModel_EDeltatLSF"), expectedValue);
	TESTL(readValue==expectedValue);

	CleanupStack::PopAndDestroy(&gpsUtcModelReader);	
	}

void CRrcPayloadStep::CheckDecodedGpsIonosphericModelResultsL(RUEPositioningGpsIonosphericModelBuilder* aGpsIonosphericModelBuilder)
	{
	RUEPositioningGpsIonosphericModelReader gpsIonosphericModelReader;
	CleanupClosePushL(gpsIonosphericModelReader);
	gpsIonosphericModelReader.OpenL();
	gpsIonosphericModelReader.DataBuffer() = aGpsIonosphericModelBuilder->DataBuffer();
	
	TUint readValue;
	TInt expectedValue;
	gpsIonosphericModelReader.GetField(TUEPositioningGpsIonosphericModel::EAlfa0, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsIonosphericModel_EAlfa0"), expectedValue);
	TESTL(readValue==expectedValue);
	
	gpsIonosphericModelReader.GetField(TUEPositioningGpsIonosphericModel::EAlfa1, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsIonosphericModel_EAlfa1"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsIonosphericModelReader.GetField(TUEPositioningGpsIonosphericModel::EAlfa2, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsIonosphericModel_EAlfa2"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsIonosphericModelReader.GetField(TUEPositioningGpsIonosphericModel::EAlfa3, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsIonosphericModel_EAlfa3"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsIonosphericModelReader.GetField(TUEPositioningGpsIonosphericModel::EBeta0, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsIonosphericModel_EBeta0"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsIonosphericModelReader.GetField(TUEPositioningGpsIonosphericModel::EBeta1, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsIonosphericModel_EBeta1"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsIonosphericModelReader.GetField(TUEPositioningGpsIonosphericModel::EBeta2, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsIonosphericModel_EBeta2"), expectedValue);
	TESTL(readValue==expectedValue);

	gpsIonosphericModelReader.GetField(TUEPositioningGpsIonosphericModel::EBeta3, readValue);
	GetIntFromConfig(ConfigSection(), _L("GpsIonosphericModel_EBeta3"), expectedValue);
	TESTL(readValue==expectedValue);
	
	CleanupStack::PopAndDestroy(&gpsIonosphericModelReader);			
	}
	
void CRrcPayloadStep::CheckDecodedGpsNavigationModelResultsL(RUEPositioningGpsNavigationModelBuilder* aGpsNavigationModelBuilder)
	{
	RUEPositioningGpsNavigationModelReader gpsNavigationModelReader;
	CleanupClosePushL(gpsNavigationModelReader);
	gpsNavigationModelReader.OpenL();
	gpsNavigationModelReader.DataBuffer() = aGpsNavigationModelBuilder->DataBuffer();
	
	if (gpsNavigationModelReader.FieldExists(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray))
		{
		RNavigationModelSatInfoArrayReader navigationModelSatInfoArrayReader;
		gpsNavigationModelReader.GetArrayReader(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray, navigationModelSatInfoArrayReader);
		TInt index(0);
		while (navigationModelSatInfoArrayReader.ElementExists(index))
			{
			RNavigationModelSatInfoReader navigationModelSatInfoReader;
			navigationModelSatInfoArrayReader.GetFieldReader(index, navigationModelSatInfoReader);			
			TUint readValue;
			TInt expectedValue;
			TBuf<256> fName;
			
			fName.Format(_L("NavigationModelSatInfo_SatID_%d"), index);
			navigationModelSatInfoReader.GetField(TNavigationModelSatInfo::ESatId, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			TSatelliteStatus readStatus;
			fName.Format(_L("NavigationModelSatInfo_SatelliteStatus_%d"), index);
			navigationModelSatInfoReader.GetField(TNavigationModelSatInfo::ESatelliteStatus, readStatus);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TSatelliteStatus expectedStatus=(TSatelliteStatus)expectedValue;
			
			TESTL(readStatus==expectedStatus);
			
			if (navigationModelSatInfoReader.FieldExists(TNavigationModelSatInfo::EEphemerisParameter))
				{
				REphemerisParameterReader ephemerisParameterReader;
				navigationModelSatInfoReader.GetFieldReader(TNavigationModelSatInfo::EEphemerisParameter, ephemerisParameterReader);
				

				
				fName.Format(_L("NavigationModelSatInfo_ECodeOnL2_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::ECodeOnL2, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);
				
				fName.Format(_L("NavigationModelSatInfo_EUraIndex_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EUraIndex, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_ESatHealth_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::ESatHealth, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);
				
				fName.Format(_L("NavigationModelSatInfo_EIodc_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EIodc, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EL2Pflag_%d"), index);				
				ephemerisParameterReader.GetField(TEphemerisParameter::EL2Pflag, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_ETGD_%d"), index);								
				ephemerisParameterReader.GetField(TEphemerisParameter::ETGD, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EToc_%d"), index);												
				ephemerisParameterReader.GetField(TEphemerisParameter::EToc, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EAf2_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EAf2, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EAf1_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EAf1, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EAf0_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EAf0, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_ECrs_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::ECrs, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EDeltaN_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EDeltaN, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EM0_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EM0, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_ECuc_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::ECuc, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EE_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EE, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_ECus_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::ECus, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EASqrt_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EASqrt, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EToe_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EToe, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EFitInterval_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EFitInterval, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EAodo_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EAodo, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_ECic_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::ECic, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EOmega0_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EOmega0, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_ECis_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::ECis, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EI0_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EI0, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_ECrc_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::ECrc, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EOmega_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EOmega, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EOmegaDot_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EOmegaDot, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);

				fName.Format(_L("NavigationModelSatInfo_EIDot_%d"), index);
				ephemerisParameterReader.GetField(TEphemerisParameter::EIDot, readValue);
				GetIntFromConfig(ConfigSection(), fName, expectedValue);
				TESTL(readValue==expectedValue);										
				}
			index++;
			}
		}
		
	CleanupStack::PopAndDestroy(&gpsNavigationModelReader);	
	}
	
	
	

void CRrcPayloadStep::CheckDecodedReferenceTimeResultsL(RUEPositioningGpsReferenceTimeBuilder* aGpsReferenceTimeBuilder)
	{
	//Copy the data to reader	
	RUEPositioningGpsReferenceTimeReader gpsReferenceTimeReader;
	CleanupClosePushL(gpsReferenceTimeReader);
	gpsReferenceTimeReader.OpenL();
	gpsReferenceTimeReader.DataBuffer() = aGpsReferenceTimeBuilder->DataBuffer();
	
	if (gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EGpsWeek))
		{
    	TUint readValue;
		gpsReferenceTimeReader.GetField(TUEPositioningGpsReferenceTime::EGpsWeek, readValue);		
		TInt expectedValue;
		GetIntFromConfig(ConfigSection(), _L("GpsReferenceTime_GpsWeek"), expectedValue);				
		TESTL(readValue==expectedValue);
		}
		
	if (gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EGpsTow1Msec))
		{
    	TUint readValue;
		gpsReferenceTimeReader.GetField(TUEPositioningGpsReferenceTime::EGpsTow1Msec, readValue);
		TInt expectedValue;
		GetIntFromConfig(ConfigSection(), _L("GpsReferenceTime_GpsTow1Msec"), expectedValue);		
		TESTL(readValue==expectedValue);			
		}
		
	if (gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime))
		{
		RUtranGpsReferenceTimeReader utranGpsRefTimeReader;		
		TInt err = gpsReferenceTimeReader.GetFieldReader(TUEPositioningGpsReferenceTime::EUtranGpsReferenceTime, utranGpsRefTimeReader);
		
		TUint readValue;
		utranGpsRefTimeReader.GetField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_MsPart, readValue);
		TInt expectedValue;
		GetIntFromConfig(ConfigSection(), _L("GpsReferenceTime_UtranGPSTimingOfCell_MsPart"), expectedValue);		
		TESTL(readValue==expectedValue);
		
		utranGpsRefTimeReader.GetField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_LsPart, readValue);
		GetIntFromConfig(ConfigSection(), _L("GpsReferenceTime_UtranGPSTimingOfCell_LsPart"), expectedValue);
		TESTL(readValue==expectedValue);
		
		if (utranGpsRefTimeReader.FieldExists(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity))
			{
			RPrimaryCpichInfoReader primaryCpichInfoReader;
			TInt err = utranGpsRefTimeReader.GetFieldReader(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity,primaryCpichInfoReader);
			primaryCpichInfoReader.GetField(TPrimaryCpichInfo::EPrimaryScramblingCode, readValue);			
			GetIntFromConfig(ConfigSection(), _L("GpsReferenceTime_ModeSpecificInfo_Fdd_ReferenceIdentity"), expectedValue);
			TESTL(readValue==expectedValue);				
			}
		else
			{
			if (utranGpsRefTimeReader.FieldExists(TUtranGpsReferenceTime::EModeSpecificInfo_Tdd_ReferenceIdentity))
				{
				utranGpsRefTimeReader.GetField(TUtranGpsReferenceTime::EModeSpecificInfo_Tdd_ReferenceIdentity, readValue);
				GetIntFromConfig(ConfigSection(), _L("GpsReferenceTime_ModeSpecificInfo_Tdd_ReferenceIdentity"), expectedValue);
				TESTL(readValue==expectedValue);					
				}
			}
			
			{
			TUint readValue;
			TInt expectedValue;
			utranGpsRefTimeReader.GetField(TUtranGpsReferenceTime::ESfn, readValue);
			GetIntFromConfig(ConfigSection(), _L("GpsReferenceTime_UtranESfn"), expectedValue);
			TESTL(readValue==expectedValue);			
			}
		
		}
	if (gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::ESfnTowUncertainty))
		{
		
    	TSfnTowUncertainty readValue;
		gpsReferenceTimeReader.GetField(TUEPositioningGpsReferenceTime::ESfnTowUncertainty, readValue);
		TInt expectedValue;
		GetIntFromConfig(ConfigSection(), _L("GpsReferenceTime_SfnTowUncertainty"), expectedValue);		
		TESTL(readValue==expectedValue);		
		}
			
	if (gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EUtranGpsDriftRate))
		{
    	TUtranGpsDriftRate readValue;
		gpsReferenceTimeReader.GetField(TUEPositioningGpsReferenceTime::EUtranGpsDriftRate, readValue);
		TInt expectedValue;
		GetIntFromConfig(ConfigSection(), _L("GpsReferenceTime_UtranGpsDriftRate"), expectedValue);		
		TESTL(readValue==expectedValue);			
		}
			
			
	if (gpsReferenceTimeReader.FieldExists(TUEPositioningGpsReferenceTime::EGpsTowAssistArray))
		{
		RGpsTowAssistArrayReader gpsTowAssistArrayReader;
		gpsReferenceTimeReader.GetArrayReader(TUEPositioningGpsReferenceTime::EGpsTowAssistArray, gpsTowAssistArrayReader);
		TInt index(0);
		while (gpsTowAssistArrayReader.ElementExists(index))
			{
			RGpsTowAssistReader gpsTowAssistReader;
			gpsTowAssistArrayReader.GetFieldReader(index, gpsTowAssistReader);			
			TUint readValue;
			TInt expectedValue;

			TBuf<256> fName;
			fName.Format(_L("GpsReferenceTime_TowAssist_SatID_%d"), index);
			gpsTowAssistReader.GetField(TGpsTowAssist::ESatID, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			fName.Format(_L("GpsReferenceTime_TowAssist_TlmMessage_%d"), index);
			gpsTowAssistReader.GetField(TGpsTowAssist::ETlmMessage, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			TESTL(readValue==expectedValue);
			
			//TODO: add tests when bool is fixed
			//gpsTowAssistReader.GetField(TGpsTowAssist::EAlert, );
			//gpsTowAssistReader.GetField(TGpsTowAssist::EAntiSpoof, );
			index++;
			}
		}			
	CleanupStack::PopAndDestroy(&gpsReferenceTimeReader);
	}

void CRrcPayloadStep::CheckDecodedReferenceLocationResultsL(RReferenceLocationBuilder* aReferenceLocationBuilder)
	{
	RReferenceLocationReader referenceLocationReader;
	CleanupClosePushL(referenceLocationReader);
	referenceLocationReader.OpenL();
	referenceLocationReader.DataBuffer() = aReferenceLocationBuilder->DataBuffer();
	
	if (referenceLocationReader.FieldExists(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide))
		{
		TUint readValue;
		TInt readIntValue;
		TInt expectedValue;
		REllipsoidPointAltitudeEllipsoideReader ellipsoidPointAltReader;
		referenceLocationReader.GetFieldReader(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, ellipsoidPointAltReader);
		
		ellipsoidPointAltReader.GetField(TEllipsoidPointAltitudeEllipsoide::EAltitude, readValue);
		GetIntFromConfig(ConfigSection(), _L("ReferenceLocation_EAltitude"), expectedValue);		
		
		ellipsoidPointAltReader.GetField(TEllipsoidPointAltitudeEllipsoide::ELatitudeSign, readIntValue);
		GetIntFromConfig(ConfigSection(), _L("ReferenceLocation_ELatitudeSign"), expectedValue);
				
		ellipsoidPointAltReader.GetField(TEllipsoidPointAltitudeEllipsoide::ELongitude, readIntValue);
		GetIntFromConfig(ConfigSection(), _L("ReferenceLocation_ELongitude"), expectedValue);
		
		ellipsoidPointAltReader.GetField(TEllipsoidPointAltitudeEllipsoide::EAltitudeDirection, readIntValue);
		GetIntFromConfig(ConfigSection(), _L("ReferenceLocation_EAltitudeDirection"), expectedValue);
				
		ellipsoidPointAltReader.GetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintySemiMajor, readValue);
		GetIntFromConfig(ConfigSection(), _L("ReferenceLocation_EUncertaintySemiMajor"), expectedValue);	
			
		ellipsoidPointAltReader.GetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintySemiMinor, readValue);
		GetIntFromConfig(ConfigSection(), _L("ReferenceLocation_EUncertaintySemiMinor"), expectedValue);
		
		ellipsoidPointAltReader.GetField(TEllipsoidPointAltitudeEllipsoide::EOrientationMajorAxis, readValue);
		GetIntFromConfig(ConfigSection(), _L("ReferenceLocation_EOrientationMajorAxis"), expectedValue);		
		
		ellipsoidPointAltReader.GetField(TEllipsoidPointAltitudeEllipsoide::EUncertaintyAltitude, readValue);
		GetIntFromConfig(ConfigSection(), _L("ReferenceLocation_EUncertaintyAltitude"), expectedValue);
				
		ellipsoidPointAltReader.GetField(TEllipsoidPointAltitudeEllipsoide::EConfidence, readValue);
		GetIntFromConfig(ConfigSection(), _L("ReferenceLocation_EConfidence"), expectedValue);		
		
		ellipsoidPointAltReader.GetField(TEllipsoidPointAltitudeEllipsoide::ELatitude, readValue);
		GetIntFromConfig(ConfigSection(), _L("ReferenceLocation_ELatitude"), expectedValue);		
		}
	CleanupStack::PopAndDestroy(&referenceLocationReader);	
	}

void CRrcPayloadStep::CheckDecodedGpsAcquisitionAssistanceResultsL(RUEPositioningGpsAcquisitionAssistanceBuilder* aGpsAcquisitionAssistanceBuilder)
	{
	RUEPositioningGpsAcquisitionAssistanceReader gpsAcquisitionAssistanceReader;
	CleanupClosePushL(gpsAcquisitionAssistanceReader);
	gpsAcquisitionAssistanceReader.OpenL();
	gpsAcquisitionAssistanceReader.DataBuffer() = aGpsAcquisitionAssistanceBuilder->DataBuffer();
	
	TUint readValue;
	if (gpsAcquisitionAssistanceReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime))
		{		
		gpsAcquisitionAssistanceReader.GetField(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime, readValue);
		}
	else
		{
			
		}
		
	if (gpsAcquisitionAssistanceReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::EUtranGpsReferenceTime))
		{
		RUtranGpsReferenceTimeReader utranGpsRefTimeReader;	
		gpsAcquisitionAssistanceReader.GetFieldReader(TUEPositioningGpsAcquisitionAssistance::EUtranGpsReferenceTime, utranGpsRefTimeReader);
		
		TUint readValue;
		utranGpsRefTimeReader.GetField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_MsPart, readValue);
		TInt expectedValue;
		GetIntFromConfig(ConfigSection(), _L("GpsAcquisitionAssistance_UtranGPSTimingOfCell_MsPart"), expectedValue);		
		//TESTL(readValue==expectedValue);
		
		utranGpsRefTimeReader.GetField(TUtranGpsReferenceTime::EUtranGPSTimingOfCell_LsPart, readValue);
		GetIntFromConfig(ConfigSection(), _L("GpsAcquisitionAssistance_UtranGPSTimingOfCell_LsPart"), expectedValue);
		//TESTL(readValue==expectedValue);
		
		if (utranGpsRefTimeReader.FieldExists(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity))
			{
			RPrimaryCpichInfoReader primaryCpichInfoReader;
			TInt err = utranGpsRefTimeReader.GetFieldReader(TUtranGpsReferenceTime::EModeSpecificInfo_Fdd_ReferenceIdentity,primaryCpichInfoReader);
			primaryCpichInfoReader.GetField(TPrimaryCpichInfo::EPrimaryScramblingCode, readValue);			
			GetIntFromConfig(ConfigSection(), _L("GpsAcquisitionAssistance_ModeSpecificInfo_Fdd_ReferenceIdentity"), expectedValue);
			//TESTL(readValue==expectedValue);				
			}
		else
			{
			if (utranGpsRefTimeReader.FieldExists(TUtranGpsReferenceTime::EModeSpecificInfo_Tdd_ReferenceIdentity))
				{
				utranGpsRefTimeReader.GetField(TUtranGpsReferenceTime::EModeSpecificInfo_Tdd_ReferenceIdentity, readValue);
				GetIntFromConfig(ConfigSection(), _L("GpsAcquisitionAssistance_ModeSpecificInfo_Tdd_ReferenceIdentity"), expectedValue);
				//TESTL(readValue==expectedValue);					
				}
			}
			
			{
			TUint readValue;
			TInt expectedValue;
			utranGpsRefTimeReader.GetField(TUtranGpsReferenceTime::ESfn, readValue);
			GetIntFromConfig(ConfigSection(), _L("GpsAcquisitionAssistance_UtranESfn"), expectedValue);
			//TESTL(readValue==expectedValue);			
			}		
		}
		
	if (gpsAcquisitionAssistanceReader.FieldExists(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray))	
		{
		RAcquisitionSatInfoArrayReader acquisitionSatInfoArrayReader;
		gpsAcquisitionAssistanceReader.GetArrayReader(TUEPositioningGpsAcquisitionAssistance::ESatelliteInformationArray, acquisitionSatInfoArrayReader);
		
		TInt index(0);
		while (acquisitionSatInfoArrayReader.ElementExists(index))
			{
			RAcquisitionSatInfoReader acquisitionSatInfoReader;
			acquisitionSatInfoArrayReader.GetFieldReader(index, acquisitionSatInfoReader);
					
			TUint readValue;
			TInt expectedValue;
			TInt readIntValue;
			TBuf<256> fName;
			
			fName.Format(_L("AcquisitionSatInfo_ESatID_%d"), index);			
			acquisitionSatInfoReader.GetField(TAcquisitionSatInfo::ESatID, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			//TESTL(readValue==expectedValue);
			
			fName.Format(_L("AcquisitionSatInfo_EDoppler0thOrder_%d"), index);
			acquisitionSatInfoReader.GetField(TAcquisitionSatInfo::EDoppler0thOrder, readIntValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			//TESTL(readIntValue==expectedValue);
			
			
			fName.Format(_L("AcquisitionSatInfo_ECodePhase_%d"), index);
			acquisitionSatInfoReader.GetField(TAcquisitionSatInfo::ECodePhase, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			//TESTL(readValue==expectedValue);
			
			fName.Format(_L("AcquisitionSatInfo_EIntegerCodePhase_%d"), index);
			acquisitionSatInfoReader.GetField(TAcquisitionSatInfo::EIntegerCodePhase, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			//TESTL(readValue==expectedValue);

			fName.Format(_L("AcquisitionSatInfo_EGpsBitNumber_%d"), index);
			acquisitionSatInfoReader.GetField(TAcquisitionSatInfo::EGpsBitNumber, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			//TESTL(readValue==expectedValue);

			TCodePhaseSearchWindow codePhaseSearchWindow;
			fName.Format(_L("AcquisitionSatInfo_ECodePhaseSearchWindow_%d"), index);
			acquisitionSatInfoReader.GetField(TAcquisitionSatInfo::ECodePhaseSearchWindow, codePhaseSearchWindow);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			//TESTL(readValue==expectedValue);


			
			RTAzimuthAndElevationReader azimuthAndElevationReader;
			acquisitionSatInfoReader.GetFieldReader(TAcquisitionSatInfo::EAzimuthAndElevation, azimuthAndElevationReader);
			
			
			fName.Format(_L("AcquisitionSatInfo_EAzimuthAndElevation_EAzimuth_%d"), index);
			azimuthAndElevationReader.GetField(TAzimuthAndElevation::EAzimuth, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			//TESTL(readValue==expectedValue);

			fName.Format(_L("AcquisitionSatInfo_EAzimuthAndElevation_EElevation_%d"), index);
			azimuthAndElevationReader.GetField(TAzimuthAndElevation::EElevation, readValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			//TESTL(readValue==expectedValue);


			RExtraDopplerInfoReader extraDopplerInfoReader;
			acquisitionSatInfoReader.GetFieldReader(TAcquisitionSatInfo::EExtraDopplerInfo, extraDopplerInfoReader);
			
			fName.Format(_L("AcquisitionSatInfo_EExtraDopplerInfo_EDoppler1stOrder_%d"), index);
			extraDopplerInfoReader.GetField(TExtraDopplerInfo::EDoppler1stOrder, readIntValue);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			//TESTL(readValue==expectedValue);

			TDopplerUncertainty dopplerUncertainty;
			fName.Format(_L("AcquisitionSatInfo_EExtraDopplerInfo_EDopplerUncertainty_%d"), index);
			extraDopplerInfoReader.GetField(TExtraDopplerInfo::EDopplerUncertainty, dopplerUncertainty);
			GetIntFromConfig(ConfigSection(), fName, expectedValue);
			//TESTL(readValue==expectedValue);
			index++;
			}
		}
		
	CleanupStack::PopAndDestroy(&gpsAcquisitionAssistanceReader);	
	}

// End of file
