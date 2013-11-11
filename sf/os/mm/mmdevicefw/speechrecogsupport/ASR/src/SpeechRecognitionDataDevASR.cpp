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
// SpeechRecognitionDataClient.cpp
// 
//

#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/speechrecognitioncustomcommands.h>
#include <mmf/common/speechrecognitiondatadevasr.h>
#include <mmf/common/speechrecognitiondatatest.h>


EXPORT_C CSDPronunciation::~CSDPronunciation()
	{
	}
	
CSDPronunciation::CSDPronunciation(TPronunciationID aPronunciationID, TModelBankID aModelBankID, TPhonemeID aPhonemeID)
	: iPronunciationID(aPronunciationID),
	  iModelBankID(aModelBankID),
	  iPhonemeID(aPhonemeID)
	{
	
	}
	
void CSDPronunciation::ConstructL()
	{
	
	}

	
EXPORT_C CSDPronunciation* CSDPronunciation::NewL(TPronunciationID aPronunciationID, TModelBankID aModelBankID, TPhonemeID aPhonemeID)
	{
	CSDPronunciation* self = CSDPronunciation::NewLC(aPronunciationID, aModelBankID, aPhonemeID);
	CleanupStack::Pop(self);
	return self;
	}	
	
EXPORT_C CSDPronunciation* CSDPronunciation::NewLC(TPronunciationID aPronunciationID, TModelBankID aModelBankID, TPhonemeID aPhonemeID)
	{
	CSDPronunciation* self = new (ELeave) CSDPronunciation(aPronunciationID, aModelBankID, aPhonemeID);
	CleanupStack::PushL(self);
	self->ConstructL();	
	return self;
	}	
	
	

EXPORT_C TPronunciationID CSDPronunciation::PronunciationID() const
	{
	return iPronunciationID;
	
	}

EXPORT_C TModelBankID CSDPronunciation::ModelBankID() const
	{
	return iModelBankID;
	}

EXPORT_C TPhonemeID CSDPronunciation::PhonemeID() const
	{
	return iPhonemeID;
	
	}

	
	
CSDLexicon::CSDLexicon(TLexiconID aLexiconID)
	{
	iLexiconID = aLexiconID;
	}


EXPORT_C CSDLexicon::~CSDLexicon()
	{
	iPronunciationArray.ResetAndDestroy();
	}

EXPORT_C CSDLexicon* CSDLexicon::NewL(TLexiconID aLexiconID)
	{
	CSDLexicon* self = CSDLexicon::NewLC(aLexiconID);
	CleanupStack::Pop(self);
	return self;	
	}
	
