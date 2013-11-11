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
// dll_fb_target.cpp
// Implementation of the Class ExeTarget for the elf2e32 tool
// @internalComponent
// @released
// 
//

#include "dll_rebuild_target.h"

/**
Constructor for the DLLRebuildTarget Class

@internalComponent
@released
*/
DLLRebuildTarget::DLLRebuildTarget(ParameterListInterface* aParameterListInterface) : ExportTypeRebuildTarget(aParameterListInterface) {
}

/**
Destructor for the DLLRebuildTarget Class

@internalComponent
@released
*/
DLLRebuildTarget::~DLLRebuildTarget() {
}




