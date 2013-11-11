// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ETel Multimode Quality of Service (QoS) Support
// GPRS Rel97/98, CDMAOne, CDMA2000, GPRS/UMTS Rel99 and UMTS Rel4.
// 
//

#include <et_clsvr.h>
#include <etelext.h>

// ETel Packet Data header files
#include "etelQoS.h"
#include "pcktptr.h"
#include <pcktcs.h>

// Used by ETel to instruct TSY to create a name for the newly opened object
_LIT(KETelNewQoSName, "::");

/***********************************************************************************/
//
// RPacketQoS
//
/***********************************************************************************/

EXPORT_C RPacketQoS::RPacketQoS()
	:iEtelPacketQoSPtrHolder(NULL)
/** Standard constructor. */
	{
	}

EXPORT_C void RPacketQoS::ConstructL()
/**
 * This function creates an instance of CPacketQoSPtrHolder and assigns it to the 
 * iEtelPacketQoSPtrHolder pointer. 
 
@capability None
*/
	{
	__ASSERT_ALWAYS(iEtelPacketQoSPtrHolder == NULL, PanicClient(EEtelPanicHandleNotClosed));
	iEtelPacketQoSPtrHolder = CPacketQoSPtrHolder::NewL(CEtelPacketPtrHolder::EMaxNumPacketQoSPtrSlots, CEtelPacketPtrHolder::EMaxNumPacketQoSPtrCSlots);
	}

EXPORT_C void RPacketQoS::Destruct()
/**
 * This function deletes and NULLs the iEtelPacketQoSPtrHolder pointer.
 
@capability None
*/
	{
	delete iEtelPacketQoSPtrHolder;
	iEtelPacketQoSPtrHolder = NULL;
	}

EXPORT_C TInt RPacketQoS::OpenNewQoS(RPacketContext& aPacketContext, TDes& aProfileName)
//This function may be called by the client application.
/** Creates a new QoS profile (i.e. RPacketQoS class instance). 

An RPacketQoS may only be opened on an existing RPacketContext subsession. 

When the new QoS profile is created, the TSY will assign it a unique name to it. The 
client can use this name to uniquely identify the particular context.

An opened RPacketContext must be closed explicitely by calling Close() to 
prevent a resource (memory) leak.

@param aPacketContext A reference to a previously instantiated RPacketContext, on 
which to open the QoS profile.
@param aProfileName On return, the unique QoS name.
@return KErrNone if successful, a system-wide error code if not. 
@capability None
*/
	{
	RSessionBase session = aPacketContext.SessionHandle();

	TRAPD(ret,ConstructL());
	if (ret)
		{
		return ret;
		}
	TInt subSessionHandle=aPacketContext.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle!=0,PanicClient(EEtelPanicNullHandle)); // client has no existing sub-session!
	TPtrC name(KETelNewQoSName);	// necessary so that server knows to ask TSY for new name

	SetSessionHandle(session);
	ret = CreateSubSession(session,EEtelOpenFromSubSession,TIpcArgs(&name,&aProfileName,subSessionHandle));
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C TInt RPacketQoS::OpenExistingQoS(RPacketContext& aPacketContext, const TDesC& aProfileName)
//This function may be called by the client application.
/** Opens a handle on an existing RPacketQoS object identified by the profile name.

An opened RPacketContext must be closed explicitely by calling Close() to
prevent a resource (memory) leak.

@param aPacketContext This identifies to which RPacketContext the QoS profile
to be opened belongs.
@param aProfileName This uniquely identifies to ETel/TSY which existing RPacketQoS
object the client wants to open. This name was previously assigned by the
TSY when the specified RPacketQoS was originally created using RPacketQoS::OpenNewQoS().
@return KErrNone if successful, KErrNotFound if the object does not exist. 
@capability None
*/
 	{
	RSessionBase session = aPacketContext.SessionHandle();

	TRAPD(ret,ConstructL());
	if (ret)
		{
		Destruct();
		return ret;
		}
	TInt subSessionHandle=aPacketContext.SubSessionHandle();
	__ASSERT_ALWAYS(subSessionHandle != 0,PanicClient(EEtelPanicNullHandle));
	__ASSERT_ALWAYS(aProfileName.Length() != 0,PanicClient(KErrBadName));

	SetSessionHandle(session);
	ret = CreateSubSession(session,EEtelOpenByNameFromSubSession,TIpcArgs(&aProfileName,TIpcArgs::ENothing,subSessionHandle));
	if (ret)
		Destruct();
	return ret;
	}

