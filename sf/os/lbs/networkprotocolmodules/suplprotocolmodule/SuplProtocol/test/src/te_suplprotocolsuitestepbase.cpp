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
 @file Te_suplprotocolSuiteStepBase.cpp
 @internalTechnology
*/

#include "te_suplprotocolsuitestepbase.h"
#include "te_suplprotocolsuitedefs.h"
#include "suplresponse.h"
#include "suplpos.h"
#include "rrlpassistancedata.h"
#include "rrlpmeasureposrequest.h"
#include "suplend.h"
#include "suplconnectionmanager.h" // only for KInetAddr

// Device driver constants

TVerdict CTe_suplprotocolSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	// START MEMORY TEST
	__UHEAP_MARK;

	// Construct a new ActiveScheduler and install it
	iSched = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iSched);
	
	// Create Module observer - Gateway side
	iGatewayObserver = CSuplGatewayObserver::NewL();

	// Create Module observer - Network side
	iNetworkObserver = CSuplNetworkTestObserver::NewL();
	
	ResetObserversL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_suplprotocolSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	// Delete Gateway observer
	delete iGatewayObserver;
	iGatewayObserver = NULL;
	
	// Delete Network Observer
	delete iNetworkObserver;
	iNetworkObserver = NULL;
	// Remove ActiveScheduler
	CActiveScheduler::Install(NULL);
	delete iSched;
	iSched = NULL;
	
	// END MEMORY TEST
	__UHEAP_MARKEND;

	return TestStepResult();
	}

CTe_suplprotocolSuiteStepBase::~CTe_suplprotocolSuiteStepBase()
	{
	
	}

CTe_suplprotocolSuiteStepBase::CTe_suplprotocolSuiteStepBase()
	{
	}


/** Reset Gateway-side observer callback status.
*/
void CTe_suplprotocolSuiteStepBase::ResetGatewayObserverL()
	{
	User::LeaveIfNull(iGatewayObserver);
	iGatewayObserver->ResetWasObserverCalled();
	}


/** Reset Network-side observer callback status.
*/
void CTe_suplprotocolSuiteStepBase::ResetNetworkObserverL()
	{
	User::LeaveIfNull(iNetworkObserver);
	iNetworkObserver->ResetWasObserverCalled();
	}


/** Reset callback status for all observers.
*/
void CTe_suplprotocolSuiteStepBase::ResetObserversL()
	{
	ResetGatewayObserverL();
	ResetNetworkObserverL();
	}


/** Wait for and check a Network-side observer callback.
@param aExpectedResult The expected type of callback.
@return TVerdict EFail if expected callback not received.
*/
TVerdict CTe_suplprotocolSuiteStepBase::
					CheckNetworkCallbackL(TInt aExpectedResult)
	{
	User::LeaveIfNull(iNetworkObserver);

	// Wait for network observer callback
	iNetworkObserver->WaitForCallBack();
	
	// Check that callback matches the expected type
	if ((aExpectedResult != iNetworkObserver->CallBackResult()) ||
		!iNetworkObserver->WasObserverCalled())
		{
		SetTestStepResult(EFail);
		}
	else if(aExpectedResult!=CSuplNetworkTestObserver::EConnectReq && 
			aExpectedResult!=CSuplNetworkTestObserver::EDisconnectReq)
		{
		CSuplSessionId* sessionId = CSuplSessionId::NewL();
		iNetworkObserver->ReceivedMessage()->GetSessionId(*sessionId);
		
		//MUST be uncommented when the support for the MSISDN will be implemented
		//if(!CheckSetSessionId(sessionId->iSetSessionId))
		//	{
		//	SetTestStepResult(EFail);
		//	}
		
		delete sessionId;
		}

	// Immediately reset observer callback status
	ResetNetworkObserverL();
	return TestStepResult();
	}


