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

#if !defined (__SMSPSTOR_H__)
#define __SMSPSTOR_H__

#include "gsmustor.h"
#include <logwrap.h>

/**
 *  @internalComponent
 */
_LIT(KReassemblyStoreName,"smsreast.dat");	//reassembly store name
const TInt KReassemblyStoreUidValue=0x1000089e;
const TUid KReassemblyStoreUid={KReassemblyStoreUidValue};  //  Used for second uid of SAR stores

/**
 *  @internalComponent
 */
_LIT(KSegmentationStoreName,"smssegst.dat");	//segmentation store name
const TInt KSegmentationStoreUidValue=0x1000089f;
const TUid KSegmentationStoreUid={KSegmentationStoreUidValue};  //  Used for second uid of SAR stores
const TInt KDefaultMaxmumNumberOfMessagesInSegmentationStore = 1000; //<Maximum number of messages in segmentation store

class CSmsMessage;
class TSmsAddr;
class TGsmSms;

//  TSAREntry fields for the reassembly store

//  Reference - concatenation reference
//  Total - total number of PDUs in SMS message
//  Count - number of PDUs received
//  Data1 - DCS bits7to4 and min and max IEI
//  Data2 - log server id
//  Data3 - SMS PDU type
//  Data4 - Storage
//  Description1 - first 32 characters from buffer
//  Description2 - original address
//  Time - time on SMS message

/**
 *  @internalComponent
 */
class CSmsReassemblyStore : public CSARStore
	{
public:
	static CSmsReassemblyStore* NewL(RFs& aFs);
	~CSmsReassemblyStore();

	void MatchPDUToExistingMessage(const CSmsMessage& aSmsMessage, TInt& aIndex);
	void UpdateExistingMessageL(const CSmsMessage& aSmsMessage,
								const TGsmSms& aGsmSms, TInt aIndex,
								TBool& aComplete,
								TBool& aDuplicateMsgRef,
								TBool& aDuplicateSlot);
	
	void NewMessagePDUL(TInt& aIndex,CSmsMessage& aSmsMessage,const TGsmSms& aGsmSms);
	void GetMessageL(TInt aIndex,CSmsMessage& aSmsMessage);
	TBool FindMessageL(const CSmsMessage& aSmsMessage, TBool aPassed, TInt& aIndex);
	void  SetPassedToClientL(TInt aIndex, TBool aPassed);
	TBool PassedToClient(TInt aIndex) const;
	void UpdateLogServerIdL(TInt aIndex, TLogId aLogServerId);
	void OpenStoreL();

private:
	void InternalizeEntryL(const TStreamId& aStreamId,CSmsMessage& aSmsMessage,CArrayFix<TInt>& aIndexArray,CArrayFix<TGsmSms>& aSmsArray);
	void ExternalizeEntryL(TStreamId& aStreamId,const CSmsMessage& aSmsMessage,const CArrayFix<TInt>& aIndexArray,const CArrayFix<TGsmSms>& aSmsArray);

	static void PopulateEntry(TSmsReassemblyEntry& aEntry,const CSmsMessage& aSmsMessage,TInt aNumSmss);
	void CreateEntryL(CSmsMessage& aSmsMessage,const CArrayFix<TInt>& aIndexArray,const CArrayFix<TGsmSms>& aSmsArray);

private:
	CSmsReassemblyStore(RFs& aFs);
	void ConstructL();

	TTime iLastReceivedTime;
	TTime iLastRealTime;
	TFileName iFullPathBuf;
	};

//  TSAREntry fields for the segmentation store

//  Reference - concatenation reference
//  Total - total number of PDUs in SMS message
//  Count - number of PDUs sent
//  Data1 - total PDUs delivered<<16 and failed
//  Data2 - log server id
//  Data3 - validity period
//  Data4 - type and min and max message reference
//  Description1 - first 32 characters from buffer
//  Description2 - original address
//  Time - time on SMS message


/**
 *  @internalComponent
 */

