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
//

/**
 @file
 @internalComponent - Internal Symbian test code 
*/


#include "tsuplasn1step.h"
#include <lbs/test/suplmessage.h>
#include <lbs/test/suplstart.h>
#include <lbs/test/suplresponse.h>
#include <lbs/test/suplposinit.h>
#include <lbs/test/suplend.h>
#include <lbs/test/suplpos.h>
//#include <suplinit.h>


void CSuplMessageStep::BuildMessageL(CSuplMessageContent* aMessageContent)
	{	
	
	//Message Length
	TInt msgLength;
	GetIntFromConfig(ConfigSection(), _L("messagelength"), msgLength);
	aMessageContent->iLength=msgLength;
		
	//Message Version
	aMessageContent->iSuplVersionContent=CSuplVersionContent::NewL();
	TInt majorVersion;
	GetIntFromConfig(ConfigSection(), _L("versionmaj"), majorVersion);
	aMessageContent->iSuplVersionContent->iMaj=majorVersion;
	
	TInt minVersion;
	GetIntFromConfig(ConfigSection(), _L("versionmin"), minVersion);	
	aMessageContent->iSuplVersionContent->iMin=minVersion;
	
	TInt servindVersion;
	GetIntFromConfig(ConfigSection(), _L("versionservind"), servindVersion);	
	aMessageContent->iSuplVersionContent->iServind=servindVersion;
	
	
	aMessageContent->iSuplSessionIdContent=CSuplSessionIdContent::NewL();
	
	//SET Session ID	
	TBool setsessionPresent;
	GetBoolFromConfig(ConfigSection(), _L("setsessionidoption"), setsessionPresent);	
	aMessageContent->iSuplSessionIdContent->iSetSessionIdPresent=setsessionPresent;
	
	if (setsessionPresent)
		{
		TInt setSessionId;
		aMessageContent->iSuplSessionIdContent->iSetSessionIdContent=CSuplSetSessionIdContent::NewL();
		GetIntFromConfig(ConfigSection(), _L("set_sessionid"), setSessionId);	
		aMessageContent->iSuplSessionIdContent->iSetSessionIdContent->iSessionId=setSessionId;
		
		TInt setIdChoice;
		aMessageContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent=CSuplSetIdContent::NewL();
		GetIntFromConfig(ConfigSection(), _L("setidchoice"), setIdChoice);	
		aMessageContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent->iSetIdType=(TSuplSetIdType)setIdChoice;
		
		if (setIdChoice==ESuplIPAddress)		
			{
			aMessageContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent->iIpAddressContent=CSuplIpAddressContent::NewL();
			
			TInt setIdIpAddressChoice;
			GetIntFromConfig(ConfigSection(), _L("setid_ipaddresschoice"), setIdIpAddressChoice);	
			aMessageContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent->iIpAddressContent->iIpAddressType=(TSuplIpAddressType)setIdIpAddressChoice;
			
			TPtrC ipAddress;
			GetStringFromConfig(ConfigSection(), _L("setid_ipaddress"), ipAddress);
			aMessageContent->iSuplSessionIdContent->iSetSessionIdContent->iSetIdContent->iIpAddressContent->iIpAddress.Copy(ipAddress);				
			}
			
		}
		
	//SLP Session ID
	TBool slpSessionIdPresent;
	GetBoolFromConfig(ConfigSection(), _L("slpsessionidoption"), slpSessionIdPresent);	
	aMessageContent->iSuplSessionIdContent->iSlpSessionIdPresent=slpSessionIdPresent;
	
	if (slpSessionIdPresent)
		{
		aMessageContent->iSuplSessionIdContent->iSlpSessionIdContent=CSuplSlpSessionIdContent::NewL();
		
		TPtrC slpSessionId;
		GetStringFromConfig(ConfigSection(), _L("slp_sessionid"), slpSessionId);
		aMessageContent->iSuplSessionIdContent->iSlpSessionIdContent->iSessionId.Copy(slpSessionId);
		
		aMessageContent->iSuplSessionIdContent->iSlpSessionIdContent->iSlpAddressContent=CSuplSlpAddressContent::NewL();
		TInt slpAdressChoice;
		GetIntFromConfig(ConfigSection(), _L("slpadresschoice"), slpAdressChoice);	
		aMessageContent->iSuplSessionIdContent->iSlpSessionIdContent->iSlpAddressContent->iSlpAddressType=(TSuplSlpAddressType)slpAdressChoice;
		
		if (slpAdressChoice==ESuplSlpIpAddressChoice)
			{
			aMessageContent->iSuplSessionIdContent->iSlpSessionIdContent->iSlpAddressContent->iIpAddressContent=CSuplIpAddressContent::NewL();
			
			TInt slpIpAddressChoice;
			GetIntFromConfig(ConfigSection(), _L("slp_ipaddresschoice"), slpIpAddressChoice);	
			aMessageContent->iSuplSessionIdContent->iSlpSessionIdContent->iSlpAddressContent->iIpAddressContent->iIpAddressType=(TSuplIpAddressType)slpIpAddressChoice;

			TPtrC slpIpAddress;
			GetStringFromConfig(ConfigSection(), _L("slp_ipaddress"), slpIpAddress);
			aMessageContent->iSuplSessionIdContent->iSlpSessionIdContent->iSlpAddressContent->iIpAddressContent->iIpAddress.Copy(slpIpAddress);				
			}
		}
		
	//Supl Message Type
	TInt ulpMessageChoice;
	GetIntFromConfig(ConfigSection(), _L("ulpmessagechoice"), ulpMessageChoice);		
	aMessageContent->iMessageChoice=ulpMessageChoice;
	
	switch (ulpMessageChoice)
		{
		case ESuplStart:
			aMessageContent->iSuplStartContent=CSuplStartContent::NewL();			
			BuildSuplStartMessageL(aMessageContent);
			break;
			
		case ESuplResponse:
			aMessageContent->iSuplResponseContent=CSuplResponseContent::NewL();			
			BuildSuplResponseMessageL(aMessageContent);		
			break;
			
		case ESuplPosInit:
			aMessageContent->iSuplPosInitContent=CSuplPosInitContent::NewL();			
			BuildSuplPosInitMessageL(aMessageContent);
		
			break;
			
		case ESuplPos:
			aMessageContent->iSuplPosContent=CSuplPosContent::NewL();			
			BuildSuplPosMessageL(aMessageContent);		
			break;	
				
		case ESuplEnd:
			aMessageContent->iSuplEndContent=CSuplEndContent::NewL();			
			BuildSuplEndMessageL(aMessageContent);				
			break;
			
		case ESuplInit:					
		case ESuplAuthReq:
		case ESuplAuthResp:		
		default:	
			User::Leave(KErrNotSupported);
			
		}
	}

