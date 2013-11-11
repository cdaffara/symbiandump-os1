// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CLtsySatHandler

#include <ctsy/ltsy/cctsydispatchercallback.h>
#include <mmtsy_ipcdefs.h>
#include <satcs.h>



#include "cltsysathandler.h"
#include <ctsy/ltsy/ltsylogger.h>
#include "ltsymacros.h"



CLtsySatHandler::CLtsySatHandler(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	: iCtsyDispatcherCallback(aCtsyDispatcherCallback)
	{
	}

	
CLtsySatHandler::~CLtsySatHandler()
	{
	}


CLtsySatHandler* CLtsySatHandler::NewLC(
	CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsySatHandler* self = new (ELeave) CLtsySatHandler(aCtsyDispatcherCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CLtsySatHandler* CLtsySatHandler::NewL(CCtsyDispatcherCallback& aCtsyDispatcherCallback)
	{
	TSYLOGENTRYEXIT;
	CLtsySatHandler* self=
			CLtsySatHandler::NewLC(aCtsyDispatcherCallback);
	CleanupStack::Pop(self);
	return self;
	}


void CLtsySatHandler::ConstructL()
/**
 * Second phase constructor.
 */
	{
	TSYLOGENTRYEXIT;
	
	
	} // CLtsySatHandler::ConstructL

TBool CLtsySatHandler::IsInterfaceSupported(TLtsyDispatchInterfaceApiId aDispatchApiId)
/**
 * Returns whether a Dispatch Interface APi is supported or not.
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 * 
 * @param aDispatchApiId Id of Dispatch interface being queried
 * @return indication whether interface is supported or not
 */
	{

	switch(aDispatchApiId)
		{
		// Insert ApiIds when support is provided in LTSY e.g.
		// case MLtsyDispatchPhoneBootNotifySimStatusReady::KLtsyDispatchPhoneBootNotifySimStatusReadyApiId: return ETrue;
		
		default:
			return EFalse;
		}
	}

void CLtsySatHandler::IsCallbackIndicatorSupported(TLtsyDispatchIndIdGroup /*aIdGroup*/, TUint32& /*aIndIdBitMask*/)
/**
 * Returns the set of callback indicators that are supported.
 * 
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *  
 * @param aIdGroup ID of group that is being queried e.g. aIdGroup=EIndIdGroup1
 * @param aIndIdBitMask [out] argument that should return a bitmask indicating which indicator callbacks are supported.
 */
	{

	//Create bitmask with bitset for each indicator ID that is supported. E.g.
	/*
	if(aIdGroup == EIndIdGroup1)
		aIndIdBitMask = KLtsyDispatchPhoneNotifyNetworkModeChangeIndId | 
						KLtsyDispatchPhoneNotifyEgprsInfoChangeIndId | 
						KLtsyDispatchPhoneNotifySignalStrengthChangeIndId;
	else
		aIndIdBitMask = 0; //no indicators from other groups supported
	*/
	}


TInt CLtsySatHandler::HandleSmsDeliverReportReqL(const TDesC8& /*aSmsDeliverReportTpdu*/)
/**
 * This request has no associated callback.
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aSmsDeliverReportTpdu The SMS-DELIVER-REPORT TPDU element to be contained in a 
 * RP-ERROR-PDU (negative acknowledgement) or RP-ACK-PDU (a positive acknowledgement) to a
 * SMS DELIVER or SMS STATUS REPORT.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleSmsDeliverReportReqL


TInt CLtsySatHandler::HandleGetImageInstanceReqL(TUint8 /*aEfImgRecordNumber*/, TUint8 /*aInstanceNumberInEfImgRecord*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatGetImageInstanceComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 * 
 * @param aEfImgRecordNumber 			The record number of the icons EF(Img) record for the 
 * 										icon image instance being retrieved. 
 * @param aInstanceNumberInEfImgRecord 	The record number of the icon instance, in the icons EF(Img) record
 * 										for the icon image instance being retrieved. 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;    
	//TSYLOGENTRYEXITARGS(_L8("aEfImgRecordNumber=%d, aInstanceNumberInEfImgRecord=%d"), aEfImgRecordNumber, aInstanceNumberInEfImgRecord);

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleGetImageInstanceReqL

TInt CLtsySatHandler::HandleGetIconDataReqL(TUint8 /*aEfImgRecordNumber*/, const TDesC8& /*aSimFilePath*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatGetIconDataComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aEfImgRecordNumber 	EF(img) record number of the icon to retrieve. 
 * @param aSimFilePath 			The full file path to the EF(Img). 
 * 								Bytes 	0-1 MF 			(0x3F00)
 * 								Bytes	2-3 DF(Telecom) (0x7F10) 
 * 								Bytes	4-5 DF(Graphics)(0x5F50)
 * 								Bytes	6-7 EF(Ef(Img)) 	(0x4F20)
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	//TSYLOGENTRYEXITARGS(_L8("aEfImgRecordNumber=%d"), aEfImgRecordNumber);

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleGetIconDataReqL


TInt CLtsySatHandler::HandleGetClutReqL(TUint8 /*aEfImgRecordNumber*/, TUint8 /*aInstanceNumberInEfImgRecord*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatGetClutComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aEfImgRecordNumber 			The record number of the icons EF(Img) record for the 
 * 										CLUT being retrieved. 
 * @param aInstanceNumberInEfImgRecord 	The record number of the icon instance, in the icons EF(Img) record
 * 										for the CLUT being retrieved. 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	//TSYLOGENTRYEXITARGS(_L8("aEfImgRecordNumber=%d, aInstanceNumberInEfImgRecord=%d"), aEfImgRecordNumber, aInstanceNumberInEfImgRecord);

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleGetClutReqL


TInt CLtsySatHandler::HandleGetDefaultBearerCapabilityReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatGetDefaultBearerCapabilityComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......

	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleGetDefaultBearerCapabilityReqL


TInt CLtsySatHandler::HandleGetSmsPpDownloadSupportedReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatGetSmsPpDownloadSupportedComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......

	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleGetSmsPpDownloadSupportedReqL


TInt CLtsySatHandler::HandleGetSmsControlActivatedReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatGetSmsControlActivatedComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	
	// TODO: Add implementation here.......

	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleGetSmsControlActivatedReqL


TInt CLtsySatHandler::HandleCellBroadcastEnvelopeReqL(const TDesC8& /*aEnvelope*/)
/**
 * This request has no associated callback.
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aEnvelope BER-TLV encoded data containing an (CELL BROADCAST DOWNLOAD) ENVELOPE command.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......

	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleCellBroadcastEnvelopeReqL

TInt CLtsySatHandler::HandleGetAccessTechnologyReqL()
/**
* This request is completed by invoking
* CCtsyDispatcherCallback::CallbackSatGetAccessTechnologyComp
*
* This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
*
* @return KErrNone on success, KErrNotSupported if this request is not supported,
* or another error code to indicate the failure otherwise.
*/
	{
	TSYLOGENTRYEXIT;
	
	TInt ret = KErrNotSupported;
	
	// TODO: Add implementation here.......
	

	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleGetAccessTechnologyReqL

TInt CLtsySatHandler::HandleTimingAdvanceReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatTimingAdvanceComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleTimingAdvanceReqL


TInt CLtsySatHandler::HandleSmControlResponseDataReqL(TUint8 /*aResult*/, const TDesC8& /*aRpDestinationAddress*/, const TDesC8& /*aTpDestinationAddress*/)
/**
 * This request has no associated callback.
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aResult MO short message control result the SIM gives to the ME concerning whether to 
 * allow, bar or modify the proposed short message, encoded as defined in "3GPP TS 11.10-4, 27.22.8 MO SHORT MESSAGE CONTROL BY SIM"
 * @param aRpDestinationAddress RP Destination Address of the Service Centre to which the ME is proposing to send the short message.
 * If the address data is not present (zero length), then assume the RP Destination Address of the Service Centre is not to be modified.
 * @param aTpDestinationAddress TP Destination Address to which the ME is proposing to send the short message.
 * If the address data is not present (zero length), then assume the TP Destination Address is not to be modified.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleSmControlResponseDataReqL


TInt CLtsySatHandler::HandleProvideLocationInfoReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatProvideLocationInfoComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleProvideLocationInfoReqL


TInt CLtsySatHandler::HandleSetPollingIntervalReqL(TUint8 /*aPollingInterval*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatSetPollingIntervalComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aPollingInterval The polling interval in seconds indicating how often the 
 * terminal sends STATUS commands related to Proactive Polling 
 * (zero indicates Polling should be turned off)
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleSetPollingIntervalReqL


TInt CLtsySatHandler::HandleSmsPpDownloadEnvelopeReqL(const TDesC8& /*aEnvelope*/)
/**
 * This request has no associated callback.
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aEnvelope BER-TLV encoded data containing an (SMS-PP DOWNLOAD) ENVELOPE command.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleSmsPpDownloadEnvelopeReqL


TInt CLtsySatHandler::HandleLocalInformationNmrReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatLocalInformationNmrComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;
	
	// TODO: Add implementation here.......

	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleLocalInformationNmrReqL


TInt CLtsySatHandler::HandleGetUssdControlSupportedReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatGetUssdControlSupportedComp
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleGetUssdControlSupportedReqL


TInt CLtsySatHandler::HandleCallAndSmsControlEnvelopeReqL(const TDesC8& /*aEnvelope*/)
/**
 * This request has no associated callback.
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aEnvelope BER-TLV encoded data containing a Call Control or MO SMS control by SIM ENVELOPE command
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleCallAndSmsControlEnvelopeReqL


TInt CLtsySatHandler::HandleRefreshAllowedReqL(const TDesC8& /*aRefreshFileList*/, const TDesC8& /*aAid*/, TUint16 /*aTsyCache*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatRefreshAllowed
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aRefreshFileList The list of files (full path) that have been changed by the UICC (see ETSI 102 221 or ETSI 151 011) 
 * @param aAid Indicates the application on the UICC to be refreshed (as defined ETSI TS 101 220) . 
 * @param aTsyCache Bit field of files that are cached in SIM ATK TSY (examine using CSatNotificationsTsy::TCachedSimAtkFiles) 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleRefreshAllowedReqL


TInt CLtsySatHandler::HandleReadyReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatReady
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * A call to this function indicates that the CTSY is ready to receive proactive commands from the LTSY. 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleReadyReqL


TInt CLtsySatHandler::HandlePCmdNotificationReqL(TUint8 /*aPCmdType*/)
/**
 * This request has no associated callback.
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aPCmdType The number of the proactive command for which notification is being requested (e.g. KSetUpMenu),
 *  as defined in ETSI 102 223 etc.
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;
	
	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandlePCmdNotificationReqL


TInt CLtsySatHandler::HandleUssdControlEnvelopeErrorReqL()
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatUssdControlEnvelopeError
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......


	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleUssdControlEnvelopeErrorReqL


TInt CLtsySatHandler::HandleTimerExpirationEnvelopeReqL(const TDesC8& /*aEnvelope*/)
/**
 * This request has no associated callback.
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aEnvelope BER-TLV encoded Timer expiration ENVELOPE command
 * see "ETSI TS 102 223" under "ENVELOPE commands" "Timer expiration" 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleTimerExpirationEnvelopeReqL


TInt CLtsySatHandler::HandleTerminalRspReqL(const TDesC8& /*aTerminalRsp*/)
/**
 * This request is completed by invoking
 * CCtsyDispatcherCallback::CallbackSatTerminalRsp
 * 
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 * 
 * @param aTerminalRsp The terminal response TLV as formatted in "ETSI TS 102 223" Under "Structure of TERMINAL RESPONSE"
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleTerminalRspReqL


TInt CLtsySatHandler::HandleMenuSelectionEnvelopeReqL(const TDesC8& /*aEnvelope*/)
/**
 * This request has no associated callback.
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aEnvelope BER-TLV encoded Envelope comprising a notification from
 * the ME that the user has either requested help about, or has selected a menu item
 * that was previously provided by the SIM application via the SETUP MENU proactive
 * command. 
 *
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleMenuSelectionEnvelopeReqL


TInt CLtsySatHandler::HandleEventDownloadEnvelopeReqL(const TDesC8& /*aEnvelope*/)
/**
 * This request has no associated callback.
 *
 * This API is optional and should only be used if the licensee wishes to including the Dispatcher beneath their own SIM ATK TSY.
 *
 * @param aEnvelope BER-TLV encoded Event Download ENVELOPE command
 * see "ETSI TS 102 223" under "ENVELOPE commands" "Event Download" 
 * 
 * @return KErrNone on success, KErrNotSupported if this request is not supported,
 * or another error code to indicate the failure otherwise.
 */
	{
	TSYLOGENTRYEXIT;

	TInt ret = KErrNotSupported;

	// TODO: Add implementation here.......



	return TSYLOGSETEXITERR(ret);
	} // CLtsySatHandler::HandleEventDownloadSyncL





