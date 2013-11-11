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

#include "mmtsy.h"
#include "testdef.h"
#include <etelmm.h>
#include <et_clsvr.h>
#include "Dmmlog.h"
#include <etelext.h>

CMobileLocationServicesDMmTsy* CMobileLocationServicesDMmTsy::NewL(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName)
	{
	CMobileLocationServicesDMmTsy* lcs=new(ELeave) CMobileLocationServicesDMmTsy(aPhone,aFac,aName);
	CleanupStack::PushL(lcs);
	lcs->ConstructL();
	CleanupStack::Pop();
	return lcs;
	}

CMobileLocationServicesDMmTsy::CMobileLocationServicesDMmTsy(CPhoneDMmTsy *aPhone, CPhoneFactoryDummyBase* aFac,const TDesC& aName) :
	CSubSessionExtDummyBase(aFac), iPhone(aPhone)
	{
    iLocationServices=aName;
	}

void CMobileLocationServicesDMmTsy::ConstructL()
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy created"));
	}

CMobileLocationServicesDMmTsy::~CMobileLocationServicesDMmTsy()
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy destroyed"));
	}

CTelObject* CMobileLocationServicesDMmTsy::OpenNewObjectByNameL(const TDesC& /*aName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CMobileLocationServicesDMmTsy::OpenNewObjectL(TDes& /*aNewName*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

void CMobileLocationServicesDMmTsy::Init()
	{
	}

TInt CMobileLocationServicesDMmTsy::RegisterNotification(const TInt /*aIpc*/)
	{
	// RegisterNotification is called when the server recognises that this notification
	// is being posted for the first time on this sub-session object.

	// It enables the TSY to "turn on" any regular notification messages that it may 
	// receive from the phone
	return KErrNone;
	}

TInt CMobileLocationServicesDMmTsy::DeregisterNotification(const TInt /*aIpc*/)
	{
	// DeregisterNotification is called when the server recognises that this notification
	// will not be posted again because the last client to have a handle on this sub-session
	// object has just closed the handle.

	// It enables the TSY to "turn off" any regular notification messages that it may
	// receive from the phone
	return KErrNone;
	}

TInt CMobileLocationServicesDMmTsy::NumberOfSlotsL(const TInt aIpc)
	{
	// NumberOfSlotsL is called by the server when it is registering a new notification
	// It enables the TSY to tell the server how many buffer slots to allocate for
	// "repost immediately" notifications that may trigger before clients collect them

	TInt numberOfSlots=1;
	switch (aIpc)
		{
	case EMobileLocationServicesNotifyMtlr:
	case EMobileLocationServicesNotifyMeasurementControl:
	case EMobileLocationServicesNotifyResetUePositioningInformation:
		numberOfSlots=2;
		break;
	default:
		break;
		}
	return numberOfSlots;         
	}

CTelObject::TReqMode CMobileLocationServicesDMmTsy::ReqModeL(const TInt aIpc)
	{
	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
	case EMobileLocationServicesSetLcsDomain:
	case EMobileLocationServicesSendMtlrResponse:
	case EMobileLocationServicesSendMolr:
	case EMobileLocationServicesSendMtlrError:
	case EMobileLocationServicesRejectMtlr:
	case EMobileLocationServicesSendMeasurementReport:
	case EMobileLocationServicesSendMeasurementControlFailure:
		break;
	case EMobileLocationServicesNotifyMtlr:
	case EMobileLocationServicesNotifyMeasurementControl:
	case EMobileLocationServicesNotifyResetUePositioningInformation:
		ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
		break;
		}
	return ret;
	}

TInt CMobileLocationServicesDMmTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage& aPackage)
	{
	// ExtFunc is called by the server when it has a "extended", i.e. non-core ETel request 
	// for the TSY to process
	// A request handle, request type and request data are passed to the TSY
	TAny* dataPtr=aPackage.Ptr1();
	TAny* dataPtr2=aPackage.Ptr2();
	// The request data has to extracted from TDataPackage and the TAny* pointers have to
	// be "cast" to the expected request data type
	switch (aIpc)
		{
	case EMobileLocationServicesSetLcsDomain:
		return SetLcsDomain(aTsyReqHandle,REINTERPRET_CAST(RMobileLocationServices::TDomain*,dataPtr));
	case EMobileLocationServicesNotifyMtlr:
		return NotifyMtlr(aTsyReqHandle,aPackage.Des1n());
	case EMobileLocationServicesSendMtlrResponse:
		return SendMtlrResponse(aTsyReqHandle,REINTERPRET_CAST(RMobileLocationServices::TMtlrResponse*,dataPtr));
	case EMobileLocationServicesSendMolr:
		return SendMolr(aTsyReqHandle,aPackage.Des1n(),aPackage.Des2n());
	case EMobileLocationServicesSendMtlrError:
		return SendMtlrResponse(aTsyReqHandle,REINTERPRET_CAST(RMobileLocationServices::TMtlrError*,dataPtr));
	case EMobileLocationServicesRejectMtlr:
		return SendMtlrResponse(aTsyReqHandle,REINTERPRET_CAST(RMobileLocationServices::TMtlrReject*,dataPtr));
	case EMobileLocationServicesNotifyMeasurementControl:
		return NotifyMeasurementControl(aTsyReqHandle,aPackage.Des1n());
	case EMobileLocationServicesSendMeasurementReport:
		return SendMeasurementReport(aTsyReqHandle,aPackage.Des1n());
	case EMobileLocationServicesSendMeasurementControlFailure:
		return SendMeasurementReport(aTsyReqHandle,REINTERPRET_CAST(RMobileLocationServices::TMeasurementControlFailure*,dataPtr));
	case EMobileLocationServicesNotifyResetUePositioningInformation:
		return NotifyResetUePositioningInformation(aTsyReqHandle,REINTERPRET_CAST(RMobileLocationServices::TUePosTechnology*, dataPtr));
	default:
		return KErrGeneral;
		}
	}

TInt CMobileLocationServicesDMmTsy::CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle)
	{
	// CancelService is called by the server when it is "cleaning-up" any still outstanding
	// asynchronous requests before closing a client's sub-session.
	// This will happen if a client closes its R-class handle without cancelling outstanding
	// asynchronous requests.
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy: - CancelService called"));
	switch (aIpc)
		{
	case EMobileLocationServicesNotifyMtlr:
		return NotifyMtlrCancel(aTsyReqHandle);
	case EMobileLocationServicesSendMolr:
		return SendMolrCancel(aTsyReqHandle);
	case EMobileLocationServicesNotifyMeasurementControl:
		return NotifyMeasurementControlCancel(aTsyReqHandle);
	case EMobileLocationServicesSendMeasurementReport:
	case EMobileLocationServicesSendMeasurementControlFailure:
		return SendMeasurementReportCancel(aTsyReqHandle);
	case EMobileLocationServicesSendMtlrResponse:
	case EMobileLocationServicesSendMtlrError:
	case EMobileLocationServicesRejectMtlr:
		return SendMtlrResponseCancel(aTsyReqHandle);
	case EMobileLocationServicesNotifyResetUePositioningInformation:
		return NotifyResetUePositioningInformationCancel(aTsyReqHandle);
	default:
		return KErrGeneral;
		}
	}
TInt CMobileLocationServicesDMmTsy::SendMtlrResponse(const TTsyReqHandle aTsyReqHandle,RMobileLocationServices::TMtlrResponse* aMtlrResponse)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::SendMtlrResponse Called"));
	if (*aMtlrResponse==DMMTSY_MTLR_RESPONSE1 || *aMtlrResponse==DMMTSY_MTLR_RESPONSE2)
		{
		iPhone->AddDelayedReq(aTsyReqHandle,this, 500000);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}

