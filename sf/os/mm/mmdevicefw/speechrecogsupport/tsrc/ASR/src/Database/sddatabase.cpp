// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "sddatabase.h"
#include "../Database/testdatabaseconstants.h"

_LIT(KTableGrammar, "GrammarTable");
_LIT(KTableLexicon, "LexiconTable");
_LIT(KTableModelBank, "ModelBankTable");
_LIT(KTableRule, "RuleTable");
_LIT(KTablePronunciation, "PronunciationTable");
_LIT(KTableModel, "ModelTable");
_LIT(KTableSettings, "SettingsTable");

_LIT(KAllCols, "*");
_LIT(KColClientUID, "ClientUID");
_LIT(KColGrammarID, "GrammarID");
_LIT(KColLexiconID, "LexiconID");
_LIT(KColModelBankID,"ModelBankID");
_LIT(KColRuleID,"RuleID");
_LIT(KColPronunciationID,"PronunciationID");
_LIT(KColModelID,"ModelID");
_LIT(KColUtteranceDuration, "UtteranceDuration");
_LIT(KColSettingsID,"SettingsID");
_LIT(KColSettingsValue,"SettingsValue");


_LIT(KSQLSelectFromTable,"Select %S From %S");
_LIT(KSQLSelectFromTableWhereCondition,"Select %S From %S Where %S=%u");

const TInt KGTColNumClientUID = 1;
const TInt KGTColNumGrammarID = 2;
const TInt KLTColNumClientUID = 1;
const TInt KLTColNumLexiconID = 2;
const TInt KMBTColNumClientUID = 1;
const TInt KMBTColNumModelBankID = 2;

const TInt KRTColNumGrammarID = 1;
const TInt KRTColNumRuleID = 2;
const TInt KRTColNumLexiconID = 3;
const TInt KRTColNumPronunciationID =4;

const TInt KPTColNumLexiconID = 1;
const TInt KPTColNumPronunciationID = 2;
const TInt KPTColNumModelBankID = 3;
const TInt KPTColNumModelID = 4;

const TInt KMTColNumModelBankID = 1;
const TInt KMTColNumModelID = 2;
const TInt KMTColNumUtteranceDurationMicroSeconds = 3;

const TInt KSTColNumSettingsID = 1;
const TInt KSTColNumSettingsValue = 2;

const TInt KSettingLastID = 1;
	
	
CSDDatabase::RSDGrammarTable::RSDGrammarTable(CSDDatabase& aDatabase)
	: iDatabase(aDatabase)
	{
	
	
	}
	
void CSDDatabase::RSDGrammarTable::OpenL(TBool aOwnedByClient)
	{
	TBuf<256> query;
	if (aOwnedByClient)	
		query.Format(KSQLSelectFromTableWhereCondition,&KAllCols,&KTableGrammar,&KColClientUID,iDatabase.ClientUid());
	else	
		query.Format(KSQLSelectFromTable,&KAllCols,&KTableGrammar,&KColClientUID,iDatabase.ClientUid());
		
	User::LeaveIfError(iView.Prepare(iDatabase.Database(),TDbQuery(query,EDbCompareNormal)));
	User::LeaveIfError(iView.EvaluateAll());
	
	}
	
void CSDDatabase::RSDGrammarTable::Close()
	{
	iView.Close();
	}
	
void CSDDatabase::RSDGrammarTable::CreateGrammarL(TGrammarID& aGrammarID)
	{
	iDatabase.StartTransactionL();
	aGrammarID = iDatabase.AllocNewIDL();	
	iView.InsertL();

	iView.SetColL(KGTColNumClientUID,iDatabase.ClientUid().iUid);
	iView.SetColL(KGTColNumGrammarID,aGrammarID);
	iView.PutL();
	}
	
void CSDDatabase::RSDGrammarTable::AddGrammarL(TGrammarID aGrammarID)
	{
	iDatabase.StartTransactionL();
	iDatabase.UpdateLastIDL(aGrammarID);
	iView.InsertL();
	iView.SetColL(KGTColNumClientUID,iDatabase.ClientUid().iUid);
	iView.SetColL(KGTColNumGrammarID,aGrammarID);
	iView.PutL();
	}
	
	
TBool CSDDatabase::RSDGrammarTable::FindGrammarL(TGrammarID aGrammarID)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KGTColNumGrammarID)==aGrammarID)
			{
			found = true;
			break;
			}
		}
	return found;
	}
	
	
	
void CSDDatabase::RSDGrammarTable::RemoveGrammarL(TGrammarID aGrammarID)
	{
	if (FindGrammarL(aGrammarID))
		{
		if (iView.ColUint32(KGTColNumClientUID)==(TUint32)iDatabase.ClientUid().iUid)		
			{
			iDatabase.StartTransactionL();
			iView.DeleteL();
			}
		else
			User::Leave(KErrNotFound);//change to not owned
		}
	else
		User::Leave(KErrNotFound);
	}

void CSDDatabase::RSDGrammarTable::GetAllGrammarIDsL(RArray<TGrammarID>& aGrammarIDs)
	{
	aGrammarIDs.Reset();
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		TGrammarID grammarID = iView.ColUint32(KGTColNumGrammarID);
		User::LeaveIfError(aGrammarIDs.Append(grammarID));
		}
	}
	
CSDDatabase::RSDLexiconTable::RSDLexiconTable(CSDDatabase& aDatabase)
	: iDatabase(aDatabase)
	{
	
	
	}
	
