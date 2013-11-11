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
// polydllrebuild_target.h
// Declaration of Class POLYDLLRebuildTarget of the elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef POLYDLLREBUILD_TARGET_H
#define POLYDLLREBUILD_TARGET_H

#include "export_type_rebuild_target.h"
#include <list>

class Symbol;

typedef std::list<Symbol*> SymbolList;

/**
This class is derived from the base class ExportTypeRebuildTarget and is responsible for
PolyDLL rebuild.

@internalComponent
@released
*/
class POLYDLLRebuildTarget : public ExportTypeRebuildTarget
{

public:
	POLYDLLRebuildTarget(ParameterListInterface* aParameterListInterface);
	~POLYDLLRebuildTarget();
	void ProcessExports();
	void GenerateOutput();
};


#endif // POLYDLLREBUILD_TARGET_H