class CSmsSegmentationStore : public CSARStore
	{
public:
	enum
		{
		ENullStatus=0x100,
		EStatusComplete=0x00
		};

	/**
	 *  @internalComponent
	 */
	class TSmsSegmentationStoreRefStatus
		{
		public:
			inline TSmsSegmentationStoreRefStatus(TInt aRef=0, TInt aStatus=ENullStatus);
			static TInt Compare(const TSmsSegmentationStoreRefStatus& aLeft, const TSmsSegmentationStoreRefStatus& aRight);
			void InternalizeL(RReadStream& aStream);
			void ExternalizeL(RWriteStream& aStream) const;
			inline TInt Reference() const;
			inline TInt Status() const;
			inline void SetStatus(TInt aStatus);

		private:

			TInt iReference;	//< Message reference number
			TInt iStatus;		//< Status for this message reference
		};

	/**
	 *  @internalComponent
	 */
	class RSmsSegmentationStoreRefStatusArray : public RArray<TSmsSegmentationStoreRefStatus>
		{
		public:
			inline RSmsSegmentationStoreRefStatusArray();
			void InsertL(const TSmsSegmentationStoreRefStatus& aRefStatus);
			TInt Find(const TSmsSegmentationStoreRefStatus& aRefStatus) const;
			void CopyL(const RSmsSegmentationStoreRefStatusArray& aOther);
			void ResetAllStatus(TInt aStatus=ENullStatus);

			void InternalizeL(RReadStream& aStream);
			void ExternalizeL(RWriteStream& aStream) const;
		};

public:
	static CSmsSegmentationStore* NewL(RFs& aFs);
	~CSmsSegmentationStore();

	TInt Next8BitReferenceL();
	TInt Next16BitReferenceL();

	void AddSubmitL(const TSmsAddr& aSmsAddr,CSmsMessage& aSubmit);
	TBool AddCommandL(const TSmsAddr& aSmsAddr,const CSmsMessage& aCommand, RSmsSegmentationStoreRefStatusArray& aRefStatus);
	TBool AddReferenceL(const CSmsMessage& aSmsMessage,TInt aReference);
	TBool AddReferenceStatusPairL(const CSmsMessage& aSmsMessage,TInt aReference, TUint aSegmentSequenceNumber);
	TBool AddStatusReportL(TInt& aIndex,TBool& aComplete,const CSmsMessage& aStatusReport);
	void GetMessageL(TInt aIndex,TSmsAddr& aSmsAddr,CSmsMessage& aSmsMessage, RSmsSegmentationStoreRefStatusArray& aRefStatus);

	inline static TBool IsPermanentStatus(TInt aStatus);
	TBool HasEntryWithLogIdL(TLogId,TInt&,TInt&);
	void OpenStoreL();

private:
	void InternalizeConcatenationReferencesL(const TStreamId& aStreamId,TInt& aReference8bit,TInt& aReference16bit);
	void ExternalizeConcatenationReferencesL(TStreamId& aStreamId,TInt aReference8bit,TInt aReference16bit);

	void InternalizeEntryL(const TStreamId& aStreamId,TSmsAddr& aSmsAddr,CSmsMessage& aSmsMessage, RSmsSegmentationStoreRefStatusArray& aRefStatus);
	void ExternalizeEntryL(TStreamId& aStreamId,const TSmsAddr& aSmsAddr,const CSmsMessage& aSmsMessage, const RSmsSegmentationStoreRefStatusArray& aRefStatus);

	static void PopulateEntry(TSmsSegmentationEntry& aEntry,/*const TSmsAddr& aSmsAddr,*/const CSmsMessage& aSmsMessage, const RSmsSegmentationStoreRefStatusArray& aRefStatus);

	static TBool StatusArrayComplete(const RSmsSegmentationStoreRefStatusArray& aRefStatus, TSAREntry& aEntry);

	CSmsSegmentationStore(RFs& aFs);
	void ConstructL();
private:
	TFileName iFullPathBuf;
	};

#include "smspstor.inl"

#endif // !defined __SMSPSTOR_H__