EXPORT_C void RPacketQoS::Close()
//This function may be called by the client application.
/** Closes the client's current sub-session with ETel.

Any outstanding requests the client may have with ETel (notifications, for 
example) will be automatically destroyed by ETel.

An opened RPacketContext must be closed explicitly by calling Close() to 
prevent a resource (memory) leak. 
@capability None
*/
	{
	CloseSubSession(EEtelClose);
	Destruct();
	}

EXPORT_C void RPacketQoS::SetProfileParameters(TRequestStatus& aStatus, TDes8& aProfile) const
//This asynchronous function may be called by the client application.
/** Passes to the TSY all the parameters required to configure Quality of Service 
for a particular context. 

The parameters are passed via a TQoSGPRSRequested, TQoSCDMA2000Requested or 
TQoSRequestedR99_R4 class, depending on the current network and packet-service.

This is an asynchronous function call.

The TQoSCapsxxxx classes inherit from TPacketDataConfigBase and therefore 
contain an iExtensionId member data identifying whether they are a GPRS, 
CDMA or R99_R4 configuration class.

To determine how to correctly unpack the descriptor once it reaches the TSY, 
the TSY must first cast the descriptor to a TPacketDataConfigBase pointer 
and check the value of the iExtensionId parameter. Referring to this value 
the TSY will then cast the descriptor to the correct TPacketDataConfigBase-
derived class.

Use RTelSubSessionBase::CancelAsyncRequest(EPacketQoSSetProfileParams) to 
cancel a previously placed asynchronous SetProfileParameters() request.


@param aStatus On completion, KErrNone if successful, a system-wide error 
code if not.
@param aProfile A TQoSGPRSRequested, TQoSCDMA2000Requested or TQoSR99_R4Requested 
containing the configuration data, packaged inside a TPckg<> and passed as a 
descriptor reference. 
@capability WriteDeviceData
@capability NetworkServices
*/
	{
	Set(EPacketQoSSetProfileParams, aStatus, aProfile);
	}

EXPORT_C void RPacketQoS::GetProfileParameters(TRequestStatus& aStatus, TDes8& aProfile) const
//This asynchronous function may be called by the client application.
/** Retrieves the TSY/phone values of all parameters associated with a negotiated 
QoS profile.

This is an asynchronous function.

QoS profile parameters are contained as member data in either a TQoSGPRSNegotiated, 
TQoSCDMA2000Negotiated, or TQoSR99_R4Negotiated class, depending on the current packet 
service. These T-classes are packaged inside a TPckg<> template class, enabling them to 
be passed as a descriptor to the TSY. 

The Negotiated QoS profile class derives from TPacketDataConfigBase and therefore contains
iExtensionId member data identifying whether it is a GPRS, CDMA or R99_4 configuration 
class. To determine how to correctly unpack the descriptor once it reaches the TSY, the 
TSY must first cast the descriptor to a TPacketDataConfigBase pointer and check whether 
the value of the iExtensionId parameter. Depending on this value, the TSY will then cast 
the descriptor to the correct TPacketDataConfigBase-derived class.

Use RTelSubSessionBase::CancelAsyncRequest(EPacketQoSGetProfileParams) to 
cancel a previously placed asynchronous GetProfileParameters() request.


@param aStatus On return, KErrNone if successful.
@param aProfile On completion, a TQoSGPRSNegotiated, TQoSCDMA2000Negotiated or 
TQoSR99_R4Negotiated containing the profile data, packaged inside a TPckg<> and 
returned as a descriptor reference.

@capability ReadDeviceData
*/
	{
	Get(EPacketQoSGetProfileParams, aStatus, aProfile);
	}

