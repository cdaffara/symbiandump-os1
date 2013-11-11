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
// This file contains all the interfaces classes that can be implemented by
// the Licensee LTSY relating to Sat related features.
//




/**
 @file
 @internalAll 
*/


#ifndef MLTSYDISPATCHSATINTERFACE_H_
#define MLTSYDISPATCHSATINTERFACE_H_

#include <ctsy/ltsy/mltsydispatchinterface.h>

namespace DispatcherSat
	{
	
	/**
	 * This namespace contains all types relating to the SAT dispatcher.
	 */
	
	/**
	 * Enum containing the SS status
	 */
	enum TSsStatus
		{
		ENotBusy,
		ESsBusy,
		EUssdBusy
		};
	
	}

class MLtsyDispatchSatSmsDeliverReport : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatSmsDeliverReportApiId = KDispatchSatFuncUnitId + 1;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsySmsDeliverReport
	 * request from the CTSY.
	 * 
	 * 
	 * It is a request call that does not have an associated complete callback.
	 *
	 * Implementation of this interface should handle the requested SMS DELIVER REPORT Tpdu as
	 * a result of the CCtsyDispatcherCallback::CallbackSatCreateSmsDeliverReportInd()
	 * 
	 * An SMS DELIVER REPORT TPDU is carried as a RP User Data element within an RP ERROR PDU 
	 * and is part of the negative acknowledgement to an SMS DELIVER or SMS STATUS REPORT.
	 * 
	 * An SMS DELIVER REPORT TPDU is also carried as a RP User Data element within an RP ACK PDU 
	 * and is part of a positive acknowledgement to a SMS DELIVER or SMS STATUS REPORT.
	 * 
	 *
	 * @param aSmsDeliverReportTpdu The SMS-DELIVER-REPORT TPDU element to be contained in a 
	 * RP-ERROR-PDU (negative acknowledgement) or RP-ACK-PDU (a positive acknowledgement) to a
	 * SMS DELIVER or SMS STATUS REPORT
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSmsDeliverReportReqL(const TDesC8& aSmsDeliverReportTpdu) = 0;

	}; // class MLtsyDispatchSatSmsDeliverReport



class MLtsyDispatchSatGetImageInstance : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatGetImageInstanceApiId = KDispatchSatFuncUnitId + 2;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyGetImageInstance
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatGetImageInstanceComp()
	 * 
	 * Implementation of this interface should retrieve an icon's image data from the UICC 
	 * designated by aEfImgRecordNumber & aInstanceNumberInEfImgRecord
	 *
	 * @param aEfImgRecordNumber 			The record number of the icons EF(img) record for the 
	 * 										icon image instance being retrieved. 
	 * @param aInstanceNumberInEfImgRecord 	The record number of the icon instance, in the icons EF(img) record
	 * 										for the icon image instance being retrieved. 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetImageInstanceReqL(TUint8 aEfImgRecordNumber, TUint8 aInstanceNumberInEfImgRecord) = 0;

	}; // class MLtsyDispatchSatGetImageInstance


class MLtsyDispatchSatGetIconData : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatGetIconDataApiId = KDispatchSatFuncUnitId + 3;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyGetIconData
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatGetIconDataComp()
	 * 
	 * Implementation of this interface should retrieve an icon's EF record from the UICC based
	 * on the record number (see ETSI TS 131 102 Under EFIMG (Image) & Files of USIM)
	 *
	 * @param aEfImgRecordNumber 	EF(img) record number of the icon to retrieve 
	 * @param aSimFilePath 			The full file path to the EF(Img) 
	 * 								Bytes 	0-1 MF 			(0x3F00)
	 * 								Bytes	2-3 DF(Telecom) (0x7F10) 
	 * 								Bytes	4-5 DF(Graphics)(0x5F50)
	 * 								Bytes	6-7 EF(Ef(Img)) 	(0x4F20)
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetIconDataReqL(TUint8 aEfImgRecordNumber, const TDesC8& aSimFilePath) = 0;

	}; // class MLtsyDispatchSatGetIconData