void CSDDatabase::RSDLexiconTable::OpenL(TBool aOwnedByClient)
	{
	TBuf<256> query;
	if (aOwnedByClient)	
		query.Format(KSQLSelectFromTableWhereCondition,&KAllCols,&KTableLexicon,&KColClientUID,iDatabase.ClientUid());
	else	
		query.Format(KSQLSelectFromTable,&KAllCols,&KTableLexicon,&KColClientUID,iDatabase.ClientUid());
		
	User::LeaveIfError(iView.Prepare(iDatabase.Database(),TDbQuery(query,EDbCompareNormal)));
	User::LeaveIfError(iView.EvaluateAll());
	
	}
	
void CSDDatabase::RSDLexiconTable::Close()
	{
	iView.Close();
	}
	
void CSDDatabase::RSDLexiconTable::CreateLexiconL(TLexiconID& aLexiconID)
	{
	iDatabase.StartTransactionL();
	aLexiconID = iDatabase.AllocNewIDL();	
	iView.InsertL();

	iView.SetColL(KLTColNumClientUID,iDatabase.ClientUid().iUid);
	iView.SetColL(KLTColNumLexiconID,aLexiconID);
	iView.PutL();
	}
	
void CSDDatabase::RSDLexiconTable::AddLexiconL(TLexiconID aLexiconID)
	{
	iDatabase.StartTransactionL();
	iDatabase.UpdateLastIDL(aLexiconID);
	iView.InsertL();
	iView.SetColL(KLTColNumClientUID,iDatabase.ClientUid().iUid);
	iView.SetColL(KLTColNumLexiconID,aLexiconID);
	iView.PutL();
	}
	
	
TBool CSDDatabase::RSDLexiconTable::FindLexiconL(TLexiconID aLexiconID)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KLTColNumLexiconID)==aLexiconID)
			{
			found = true;
			break;
			}
		}
	return found;
	}
	
	
	
void CSDDatabase::RSDLexiconTable::RemoveLexiconL(TLexiconID aLexiconID)
	{
	if (FindLexiconL(aLexiconID))
		{
		if (iView.ColUint32(KLTColNumClientUID)==(TUint32)iDatabase.ClientUid().iUid)		
			{
			iDatabase.StartTransactionL();
			iView.DeleteL();
			}
		else
			User::Leave(KErrNotFound);//change to not owned
		}
	else
		User::Leave(KErrNotFound);
	}

void CSDDatabase::RSDLexiconTable::GetAllLexiconIDsL(RArray<TLexiconID>& aLexiconIDs)
	{
	aLexiconIDs.Reset();
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		TLexiconID lexiconID = iView.ColUint32(KLTColNumLexiconID);
		User::LeaveIfError(aLexiconIDs.Append(lexiconID));
		}
	}
	
CSDDatabase::RSDModelBankTable::RSDModelBankTable(CSDDatabase& aDatabase)
	: iDatabase(aDatabase)
	{
	
	
	}
	
void CSDDatabase::RSDModelBankTable::OpenL(TBool aOwnedByClient)
	{
	TBuf<256> query;
	if (aOwnedByClient)	
		query.Format(KSQLSelectFromTableWhereCondition,&KAllCols,&KTableModelBank,&KColClientUID,iDatabase.ClientUid());
	else	
		query.Format(KSQLSelectFromTable,&KAllCols,&KTableModelBank,&KColClientUID,iDatabase.ClientUid());
		
	User::LeaveIfError(iView.Prepare(iDatabase.Database(),TDbQuery(query,EDbCompareNormal)));
	User::LeaveIfError(iView.EvaluateAll());
	
	}
	
void CSDDatabase::RSDModelBankTable::Close()
	{
	iView.Close();
	}
	
TModelBankID CSDDatabase::RSDModelBankTable::CreateModelBankL()
	{
	iDatabase.StartTransactionL();
	TModelBankID modelBankID;
	modelBankID = iDatabase.AllocNewIDL();	
	iView.InsertL();

	iView.SetColL(KMBTColNumClientUID,iDatabase.ClientUid().iUid);
	iView.SetColL(KMBTColNumModelBankID,modelBankID);
	iView.PutL();
	return modelBankID;
	}
	
void CSDDatabase::RSDModelBankTable::AddModelBankL(TModelBankID aModelBankID)
	{
	iDatabase.StartTransactionL();
	iDatabase.UpdateLastIDL(aModelBankID);
	iView.InsertL();
	iView.SetColL(KMBTColNumClientUID,iDatabase.ClientUid().iUid);
	iView.SetColL(KMBTColNumModelBankID,aModelBankID);
	iView.PutL();
	}
	
	
TBool CSDDatabase::RSDModelBankTable::FindModelBankL(TModelBankID aModelBankID)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KMBTColNumModelBankID)==aModelBankID)
			{
			found = true;
			break;
			}
		}
	return found;
	}
	
	
	
void CSDDatabase::RSDModelBankTable::RemoveModelBankL(TModelBankID aModelBankID)
	{
	if (FindModelBankL(aModelBankID))
		{
		if (iView.ColUint32(KMBTColNumClientUID)==(TUint32)iDatabase.ClientUid().iUid)		
			{
			iDatabase.StartTransactionL();
			iView.DeleteL();
			}
		else
			User::Leave(KErrNotFound);//change to not owned
		}
	else
		User::Leave(KErrNotFound);
	}

void CSDDatabase::RSDModelBankTable::GetAllModelBankIDsL(RArray<TModelBankID>& aModelBankIDs)
	{
	aModelBankIDs.Reset();
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		TModelBankID modelBankID = iView.ColUint32(KMBTColNumModelBankID);
		User::LeaveIfError(aModelBankIDs.Append(modelBankID));
		}
	}
	

CSDDatabase::RSDModelTable::RSDModelTable(CSDDatabase& aDatabase)
	: iDatabase(aDatabase)
	{
	
	
	}
	