/**
   Destructor
 */
CSuplMessageStep::~CSuplMessageStep()
	{
	iTheFs.Close();
	}

/**
   Constructor
 */
CSuplMessageStep::CSuplMessageStep()
	{

	TInt ret=iTheFs.Connect();
	TEST(ret==KErrNone);
	iTheFs.ShareProtected();
	
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSuplMessageStep);
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CSuplMessageStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
   @return - TVerdict code
   Override of base class virtual
 */
TVerdict CSuplMessageStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}


TVerdict CSuplMessageStep::doTestStepL()
	{
	__UHEAP_MARK;
		
	CSuplMessageContent* suplMsgContent=CSuplMessageContent::NewLC();
	BuildMessageL(suplMsgContent);
	
	CSuplMessage* suplMessageEncoder=CSuplMessage::NewLC(*suplMsgContent);
	
	//Encode supl start message
	RBuf8 encBuf;
	CleanupClosePushL(encBuf);
	TRAPD(err, suplMessageEncoder->EncodeL(encBuf));
	User::LeaveIfError(err);
	
	//Read the data from file
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
	
	//Compare the data		
	TInt comRet=encBuf.CompareF(buffer);

	if (comRet != 0)
		{
		SetTestStepResult(EFail);	
		CSuplMessage* suplMessageDecoder=CSuplMessage::NewLC();
		CSuplMessageContent* suplMessageResultContent=suplMessageDecoder->DecodeL(buffer);
		delete suplMessageResultContent;
		CleanupStack::PopAndDestroy(suplMessageDecoder);

		}
	else
		{
		//Decode the encoded message
		CSuplMessage* suplMessageDecoder=CSuplMessage::NewLC();
		CSuplMessageContent* suplMessageResultContent=suplMessageDecoder->DecodeL(encBuf);
		
		//Compare the decoded result with the original one
		if (*suplMessageResultContent==*suplMsgContent)
			{
			SetTestStepResult(EPass);
			}
		else
			{
			SetTestStepResult(EFail);
			}

		delete suplMessageResultContent;
		CleanupStack::PopAndDestroy(suplMessageDecoder);
		}
	CleanupStack::PopAndDestroy(&buffer);
	CleanupStack::PopAndDestroy(&encBuf);
	CleanupStack::PopAndDestroy(suplMessageEncoder);
	CleanupStack::PopAndDestroy(suplMsgContent);
	
	__UHEAP_MARKEND;
	return TestStepResult();
			
	}
	
	