class MLtsyDispatchSatGetClut : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatGetClutApiId = KDispatchSatFuncUnitId + 4;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyGetClut
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatGetClutComp()
	 * 
	 * Implementation of this interface should retrieve the CLUT (Colour Lookup Table) for an image instance 
	 * designated by aEfImgRecordNumber & aInstanceNumberInEfImgRecord
	 *
	 * @param aEfImgRecordNumber 			The record number of the icons EF(Img) record for the 
	 * 										CLUT being retrieved.
	 * @param aInstanceNumberInEfImgRecord 	The record number of the icon instance, in the icons EF(Img) record
	 * 										for the CLUT being retrieved.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetClutReqL(TUint8 aEfImgRecordNumber, TUint8 aInstanceNumberInEfImgRecord) = 0;

	}; // class MLtsyDispatchSatGetClut



class MLtsyDispatchSatGetDefaultBearerCapability : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatGetDefaultBearerCapabilityApiId = KDispatchSatFuncUnitId + 5;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyGetBearerCapability
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatGetDefaultBearerCapabilityComp()
	 * 
	 * Implementation of this interface should retrieve the current default bearer capability configuration.
	 * 
	 * The default bearer capability configuration (coding as defined by the access technologies) will be used 
	 * by the CTSY when creating a Call Control ENVELOPE (the capability configuration parameters) if a zero length 
	 * capability parameters is passed when creating the Envelope. 
	 * See "ETSI TS 102 223" "Structure of ENVELOPE (CALL CONTROL)"
	 * 
	 * @see MLtsyDispatchSatCallAndSmsControlEnvelope::HandleCallAndSmsControlEnvelopeReqL()
	 * @see CCtsyDispatcherCallback::CallbackSatCallControlEnvelopeResponseInd()
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetDefaultBearerCapabilityReqL() = 0;

	}; // class MLtsyDispatchSatGetDefaultBearerCapability



class MLtsyDispatchSatGetSmsPpDownloadSupported : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatGetSmsPpDownloadSupportedApiId = KDispatchSatFuncUnitId + 6;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsySmsPpDdlStatus
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatGetSmsPpDownloadSupportedComp()
	 * 
	 * Implementation of this interface should retrieve if the service 
	 * "data download via SMS Point-to-point" is allocated and activated in the SIM Service Table
	 * and return the result via CCtsyDispatcherCallback::CallbackSatGetSmsPpDownloadSupportedComp()
	 * 
	 * @see 3GPP TS 11.14 section 7.1
	 * @see 3GPP TS 11.11 section 11.6.12
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetSmsPpDownloadSupportedReqL() = 0;

	}; // class MLtsyDispatchSatGetSmsPpDownloadSupported



class MLtsyDispatchSatGetSmsControlActivated : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatGetSmsControlActivatedApiId = KDispatchSatFuncUnitId + 7;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyMoSmsControlActivation
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatGetSmsControlActivatedComp()
	 * 
	 * Implementation of this interface should retrieve the current activation state of 
	 * MO-SMS Control by USIM (Service n°31 in EFust).
	 * 
	 * @see 3GPP TS 11.11 section 11.6.16
	 * @see 3GPP TS 31.102 section 4.2.8
	 * 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetSmsControlActivatedReqL() = 0;

	}; // class MLtsyDispatchSatGetSmsControlActivated

class MLtsyDispatchSatGetAccessTechnology : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatGetAccessTechnologyApiId = KDispatchSatFuncUnitId + 8;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyAccTech
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatGetAccessTechnologyComp()
	 * 
	 * Implementation of this interface should cause the current access technology to be
	 * sent up to the CTSY via the CallbackSatGetAccessTechnologyComp() callback.
	 * 
	 * This request is generated when a LOCAL INFORMATION (access technology)
	 * proactive command is received from UICC.
	 * 
	 *
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetAccessTechnologyReqL() = 0;

	}; // class MLtsyDispatchSatGetAccessTechnology