TInt CMobileLocationServicesDMmTsy::SendMtlrResponse(const TTsyReqHandle aTsyReqHandle,RMobileLocationServices::TMtlrError* aMtlrError)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::SendMtlrResponse Called"));
	if (*aMtlrError==DMMTSY_MTLR_ERROR1 || *aMtlrError==DMMTSY_MTLR_ERROR2)
		{
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}
	
TInt CMobileLocationServicesDMmTsy::SendMtlrResponse(const TTsyReqHandle aTsyReqHandle,RMobileLocationServices::TMtlrReject* aMtlrReject)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::SendMtlrResponse Called"));
	if(*aMtlrReject==DMMTSY_MTLR_REJECT1 || *aMtlrReject==DMMTSY_MTLR_REJECT2)
		{
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}

TInt CMobileLocationServicesDMmTsy::NotifyMtlr(const TTsyReqHandle aTsyReqHandle,TDes8* aNotifyMtlr)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::NotifyMtlr Called"));
	RMobileLocationServices::TNotifyMtlrV7Pckg *notifyMtlrPckg = (RMobileLocationServices::TNotifyMtlrV7Pckg*)(aNotifyMtlr);
	RMobileLocationServices::TNotifyMtlrV7 &	notifyMtlr = (*notifyMtlrPckg)();
	if(notifyMtlr.iLocationInfo.iLcsClientName.iLcsDataCodingScheme == DMMTSY_DATA_CODING_SCHEME1)
		{
		notifyMtlr.iMtlrLocationEstimateType=DMMTSY_LOCATION_ESTIMATE_TYPE1;
		notifyMtlr.iNotificationType=DMMTSY_NOTIFICATION_TYPE1;
		iPhone->AddDelayedReq(aTsyReqHandle,this, 100000);
		}
	if(notifyMtlr.iLocationInfo.iLcsClientName.iLcsDataCodingScheme == DMMTSY_DATA_CODING_SCHEME2)
		{
		notifyMtlr.iMtlrLocationEstimateType=DMMTSY_LOCATION_ESTIMATE_TYPE2;
		notifyMtlr.iNotificationType=DMMTSY_NOTIFICATION_TYPE2;
		iPhone->AddDelayedReq(aTsyReqHandle,this, 100000);	
		}
	if(notifyMtlr.iLocationInfo.iLcsClientName.iLcsDataCodingScheme == DMMTSY_DATA_CODING_SCHEME3)
		{
		notifyMtlr.iMtlrLocationEstimateType=DMMTSY_LOCATION_ESTIMATE_TYPE2;
		notifyMtlr.iNotificationType=DMMTSY_NOTIFICATION_TYPE3;
		iPhone->AddDelayedReq(aTsyReqHandle,this, 100000);	
		}
	if(notifyMtlr.iLocationInfo.iLcsClientName.iLcsDataCodingScheme == DMMTSY_DATA_CODING_SCHEME4)		
		{
		notifyMtlr.iMtlrLocationEstimateType=DMMTSY_LOCATION_ESTIMATE_TYPE2;
		notifyMtlr.iNotificationType=DMMTSY_NOTIFICATION_TYPE4;
		iPhone->AddDelayedReq(aTsyReqHandle,this, 100000);	
		}
	if(notifyMtlr.iLocationInfo.iLcsClientName.iLcsDataCodingScheme == DMMTSY_DATA_CODING_SCHEME5)		
		{
		notifyMtlr.iMtlrLocationEstimateType=DMMTSY_LOCATION_ESTIMATE_TYPE2;
		notifyMtlr.iNotificationType=DMMTSY_NOTIFICATION_TYPE5;
		iPhone->AddDelayedReq(aTsyReqHandle,this, 100000);	
		}
	if(notifyMtlr.iLocationInfo.iLcsClientName.iLcsDataCodingScheme == DMMTSY_DATA_CODING_SCHEME6)
		{
		notifyMtlr.iLocationInfo.iLcsClientID.Copy(DMMTSY_INVALID_EXTERNAL_CLIENT_ID) ;
		iPhone->AddDelayedReq(aTsyReqHandle,this, 100000);	
		}
	return KErrNone;
	}

