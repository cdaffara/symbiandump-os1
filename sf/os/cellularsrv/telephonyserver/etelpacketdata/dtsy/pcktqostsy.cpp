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
// PACKETQOSTSY.CPP
// 
//

#include "pckttsy.h"
#include "testdef.h"
#include "etelQoS.h"
#include <et_clsvr.h>
#include "dpcktlog.h"
#include <pcktcs.h>

#include "coretsy.h"

/**************************************************************************/
//
//	CGprsQoSDGprsTsy
//
/**************************************************************************/

CGprsQoSDGprsTsy* CGprsQoSDGprsTsy::NewL(CPhoneFactoryDummyBase* aFac)
/** 
 * NewL method - Standard 2 phase constructor.
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy: NewL()"));

	CGprsQoSDGprsTsy* subsession=new(ELeave) CGprsQoSDGprsTsy(aFac);
	CleanupStack::PushL(subsession);
	subsession->ConstructL();
	CleanupStack::Pop();
	return subsession;
	}

void CGprsQoSDGprsTsy::Init()
	{}

CGprsQoSDGprsTsy::CGprsQoSDGprsTsy(CPhoneFactoryDummyBase* aFac)
	:CSubSessionExtDummyBase(aFac)
	{
	}

void CGprsQoSDGprsTsy::ConstructL()
/**
 * ConstructL method
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy: Entered constructor"));
	}

CGprsQoSDGprsTsy::~CGprsQoSDGprsTsy()
/**
 * Destructor
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy: Entered destructor"));
	}

CTelObject* CGprsQoSDGprsTsy::OpenNewObjectByNameL(const TDesC& /*aName*/)
/**
 * Opening object(s) from RPacketQoS is not supported.
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy: OpenNewObjectByNameL() method"));
	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject* CGprsQoSDGprsTsy::OpenNewObjectL(TDes& /*aNewName*/)
/**
 * Opening object(s) from RPacketQoS is not supported
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy: OpenNewObjectL() method"));

	User::Leave(KErrNotSupported);
	return NULL;
	}

CTelObject::TReqMode CGprsQoSDGprsTsy::ReqModeL(const TInt aIpc)
/**
 * ReqModeL is called from the server's CTelObject::ReqAnalyserL in order to check the 
 * type of request it has.
 * The following are example request types for this dummy TSY. All TSYs do not have to 
 * have these request types but they have been given "sensible" values in this dummy TSY code.
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy: ReqModeL() method"));

	CTelObject::TReqMode ret=0;
	switch (aIpc)
		{
		case EPacketQoSSetProfileParamsCancel:
		case EPacketQoSGetProfileParamsCancel:
		case EPacketQoSGetProfileCapsCancel:
		case EPacketQoSNotifyProfileChangedCancel:
			// The server should not use ReqMode on Cancel requests
			User::Leave(KErrNotSupported);
			break;

		case EPacketQoSSetProfileParams:
		case EPacketQoSGetProfileParams:
		case EPacketQoSGetProfileCaps:	
			break;

		case EPacketQoSNotifyProfileChanged:
			ret=KReqModeMultipleCompletionEnabled | KReqModeRePostImmediately;
			break;

		default:
			User::Leave(KErrNotSupported);
			break;
		}

	return ret;
	}

TInt CGprsQoSDGprsTsy::RegisterNotification(const TInt aIpc)
/**
 * RegisterNotification is called when the server recognises that this notification is 
 * being posted for the first time on this sub-session object.
 * It enables the TSY to "turn on" any regular notification messages that it may receive 
 * from the phone.
 */
	{
	switch (aIpc)
		{
		case EPacketQoSNotifyProfileChanged:
			LOGTEXT(_L8("CGprsQoSDGprsTsy: RegisterNotification - Profile Changed"));
			return KErrNone;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsQoSDGprsTsy: Register error, unknown IPC"));
			return KErrNotSupported;
		}
	}