class MLtsyDispatchSatCellBroadcastEnvelope : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatCellBroadcastEnvelopeApiId = KDispatchSatFuncUnitId + 9;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyCellBroadcast
	 * request from the CTSY.
	 * 
	 * It is a request call that does not have an associated complete callback.
	 * 
	 * 
	 * Implementation of this interface should hanldle Cell Broadcast Envelope commands.
	 * This ENVELOPE is created as a result of a CallbackSatCreateCellBroadcastEnvelopeInd() call.
	 *
	 * Once the ME receives a new Cell Broadcast message and the service, if "data download via SMS-CB" is 
	 * allocated and activated in the SIM Service Table and if the message identifier of the Cell Broadcast 
	 * is contained within the message identifiers of the EF(CBMID), the the cell broadcast page is passed 
	 * to the SIM using the ENVELOPE (CELL BROADCAST DOWNLOAD) command.
	 * 
	 * See "3GPP TS 11.14" Under "Cell Broadcast data download"
	 * 
	 *
	 * @param aEnvelope BER-TLV encoded data containing an (CELL BROADCAST DOWNLOAD) ENVELOPE command. 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleCellBroadcastEnvelopeReqL(const TDesC8& aEnvelope) = 0;

	}; // class MLtsyDispatchSatCellBroadcastEnvelope



class MLtsyDispatchSatTimingAdvance : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatTimingAdvanceApiId = KDispatchSatFuncUnitId + 10;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyTimingAdvance
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatTimingAdvanceComp()
	 * 
	 * Implementation of this interface should ascertain the Timing Advance measurement from the GSM/3G
	 * protocol. It can then be provided to the SIM application in response to a PROVIDE LOCAL INFO 
	 * proactive command.
	 * 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleTimingAdvanceReqL() = 0;

	}; // class MLtsyDispatchSatTimingAdvance



class MLtsyDispatchSatSmControlResponseData : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatSmControlResponseDataApiId = KDispatchSatFuncUnitId + 11;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatNotifyMoSmControlRequest
	 * request from the CTSY.
	 * 
	 * It is a request call that does not have an associated complete callback.
	 * 
	 * Implementation of this interface should handle the parsed Mo Short Message Control response TLV data.
	 * 
	 * As a result of the ENVELOPE (MO SHORT MESSAGE CONTROL) command sent to the SIM, the SIM will respond
	 * with a response TLV, this is can be passed to the LTSY via the 
	 * CCtsyDispatcherCallback::CallbackSatSmControlEnvelopeResponseInd().
	 * The CTSY parses this TLV, completes the RSat::NotifySmControlRequest() on the client side and passes
	 * down the parsed data to the LTSY via this handler.
	 * 
	 * See "3GPP 11.14 - MO Short Message Control by SIM"
	 *
	 * @param aResult MO short message control result the SIM gives to the ME concerning whether to 
	 * allow, bar or modify the proposed short message, encoded as defined in "3GPP TS 11.10-4, 27.22.8 MO SHORT MESSAGE CONTROL BY SIM" 
	 * @param aRpDestinationAddress RP Destination Address of the Service Center to which the ME is proposing to send the short message.
	 * If the address data is not present (zero length), then assume the RP Destination Address of the Service Center is not to be modified. 
	 * @param aTpDestinationAddress TP Destination Address to which the ME is proposing to send the short message.
	 * If the address data is not present (zero length), then assume the TP Destination Address is not to be modified. 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSmControlResponseDataReqL(
			TUint8 aResult, const TDesC8& aRpDestinationAddress, const TDesC8& aTpDestinationAddress) = 0;

	}; // class MLtsyDispatchSatSmControlResponseData



class MLtsyDispatchSatProvideLocationInfo : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatProvideLocationInfoApiId = KDispatchSatFuncUnitId + 12;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyProvideLocalInfo
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatProvideLocationInfoComp()
	 * 
	 * Implementation of this interface should cause location information data to be provided, which can then be used
	 * for Call/SS/USSD Control.
	 * 
	 * See "3GPP TS 11.14" section 9.1.1
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleProvideLocationInfoReqL() = 0;

	}; // class MLtsyDispatchSatProvideLocationInfo