TInt CMobileLocationServicesDMmTsy::SendMolr(const TTsyReqHandle aTsyReqHandle,TDes8* aStartMolr,TDes8* aMolrReturnResult)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::SendMolr Called"));
	RMobileLocationServices::TStartMolrV7Pckg *startMolrPckg = (RMobileLocationServices::TStartMolrV7Pckg*)(aStartMolr);
	RMobileLocationServices::TStartMolrV7 &	startMolr = (*startMolrPckg)();
	RMobileLocationServices::TMolrReturnResultV7Pckg *molrReturnResultPckg = (RMobileLocationServices::TMolrReturnResultV7Pckg*)(aMolrReturnResult);
	RMobileLocationServices::TMolrReturnResultV7 &	molrReturnResult = (*molrReturnResultPckg)();
	if(startMolr.iMolrType==DMMTSY_MOLR_TYPE1 && startMolr.iLcsClientExternalID==DMMTSY_INVALID_EXTERNAL_CLIENT_ID)
		{
		molrReturnResult.iLocationEstimate=DMMTSY_MOLR_LOCATION_ESTIMATE;
		molrReturnResult.iMolrErrors=DMMTSY_MOLR_ERROR_TYPE2;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	else if(startMolr.iMolrType==DMMTSY_MOLR_TYPE1 ||
			startMolr.iMolrType==DMMTSY_MOLR_TYPE2 ||
			startMolr.iQos.iHorizontalAccuracy == DMMTSY_MOLR_HORIZONTAL_ACCURACY ||
			startMolr.iLcsClientExternalID==DMMTSY_EXTERNAL_CLIENT_ID ||
			startMolr.iMolrLocationEstimateType == DMMTSY_MOLR_LOCATION_ESTIMATION_TYPE1 ||
			startMolr.iLocationMethod == DMMTSY_MOLR_LOCATION_METHOD3 ||			
	    	startMolr.iQos.iVerticalAccuracy == DMMTSY_MOLR_VERTICAL_ACCURACY ||
			startMolr.iQos.iResponseTime == DMMTSY_MOLR_RESPONSE_TIME1 ||
			startMolr.iGpsAssistanceData==DMMTSY_MOLR_GPS_ASSISTANCE_DATA)
		{
		molrReturnResult.iLocationEstimate=DMMTSY_MOLR_LOCATION_ESTIMATE;
		molrReturnResult.iVelocityEstimate=DMMTSY_MOLR_VELOCITY_ESTIMATE;
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
		return KErrNone;
	}
	
TInt CMobileLocationServicesDMmTsy::NotifyMtlrCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::NotifyMtlrCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
TInt CMobileLocationServicesDMmTsy::SendMolrCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::SendMolrCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
TInt CMobileLocationServicesDMmTsy::SendMtlrResponseCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::SendMtlrResponseCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
TInt CMobileLocationServicesDMmTsy::NotifyMeasurementControl(const TTsyReqHandle aTsyReqHandle,TDes8* aMeasurementControl)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::NotifyMeasurementControl Called"));
	RMobileLocationServices::TMeasurementControlV7Pckg *measurementControlPckg = (RMobileLocationServices::TMeasurementControlV7Pckg*)(aMeasurementControl);
	RMobileLocationServices::TMeasurementControlV7 &	measurementControl = (*measurementControlPckg)();
	// modified measurement control data
	if(measurementControl.iMeasurementIdentity==DMMTSY_PHONE_LCS_MODIFIED_MEASUREMENTID)
		{
		measurementControl.iMeasurementIdentity=DMMTSY_PHONE_LCS_MODIFIED_MEASUREMENTID;
		
		//Reference Time Data populated
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iReferencTime.iGpsWeek = DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSWK;		
     	measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iReferencTime.iGpsTowOneMsec = DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSTOWONEMSCE;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iGpsAddlDataStatus.iReferenceTimeRequest = EFalse;	
		
		//	Acquisition Assistance Data populated
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iLsPart = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_LSPART;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iMsPart = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MSPART;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iSfn = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_SFN;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iGpsAddlDataStatus.iAcquisitionAssistanceReq = EFalse;
		
		// Reference Location data populated
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iReferenceLocation.iLatitudeSign = DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LATSIGN1;	
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iReferenceLocation.iLatitude = DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LAT;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iReferenceLocation.iLongitude = DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LONG;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iGpsAddlDataStatus.iReferenceLocationRequest = EFalse;

		//	NavigationModel data populated
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iNavigationModel.iEphemerisParameter.iCodeOnL2 = DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_EPHEMERIS ;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iNavigationModel.iSatID = DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATID;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iNavigationModel.iSatelliteStatus = DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS1;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iGpsAddlDataStatus.iNavigationModelRequest = EFalse;
		
		//DGPS corrections data populated
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iDgpsCorrections.iGpsTow = DMMTSY_PHONE_LCS_MC_GPS_CORRECTIONS_TOW;	
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iGpsAddlDataStatus.iDgpsCorrectionsRequest = EFalse;	
		
		//	Ionospheric data populated
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iIonosphericModel.iAlfa1 = 	DMMTSY_PHONE_LCS_MC_GPS_IONOSPHERIC_MODEL1;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iGpsAddlDataStatus.iIonosphericModelRequest = EFalse;	
		
		//UTC data populated
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iUtcModel.iA1 = DMMTSY_PHONE_LCS_MC_GPS_UTC_MODEL1;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iGpsAddlDataStatus.iUtcModelRequest = EFalse;
		
		//Almanac data populated
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iAlmanac.iWnA = DMMTSY_PHONE_LCS_MC_GPS_ALMANAC;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iGpsAddlDataStatus.iAlmanacRequest	= EFalse;

		//RealTime integrity data populated
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iBadSatList[0] = DMMTSY_PHONE_LCS_MC_GPS_BADSATLIST;
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iGpsAddlDataStatus.iRealTimeIntegrityRequest = EFalse;
		
		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iAcquisitionAssistance.iGpsReferenceTime = DMMTSY_PHONE_LCS_MC_GPS_ACQASSISTANCE_TIME;
		measurementControl.iMeasReportTransferMode = DMMTSY_PHONE_LCS_MC_RPTTRANSFERMODE;

		measurementControl.iMeasurementCommand.iModify.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsReferenceTime.iCellParametersID = DMMTSY_PHONE_LCS_MC_GPS_MODE_CELLPARAMID;
		measurementControl.iMeasurementCommand.iModify.iUePosReportingQuantity.iHorzAccuracy = DMMTSY_PHONE_LCS_MC_HORZACCURACY;
		measurementControl.iMeasurementCommand.iModify.iUePosReportingQuantity.iVertAccuracy = DMMTSY_PHONE_LCS_MC_VERTACCURACY;

		//Additional Data request is False
		measurementControl.iMeasurementCommand.iModify.iUePosReportingQuantity.iAddlAssistanceDataReq = EFalse;

		iPhone->AddDelayedReq(aTsyReqHandle,this, 1000000);
		}
	//incomplete measurement control
	else if(measurementControl.iMeasurementIdentity==DMMTSY_PHONE_LCS_MEASUREMENTID1)
		{
		measurementControl.iMeasurementIdentity=DMMTSY_PHONE_LCS_MEASUREMENTID1;

		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iGpsWeek = DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSWK;		
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iGpsTowOneMsec = DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSTOWONEMSCE;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iLsPart = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_LSPART;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iMsPart = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MSPART;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iSfn = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_SFN;

		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iDgpsCorrections.iGpsTow = DMMTSY_PHONE_LCS_MC_GPS_CORRECTIONS_TOW;	
		
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iEphemerisParameter.iCodeOnL2 = DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_EPHEMERIS ;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iSatID = DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATID;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iSatelliteStatus = DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS1;
		
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iIonosphericModel.iAlfa1 = 	DMMTSY_PHONE_LCS_MC_GPS_IONOSPHERIC_MODEL1;

		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAlmanac.iWnA = DMMTSY_PHONE_LCS_MC_GPS_ALMANAC;

		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iGpsReferenceTime = DMMTSY_PHONE_LCS_MC_GPS_ACQASSISTANCE_TIME;

		measurementControl.iMeasReportTransferMode = DMMTSY_PHONE_LCS_MC_RPTTRANSFERMODE;
		measurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iAddlAssistanceDataReq = ETrue;
		
		iPhone->AddDelayedReq(aTsyReqHandle,this, 1000000);		
		}
	// measurement control failure case
	else if(measurementControl.iMeasurementIdentity==DMMTSY_PHONE_LCS_INVALID_MEASUREMENTID)
		{
		measurementControl.iMeasurementIdentity=DMMTSY_PHONE_LCS_INVALID_MEASUREMENTID;
		iPhone->AddDelayedReq(aTsyReqHandle,this, 1000000);		
		}
	//complete measurement control data
	else
		{
		measurementControl.iMeasurementIdentity=DMMTSY_PHONE_LCS_MEASUREMENTID;
		
		//Reference Time Data populated
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iGpsWeek = DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSWK;		
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iGpsTowOneMsec = DMMTSY_PHONE_LCS_MC_GPS_REFTIME_GPSTOWONEMSCE;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iReferenceTimeRequest = EFalse;	
		
		//	Acquisition Assistance Data populated
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iLsPart = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_LSPART;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iMsPart = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MSPART;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iSfn = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_SFN;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iAcquisitionAssistanceReq = EFalse;
		
		// Reference Location data populated
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLatitudeSign = DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LATSIGN1;	
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLatitude = DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LAT;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferenceLocation.iLongitude = DMMTSY_PHONE_LCS_MC_GPS_REFLOC_LONG;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iReferenceLocationRequest = EFalse;

		//	NavigationModel data populated
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iEphemerisParameter.iCodeOnL2 = DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_EPHEMERIS ;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iSatID = DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATID;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iNavigationModel.iSatelliteStatus = DMMTSY_PHONE_LCS_MC_GPS_NAVIGATION_SATSTATUS1;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iNavigationModelRequest = EFalse;
		
		//DGPS corrections data populated
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iDgpsCorrections.iGpsTow = DMMTSY_PHONE_LCS_MC_GPS_CORRECTIONS_TOW;	
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iDgpsCorrectionsRequest = EFalse;	
		
		//	Ionospheric data populated
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iIonosphericModel.iAlfa1 = 	DMMTSY_PHONE_LCS_MC_GPS_IONOSPHERIC_MODEL1;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iIonosphericModelRequest = EFalse;	
		
		//UTC data populated
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iUtcModel.iA1 = DMMTSY_PHONE_LCS_MC_GPS_UTC_MODEL1;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iUtcModelRequest = EFalse;
		
		//Almanac data populated
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAlmanac.iWnA = DMMTSY_PHONE_LCS_MC_GPS_ALMANAC;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iAlmanacRequest	= EFalse;

		//RealTime integrity data populated
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iBadSatList[0] = DMMTSY_PHONE_LCS_MC_GPS_BADSATLIST;
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iGpsAddlDataStatus.iRealTimeIntegrityRequest = EFalse;
		
		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iGpsReferenceTime = DMMTSY_PHONE_LCS_MC_GPS_ACQASSISTANCE_TIME;
		measurementControl.iMeasReportTransferMode = DMMTSY_PHONE_LCS_MC_RPTTRANSFERMODE;

		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsReferenceTime.iCellParametersID = DMMTSY_PHONE_LCS_MC_GPS_MODE_CELLPARAMID;
		measurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iHorzAccuracy = DMMTSY_PHONE_LCS_MC_HORZACCURACY;
		measurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iVertAccuracy = DMMTSY_PHONE_LCS_MC_VERTACCURACY;

		measurementControl.iVelocityRequested = DMMTSY_PHONE_LCS_MC_VELOCITY_REQUESTED;

		//Additional Data request is False
		measurementControl.iMeasurementCommand.iSetup.iUePosReportingQuantity.iAddlAssistanceDataReq = EFalse;

		measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iModeSpecificInfoStatus = DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MODESTATUS_REQUESTED;
		
		if(measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iModeSpecificInfoStatus == DMMTSY_PHONE_LCS_MC_GPS_UTRANREFTIME_MODESTATUS_REQUESTED)
			{
			measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iModeSpecificInfo.iPrimaryScramblingCode =DMMTSY_PHONE_LCS_MC_GPS_MODE_PRIMARYCODE;
			}
		else
			{
			measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iReferencTime.iUtranGpsRefTime.iModeSpecificInfo.iCellParametersID = DMMTSY_PHONE_LCS_MC_GPS_MODE_CELLPARAMID;
			}
		
		if(measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsRefTimeStatus == DMMTSY_PHONE_LCS_MC_GPS_ACQASSISTANCE_REFTIMEST_REQUESTED)
			{
			measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsReferenceTime.iPrimaryScramblingCode = DMMTSY_PHONE_LCS_MC_GPS_MODE_PRIMARYCODE;
			}
		else
			{
			measurementControl.iMeasurementCommand.iSetup.iUePosGpsAssistanceData.iAcquisitionAssistance.iUtranGpsReferenceTime.iCellParametersID = DMMTSY_PHONE_LCS_MC_GPS_MODE_CELLPARAMID;
			}

		iPhone->AddDelayedReq(aTsyReqHandle,this, 1000000);
		}
	return KErrNone;
	}
	
