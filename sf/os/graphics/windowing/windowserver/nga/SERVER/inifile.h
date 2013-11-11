// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CIniFile Class definitions
// 
//

#ifndef __INIFILE_H__
#define __INIFILE_H__

#include "server.h"

class CIniSection : public CBase
	{
	friend class CIniFile;
public:
	~CIniSection();
	void AddVariableL(const TDesC& aNewVariable);
private:
	CIniSection(TInt aScreen);
	void ConstructL();
	void ConstructL(const TDesC& aName);
	TBool FindVar(const TDesC &aVarName, TInt &aResult);
	TBool FindVar(const TDesC &aVarName, TPtrC &aResult);
	TBool FindVarName(const TDesC& aVarName, TInt& index);
	static TPtrC VarName(const TDesC& aVarString);
	inline TInt Screen() const;
	inline const TDesC& Name() const;
private:
	TInt iScreen;
	RBuf iName;
	CArrayPtrFlat<TDesC>* iPtrArray ;
	} ;

class CIniFile : public CBase, public MWsIniFile
	{
public:
    static CIniFile* NewL();
	~CIniFile();
	TInt NumberOfScreens() const;
	void FreeData();

public: // from MWsIniFile
	TBool FindVar(const TDesC &aVarName, TPtrC &aResult);
	TBool FindVar(const TDesC &aVarName, TInt &aResult);
	TBool FindVar(const TDesC &aVarName);

	TBool FindVar( TInt aScreen, const TDesC &aVarName);
	TBool FindVar( TInt aScreen, const TDesC &aVarName, TInt &aResult);
	TBool FindVar( TInt aScreen, const TDesC& aVarName, TPtrC &aResult);

	TBool FindVar(const TDesC& aSection, const TDesC &aVarName);
	TBool FindVar(const TDesC& aSection, const TDesC &aVarName, TInt &aResult);
	TBool FindVar(const TDesC& aSection, const TDesC& aVarName, TPtrC &aResult);

private:
    CIniFile();
    void ConstructL();
	void doConstructL(RFile &aFile);
	CIniSection * AddOrFindIniSectionL(TPtr& aSectionName);
	CIniSection * AddOrFindScreenSectionL(TInt aScreen);
	CIniSection * AddOrFindNamedSectionL(const TDesC& aName);
	CIniSection * CreateSectionL(TInt aScreen); // utility function
	CIniSection * CreateSectionL(const TDesC& aSectionName); // utility function
	CIniSection * FindSection(TInt aScreen);
	CIniSection * FindSection(const TDesC& aName);
private:
	RPointerArray<CIniSection> iSectionArray;
	TInt iScreenCount;
	};

GLREF_D CIniFile * WsIniFile;

#endif
