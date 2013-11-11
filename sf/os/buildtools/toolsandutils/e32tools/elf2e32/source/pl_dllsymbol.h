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
// Implementation of the Class DllSymbol for the elf2e32 tool
// @internalComponent
// @released
// 
//

#if !defined(_PL_DLLSYMBOL_H_)
#define _PL_DLLSYMBOL_H_

#include <tools/elfdefs.h>
#include "pl_symbol.h"

/**
The class DLLSymbol properties for both imported and exported symbols.
@internalComponent
@released
*/
class DllSymbol : public Symbol
{

public:
	DllSymbol(char* aName,SymbolType aType, Elf32_Sym* aElfSym, PLUINT32 aSymbolIndex);
	DllSymbol(Symbol* aSymbol, SymbolType aType, bool aAbsent);
	~DllSymbol();
	
	Elf32_Sym	*iElfSym;
	/**
	 * The index of this symbol in the symbol table(required for the hash table while
	 * creating the dso).
	 */
	PLUINT32		iSymbolIndex;

};




#endif // !defined(_PL_DLLSYMBOL_H_)
