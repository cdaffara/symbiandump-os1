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


#ifndef __SMSPPROC_H__
#define __SMSPPROC_H__

#include <etelmm.h>
#include <smsuaddr.h>
#include <smsuact.h>
#include <logwrap.h>
#include <logsmspdudata.h>
#include "smspstor.h"
#include "gsmuSmsSlot.h"


class CFacadeSmsReassemblyStore;
class CSmsSegmentationStore;
class TSmsSettings;
class CSmsEventLogger;

class CSmspReceiveMode;
class MSmsComm;
class CSmsMonitorDiskSpace;
class CSmsPDURead;

/**
@internalComponent
*/

const TInt KDefaultMaxmumNumberOfCompleteMessagesInReassemblyStore = 10; //<Maximum number of comlete messages in reassembly store


/**
 *  @internalComponent
 */
NONSHARABLE_CLASS(CSmsPDUProcessor) : public CBase
	{
public:
	enum
		{
		ESmsMaxDeliverReportBufferSize = 160
		};

	//
	// Public static constructor and destructor...
	//
	static CSmsPDUProcessor* NewL(MSmsComm& aSmsComm, const TSmsSettings& aSmsSettings,
								  CFacadeSmsReassemblyStore& aReassemblyStore,
								  CSmsSegmentationStore& aSegmentationStore,
								  CSmsMonitorDiskSpace& aSmsMonitorDiskSpace);
	~CSmsPDUProcessor();

	//
	// Functions to return basic information about the PDU...
	//
	inline TBool IsComplete() const;
	inline TBool IsMobileTerminated() const;
	inline TBool IsEnumeration() const;
	inline TBool IsClass0Message() const;
	inline TBool IsClass1Message() const;
	inline TBool IsClass2Message() const;
	inline TBool IsClass3Message() const;
	inline TBool IsPIDType0() const;
	inline TBool IsForwardMessageToClient() const;
	inline TBool IsMarkedForAutoDelete() const;
	inline TBool IsPDUToBeDeleted() const;
	inline TBool IsMessageGoingToBeStored() const;
	
	//
	// Configurable options for processing...
	//	
	inline void SetApplyAutoDeletionToClass2(TBool aSetting);
	inline TBool IsApplyAutoDeletionToClass2() const;
	inline void SetDiscardType0Class0(TBool aSetting);
	inline TBool IsDiscardType0Class0() const;
	inline void SetDiscardType0Class2(TBool aSetting);
	inline TBool IsDiscardType0Class2() const;

	//
	// General process functions...
	//
	void DecodeAndProcessPDUL(TGsmSmsSlot& aSlot, TBool aIsEnumeration);
	void UpdateLogServerIdL();
	void ProcessMessageIfCompleteL();
	void DeletePDUL();
	
	//
	// Access functions for private members...
	//
	inline TPtrC DeliverReportBuffer() const;
	inline CSmsMessage* SmsMessage() const;
	inline CSmsMessage* OriginalSmsMessage() const;
	inline const TLogSmsPduData* SmsPDUData() const;
	inline TInt Index() const;

private:
	//
	// Private constructor and second phase constructor...
	//
	CSmsPDUProcessor(MSmsComm& aSmsComm, const TSmsSettings& aSmsSettings,
				     CFacadeSmsReassemblyStore& aReassemblyStore,
				     CSmsSegmentationStore& aSegmentationStore,
				     CSmsMonitorDiskSpace& aSmsMonitorDiskSpace);

	void ConstructL();

	//
	// Private processing functions...
	//
	void AnalysePDUCharacteristics();
	void FindOriginalMessageAndProcessStatusReportL();
	void UpdateStatusReportL();
	void AddSlotToSmsMessageIfRequiredL();
	void AddSegmentOfMessageToReassemblyStoreIfRequiredL();

private:
	MSmsComm&  iSmsComm;
	const TSmsSettings&  iSmsSettings;
	CFacadeSmsReassemblyStore&  iReassemblyStore;
	CSmsSegmentationStore&  iSegmentationStore;
	TGsmSms  iGsmSms;
	CSmsMessage*  iSmsMessage;
	CSmsMessage* iOriginalSmsMessage;
	TLogSmsPduData iSmsPDUData;
	TInt iIndex;

	TGsmSmsSlot iSlot;
	TSmsAddr iSmsAddr;
	CSmsSegmentationStore::RSmsSegmentationStoreRefStatusArray iStatusArray;
	TBuf16<ESmsMaxDeliverReportBufferSize> iDeliverReportBuffer;
	CSmsMonitorDiskSpace& iSmsMonitorDiskSpace;

	TBool  iIsEnumeration;
	TBool  iIsComplete;
	TBool  iIsMobileTerminated;
	TBool  iIsClass0Message;
	TBool  iIsClass1Message;
	TBool  iIsClass2Message;
	TBool  iIsClass3Message;
	TBool  iIsPIDType0;
	TBool  iIsForwardMessageToClient;
	TBool  iIsMarkedForAutoDelete;
	TBool  iIsPDUToBeDeleted;
	TBool  iIsMessageGoingToBeStored;
	TBool  iOptionApplyAutoDeletionToClass2;
	TBool  iOptionDiscardType0Class2;
	TBool  iOptionDiscardType0Class0;
	TBool  iIsWapSms;
	};

#include "smspproc.inl"

#endif // !defined __SMSPPROC_H__
