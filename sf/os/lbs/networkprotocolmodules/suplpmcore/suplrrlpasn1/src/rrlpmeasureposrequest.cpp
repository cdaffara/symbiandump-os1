// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
 
*/

#include "RRLP-Messages.h"
#include "rrlpmeasureposrequest.h"
#include "supldevloggermacros.h" 
#include "suplrrlpasn1common.h"
#include <math.h>

/**
Static factory constructor
*/
EXPORT_C CRrlpMeasurePositionRequest* CRrlpMeasurePositionRequest::NewL()
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::NewL() Begin\n");
	CRrlpMeasurePositionRequest* self = new (ELeave) CRrlpMeasurePositionRequest();
	CleanupStack::PushL(self);
	self->ConstructL();
	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::NewL() End\n");
	CleanupStack::Pop(self);
	return self;
	}

/** 
Default constructor 
*/
CRrlpMeasurePositionRequest::CRrlpMeasurePositionRequest()
 : CRrlpMessageBase(ERrlpMeasurePositionReq, EFalse)
 	{
 	}

/** 
Second stage constructor 
*/
void CRrlpMeasurePositionRequest::ConstructL()
	{
	}
		
/**
Destructor
*/
CRrlpMeasurePositionRequest::~CRrlpMeasurePositionRequest()
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::~CRrlpMeasurePositionRequest() Begin\n");
	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::~CRrlpMeasurePositionRequest() End\n");
	}

/** 
PositionInstruct()

Populates the Requested Positioning Methods and Quality according to the 
Positioning Instructions component of the Measure Position Request message.

Note that the field Multiple Sets is ignored (indicates if multiple 
information sets may be sent if the device supports this GaNSS feature.

Note also that Environment Characterization component is also ignored. This
relates to expected multipath and NLOS in the current area.

@param aQuality QoP, populated as per received PositionInstruction parameters
@param aMethod  Request method, populated according to received PositionInstruction parameters
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CRrlpMeasurePositionRequest::GetPositionInstruct(TLbsNetPosRequestQuality& aQuality, TLbsNetPosRequestMethod&  aMethod)
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::PositionInstruct() Begin\n");
	__ASSERT_DEBUG(iData->component.u.msrPositionReq != NULL, User::Invariant());
	
	ASN1T_MsrPosition_Req* msgBody = iData->component.u.msrPositionReq;

	TInt index = 0;
	TInt uncertainty = 0;
	TBool uncertaintySpecified = ETrue;
	TLbsNetPosMethod posMethods[4];
	
	// populate aMethod according to requested positioning methods
	TBool gpsSupported = (msgBody->positionInstruct.positionMethod != PositionMethod::eotd);
	TBool eotdSupported = (msgBody->positionInstruct.positionMethod != PositionMethod::gps);

	// build an array of requested pos methods, and set the uncertainty value
	switch (msgBody->positionInstruct.methodType.t)
		{
		case T_MethodType_msAssisted:
			if (gpsSupported)
				{
				posMethods[index].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
				++index;
				}
			if (eotdSupported)
				{
				posMethods[index].SetPosMethod(KLbsPositioningMeansEotd, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
				++index;
				}
			//Accuracy is an optional parameter in the case of TA 
			if (msgBody->positionInstruct.methodType.u.msAssisted->m.accuracyPresent)
				{
				uncertainty = msgBody->positionInstruct.methodType.u.msAssisted->accuracy;
				}
			else
			    {
			     //if accuracy is not being specified
			    uncertaintySpecified = EFalse;
			    }
			break;

		case T_MethodType_msBased:
			if (gpsSupported)
				{
				posMethods[index].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
				++index;
				}
			if (eotdSupported)
				{
				posMethods[index].SetPosMethod(KLbsPositioningMeansEotd, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
				++index;
				}
			
			uncertainty = msgBody->positionInstruct.methodType.u.msBased;
			break;

		case T_MethodType_msBasedPref:
			if (gpsSupported)
				{
				posMethods[index].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
				++index;
				posMethods[index].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
				++index;
				}
			if (eotdSupported)
				{
				posMethods[index].SetPosMethod(KLbsPositioningMeansEotd, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
				++index;
				posMethods[index].SetPosMethod(KLbsPositioningMeansEotd, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
				++index;
				}
			uncertainty = msgBody->positionInstruct.methodType.u.msBasedPref;
			break;

		case T_MethodType_msAssistedPref:
			if (gpsSupported)
				{
				posMethods[index].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
				++index;
				posMethods[index].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);		
				++index;
				}
			if (eotdSupported)
				{
				posMethods[index].SetPosMethod(KLbsPositioningMeansEotd, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
				++index;
				posMethods[index].SetPosMethod(KLbsPositioningMeansEotd, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);		
				++index;
				}
			uncertainty = msgBody->positionInstruct.methodType.u.msAssistedPref;
			break;

		default:
			__ASSERT_DEBUG(0, User::Invariant());
			return KErrCorrupt;
		}
	
	// populate the return pos method array
	// index now indicates number of pos methods in the array
	TInt err = aMethod.SetPosMethods(posMethods, index);
	if (KErrNone != err)
		{
		return err;
		}
	
	// set the uncertainty values
	if (uncertaintySpecified)
	    {
	    TReal32 uncertMeters = UncertaintyToMeters(uncertainty);
	    aQuality.SetMinHorizontalAccuracy(uncertMeters);
	    aQuality.SetMinVerticalAccuracy(uncertMeters);
	    }
	else
	    {
        // Set accuracy to NaN to prompt LBS to use accuracies from its pre-configured
        // profiles if available.
	    TRealX nan;
	    nan.SetNaN();
        aQuality.SetMinHorizontalAccuracy(nan);
        aQuality.SetMinVerticalAccuracy(nan);
	    }
    
	// set the maximum fix time
	TInt fixTimeSec;
	fixTimeSec = 1 << msgBody->positionInstruct.measureResponseTime;
	TTimeIntervalMicroSeconds timeMs(fixTimeSec * KLbsMilliSeconds);
	aQuality.SetMaxFixTime(timeMs);
	
	// indicate if GPS Timing Of Cell Frames data has been requested
	//   see ASN1T_Rel98_MsrPosition_Req_Extension
	if (msgBody->m.rel98_MsrPosition_Req_extensionPresent != 0
	   && msgBody->rel98_MsrPosition_Req_extension.m.gpsTimeAssistanceMeasurementRequestPresent != 0)
	    {
	    aMethod.SetGpsTimingOfCellFramesRequested(ETrue);
	    }
	
	// no RRLP component to populate the max fix age parameter.

	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::PositionInstruct() End\n");
	return KErrNone;
	}

/**
UncertaintyToMeters()

Converts from uncertainty code to meters, as defined in 3GPP TS 23.032:
uncertainty r = C( (1+x)^k - 1 )

r = uncertainty in meters
C = 10;
x = 0.1
k = uncertainty code

@param  aUncertainty uncertainty value k, as per the above formula
@return TReal32 uncertainty in meters
*/
TReal32 CRrlpMeasurePositionRequest::UncertaintyToMeters(TInt aUncertainty)
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::UncertaintyToMeters() Begin\n");
	TReal64 r;
	TReal64 xPow;
	Math::Pow(xPow, 1.1, aUncertainty);
	r = (xPow-1)*10;

	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::UncertaintyToMeters() End\n");
	return (TReal32)r;
	}
	