void CSDDatabase::RSDModelTable::OpenL(TModelBankID aModelBankID)
	{
	if (!iDatabase.ModelBankExistsL(aModelBankID))
		User::Leave(KErrNotFound);

	iModelBankID = aModelBankID;
	TBuf<256> query;
	query.Format(KSQLSelectFromTableWhereCondition,&KAllCols,&KTableModel,&KColModelBankID,aModelBankID);		
	User::LeaveIfError(iView.Prepare(iDatabase.Database(),TDbQuery(query,EDbCompareNormal)));
	User::LeaveIfError(iView.EvaluateAll());
	
	}
	
void CSDDatabase::RSDModelTable::Close()
	{
	iView.Close();
	}
	
TModelID CSDDatabase::RSDModelTable::CreateModelL(TModelData aModelData)
	{
	iDatabase.StartTransactionL();
	TModelID modelID = iDatabase.AllocNewIDL();	
	iView.InsertL();
	iView.SetColL(KMTColNumModelBankID,iModelBankID);
	iView.SetColL(KMTColNumModelID,modelID);
	iView.SetColL(KMTColNumUtteranceDurationMicroSeconds,aModelData.iUtteranceDurationMicroSeconds);
	iView.PutL();
	return modelID;
	}
	
void CSDDatabase::RSDModelTable::AddModelL(TModelData aModelData)
	{
	iDatabase.StartTransactionL();
	iDatabase.UpdateLastIDL(aModelData.iModelID);
	iView.InsertL();
	iView.SetColL(KMTColNumModelBankID,iModelBankID);
	iView.SetColL(KMTColNumModelID,aModelData.iModelID);
	iView.SetColL(KMTColNumUtteranceDurationMicroSeconds, aModelData.iUtteranceDurationMicroSeconds);
	iView.PutL();
	}

void CSDDatabase::RSDModelTable::GetModelDataL(TModelID aModelID, TModelData& aModelData)
	{
	if (!FindModelL(aModelID))
		User::Leave(KErrNotFound);

	iView.GetL();
	aModelData.iModelID = iView.ColUint32(KMTColNumModelID);
	aModelData.iUtteranceDurationMicroSeconds = iView.ColUint32(KMTColNumUtteranceDurationMicroSeconds);
	}

	
	
TBool CSDDatabase::RSDModelTable::FindModelL(TModelID aModelID)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KMTColNumModelID)==aModelID) 
			{
			found = true;
			break;
			}
		}
	return found;
	}
	
	
	
void CSDDatabase::RSDModelTable::RemoveModelL(TModelID aModelID)
	{
	if (FindModelL(aModelID))
		{
		iDatabase.StartTransactionL();
		iView.DeleteL();
		}
	else
		User::Leave(KErrNotFound);
	}

void CSDDatabase::RSDModelTable::GetAllModelIDsL(RArray<TModelID>& aModelIDs)
	{
	aModelIDs.Reset();
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		TModelID modelID = iView.ColUint32(KMTColNumModelID);
		User::LeaveIfError(aModelIDs.Append(modelID));
		}
	}

//------------------------------------------------------------------------------------------------
CSDDatabase::RSDSettingsTable::RSDSettingsTable(CSDDatabase& aDatabase)
	: iDatabase(aDatabase)
	{
	
	
	}
	
void CSDDatabase::RSDSettingsTable::OpenL()
	{
	TBuf<256> query;
	query.Format(KSQLSelectFromTable,&KAllCols,&KTableSettings);		
	User::LeaveIfError(iView.Prepare(iDatabase.Database(),TDbQuery(query,EDbCompareNormal)));
	User::LeaveIfError(iView.EvaluateAll());
	}
	
void CSDDatabase::RSDSettingsTable::Close()
	{
	iView.Close();
	}
	
TUint32 CSDDatabase::RSDSettingsTable::GetValueL(TUint32 aSettingID)
	{
	TUint32 value = 0;
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KSTColNumSettingsID)==aSettingID)
			{
			value = iView.ColUint32(KSTColNumSettingsValue);	
			found = ETrue;
			break;
			}

		}
	if (!found)
		User::Leave(KErrNotFound);

	return value;
	}

void CSDDatabase::RSDSettingsTable::SetValueL(TUint32 aSettingID, TUint32 aValue)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KSTColNumSettingsID)==aSettingID)
			{
			iView.UpdateL();
			iView.SetColL(KSTColNumSettingsValue, aValue);	
			iView.PutL();
			found = ETrue;
			break;
			}

		}
	// if it wasn't found, add it
	if (!found)
		{
		iView.InsertL();
		iView.SetColL(KSTColNumSettingsID, aSettingID);	
		iView.SetColL(KSTColNumSettingsValue, aValue);	
		iView.PutL();
		}
	}
	

//------------------------------------------------------------------------------------------------

CSDDatabase::RSDPronunciationTable::RSDPronunciationTable(CSDDatabase& aDatabase)
	: iDatabase(aDatabase)
	{
	
	
	}
	
void CSDDatabase::RSDPronunciationTable::OpenL(TLexiconID aLexiconID)
	{
	if (!iDatabase.LexiconExistsL(aLexiconID))
		User::Leave(KErrNotFound);

	iLexiconID = aLexiconID;
	TBuf<256> query;
	query.Format(KSQLSelectFromTableWhereCondition,&KAllCols,&KTablePronunciation,&KColLexiconID,aLexiconID);		
	User::LeaveIfError(iView.Prepare(iDatabase.Database(),TDbQuery(query,EDbCompareNormal)));
	User::LeaveIfError(iView.EvaluateAll());
	
	}
	