EXPORT_C void RPacketQoS::GetProfileCapabilities(TRequestStatus& aStatus, TDes8& aProfileCaps) const
//This asynchronous function may be called by the client application.
/** Gets the Quality of Service capabilities for the current packet service. 

Depending on the the packet service type, a TQoSCapsGPRS, TQoSCapsCDMA2000,  
TQoSCapsR99_R4 or TQoSCapsR5 will be passed in. The TSY will assign a bitmask for each 
parameter in the relevant TQoSCapsxxx class. This bitmask contains all values 
which can be assigned to the particular parameter for the current packet 
service and phone.

This is an asynchronous function.

The TQoSCapsxxxx classes inherit from TPacketDataConfigBase and therefore 
contain iExtensionId member data identifying the configuration class 
(GPRS Rel97/98, CDMA, Rel99 or Rel4).

To determine how to correctly unpack the descriptor once it reaches the TSY, the 
TSY must first cast the descriptor to a TPacketDataConfigBase pointer and check whether 
the value of the iExtensionId parameter. Depending on this value, the TSY will then cast 
the descriptor to the correct TPacketDataConfigBase-derived class.

Use RTelSubSessionBase::CancelAsyncRequest(EPacketQoSGetProfileCaps) to cancel 
a previously placed asynchronous GetProfileCapabilities() request.


@param aStatus On completion, KErrNone if successful.
@param aProfileCaps A reference to a TQoSCapsGPRS, TQosCapsCDMA2000,  
TQoSCapsR99_R4 or TQoSCapsR5 packaged in a TPckg and returned as a descriptor. 

@capability None
*/
	{
	Get(EPacketQoSGetProfileCaps, aStatus, aProfileCaps);
	}

EXPORT_C void RPacketQoS::NotifyProfileChanged(TRequestStatus& aStatus, TDes8& aProfile) const
//may be called by the client application.
/**
Notifies a client of a change in the negotiated QoS profile is received 
by the TSY from the phone. 

This is a asynchronous function.

The new QoS profile returned will be either TQoSGPRSNegotiated, 
TQoSCDMA2000Negotiated or TQoSR99_R4Negotiated, depending on the 
current packet service. The TQoSxxxNegotiated class will contain 
updated values for each QoS parameter.
 
If the client wishes to receive further notifications, NotifyProfileChanged() 
must be called again. 

Use RTelSubSessionBase::CancelAsyncRequest(EPacketQoSNotifyProfileChanged) 
to cancel a previously placed asynchronous NotifyProfileChanged() request.


@param aStatus On completion, KErrNone if successful.
@param aProfile A descriptor reference to a TPckg<TQoSXXXNegotiated> object. 

@capability ReadDeviceData
*/
 	{
	Get(EPacketQoSNotifyProfileChanged, aStatus, aProfile);
	}


EXPORT_C RPacketQoS::TQoSCapsGPRS::TQoSCapsGPRS()
:TPacketDataConfigBase(), iPrecedence(EUnspecifiedPrecedence), iDelay(EUnspecifiedDelayClass),
 iReliability(EUnspecifiedReliabilityClass), iPeak(EUnspecifiedPeakThroughput),
 iMean(EUnspecifiedMeanThroughput)
/** Standard constructor. 

@deprecated v9.3 Use RPacketQoS::TQoSCapsR99_R4::TQoSCapsR99_R4 or RPacketQoS::TQoSCapsR5::TQoSCapsR5 instead. 

Initializes the GPRS QoS profile with default values. Sets iExtensionId to KConfigGPRS. 
*/
	{
	iExtensionId = KConfigGPRS;
	}

EXPORT_C RPacketQoS::TQoSGPRSRequested::TQoSGPRSRequested()
:TPacketDataConfigBase(), iReqPrecedence(EUnspecifiedPrecedence), iMinPrecedence(EUnspecifiedPrecedence),
 iReqDelay(EUnspecifiedDelayClass), iMinDelay(EUnspecifiedDelayClass),
 iReqReliability(EUnspecifiedReliabilityClass), iMinReliability(EUnspecifiedReliabilityClass),
 iReqPeakThroughput(EUnspecifiedPeakThroughput), iMinPeakThroughput(EUnspecifiedPeakThroughput),
 iReqMeanThroughput(EUnspecifiedMeanThroughput), iMinMeanThroughput(EUnspecifiedMeanThroughput)
