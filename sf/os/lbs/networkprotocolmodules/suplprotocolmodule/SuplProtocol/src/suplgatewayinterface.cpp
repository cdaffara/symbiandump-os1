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
// This file provides the class implementation of the ECOM plug-in 
// interface for the SUPL Protocol Module.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#include <ecom/implementationproxy.h>
#include "suplgatewayinterface.h"
#include "supldevloggermacros.h"


/** The unique ID of this plug-in implementation.
This corresponds to the implementation UID specified in the .rss file
for this protocol module.
*/
const TInt KPluginUidValue = 0x10285A9D;


//=============================================================================
// CSuplGatewayInterface
//=============================================================================

/** Static public constructor.
@param aObserver Plug-in parameters, which includes a reference 
to this protocol module's observer.
@return A new instance of the CSuplGatewayInterface class.
*/
CSuplGatewayInterface* CSuplGatewayInterface::NewL(TAny* aParam)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::NewL() Begin\n");
	TLbsNetProtocolModuleParams* param = reinterpret_cast<TLbsNetProtocolModuleParams*>(aParam);
	CSuplGatewayInterface* self = new (ELeave) CSuplGatewayInterface(static_cast<MLbsNetworkProtocolObserver2&>(param->Observer()));
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::NewL() End\n");
	return self;
	}


/** Standard destructor.
*/  
CSuplGatewayInterface::~CSuplGatewayInterface()
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::~CSuplGatewayInterface() Begin\n");
	delete iManager;
	SUPLLOG(ELogP1, "CSuplGatewayInterface::~CSuplGatewayInterface() End\n");
	}


/** Default constructor
@param aObserver Reference to this protocol module's observer.
*/
CSuplGatewayInterface::CSuplGatewayInterface(MLbsNetworkProtocolObserver2& aObserver)
: iObserver(aObserver)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::CSuplGatewayInterface() Begin\n");
	SUPLLOG(ELogP1, "CSuplGatewayInterface::CSuplGatewayInterface() End\n");
	}


/** Second stage private constructor.
*/
void CSuplGatewayInterface::ConstructL()
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::ConstructL() Begin\n");
	iManager = CSuplProtocolManager::NewL(*this);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::ConstructL() End\n");
	}


/** Send a response to a privacy request to the network.
@see CLbsNetworkProtocolBase::RespondPrivacyRequest()
*/
void CSuplGatewayInterface::RespondPrivacyRequest(const TLbsNetSessionId& aSessionId, const TLbsPrivacyResponse& aResponse, TInt aReason)
	{	
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RespondPrivacyRequest() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::RespondPrivacyRequest\n");
	iManager->PrivacyResp(aSessionId, aResponse, aReason);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RespondPrivacyRequest() End\n");
	}

/** Send a response to a location request to the network.
@see CLbsNetworkProtocolBase::RespondLocationRequest()
*/
void CSuplGatewayInterface::RespondLocationRequest(const TLbsNetSessionId& aSessionId, TInt aReason, 
												const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RespondLocationRequest() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::RespondLocationRequest\n");
	iManager->LocationResp(aSessionId, aReason, aPosInfo);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RespondLocationRequest() End\n");
	}

/** Send a request for GPS assistance data to the network.
LBS does not currently send valid infomation in the sessionIdArray
@see CLbsNetworkProtocolBase::RequestAssistanceData()
*/
void CSuplGatewayInterface::RequestAssistanceData(TLbsAsistanceDataGroup aDataRequestMask,
												const TLbsNetSessionIdArray& /*aSessionIdArray*/)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RequestAssistanceData() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::RequestAssistanceData\n");
	iManager->AssistanceDataReq(aDataRequestMask);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RequestAssistanceData() End\n");
	}

/** Send a request for an approximate location based on the current network cell to the network.
@see CLbsNetworkProtocolBase::RequestNetworkLocation()
*/
void CSuplGatewayInterface::RequestNetworkLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions)
	{	
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RequestNetworkLocation() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::RequestNetworkLocation\n");
	iManager->NetworkBasedLocationReq(aSessionId, aOptions);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RequestNetworkLocation() End\n");
	}


