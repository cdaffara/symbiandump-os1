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
// This file contains the definition for the ETelMM sub-session
// RMobileLocationServices, which allows access to MT-LR and MO-LR Requests supporting LBS Framwork
// applications and all their functionality.
// 
//

/**
 @file
*/

// From core API
#include <etelext.h>

// Multimode header files
#include <etelmm.h>
#include "mm_hold.h"
#include "mmretrieve.h"

//
//
//  RMobileLocationServices
//
//

/**
Default empty constructor, and is present only to support virtual
function table export.
@publishedPartner
*/
EXPORT_C RMobileLocationServices::RMobileLocationServices(): iMmPtrHolder(NULL)
	{
        
	}

EXPORT_C TInt RMobileLocationServices::Open(RMobilePhone& aPhone)
/**
This function opens a RMobileLocationServices sub-session from a 
RMobilePhone sub-session.The name of the sub-session opened in the 
TSY will equal the string defined by the KETelLocationServices LIT definition.

The API can be called only by the Location Services Network Gateway Process.

In case of hardware the API can be called by providing the SID of the 
Location Services Network Gateway Process in ETel.iby file 
found at \epoc32\rom\include

A patchable constant KLocationServicesNetworkGatewaySid is used for 
defining the SID value.
In ETel.iby file find the following line
patchdata etel.dll@KLocationServicesNetworkGatewaySid <SID>
For example:
patchdata etel.dll@KLocationServicesNetworkGatewaySid 0x12345678

In case of emulator the API can be called by providing the SID of the 
default phone application in epoc.ini file found at \epoc32\data
In epoc.ini file find the following line
etel_KLocationServicesNetworkGatewaySid <SID>
For example:
etel_KLocationServicesNetworkGatewaySid 0x12345678

@param aPhone The RMobilePhone sub-session relative to which this
              sub-session will open.
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@capability SID
@publishedPartner
*/
	{
	RSessionBase* session=&aPhone.SessionHandle();
	__ASSERT_ALWAYS(session!=NULL,PanicClient(EEtelPanicNullHandle));
	TInt subSessionHandle=aPhone.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=NULL,PanicClient(EEtelPanicNullHandle));

	TRAPD(ret,ConstructL());
	if (ret)
		{
		return ret;
		}
	TPtrC name(KETelLocationServices);

	SetSessionHandle(*session);
	TIpcArgs args(&name,TIpcArgs::ENothing,subSessionHandle);

	ret = CreateSubSession(*session,EEtelOpenFromSubSessionLcs,args);
	if (ret)
		{
		Destruct();	
		}
	return ret;
	}

/**
This function member closes a RMobileLocationServices sub-session. 
@capability None
@publishedPartner
*/
EXPORT_C void RMobileLocationServices::Close()
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

/**
Default constructor, initialising version number of this data
structure.

@see TMultimodeEtelV7Api

@publishedPartner
*/
EXPORT_C RMobileLocationServices::TNotifyMtlrV7::TNotifyMtlrV7()
	{
	iExtensionId = KEtelExtMultimodeV7;
	}

/**
Default constructor, initialising version number of this data
structure.

@see TMultimodeEtelV7Api

@publishedPartner
*/
EXPORT_C RMobileLocationServices::TStartMolrV7::TStartMolrV7()
	{
	iExtensionId = KEtelExtMultimodeV7;
	}

/**
Default constructor, initialising version number of this data
structure.

@see TMultimodeEtelV7Api

@publishedPartner
*/
EXPORT_C RMobileLocationServices::TMolrReturnResultV7::TMolrReturnResultV7()
	{
	iExtensionId = KEtelExtMultimodeV7;
	}

/**
Default constructor, initialising version number of this data
structure.

@see TMultimodeEtelV7Api

@publishedPartner
*/
EXPORT_C RMobileLocationServices::TMeasurementControlV7::TMeasurementControlV7()
	{
	iExtensionId = KEtelExtMultimodeV7;
	}

/**
Default constructor, initialising version number of this data
structure.

@see TMultimodeEtelV7Api

@publishedPartner
*/
EXPORT_C RMobileLocationServices::TMeasurementReportV7::TMeasurementReportV7()
	{
	iExtensionId = KEtelExtMultimodeV7;
	}

