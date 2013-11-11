// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the definition of CSmsPduDatabase and a description of the database and tag format
// 
//

/**
 @file
 @internalComponent
*/

#ifndef __SMS_PDU_DB_H__
#define __SMS_PDU_DB_H__

#include "smspdudbitems.h"
#include "smsstackutils.h"

class CTestConfig;
class CTestConfigSection;

_LIT8(KSmsPduDbDefaultSection, "db");
_LIT(KSmsPduDbDefaultFilename, "smspdudb.txt");
_LIT(KSmsPduDbDefaultComponent, "sms");

class CSmsPduDatabase : public CBase
/**
 *  CSmsPduDatabase can read a SMS PDU database file and return the desired PDU or Message. This class can also parse CConfigTestItem's and special tags in its PduFactoryL() functions.
 */
	{
	private:

		enum TSmsPduDbTag
			/**
			 *  Position of elements in special tag that specifies an item in the PDU database
			 *  Has the format <[MESSAGE_TYPE]; [ID]; {SEGMENT}>. See PduFactoryL(const TDesC&) for more information.
			 */
			{
			ETagType = 0,	//< 1st element, alphanumeric. E.g. submit
			ETagId,			//< 2nd element, integer
			ETagSegment		//< 3rd element, optional integer
			};

		class TSmsPduDbId
			/**
			 *  Used internally within CSmsPduDatabase for quickly parsing the CTestConfig and obtaining an array of CConfigTestItem and ID pairs.
			 */
			{
			public:

				inline TSmsPduDbId(const CTestConfigItem* aItem=NULL, TInt aId=0);

			public:

				const CTestConfigItem* iItem;
				TInt iId;
			};

		class RSmsPduDbIdArray : public RArray<TSmsPduDbId>
			{
			public:

				inline RSmsPduDbIdArray(const TDesC8& aType, TBool aAllowDuplicates=EFalse);
				TInt Find(TInt aId) const;
				void InsertL(const TSmsPduDbId& aId);
				inline const TDesC8& Type() const;

			private:

				static TInt Compare(const TSmsPduDbId& aLeft, const TSmsPduDbId& aRight);

			private:

				const TDesC8& iType;
				TBool iAllowDuplicates;
			};

	public:

		IMPORT_C static CSmsPduDatabase* NewL(RFs& aFs); //< Uses section=KSmsPduDbDefaultSection, fileName=KSmsPduDbDefaultFilename, component=KSmsPduDbDefaultComponent
		IMPORT_C static CSmsPduDatabase* NewL(RFs& aFs, const TDesC8& aSection, const TDesC& aFileName, const TDesC& aComponent);
		IMPORT_C static CSmsPduDatabase* NewL(RFs& aFs, const CTestConfigSection& aSection);

		IMPORT_C ~CSmsPduDatabase();

		IMPORT_C CSmsPduDbPdu* GetPduLC(CSmsPDU::TSmsPDUType aType, TInt aId) const;
		IMPORT_C CSmsPduDbConcatSegment* GetConcatSegmentLC(CSmsPDU::TSmsPDUType aType, TInt aId, TInt aSegment) const;
		IMPORT_C CSmsPduDbConcat* GetConcatLC(CSmsPDU::TSmsPDUType aType, TInt aId) const;
		IMPORT_C CSmsPduDbMessage* GetMessageLC(CSmsPDU::TSmsPDUType aType, TInt aId) const;

		IMPORT_C void GetPduL(RPointerArray<CSmsPduDbPdu>& aArray, CSmsPDU::TSmsPDUType aType) const;
		IMPORT_C void GetConcatL(RPointerArray<CSmsPduDbConcat>& aArray, CSmsPDU::TSmsPDUType aType) const;
		IMPORT_C void GetMessageL(RPointerArray<CSmsPduDbMessage>& aArray, CSmsPDU::TSmsPDUType aType) const;

		IMPORT_C void RewriteFileL(const TDesC& aInputFileName, const TDesC& aInputComponent, const TDesC& aOutputFileName) const; //< Rewrites the SIM TSY config.txt using items in this PDU Database

		IMPORT_C CSmsPduDbPdu* PduFactoryL(const TDesC8& aTag, CSmsPDU::TSmsPDUType aTypeForMessageTags = CSmsPDU::ESmsSubmit) const;
		IMPORT_C CSmsPduDbPdu* PduFactory(const TDesC8& aTag, CSmsPDU::TSmsPDUType aTypeForMessageTags = CSmsPDU::ESmsSubmit) const;

		IMPORT_C CSmsPduDbPdu* PduFactoryL(const CTestConfigItem& aItem, CSmsPDU::TSmsPDUType aTypeForMessageTags = CSmsPDU::ESmsSubmit) const;
		IMPORT_C CSmsPduDbPdu* PduFactory(const CTestConfigItem& aItem, CSmsPDU::TSmsPDUType aTypeForMessageTags = CSmsPDU::ESmsSubmit) const;

		inline CTestConfig& ConfigFile();
		inline const CTestConfig& ConfigFile() const;
		inline void SetSectionL(const TDesC8& aSection);
		inline void SetSection(const CTestConfigSection& aSection);

	private:

		CSmsPduDatabase(RFs& aFs);

		void GetIdsL(RSmsPduDbIdArray& aIds, TInt aId) const;
		void GetIdsL(RSmsPduDbIdArray& aIds) const;
		TPtrC8 GetTypeL(CSmsPDU::TSmsPDUType aType) const;

		TPtrC8 GetConcatTypeL(CSmsPDU::TSmsPDUType aType) const;
		CSmsPduDbConcatSegment* GetConcatSegmentL(const RSmsPduDbIdArray& aIdArray, CSmsPDU::TSmsPDUType aType, TInt aId, TInt aSegment) const;

		void ReadFileL(const TDesC& aInputFileName, const TDesC& aInputComponent, CBufFlat& aData) const;
		void WriteFileL(const TDesC& aOutputFileName, const CBufFlat& aData) const;
		void ParseFileL(CBufFlat& aData) const;
		void ParseTagL(CBufFlat& aData, TPtrC8& aPtr, TLex8& aLex) const;

		TBool IsPdu(const TDesC8& aTag, CSmsPDU::TSmsPDUType& aType) const;
		TBool IsConcatSegment(const TDesC8& aTag, CSmsPDU::TSmsPDUType& aType) const;
		inline TBool IsMessage(const TDesC8& aTag) const;

	private:

		RFs& iFs;
		CTestConfig* iConfigFile;
		const CTestConfigSection* iSection; //< Owned by iConfigFile
	};

_LIT(KSmsPduDbPanic, "SmsPduDatabase");
enum TSmsPduDbPanic
	{
	EPduDbPanicConfigFileNotConstructed,
	EPduDbPanicPduNotConstructed
	};

inline void PduDbPanic(TSmsPduDbPanic);

#include "smspdudb.inl"

#endif