void CSDDatabase::RSDPronunciationTable::Close()
	{
	iView.Close();
	}
	
TPronunciationID CSDDatabase::RSDPronunciationTable::CreatePronunciationL(TPronunciationData aPronunciationData)
	{
	TPronunciationID pronunciationID;
	iDatabase.StartTransactionL();
	pronunciationID = iDatabase.AllocNewIDL();
	iView.InsertL();
	iView.SetColL(KPTColNumLexiconID,iLexiconID);
	iView.SetColL(KPTColNumPronunciationID,pronunciationID);
	iView.SetColL(KPTColNumModelBankID,aPronunciationData.iModelBankID);
	iView.SetColL(KPTColNumModelID,aPronunciationData.iModelID);
	iView.PutL();
	return pronunciationID;
	}
	
void CSDDatabase::RSDPronunciationTable::AddPronunciationL(TPronunciationData aPronunciationData)
	{
	iDatabase.StartTransactionL();
	iDatabase.UpdateLastIDL(aPronunciationData.iPronunciationID);
	iView.InsertL();
	iView.SetColL(KPTColNumLexiconID,iLexiconID);
	iView.SetColL(KPTColNumPronunciationID,aPronunciationData.iPronunciationID);
	iView.SetColL(KPTColNumModelBankID,aPronunciationData.iModelBankID);
	iView.SetColL(KPTColNumModelID,aPronunciationData.iModelID);
	iView.PutL();
	}

void CSDDatabase::RSDPronunciationTable::GetPronunciationDataL(TPronunciationID aPronunciationID, TPronunciationData& aPronunciationData)
	{
	if (!FindPronunciationL(aPronunciationID))
		User::Leave(KErrNotFound);

	iView.GetL();
	aPronunciationData.iPronunciationID = iView.ColUint32(KPTColNumPronunciationID);
	aPronunciationData.iModelBankID = iView.ColUint32(KPTColNumModelBankID);
	aPronunciationData.iModelID = iView.ColUint32(KPTColNumModelID);
	}

	
	
TBool CSDDatabase::RSDPronunciationTable::FindPronunciationL(TPronunciationID aPronunciationID)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KPTColNumPronunciationID)==aPronunciationID)
			{
			found = true;
			break;
			}
		}
	return found;
	}
	
	
	
void CSDDatabase::RSDPronunciationTable::RemovePronunciationL(TPronunciationID aPronunciationID)
	{
	if (FindPronunciationL(aPronunciationID))
		{
		iDatabase.StartTransactionL();
		iView.DeleteL();
		}
	else
		User::Leave(KErrNotFound);
	}

void CSDDatabase::RSDPronunciationTable::GetAllPronunciationIDsL(RArray<TPronunciationID>& aPronunciationIDs)
	{
	aPronunciationIDs.Reset();
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		TPronunciationID modelID = iView.ColUint32(KPTColNumPronunciationID);
		User::LeaveIfError(aPronunciationIDs.Append(modelID));
		}
	}
//--------------------------------------------------------------------------------------------------

CSDDatabase::RSDRuleTable::RSDRuleTable(CSDDatabase& aDatabase)
	: iDatabase(aDatabase)
	{
	
	
	}
	
void CSDDatabase::RSDRuleTable::OpenL(TGrammarID aGrammarID)
	{
	if (!iDatabase.GrammarExistsL(aGrammarID))
		User::Leave(KErrNotFound);

	iGrammarID = aGrammarID;
	TBuf<256> query;
	query.Format(KSQLSelectFromTableWhereCondition,&KAllCols,&KTableRule,&KColGrammarID,aGrammarID);		
	User::LeaveIfError(iView.Prepare(iDatabase.Database(),TDbQuery(query,EDbCompareNormal)));
	User::LeaveIfError(iView.EvaluateAll());
	
	}
	
void CSDDatabase::RSDRuleTable::Close()
	{
	iView.Close();
	}
	
TRuleID CSDDatabase::RSDRuleTable::CreateRuleL(TRuleData aRuleData)
	{
	iDatabase.StartTransactionL();
	TRuleID ruleID = iDatabase.AllocNewIDL();	
	iView.InsertL();
	iView.SetColL(KRTColNumGrammarID,iGrammarID);
	iView.SetColL(KRTColNumRuleID,ruleID);
	iView.SetColL(KRTColNumLexiconID,aRuleData.iLexiconID);
	iView.SetColL(KRTColNumPronunciationID,aRuleData.iPronunciationID);
	iView.PutL();
	return ruleID;
	}
	
void CSDDatabase::RSDRuleTable::AddRuleL(TRuleData aRuleData)
	{
	iDatabase.StartTransactionL();
	iDatabase.UpdateLastIDL(aRuleData.iRuleID);

	iView.InsertL();
	iView.SetColL(KRTColNumGrammarID,iGrammarID);
	iView.SetColL(KRTColNumRuleID,aRuleData.iRuleID);
	iView.SetColL(KRTColNumLexiconID,aRuleData.iLexiconID);
	iView.SetColL(KRTColNumPronunciationID,aRuleData.iPronunciationID);
	iView.PutL();
	}
	
	
TBool CSDDatabase::RSDRuleTable::FindRuleL(TRuleID aRuleID)
	{
	TBool found = EFalse;
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		if (iView.ColUint32(KRTColNumRuleID)==aRuleID)
			{
			found = true;
			break;
			}
		}
	return found;
	}
	
