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
// This file provides the class implementation that handles the
// network interface for the Test Protocol Module.
// 
//

#include <e32base.h>
#include <lbs/lbsprivacycontroller.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "lbsdevloggermacros.h"
#include "cnetworkinterface.h"


/** Static constructor.
@param aObserver Network interface observer
@return A new instance of the CNetworkInterface class
*/  
CNetworkInterface* CNetworkInterface::NewL(MNetworkObserver& aObserver)
	{
	LBSLOG(ELogP2, "CNetworkInterface::NewL");
	CNetworkInterface* self = new (ELeave) CNetworkInterface(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard constructor.
@param aObserver Network interface observer
*/  
CNetworkInterface::CNetworkInterface(MNetworkObserver& aObserver)
: iObserver(aObserver), iNetworkError(ENone), iInMOLRSession(EFalse)
	{
	LBSLOG(ELogP2, "CNetworkInterface::CNetworkInterface");
	}


/** Standard destructor.
*/  
CNetworkInterface::~CNetworkInterface()
	{
	LBSLOG(ELogP2, "CNetworkInterface::~CNetworkInterface");
	iNetwork.Close();
	}


/** Private second-stage constructor.
*/  
void CNetworkInterface::ConstructL()
	{
	LBSLOG(ELogP2, "CNetworkInterface::ConstructL");
	// Retrieve LBS capabilities
	GetCapabilities(iCapabilities);
	}


/** Connect
*/  
void CNetworkInterface::Connect()
	{
	LBSLOG(ELogP2, "CNetworkInterface::Connect");
	// Attempt to connect
	TInt error = KErrNone;
	TRAPD(leaveVal, error = iNetwork.ConnectL(this));

	// Connection error is stored
	if ((KErrNone != leaveVal) || (KErrNone != error))
		{
		LBSLOG(ELogP2, "CNetworkInterface::Connect() Failed to connect to network");
		iNetworkError = EConnection;
		}
	else
		{
		iNetworkError = ENone;
		}
	}


/** Check network connection and reconnect if required
Indicate if the network is connected.
@return TNetworkError Network error indication.
*/  
CNetworkInterface::TNetworkError CNetworkInterface::CheckConnection()
	{
	LBSLOG2(ELogP2, "CNetworkInterface::CheckConnection() would return error %d", iNetworkError);
	
	iNetworkError = ENone;
	return iNetworkError; 
	}


/** Request MO-LR
Send an MO-LR request to the network
@param aDestination The destination for the subsequent location position.
*/  
CNetworkInterface::TNetworkError CNetworkInterface::MoLrReq(const TDesC& aDestination)
	{
	LBSLOG(ELogP2, "CNetworkInterface::MoLrReq()");
	// Connected?
	if (ENone == CheckConnection())
		{
		TRAPD(errorVal, iNetwork.RegisterLcsMoLrL(aDestination));
		// Leave error is significant and is treated as a connection error
		if (KErrNone != errorVal)
			{
			LBSLOG(ELogP2, "CNetworkInterface::MoLrReq() Failed to send to network");
			iNetworkError = EConnection;
			}
		else
			{
			LBSLOG(ELogP2, "CNetworkInterface::MoLrReq() In MOLR Session");
			iInMOLRSession = ETrue;
			}
		}

	return iNetworkError;
	}


/** MO-LR Complete Indication
Send MO-LR completion to the network.
@param aReason The MO-LR completion reason
*/
CNetworkInterface::TNetworkError CNetworkInterface::MoLrCompleteInd(const TInt& aReason)
	{
	LBSLOG(ELogP2, "CNetworkInterface::MoLrCompleteInd");
	LBSLOG2(ELogP2, "Reason : %d\n", aReason);	
	// Connected?
	if (ENone == CheckConnection() && iInMOLRSession)
		{
		TRAPD(errorVal, iNetwork.ReleaseLcsMoLrL(aReason));
		// Leave error is significant and is treated as a connection error
		if (KErrNone != errorVal)
			{
			LBSLOG(ELogP2, "CNetworkInterface::MoLrCompleteInd() Failed to send to network");
			iNetworkError = EConnection;
			}
		else
			{
			LBSLOG(ELogP2, "CNetworkInterface::MoLrCompleteInd() MOLR Session Closed");
			iInMOLRSession = EFalse;
			}
		}
	else
		{
		LBSLOG(ELogP2, "CNetworkInterface::MoLrCompleteInd() MOLR Session ALREADY Closed");
		}

	return iNetworkError;
	}


/** Measurement control request
Send assistance data request to the network.
@param aAssistData A mask identifying the set of data requested.
*/
CNetworkInterface::TNetworkError CNetworkInterface::MeasurementControlReq(const TLbsAsistanceDataGroup& aAssistData)
	{
	LBSLOG(ELogP2, "CNetworkInterface::MeasurementControlReq");
	// Connected?
	if (ENone == CheckConnection())
		{
		TRAPD(errorVal, iNetwork.MeasurementReportLocationRequestMoreAssistanceDataL(aAssistData));
		// Leave error is significant and is treated as a connection error
		if (KErrNone != errorVal)
			{
			LBSLOG(ELogP2, "CNetworkInterface::MeasurementControlReq() Failed to send to network");
			iNetworkError = EConnection;
			}
		}

	return iNetworkError;
	}


/** Location Response
Send the location position response to the network.
@param aPosInfo Location information.
*/
CNetworkInterface::TNetworkError CNetworkInterface::LocationResp(const TPositionInfoBase& aPosInfo)
	{
	LBSLOG(ELogP2, "CNetworkInterface::LocationResp");
	// Connected?
	if (ENone == CheckConnection())
		{
		TRAPD(errorVal, iNetwork.MeasurementReportLocationL(aPosInfo));
		// Leave error is significant and is treated as a connection error
		if (KErrNone != errorVal)
			{
			LBSLOG(ELogP2, "CNetworkInterface::LocationResp() Failed to send to network");
			iNetworkError = EConnection;
			}
		}

	return iNetworkError;
	}


/** Location Response error
Send the location error response to the network.
@param aReason Location error reason.
*/
CNetworkInterface::TNetworkError CNetworkInterface::LocationResp(TInt aReason)
	{
	LBSLOG(ELogP2, "CNetworkInterface::LocationResp");
	// Connected?
	if (ENone == CheckConnection())
		{
		TRAPD(errorVal, iNetwork.MeasurementControlFailureL(aReason));
		// Leave error is significant and is treated as a connection error
		if (KErrNone != errorVal)
			{
			LBSLOG(ELogP2, "CNetworkInterface::LocationResp() Failed to send to network");
			iNetworkError = EConnection;
			}
		}

	return iNetworkError;
	}


/** MT-LR response
Send the privacy response to the network.
@param aResponse Privacy response value
*/
CNetworkInterface::TNetworkError CNetworkInterface::MtLrResp(
			const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse)
	{
	LBSLOG(ELogP2, "CNetworkInterface::MtLrResp");
	// Connected?
	if (ENone == CheckConnection())
		{
		TRAPD(errorVal, iNetwork.ReleaseLcsLocationNotificationL(aResponse));
		// Leave error is significant and is treated as a connection error
		if (KErrNone != errorVal)
			{
			LBSLOG(ELogP2, "CNetworkInterface::MtLrResp() Failed to send to network");
			iNetworkError = EConnection;
			}
		}

	return iNetworkError;
	}


/** Measurement control indication
@see MLbsNetSimObserver
*/
void CNetworkInterface::ProcessMeasurementControlLocation(const TPositionInfoBase& aPosInfo,
		const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality)
	{
	LBSLOG(ELogP2, "CNetworkInterface::ProcessMeasurementControlLocation");
#pragma message("Receive TLbsNetPosRequestMethod from NetSim")
	TLbsNetPosMethod posMethod[1];
	iCapabilities.GetPosMethod(0, posMethod[0]);
	// GetCapabilities has not been implemented, so need to set this to a useful default value (Terminal Based):
	if(!posMethod[0].PosMode())
		{
		posMethod[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
		}
	TLbsNetPosRequestMethod reqMethod;
	reqMethod.SetPosMethods(posMethod, 1);

	iObserver.MeasurementControlInd(aPosInfo, aData, aQuality, reqMethod);
	}


/** Measurement control error indication
@see MLbsNetSimObserver
*/
void CNetworkInterface::ProcessMeasurementControlLocationError(TInt aReason)
	{
	LBSLOG(ELogP2, "CNetworkInterface::ProcessMeasurementControlLocationError");
	LBSLOG2(ELogP2, "Reason : %d\n", aReason);	
	iObserver.MeasurementControlErrorInd(aReason);
	}


/** MO-LR Result indication
@see MLbsNetSimObserver
*/
void CNetworkInterface::ProcessFacilityLcsMoLrResult(TInt aResult, const TPositionInfoBase* aPosInfo)
	{
	LBSLOG(ELogP2, "CNetworkInterface::ProcessFacilityLcsMoLrResult");
	LBSLOG2(ELogP2, "Result : %d\n", aResult);	
	iObserver.NetworkResultInd(aResult, aPosInfo);
	}


/** MT-LR Register indication
@see MLbsNetSimObserver
*/
void CNetworkInterface::ProcessRegisterLcsLocationNotification(const TLbsExternalRequestInfo& reqInfo,
		const TLbsNetPosRequestPrivacy& aPrivacy)
	{
	LBSLOG(ELogP2, "CNetworkInterface::ProcessRegisterLcsLocationNotification");
	iObserver.MtLrReq(reqInfo, aPrivacy);
	}


/** MT-LR Release indication
@see MLbsNetSimObserver
*/
void CNetworkInterface::ProcessReleaseLcsLocationNotification(TInt aReason)
	{
	LBSLOG(ELogP2, "CNetworkInterface::ProcessReleaseLcsLocationNotification");
	LBSLOG2(ELogP2, "Reason : %d\n", aReason);	
	iObserver.MtLrCancelInd(aReason);
	}
/** Request to reset the assistance data held by the device
@see MLbsNetSimObserver
 */
void CNetworkInterface::ProcessResetAssistanceData(TLbsAssistanceDataGroup aMask)
	{
	LBSLOG(ELogP2, "CNetworkInterface::ProcessResetAssistanceData");
	LBSLOG2(ELogP2, "Mask : 0x%x\n", aMask);
	iObserver.ResetAssistanceData(aMask);	
	}

/** Network related error indication
@see MLbsNetSimObserver
*/
void CNetworkInterface::ProcessError(TInt aReason)
	{
	LBSLOG(ELogP2, "CNetworkInterface::ProcessError");
	LBSLOG2(ELogP2, "Reason : %d\n", aReason);	
	// Set error indication for next time we attempt to access network
	iNetworkError = EConnection;
	iObserver.NetworkErrorInd(aReason);
	iInMOLRSession = EFalse; 
	}


/** Network request for LBS capabilities
@see MLbsNetSimObserver
*/
void CNetworkInterface::GetCapabilities(TLbsNetPosCapabilities& aCapabilities) const
	{
	LBSLOG(ELogP2, "CNetworkInterface::GetCapabilities");
	iObserver.GetCapabilities(aCapabilities);
	}
