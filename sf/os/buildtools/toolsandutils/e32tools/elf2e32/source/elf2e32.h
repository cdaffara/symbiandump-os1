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
// Implementation of the Header file for Class Elf2E32
// @internalComponent
// @released
// 
//

#ifndef ELF2E32_H
#define ELF2E32_H

#include "pl_common.h"
#include "parametermanager.h"

class UseCaseBase;
class ParameterListInterface;
/**
This class gets the single instance of the ParameterManager.

@internalComponent
@released
*/
class Elf2E32
{

public:
	Elf2E32();
	Elf2E32(int argc, char** argv);
	virtual ~Elf2E32();
	UseCaseBase * SelectUseCase();
	int Execute();
	static void ValidateDSOGeneration(ParameterListInterface *aParameterListInterface, ETargetType aTargetType);
	static void ValidateE32ImageGeneration(ParameterListInterface *aParameterListInterface, ETargetType aTargetType);
	static ParameterListInterface * GetInstance(int aArgc, char ** aArgv);

private:

	ETargetType iTargetType;

	/** Pointer to the ParameterListInterface which is the abstract base class */
	ParameterListInterface * iParameterListInterface;

	/** Pointer to the UseCaseBase and says whether the usecase is CreateLibrary or CreateDLL or CreateEXE*/
	UseCaseBase * iUseCase;

	/** Static Pointer to the ParameterListInterface */
	static ParameterListInterface * iInstance;
};


#endif // ELF2E32_H