void CSuplMessageStep::BuildSuplStartMessageL(CSuplMessageContent* aMessageContent)
	{
	// Qop field should be true
	TBool qopPresent;
	GetBoolFromConfig(ConfigSection(), _L("qoppresent"), qopPresent);
	aMessageContent->iSuplStartContent->iQopPresent=qopPresent;
	
	aMessageContent->iSuplStartContent->iSetCapabilitiesContent=CSuplSetCapabilitiesContent::NewL();
	BuildSetCapabilitiesL(aMessageContent->iSuplStartContent->iSetCapabilitiesContent);
	
	//LocationId --- CellInfo
	aMessageContent->iSuplStartContent->iLocationIdContent=CSuplLocationIdContent::NewL();
	
	aMessageContent->iSuplStartContent->iLocationIdContent->iCellInfoContent=CSuplCellInfoContent::NewL();
	TInt cellInfoChoice;
	GetIntFromConfig(ConfigSection(), _L("cellinfochoice"), cellInfoChoice);		
	aMessageContent->iSuplStartContent->iLocationIdContent->iCellInfoContent->iCellInformation=(TCellInformationChoice)cellInfoChoice;
	BuildCellInfoL(aMessageContent->iSuplStartContent->iLocationIdContent->iCellInfoContent);
	
	TInt locStatus;
	GetIntFromConfig(ConfigSection(), _L("locid_status"), locStatus);		
	aMessageContent->iSuplStartContent->iLocationIdContent->iLocationStatus=(TSuplLocationStatus)locStatus;	
	
	// QoP
	aMessageContent->iSuplStartContent->iQoPContent = CSuplQoPContent::NewL();
    BuildQoPL(aMessageContent->iSuplStartContent->iQoPContent);
	}

void CSuplMessageStep::BuildSuplResponseMessageL(CSuplMessageContent* aMessageContent)
	{
	// SLP Address field should be false
	TBool slpAddressPresent;
	GetBoolFromConfig(ConfigSection(), _L("slpaddresspresent"), slpAddressPresent);
	aMessageContent->iSuplResponseContent->iSlpAddressPresent=slpAddressPresent;
	
	// Set Auth Key field should be false
	TBool setAuthKeyPresent;
	GetBoolFromConfig(ConfigSection(), _L("setauthkeypresent"), setAuthKeyPresent);
	aMessageContent->iSuplResponseContent->iSetAuthKeyPresent=setAuthKeyPresent;
	
	// Set Auth Key field should be false
	TBool keyId4Present;
	GetBoolFromConfig(ConfigSection(), _L("keyid4present"), keyId4Present);
	aMessageContent->iSuplResponseContent->iKeyId4Present=keyId4Present;
	
	// Response --- PosMethod	
	TInt posMethod;
	GetIntFromConfig(ConfigSection(), _L("response_Posmethod"), posMethod);		
	aMessageContent->iSuplResponseContent->iPosMethod=(TSuplPosMethod)posMethod;			
		
	}