TInt CMobileLocationServicesDMmTsy::SendMeasurementReport(const TTsyReqHandle aTsyReqHandle,TDes8* aMeasurementReport)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::SendMeasurementReport Called"));
	RMobileLocationServices::TMeasurementReportV7Pckg *measurementReportPckg = (RMobileLocationServices::TMeasurementReportV7Pckg*)(aMeasurementReport);
    RMobileLocationServices::TMeasurementReportV7 &	measurementReport = (*measurementReportPckg)();
	//successful measurement report
	if(measurementReport.iMeasurementIdentity==DMMTSY_PHONE_LCS_MEASUREMENTID && measurementReport.iMeasuredResults.iTUePosMeasuredResults.iPosErrorStatus == DMMTSY_PHONE_LCS_MCR_POSERROR_STATUS_FALSE)
		{
		iPhone->AddDelayedReq(aTsyReqHandle,this);		
		}
	//measurment report with modified data
	else if(measurementReport.iMeasurementIdentity==DMMTSY_PHONE_LCS_MODIFIED_MEASUREMENTID && measurementReport.iMeasuredResults.iTUePosMeasuredResults.iPosErrorStatus == DMMTSY_PHONE_LCS_MCR_POSERROR_STATUS_FALSE)
		{
		iPhone->AddDelayedReq(aTsyReqHandle,this);		
		}
	//unsuccessful measurement report
	else if(measurementReport.iMeasurementIdentity==DMMTSY_PHONE_LCS_MEASUREMENTID1 && 
			measurementReport.iMeasuredResults.iTUePosMeasuredResults.iPosErrorStatus == DMMTSY_PHONE_LCS_MCR_POSERROR_STATUS_TRUE)
		{
			if(	measurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iAlmanacRequest == DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_ALMANACREQ ||
				measurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iUtcModelRequest == DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_UTCREQ	||
				measurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iIonosphericModelRequest == DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_IONREQ 	||
				measurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iNavigationModelRequest == DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_NAVREQ 	||
				measurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iDgpsCorrectionsRequest == DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_DPSREQ 	||
				measurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iReferenceLocationRequest == DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_REFLOCREQ 	||
			 	measurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iReferenceTimeRequest == DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_REFTIMEREQ 	||
				measurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iAcquisitionAssistanceReq == DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_ACQASSITREQ 	||	
				measurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iGpsAddlAssistDataReq.iRealTimeIntegrityRequest == DMMTSY_PHONE_LCS_MC_GPS_ADDLASSITDATA_REALTIMEINTGREQ ||
			 	measurementReport.iMeasuredResults.iTUePosMeasuredResults.iUePosError.iPosErrorCause == DMMTSY_PHONE_LCS_MC_GPS_ERRORCAUSE5)
				{
				iPhone->AddDelayedReq(aTsyReqHandle,this);
				}
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);	
		}
	return KErrNone;
	}
	