TBool CSDDatabase::RSDRuleTable::IsRuleValidL(TRuleID aRuleID)
	{
	TBool valid = EFalse;
	TBool found = FindRuleL(aRuleID);
	if (found)
		{
		TLexiconID lexiconID = iView.ColUint32(KRTColNumLexiconID);
		TPronunciationID pronunciationID = iView.ColUint32(KRTColNumPronunciationID);
		if (iDatabase.LexiconExistsL(lexiconID) && 
			iDatabase.PronunciationExistsL(lexiconID, pronunciationID))
			{
			valid = ETrue;
			}
		}
	else
		User::Leave(KErrNotFound);
	return valid;
	}
	
	
void CSDDatabase::RSDRuleTable::RemoveRuleL(TRuleID aRuleID)
	{
	if (FindRuleL(aRuleID))
		{
		iDatabase.StartTransactionL();
		iView.DeleteL();
		}
	else
		User::Leave(KErrNotFound);
	}

void CSDDatabase::RSDRuleTable::GetAllRuleIDsL(RArray<TRuleID>& aRuleIDs)
	{
	aRuleIDs.Reset();
	for (iView.FirstL();iView.AtRow();iView.NextL())
		{
		iView.GetL();
		TRuleID modelID = iView.ColUint32(KRTColNumRuleID);
		User::LeaveIfError(aRuleIDs.Append(modelID));
		}
	}
//--------------------------------------------------------------------------------------------------


CSDDatabase* CSDDatabase::NewL(const TDesC& aFileName, TBool aUseExisting)
	{
	CSDDatabase* self = new (ELeave) CSDDatabase;
	CleanupStack::PushL(self);
	self->ConstructL(aFileName, aUseExisting);
	CleanupStack::Pop();
	return self;	
	}
	
	
void CSDDatabase::SetClientUid(TUid aClientUid)
	{
	iClientUid = aClientUid;
	}
	
void CSDDatabase::ConstructL(const TDesC& aFileName, TBool aUseExisting)
	{
 	if (aUseExisting)
		OpenDatabaseL(aFileName);
	else
		CreateDatabaseL(aFileName);
	}
	
void CSDDatabase::OpenDatabaseL(const TDesC& aFileName)
	{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	iDbStore = CFileStore::OpenL(fsSession,aFileName,EFileRead|EFileWrite);
	// open the database from the root stream
	iDatabase.OpenL(iDbStore,iDbStore->Root());	
	iDatabaseOpened = ETrue;
	}
	
	
void CSDDatabase::CreateDatabaseL(const TDesC& aFileName)
	{
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	iDbStore = CPermanentFileStore::ReplaceL(fsSession,aFileName,EFileRead|EFileWrite);
	// Complete file store creation
	iDbStore->SetTypeL(iDbStore->Layout());

				// Create a database in the store
	TStreamId id=iDatabase.CreateL(iDbStore);
	iDbStore->SetRootL(id);

				// Complete database creation by commiting the store
	iDbStore->CommitL();

	// create the CDs table
	DoCreateTablesL();

	// create an index
	DoCreateIndexesL();

	// add the inital settings
	RSDSettingsTable settingsTable(*this);
	settingsTable.OpenL();
	CleanupClosePushL(settingsTable);
	settingsTable.SetValueL(KSettingLastID, 0);
	CleanupStack::PopAndDestroy(&settingsTable);
	}

CSDDatabase::CSDDatabase()
	{
	
	}
	
CSDDatabase::~CSDDatabase()
	{
	if (iDatabaseOpened)
		{
		if (iDatabase.InTransaction())
			{
			iDatabase.Rollback();
			if (iDatabase.IsDamaged())
				iDatabase.Recover();
			}
		}
	iDatabase.Close();		
	delete iDbStore;
	}
	
	
void CSDDatabase::DoCreateTablesL()
	{
	CDbColSet* columns;
	
	// create Grammar Table
	columns=CDbColSet::NewLC();
	columns->AddL(TDbCol(KColClientUID,EDbColUint32));
	columns->AddL(TDbCol(KColGrammarID,EDbColUint32));
	User::LeaveIfError(iDatabase.CreateTable(KTableGrammar,*columns));	
	CleanupStack::PopAndDestroy(columns);

	// create Lexicon Table
	columns=CDbColSet::NewLC();
	columns->AddL(TDbCol(KColClientUID,EDbColUint32));
	columns->AddL(TDbCol(KColLexiconID,EDbColUint32));
	User::LeaveIfError(iDatabase.CreateTable(KTableLexicon,*columns));	
	CleanupStack::PopAndDestroy(columns);

	// create ModelBank Table
	columns=CDbColSet::NewLC();
	columns->AddL(TDbCol(KColClientUID,EDbColUint32));
	columns->AddL(TDbCol(KColModelBankID,EDbColUint32));
	User::LeaveIfError(iDatabase.CreateTable(KTableModelBank,*columns));	
	CleanupStack::PopAndDestroy(columns);
	
	// create Rule Table
	columns=CDbColSet::NewLC();
	columns->AddL(TDbCol(KColGrammarID,EDbColUint32));
	columns->AddL(TDbCol(KColRuleID,EDbColUint32));
	columns->AddL(TDbCol(KColLexiconID,EDbColUint32));
	columns->AddL(TDbCol(KColPronunciationID,EDbColUint32));

	User::LeaveIfError(iDatabase.CreateTable(KTableRule,*columns));	
	CleanupStack::PopAndDestroy(columns);

	// create Pronunciation Table
	columns=CDbColSet::NewLC();
	columns->AddL(TDbCol(KColLexiconID,EDbColUint32));
	columns->AddL(TDbCol(KColPronunciationID,EDbColUint32));
	columns->AddL(TDbCol(KColModelBankID,EDbColUint32));
	columns->AddL(TDbCol(KColModelID,EDbColUint32));
	User::LeaveIfError(iDatabase.CreateTable(KTablePronunciation,*columns));	
	CleanupStack::PopAndDestroy(columns);

	// create Model Table
	columns=CDbColSet::NewLC();
	columns->AddL(TDbCol(KColModelBankID,EDbColUint32));
	columns->AddL(TDbCol(KColModelID,EDbColUint32));
	columns->AddL(TDbCol(KColUtteranceDuration,EDbColUint32));
	User::LeaveIfError(iDatabase.CreateTable(KTableModel,*columns));	
	CleanupStack::PopAndDestroy(columns);

	// create Settings Table
	columns=CDbColSet::NewLC();
	columns->AddL(TDbCol(KColSettingsID,EDbColUint32));
	columns->AddL(TDbCol(KColSettingsValue,EDbColUint32));
	User::LeaveIfError(iDatabase.CreateTable(KTableSettings,*columns));	
	CleanupStack::PopAndDestroy(columns);
	}
	
	