void CSuplMessageStep::BuildSuplEndMessageL(CSuplMessageContent* aMessageContent)
	{	
	// Position field should be false
	TBool positionPresent;
	GetBoolFromConfig(ConfigSection(), _L("positionpresent"), positionPresent);
	aMessageContent->iSuplEndContent->iPositionPresent=positionPresent;
	
	// Status Code field should be false
	TBool statusCodePresent;
	GetBoolFromConfig(ConfigSection(), _L("statuscodepresent"), statusCodePresent);
	aMessageContent->iSuplEndContent->iStatusCodePresent=statusCodePresent;

	// Ver field should be false
	TBool verPresent;
	GetBoolFromConfig(ConfigSection(), _L("verpresent"), verPresent);
	aMessageContent->iSuplEndContent->iVerPresent=verPresent;		
	}
void CSuplMessageStep::BuildSuplPosMessageL(CSuplMessageContent* aMessageContent)
	{
		
	// Velocity field should be false
	TBool velocityPresent;
	GetBoolFromConfig(ConfigSection(), _L("velocitypresent"), velocityPresent);
	aMessageContent->iSuplPosContent->iVelocityPresent=velocityPresent;

	aMessageContent->iSuplPosContent->iPosPayloadContent=CSuplPosPayloadContent::NewL();
	TInt posPayloadType;
	GetIntFromConfig(ConfigSection(), _L("pospayloadtype"), posPayloadType);		
	aMessageContent->iSuplPosContent->iPosPayloadContent->iPayloadType=(TSuplPosMethod)posPayloadType;			

	
	// PosPayLoad	
	TPtrC posPayLoad;
	GetStringFromConfig(ConfigSection(), _L("pospayload"), posPayLoad);
	aMessageContent->iSuplPosContent->iPosPayloadContent->iPayload.ReAllocL(posPayLoad.Length());	
	aMessageContent->iSuplPosContent->iPosPayloadContent->iPayload.Copy(posPayLoad);
		
	}
void CSuplMessageStep::BuildSuplPosInitMessageL(CSuplMessageContent* aMessageContent)
	{
	//aMessageContent->iSuplPosInitContent=CSuplPosInitContent::NewL();
	
	// Request Assist Data field should be true
	TBool reqAssistDataPresent;
	GetBoolFromConfig(ConfigSection(), _L("requestassistdatapresent"), reqAssistDataPresent);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataPresent=reqAssistDataPresent;
	
	// Position field should be false
	TBool positionPresent;
	GetBoolFromConfig(ConfigSection(), _L("positionpresent"), positionPresent);
	aMessageContent->iSuplPosInitContent->iPositionPresent=positionPresent;
	
	// Position field should be false
	TBool suplPosPresent;
	GetBoolFromConfig(ConfigSection(), _L("suplpospresent"), suplPosPresent);
	aMessageContent->iSuplPosInitContent->iSuplPosPresent=suplPosPresent;
	
	// Ver field should be false
	TBool verPresent;
	GetBoolFromConfig(ConfigSection(), _L("verpresent"), verPresent);
	aMessageContent->iSuplPosInitContent->iVerPresent=verPresent;
	
	aMessageContent->iSuplPosInitContent->iSetCapContent=CSuplSetCapabilitiesContent::NewL();
	
	
	// Build Set Capabilities
	BuildSetCapabilitiesL(aMessageContent->iSuplPosInitContent->iSetCapContent);
	
	// Request Assist Data	
	// Navigation model field should be false
	
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent=CSuplRequestedAssistDataContent::NewL();
	
	TBool naviModelPresent;
	GetBoolFromConfig(ConfigSection(), _L("navigationmodelpresent"), naviModelPresent);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent->iNavigationModelDataPresent=naviModelPresent;
	
	TBool almanacRequested;
	GetBoolFromConfig(ConfigSection(), _L("almanacrequested"), almanacRequested);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent->iAlmanacRequested=almanacRequested;
	
	TBool utcModelRequested;
	GetBoolFromConfig(ConfigSection(), _L("utcmodelrequested"), utcModelRequested);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent->iUtcModelRequested=utcModelRequested;

	TBool ionosphericModelRequested;
	GetBoolFromConfig(ConfigSection(), _L("ionosphericmodelrequested"), ionosphericModelRequested);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent->iIonosphericModelRequested=ionosphericModelRequested;
	
	TBool dgpsCorrectionsRequested;
	GetBoolFromConfig(ConfigSection(), _L("dgpscorrectionsrequested"), dgpsCorrectionsRequested);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent->iDgpsCorrectionsRequested=dgpsCorrectionsRequested;
	
	TBool referenceLocationRequested;
	GetBoolFromConfig(ConfigSection(), _L("referencelocationrequested"), referenceLocationRequested);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent->iReferenceLocationRequested=referenceLocationRequested;

	TBool referenceTimeRequested;
	GetBoolFromConfig(ConfigSection(), _L("referencetimerequested"), referenceTimeRequested);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent->iReferenceTimeRequested=referenceTimeRequested;

	TBool acquisitionAssistanceRequested;
	GetBoolFromConfig(ConfigSection(), _L("acquisitionassistancerequested"), acquisitionAssistanceRequested);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent->iAcquisitionAssistanceRequested=acquisitionAssistanceRequested;

	TBool realTimeIntegrityRequested;
	GetBoolFromConfig(ConfigSection(), _L("realtimeintegrityrequested"), realTimeIntegrityRequested);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent->iRealTimeIntegrityRequested=realTimeIntegrityRequested;

	TBool navigationModelRequested;
	GetBoolFromConfig(ConfigSection(), _L("navigationmodelrequested"), navigationModelRequested);
	aMessageContent->iSuplPosInitContent->iRequestedAssistDataContent->iNavigationModelRequested=navigationModelRequested;
	
	aMessageContent->iSuplPosInitContent->iLocationIdContent=CSuplLocationIdContent::NewL();
	
	aMessageContent->iSuplPosInitContent->iLocationIdContent->iCellInfoContent=CSuplCellInfoContent::NewL();
	// build cell info
	
	BuildCellInfoL(aMessageContent->iSuplPosInitContent->iLocationIdContent->iCellInfoContent);
		
	TInt locStatus;
	GetIntFromConfig(ConfigSection(), _L("locid_status"), locStatus);		
	aMessageContent->iSuplPosInitContent->iLocationIdContent->iLocationStatus=(TSuplLocationStatus)locStatus;	
	}