/** Tell the protocol module to finish the current network based location request.
@see CLbsNetworkProtocolBase::CancelNetworkLocation()
*/
void CSuplGatewayInterface::CancelNetworkLocation(const TLbsNetSessionId& aSessionId,
							TInt aReason)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::CancelNetworkLocation() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::CancelNetworkLocation\n");
	iManager->NetworkBasedLocationCompleteInd(aSessionId, aReason);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::CancelNetworkLocation() End\n");
	}


/** Request self locate.
@see CLbsNetworkProtocolBase::RequestSelfLocation()
*/
void CSuplGatewayInterface::RequestSelfLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions)	
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RequestSelfLocation() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::RequestSelfLocation\n");
	iManager->SelfLocationReq(aSessionId, aOptions);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RequestSelfLocation() End\n");
	}

/** Cancel request for self locate.
@see CLbsNetworkProtocolBase::CancelSelfLocation()
*/
void CSuplGatewayInterface::CancelSelfLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::CancelSelfLocation() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::CancelSelfLocation\n");
	iManager->SelfLocationCompleteInd(aSessionId, aReason);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::CancelSelfLocation() End\n");
	}

/** Lbs System Status information.
@see CLbsNetworkProtocolBase::AdviceSystemStatus()
*/
void CSuplGatewayInterface::AdviceSystemStatus(TLbsSystemStatus aStatus)
	{	
	SUPLLOG(ELogP1, "CSuplGatewayInterface::AdviceSystemStatus() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::AdviceSystemStatus\n");
	iManager->SystemStatusInd(aStatus);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::AdviceSystemStatus() End\n");
	}

/** Send a request to send the handsets location to a third party to the network.
This procedure is not supported by SUPL version 1. This method forwards the request
to the handler of unsupported procedures.
@see CLbsNetworkProtocolBase::RequestTransmitLocation()
*/
void CSuplGatewayInterface::RequestTransmitLocation(
									const TLbsNetSessionId& aSessionId,
									const TDesC& aDestination,
									TInt aPriority,
									const TLbsNetPosRequestOptionsBase& aOptions)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RequestTransmitLocation() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::RequestTransmitLocation\n");
	iManager->TransmitLocationReq(aSessionId, aDestination, aPriority, aOptions);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::RequestTransmitLocation() End\n");
	}

/** Tell the protocol module to end the current send location to third party request.
This procedure is not supported by SUPL version 1. This method forwards the request
to the handler of unsupported procedures.
@see CLbsNetworkProtocolBase::CancelTransmitLocation()
*/
void CSuplGatewayInterface::CancelTransmitLocation(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::CancelTransmitLocation() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::CancelTransmitLocation\n");
	iManager->TransmitLocationCompleteInd(aSessionId, aReason);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::CancelTransmitLocation() End\n");
	}


/** Send privacy request to protocol module's observer.
@param aSessionId The Id of the location session within the LBS sub-system.
@param aEmergency ETrue if this is an emergency privacy request, EFalse otherwise.
@param aPrivacy Type of privacy request specified by the network.
@param aRequestInfo Data about the external requestor of this handsets location.

@see MLbsNetworkProtocolObserver2::ProcessPrivacyRequest()
*/
void CSuplGatewayInterface::PrivacyReq(const TLbsNetSessionId& aSessionId, TBool aEmergency, 
					const TLbsNetPosRequestPrivacy& aPrivacy,
					const TLbsExternalRequestInfo& aRequestInfo)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::PrivacyReq() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::PrivacyReq\n");
	iObserver.ProcessPrivacyRequest(aSessionId, aEmergency, aPrivacy, aRequestInfo);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::PrivacyReq() End\n");
	}


/** Send network-based location to protocol module's observer.
@param aReferenceLocation The network-based reference location.

@see MLbsNetworkProtocolObserver2::ProcessNetworkBasedLocation() 
*/
void CSuplGatewayInterface::NetworkLocationInd(const TLbsNetSessionId& aSessionId, 
					const TPositionInfoBase& aPosInfo)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::NetworkLocationInd() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::NetworkLocationInd\n");
	iObserver.ProcessLocationUpdate(aSessionId, aPosInfo);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::NetworkLocationInd() End\n");
	}