/** Wait for and check a Gateway-side observer callback.
@param aExpectedResult The expected type of callback.
@return TVerdict EFail if expected callback not received.
*/
TVerdict CTe_suplprotocolSuiteStepBase::
					CheckGatewayCallbackL(TInt aExpectedResult)
	{
	User::LeaveIfNull(iGatewayObserver);

	// Wait for gateway observer callback
	iGatewayObserver->WaitForCallBack();
	
	// Check that callback matches the expected type
	if ((aExpectedResult != iGatewayObserver->CallBackResult()) ||
		!iGatewayObserver->WasObserverCalled()
	   )
		{
		SetTestStepResult(EFail);
		}

	// Immediately reset observer callback status
	ResetGatewayObserverL();
	return TestStepResult();
	}


/** Set dummy data into assistance data set.
This utility method adds dummy data within an assistance data set.

@param aAssistData The assistance data set to have dummy data added.
@param aDataRequestMask Identifies the set of data types for which
	dummy data is to be added.
*/
void CTe_suplprotocolSuiteStepBase::SetDummyAssistanceData(
								RLbsAssistanceDataBuilderSet& aAssistData, 
								TLbsAsistanceDataGroup aDataRequestMask)
	{
	// Clear any previously stored data.
	aAssistData.ClearAllBuilders();

	// Get UTC time
	TTime utcTime;
	utcTime.UniversalTime();

	// Set up dummy data for all relevant requested items
	if (aDataRequestMask & EAssistanceDataAquisitionAssistance)
		{
		RUEPositioningGpsAcquisitionAssistanceBuilder* acquisition;
		if (KErrNone == aAssistData.GetDataBuilder(acquisition))
			{
			acquisition->SetTimeStamp(utcTime);
			acquisition->SetField(TUEPositioningGpsAcquisitionAssistance::EGpsReferenceTime, 
								  TUint(123055));
			}
		}

	if (aDataRequestMask & EAssistanceDataNavigationModel)
		{
		RUEPositioningGpsNavigationModelBuilder* navigation;
		if (KErrNone == aAssistData.GetDataBuilder(navigation))
			{
			navigation->SetTimeStamp(utcTime);

			RNavigationModelSatInfoArrayBuilder satinfoArray;
			navigation->GetArrayBuilder(TUEPositioningGpsNavigationModel::ENavigationModelSatelliteInfoArray, 
										satinfoArray);
			RNavigationModelSatInfoBuilder satinfoBuilder;
			satinfoArray.GetFieldBuilder(0, satinfoBuilder);
			satinfoBuilder.SetField(TNavigationModelSatInfo::ESatId, TUint(1));
			}
		}

	if (aDataRequestMask & EAssistanceDataIonosphericModel)
		{
		RUEPositioningGpsIonosphericModelBuilder* ionospheric;
		if (KErrNone == aAssistData.GetDataBuilder(ionospheric))
			{
			ionospheric->SetTimeStamp(utcTime);
			ionospheric->SetField(TUEPositioningGpsIonosphericModel::EAlfa0, TUint(123));
			}
		}

	if (aDataRequestMask & EAssistanceDataReferenceLocation)
		{
		RReferenceLocationBuilder* refLoc;
		if (KErrNone == aAssistData.GetDataBuilder(refLoc))
			{
			refLoc->SetTimeStamp(utcTime);

			REllipsoidPointAltitudeEllipsoideBuilder locBuilder;
			refLoc->GetFieldBuilder(TReferenceLocation::EEllipsoidPointAltitudeEllipsoide, 
									locBuilder);
			locBuilder.SetField(TEllipsoidPointAltitudeEllipsoide::ELatitudeSign,
								TEllipsoidPointAltitudeEllipsoide::ESouth);
			}
		}

	if (aDataRequestMask & EAssistanceDataPositioningGpsUtcModel)
		{
		RUEPositioningGpsUtcModelBuilder* utcModel;
		if (KErrNone == aAssistData.GetDataBuilder(utcModel))
			{
			utcModel->SetTimeStamp(utcTime);
			utcModel->SetField(TUEPositioningGpsUtcModel::EA1, TUint(12));
			}
		}

	if (aDataRequestMask & EAssistanceDataAlmanac)
		{
		RUEPositioningGpsAlmanacBuilder* almanac;
		if (KErrNone == aAssistData.GetDataBuilder(almanac))
			{
			almanac->SetTimeStamp(utcTime);
			almanac->SetField(TUEPositioningGpsAlmanac::EWnA, TUint(12));
			}
		};

	if (aDataRequestMask & EAssistanceDataBadSatList)
		{
		RBadSatListBuilder* badsat;
		if (KErrNone == aAssistData.GetDataBuilder(badsat))
			{
			badsat->SetTimeStamp(utcTime);

			RBadSatArrayBuilder badsatArray;
			badsat->GetArrayBuilder(TBadSatList::EBadSatIdArray, 
										   badsatArray);
			badsatArray.SetElement(0, TUint(1));
			}
		}

	if (aDataRequestMask & EAssistanceDataReferenceTime)
		{
		RUEPositioningGpsReferenceTimeBuilder* refTime;
		if (KErrNone == aAssistData.GetDataBuilder(refTime))
			{
			refTime->SetTimeStamp(utcTime);
			refTime->SetField(TUEPositioningGpsReferenceTime::EGpsWeek, TUint(12));
			}
		}

	}
	
