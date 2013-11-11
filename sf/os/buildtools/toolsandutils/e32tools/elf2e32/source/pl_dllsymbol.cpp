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

#include "pl_dllsymbol.h"


/**
Constructor for class DllSymbol
@param aName - symbol name
@param aType - symbol type
@param aElfSym - elf symbol
@param aSymbolIndex - index in the symbol table
@internalComponent
@released
*/
DllSymbol::DllSymbol(char* aName, SymbolType aType, Elf32_Sym* aElfSym, \
					PLUINT32 aSymbolIndex): Symbol(aName, aType), iElfSym(aElfSym), iSymbolIndex(aSymbolIndex)
{
}

/**
Constructor for class DllSymbol
@param aSymbol - symbol name
@param aType - symbol type
@param aAbsent - flag if the symbol is marked absent
@internalComponent
@released
*/
DllSymbol::DllSymbol(Symbol* aSymbol, SymbolType aType, bool aAbsent): Symbol(*aSymbol, aType, aAbsent), iElfSym(NULL), iSymbolIndex(0)
{
}

/**
Destructor for class DllSymbol
@internalComponent
@released
*/
DllSymbol::~DllSymbol(){

}







