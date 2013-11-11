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
// polydllfb_target.h
// Declaration of Class POLYDLLFBTarget of the elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef POLYDLLFB_TARGET_H
#define POLYDLLFB_TARGET_H

#include "export_type_fb_target.h"
#include <list>

class Symbol;

typedef std::list<Symbol*> SymbolList;

/**
This class is derived from the class ExportTypeFBTarget and is responsible for creation of
PolyDll first build.

@internalComponent
@released
*/
class POLYDLLFBTarget : public ExportTypeFBTarget
{

public:
	POLYDLLFBTarget(ParameterListInterface* aParameterListInterface);
	~POLYDLLFBTarget();
	void ProcessExports();
	void GenerateOutput();

};


#endif // POLYDLLFB_TARGET_H