CSuplMessageBase* CTe_suplprotocolSuiteStepBase::BuildSuplResponseL(TPositionModuleInfo::TTechnologyType aTechType)
{
	CSuplResponse* resp = CSuplResponse::NewL();
		
	CSuplSessionId* sessionId = CSuplSessionId::NewL();
	CleanupStack::PushL(sessionId);
	
	// Set session Id must be same as in SUPL START
	sessionId->iSetSessionId->iSessionId = iNetworkObserver->SessionId();
	SetSessionSetId(sessionId->iSetSessionId->iSetId);
	sessionId->iSetSessionIdPresent=ETrue;
	
	// SLP session Id
	_LIT8(KSlpIdSessionId, "EFGH");
	sessionId->iSlpSessionId->iSessionId.Copy(KSlpIdSessionId);
	sessionId->iSlpSessionId->iSlpAddress->iSlpAddressType = ESuplSlpAddressTypeIp; 
	sessionId->iSlpSessionId->iSlpAddress->iIpAddress->iIpAddressType = ESuplIpAddressTypeV4;	
	_LIT8(KSlpIdIpAddress, "VWXY");
	sessionId->iSlpSessionId->iSlpAddress->iIpAddress->iIpAddress.Copy(KSlpIdIpAddress);
	sessionId->iSlpSessionIdPresent=ETrue;
	
	// Set the Positioning Method preferred by the SLP
	TLbsNetPosRequestMethod method;
	TLbsNetPosMethod method1;
	method1.SetPosMethod(KLbsPositioningMeansGps, aTechType);
	const TLbsNetPosMethod posMethods [] =
		{
		method1
		};
		
	method.SetPosMethods(posMethods, 1);
	
	resp->SetPosMethod(method);
	resp->SetSessionId(*sessionId);
	
	CleanupStack::PopAndDestroy(sessionId);
	
	return resp;
}


/** Build a SUPL POS with an RRLP ASSISTANCE DATA payload
*/
CSuplMessageBase* CTe_suplprotocolSuiteStepBase::BuildSuplPosAssitDataL(TLbsAsistanceDataGroup aDataInMessage, TBool moreDataFollows)
{
	CSuplPos* suplPos = CSuplPos::NewL(EFalse);
	CleanupStack::PushL(suplPos);
	
	CSuplSessionId* sessionId = CSuplSessionId::NewL();
	CleanupStack::PushL(sessionId);
	
	// Set session Id must be same as in SUPL START
	sessionId->iSetSessionId->iSessionId = iNetworkObserver->SessionId();
	SetSessionSetId(sessionId->iSetSessionId->iSetId);
	sessionId->iSetSessionIdPresent=ETrue;
	
	// SLP session Id
	_LIT8(KSlpIdSessionId, "EFGH");
	sessionId->iSlpSessionId->iSessionId.Copy(KSlpIdSessionId);
	sessionId->iSlpSessionId->iSlpAddress->iSlpAddressType = ESuplSlpAddressTypeIp; 
	sessionId->iSlpSessionId->iSlpAddress->iIpAddress->iIpAddressType = ESuplIpAddressTypeV4;
	_LIT8(KSlpIdIpAddress, "VWXY");
	sessionId->iSlpSessionId->iSlpAddress->iIpAddress->iIpAddress.Copy(KSlpIdIpAddress);
	sessionId->iSlpSessionIdPresent=ETrue;
	
	
	RLbsAssistanceDataBuilderSet builderSet;
	CleanupClosePushL(builderSet);
	builderSet.OpenL();
	
	SetDummyAssistanceData(builderSet, aDataInMessage);
	
	CRrlpAssistanceData* payload = CRrlpAssistanceData::NewL();
	CleanupStack::PushL(payload);
	
	payload->SetAssistanceDataGroup(aDataInMessage);
	
	payload->SetAssistanceDataBuilderSet(builderSet);

	payload->SetMoreAssDataToBeSent(moreDataFollows);
	
	suplPos->SetPosPayload(payload);
	suplPos->SetSessionId(*sessionId);
	
	CleanupStack::Pop(payload);
	CleanupStack::PopAndDestroy();//aBuilderSet
	CleanupStack::PopAndDestroy(sessionId);
	CleanupStack::Pop(suplPos);

	return suplPos;
}