EXPORT_C void RMobileLocationServices::NotifyMtlr(TRequestStatus& aReqStatus,TDes8& aNotifyMtlr) 
/** 
This member function notifies the client about the incoming MTLR requests 
and provides the Client with a class reference,Which contains the details 
of the request made by the External client via Network.
USE CancelAsyncRequest(EMobileLocationServicesNotifyMtlr) to cancel the 
previously placed NotifyMtlr() request.

As this API can be used for emergency requests as well as the non-emergency 
ones, this method is always treated as an emergency request by ETel/TSY. 
Clients that need to handle the emergency LCS requests should pre-allocate 
enough memory during system initialisation to reserve it for handling of 
the emergency LCS MT-LR. The emergency client also needs to establish itself 
as an "Emergency Client" to the Etel server at startup, to ensure that enough 
memory is pre-allocated by ETel/TSY and that the handling of this notification 
will never fail due to lack of memory. The client may establish itself as an 
Emergency Client by calling the RPhone::SetEmergencyClient() API, 
specifying EEmergencyLCSRequest as the request type. 

@param aReqStatus 	On return, KErrNone if successful.
@param aNotifyMtlr On completion,the mobile terminated location request 
in TNotifyMtlrV7Pckg.
@see TNotifyMtlrV7
@capability None
@publishedPartner
*/
	{
	TReqPriorityType type=EIsaEmergencyRequest;
	Get(EMobileLocationServicesNotifyMtlr, aReqStatus, aNotifyMtlr, type);
	}

EXPORT_C void RMobileLocationServices::SendMtlrResponse(TRequestStatus& aReqStatus,const TMtlrResponse aMtlrResponse)  
/** 
This member function sends the response given by the client to the MTLR request 
made by the network

Clients that need to handle the emergency LCS requests should pre-allocate enough 
memory during system initialisation to reserve it for responding to the 
emergency LCS MT-LR. The emergency client also needs to establish itself 
as an "Emergency Client" to the Etel server at startup, to ensure that enough 
memory is preallocated by ETel/TSY and that the handling of this request will never 
fail due to lack of memory. The client may establish itself as an Emergency Client 
by calling the RPhone::SetEmergencyClient() API, specifying EEmergencyLCSRequest 
as the request type. 

@param aReqStatus 	On return, KErrNone if successful.
@param aMtlrResponse It holds the response of the UE after a privacy check has being made 
					 on the request made by the External Client.
@see RMobileLocationServices::NotifyMtlr()
@capability None
@publishedPartner
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtrC8& ptr = iMmPtrHolder->SetC(CMobileLocationServicesPtrHolder::ESlot1LcsSendMtlrResponse,aMtlrResponse);
	TReqPriorityType type=EIsaEmergencyRequest;
	Set(EMobileLocationServicesSendMtlrResponse,aReqStatus, ptr, type);
	}

EXPORT_C void RMobileLocationServices::SendMtlrResponse(TRequestStatus& aReqStatus,const TMtlrError aMtlrError) 
/**
This function sends a response to the MTLR request made by the Network in 
erroneous conditions.

Clients that need to handle the emergency LCS requests should pre-allocate enough 
memory during system initialisation to reserve it for responding to the emergency 
LCS MT-LR. The emergency client also needs to establish itself as an 
"Emergency Client" to the Etel server at startup, to ensure that enough memory is 
preallocated by ETel/TSY and that the handling of this request will never fail due 
to lack of memory. The client may establish itself as an Emergency Client by calling 
the RPhone::SetEmergencyClient() API, specifying EEmergencyLCSRequest as the request type. 

@param aReqStatus 	On return, KErrNone if successful.
@param aMtlrError   It contains the error report generated by the UE,which is sent 
back to the Network
@see RMobileLocationServices::NotifyMtlr
@capability None
@publishedPartner
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobileLocationServicesPtrHolder::ESlot1LcsSendMtlrError, aMtlrError);
	TReqPriorityType type=EIsaEmergencyRequest;
	Set(EMobileLocationServicesSendMtlrError,aReqStatus, ptr1, type);
	}

EXPORT_C void RMobileLocationServices::SendMtlrResponse(TRequestStatus& aReqStatus,const TMtlrReject aMtlrReject)
/**
This function sends a response to the MTLR request made by the Network,When 
UE wants to reject the MTLR request made by an External Client.

Clients that need to handle the emergency LCS requests should pre-allocate enough 
memory during system initialisation to reserve it for responding to the emergency 
LCS MT-LR. The emergency client also needs to establish itself as an 
"Emergency Client" to the Etel server at startup, to ensure that enough memory 
is preallocated by ETel/TSY and that the handling of this request will never fail due to 
lack of memory. The client may establish itself as an Emergency Client by calling 
the RPhone::SetEmergencyClient() API, specifying EEmergencyLCSRequest as the request type. 

@param aReqStatus 	On return, KErrNone if successful.
@param aMtlrReject It contains the details of the Rejection made by the UE.
@capability None
@publishedPartner
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtrC8& ptr1 = iMmPtrHolder->SetC(CMobileLocationServicesPtrHolder::ESlot1LcsMtlrReject, aMtlrReject);
	TReqPriorityType type=EIsaEmergencyRequest;
	Set(EMobileLocationServicesRejectMtlr,aReqStatus, ptr1, type);
	}


EXPORT_C void RMobileLocationServices::SendMolr(TRequestStatus& aReqStatus,const TDesC8& aStartMolr,TDes8& aMolrReturnResult) 
/** 
This member function is used to trigger a Molr request. It carries all the details 
of the Molr request the UE wants to send to the network.This request is completed 
after the completion of SendMeasurementReport API when the Network sends back an Error 
report sent by the remote External client which received the UE location via network
or a Location estimate response to the Self-Location Molr request i.e being made by 
the UE to the Network.

@param aReqStatus 	On return, KErrNone if successful.
@param aStartMolr   A TStartMolrV7Pckg holding the details of the 
MOLR(Mobile Originating Location Request) made by the UE.
@see TStartMolrV7
@param aMolrReturnResult On completion,result containing the velocity and location 
estimate or errors in TMolrReturnResultV7Pckg.
@see TMolrReturnResultV7
@capability None						 			    
@publishedPartner
*/
	{
	SetAndGet(EMobileLocationServicesSendMolr,aReqStatus, aStartMolr,aMolrReturnResult);
	}

