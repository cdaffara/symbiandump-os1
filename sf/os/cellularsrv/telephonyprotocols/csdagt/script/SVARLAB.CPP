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
// Variable and Label Storage/Retrieval
// 
//

/**
 @file Svarlab.cpp 
*/

#include "SVARLAB.H"
#include "ND_STD.H"
#include "SSCRREAD.H"

_LIT(KIntegerVariableString,"%d");

const TInt KLabelArrayGranularity=10;
const TInt KVarArrayGranularity=20;
const TInt KVarValueLength=15;

// CScriptLabelMan definitions 

CScriptLabelMan* CScriptLabelMan::NewL()

/**
2 phased constructor for CScriptLabelMan, first phase.

@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CScriptLabelMan object.
*/
	{
	CScriptLabelMan* m=new(ELeave) CScriptLabelMan();
	CleanupStack::PushL(m);
	m->ConstructL();
	CleanupStack::Pop();
	return m;
	}

CScriptLabelMan::CScriptLabelMan()
/**
Constructor for CScriptReader, used in the first phase of construction.
*/
	{}

void CScriptLabelMan::ConstructL()
/**
Instantiate member variables.
*/
	{
	iLabelArray=new(ELeave) CArrayFixFlat<TLabel>(KLabelArrayGranularity);
	}

CScriptLabelMan::~CScriptLabelMan()
/**
Destructor.
Delete members.
*/
	{
	DeleteAll();
	delete iLabelArray;
	}

void CScriptLabelMan::AddLabelL(const TDesC& aLabelName,const TLinePosition& aPosition)
/**
Add a label with name aLabelName and position aPosition to the array if it is 
not already in the array.

@param aLabelName a reference to label name.
@param aPosition a reference to script position.
*/
	{
	if (aLabelName.Length()>KMaxLabelLength)
		User::Leave(KErrLabelNameTooLong);

	TLinePosition dummyPos;
	if (FindLabel(aLabelName,dummyPos)==KErrNotFound)
		{
		TLabel label;
		label.iName.CopyF(aLabelName);
		label.iPosition=aPosition;
		iLabelArray->AppendL(label);
		}
	else
		{
		// Encountered a label name that already exists - If it's
		// in EXACTLY the same place in the script as the existing
		// instance then it's actually the SAME label re-visited as
		// a result of executing a loop/branch operation, if it's in
		// a different place then it's an error in the script...
		if ((aPosition.iLineCount != dummyPos.iLineCount) ||
			(aPosition.iOffset != dummyPos.iOffset))
			{
			User::Leave(KErrAlreadyExists);
			}
		}
	}

TInt CScriptLabelMan::FindLabel(const TDesC& aLabelName,TLinePosition& aPosition)
/**
Find label with name aLabelName and set aPos to position and aLine to line.

@param aLabelName a reference to label name.
@param aPosition a reference to script position.
@return a KErrNone or KErrNotFound.
*/
	{
	TInt index;
	for (index=0; index<iLabelArray->Count(); index++)
		{
		if((*iLabelArray)[index].iName.CompareF(aLabelName)==0)
			{
			aPosition=(*iLabelArray)[index].iPosition;
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

void CScriptLabelMan::DeleteAll()
/**
Delete all labels from the array
*/
	{
	if (!iLabelArray)
		return;

	TInt count=iLabelArray->Count();
	iLabelArray->Delete(0,count);
	}

// CScriptVarMan definitions 

CScriptVarMan* CScriptVarMan::NewL()
/**
2 phased constructor for CScriptVarMan, first phase.

@exception Leaves if ConstructL() leaves, or not enough memory is available.
@return a new CScriptVarMan object.
*/
	{
	CScriptVarMan* m=new(ELeave) CScriptVarMan();
	CleanupStack::PushL(m);
	m->ConstructL();
	CleanupStack::Pop();
	return m;
	}

CScriptVarMan::CScriptVarMan() 
/**
Constructor for CScriptReader, used in the first phase of construction.
*/
	{}

void CScriptVarMan::ConstructL()
/**
Instantiate member variables.
*/
	{
	iVarArray=new(ELeave) CArrayFixFlat<TVar>(KVarArrayGranularity);
	}

CScriptVarMan::~CScriptVarMan()
/**
Destructor.
Delete members.
*/
	{
	DeleteAll();
	delete iVarArray;
	}

void CScriptVarMan::AddVariableL(const TDesC& aName,const TDesC& aContent)
/**
Add variable with name aName and aContent to the array

@param aName a reference to variable name.
@param aContent a reference to content.
*/
	{
	if (aName.Length()>KMaxVarNameLength)
		User::Leave(KErrVariableNameTooLong);

	HBufC* contentCopy=HBufC::NewLC(aContent.Length());
	contentCopy->Des().Copy(aContent);

	TPtrC dummy;
	if (FindVariable(aName,dummy)==KErrNotFound)
		{
		TVar var;
		var.iContent=contentCopy;
		var.iName.CopyF(aName);
		iVarArray->AppendL(var);
		}
	else
		{	
		delete (*iVarArray)[iIndex].iContent;		// FindVariable() will have set iIndex correctly
		(*iVarArray)[iIndex].iContent=contentCopy;
		}
	CleanupStack::Pop();
	}

void CScriptVarMan::AddVariableL(const TDesC& aName,TInt aVal)
/**
Add variable with name aName and value aVal to the array

@param aName a reference to variable name.
@param aVal is variable value.
*/
	{
	TBuf<KVarValueLength> buf;
	buf.Format(KIntegerVariableString,aVal);
	AddVariableL(aName,buf);
	}

TInt CScriptVarMan::FindVariable(const TDesC& aName,TPtrC& aContent)
/**
Find variable with name aName and put its value in aContent

@param aName a reference to variable name.
@param aContent a reference to content.
@return a KErrNone or KErrNotFound.
*/
	{
	for (iIndex=0; iIndex<iVarArray->Count(); iIndex++)
		{
		if ((*iVarArray)[iIndex].iName.CompareF(aName)==0)
			{
			aContent.Set((*iVarArray)[iIndex].iContent->Des());
			return KErrNone;
			}
		}
	return KErrNotFound;
	}

void CScriptVarMan::DeleteAll()
/**
Delete all variables from the array
*/
	{
	if (!iVarArray)
		return;

	TInt i;
	for (i=0; i<iVarArray->Count(); i++)
		delete (*iVarArray)[i].iContent;
	iVarArray->Reset();
	}
