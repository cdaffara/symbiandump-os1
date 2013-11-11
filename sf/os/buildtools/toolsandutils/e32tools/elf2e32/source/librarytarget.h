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
// Implementation of the Header file for Class LibraryTarget of the elf2e32 tool
// @internalComponent
// @released
// 
//


#if !defined(SYMBIAN_LIBRARYTARGET_H_)
#define SYMBIAN_LIBRARYTARGET_H_

#include "usecasebase.h"
#include <list>

class Symbol;
class DSOHandler;
class DefFile;
typedef std::list<Symbol*> SymbolList;

/**
This class is derived from the base class UseCaseBase and is responsible for creation of
Library Target.It passes the input DEF file to the DEFfile parser to get the Symbol list
and then passes the Symbol List to the DSOHandler to generate the DSO file.

@internalComponent
@released
*/
class LibraryTarget : public UseCaseBase
{

public:
	LibraryTarget(ParameterListInterface* aParameterListInterface);
	virtual ~LibraryTarget();
	int Execute();
	SymbolList* ReadInputDefFile();
	void GenerateOutput(SymbolList* aSymList);

private:
	DSOHandler	*iElfIfc;
	DefFile *iDefFile;
};



#endif // !defined(SYMBIAN_LIBRARYTARGET_H_)