EXPORT_C void RMobileLocationServices::NotifyMeasurementControl(TRequestStatus& aReqStatus,TDes8& aMeasurementControl)
/**
This function waits for the notification of incoming A-GPS assistance 
data from the network in continuance of the MTLR or MOLR.

As this API can be used for emergency requests as well as the non-emergency ones, 
this method is always treated as an emergency request by ETel/TSY. Clients that need to 
handle the emergency LCS requests should preallocate enough memory during 
system initialisation to reserve it for handling of the emergency LCS Measrement Control. 
The emergency client also needs to establish itself as an "Emergency Client" to the Etel 
server at startup, to ensure that enough memory is pre-allocated by ETel/TSY and that the
handling of this notification will never fail due to lack of memory. The client may 
establish itself as an Emergency Client by calling the RPhone::SetEmergencyClient() API, 
specifying EEmergencyLCSRequest as the request type. 

@param aReqStatus 	On return, KErrNone if successful, a system wide error code if not.
@param aMeasurementControl	On completion, A TMeasurementControlV7Pckg holding 
the A-GPS assistance data.
@see TMeasurementControlV7
@see RMobileLocationServices::NotifyMltr()
@see RMobileLocationServices::SendMolr()

@capability None
@publishedPartner
*/
	{
	TReqPriorityType type=EIsaEmergencyRequest;
	Get(EMobileLocationServicesNotifyMeasurementControl,aReqStatus,aMeasurementControl,type);
	}

EXPORT_C void RMobileLocationServices::SendMeasurementReport(TRequestStatus& aReqStatus,const TDesC8& aMeasurementReport)
/**
This function sends the location information, computed by the UE, 
to the network. Location information is computed using the A-GPS assistance
data received from the network through the NotifyMeasurementControl API.

Clients that need to handle the emergency LCS requests should preallocate enough memory 
during system initialisation to reserve it for handling of the emergency LCS MT-LR 
procedure. The emergency client also needs to establish itself as an "Emergency Client" to the 
Etel server at startup, to ensure that enough memory is preallocated by ETel/TSY 
and that the handling of this request will never fail due to lack of memory. 
The client may establish itself as an Emergency Client by calling the 
RPhone::SetEmergencyClient() API, specifying EEmergencyLCSRequest as the request type. 

@param aReqStatus 	On return, KErrNone if successful, a system wide error code if not.
@param aMeasurementReport	A TMeasurementReportV7Pckg holding the location information 
report of the UE to be sent to the network.
@see TMeasurementReportV7
@see RMobileLocationServices::NotifyMeasurementControl()

@capability None
@publishedPartner
*/
	{
	TReqPriorityType type=EIsaEmergencyRequest;
	Set(EMobileLocationServicesSendMeasurementReport,aReqStatus,aMeasurementReport,type);
	}

