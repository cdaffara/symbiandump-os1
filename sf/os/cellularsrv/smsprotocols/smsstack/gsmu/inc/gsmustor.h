// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declares the classes TSAREntry and CSARStore
// Abbreviations:
// PDU - Packet Data Unit
// SAR - Segmentation and Reassembly Store
// SR  - Status Report
// 
//

/**
 @file
*/

#ifndef GSMUSTOR_H__
#define GSMUSTOR_H__

#include <e32base.h>
#include <s32std.h>
#include <s32file.h>
#include <gsmumsg.h>
#include <babackup.h>

/**
 *  Filestore second UID for SAR stores.
 *  @publishedAll
 *  @released
 */
const TUid KSARStoreUid={0x1000089d};

//
// forward declarations
//
class RReadStream;
class RWriteStream;

/**
 *  Provides generic information useful for the segmentation and reassembly of
 *  SMS messages and reassembly of WAP datagrams.
 *  
 *  The constituent parts of both concatenated SMS messages and concatenated 7-bit
 *  WAP datagrams contain a reference, the total number of parts and the index
 *  of the part in the complete message or datagram.
 *  
 *  Field for the segmentation store are:
 *  
 *  Reference - concatenation reference
 *  Total - total number of PDUs in SMS message
 *  Count - number of PDUs sent
 *  Data1 - total PDUs delivered<<16 and failed
 *  Data2 - log server id
 *  Data3 - validity period
 *  Data4 - type and min and max message reference
 *  Description1 - first 32 characters from buffer
 *  Description2 - original address
 *  Time - time on SMS message
 *  
 *  Abstracts an entry which is stored in the CSARStore
 *  
 *  The data stored in Data1-4 has a different meaning depending on if it is used
 *  by the Reassembly store or the Segmentation store. The lists below detail
 *  the uses:
 *  
 *  Reassembly store:
 *  
 *  Data1: byte 3: bits7to4; byte 1: identifier1; byte 0: identifier2.
 *  
 *  Data2: Log server ID.
 *  
 *  Data3: byte 0: PDU type.
 *  
 *  Data4: SMS message storage .
 *  
 *  Segmentation store:
 *  
 *  Data1: bytes 3, 2: N delivered; byte 1,0 N failed.
 *  
 *  Data2: Log server ID.
 *  
 *  Data3: SMS submit validity period.
 *  
 *  Data4: byte 3,2 message PDU type; byte 1: reference1; byte 0: reference2.
 *  
 *  WAP reassembly store:
 *  
 *  Data1: to port.
 *  
 *  Data2: not used .
 *  
 *  Data3: not used.
 *  
 *  Data4: not used.
 *  
 *  <pre>
 *  
 *  The data stored in Data1-4 has a different meaning depending on if it is used
 *  by the Reassembly store or the Segmentation store. The tables below detail
 *  the uses:
 *  
 *  Reassembly Store:
 *  
 *  -----------------------------------------------------------------
 *  |  member |    3      |      2      |      1      |      0      |
 *  -----------------------------------------------------------------
 *  | iData1  |    bits7to4             | identifier1 | identifier2 |
 *  | iData2  |                   Log server ID                     |
 *  | iData3  |                                       | PDU type    |
 *  | iData4  |                 SMS message storage                 |
 *  -----------------------------------------------------------------
 *  
 *  Segmentation Store:
 *  
 *  -----------------------------------------------------------------
 *  |  member |    3      |      2      |      1      |      0      |
 *  -----------------------------------------------------------------
 *  | iData1  |    N delivered          |         N failed          |
 *  | iData2  |                   Log server ID                     |
 *  | iData3  |              SMS submit validity period             |
 *  | iData4  |   message PDU type      | reference1  | reference2  |
 *  -----------------------------------------------------------------
 *  
 *  WAP Reassembly Store:
 *  
 *  -----------------------------------------------------------------
 *  |  member |    3      |      2      |      1      |      0      |
 *  -----------------------------------------------------------------
 *  | iData1  |                    to port                          |
 *  | iData2  |                   not used                          |
 *  | iData3  |                   not used                          |
 *  | iData4  |                   not used                          |
 *  -----------------------------------------------------------------
 *  
 *  </pre>
 *  
 *  @publishedAll
 *  @released 
 *
 */