/**
Standard constructor. 

@deprecated v9.3 Use RPacketQoS::TQoSR99_R4Requested::TQoSR99_R4Requested or RPacketQoS::TQoSR5Requested::TQoSR5Requested instead.

Initializes the requested and minimum GPRS QoS values to the defaults. 
Sets the iExtensionId to KConfigGPRS. 
*/
	{
	iExtensionId = KConfigGPRS;
	}

EXPORT_C RPacketQoS::TQoSGPRSNegotiated::TQoSGPRSNegotiated()
:TPacketDataConfigBase(), iPrecedence(EUnspecifiedPrecedence), iDelay(EUnspecifiedDelayClass),
 iReliability(EUnspecifiedReliabilityClass), iPeakThroughput(EUnspecifiedPeakThroughput),
 iMeanThroughput(EUnspecifiedMeanThroughput)
/** Standard constructor. 

@deprecated v9.3 Use RPacketQoS::TQoSR99_R4Negotiated::TQoSR99_R4Negotiated or RPacketQoS::TQoSR5Negotiated::TQoSR5Negotiated instead.

Initializes the negotiated GPRS QoS values to the defaults. 
Sets iExtensionId to KConfigGPRS. */
	{
	iExtensionId = KConfigGPRS;
	}

EXPORT_C RPacketQoS::TQoSCapsCDMA2000::TQoSCapsCDMA2000()
:TPacketDataConfigBase()
/** Standard constructor. 

Initializes the CDMA QoS profile with default values. Sets iExtensionId to KConfigGPRS. 
*/
	{
	iExtensionId = KConfigCDMA;
	}

EXPORT_C RPacketQoS::TQoSCDMA2000Requested::TQoSCDMA2000Requested()
:TPacketDataConfigBase()
/** Standard constructor. 

Initializes the requested CDMA QoS values to the defaults. Sets iExtensionId to KConfigCDMA. 
*/
	{
	iExtensionId = KConfigCDMA;
	}

EXPORT_C RPacketQoS::TQoSCDMA2000Negotiated::TQoSCDMA2000Negotiated()
:TPacketDataConfigBase()
/** Standard constructor. Sets iExtentionId to KConfigCDMA. */
	{
	iExtensionId = KConfigCDMA;
	}


EXPORT_C RPacketQoS::TQoSCapsR99_R4::TQoSCapsR99_R4()
: TPacketDataConfigBase(),
  iTrafficClass(ETrafficClassUnspecified),
  iDeliveryOrderReqd(EDeliveryOrderUnspecified),
  iDeliverErroneousSDU(EErroneousSDUDeliveryUnspecified),
  iBER(EBERUnspecified),
  iSDUErrorRatio(ESDUErrorRatioUnspecified),
  iTrafficHandlingPriority(ETrafficPriorityUnspecified)
/**
 * Constructor - All values are initialized to unspecified.
 */
	{
	iExtensionId = KConfigRel99Rel4;	
	}

EXPORT_C RPacketQoS::TQoSCapsR5::TQoSCapsR5()
: TQoSCapsR99_R4(),
  iSignallingIndication(EFalse),
  iSourceStatisticsDescriptor(ESourceStatisticsDescriptorUnknown)
/**
 * Constructor - All values are initialized to unspecified.
 */
	{
	iExtensionId = KConfigRel5;	
	}
	