EXPORT_C void RMobileLocationServices::SendMeasurementReport(TRequestStatus& aReqStatus,const TMeasurementControlFailure aMeasurementControlFailure)
/**
This function sends the measurement control failure cause, encountered while processing
the A-GPS assistance data to compute location information, to the network. 

Clients that need to handle the emergency LCS requests should preallocate enough memory 
during system initialisation to reserve it for handling of the emergency LCS MT-LR procedure. 
The emergency client also needs to establish itself as an "Emergency Client" to the 
Etel server at startup, to ensure that enough memory is preallocated by ETel/TSY and 
that the handling of this request will never fail due to lack of memory. The client 
may establish itself as an Emergency Client by calling the 
RPhone::SetEmergencyClient() API, specifying EEmergencyLCSRequest as the request type. 

@param aReqStatus 	On return, KErrNone if successful, a system wide error code if not.
@param aMeasurementControlFailure	Holds the measurement control failure cause to be sent
									to the network.
@see RMobileLocationServices::NotifyMeasurementControl()

@capability None
@publishedPartner
*/
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtrC8& ptr = iMmPtrHolder->SetC(CMobileLocationServicesPtrHolder::ESlot1LcsSendMeasurementControlFailure, aMeasurementControlFailure);
	TReqPriorityType type=EIsaEmergencyRequest;
	Set(EMobileLocationServicesSendMeasurementControlFailure,aReqStatus,ptr,type);
	}

/**
Delayed construction of heap stored data members.

@leave KErrNoMemory Heap memory allocation failure for
 CMobileLocationServicesPtrHolder object.
*/
void RMobileLocationServices::ConstructL()
	{
	__ASSERT_ALWAYS(iMmPtrHolder==NULL,PanicClient(EEtelPanicHandleNotClosed));
	iMmPtrHolder = CMobileLocationServicesPtrHolder::NewL(CMobileLocationServicesPtrHolder::EMaxNumberLocationServicesPtrSlots,
	CMobileLocationServicesPtrHolder::EMaxNumberLocationServicesPtrCSlots);
	}


/**
Called internally when RMobileLocationServices instance is no longer
required, to ensure clean up of data members from memory.
*/
void RMobileLocationServices::Destruct()
	{
	delete iMmPtrHolder;
	iMmPtrHolder = NULL;
	ResetSessionHandle();
	}


EXPORT_C TInt RMobileLocationServices::SetLcsDomain(const TDomain aDomain) 
/*
This function set the domain into either circuit switched or packet switched.
This is the domain that applies to MO-LR,if the domain is not specified the default domain
will be Packed switched

@param TDomain	Holds the domain information of the phone to be sent
				to the network.

@capability NetworkServices
@capability Location
@publishedPartner
*/
	{
	TPckg<TDomain> ptr(aDomain);
	return Set(EMobileLocationServicesSetLcsDomain,ptr);
	}

/*
This function requests a notification when a signal is received from the network 
to reset the UE Positioning Information held by the handset.
@see 3GPP TS 34.109 V7.2.0 Section 6.10
@see TUePosTechnology

@param aReqStatus 	On return, KErrNone if successful, a system wide error code if not.
@param aUePosTechnology	The positioning data to reset (e.g., A-GPS). See 3GPP TS 34.109 V7.2.0 Section 6.10 
@capability None
@publishedPartner
@released
*/
EXPORT_C void RMobileLocationServices::NotifyResetUePositioningInformation(TRequestStatus& aReqStatus, TUePosTechnology& aUePosTechnology) const
	{
	__ASSERT_ALWAYS(iMmPtrHolder!=NULL,PanicClient(EEtelPanicNullHandle));
	TPtr8& ptr1 = iMmPtrHolder->Set(CMobileLocationServicesPtrHolder::ESlot1LcsNotifyUeResetPositioningInformation, aUePosTechnology);

	Get(EMobileLocationServicesNotifyResetUePositioningInformation, aReqStatus, ptr1);
	}
