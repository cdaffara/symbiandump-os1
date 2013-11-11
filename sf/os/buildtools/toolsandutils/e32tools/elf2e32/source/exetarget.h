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
// Implementation of the Header file for Class ExeTarget of the elf2e32 tool
// @internalComponent
// @released
// 
//


#ifndef EXETARGET_H
#define EXETARGET_H

#include "elffilesupplied.h"

/**
This class is derived from the base class ElfFileSupplied and is responsible for creation of
EXE Target.

@internalComponent
@released
*/
class ExeTarget : public ElfFileSupplied
{

public:
	ExeTarget(ParameterListInterface* aParameterListInterface);
	virtual ~ExeTarget();
	void ProcessExports();
	void GenerateOutput();

	bool ImageIsDll();

};


#endif // EXETARGET_H