void CSDDatabase::DoCreateIndexesL()
	{
	
	
	}
	
TGrammarID CSDDatabase::CreateGrammarL()
	{
	TGrammarID grammarID;
	RSDGrammarTable grammarTable(*this);
	CleanupClosePushL(grammarTable);
	grammarTable.OpenL();
	grammarTable.CreateGrammarL(grammarID);
	CleanupStack::PopAndDestroy(&grammarTable);
	return grammarID;
	}
void CSDDatabase::AddGrammarL(TGrammarID aGrammarID)
	{
	RSDGrammarTable grammarTable(*this);
	CleanupClosePushL(grammarTable);
	grammarTable.OpenL();
	grammarTable.AddGrammarL(aGrammarID);
	CleanupStack::PopAndDestroy(&grammarTable);
	}
	
void CSDDatabase::RemoveGrammarL(TGrammarID aGrammarID)
	{
	RSDGrammarTable grammarTable(*this);
	CleanupClosePushL(grammarTable);
	grammarTable.OpenL();
	grammarTable.RemoveGrammarL(aGrammarID);
	CleanupStack::PopAndDestroy(&grammarTable);
	}
	
void CSDDatabase::GetAllGrammarIDsL(RArray<TGrammarID>& aGrammarIDs, TBool aOwnedByClient)
	{
	RSDGrammarTable grammarTable(*this);
	CleanupClosePushL(grammarTable);
	grammarTable.OpenL(aOwnedByClient);
	grammarTable.GetAllGrammarIDsL(aGrammarIDs);
	CleanupStack::PopAndDestroy(&grammarTable);
	}
TBool CSDDatabase::GrammarExistsL(TGrammarID aGrammarID)
	{
	RSDGrammarTable grammarTable(*this);
	CleanupClosePushL(grammarTable);
	grammarTable.OpenL();
	TBool found = grammarTable.FindGrammarL(aGrammarID);
	CleanupStack::PopAndDestroy(&grammarTable);
	return found;
	}
	
TLexiconID CSDDatabase::CreateLexiconL()
	{
	TLexiconID lexiconID;
	RSDLexiconTable lexiconTable(*this);
	CleanupClosePushL(lexiconTable);
	lexiconTable.OpenL();
	lexiconTable.CreateLexiconL(lexiconID);
	CleanupStack::PopAndDestroy(&lexiconTable);
	return lexiconID;
	}
void CSDDatabase::AddLexiconL(TLexiconID aLexiconID)
	{
	RSDLexiconTable lexiconTable(*this);
	CleanupClosePushL(lexiconTable);
	lexiconTable.OpenL();
	lexiconTable.AddLexiconL(aLexiconID);
	CleanupStack::PopAndDestroy(&lexiconTable);
	}
	
void CSDDatabase::RemoveLexiconL(TLexiconID aLexiconID)
	{
	RSDLexiconTable lexiconTable(*this);
	CleanupClosePushL(lexiconTable);
	lexiconTable.OpenL();
	lexiconTable.RemoveLexiconL(aLexiconID);
	CleanupStack::PopAndDestroy(&lexiconTable);
	}
	
void CSDDatabase::GetAllLexiconIDsL(RArray<TLexiconID>& aLexiconIDs, TBool aOwnedByClient)
	{
	RSDLexiconTable lexiconTable(*this);
	CleanupClosePushL(lexiconTable);
	lexiconTable.OpenL(aOwnedByClient);
	lexiconTable.GetAllLexiconIDsL(aLexiconIDs);
	CleanupStack::PopAndDestroy(&lexiconTable);
	}
TBool CSDDatabase::LexiconExistsL(TLexiconID aLexiconID)
	{
	RSDLexiconTable lexiconTable(*this);
	CleanupClosePushL(lexiconTable);
	lexiconTable.OpenL();
	TBool found = lexiconTable.FindLexiconL(aLexiconID);
	CleanupStack::PopAndDestroy(&lexiconTable);
	return found;
	}
	
TModelBankID CSDDatabase::CreateModelBankL()
	{
	TModelBankID modelBankID;
	RSDModelBankTable modelBankTable(*this);
	CleanupClosePushL(modelBankTable);
	modelBankTable.OpenL();
	modelBankID = modelBankTable.CreateModelBankL();
	CleanupStack::PopAndDestroy(&modelBankTable);
	return modelBankID;
	}
void CSDDatabase::AddModelBankL(TModelBankID aModelBankID)
	{
	RSDModelBankTable modelBankTable(*this);
	CleanupClosePushL(modelBankTable);
	modelBankTable.OpenL();
	modelBankTable.AddModelBankL(aModelBankID);
	CleanupStack::PopAndDestroy(&modelBankTable);
	}
	