/** Send assistance data to protocol module's observer.
@param aGroupMask This identifies the data group relevant for this indication.
@param aData This holds GPS assistance data to be delivered to LBS.
@param aReason A general error associated with assistance data retrieval.

@see MLbsNetworkProtocolObserver2::ProcessAssistanceData() 
*/
void CSuplGatewayInterface::AssistanceDataInd(const TLbsAsistanceDataGroup& aGroupMask,
						const RLbsAssistanceDataBuilderSet& aData, TInt aReason)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::AssistanceDataInd() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::AssistanceDataInd\n");
	// LBS does not yet support arrays of session IDs in the assistance data
	// notification.
	TLbsNetSessionIdArray dummySessIdArray;
	iObserver.ProcessAssistanceData(aGroupMask, aData, aReason, dummySessIdArray);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::AssistanceDataInd() End\n");
	}


/** Send location request to protocol module's observer.
@param aSessionId The Id of the location session within the LBS sub-system.
@param aEmergency ETrue if this is an emergency request, EFalse otherwise.
@param aType The type of location request. The type is determined by preceding
			 requests and other context information that only the network
			 protocol module knows about. So it is the responsibility
			 of the network protocol module to set the type of each request.
@param aQuality The network-requested quality of service (QoS) of the 
				location calculation.

@see MLbsNetworkProtocolObserver2::ProcessLocationRequest() 
*/
void CSuplGatewayInterface::LocationReq(const TLbsNetSessionId& aSessionId,
					const MLbsNetworkProtocolObserver2::TLbsNetProtocolService& aType, 
					const TLbsNetPosRequestQuality& aQuality,
					const TLbsNetPosRequestMethod& aPosMethod)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::LocationReq() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::LocationReq\n");
	iObserver.ProcessLocationRequest(aSessionId, EFalse, aType, aQuality, aPosMethod);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::LocationReq() End\n");
	}


/** Send session complete indication to protocol module's observer.
@param aSessionId The Id of the session completed.
@param aReason Reason for the completion of the request. 
			KErrNone if the request is successfully completed, otherwise
			it is one of a range of error codes.

@see MLbsNetworkProtocolObserver2::ProcessSessionComplete() 
*/
void CSuplGatewayInterface::SessionCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::SessionCompleteInd() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::SessionCompleteInd\n");
	iObserver.ProcessSessionComplete(aSessionId, aReason);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::SessionCompleteInd() End\n");
	}


/** Network request for LBS capabilities
@param aCapabilities The object to hold the returned capabilities from LBS.

@see MLbsNetworkProtocolObserver2::GetCapabilities() 
*/
void CSuplGatewayInterface::GetCapabilities(TLbsNetPosCapabilities& aCapabilities)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::GetCapabilities() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::GetCapabilities\n");
	iObserver.GetCurrentCapabilities(aCapabilities);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::GetCapabilities() End\n");
	}


/** Advise LBS of a change in active service status
@param aActiveServiceMask A mask specifying the currently active services.

@see MLbsNetworkProtocolObserver2::StatusUpdate() 
*/
void CSuplGatewayInterface::StatusUpdate(MLbsNetworkProtocolObserver2::TLbsNetProtocolServiceMask aActiveServiceMask)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::StatusUpdate() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::StatusUpdate\n");
	iObserver.ProcessStatusUpdate(aActiveServiceMask);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::StatusUpdate() End\n");
	}


/** This method for cancelling an MTLR is currently not used by LBS
Its effect in the MTLR state machine is the same as a location update
with KErrCancel, hence the implementation.
*/
void CSuplGatewayInterface::CancelExternalLocation(const TLbsNetSessionId& aSessionId,
								TInt /*aReason*/)
	{
	SUPLLOG(ELogP1, "CSuplGatewayInterface::CancelExternalLocation() Begin\n");
	SUPLLOG(ELogP7, "CSuplGatewayInterface::CancelExternalLocation\n");
	TPositionInfo aDummyPosition;
	iManager->LocationResp(aSessionId, KErrCancel, aDummyPosition);
	SUPLLOG(ELogP1, "CSuplGatewayInterface::CancelExternalLocation() End\n");
	}

/** Defines the plug-in interface implementation UIDs, required by ECOM.
*/
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KPluginUidValue, CSuplGatewayInterface::NewL)
    };


/** Exported function for ECOM to retrieve implementations table
*/
EXPORT_C 
const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
	SUPLLOG(ELogP1, "CSuplGatewayInterface::StatusUpdate() Begin\n");
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    SUPLLOG(ELogP1, "CSuplGatewayInterface::StatusUpdate() End\n");
    return ImplementationTable;
    }