TInt CGprsQoSDGprsTsy::DeregisterNotification(const TInt aIpc)
/**
 * DeregisterNotification is called when the server recognises that this notification
 * will not be posted again because the last client to have a handle on this sub-session
 * object has just closed the handle.
 * It enables the TSY to "turn off" any regular notification messages that it may 
 * receive from the phone.
 */
	{
	switch (aIpc)
		{
		case EPacketQoSNotifyProfileChanged:
			LOGTEXT(_L8("CGprsQoSDGprsTsy: DeregisterNotification - Profile Changed"));
			return KErrNone;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsQoSDGprsTsy: Deregister error, unknown IPC"));
			return KErrNotSupported;
		}
	}

TInt CGprsQoSDGprsTsy::NumberOfSlotsL(const TInt aIpc)
/**
 * NumberOfSlotsL is called by the server when it is registering a new notification.
 * It enables the TSY to tell the server how many buffer slots to allocate for
 * "repost immediately" notifications that may trigger before clients collect them.
 */
	{
	TInt numberOfSlots=1;
	switch (aIpc)
		{
		case EPacketQoSNotifyProfileChanged:
			LOGTEXT(_L8("CGprsDGprsTsy: Registered with 5 slots"));
			numberOfSlots=5;
			break;
		default:
			// Unknown or invalid IPC
			LOGTEXT(_L8("CGprsQoSDGprsTsy: Number of Slots error, unknown IPC"));
			User::Leave(KErrNotSupported);
			break;
		}  
	return numberOfSlots;
	}


TInt CGprsQoSDGprsTsy::ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,
							  const TDataPackage& aPackage)
/**
 * ExtFunc is called by the server when it has an "extended", i.e. non-core ETel request 
 * for the TSY to process.
 * A request handle, request type and request data are passed to the TSY.
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy: ExtFunc() method"));

	switch (aIpc)
		{
		case EPacketQoSSetProfileParams:
			return SetProfile(aTsyReqHandle, aPackage.Des1n());
		case EPacketQoSSetProfileParamsCancel:
			return SetProfileCancel(aTsyReqHandle);
		case EPacketQoSGetProfileParams:
			return GetProfile(aTsyReqHandle, aPackage.Des1n());
		case EPacketQoSGetProfileParamsCancel:
			return GetProfileCancel(aTsyReqHandle);
		case EPacketQoSNotifyProfileChanged:
			return NotifyProfileChanged(aTsyReqHandle, aPackage.Des1n());
		case EPacketQoSNotifyProfileChangedCancel:
			return NotifyProfileChangedCancel(aTsyReqHandle);
		case EPacketQoSGetProfileCaps:
			return GetProfileCaps(aTsyReqHandle, aPackage.Des1n());
		case EPacketQoSGetProfileCapsCancel:
			return GetProfileCapsCancel(aTsyReqHandle);
		default:
			return KErrNotSupported;
		}
	}

TInt CGprsQoSDGprsTsy::CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle)
/**
 * CancelService is called by the server when it is "cleaning-up" any still outstanding
 * asynchronous requests before closing a client's sub-session.
 * This will happen if a client closes its R-class handle without cancelling outstanding
 * asynchronous requests.
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy: - CancelService called"));
	switch (aIpc)
		{
		case EPacketQoSSetProfileParams:
			return SetProfileCancel(aTsyReqHandle);
		case EPacketQoSGetProfileParams:
			return GetProfileCancel(aTsyReqHandle);
		case EPacketQoSGetProfileCaps:
			return GetProfileCapsCancel(aTsyReqHandle);
		case EPacketQoSNotifyProfileChanged:
			return NotifyProfileChangedCancel(aTsyReqHandle);
		default:
			return KErrGeneral; 
		} 
	}

/***********************************************************************************/
//
// The following methods are called from ExtFunc and/or CancelService.
// Each of these will process a TSY request or cancel a TSY request
// Here, example values are returned or checked within this dummy TSY in order to ensure
// that the integrity of the data passed to/from client is maintained
//
/***********************************************************************************/