void CSDDatabase::RemoveModelBankL(TModelBankID aModelBankID)
	{
	RSDModelBankTable modelBankTable(*this);
	CleanupClosePushL(modelBankTable);
	modelBankTable.OpenL();
	modelBankTable.RemoveModelBankL(aModelBankID);
	CleanupStack::PopAndDestroy(&modelBankTable);
	}
	
void CSDDatabase::GetAllModelBankIDsL(RArray<TModelBankID>& aModelBankIDs, TBool aOwnedByClient)
	{
	RSDModelBankTable modelBankTable(*this);
	CleanupClosePushL(modelBankTable);
	modelBankTable.OpenL(aOwnedByClient);
	modelBankTable.GetAllModelBankIDsL(aModelBankIDs);
	CleanupStack::PopAndDestroy(&modelBankTable);
	}
TBool CSDDatabase::ModelBankExistsL(TModelBankID aModelBankID)
	{
	RSDModelBankTable modelBankTable(*this);
	CleanupClosePushL(modelBankTable);
	modelBankTable.OpenL();
	TBool found = modelBankTable.FindModelBankL(aModelBankID);
	CleanupStack::PopAndDestroy(&modelBankTable);
	return found;
	}
	
	
//------------------------------------------------------------------------------------------
TModelID CSDDatabase::CreateModelL(TModelBankID aModelBankID, TModelData aModelData)
	{
//	TModelID modelID;
	RSDModelTable modelTable(*this);
	CleanupClosePushL(modelTable);
	modelTable.OpenL(aModelBankID);
//	modelID = modelTable.CreateModelL(aModelData);
	modelTable.CreateModelL(aModelData);	// EABI warning removal
	CleanupStack::PopAndDestroy(&modelTable);
	return aModelData.iModelID;
	}
void CSDDatabase::AddModelL(TModelBankID aModelBankID, TModelData aModelData)
	{
	RSDModelTable modelTable(*this);
	CleanupClosePushL(modelTable);
	modelTable.OpenL(aModelBankID);
	modelTable.AddModelL(aModelData);
	CleanupStack::PopAndDestroy(&modelTable);
	}
	
void CSDDatabase::RemoveModelL(TModelBankID aModelBankID, TModelID aModelID)
	{
	RSDModelTable modelTable(*this);
	CleanupClosePushL(modelTable);
	modelTable.OpenL(aModelBankID);
	modelTable.RemoveModelL(aModelID);
	CleanupStack::PopAndDestroy(&modelTable);
	}

void CSDDatabase::GetModelDataL(TModelBankID aModelBankID, TModelID aModelID, TModelData& aModelData)
	{
	RSDModelTable modelTable(*this);
	CleanupClosePushL(modelTable);
	modelTable.OpenL(aModelBankID);
	modelTable.GetModelDataL(aModelID, aModelData);
	CleanupStack::PopAndDestroy(&modelTable);
	}


void CSDDatabase::GetAllModelIDsL(TModelBankID aModelBankID, RArray<TModelID>& aModelIDs)
	{
	RSDModelTable modelTable(*this);
	CleanupClosePushL(modelTable);
	modelTable.OpenL(aModelBankID);
	modelTable.GetAllModelIDsL(aModelIDs);
	CleanupStack::PopAndDestroy(&modelTable);
	}
TBool CSDDatabase::ModelExistsL(TModelBankID aModelBankID, TModelID aModelID)
	{
	RSDModelTable modelTable(*this);
	CleanupClosePushL(modelTable);
	modelTable.OpenL(aModelBankID);
	TBool found = modelTable.FindModelL(aModelID);
	CleanupStack::PopAndDestroy(&modelTable);
	return found;
	}
//------------------------------------------------------------------------------------------
TPronunciationID CSDDatabase::CreatePronunciationL(TLexiconID aLexiconID, TPronunciationData aPronunciationData)
	{
	TPronunciationID pronunciationID;
	RSDPronunciationTable pronunciationTable(*this);
	CleanupClosePushL(pronunciationTable);
	pronunciationTable.OpenL(aLexiconID);
	pronunciationID = pronunciationTable.CreatePronunciationL(aPronunciationData);
	CleanupStack::PopAndDestroy(&pronunciationTable);
	return pronunciationID;
	}
void CSDDatabase::AddPronunciationL(TLexiconID aLexiconID, TPronunciationData aPronunciationData)
	{
	RSDPronunciationTable pronunciationTable(*this);
	CleanupClosePushL(pronunciationTable);
	pronunciationTable.OpenL(aLexiconID);
	pronunciationTable.AddPronunciationL(aPronunciationData);
	CleanupStack::PopAndDestroy(&pronunciationTable);
	}
	
void CSDDatabase::RemovePronunciationL(TLexiconID aLexiconID, TPronunciationID aPronunciationID)
	{
	RSDPronunciationTable pronunciationTable(*this);
	CleanupClosePushL(pronunciationTable);
	pronunciationTable.OpenL(aLexiconID);
	pronunciationTable.RemovePronunciationL(aPronunciationID);
	CleanupStack::PopAndDestroy(&pronunciationTable);
	}

void CSDDatabase::GetPronunciationDataL(TLexiconID aLexiconID, TPronunciationID aPronunciationID, TPronunciationData& aPronunciationData)
	{
	RSDPronunciationTable pronunciationTable(*this);
	CleanupClosePushL(pronunciationTable);
	pronunciationTable.OpenL(aLexiconID);
	pronunciationTable.GetPronunciationDataL(aPronunciationID, aPronunciationData);
	CleanupStack::PopAndDestroy(&pronunciationTable);
	}
	