class TSAREntry
	{
public:
	enum
		{
		ESmsSAREntryDescriptionLength=32
		};
public:
	IMPORT_C TSAREntry();
	inline TInt Reference() const;
	inline void SetReference(TInt aReference);
	inline TInt Total() const;
	inline void SetTotal(TInt aTotal);
	inline TInt Count() const;
	inline void SetCount(TInt aCount);
	inline TBool IsComplete() const;

	/**
	 *  Gets the Log server ID field.
	 *  
	 *  	@return Log server ID field 
	 */
	inline TInt LogServerId() const {return iData2;}
	/**
	 *  Sets the Log server ID field.
	 *  
	 *  	@param aId Log server ID field 
	 */
	inline void SetLogServerId(TInt aId) {iData2 = aId;}

	inline TPtrC Description1() const;
	inline void SetDescription1(const TDesC& aDescription);
	inline TPtrC Description2() const;
	inline void SetDescription2(const TDesC& aDescription);

	inline TInt Data3() const;
	inline void SetData3(TInt aData);

	inline const TTime& Time() const;
	inline void SetTime(const TTime& aTime);

	inline TStreamId DataStreamId() const;
	inline void SetDataStreamId(TStreamId aStreamId);

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

private:
	inline TBool IsDeleted() const;
	inline void SetIsDeleted(TBool aIsDeleted);
	inline TBool IsAdded() const;
	inline void SetIsAdded(TBool aIsAdded);

private:
	enum TSAREntryFlags
		{
		ESAREntryIsDeleted=0x01, //< indicates that this entry is deleted
		ESAREntryIsAdded=0x02    //< indicated that this entry is added
		};
private:
	TInt iReference; //< concatenation reference
	TInt iTotal;     //< total number of PDUs in this SMS message
	TInt iCount;     //< number of PDUs sent/received for this SMS message
protected:
	TInt iData1;     
	TInt iData2;     //< Log server ID
	TInt iData3;
	TInt iData4;
private:
	TBuf<ESmsSAREntryDescriptionLength> iDescription1; //< first 32 characters from buffer
	TBuf<ESmsSAREntryDescriptionLength> iDescription2; //< original address
	TTime iTime;                                       //< time when message was sent/received
	TStreamId iDataStreamId;
	TInt iFlags;  //  Not externalized
	friend class CSARStore;
	friend class CSmsPermanentFileStore;
	friend class CPreallocatedFile;
	};


/**
 *  Abstraction of an SMS reassembly store entry for incoming SMS messages.
 *  
 *  This class should reflect parts of a complete SMS message to be stored in
 *  the reassembly store.
 *  @publishedAll
 *  @released 
 */
class TSmsReassemblyEntry : public TSAREntry
	{
private:
	enum
		{
		KMarkedAsPassedToClientFlag = 0x80000000,
		KPduTypeMask = 0xFF,
		KReassemblyEntryClear = 0x00
		};

public:
	// iData1
	/**
	 *  Gets bits 7 to 4 from Data1 field.
	 *  
	 *  	@return Bits 7 to 4 of Data 1 
	 */
	inline TInt Bits7to4() const {return (iData1 >> 16) & 0xffff;}
	/**
	 *  Gets Identifier1 from Data1 field.
	 *  
	 *  	@return Identifier1 
	 */
	inline TInt Identifier1() const {return (iData1 >> 8) & 0xff;}
	/**
	 *  Gets Identifier2 from Data1 field.
	 *  
	 *  	@return Identifier2 
	 */
	inline TInt Identifier2() const {return (iData1 >> 0) & 0xff;}
	inline void SetBits7to4andIdentifiers(TInt aBits7to4, TInt aId1, TInt aId2)
	/**
	 *  Sets bits 7 to 4, Identifier1, and Identifier2, in Data1 field.
	 *  
	 *  @param aBits7to4 Bits 7 to 4
	 *  @param aId1 Identifier1
	 *  	@param aId2 Identifier2 
	 */
	{iData1 = (TInt)((aBits7to4 << 16) | ((aId1 & 0xff)<< 8) | (aId2 & 0xff)); }

	// iData3
	/**
	 *  Gets PDU type.
	 *  
	 *  	@return PDU type 
	 */
	inline CSmsPDU::TSmsPDUType PduType() const {return (CSmsPDU::TSmsPDUType)(iData3 & 0xff);} // 8 bits
	/**
	 *  Sets PDU type.
	 *  
	 *  	@param aPduType PDU type 
	 */
	inline void SetPduType(CSmsPDU::TSmsPDUType aPduType) {iData3 = (iData3 & ~KPduTypeMask) | (TInt)(aPduType & KPduTypeMask);}

	/**
	 *  Tests if Passed To Client flag is set.
	 *  
	 *  	@return True if Passed To Client flag is set. 
	 */
	inline TBool PassedToClient() const {return (iData3 & KMarkedAsPassedToClientFlag) ? ETrue:EFalse;}
	/**
	 *  Sets Passed To Client flag.
	 *  
	 *  	@param aPassed True if Passed To Client flag is set. 
	 */
	inline void SetPassedToClient(TBool aPassed) {iData3 = (iData3 & ~KMarkedAsPassedToClientFlag) | (aPassed ? KMarkedAsPassedToClientFlag : KReassemblyEntryClear);}

	// iData4
	/**
	 *  Gets storage information.
	 *  
	 *  	@return Storage information 
	 */
	inline CSmsMessage::TMobileSmsStorage Storage() const {return (CSmsMessage::TMobileSmsStorage)(iData4 & 0xff);}
	/**
	 *  Sets storage information.
	 *  
	 *  	@param aStorage Storage information 
	 */
	inline void SetStorage(CSmsMessage::TMobileSmsStorage aStorage) {iData4 = (CSmsMessage::TMobileSmsStorage)aStorage;}
	};