TInt CGprsQoSDGprsTsy::SetProfile(const TTsyReqHandle aTsyReqHandle,const TDesC8* aConfig)
/** 
 * Set QoS Profile parameters
 * This method is called by both the Hurricane and JetStream API tests.
 * The iExtensionId parameter is checked to determine the packet service.
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy::SetProfile called"));

	User::After(300000); // wait to have KRequestPending
	TInt proceed = 1; // Determines how the function finishes this request
	TPacketDataConfigBase& aCheckId = (*(TPckg<TPacketDataConfigBase>*) aConfig)();

	switch (aCheckId.ExtensionId())
		{
		case TPacketDataConfigBase::KConfigGPRS :
			{
			// If GPRS Rel97/98, validate the descriptor
			TPckg<RPacketQoS::TQoSGPRSRequested>* qosConfigGPRSPckg = (TPckg<RPacketQoS::TQoSGPRSRequested>*)aConfig;
			RPacketQoS::TQoSGPRSRequested& qosConfigV1 = (*qosConfigGPRSPckg)();

			if ((qosConfigV1.iMinDelay != DPCKTTSY_DELAY_MIN1)||
			(qosConfigV1.iMinMeanThroughput != DPCKTTSY_MEAN_THROUGHPUT_MIN1)||
			(qosConfigV1.iMinPeakThroughput != DPCKTTSY_PEAK_THROUGHPUT_MIN1)||
			(qosConfigV1.iMinPrecedence != DPCKTTSY_PRECEDENCE_MIN1)||
			(qosConfigV1.iMinReliability != DPCKTTSY_RELIABILITY_MIN1)||
			(qosConfigV1.iReqDelay != DPCKTTSY_DELAY_REQ1)||
			(qosConfigV1.iReqMeanThroughput != DPCKTTSY_MEAN_THROUGHPUT_REQ1)||
			(qosConfigV1.iReqPeakThroughput != DPCKTTSY_PEAK_THROUGHPUT_REQ1)||
			(qosConfigV1.iReqPrecedence != DPCKTTSY_PRECEDENCE_REQ1)||
			(qosConfigV1.iReqReliability != DPCKTTSY_RELIABILITY_REQ1))
				{	
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				proceed--;		// The method should return with KErrNone after calling ReqCompleted().
				}
			}
			break;
		case TPacketDataConfigBase::KConfigRel99Rel4 : 
			{
			// If Rel99/4, validate the descriptor and complete the request
			TPckg<RPacketQoS::TQoSR99_R4Requested>* qosConfigR99R4Pckg = (TPckg<RPacketQoS::TQoSR99_R4Requested>*)aConfig;
			RPacketQoS::TQoSR99_R4Requested& qosConfigV1 = (*qosConfigR99R4Pckg)();
			
			if ((qosConfigV1.iMinTrafficClass != DPCKTTSY_TRAFFIC_CLASS_MIN)||
			(qosConfigV1.iMinDeliveryOrderReqd != DPCKTTSY_DELIVERY_ORDER_MIN)||
			(qosConfigV1.iMinDeliverErroneousSDU != DPCKTTSY_ERRONEOUS_SDU_DELIVERY_MIN)||
			(qosConfigV1.iMinAcceptableMaxSDUSize != DPCKTTSY_MAX_SDU_SIZE_MIN)||
			(qosConfigV1.iMinAcceptableMaxRate.iUplinkRate != DPCKTTSY_MAX_UPLINK_BITRATE_MIN)||
			(qosConfigV1.iMinAcceptableMaxRate.iDownlinkRate != DPCKTTSY_MAX_DOWNLINK_BITRATE_MIN)||
			(qosConfigV1.iMaxBER != DPCKTTSY_BER_MAX)||
			(qosConfigV1.iMaxSDUErrorRatio != DPCKTTSY_SDU_ERROR_RATIO_MAX)||
			(qosConfigV1.iMaxTransferDelay != DPCKTTSY_TRANSFER_DELAY_MAX)||
			(qosConfigV1.iMinGuaranteedRate.iUplinkRate != DPCKTTSY_GRNT_UPLINK_BITRATE_MIN)||
			(qosConfigV1.iMinGuaranteedRate.iDownlinkRate != DPCKTTSY_GRNT_DOWNLINK_BITRATE_MIN)||
	
			(qosConfigV1.iReqTrafficClass != DPCKTTSY_TRAFFIC_CLASS_REQ)||
			(qosConfigV1.iReqDeliveryOrderReqd != DPCKTTSY_DELIVERY_ORDER_REQ)||
			(qosConfigV1.iReqDeliverErroneousSDU != DPCKTTSY_ERRONEOUS_SDU_DELIVERY_REQ)||
			(qosConfigV1.iReqMaxSDUSize != DPCKTTSY_MAX_SDU_SIZE_REQ)||
			(qosConfigV1.iReqMaxRate.iUplinkRate != DPCKTTSY_MAX_UPLINK_BITRATE_REQ)||
			(qosConfigV1.iReqMaxRate.iDownlinkRate != DPCKTTSY_MAX_DOWNLINK_BITRATE_REQ)||
			(qosConfigV1.iReqBER != DPCKTTSY_BER_REQ)||
			(qosConfigV1.iReqSDUErrorRatio != DPCKTTSY_SDU_ERROR_RATIO_REQ)||
			(qosConfigV1.iReqTransferDelay != DPCKTTSY_TRANSFER_DELAY_REQ)||
			(qosConfigV1.iReqGuaranteedRate.iUplinkRate != DPCKTTSY_GRNT_UPLINK_BITRATE_REQ)||
			(qosConfigV1.iReqGuaranteedRate.iDownlinkRate != DPCKTTSY_GRNT_DOWNLINK_BITRATE_REQ))
				{	
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				proceed--;		// The method should return with KErrNone after calling ReqCompleted().
				}
			}
			break;
		case TPacketDataConfigBase::KConfigRel5 : 
			{
			// If R5, validate the descriptor and complete the request
			TPckg<RPacketQoS::TQoSR5Requested>* qosConfigR5Pckg = (TPckg<RPacketQoS::TQoSR5Requested>*)aConfig;
			RPacketQoS::TQoSR5Requested& qosConfigV1 = (*qosConfigR5Pckg)();
			
			if ((!qosConfigV1.iSignallingIndication)||
				(qosConfigV1.iSourceStatisticsDescriptor != DPCKTTSY_SRC_STATSDESC))
				{	
				ReqCompleted(aTsyReqHandle,KErrCorrupt);
				proceed--;		// The method should return with KErrNone after calling ReqCompleted().
				}
			}
			break;
		default:
			ReqCompleted(aTsyReqHandle,KErrNotSupported);
			proceed--;		// The method should return with KErrNone after calling ReqCompleted().
			break;
		}	// switch
	
	// If the data is corrupt, proceed should be zero, and the method completes immediately.
	if (!proceed)
		return KErrNone;
	else				// Data is NOT corrupt, continue the implementation
		{
		if(!iSetProfile++)
			ReqCompleted(aTsyReqHandle,KErrNone);
		switch(iSetProfile)
			{
			case 3:
				ReqCompleted(aTsyReqHandle,KErrNone);
				break;
				
			case 4:
				break;
			}
		iTsyAsyncReqHandle = aTsyReqHandle;
		return KErrNone;
		}
	}

TInt CGprsQoSDGprsTsy::SetProfileCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy::SetProfileCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsQoSDGprsTsy::GetProfile(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig)
/** 
 * Get QoS Profile parameters
 * This method is called by both the Hurricane and JetStream API tests.
 * The iExtensionId parameter is checked to determine the packet service.
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy::GetProfile called"));

	User::After(300000); // wait to have KRequestPending
	if(!iGetProfile++)
		{
		TPacketDataConfigBase& aCheckId = (*(TPckg<TPacketDataConfigBase>*) aConfig)();

		// CDMA QoS profiles are currently not supported in this Dummy TSY.
		if (aCheckId.ExtensionId() == TPacketDataConfigBase::KConfigCDMA)
			ReqCompleted(aTsyReqHandle,KErrNotSupported);

		// If GPRS Rel97/98, unpack the descriptor, get the QoS profile parameters 
		// and complete the request
		if (aCheckId.ExtensionId() == TPacketDataConfigBase::KConfigGPRS)
			{		
			TPckg<RPacketQoS::TQoSGPRSNegotiated>* qosConfigGPRSPckg = (TPckg<RPacketQoS::TQoSGPRSNegotiated>*)aConfig;
			RPacketQoS::TQoSGPRSNegotiated& qosConfigV1 = (*qosConfigGPRSPckg)();

			qosConfigV1.iDelay = DPCKTTSY_DELAY_NEG2;
			qosConfigV1.iMeanThroughput = DPCKTTSY_MEAN_THROUGHPUT_NEG2;
			qosConfigV1.iPeakThroughput = DPCKTTSY_PEAK_THROUGHPUT_NEG2;
			qosConfigV1.iPrecedence = DPCKTTSY_PRECEDENCE_NEG2;
			qosConfigV1.iReliability = DPCKTTSY_RELIABILITY_NEG2;

			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (aCheckId.ExtensionId() == TPacketDataConfigBase::KConfigRel99Rel4) 
			{
			// If Rel99/Rel4 network, unpack the descriptor, set the QoS profile parameters 
			// and complete the request
			TPckg<RPacketQoS::TQoSR99_R4Negotiated>* qosConfigR99R4Pckg = (TPckg<RPacketQoS::TQoSR99_R4Negotiated>*)aConfig;
			RPacketQoS::TQoSR99_R4Negotiated& qosConfigV1 = (*qosConfigR99R4Pckg)();

			qosConfigV1.iTrafficClass = DPCKTTSY_TRAFFIC_CLASS_NEG;
			qosConfigV1.iDeliveryOrderReqd = DPCKTTSY_DELIVERY_ORDER_NEG;
			qosConfigV1.iDeliverErroneousSDU = DPCKTTSY_ERRONEOUS_SDU_DELIVERY_NEG;
			qosConfigV1.iMaxSDUSize = DPCKTTSY_MAX_SDU_SIZE_NEG;
			qosConfigV1.iMaxRate.iUplinkRate = DPCKTTSY_MAX_UPLINK_BITRATE_NEG;
			qosConfigV1.iMaxRate.iDownlinkRate = DPCKTTSY_MAX_DOWNLINK_BITRATE_NEG;
			qosConfigV1.iBER = DPCKTTSY_BER_NEG;
			qosConfigV1.iSDUErrorRatio = DPCKTTSY_SDU_ERROR_RATIO_NEG;
			qosConfigV1.iTransferDelay = DPCKTTSY_TRANSFER_DELAY_NEG;
			qosConfigV1.iGuaranteedRate.iUplinkRate = DPCKTTSY_GRNT_UPLINK_BITRATE_NEG;
			qosConfigV1.iGuaranteedRate.iDownlinkRate = DPCKTTSY_GRNT_DOWNLINK_BITRATE_NEG;
			
			ReqCompleted(aTsyReqHandle,KErrNone);
			}		
		}	// !iGetProfile++
		iTsyAsyncReqHandle = aTsyReqHandle;
		return KErrNone;
	}

TInt CGprsQoSDGprsTsy::GetProfileCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy::GetProfileCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}

TInt CGprsQoSDGprsTsy::GetProfileCaps(const TTsyReqHandle aTsyReqHandle,TDes8* aConfig)
/** 
 * Get Profile capabilities.
 * This method is called by both the Hurricane and JetStream API tests.
 * The iExtensionId parameter is checked to determine the packet service.
 */
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy::GetProfileCaps called"));

	User::After(300000); // wait to have KRequestPending
	if(!iGetProfileCaps++)
		{
		TPacketDataConfigBase& aCheckId = (*(TPckg<TPacketDataConfigBase>*) aConfig)();

		// CDMA QoS profiles are currently not supported in this Dummy TSY.
		if (aCheckId.ExtensionId() == TPacketDataConfigBase::KConfigCDMA)
			ReqCompleted(aTsyReqHandle,KErrNotSupported);

		// If GPRS Rel97/98, unpack the descriptor, set the QoS profile caps 
		// and complete the request
		if (aCheckId.ExtensionId() == TPacketDataConfigBase::KConfigGPRS)
			{		
			TPckg<RPacketQoS::TQoSCapsGPRS>* qosCapsGPRSPckg = (TPckg<RPacketQoS::TQoSCapsGPRS>*)aConfig;
			RPacketQoS::TQoSCapsGPRS& qosCaps = (*qosCapsGPRSPckg)();

			qosCaps.iDelay = DPCKTTSY_DELAY;
			qosCaps.iMean = DPCKTTSY_MEAN_THROUGHPUT;
			qosCaps.iPeak = DPCKTTSY_PEAK_THROUGHPUT;
			qosCaps.iPrecedence = DPCKTTSY_PRECEDENCE;
			qosCaps.iReliability = DPCKTTSY_RELIABILITY;

			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (aCheckId.ExtensionId() == TPacketDataConfigBase::KConfigRel99Rel4) 
			{
			// If Rel99/Rel4 network, unpack the descriptor, set the QoS profile parameters 
			// and complete the request
			TPckg<RPacketQoS::TQoSCapsR99_R4>* qosCapsR99R4Pckg = (TPckg<RPacketQoS::TQoSCapsR99_R4>*)aConfig;
			RPacketQoS::TQoSCapsR99_R4& qosCaps = (*qosCapsR99R4Pckg)();

			qosCaps.iTrafficClass = DPCKTTSY_TRAFFIC_CLASS;
			qosCaps.iDeliveryOrderReqd = DPCKTTSY_DELIVERY_ORDER;
			qosCaps.iDeliverErroneousSDU = DPCKTTSY_ERRONEOUS_SDU_DELIVERY;
			qosCaps.iBER = DPCKTTSY_BER;
			qosCaps.iSDUErrorRatio = DPCKTTSY_SDU_ERROR_RATIO;
			qosCaps.iTrafficHandlingPriority = DPCKTTSY_TRFFC_HNDLG_PRIORITY;

			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}	// !iGetProfileCaps++
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}

TInt CGprsQoSDGprsTsy::GetProfileCapsCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy::GetProfileCapsCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}