EXPORT_C CSDLexicon* CSDLexicon::NewLC(TLexiconID aLexiconID)
	{
	CSDLexicon* self = new (ELeave) CSDLexicon(aLexiconID);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

void CSDLexicon::ConstructL() 
	{
	}


EXPORT_C TInt CSDLexicon::Add(CSDPronunciation& aPronunciation)
	{
	if (FindPronunciation(aPronunciation.PronunciationID())!=KErrNotFound)
		return KErrAlreadyExists;
	return iPronunciationArray.Append(&aPronunciation);
	}


EXPORT_C TInt CSDLexicon::FindPronunciation(TPronunciationID aPronunciationID) const
	{
	TBool found = EFalse;
	TInt index = 0;
	TInt count = iPronunciationArray.Count();
	for (TInt i=0;i < count;i++) 
		{
		if (aPronunciationID == iPronunciationArray[i]->PronunciationID()) 
			{
			found = true;
			index = i;
			break;
			}
		}
	if (found)
		return index;
	else
		return KErrNotFound;
	}


EXPORT_C TInt CSDLexicon::Count() const
	{
	return iPronunciationArray.Count();
	}


EXPORT_C const CSDPronunciation& CSDLexicon::At(TInt aIndex) const
	{
	return *iPronunciationArray[aIndex];
	}

EXPORT_C TLexiconID CSDLexicon::LexiconID() const
	{
	return iLexiconID;
	}


	
//------------------------------------------------------------------------------------------
EXPORT_C CSDRule::~CSDRule()
	{
	}
	
CSDRule::CSDRule(TRuleID aRuleID, TLexiconID aLexiconID, TPronunciationID aPronunciationID)
	: iRuleID(aRuleID),
	  iLexiconID(aLexiconID),
	  iPronunciationID(aPronunciationID)
	{
	
	}
	
void CSDRule::ConstructL()
	{
	
	}

	
EXPORT_C CSDRule* CSDRule::NewL(TRuleID aRuleID, TLexiconID aLexiconID, TPronunciationID aPronunciationID)
	{
	CSDRule* self = CSDRule::NewLC(aRuleID, aLexiconID, aPronunciationID);
	CleanupStack::Pop(self);
	return self;
	}	
	
EXPORT_C CSDRule* CSDRule::NewLC(TRuleID aRuleID, TLexiconID aLexiconID, TPronunciationID aPronunciationID)
	{
	CSDRule* self = new (ELeave) CSDRule(aRuleID, aLexiconID, aPronunciationID);
	CleanupStack::PushL(self);
	self->ConstructL();	
	return self;
	}	
	
	

EXPORT_C TRuleID CSDRule::RuleID() const
	{
	return iRuleID;
	
	}

EXPORT_C TLexiconID CSDRule::LexiconID() const
	{
	return iLexiconID;
	}

EXPORT_C TPronunciationID CSDRule::PronunciationID() const
	{
	return iPronunciationID;
	
	}

	
	
CSDGrammar::CSDGrammar(TGrammarID aGrammarID)
	{
	iGrammarID = aGrammarID;
	}


EXPORT_C CSDGrammar::~CSDGrammar()
	{
	iRuleArray.ResetAndDestroy();
	}

EXPORT_C CSDGrammar* CSDGrammar::NewL(TGrammarID aGrammarID)
	{
	CSDGrammar* self = CSDGrammar::NewLC(aGrammarID);
	CleanupStack::Pop(self);
	return self;	
	}
	
EXPORT_C CSDGrammar* CSDGrammar::NewLC(TGrammarID aGrammarID)
	{
	CSDGrammar* self = new (ELeave) CSDGrammar(aGrammarID);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

void CSDGrammar::ConstructL() 
	{
	}

EXPORT_C TGrammarID CSDGrammar::GrammarID() const
	{
	return iGrammarID;
	}


EXPORT_C TInt CSDGrammar::Add(CSDRule& aRule)
	{
	if (FindRule(aRule.RuleID())!=KErrNotFound)
		return KErrAlreadyExists;

	return iRuleArray.Append(&aRule);
	}


EXPORT_C TInt CSDGrammar::FindRule(TRuleID aRuleID) const
	{
	TBool found = EFalse;
	TInt index = 0;
	TInt count = iRuleArray.Count();
	for (TInt i=0;i < count;i++) 
		{
		if (aRuleID == iRuleArray[i]->RuleID()) 
			{
			found = true;
			index = i;
			break;
			}
		}
	if (found)
		return index;
	else
		return KErrNotFound;
	}

EXPORT_C void CSDGrammar::Delete(TRuleID aRuleID)
	{
	TInt count = iRuleArray.Count();
	for (TInt i=0;i < count;i++) 
		{
		if (aRuleID == iRuleArray[i]->RuleID()) 
			{
			iRuleArray.Remove(i);
			break;
			}
		}
	}


EXPORT_C TInt CSDGrammar::Count() const
	{
	return iRuleArray.Count();
	}




EXPORT_C const CSDRule& CSDGrammar::At(TInt aIndex) const
	{
	return *iRuleArray[aIndex];
	}

//------------------------------------------------------------------------------------------
EXPORT_C CSDModel::~CSDModel()
	{
	TUint8* data = const_cast<TUint8*>(iAcousticModelPtr.Ptr());
	delete[] data;
	}

CSDModel::CSDModel()
	: iAcousticModelPtr(NULL,0)
	{
	
	}


CSDModel::CSDModel(TPhonemeID aPhonemeID)
	: iPhonemeID(aPhonemeID), iAcousticModelPtr(NULL,0)
	{
	
	}
	
void CSDModel::ConstructL()
	{
	
	}


EXPORT_C CSDModel* CSDModel::NewL()
	{
	CSDModel* self = CSDModel::NewLC();
	CleanupStack::Pop(self);
	return self;
	}	
	
EXPORT_C CSDModel* CSDModel::NewLC()
	{
	CSDModel* self = new (ELeave) CSDModel;
	CleanupStack::PushL(self);
	self->ConstructL();	
	return self;
	}	


	
EXPORT_C CSDModel* CSDModel::NewL(TPhonemeID aPhonemeID)
	{
	CSDModel* self = CSDModel::NewLC(aPhonemeID);
	CleanupStack::Pop(self);
	return self;
	}	
	
EXPORT_C CSDModel* CSDModel::NewLC(TPhonemeID aPhonemeID)
	{
	CSDModel* self = new (ELeave) CSDModel(aPhonemeID);
	CleanupStack::PushL(self);
	self->ConstructL();	
	return self;
	}	
	

EXPORT_C TPhonemeID CSDModel::PhonemeID() const
	{
	return iPhonemeID;
	
	}


EXPORT_C void CSDModel::SetPhonemeID(TPhonemeID aPhonemeID)
	{
	iPhonemeID = aPhonemeID;
	}

EXPORT_C void CSDModel::SetAcousticModel(TPtr8& aAcousticModelPtr)
	{
	iAcousticModelPtr.Set(aAcousticModelPtr);
	}

EXPORT_C const TDesC8& CSDModel::AcousticModel() const
	{
	return iAcousticModelPtr;
	}
	
	
CSDModelBank::CSDModelBank(TModelBankID aModelBankID)
	{
	iModelBankID = aModelBankID;
	}


EXPORT_C CSDModelBank::~CSDModelBank()
	{
	iModelArray.ResetAndDestroy();
	}

EXPORT_C CSDModelBank* CSDModelBank::NewL(TModelBankID aModelBankID)
	{
	CSDModelBank* self = CSDModelBank::NewLC(aModelBankID);
	CleanupStack::Pop(self);
	return self;	
	}
	
EXPORT_C CSDModelBank* CSDModelBank::NewLC(TModelBankID aModelBankID)
	{
	CSDModelBank* self = new (ELeave) CSDModelBank(aModelBankID);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}

void CSDModelBank::ConstructL() 
	{
	}

EXPORT_C TInt CSDModelBank::Add(CSDModel& aModel)
	{
	if (FindModel(aModel.PhonemeID())!=KErrNotFound)
		return KErrAlreadyExists;

	return iModelArray.Append(&aModel);
	}


EXPORT_C TInt CSDModelBank::FindModel(TModelID aModelID) const
	{
	TBool found = EFalse;
	TInt index = 0;
	TInt count = iModelArray.Count();
	for (TInt i=0;i < count;i++) 
		{
		if (aModelID == iModelArray[i]->PhonemeID()) 
			{
			found = true;
			index = i;
			break;
			}
		}
	if (found)
		return index;
	else
		return KErrNotFound;

	}


EXPORT_C TInt CSDModelBank::Count() const
	{
	return iModelArray.Count();
	}

EXPORT_C TModelBankID CSDModelBank::ModelBankID() const
	{
	return iModelBankID;
	}



EXPORT_C const CSDModel& CSDModelBank::At(TInt aIndex) const
	{
	return *iModelArray[aIndex];
	}
//-------------------------------------------------------------------------------------
EXPORT_C CSDResult::~CSDResult()
	{
	}
	
CSDResult::CSDResult()
	{
	}
CSDResult::CSDResult(TGrammarID aGrammarID, TRuleID aRuleID)
	: iGrammarID(aGrammarID),
	  iRuleID(aRuleID)
	{
	
	}
	
void CSDResult::ConstructL()
	{
	
	}

EXPORT_C CSDResult* CSDResult::NewLC()
	{
	CSDResult* self = new (ELeave) CSDResult;
	CleanupStack::PushL(self);
	self->ConstructL();	
	return self;
	}	
	
EXPORT_C CSDResult* CSDResult::NewL()
	{
	CSDResult* self = CSDResult::NewLC();
	CleanupStack::Pop(self);
	return self;
	}	
	
EXPORT_C CSDResult* CSDResult::NewLC(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	CSDResult* self = new (ELeave) CSDResult(aGrammarID, aRuleID);
	CleanupStack::PushL(self);
	self->ConstructL();	
	return self;
	}	
	
EXPORT_C CSDResult* CSDResult::NewL(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	CSDResult* self = CSDResult::NewLC(aGrammarID, aRuleID);
	CleanupStack::Pop(self);
	return self;
	}	

	
EXPORT_C void CSDResult::SetGrammarID(TGrammarID aGrammarID)
	{
	iGrammarID = aGrammarID;	
	
	}

EXPORT_C TGrammarID CSDResult::GrammarID() const
	{
	return iGrammarID;
	
	}


EXPORT_C TInt32 CSDResult::Score() const
	{
	return iScore;
	}

EXPORT_C void CSDResult::SetScore(TInt32 aScore)
	{
	iScore = aScore;
	}

EXPORT_C void CSDResult::SetRuleID(TRuleID aRuleID)
	{
	iRuleID = aRuleID;
	}

EXPORT_C TRuleID CSDResult::RuleID() const
	{
	return iRuleID;
	}
	
	
CSDResultSet::CSDResultSet()
	{
	}


EXPORT_C CSDResultSet::~CSDResultSet()
	{
	iResultArray.ResetAndDestroy();
	delete iDiagnostic;
	}

EXPORT_C CSDResultSet* CSDResultSet::NewL()
	{
	CSDResultSet* self = CSDResultSet::NewLC();
	CleanupStack::Pop(self);
	return self;	
	}
	
EXPORT_C CSDResultSet* CSDResultSet::NewLC()
	{
	CSDResultSet* self = new (ELeave) CSDResultSet;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}


EXPORT_C TInt CSDResultSet::Add(CSDResult& aResult)
	{
	return iResultArray.Append(&aResult);
	}

EXPORT_C TInt CSDResultSet::Count() const
	{
	return iResultArray.Count();
	}

	

EXPORT_C const CSDResult& CSDResultSet::At(TInt aIndex)
	{
	return *iResultArray[aIndex];
	}

EXPORT_C void CSDResultSet::SetDiagnostic(TDiagnostic& aDiagnostic)
	{
	iDiagnostic = &aDiagnostic;
	}

EXPORT_C const TDiagnostic& CSDResultSet::Diagnostic() const
	{
	return *iDiagnostic;
	}


	
	
void CSDResultSet::ConstructL()
	{
	}

EXPORT_C TDiagnostic::TDiagnostic() 
	: iData(NULL,0)
	{
	iSNR = 0;
	iAccepted = EFalse;
	}
	

EXPORT_C TDiagnostic::~TDiagnostic() 
	{
	}

