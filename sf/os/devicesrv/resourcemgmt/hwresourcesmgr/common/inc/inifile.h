// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declares the class CIniFile for accessing ini file.
// 
//

/**
 @file
*/

#ifndef INIFILE_H
#define INIFILE_H

#include <e32base.h>

const TInt KTokenSize=32;

NONSHARABLE_CLASS(CIniFile) : public CBase 
	{
public:
	enum TIniPanic
		{
		ESectionNameTooBig,
		EVarNameTooBig
		};

public:
	static IMPORT_C CIniFile* NewL(const TDesC& aName, const TDesC& aPath);
	virtual IMPORT_C ~CIniFile();
	IMPORT_C TBool FindVar(const TDesC &aSection, const TDesC &aVarName, TPtrC &aResult);

protected:
	void Panic(TIniPanic aPanic);
	CIniFile();
	void ConstructL(const TDesC& aName, const TDesC& aPath);

protected: // owned
	HBufC* iName; // full name of ini file
	HBufC* iToken;
	TPtr iPtr;
	};

#endif // INIFILE_H
