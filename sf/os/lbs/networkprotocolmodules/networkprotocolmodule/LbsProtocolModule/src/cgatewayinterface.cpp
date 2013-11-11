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
// This file provides the class implementation of the ECOM plug-in 
// interface for the LBS test Network Protocol Module.
// 
//

#include <ecom/implementationproxy.h>
#include "lbsdevloggermacros.h"
#include "cgatewayinterface.h"

#ifdef ENABLE_LBS_DEV_LOGGER
#include <lbs/lbsnetclasstypes.h>
#endif


/** The unique ID of this plug-in implementation.
This corresponds to the implementation UID specified in the .rss file
for this protocol module.
*/
const TInt KPluginUidValue = 0x10281D70;


//=============================================================================
// CGatewayInterface
//=============================================================================

/** Static public constructor.
@param aObserver Plug-in parameters, which includes a reference 
to this protocol module's observer.
@return A new instance of the CGatewayInterface class.
*/  
CGatewayInterface* CGatewayInterface::NewL(TAny* aParam)
	{
	LBSLOG(ELogP2, "CGatewayInterface::NewL");

	TLbsNetProtocolModuleParams* param = reinterpret_cast<TLbsNetProtocolModuleParams*>(aParam);
	CGatewayInterface* self = new (ELeave) CGatewayInterface(param->Observer());
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


/** Standard destructor.
*/  
CGatewayInterface::~CGatewayInterface()
	{
	LBSLOG(ELogP2, "CGatewayInterface::~CGatewayInterface");
	delete iManager;
	}


/** Default constructor
@param aObserver Reference to this protocol module's observer.
*/
CGatewayInterface::CGatewayInterface(MLbsNetworkProtocolObserver& aObserver)
: iObserver(aObserver)
	{
	LBSLOG(ELogP2, "CGatewayInterface::CGatewayInterface");
	}


/** Second stage private constructor.
*/  
void CGatewayInterface::ConstructL()
	{
	LBSLOG(ELogP2, "CGatewayInterface::ConstructL");
	iManager = CProtocolManager::NewL(*this);
	}


/** Send a response to a privacy request to the network.
@see CLbsNetworkProtocolBase::RespondPrivacyRequest()
*/
void CGatewayInterface::RespondPrivacyRequest(const TLbsNetSessionId& aSessionId, const TLbsPrivacyResponse& aResponse)
	{
	LBSLOG(ELogP2, "CGatewayInterface::RespondPrivacyRequest");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "PrivResp : %d\n", aResponse);	
	iManager->PrivacyResp(aSessionId, aResponse);
	}


/** Send a response to a location request to the network.
@see CLbsNetworkProtocolBase::RespondLocationRequest()
*/
void CGatewayInterface::RespondLocationRequest(const TLbsNetSessionId& aSessionId, TInt aReason, 
												const TPositionInfoBase& aPosInfo)
	{
	LBSLOG(ELogP2, "CGatewayInterface::RespondLocationRequest");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "Reason : %d\n", aReason);	
	iManager->LocationResp(aSessionId, aReason, aPosInfo);
	}


/** Send a request for GPS assistance data to the network.
@see CLbsNetworkProtocolBase::RequestAssistanceData()
*/
void CGatewayInterface::RequestAssistanceData(TLbsAsistanceDataGroup aDataRequestMask)
	{
	LBSLOG(ELogP2, "CGatewayInterface::RequestAssistanceData");
	LBSLOG2(ELogP2, "Data Mask : 0x%x", aDataRequestMask);
	iManager->AssistanceDataReq(aDataRequestMask);
	}

/** Send a request for an approximate location based on the current network cell to the network.
@see CLbsNetworkProtocolBase::RequestNetworkLocation()
*/
void CGatewayInterface::RequestNetworkLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions)
	{
	LBSLOG(ELogP2, "CGatewayInterface::RequestNetworkLocation");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	iManager->NetworkBasedLocationReq(aSessionId, aOptions);
	}


/** Tell the protocol module to finish the current network based location request.
@see CLbsNetworkProtocolBase::CancelNetworkLocation()
*/
void CGatewayInterface::CancelNetworkLocation(const TLbsNetSessionId& aSessionId,
							TInt aReason)
	{
	LBSLOG(ELogP2, "CGatewayInterface::CancelNetworkLocation");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "Reason : %d\n", aReason);	
	iManager->NetworkBasedLocationCompleteInd(aSessionId, aReason);
	}


