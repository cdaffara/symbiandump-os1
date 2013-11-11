// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SDDATABASE_H__
#define __SDDATABASE_H__

#include <e32base.h>

class CSecurityDecryptBase;
class CSecurityEncryptBase;

#include <d32dbms.h>
#include <s32file.h>
#include <mmf/common/speechrecognitiondatacommon.h>


class CSDDatabase : public CBase
	{
public:
	class TPronunciationData
		{
	public:
		TPronunciationID iPronunciationID;
		TModelBankID iModelBankID;
		TModelID iModelID;
		};
		
	class TRuleData
		{
	public:
		TRuleID iRuleID;
		TLexiconID iLexiconID;
		TPronunciationID iPronunciationID;
		};

	class TModelData
		{
	public:
		TModelID iModelID;
		TUint32 iUtteranceDurationMicroSeconds;
		};



	class RSDGrammarTable
		{
	public:
		RSDGrammarTable(CSDDatabase& aDatabase);		
		
		void OpenL(TBool aOwnedByClient = EFalse);
		
		void CreateGrammarL(TGrammarID& aGrammarID);
		void AddGrammarL(TGrammarID aGrammarID);
		void RemoveGrammarL(TGrammarID aGrammarID);

		TBool FindGrammarL(TGrammarID aGrammarID);
		void GetAllGrammarIDsL(RArray<TGrammarID>& aGrammarIDs);
		void Close();
		
	private:
		CSDDatabase& iDatabase;
		RDbView iView;
		};
		
	class RSDLexiconTable
		{
	public:
		RSDLexiconTable(CSDDatabase& aDatabase);		
		
		void OpenL(TBool aOwnedByClient = EFalse);
		
		void CreateLexiconL(TLexiconID& aLexiconID);
		void AddLexiconL(TLexiconID aLexiconID);
		void RemoveLexiconL(TLexiconID aLexiconID);

		TBool FindLexiconL(TLexiconID aLexiconID);
		void GetAllLexiconIDsL(RArray<TLexiconID>& aLexiconIDs);
		void Close();
		
	private:
		CSDDatabase& iDatabase;
		RDbView iView;
		};
		
	class RSDModelBankTable
		{
	public:
		RSDModelBankTable(CSDDatabase& aDatabase);		
		
		void OpenL(TBool aOwnedByClient = EFalse);
		
		TModelBankID CreateModelBankL();
		void AddModelBankL(TModelBankID aModelBankID);
		void RemoveModelBankL(TModelBankID aModelBankID);

		TBool FindModelBankL(TModelBankID aModelBankID);
		void GetAllModelBankIDsL(RArray<TModelBankID>& aModelBankIDs);
		void Close();
		
	private:
		CSDDatabase& iDatabase;
		RDbView iView;
		};
		
	class RSDRuleTable
		{
	public:
		RSDRuleTable(CSDDatabase& aDatabase);		
		
		void OpenL(TGrammarID aGrammarID);
		
		TRuleID CreateRuleL(TRuleData aRuleData);
		void AddRuleL(TRuleData aRuleData);
		void RemoveRuleL(TRuleID aRuleID);

		void GetRuleDataL(TRuleID aRuleID, TRuleData& aRuleData);

		TBool FindRuleL(TRuleID aRuleID);
		TBool IsRuleValidL(TRuleID aRuleID);
		void GetAllRuleIDsL(RArray<TRuleID>& aRuleIDs);
		void Close();
		
	private:
		CSDDatabase& iDatabase;
		RDbView iView;
		TGrammarID iGrammarID;
		};
		
	class RSDPronunciationTable
		{
	public:
		RSDPronunciationTable(CSDDatabase& aDatabase);		
		
		void OpenL(TLexiconID aLexiconID);
		
		TPronunciationID CreatePronunciationL(TPronunciationData aPronunciationData);
		void AddPronunciationL(TPronunciationData aPronunciationData);
		void RemovePronunciationL(TPronunciationID aPronunciationID);
		void GetPronunciationDataL(TPronunciationID aPronunciationID, TPronunciationData& aPronunciationData);

		TBool FindPronunciationL(TPronunciationID aPronunciationID);
		void GetAllPronunciationIDsL(RArray<TPronunciationID>& aPronunciationIDs);
		void Close();
		
	private:
		CSDDatabase& iDatabase;
		RDbView iView;
		TLexiconID iLexiconID;
		};


		
		
	class RSDModelTable
		{
	public:
		RSDModelTable(CSDDatabase& aDatabase);		
		
		void OpenL(TModelBankID aModelBankID);
		
		TModelID CreateModelL(TModelData aModelData);
		void AddModelL(TModelData aModelData);
		void RemoveModelL(TModelID aModelID);

		void GetModelDataL(TModelID aModelID, TModelData& aModelData);

		TBool FindModelL(TModelID aModelID);
		void GetAllModelIDsL(RArray<TModelID>& aModelIDs);
		void Close();
		
	private:
		CSDDatabase& iDatabase;
		RDbView iView;
		TModelBankID iModelBankID;
		};

	class RSDSettingsTable
		{
	public:
		RSDSettingsTable(CSDDatabase& aDatabase);		
		
		void OpenL();

		TUint32 GetValueL(TUint32 aIndex);
		void SetValueL(TUint32 aIndex, TUint32 aValue);

		void Close();
		
	private:
		CSDDatabase& iDatabase;
		RDbView iView;
		};





		
public:		
	static CSDDatabase* NewL(const TDesC& aFileName, TBool aUseExisting=ETrue);
	
	TGrammarID CreateGrammarL();
	void AddGrammarL(TGrammarID aGrammarID);
	void RemoveGrammarL(TGrammarID aGrammarID);
	void GetAllGrammarIDsL(RArray<TGrammarID>& aGrammarIDs, TBool aOwnedByClient=EFalse);
	TBool GrammarExistsL(TGrammarID aGrammarID);
	
	TLexiconID CreateLexiconL();
	void AddLexiconL(TLexiconID aLexiconID);
	void RemoveLexiconL(TLexiconID aLexiconID);
	void GetAllLexiconIDsL(RArray<TLexiconID>& aLexiconIDs, TBool aOwnedByClient=EFalse);
	TBool LexiconExistsL(TLexiconID aLexiconID);
	
	TModelBankID CreateModelBankL();
	void AddModelBankL(TModelBankID aModelBankID);
	void RemoveModelBankL(TModelBankID aModelBankID);
	void GetAllModelBankIDsL(RArray<TModelBankID>& aModelBankIDs, TBool aOwnedByClient=EFalse);
	TBool ModelBankExistsL(TModelBankID aModelBankID);


	TRuleID CreateRuleL(TGrammarID aGrammarID, TRuleData aRuleData);
	void AddRuleL(TGrammarID aGrammarID, TRuleData aRuleData);
	void RemoveRuleL(TGrammarID aGrammarID, TRuleID aRuleID);
	void GetAllRuleIDsL(TGrammarID aGrammarID, RArray<TRuleID>& aRuleIDs);
	TBool RuleExistsL(TGrammarID aGrammarID, TRuleID aRuleID);
	
	TPronunciationID CreatePronunciationL(TLexiconID aLexiconID, TPronunciationData aPronunciationData);
	void AddPronunciationL(TLexiconID aLexiconID, TPronunciationData aPronunciationData);
	void RemovePronunciationL(TLexiconID aLexiconID, TPronunciationID aPronunciationID);
	void GetPronunciationDataL(TLexiconID aLexiconID, TPronunciationID aPronunciationID, TPronunciationData& aPronunciation);
	void GetAllPronunciationIDsL(TLexiconID aLexiconID, RArray<TPronunciationID>& aPronunciationID);
	TBool PronunciationExistsL(TLexiconID aLexiconID, TPronunciationID aPronunciationID);
	
	TModelID CreateModelL(TModelBankID aModelBankID, TModelData aModelData);
	void AddModelL(TModelBankID aModelBankID, TModelData aModelData);
	void RemoveModelL(TModelBankID aModelBankID, TModelID aModelID);
	void GetModelDataL(TModelBankID aModelBankID, TModelID aModelID, TModelData& aModelData);
	void GetAllModelIDsL(TModelBankID aModelBankID, RArray<TModelID>& aModelIDs);
	TBool ModelExistsL(TModelBankID aModelBankID, TModelID aModelID);

	
	TUid ClientUid();
	RDbDatabase& Database();
	
	void StartTransactionL();
	void CommitChangesL();
	
	TUint32 AllocNewIDL();
	void UpdateLastIDL(TUint32 aLastID);
	
	void SetClientUid(TUid aClientUid);
	~CSDDatabase(); 
	
	void CreateTestDatabaseL();
	
private:
	void OpenDatabaseL(const TDesC& aFileName);
	void CreateDatabaseL(const TDesC& aFileName);
	
	void DoCreateTablesL();
	void DoCreateIndexesL();
	
	CSDDatabase();
	
	void ConstructL(const TDesC& aFileName, TBool aUseExisting);
	

	TUid iClientUid;
	CFileStore* iDbStore;
	RDbStoreDatabase iDatabase;
	TBool iDatabaseOpened;
	
	TUint32 iLastID;
		
	};
	
#endif //__SDDATABASE_H__