/** 
AssistanceDataPresent()

Indicates whether supported assistance data is present in the received message.
This implementation supports only receipt of GPS assistance data, hence this
method returns ETrue if GPS assistance data is present in the received content.

@return ETrue if valid assistance data is present in the received message 
*/
EXPORT_C TBool CRrlpMeasurePositionRequest::AssistanceDataPresent()
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::AssistanceDataPresent() Begin\n");
	__ASSERT_DEBUG(iData != NULL, User::Invariant());
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->component.u.msrPositionReq != NULL, User::Invariant());
		
	if (iData->component.u.msrPositionReq->m.gps_AssistDataPresent == 0)
		{
		SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::AssistanceDataPresent(EFalse) End\n");
		return EFalse;
		}
	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::AssistanceDataPresent(ETrue) End\n");
	return ETrue;
	}
	
/** 
BuildAssistanceData()

@param  aDataMask populated to indicate what assistance data exists
@param  aData     reference to assistance data builder object to populate with
                  received assistance data
@return error indication, KErrNone otherwise
*/
EXPORT_C TInt CRrlpMeasurePositionRequest::BuildAssistanceData(TLbsAsistanceDataGroup& aDataMask, RLbsAssistanceDataBuilderSet& aAssistanceDataBuilderSet)
	{
	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::BuildAssistanceData() Begin\n");
	__ASSERT_DEBUG(iData != NULL, User::Invariant());
	__ASSERT_DEBUG(!iIsOutgoingMessage, User::Invariant());
	__ASSERT_DEBUG(iData->component.u.msrPositionReq != NULL, User::Invariant());

	
	// return immediately if no gps assistance data present
	if (iData->component.u.msrPositionReq->m.gps_AssistDataPresent == 0)
		{
		SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::BuildAssistanceData() End (no GPS data present)\n");
		return KErrNone;
		}

	// pointer to the GPS Assistance Data component
	ASN1T_ControlHeader& gpsData = iData->component.u.msrPositionReq->gps_AssistData.controlHeader;

	SUPLLOG(ELogP1, "CRrlpMeasurePositionRequest::BuildAssistanceData() End\n");
	return CRrlpMessageBase::BuildAssistanceData(aDataMask, aAssistanceDataBuilderSet, gpsData);
	}


/**
GetExtendedReference()

Populates aRrlpRef if Rel-5 Extended Reference is present in the received message

@param  aRrlpRef local copy of the session reference details
@return KErrNotFound if the extended reference parameters are not present,
        KErrNone otherwise
*/
TInt CRrlpMeasurePositionRequest::GetExtendedReference(TRrlpReference& aRrlpRef)
	{
	__ASSERT_DEBUG(iData->component.u.msrPositionReq != NULL, User::Invariant());
	
	if (iData->component.u.msrPositionReq->m.rel5_MsrPosition_Req_extensionPresent != 0)
		{
		aRrlpRef.aRel5EntendedRefPresent = ETrue;
		aRrlpRef.aRel5SmlcCode = iData->component.u.msrPositionReq->rel5_MsrPosition_Req_extension.extended_reference.smlc_code;
		aRrlpRef.aRel5TransactionId = iData->component.u.msrPositionReq->rel5_MsrPosition_Req_extension.extended_reference.transaction_ID;
		}
	else	
		{
		return KErrNotFound;
		}
	return KErrNone;
	}