/** Request self locate.
@see CLbsNetworkProtocolBase::RequestSelfLocation()
*/
void CGatewayInterface::RequestSelfLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions)	
	{
	LBSLOG(ELogP2, "CGatewayInterface::RequestSelfLocation");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());
#ifdef ENABLE_LBS_DEV_LOGGER
	switch (aOptions.ClassType())
		{
		case ELbsNetPosRequestOptionsClass:
			{
			const TLbsNetPosRequestOptions& options(static_cast<const TLbsNetPosRequestOptions&>(aOptions));
			LBSLOG(ELogP2, "ELbsNetPosRequestOptionsClass params:");
			LBSLOG2(ELogP2, "\tNewClientConnected() : %d", options.NewClientConnected());
			(void) options; // avoid a compiler warning
			break;
			}
		case ELbsNetPosRequestOptionsAssistanceClass:
			{
			const TLbsNetPosRequestOptions& options(static_cast<const TLbsNetPosRequestOptions&>(aOptions));
			LBSLOG(ELogP2, "ELbsNetPosRequestOptionsClass params:");
			LBSLOG2(ELogP2, "\tNewClientConnected() : %d", options.NewClientConnected());
			const TLbsNetPosRequestOptionsAssistance& optionsAssistance(static_cast<const TLbsNetPosRequestOptionsAssistance&>(aOptions));
			LBSLOG(ELogP2, "ELbsNetPosRequestOptionsAssistanceClass params:");
			LBSLOG2(ELogP2, "\tDataRequestMask() : 0x%x", optionsAssistance.DataRequestMask());
			LBSLOG2(ELogP2, "\tPosMode() : 0x%x", optionsAssistance.PosMode());
			(void) options; // avoid a compiler warning
			(void) optionsAssistance; // avoid a compiler warning
			break;
			}
		}
#endif
	iManager->SelfLocationReq(aSessionId, aOptions);
	}


/** Cancel request for self locate.
@see CLbsNetworkProtocolBase::CancelSelfLocation()
*/
void CGatewayInterface::CancelSelfLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason)
	{
	LBSLOG(ELogP2, "CGatewayInterface::CancelSelfLocation");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "Reason : %d\n", aReason);	
	iManager->SelfLocationCompleteInd(aSessionId, aReason);
	}

/** Lbs System Status information.
@see CLbsNetworkProtocolBase::AdviceSystemStatus()
*/
void CGatewayInterface::AdviceSystemStatus(TLbsSystemStatus aStatus)
	{
	LBSLOG(ELogP2, "CGatewayInterface::AdviceSystemStatus");
	LBSLOG2(ELogP2, "Status : %d\n", aStatus);	
	iManager->SystemStatusInd(aStatus);
	}


/** Send a request to send the handsets location to a third party to the network.
@see CLbsNetworkProtocolBase::RequestTransmitLocation()
*/
void CGatewayInterface::RequestTransmitLocation(const TLbsNetSessionId& aSessionId,
								const TDesC& aDestination, TInt aPriority)
	{
	LBSLOG(ELogP2, "CGatewayInterface::RequestTransmitLocation");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "Priority : %d\n", aPriority);	
	iManager->TransmitLocationReq(aSessionId, aDestination, aPriority);
	}


/** Tell the protocol module to end the current send location to third party request.
@see CLbsNetworkProtocolBase::CancelTransmitLocation()
*/
void CGatewayInterface::CancelTransmitLocation(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	LBSLOG(ELogP2, "CGatewayInterface::CancelTransmitLocation");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "Reason : %d\n", aReason);	
	iManager->TransmitLocationCompleteInd(aSessionId, aReason);
	}


/** Send privacy request to protocol module's observer.
@param aSessionId The Id of the location session within the LBS sub-system.
@param aEmergency ETrue if this is an emergency privacy request, EFalse otherwise.
@param aPrivacy Type of privacy request specified by the network.
@param aRequestInfo Data about the external requestor of this handsets location.

@see MLbsNetworkProtocolObserver::ProcessPrivacyRequest() 
*/
void CGatewayInterface::PrivacyReq(const TLbsNetSessionId& aSessionId, TBool aEmergency, 
					const TLbsNetPosRequestPrivacy& aPrivacy,
					const TLbsExternalRequestInfo& aRequestInfo)
	{
	LBSLOG(ELogP2, "CGatewayInterface::PrivacyReq");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "Emergency : %d", aEmergency);
	LBSLOG3(ELogP2, "PrivacyReq : (%d, %d)\n", aPrivacy.RequestAdvice(), aPrivacy.RequestAction());	
	iObserver.ProcessPrivacyRequest(aSessionId, aEmergency, aPrivacy, aRequestInfo);
	}


