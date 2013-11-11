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

#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/common/speechrecognitioncustomcommands.h>
#include <mmf/common/speechrecognitiondataclient.h>
#include <mmf/common/speechrecognitiondatatest.h>


EXPORT_C CSDClientResult::~CSDClientResult()
	{
	}
	
CSDClientResult::CSDClientResult()
	{
	}
CSDClientResult::CSDClientResult(TGrammarID aGrammarID, TRuleID aRuleID)
	: iGrammarID(aGrammarID),
	  iRuleID(aRuleID)
	{
	
	}
	
void CSDClientResult::ConstructL()
	{
	
	}

EXPORT_C CSDClientResult* CSDClientResult::NewLC()
	{
	CSDClientResult* self = new (ELeave) CSDClientResult;
	CleanupStack::PushL(self);
	self->ConstructL();	
	return self;
	}	
	
EXPORT_C CSDClientResult* CSDClientResult::NewL()
	{
	CSDClientResult* self = CSDClientResult::NewLC();
	CleanupStack::Pop(self);
	return self;
	}	
	
EXPORT_C CSDClientResult* CSDClientResult::NewLC(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	CSDClientResult* self = new (ELeave) CSDClientResult(aGrammarID, aRuleID);
	CleanupStack::PushL(self);
	self->ConstructL();	
	return self;
	}	
	
EXPORT_C CSDClientResult* CSDClientResult::NewL(TGrammarID aGrammarID, TRuleID aRuleID)
	{
	CSDClientResult* self = CSDClientResult::NewLC(aGrammarID, aRuleID);
	CleanupStack::Pop(self);
	return self;
	}	

	
EXPORT_C void CSDClientResult::SetGrammarID(TGrammarID aGrammarID)
	{
	iGrammarID = aGrammarID;	
	
	}

EXPORT_C TGrammarID CSDClientResult::GrammarID() const
	{
	return iGrammarID;
	
	}

EXPORT_C void CSDClientResult::SetRuleID(TRuleID aRuleID)
	{
	iRuleID = aRuleID;
	}

EXPORT_C TRuleID CSDClientResult::RuleID() const
	{
	return iRuleID;
	}
	
void CSDClientResult::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(iGrammarID);
	aStream.WriteUint32L(iRuleID);
	}
	
void CSDClientResult::InternalizeL(RReadStream& aStream)
	{
	iGrammarID = static_cast<TGrammarID> (aStream.ReadUint32L());
	iRuleID = static_cast<TRuleID>(aStream.ReadUint32L());
	}
	
CSDClientResultSet::CSDClientResultSet()
	{
	}


EXPORT_C CSDClientResultSet::~CSDClientResultSet()
	{ 
	iResultArray.ResetAndDestroy();
	delete iDiagnostic;
	}

EXPORT_C CSDClientResultSet* CSDClientResultSet::NewL()
	{
	CSDClientResultSet* self = CSDClientResultSet::NewLC();
	CleanupStack::Pop(self);
	return self;	
	}
	
EXPORT_C CSDClientResultSet* CSDClientResultSet::NewLC()
	{
	CSDClientResultSet* self = new (ELeave) CSDClientResultSet;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;	
	}


EXPORT_C void CSDClientResultSet::SetMaxResultsL(TInt aMaxResults)
	{
	iMaxResults = aMaxResults;
	if (iResultArray.Count() < iMaxResults) 
		{
		TInt numAdd = iMaxResults - iResultArray.Count();
		for (TInt i = 0;i<numAdd;i++)
			{
			CSDClientResult* result = CSDClientResult::NewLC();
			User::LeaveIfError(iResultArray.Append(result));
			CleanupStack::Pop(result);
			}

		}
	}


EXPORT_C TInt CSDClientResultSet::MaxResults() const
	{
	return iMaxResults;
	}

EXPORT_C const CSDClientResult& CSDClientResultSet::At(TInt aIndex) const
	{
	return *iResultArray[aIndex];
	}

EXPORT_C void CSDClientResultSet::SetResultCount(TInt aResultCount)
	{
	iResultCount = aResultCount;	
	}

EXPORT_C TInt CSDClientResultSet::ResultCount() const
	{
	return iResultCount;
	
	}

EXPORT_C void CSDClientResultSet::SetDiagnostic(TDiagnostic& aDiagnostic)
	{
	iDiagnostic = &aDiagnostic;
	}

EXPORT_C const TDiagnostic& CSDClientResultSet::Diagnostic() const
	{
	return *iDiagnostic;
	}

EXPORT_C void CSDClientResultSet::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(iMaxResults);
	aStream.WriteUint32L(iResultCount);
	for (TInt i=0;i<iMaxResults;i++)
		iResultArray[i]->ExternalizeL(aStream);
	}
	
EXPORT_C void CSDClientResultSet::InternalizeL(RReadStream& aStream)
	{
	iResultArray.ResetAndDestroy();
	iMaxResults = aStream.ReadUint32L();
	iResultCount = aStream.ReadUint32L();
	for (TInt i=0;i<iMaxResults;i++)
		{
		CSDClientResult* clientResult = CSDClientResult::NewLC();
		clientResult->InternalizeL(aStream);
		User::LeaveIfError(iResultArray.Append(clientResult));
		CleanupStack::Pop(clientResult);
		}
	}
	
void CSDClientResultSet::ConstructL()
	{
	}
	
enum TDllReason {};
EXPORT_C TInt E32Dll(TDllReason /* aReason */)	
	{
	return (KErrNone);
	}