EXPORT_C RPacketQoS::TQoSR99_R4Requested::TQoSR99_R4Requested()
: TPacketDataConfigBase(),
  iReqTrafficClass(ETrafficClassUnspecified),
  iMinTrafficClass(ETrafficClassUnspecified),
  iReqDeliveryOrderReqd(EDeliveryOrderUnspecified),
  iMinDeliveryOrderReqd(EDeliveryOrderUnspecified),
  iReqDeliverErroneousSDU(EErroneousSDUDeliveryUnspecified),	
  iMinDeliverErroneousSDU(EErroneousSDUDeliveryUnspecified),
  iReqMaxSDUSize(0),						 
  iMinAcceptableMaxSDUSize(0),
  iReqBER(EBERUnspecified),
  iMaxBER(EBERUnspecified),					
  iReqSDUErrorRatio(ESDUErrorRatioUnspecified),
  iMaxSDUErrorRatio(ESDUErrorRatioUnspecified),			
  iReqTrafficHandlingPriority(ETrafficPriorityUnspecified),
  iMinTrafficHandlingPriority(ETrafficPriorityUnspecified),
  iReqTransferDelay(0),
  iMaxTransferDelay(0)
/**
 * Constructor - All values are initialized to unspecified or zero.
 */
	{
	iExtensionId = KConfigRel99Rel4;		

	
	iReqMaxRate.iUplinkRate = 0;			//< Range: 1kbps to 8640kbps in varying increments	
	iReqMaxRate.iDownlinkRate = 0;			//< Range: 1kbps to 8640kbps in varying increments					
	iMinAcceptableMaxRate.iUplinkRate = 0;	//< Range: 1kbps to 8640kbps in varying increments		
	iMinAcceptableMaxRate.iDownlinkRate = 0;//< Range: 1kbps to 8640kbps in varying increments			
	
	iReqGuaranteedRate.iUplinkRate = 0;		//< Range: 1kbps to 8640kbps in varying increments	
	iReqGuaranteedRate.iDownlinkRate = 0;	//< Range: 1kbps to 8640kbps in varying increments	
	iMinGuaranteedRate.iUplinkRate = 0;		//< Range: 1kbps to 8640kbps in varying increments	
	iMinGuaranteedRate.iDownlinkRate = 0;	//< Range: 1kbps to 8640kbps in varying increments	
	}

EXPORT_C RPacketQoS::TQoSR5Requested::TQoSR5Requested()
: TQoSR99_R4Requested(),
  iSignallingIndication(EFalse),
  iSourceStatisticsDescriptor(ESourceStatisticsDescriptorUnknown)
 /**
  * Constructor - The values are initialized to zero and unknown.
    Sets iExtentionId to KConfigRel5.
 */
	{
	iExtensionId = KConfigRel5;
	}

EXPORT_C RPacketQoS::TQoSR99_R4Negotiated::TQoSR99_R4Negotiated()
: TPacketDataConfigBase(),
  iTrafficClass(ETrafficClassUnspecified),
  iDeliveryOrderReqd(EDeliveryOrderUnspecified),
  iDeliverErroneousSDU(EErroneousSDUDeliveryUnspecified),
  iMaxSDUSize(0),
  iBER(EBERUnspecified),
  iSDUErrorRatio(ESDUErrorRatioUnspecified),
  iTrafficHandlingPriority(ETrafficPriorityUnspecified),
  iTransferDelay(0)
/**
 * Constructor - All values are initialized to unspecified or zero.
 */
	{
	iExtensionId = KConfigRel99Rel4;	
	
	iMaxRate.iUplinkRate = 0;				//< Range: 1kbps to 8640kbps in varying increments	
	iMaxRate.iDownlinkRate = 0;				//< Range: 1kbps to 8640kbps in varying increments	
				
	iGuaranteedRate.iUplinkRate = 0;		//< Range: 1kbps to 8640kbps in varying increments	
	iGuaranteedRate.iDownlinkRate = 0;		//< Range: 1kbps to 8640kbps in varying increments	
	}
	
EXPORT_C RPacketQoS::TQoSR5Negotiated::TQoSR5Negotiated()
: TQoSR99_R4Negotiated(),
  iSignallingIndication(EFalse),
  iSourceStatisticsDescriptor(ESourceStatisticsDescriptorUnknown)
 /**
  * Constructor - The values are initialized to zero and unknown.
    Sets iExtentionId to KConfigRel5.
 */
	{
	iExtensionId = KConfigRel5;
	}