CSuplMessageBase* CTe_suplprotocolSuiteStepBase::BuildSuplPosRequestL(TPositionModuleInfo::TTechnologyType aTechType, TLbsAsistanceDataGroup aDataInMessage, const TUid& aPosMeans)
{
	CSuplPos* suplPos = CSuplPos::NewL(EFalse);

	CleanupStack::PushL(suplPos);
	
	CSuplSessionId* sessionId = CSuplSessionId::NewL();
	CleanupStack::PushL(sessionId);
	
	// Set session Id must be same as in SUPL START
	sessionId->iSetSessionId->iSessionId = iNetworkObserver->SessionId();
	SetSessionSetId(sessionId->iSetSessionId->iSetId);
	sessionId->iSetSessionIdPresent=ETrue;
	
	// SLP session Id
	_LIT8(KSlpIdSessionId, "EFGH");
	sessionId->iSlpSessionId->iSessionId.Copy(KSlpIdSessionId);
	sessionId->iSlpSessionId->iSlpAddress->iSlpAddressType = ESuplSlpAddressTypeIp; 
	sessionId->iSlpSessionId->iSlpAddress->iIpAddress->iIpAddressType = ESuplIpAddressTypeV4;
	_LIT8(KSlpIdIpAddress, "VWXY");
	sessionId->iSlpSessionId->iSlpAddress->iIpAddress->iIpAddress.Copy(KSlpIdIpAddress);
	sessionId->iSlpSessionIdPresent=ETrue;
	
	
	CRrlpMeasurePositionRequest* payload = CRrlpMeasurePositionRequest::NewL();
	CleanupStack::PushL(payload);

	
	// Create specified Assistance Data Builder Set and set it
	// in the payload
	if (aDataInMessage != EAssistanceDataNone)
		{
		RLbsAssistanceDataBuilderSet builderSet;
		CleanupClosePushL(builderSet);
		builderSet.OpenL();
		SetDummyAssistanceData(builderSet, aDataInMessage);
		payload->SetAssistanceDataGroup(aDataInMessage);
		payload->SetAssistanceDataBuilderSet(builderSet);
		CleanupStack::PopAndDestroy();//builderSet
		}

	// Set in the payload the Positioning Method preferred by the SLP
	TLbsNetPosRequestMethod method;
	TLbsNetPosMethod method1;
	method1.SetPosMethod(aPosMeans, aTechType);
	const TLbsNetPosMethod posMethods [] =
		{
		method1
		};
	method.SetPosMethods(posMethods, 1);
	payload->SetMethod(method);


	// Add quality to the payload to set a MaxFixTime
	// (otherwise we risk an inmediate error due to LBS 
	// not responding)
	TLbsNetPosRequestQuality quality;
	quality.SetMaxFixTime(TTimeIntervalMicroSeconds(3000000));// 3 seconds
	payload->SetQuality(quality);

	// Set payload in SUPL POS
	suplPos->SetPosPayload(payload);
	// set sessionid
	suplPos->SetSessionId(*sessionId);


	// Set SET ID and SLP ID in session ID !!!


	CleanupStack::Pop(payload);
	CleanupStack::PopAndDestroy(sessionId);
	CleanupStack::Pop(suplPos);

	return suplPos;	
}