void CSuplMessageStep::BuildCellInfoL(CSuplCellInfoContent* aContent)
	{
	TInt cellInfoChoice;
	GetIntFromConfig(ConfigSection(), _L("cellinfochoice"), cellInfoChoice);		
	aContent->iCellInformation=(TCellInformationChoice)cellInfoChoice;

	switch (cellInfoChoice)
		{
	case EGsmCellInformation:
			{
			aContent->iGsmCellInformationContent=CSuplGsmCellInformationContent::NewL();
			TBool nmrPresent;
			GetBoolFromConfig(ConfigSection(), _L("gsm_cellinfo_NMRPresent"), nmrPresent);	
			aContent->iGsmCellInformationContent->iNMRPresent=nmrPresent;

			TBool taPresent;
			GetBoolFromConfig(ConfigSection(), _L("gsm_cellinfo_TAPresent"), taPresent);	
			aContent->iGsmCellInformationContent->iTAPresent=taPresent;

			TInt refMCC;
			GetIntFromConfig(ConfigSection(), _L("gsm_cellinfo_refMCC"), refMCC);		
			aContent->iGsmCellInformationContent->iRefMCC=refMCC;

			TInt refMNC;
			GetIntFromConfig(ConfigSection(), _L("gsm_cellinfo_refMNC"), refMNC);		
			aContent->iGsmCellInformationContent->iRefMNC=refMNC;

			TInt refLAC;
			GetIntFromConfig(ConfigSection(), _L("gsm_cellinfo_refLAC"), refLAC);		
			aContent->iGsmCellInformationContent->iRefLAC=refLAC;

			TInt refCI;
			GetIntFromConfig(ConfigSection(), _L("gsm_cellinfo_refCI"), refCI);		
			aContent->iGsmCellInformationContent->iRefCI=refCI;			
			}
			break;
	case EWcdmaCellInformation:
			{
			aContent->iWcdmaCellInformationContent=CSuplWcdmaCellInformationContent::NewL();
			TBool frequencyInfoPresent;
			GetBoolFromConfig(ConfigSection(), _L("wcdma_cellinfo_frequencyInfoPresent"), frequencyInfoPresent);	
			aContent->iWcdmaCellInformationContent->iFrequencyInfoPresent=frequencyInfoPresent;
			
			TBool primaryScramblingCodePresent;
			GetBoolFromConfig(ConfigSection(), _L("wcdma_cellinfo_primaryScramblingCodePresent"), primaryScramblingCodePresent);	
			aContent->iWcdmaCellInformationContent->iPrimaryScramblingCodePresent=primaryScramblingCodePresent;

			TBool measuredResultsListPresent;
			GetBoolFromConfig(ConfigSection(), _L("wcdma_cellinfo_primaryScramblingCodePresent"), measuredResultsListPresent);	
			aContent->iWcdmaCellInformationContent->iMeasuredResultsListPresent=measuredResultsListPresent;


			TInt refMCC;
			GetIntFromConfig(ConfigSection(), _L("wcdma_cellinfo_refMCC"), refMCC);		
			aContent->iWcdmaCellInformationContent->iRefMCC=refMCC;

			TInt refMNC;
			GetIntFromConfig(ConfigSection(), _L("wcdma_cellinfo_refMNC"), refMNC);		
			aContent->iWcdmaCellInformationContent->iRefMNC=refMNC;

			TInt refUC;
			GetIntFromConfig(ConfigSection(), _L("wcdma_cellinfo_refUC"), refUC);		
			aContent->iWcdmaCellInformationContent->iRefUC=refUC;				
			}
			break;
	case ECdmaCellInformation:
			{
			aContent->iCdmaCellInformationContent=CSuplCdmaCellInformationContent::NewL();
			TInt refNID;
			GetIntFromConfig(ConfigSection(), _L("cdma_cellinfo_refNID"), refNID);
			aContent->iCdmaCellInformationContent->iRefNID=refNID;
			
			TInt refSID;
			GetIntFromConfig(ConfigSection(), _L("cdma_cellinfo_refSID"), refSID);
			aContent->iCdmaCellInformationContent->iRefSID=refSID;
			
			TInt refBASEID;
			GetIntFromConfig(ConfigSection(), _L("cdma_cellinfo_refBASEID"), refBASEID);
			aContent->iCdmaCellInformationContent->iRefBASEID=refBASEID;

			TInt refBASELAT;
			GetIntFromConfig(ConfigSection(), _L("cdma_cellinfo_refBASELAT"), refBASELAT);
			aContent->iCdmaCellInformationContent->iRefBASELAT=refBASELAT;
			
			TInt reBASELONG;
			GetIntFromConfig(ConfigSection(), _L("cdma_cellinfo_reBASELONG"), reBASELONG);
			aContent->iCdmaCellInformationContent->iReBASELONG=reBASELONG;
			
			TInt refREFPN;
			GetIntFromConfig(ConfigSection(), _L("cdma_cellinfo_refREFPN"), refREFPN);
			aContent->iCdmaCellInformationContent->iRefREFPN=refREFPN;
			
			TInt refWeekNumber;
			GetIntFromConfig(ConfigSection(), _L("cdma_cellinfo_refWeekNumber"), refWeekNumber);
			aContent->iCdmaCellInformationContent->iRefWeekNumber=refWeekNumber;
			
			TInt refSeconds;
			GetIntFromConfig(ConfigSection(), _L("cdma_cellinfo_refSecond"), refSeconds);
			aContent->iCdmaCellInformationContent->iRefSeconds=refSeconds;				
			}
			break;
		}		
		
	}