class MLtsyDispatchSatSetPollingInterval : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatSetPollingIntervalApiId = KDispatchSatFuncUnitId + 13;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsySetPolling
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatSetPollingIntervalComp()
	 * 
	 * Implementation of this interface should attempt to update the polling interval in the LTSY.
	 * 
	 * A Proactive command from the UICC indicates the poll interval it requests from then onwards.
	 * The CTSY then notifies the LTSY the maximum poll interval it should use in seconds 
	 * (via a HandleSetPollingIntervalReqL()), the LTSY completes the request (via a CallbackSatSetPollingIntervalComp())
	 * indicating the interval that will be used. If successful the CTSY then sends a terminal response, in response 
	 * to the original proactive command. If unsuccessful and polling is currently off a terminal response is sent
	 * with a result of Unable To Process Command, in the case where polling is currently on the request to the LTSY is 
	 * re-attempted with a legacy polling interval of 25 seconds.
	 * 
	 * The polling interval ranges from a minimum of 5 and maximum of 255 seconds, 
	 * a polling interval of zero (0) indiates that polling should be turned off.
	 * 
	 * See "ETSI TS 102 223" "POLL INTERVAL"
	 *
	 * @param aPollingInterval The polling interval in seconds indicating how often the 
	 * terminal sends STATUS commands related to Proactive Polling 
	 * (zero indicates Polling should be turned off) 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSetPollingIntervalReqL(TUint8 aPollingInterval) = 0;

	}; // class MLtsyDispatchSatSetPollingInterval



class MLtsyDispatchSatSmsPpDownloadEnvelope : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatSmsPpDownloadEnvelopeApiId = KDispatchSatFuncUnitId + 14;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsySmsPpDdl
	 * request from the CTSY.
	 * 
	 * It is a request call that does not have an associated complete callback.
	 * 
	 * Implementation of this interface should hanlde a SMS-PP DOWNLOAD, ENVELOPE command 
	 * to be passed to the SIM.
	 * 
	 * See "3GPP 11.14" Under "SMS-PP data download"
	 * 
	 * @param aEnvelope BER-TLV encoded data containing an (SMS-PP DOWNLOAD) ENVELOPE command 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleSmsPpDownloadEnvelopeReqL(const TDesC8& aEnvelope) = 0;

	}; // class MLtsyDispatchSatSmsPpDownloadEnvelope



class MLtsyDispatchSatLocalInformationNmr : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatLocalInformationNmrApiId = KDispatchSatFuncUnitId + 15;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyLocalInformationNmr
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatLocalInformationNmrComp()
	 * 
	 * Implementation of this interface should request Network Measurement Results (NMR).
	 * They can be requested by a "LOCAL INFO" proactive command and this API is called
	 * during the construction of the command's Terminal Response.
	 * 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleLocalInformationNmrReqL() = 0;

	}; // class MLtsyDispatchSatLocalInformationNmr

class MLtsyDispatchSatCallAndSmsControlEnvelope : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatCallAndSmsControlEnvelopeApiId = KDispatchSatFuncUnitId + 16;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsySendEnvelope
	 * request from the CTSY.
	 * 
	 * It is a request call that does not have an associated complete callback.
	 * 
	 * Implementation of this interface should cause the TLV data to be sent in an ENVELOPE
	 * to the UICC. This interface is used for ENVELOPE commands related to call control events. 
	 * 
	 *
	 * @param aEnvelope BER-TLV encoded data containing a Call Control or MO SMS control by SIM ENVELOPE command. 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleCallAndSmsControlEnvelopeReqL(const TDesC8& aEnvelope) = 0;

	}; // class MLtsyDispatchSatCallAndSmsControlEnvelope



class MLtsyDispatchSatRefreshAllowed : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatRefreshAllowedApiId = KDispatchSatFuncUnitId + 17;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyServiceRequest
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatRefreshAllowedComp()
	 *
	 * 
	 * Implementation of this interface allows the main RSat clients report (RSat::RefreshAllowed()) 
	 * if the refresh can be carried out to be passed to the LTSY.
	 * The completion CCtsyDispatcherCallback::CallbackSatRefreshAllowedComp()
	 * completes any outstanding RSat::NotifyRefreshPCmd() requests.
	 * 
	 *
	 * @param aRefreshFileList The list of files (full path) that have been changed by the UICC (see ETSI 102 221 or ETSI 151 011)
	 * @param aAid Indicates the application on the UICC to be refreshed (as defined ETSI TS 101 220) . 
	 * @param aTsyCache Bit field of files that are cached in SIM ATK TSY (examin using CSatNotificationsTsy::TCachedSimAtkFiles) 
	 * 
	 * @see RSat::NotifyRefreshRequired()
	 * @see RSat::RefreshAllowed()
	 * @see RMmCustomAPI::SatRefreshCompleteNotification()
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleRefreshAllowedReqL(const TDesC8& aRefreshFileList, const TDesC8& aAid, TUint16 aTsyCache) = 0;

	}; // class MLtsyDispatchSatRefreshAllowed