/** Build a SUPL END
*/
CSuplMessageBase* CTe_suplprotocolSuiteStepBase::BuildSuplEndL(TBool aSetDummyPos, TInt errorCode)
{
	CSuplEnd* suplEnd = CSuplEnd::NewL(EFalse);
	CleanupStack::PushL(suplEnd);
	
	
	CSuplSessionId* sessionId = CSuplSessionId::NewL();
	CleanupStack::PushL(sessionId);
	
	// Set session Id must be same as in SUPL START
	sessionId->iSetSessionId->iSessionId = iNetworkObserver->SessionId();
	SetSessionSetId(sessionId->iSetSessionId->iSetId);
	sessionId->iSetSessionIdPresent=ETrue;
	
	// SLP session Id
	_LIT8(KSlpIdSessionId, "EFGH");
	sessionId->iSlpSessionId->iSessionId.Copy(KSlpIdSessionId);
	sessionId->iSlpSessionId->iSlpAddress->iSlpAddressType = ESuplSlpAddressTypeIp; 
	sessionId->iSlpSessionId->iSlpAddress->iIpAddress->iIpAddressType = ESuplIpAddressTypeV4;
	_LIT8(KSlpIdIpAddress, "VWXY");
	sessionId->iSlpSessionId->iSlpAddress->iIpAddress->iIpAddress.Copy(KSlpIdIpAddress);
	sessionId->iSlpSessionIdPresent=ETrue;
	
	
	if (aSetDummyPos)
	{
		TPosition position;
		TPositionInfo positionInfo;
		positionInfo.SetPosition(position);

		suplEnd->SetPosition(positionInfo);
	}
	
	if (KErrNone != errorCode)
	{
		// Set a SUPL status code with
		// reason code value between 0 and 99
		//
		TSuplStatusCode status = ESuplStatusProtocolError;
		suplEnd->SetStatusCode(status);
	}
	
	//set sessionId
	suplEnd->SetSessionId(*sessionId);
	
	CleanupStack::PopAndDestroy(sessionId);
	CleanupStack::Pop(suplEnd);
	
	return suplEnd;
}

void CTe_suplprotocolSuiteStepBase::SetSessionSetId(CSuplSetId* aSetId)
	{
	if(!aSetId)
		{
		return;
		}
	
	aSetId->iSetIdType = ESuplSetIdTypeMsisdn; // TSuplSetIdType
	TDes8& des = aSetId->iSetId;
	TPtrC num;
	if(GetStringFromConfig(ConfigSection(), _L("MSISDN"), num))
		{
		des.Copy(num);
		}
	else
		{
		//Fills the descriptor with all bits set to 1
		des.SetLength(des.MaxLength());
		des.Fill(0xFF);
		}
	}

TBool CTe_suplprotocolSuiteStepBase::CheckSetSessionId(const CSuplSetSessionId* aSetSessionId)
	{
	if(!aSetSessionId || !aSetSessionId->iSetId)
		{
		return EFalse;
		}
	CSuplSetId* setId = aSetSessionId->iSetId;
	if(setId->iSetIdType!=ESuplSetIdTypeMsisdn)
		{
		return EFalse;
		}
	
	TDes8& des = setId->iSetId;
	TPtrC num;  
	if(GetStringFromConfig(ConfigSection(), _L("MSISDN"), num))
		{
		if(num.Length()!=num.Length())
			{
			return EFalse;
			}
		for(TInt i=num.Length()-1; i>=0; --i)
			{
			if(num[i]!=des[i])
				{
				return EFalse;
				}
			}
		}
	else
		{
		if(des.Length()!=des.MaxLength())
			{
			return EFalse;
			}
		for(TInt i=des.Length()-1; i>=0; --i)
			{
			if(des[i]!=0xFF)
				{
				return EFalse;
				}
			}
		}
	return ETrue;
	}