void CSDDatabase::GetAllPronunciationIDsL(TLexiconID aLexiconID, RArray<TPronunciationID>& aPronunciationIDs)
	{
	RSDPronunciationTable pronunciationTable(*this);
	CleanupClosePushL(pronunciationTable);
	pronunciationTable.OpenL(aLexiconID);
	pronunciationTable.GetAllPronunciationIDsL(aPronunciationIDs);
	CleanupStack::PopAndDestroy(&pronunciationTable);
	}
TBool CSDDatabase::PronunciationExistsL(TLexiconID aLexiconID, TPronunciationID aPronunciationID)
	{
	RSDPronunciationTable pronunciationTable(*this);
	CleanupClosePushL(pronunciationTable);
	pronunciationTable.OpenL(aLexiconID);
	TBool found = pronunciationTable.FindPronunciationL(aPronunciationID);
	CleanupStack::PopAndDestroy(&pronunciationTable);
	return found;
	}	
	
//------------------------------------------------------------------------------------------
TRuleID CSDDatabase::CreateRuleL(TGrammarID aGrammarID, TRuleData aRuleData)
	{
	TRuleID ruleID;
	RSDRuleTable ruleTable(*this);
	CleanupClosePushL(ruleTable);
	ruleTable.OpenL(aGrammarID);
	ruleID = ruleTable.CreateRuleL(aRuleData);
	CleanupStack::PopAndDestroy(&ruleTable);
	return ruleID;
	}
void CSDDatabase::AddRuleL(TGrammarID aGrammarID, TRuleData aRuleData)
	{
	RSDRuleTable ruleTable(*this);
	CleanupClosePushL(ruleTable);
	ruleTable.OpenL(aGrammarID);
	ruleTable.AddRuleL(aRuleData);
	CleanupStack::PopAndDestroy(&ruleTable);
	}
	
void CSDDatabase::RemoveRuleL(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	RSDRuleTable ruleTable(*this);
	CleanupClosePushL(ruleTable);
	ruleTable.OpenL(aGrammarID);
	ruleTable.RemoveRuleL(aRuleID);
	CleanupStack::PopAndDestroy(&ruleTable);
	}
	
void CSDDatabase::GetAllRuleIDsL(TGrammarID aGrammarID, RArray<TRuleID>& aRuleIDs)
	{
	RSDRuleTable ruleTable(*this);
	CleanupClosePushL(ruleTable);
	ruleTable.OpenL(aGrammarID);
	ruleTable.GetAllRuleIDsL(aRuleIDs);
	CleanupStack::PopAndDestroy(&ruleTable);
	}
TBool CSDDatabase::RuleExistsL(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	RSDRuleTable ruleTable(*this);
	CleanupClosePushL(ruleTable);
	ruleTable.OpenL(aGrammarID);
	TBool found = ruleTable.FindRuleL(aRuleID);
	CleanupStack::PopAndDestroy(&ruleTable);
	return found;
	}	

TUint32 CSDDatabase::AllocNewIDL()
	{
	RSDSettingsTable settingsTable(*this);
	CleanupClosePushL(settingsTable);
	settingsTable.OpenL();
	TUint32 lastID = settingsTable.GetValueL(KSettingLastID);
	lastID++;
	settingsTable.SetValueL(KSettingLastID, lastID);
	CleanupStack::PopAndDestroy(&settingsTable);
	return lastID;
	}

void CSDDatabase::UpdateLastIDL(TUint32 aAddedID)
	{
	RSDSettingsTable settingsTable(*this);
	CleanupClosePushL(settingsTable);
	settingsTable.OpenL();
	TUint32 lastID = settingsTable.GetValueL(KSettingLastID);
	if (aAddedID > lastID)
		settingsTable.SetValueL(KSettingLastID, aAddedID);
	CleanupStack::PopAndDestroy();
	}

	
//TUint32 CSDDatabase::iLastID = 100;

TUid CSDDatabase::ClientUid()
	{
	return iClientUid;
	}
	
RDbDatabase& CSDDatabase::Database() 
	{
	return iDatabase;
	}
	
void CSDDatabase::CommitChangesL()
	{
	iDatabase.Commit();
	}
	
void CSDDatabase::StartTransactionL()
	{
	if (!iDatabase.InTransaction())
		iDatabase.Begin();
	}
	
void CSDDatabase::CreateTestDatabaseL()
	{
	TInt i,j;
	// Add Grammars
	for (i = 0;i<KNumGrammars;i++)
		AddGrammarL(KGrammarIDs[i]);
		
	// Add Lexicons
	for (i = 0;i<KNumLexicons;i++)
		AddLexiconL(KLexiconIDs[i]);
	
	// Add ModelBanks
	for (i = 0;i<KNumLexicons;i++)
		AddModelBankL(KModelBankIDs[i]);
	
	// Add Rules
	for (i = 0;i<KNumGrammars;i++)
		for (j = 0; j < KNumRules; j++)
			{
			TGrammarID grammarID = KGrammarIDs[i];
			TRuleData ruleData = KRuleIDs[i][j];
			AddRuleL(grammarID, ruleData);
			}
		
	// Add Pronunciations
	for (i = 0;i<KNumLexicons;i++)
		for (j = 0; j < KNumPronunciations; j++)
			{
			TLexiconID lexiconID = KLexiconIDs[i];
			TPronunciationData pronunciationData = KPronunciationIDs[i][j];
			AddPronunciationL(lexiconID, pronunciationData);
			}
			
	// Add Models
	for (i = 0;i<KNumModelBanks;i++)
		for (j = 0; j < KNumModels; j++)
			{
			TModelData modelData = KModelIDs[i][j];
			TModelBankID modelBankID = KModelBankIDs[i];
			AddModelL(modelBankID, modelData);
			}
	CommitChangesL();
	}

	