/**
 *  Abstraction of an SMS segmentation store entry for outgoing SMS messages.
 *  
 *  This class should reflect parts of a complete SMS message to be stored in
 *  the segmentation store.
 *  @publishedAll
 *  @released 
 */
class TSmsSegmentationEntry : public TSAREntry
	{
public:
	// iData1
	/**
	 *  Gets number of delivered PDUs.
	 *  
	 *  	@return Number of delivered PDUs 
	 */
	inline TInt Delivered() const {return (iData1 >> 16);}
	/**
	 *  Gets number of permanently failed PDUs.
	 *  
	 *  	@return Number of permanently failed PDUs 
	 */
	inline TInt Failed() const {return (iData1 & 0xffff);}
	inline void SetDeliveredAndFailed(TInt aDelivered, TInt aFailed)
	/**
	 *  Sets the number of delivered and failed PDUs.
	 *  
	 *  @param aDelivered Number of delivered PDUs
	 *  	@param aFailed Number of permanently failed PDUs 
	 */
		{iData1 = ((aDelivered << 16) | (aFailed & 0xffff));}

	// iData3
	/**
	 *  Gets the Validity Period.
	 *  
	 *  	@return Validity Period 
	 */
	inline TInt ValidityPeriod() const {return (iData3); }
	/**
	 *  Sets the Validity Period.
	 *  
	 *  	@param aValPer Validity Period 
	 */
	inline void SetValidityPeriod(TInt aValPer) {iData3 = aValPer; }

	// iData4
	/**
	 *  Tests if a status report is expected.
	 *  
	 *  	@return True if a status report is expected 
	 */
	inline TBool ExpectStatusReport() const {return (((iData4 >> 24) & 0x01) == 0x01); }
	/**
	 *  Gets the type of the PDU.
	 *  
	 *  	@return Type of the PDU 
	 */
	inline CSmsPDU::TSmsPDUType PduType() const {return (CSmsPDU::TSmsPDUType)((iData4 >> 16) & 0xff); }
	/**
	 *  Gets the message reference (low byte).
	 *  
	 *  	@return Message reference (low byte) 
	 */
	inline TInt Reference1() const {return ((iData4 >> 8) & 0xff); }
	/**
	 *  Gets the message reference (high byte).
	 *  
	 *  	@return Message reference (high byte) 
	 */
	inline TInt Reference2() const {return ((iData4 >> 0) & 0xff); }
	inline void SetPduTypeAndRefs(TBool aSR, TInt aPduType, TInt aRef1, TInt aRef2)
	/**
	 *  Sets status report flag, PDU type, and message references.
	 *  
	 *  @param aSR True if a status report is expected
	 *  @param aPduType Type of the PDU
	 *  @param aRef1 Message reference (low byte)
	 *  	@param aRef2 Message reference (high byte) 
	 */
		{iData4 = (((aSR & 0x01) << 24) | (aPduType << 16) | ((aRef1 & 0xff ) << 8) | (aRef2 & 0xff));}
	};