TInt CMobileLocationServicesDMmTsy::SendMeasurementReport(const TTsyReqHandle aTsyReqHandle,RMobileLocationServices::TMeasurementControlFailure* aMeasurementControlFailure)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::SendMeasurementControlFailure Called"));
	if( *aMeasurementControlFailure == DMMTSY_PHONE_LCS_MCTRLFAILURE1 ||
		*aMeasurementControlFailure == DMMTSY_PHONE_LCS_MCTRLFAILURE2 ||
		*aMeasurementControlFailure == DMMTSY_PHONE_LCS_MCTRLFAILURE3 ||
		*aMeasurementControlFailure == DMMTSY_PHONE_LCS_MCTRLFAILURE4 ||
		*aMeasurementControlFailure == DMMTSY_PHONE_LCS_MCTRLFAILURE5 ||
		*aMeasurementControlFailure == DMMTSY_PHONE_LCS_MCTRLFAILURE6 )
		{
		iPhone->AddDelayedReq(aTsyReqHandle,this);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}

TInt CMobileLocationServicesDMmTsy::NotifyMeasurementControlCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::NotifyMeasurementControlCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}
	
TInt CMobileLocationServicesDMmTsy::SendMeasurementReportCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::SendMeasurementReportCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}

TInt CMobileLocationServicesDMmTsy::SetLcsDomain(const TTsyReqHandle aTsyReqHandle,RMobileLocationServices::TDomain* aDomain)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::SetLcsDomain is Called"));
	if (*aDomain==DMMTSY_SETLCS_DOMAIN1 || *aDomain==DMMTSY_SETLCS_DOMAIN2)
		{
		ReqCompleted(aTsyReqHandle,KErrNone);
		}
	else
		{
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
		}
	return KErrNone;
	}
	

TInt CMobileLocationServicesDMmTsy::NotifyResetUePositioningInformation(const TTsyReqHandle aTsyReqHandle, RMobileLocationServices::TUePosTechnology* aUePosTechnology)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::NotifyResetUePositioningInformation Called"));
	*aUePosTechnology = RMobileLocationServices::KUePosTechnologyAGPS; // 0
	iPhone->AddDelayedReq(aTsyReqHandle,this);
	return KErrNone;
	}

TInt CMobileLocationServicesDMmTsy::NotifyResetUePositioningInformationCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CMobileLocationServicesDMmTsy::NotifyResetUePositioningInformationCancel Called"));
	iPhone->RemoveDelayedReq(aTsyReqHandle);
	ReqCompleted(aTsyReqHandle,KErrCancel);
	return KErrNone;
	}