/** Send network-based location to protocol module's observer.
@param aReferenceLocation The network-based reference location.

@see MLbsNetworkProtocolObserver::ProcessNetworkBasedLocation() 
*/
void CGatewayInterface::NetworkLocationInd(const TLbsNetSessionId& aSessionId, 
					const TPositionInfoBase& aPosInfo)
	{
	LBSLOG(ELogP2, "CGatewayInterface::NetworkLocationInd");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	iObserver.ProcessLocationUpdate(aSessionId, aPosInfo);
	}


/** Send assistance data to protocol module's observer.
@param aGroupMask This identifies the data group relevant for this indication.
@param aData This holds GPS assistance data to be delivered to LBS.
@param aReason A general error associated with assistance data retrieval.

@see MLbsNetworkProtocolObserver::ProcessAssistanceData() 
*/
void CGatewayInterface::AssistanceDataInd(const TLbsAsistanceDataGroup& aGroupMask,
						const RLbsAssistanceDataBuilderSet& aData, TInt aReason)
	{
	LBSLOG(ELogP2, "CGatewayInterface::AssistanceDataInd");
	LBSLOG2(ELogP2, "Data Mask : 0x%x", aGroupMask);
	LBSLOG2(ELogP2, "Reason : %d\n", aReason);	
	iObserver.ProcessAssistanceData(aGroupMask, aData, aReason);
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

@see MLbsNetworkProtocolObserver::ProcessLocationRequest() 
*/
void CGatewayInterface::LocationReq(const TLbsNetSessionId& aSessionId, TBool aEmergency,
					const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aType, 
					const TLbsNetPosRequestQuality& aQuality,
					const TLbsNetPosRequestMethod& aPosMethod)
	{
	LBSLOG(ELogP2, "CGatewayInterface::LocationReq");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "Emergency : %d", aEmergency);
	LBSLOG2(ELogP2, "Type : %d", aType);
	iObserver.ProcessLocationRequest(aSessionId, aEmergency, aType, aQuality, aPosMethod);
	}


/** Send session complete indication to protocol module's observer.
@param aSessionId The Id of the session completed.
@param aReason Reason for the completion of the request. 
			KErrNone if the request is successfully completed, otherwise
			it is one of a range of error codes.

@see MLbsNetworkProtocolObserver::ProcessSessionComplete() 
*/
void CGatewayInterface::SessionCompleteInd(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	LBSLOG(ELogP2, "CGatewayInterface::SessionCompleteInd");
	LBSLOG3(ELogP2, "SessionId : (0x%x, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "Reason : %d\n", aReason);	
	iObserver.ProcessSessionComplete(aSessionId, aReason);
	}


/** Network request for LBS capabilities
@param aCapabilities The object to hold the returned capabilities from LBS.

@see MLbsNetworkProtocolObserver::GetCapabilities() 
*/
void CGatewayInterface::GetCapabilities(TLbsNetPosCapabilities& aCapabilities)
	{
	LBSLOG(ELogP2, "CGatewayInterface::GetCapabilities");
	iObserver.GetCurrentCapabilities(aCapabilities);
	}


/** Advise LBS of a change in active service status
@param aActiveServiceMask A mask specifying the currently active services.

@see MLbsNetworkProtocolObserver::StatusUpdate() 
*/
void CGatewayInterface::StatusUpdate(MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask aActiveServiceMask)
	{
	LBSLOG(ELogP2, "CGatewayInterface::StatusUpdate");
	LBSLOG2(ELogP2, "Status Mask : 0x%x", aActiveServiceMask);
	iObserver.ProcessStatusUpdate(aActiveServiceMask);
	}

/** Asks LBS to reset the assistance data it holds
@param aMask A mask specifying the assistance data to be reset.

@see MLbsNetworkProtocolObserver::ProcessAssistanceData() 
*/
void CGatewayInterface::ResetAssistanceData(TLbsAssistanceDataGroup aMask, const RLbsAssistanceDataBuilderSet& aData)
	{
	LBSLOG(ELogP2, "CGatewayInterface::ResetAssistanceData");
	LBSLOG2(ELogP2, "Mask : 0x%x", aMask);
	iObserver.ProcessAssistanceData(aMask, aData, KPositionAssistanceDataReset);
	}

/** Defines the plug-in interface implementation UIDs, required by ECOM.
*/
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY(KPluginUidValue, CGatewayInterface::NewL)
    };


/** Exported function for ECOM to retrieve implementations table
*/
EXPORT_C 
const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

