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

#include "exexp_fb_target.h"

/**
Constructor for the ExexpFBTarget Class

@internalComponent
@released
*/
ExexpFBTarget::ExexpFBTarget(ParameterListInterface* aParameterListInterface):
ExportTypeFBTarget(aParameterListInterface)
{
}

/**
Destructor for the ExexpFBTarget Class

@internalComponent
@released
*/
ExexpFBTarget::~ExexpFBTarget(){

}

/**
Function to check if the provided input is a DLL.
 
@internalComponent
@released

@result False since EXEXP Target is not a DLL.
*/
bool ExexpFBTarget::ImageIsDll()
{
	return false;
}

/**
Function to check if writable data is allowed.
 
@internalComponent
@released

@result True, since EXEXP targets can have writable data
*/
bool ExexpFBTarget::AllowDllData()
{
	return true;
}
