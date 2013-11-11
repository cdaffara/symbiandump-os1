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
//

/**
 @file
 @internalAll 
*/


#ifndef __SMSPREAD_H__
#define __SMSPREAD_H__

#include <e32base.h>
#include "smspcomm.h"
#include <etelmm.h>
#include "smsuact.h"
#include <logwrap.h>
#include "smspproc.h"
#include "smspenum.h" // for TGsmSmsSlot definition


class CFacadeSmsReassemblyStore;
class CSmsSegmentationStore;
class RFs;
class CSmsEventLogger;
class CSmspReceiveMode;
class CSmsEventLogger;
class TLogSmsPduData;
class TSmsSettings;
class CSmsPduReadTimer;


/**
 *  Logs a CSmsMessage
 *  @internalComponent
 */
class CSmsPDUReadLogger : public CSmsuActiveBase
	{
	public:

		static CSmsPDUReadLogger* NewL(RFs& aFs, TInt aPriority);
		~CSmsPDUReadLogger();
		void ChangeOrAddLogEvent(CSmsMessage& aSmsMessage, const TLogSmsPduData& aSmsPDUData, const TTime* aTime, TRequestStatus& aStatus);

	private:

		CSmsPDUReadLogger(TInt aPriority);
		void ConstructL(RFs& aFs);

		void AddLogEvent();
		void GetLogEvent();
		void ChangeLogEvent();
		void DoRunL();
		void DoCancel();
		void Complete(TInt aStatus);

	private:

		enum TSmsPDUReadLoggerState
			{
			ESmsPDUReadLoggerIdle,	//< Idle
			ESmsPDUReadLoggerGet,	//< Getting Log Event
			ESmsPDUReadLoggerChange,//< Changing Log Event
			ESmsPDUReadLoggerAdd	//< Adding Log Event
			} iState;

		CSmsEventLogger* iLogger;			//< Used for Logging the CSmsMessage
		CSmsMessage* iSmsMessage;			//< The Message to Log. Pointer not "owned" by this class
		TTime iTime;
		const TTime* iTimePtr;
		const TLogSmsPduData* iSmsPDUData;	//< Data to add to the Log Event. Pointer not "owned" by this class
	};


/**
 *  Class CSmsStoreRead for setting TSY receive mode and receiving new incoming
 *  message from the phone side
 *  @internalComponent
 */
