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
// Declares the CSmsuActiveBase based class CSmsMessageSend.
// 
//

/**
 @file
 @internalAll
*/

#ifndef __SMSPSEND_H__
#define __SMSPSEND_H__

#include "smspcomm.h"
#include <logwrap.h>
#include <logsmspdudata.h>
#include "smsuaddr.H"
#include <etelmm.h>
#include "Gsmumsg.h"
#include "smsuact.h"
#include "smspcdb.h"


class TSmsSettings;
class CSmsSegmentationStore;
class CLogWrapper;
class CLogEvent;
class CSmsEventLogger;
class CSmspSetBearer;


/**
 *  @internalComponent
 */
const TUint8 KTpFcsUnspecifiedError = 0xFF; 	// TP-FCS Unspecified error cause (GSM 03.40/3G TS 23.040)


/**
 *  @internalComponent
 */
class CSmsMessageSend : public CSmsuActiveBase
	{
public:
	static CSmsMessageSend* NewL(CSmsSegmentationStore& aSegmentationStore,
								 const TSmsSettings& aSmsSettings,
								 const RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps,
								 RMobileSmsMessaging& aSmsMessaging,
								 TInt aPriority,
								 CSmspSetBearer& aSmspSetBearer);
	~CSmsMessageSend();

	void Start(CSmsMessage& aSmsMessage, TInt aOptions,
			   const TSmsAddr& aSmsAddr, TRequestStatus& aStatus);

protected:
	void DoRunL();
	void DoCancel();

private:
	enum TSmsMessageSendState
		{
		ESmsMessageSendIdle,
		ESmsMessageSendSetBearer,
		ESmsMessageSendSegmentingMessage,
		ESmsMessageSendCreatingLogServerEvent,
		ESmsMessageSendPDU,
		ESmsMessageSendUpdatingSegmentationStore,
		ESmsMessageSendUpdatingLogEvent
		};

	CSmsMessageSend(CSmsSegmentationStore& aSegmentationStore,
				    const TSmsSettings& aSmsSettings,
				    const RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps,
				    RMobileSmsMessaging& aSmsMessaging,
					TInt aPriority,
					CSmspSetBearer& aSmspSetBearer);
	void ConstructL();

	void SegmentMessage();
	void DoSegmentMessageL();
	void CreateLogServerEvent();
	void SendNextPDU();
	void UpdateSegmentationStore();
	void DoUpdateSegmentationStoreL();
	void UpdateLogServerEvent();
	void SetData();
	void Complete(TInt aStatus);
	void DecodeSubmitReportL();
	TBool IsRPError() const;

	TSmsMessageSendState  iState;
	CSmsSegmentationStore&  iSegmentationStore;
	CSmspSetBearer&  iSmspSetBearer;			// a reference to owner class' instance
	const TSmsSettings&  iSmsSettings;					//< Reference to object that contains timeout settings
	const RMobileSmsMessaging::TMobileSmsCapsV1&  iMobileSmsCaps;	//< Reference to the SMS messaging capabilities of TSY/phone.
	RMobileSmsMessaging&  iSmsMessaging; 				//< Reference to ETEL SMS messaging subsession
	RMobileSmsMessaging::TMobileSmsSendAttributesV1  iMobileSmsSendAttributesV1; //< Attribute class for sending SMS message
	RMobileSmsMessaging::TMobileSmsSendAttributesV1Pckg  iMobileSmsSendAttributesV1Pckg; //< Packaged attribute class for sending SMS message

	CSmsMessage*  iSmsMessage;
	TSmsAddr  iSmsAddr;
	TInt  iOptions;
	CSmsEventLogger*  iSmsEventLogger;
	TLogSmsPduData  iSmsPDUData;
	TInt  iSendError;
	CArrayFixFlat<TGsmSms>  iSmsArray;
	TGsmSmsTelNumber  iToFromTelNumber;
	TInt  iSegmentSequenceNumber;
	};


#endif // __SMSPSEND_H__
