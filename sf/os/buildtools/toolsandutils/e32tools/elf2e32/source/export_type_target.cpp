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
// Implementation of the Class ExexpTarget for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "export_type_target.h"

/**
Constructor for the ExportTypeTarget Class

@internalComponent
@released
*/
ExportTypeTarget::ExportTypeTarget(ParameterListInterface* aParameterListInterface) : ElfFileSupplied(aParameterListInterface), iDefExports(NULL) {
}

/**
Destructor for the ExportTypeTarget Class

@internalComponent
@released
*/
ExportTypeTarget::~ExportTypeTarget() {
}

/**
Function to generate the output E32 image. Since this is a targettype which can have
export symbols, the output generated can also include DEF File (if any), DSO file and
and E32 image.
 
@internalComponent
@released
*/
void ExportTypeTarget::GenerateOutput() 
{
	WriteDefFile();
	WriteDSOFile();
	WriteE32();
}

/**
Function to check if the provided input is a DLL.
 
@internalComponent
@released

@result True since Export Type Target is a DLL.
*/
bool ExportTypeTarget::ImageIsDll()
{
	return true;
}