class MLtsyDispatchSatReady : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatReadyApiId = KDispatchSatFuncUnitId + 18;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyReady
	 * request from the CTSY.
	 * 
	 * Implementation of this interface allows the CTSY to signal that it is ready to receive proactive commands.
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleReadyReqL() = 0;

	}; // class MLtsyDispatchSatReady



class MLtsyDispatchSatPCmdNotification : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatPCmdNotificationApiId = KDispatchSatFuncUnitId + 19;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyGetIfPendingNotification
	 * request from the CTSY.
	 * 
	 * It is a request call that does not have an associated complete callback.
	 * 
	 * This function uses this API to register an interest in a particular proactive command. Once this notification
	 * has been set up, the LTSY should call CCtsyDispatcherCallback::CallbackSatPcmdInd() to notify the CTSY the next 
	 * time this proactive command is generated by the UICC (or provide notification immediately if the command is pending).
	 * 
	 * Proactive commands which are received via CCtsyDispatcherCallback::CallbackSatPcmdInd() while a notification has not 
	 * been requested for this proactive command then a Terminal Response containing the return code "KMeUnableProcessCmd" 
	 * will be sent back to the UICC. Therfore it is perfectly acceptable for the LTSY to pass all proactive commands directly
	 * to the CTSY if it is preferred to have CTSY process proactive commands. If the latter implementation is chosen, LTSY does 
	 * not have to implement the MLtsyDispatchSatPCmdNotification interface at all. 
	 * 
	 * @param aPCmdType The number of the proactive command for which notification is being requested (e.g. KSetUpMenu),
	 *  as defined in ETSI 102 223 etc. these are defined in bertlv_defs.h  
	 *
	 * @see bertlv_defs.h
	 * 
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandlePCmdNotificationReqL(TUint8 aPCmdType) = 0;

	}; // class MLtsyDispatchSatPCmdNotification



class MLtsyDispatchSatUssdControlEnvelopeError : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatUssdControlEnvelopeErrorApiId = KDispatchSatFuncUnitId + 20;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyEnvelopeError
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatGetUssdControlSupportedComp()
	 * 
	 * This interface is invoked if it has not been possible to construct an envelope for 
	 * USSD control. This would occur if USSD strings are not supported for call control,
	 * and the USSD control request cannot be submitted as an SS control request because it 
	 * contains characters other than *, #, 0-9.
	 * 
	 * Implementation of this interface should handle this error appropriately (such as by
	 * continuing the USSD setup process). 
	 * 
	 * @see MLtsyDispatchSatGetUssdControlSupported::HandleGetUssdControlSupportedReqL()
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleUssdControlEnvelopeErrorReqL() = 0;

	}; // class MLtsyDispatchSatUssdControlEnvelopeError



class MLtsyDispatchSatTimerExpirationEnvelope : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatTimerExpirationEnvelopeApiId = KDispatchSatFuncUnitId + 21;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyTimerExpiredIPC
	 * request from the CTSY.
	 * 
 	 * It is a request call that does not have an associated complete callback.
	 * 
	 * The Timer managment proactive command requests the terminal to manage timers 
	 * running physically in the terminal.
	 * 
	 * When a timer expires (i.e. reaches zero), the terminal shall use the Timer Expiration 
	 * mechanism to transfer the identifier of the timer that has expired and the difference 
	 * between the time when this transfer occurs and the time when the timer was initially started. 
	 * The terminal then deactivates the timer.
	 * 
	 * This handler passes to the LTSY the Timer expiration ENVELOPE command, when a timer 
	 * previously started by a TIMER MANAGEMENT proactive command has expired.
	 * 
	 * Note that the command will not be sent until any proactive command is no longer ongoing.
	 * (i.e. the Terminal responses completion (CCtsyDispatcherCallback::CallbackSatTerminalRspComp() )
	 * to the proactive command has been completed by the LTSY) 
	 *
	 *
	 * @param aEnvelope BER-TLV encoded Timer expiration ENVELOPE command
	 * see "ETSI TS 102 223" under "ENVELOPE commands" "Timer expiration" 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleTimerExpirationEnvelopeReqL(const TDesC8& aEnvelope) = 0;

	}; // class MLtsyDispatchSatTimerExpirationEnvelope