void CSuplMessageStep::BuildSetCapabilitiesL(CSuplSetCapabilitiesContent* aContent)
	{
	// SETCAPABILITY --- PosTechnology
	aContent->iPosTechnologyContent=CSuplPosTechnologyContent::NewL();
	TBool posTechAgpsSetAssisted;
	GetBoolFromConfig(ConfigSection(), _L("setcap_Postech_agpssetassisted"), posTechAgpsSetAssisted);	
	aContent->iPosTechnologyContent->iAgpsSETassisted=posTechAgpsSetAssisted;
	
	TBool posTechAgpsSetBased;
	GetBoolFromConfig(ConfigSection(), _L("setcap_Postech_agpssetbased"), posTechAgpsSetBased);	
	aContent->iPosTechnologyContent->iAgpsSETBased=posTechAgpsSetBased;

	TBool posTechAutonomousGps;
	GetBoolFromConfig(ConfigSection(), _L("setcap_Postech_autonomousgps"), posTechAutonomousGps);	
	aContent->iPosTechnologyContent->iAutonomousGPS=posTechAutonomousGps;
	
	TBool posTechAflt;
	GetBoolFromConfig(ConfigSection(), _L("setcap_Postech_aflt"), posTechAflt);	
	aContent->iPosTechnologyContent->iAFLT=posTechAflt;
	
	TBool posTechEcid;
	GetBoolFromConfig(ConfigSection(), _L("setcap_Postech_acid"), posTechEcid);	
	aContent->iPosTechnologyContent->iECID=posTechEcid;
	
	TBool posTechEotd;
	GetBoolFromConfig(ConfigSection(), _L("setcap_Postech_aotd"), posTechEotd);	
	aContent->iPosTechnologyContent->iEOTD=posTechEotd;
	
	TBool posTechOtdoa;
	GetBoolFromConfig(ConfigSection(), _L("setcap_Postech_atdoa"), posTechOtdoa);	
	aContent->iPosTechnologyContent->iOTDOA=posTechOtdoa;
	
	// SETCAPABILITY --- PosMethod	
	TInt posMethod;
	GetIntFromConfig(ConfigSection(), _L("setcap_Posmethod"), posMethod);		
	aContent->iPrefMethod=(TSuplPrefMethod)posMethod;	
	
	// SETCAPABILITY --- PosProtocol
	aContent->iPosProtocolContent=CSuplPosProtocolContent::NewL();
	TBool posProtTia801;
	GetBoolFromConfig(ConfigSection(), _L("setcap_Posprotocol_tia801"), posProtTia801);	
	aContent->iPosProtocolContent->iTia801=posProtTia801;
	
	TBool posProtRrlp;
	GetBoolFromConfig(ConfigSection(), _L("setcap_Posprotocol_rrlp"), posProtRrlp);	
	aContent->iPosProtocolContent->iRrlp=posProtRrlp;
	
	
	TBool posProtRrc;
	GetBoolFromConfig(ConfigSection(), _L("setcap_Posprotocol_rrc"), posProtRrc);	
	aContent->iPosProtocolContent->iRrc=posProtRrc;		
		
	}