/**
 *  Abstraction of a WAP reassembly store entry.
 *  This class should reflect a complete WAP message.
 *  @publishedAll
 *  @released
 */
class TWapReassemblyEntry : public TSAREntry
	{
public:
	/**
	 *  Gets the destination port number.
	 *  
	 *  	@return Destination port number 
	 */
	inline TInt ToPort() const {return iData1;}
	/**
	 *  Sets the destination port number.
	 *  
	 *  	@param aToPort Destination port number 
	 */
	inline void SetToPort(TInt aToPort){iData1 = aToPort;}
	};

/**
 *  Controls the segmentation and reassembly (SAR) store, used for SMS segmentation
 *  and reassembly, and WAP reassembly.
 *  @publishedAll
 *  @released 
 */
class CSARStore : public CBase
	{
public:
	IMPORT_C void OpenL(const TDesC& aFullName,const TUid& aThirdUid);
	IMPORT_C void Close();
	IMPORT_C void CompactL();

	IMPORT_C const CArrayFix<TSAREntry>& Entries() const;
	IMPORT_C void PurgeL(const TTimeIntervalMinutes& aTimeIntervalMinutes,TBool aPurgeIncompleteOnly);
	IMPORT_C void PurgeL(TInt aKSegmentationLifetimeMultiplier,TBool aPurgeIncompleteOnly);
	IMPORT_C void DeleteEntryL(TInt aIndex);

	IMPORT_C void BeginTransactionLC();
	IMPORT_C void CommitTransactionL(); //Pops this off this cleanup stack and commits iFileStore
	void Revert();

	inline RFs& FileSession() const;
	inline TBool InTransaction()const;
	IMPORT_C void DoOpenL();
	IMPORT_C void PrivatePath(TDes& aPath);


protected:
	IMPORT_C CSARStore(RFs& aFs);
	IMPORT_C ~CSARStore();

	IMPORT_C CFileStore& FileStore();
	IMPORT_C const CFileStore& FileStore() const;

	IMPORT_C void AddEntryL(const TSAREntry& aEntry);
	IMPORT_C void ChangeEntryL(TInt aIndex,const TSAREntry& aNewEntry);

	IMPORT_C TStreamId ExtraStreamId() const;
	IMPORT_C void SetExtraStreamIdL(const TStreamId& aExtraStreamId);

private:
	void DoDeleteEntryL(TInt aIndex);  //  Deletes stream but doesn't call iFileStore->CommitL()
	void InternalizeEntryArrayL();
	void ExternalizeEntryArrayL();
	void DoCommitAndCompactL();
	void RemoveDeletedEntries();
	void ReinstateDeletedEntries();

private:
	enum
		{
		KNumStoreCommitsBeforeCompaction=16
		};

protected:
	RFs& iFs;                             //< File server handle.
	/** Maximum number of SMS messages that can be stored in segmentation store.*/
	TInt iMaxmumNumberOfMessagesInSegmentationStore;
	
private:

	CFileStore* iFileStore;               //< pointer to the file store
	CArrayFixSeg<TSAREntry> iEntryArray; //< array of SAR entries
	TStreamId iExtraStreamId;             //< used for any other data that needs persisting
	TInt iCommitCount;                    //< counts number of Commit's - used for CompactL
	TPtrC iFullName;                      //< holds the name of the File Store
	TUid iThirdUid;
	TBool iInTransaction;					//< For debugging purposes to ensure only one transaction at a time
	};

/**
 *  Closes all SAR stores if a backup event starts.
 *  
 *  This is internal and not intended for use.
 *  @publishedAll
 *  @released 
 */
class CGsmuBackupObserver : public CBase, MBackupOperationObserver
	{
	public:
		IMPORT_C static CGsmuBackupObserver* NewL();
		IMPORT_C ~CGsmuBackupObserver();
		IMPORT_C void AddSARStoreL(CSARStore& aSARStore);
		IMPORT_C void RemoveSARStore(const CSARStore& aSARStore);

	private:
		CGsmuBackupObserver();
		void ConstructL();
		void HandleBackupOperationEventL(const TBackupOperationAttributes& aBackupOperationAttributes);

		CBaBackupSessionWrapper* iBackup;
		RPointerArray<CSARStore> iSARStores;
	};

#include "gsmustor.inl"

#endif // !defined GSMUSTOR_H__
