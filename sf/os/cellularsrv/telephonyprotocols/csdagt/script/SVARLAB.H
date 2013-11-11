/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Variable and Label Store/Retrieve Header
* 
*
*/



/**
 @file Svarlab.h
 @internalComponent
*/

#ifndef __SVARLAB_H__
#define __SVARLAB_H__

#include "SSCRREAD.H"

const TInt KMaxLabelLength=20;
const TInt KMaxVarNameLength=20;

struct TLabel
	{
	TBuf<KMaxLabelLength> iName;
	TLinePosition iPosition;
	};


class CScriptLabelMan : public CBase
/**
One of these per CScriptExecutor.  Manages the list of labels used by the script.

@internalComponent
*/
	{
public:
	static CScriptLabelMan* NewL();
	~CScriptLabelMan();
	void AddLabelL(const TDesC& aLabel,const TLinePosition& aPosition);
	TInt FindLabel(const TDesC& aLabelName,TLinePosition& aPosition);
	void DeleteAll();
private:
	CScriptLabelMan();
	void ConstructL();
private:
	CArrayFixFlat<TLabel>* iLabelArray;
	};

struct TVar
	{
	TBuf<KMaxVarNameLength> iName;
	HBufC* iContent;
	};


class CScriptVarMan : public CBase
/**
One of these per CScriptExecutor. Manages a list of variables used by the script.

@internalComponent
*/
	{
public:
	static CScriptVarMan* NewL();
	~CScriptVarMan();

	void AddVariableL(const TDesC& aName,const TDesC& aContent);
	void AddVariableL(const TDesC& aName,TInt aVal);
	TInt FindVariable(const TDesC& aName,TPtrC& aContent);
	void DeleteAll();
private:
	CScriptVarMan();
	void ConstructL();
private:
	TInt iIndex;
	CArrayFixFlat<TVar>* iVarArray;
	};

#endif