void CSuplMessageStep::BuildQoPL(CSuplQoPContent* aContent)
	{
	aContent->iVeraccPresent = EFalse;
	aContent->iMaxLocAgePresent = EFalse;
	aContent->iDelayPresent = EFalse;
	aContent->iHoracc= 20;
	}

void CSuplMessageStep::BuildPositionInfoL(CSuplPositionContent* posContent)
{
	TInt Longitude ; // -180 to 180
	TInt Latitude ; // -90 to 90

	GetIntFromConfig(ConfigSection(), _L("posInfo_Longitude"), Longitude);
	GetIntFromConfig(ConfigSection(), _L("posInfo_Latitude"), Latitude);

	posContent->iPositionEstimateContent = CSuplPositionEstimateContent::NewL();	
	
	if (Latitude >= 0)
		{
		posContent->iPositionEstimateContent->iLatitudeSign = ENorth;
		}
	else
		{
		posContent->iPositionEstimateContent->iLatitudeSign = ESouth;
		}
		
	posContent->iVelocityPresent = EFalse;
	posContent->iUtcTimeStamp.UniversalTime(); // Use the current time. SUPL server ignores actual value.
	
	posContent->iPositionEstimateContent->iUncertainty = EFalse;
	posContent->iPositionEstimateContent->iConfidence = EFalse;
	posContent->iPositionEstimateContent->iAltitudInfo = EFalse;
	
	// Set Location 
	posContent->iPositionEstimateContent->iLatitude = static_cast<TUint32>((Abs(Latitude) * 8388608.0 + 0.5) / 90.0);
	posContent->iPositionEstimateContent->iLongitude = static_cast<TInt32>((Longitude * 2097152.0 + 0.5) / 45.0);
}
// End of file