TInt CGprsQoSDGprsTsy::NotifyProfileChanged(const TTsyReqHandle aTsyReqHandle, TDes8* aConfig)
/** 
 * This method is called by both the Hurricane API tests and the JetStream tests.
 * The iExtensionId parameter is checked to determine the packet service.
 */
	{
	User::After(300000); // wait to have KRequestPending
	TPacketDataConfigBase& aCheckId = (*(TPckg<TPacketDataConfigBase>*) aConfig)();
	
	if (!iNotifyProfileChanged++)
		{
		LOGTEXT(_L8("CGprsQoSDGprsTsy::NotifyProfileChanged called"));

		// CDMA QoS profiles are currently not supported in this Dummy TSY.
		if (aCheckId.ExtensionId() == TPacketDataConfigBase::KConfigCDMA)
			ReqCompleted(aTsyReqHandle,KErrNotSupported);

		// If GPRS Rel97/98, unpack the descriptor, set the QoS profile parameters 
		// and complete the request
		if (aCheckId.ExtensionId() == TPacketDataConfigBase::KConfigGPRS)
			{
			TPckg<RPacketQoS::TQoSGPRSNegotiated>* qosNegotiatedGPRSPckg = (TPckg<RPacketQoS::TQoSGPRSNegotiated>*)aConfig;
			RPacketQoS::TQoSGPRSNegotiated& qosNotify = (*qosNegotiatedGPRSPckg)();

			qosNotify.iDelay = DPCKTTSY_DELAY_MIN1;
			qosNotify.iMeanThroughput = DPCKTTSY_MEAN_THROUGHPUT_MIN1;
			qosNotify.iPeakThroughput = DPCKTTSY_PEAK_THROUGHPUT_MIN1;
			qosNotify.iPrecedence = DPCKTTSY_PRECEDENCE_MIN1;
			qosNotify.iReliability = DPCKTTSY_RELIABILITY_MIN1;

			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		else if (aCheckId.ExtensionId() == TPacketDataConfigBase::KConfigRel99Rel4) 
			{
			// If Rel99/Rel4 network, unpack the descriptor, set the QoS profile parameters 
			// and complete the request
			TPckg<RPacketQoS::TQoSR99_R4Negotiated>* qosNegotiatedR99_R4Pckg = (TPckg<RPacketQoS::TQoSR99_R4Negotiated>*)aConfig;
			RPacketQoS::TQoSR99_R4Negotiated& qosNotifyR99_R4 = (*qosNegotiatedR99_R4Pckg)();

			qosNotifyR99_R4.iTrafficClass = DPCKTTSY_TRAFFIC_CLASS_MIN;
			qosNotifyR99_R4.iDeliveryOrderReqd = DPCKTTSY_DELIVERY_ORDER_MIN;
			qosNotifyR99_R4.iDeliverErroneousSDU = DPCKTTSY_ERRONEOUS_SDU_DELIVERY_MIN;
			qosNotifyR99_R4.iMaxSDUSize = DPCKTTSY_MAX_SDU_SIZE_MIN;
			qosNotifyR99_R4.iMaxRate.iUplinkRate = DPCKTTSY_MAX_UPLINK_BITRATE_MIN;
			qosNotifyR99_R4.iMaxRate.iDownlinkRate = DPCKTTSY_MAX_DOWNLINK_BITRATE_MIN;
			qosNotifyR99_R4.iBER = DPCKTTSY_BER_MAX;
			qosNotifyR99_R4.iSDUErrorRatio = DPCKTTSY_SDU_ERROR_RATIO_MAX;
			qosNotifyR99_R4.iTrafficHandlingPriority = DPCKTTSY_TRFFC_HNDLG_PRIORITY_MIN;
			qosNotifyR99_R4.iTransferDelay = DPCKTTSY_TRANSFER_DELAY_MAX;
			qosNotifyR99_R4.iGuaranteedRate.iUplinkRate = DPCKTTSY_GRNT_UPLINK_BITRATE_MIN;
			qosNotifyR99_R4.iGuaranteedRate.iDownlinkRate = DPCKTTSY_GRNT_DOWNLINK_BITRATE_MIN;
			
			ReqCompleted(aTsyReqHandle,KErrNone);
			}
		}	// !iNotifyProfileChanged++
			
	if (aCheckId.ExtensionId() == TPacketDataConfigBase::KConfigRel5)
		{
		TPckg<RPacketQoS::TQoSR5Negotiated>* qosNegotiatedR5Pckg = (TPckg<RPacketQoS::TQoSR5Negotiated>*)aConfig;
		RPacketQoS::TQoSR5Negotiated& qosNotifyR5 = (*qosNegotiatedR5Pckg)();
		switch(iNotifyProfileChanged)
			{
			case 3:
				qosNotifyR5.iSignallingIndication = DPCKTTSY_SIG_IND;
				qosNotifyR5.iSourceStatisticsDescriptor = DPCKTTSY_SRC_STATSDESC;
					
				ReqCompleted(aTsyReqHandle,KErrNone);
				break;
			
			case 4:
				break;
				
			default:
				ReqCompleted(aTsyReqHandle,KErrArgument);
				break;
			}
		}
			
	iTsyAsyncReqHandle = aTsyReqHandle;
	return KErrNone;
	}


TInt CGprsQoSDGprsTsy::NotifyProfileChangedCancel(const TTsyReqHandle aTsyReqHandle)
	{
	LOGTEXT(_L8("CGprsQoSDGprsTsy::NotifyProfileChangedCancel called"));
	User::After(300000); // wait to have KRequestPending
	if(aTsyReqHandle == iTsyAsyncReqHandle)
		ReqCompleted(aTsyReqHandle,KErrCancel);
	else
		ReqCompleted(aTsyReqHandle,KErrCorrupt);
	return KErrNone;
	}