class MLtsyDispatchSatTerminalRsp : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatTerminalRspApiId = KDispatchSatFuncUnitId + 22;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTerminalRsp
	 * request from the CTSY.
	 *
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatTerminalRspComp()
	 *
	 * Implementation of this interface should allow responses to Proactive Commands to be provided to the LTSY.
	 *
	 * @param aTerminalRsp The terminal response TLV as formatted in "ETSI TS 102 223" Under "Structure of TERMINAL RESPONSE" 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleTerminalRspReqL(const TDesC8& aTerminalRsp) = 0;

	}; // class MLtsyDispatchSatTerminalRsp



class MLtsyDispatchSatMenuSelectionEnvelope : public MLtsyDispatchInterface
	{
public:
 
	static const TInt KLtsyDispatchSatMenuSelectionEnvelopeApiId = KDispatchSatFuncUnitId + 23;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatMenuSelection
	 * request from the CTSY.
	 * 
	 * It is a request call that does not have an associated complete callback.
	 * 
	 * When the SIM application has previously specified a menu to the ME via the SETUP MENU proactive
	 * command, this API is used by the ME to communicate to the SIM application either that the user has 
	 * selected one of the menu items, or that the user has requested help information for one of the 
	 * menu items.
	 *  
	 *
	 * @param aEnvelope BER-TLV encoded Envelope containing the identifier of a menu item and whether the 
	 * user of the ME has selected the menu item or requested help about it. 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleMenuSelectionEnvelopeReqL(const TDesC8& aEnvelope) = 0;

	}; // class MLtsyDispatchSatMenuSelectionEnvelope



class MLtsyDispatchSatEventDownloadEnvelope : public MLtsyDispatchInterface
	{
public:

	static const TInt KLtsyDispatchSatEventDownloadEnvelopeApiId = KDispatchSatFuncUnitId + 24;

	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatEventDownload
	 * request from the CTSY.
	 * 
	 * It is a request call that does not have an associated complete callback.
	 * 
	 * The SET UP EVENT LIST proactive command gives the phone a list of events that may occur
	 * to the phone.  The event list may include a change of access technology, user
	 * action, browser termination and more.   The UICC must be informed when an event
	 * on the list occurs - this is via  
	 * MLtsyDispatchSatEventDownloadEnvelope::HandleEventDownloadEnvelopeReqL()
	 *
	 * @param aEnvelope BER-TLV encoded Event Download ENVELOPE command
	 * see "ETSI TS 102 223" under "ENVELOPE commands" "Event Download" 
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 * 
	 * @see RSat::EventDownload()
	 */
	virtual TInt HandleEventDownloadEnvelopeReqL(const TDesC8& aEnvelope) = 0;

	}; // class MLtsyDispatchSatEventDownloadEnvelope


class MLtsyDispatchSatGetUssdControlSupported : public MLtsyDispatchInterface
	{
public:
	
	static const TInt KLtsyDispatchSatGetUssdControlSupportedApiId = KDispatchSatFuncUnitId + 25;
	
	/**
	 * This API is optional and should only be used if the licensee wishes to including the
	 * Dispatcher beneath their own SIM ATK TSY.
	 * 
	 * The CTSY Dispatcher shall invoke this function on receiving the ESatTsyUssdControlSupported
	 * request from the CTSY.
	 * 
	 * It is a request call that is completed by invoking
	 * CCtsyDispatcherCallback::CallbackSatGetUssdControlSupportedComp()
	 *
	 * Asks whether USSD string data objects are supported in call control. This setting is 
	 * stored in EF-SST - see ETSI 151 011 etc. If USSD control is not supported, USSD control 
	 * requests that can be coded as an SS string (i.e. *,#,0-9) will be submitted in this format.  
	 *
	 * @return KErrNone on success, otherwise another error code indicating the
	 * failure.
	 */
	virtual TInt HandleGetUssdControlSupportedReqL() = 0;
	
	}; //class MLtsyDispatchSatGetUssdControlSupported

#endif /*MLTSYDISPATCHSATINTERFACE_H_*/