class CSmsPDURead : public CSmsuActiveBase
	{
friend class CSmsPduReadTimer;
public:
	static CSmsPDURead* NewL(MSmsComm& aSmsComm,
							 const TSmsSettings& aSmsSettings,
							 RMobileSmsMessaging& aSmsMessaging,
							 CFacadeSmsReassemblyStore& aReassemblyStore,
							 CSmsSegmentationStore& aSegmentationStore,
							 const RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps,
							 TInt aPriority,
							 CSmspReceiveMode& aSmspReceiveMode,
							 CSmsMonitorDiskSpace& aSmsMonitorDiskSpace);
	~CSmsPDURead();

	void Start();
	void ResumeSmsReception();

	const CSmsMessage* CurrentMessage() {return (iPduProcessor ? iPduProcessor->SmsMessage() : NULL);}

protected:
	void DoRunL();
	void DoCancel();

private:
    enum TRpErrorCodeIndex
        {
        // Only error conditions which can be configured to be negatively acknowledged
        // are enumerated.
        // MemoryCapacityExceeded is not enumerated, as this condition will always be
        // negatively acknowledged.
        EInvalidShortMsgTransferReferenceValue = 0,
        ESemanticallyIncorrectMessage = 1,
        EInvalidMandatoryInformaton = 2,
        EMessageTypeNonExistent = 3,
        EMessageNotCompatibleWithShortMessageProtocolState = 4,
        EInformationElementNonExistentOrNotImplemented = 5,
        EProtocolErrorUnspecified = 6,
        EUndefinedIndex = 7
        };

private:
	CSmsPDURead(MSmsComm& aSmsComm,
				const TSmsSettings& aSmsSettings,
				RMobileSmsMessaging& aSmsMessaging,
				CFacadeSmsReassemblyStore& aReassemblyStore,
				CSmsSegmentationStore& aSegmentationStore,
				const RMobileSmsMessaging::TMobileSmsCapsV1& aMobileSmsCaps,
				TInt aPriority,
				CSmspReceiveMode& aSmspReceiveMode,
				CSmsMonitorDiskSpace& aSmsMonitorDiskSpace);

    void ConstructL();
    void Receive();
    void ProcessPDUL();
    void AddLogEvent();
    void DoEncodeDeliverReportL();
    void SendDeliverReport();
    TBool IsSupported() const;
    void Complete(TInt aError);

    void SetErrorCodeStatus(enum TRpErrorCodeIndex aIndex, TBool aValue);
    TBool GetErrorCodeStatus(enum TRpErrorCodeIndex aIndex);

    void  ReadConfigurableSmsSettingsL();
    TBool MapRpErrorToIndex(TInt aRpErrorCode, TRpErrorCodeIndex& aIndex);

    void MakeStateTransitionBasedOnErrorCode();


private:
	enum TSmsPDUReadState
		{
		ESmsPDUReadIdle,				//< Idle
		ESmsPDUReadSetReceiveMode,		//< Setting receive mode
		ESmsPDUReadResumeReception,		//< Resuming reception of SMSs
		ESmsPDUReadWaitingForPDU,		//< Waiting new incoming message
		ESmsPDUReadProcessingPDU,		//< Performing various async procesing

		ESmsPDUReadForwardingPDUToClient,
		ESmsPDUReadNotForwardingPDUToClient,

		ESmsPDUReadSystemOutOfMemorySendNegativeAck,
		ESmsPDUReadAcceptValidPduSendPositiveAck,
		ESmsPDUReadAbsorbInvalidPduSendPositiveAck,
		ESmsPDUReadInvalidPDUSendNegativeAck,

		ESmsPDUReadLogging,
		ESmsPDUWaitTimerAfterOOM,
		ESmsPDUWaitingForDiskMonitor,
		ESmsPDUReadAbsorbValidPduSendPositiveAck
		};

private:
	TSmsPDUReadState iState;			//< The current state of the state machine

	RMobileSmsMessaging& iSmsMessaging;
	RMobileSmsMessaging::TMobileSmsGsmTpdu iMsgData;	//< Pdu buffer
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1 iMsgReceiveAttributes;	//< Class for received message attributes
    RMobileSmsMessaging::TMobileSmsReceiveAttributesV1Pckg iMsgReceiveAttributesPckg;	//< Package for iMsgReceiveAttributes
    
	TGsmSmsSlot iSlot;  //  Current iSlot entry

	CSmsPDUProcessor*  iPduProcessor;

	MSmsComm& iSmsComm;
	const TSmsSettings& iSmsSettings;
	CFacadeSmsReassemblyStore& iReassemblyStore;
	CSmsSegmentationStore& iSegmentationStore;
	const RMobileSmsMessaging::TMobileSmsCapsV1& iMobileSmsCaps;
	TGsmSms iDeliverReport;

	CSmsPDUReadLogger* iLogger;

	CSmspReceiveMode& iReceiveMode;	// a reference to owner class's instance
	RTimer	iOOMTimer;
	CSmsMonitorDiskSpace& iSmsMonitorDiskSpace;  // a reference to the iSmsMonitorDiskSpace object

	TUint8 iRpErrorCodesSupported;
    TBool  iConfigAutomaticDeletionForClass2;
    TBool  iConfigDiscardingType0Class2;
    TBool  iConfigDiscardingType0Class0; 

	TBool  iSmsReceptionSupended;
	TBool  iDelayedResumeSmsReceptionRequired;
	};

#endif // !defined __SMSPREAD_H__
