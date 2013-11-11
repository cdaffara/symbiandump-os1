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
// Implementation of the Header file for DLL Rebuild Target Type
// @internalComponent
// @released
// 
//

#ifndef DLL_REBUILD_TARGET_H
#define DLL_REBUILD_TARGET_H

#include "export_type_rebuild_target.h"

/**
This class is derived from the base class ExportTypeRebuildTarget and is responsible for creation of
DLL Target.

@internalComponent
@released
*/

class DLLRebuildTarget : public ExportTypeRebuildTarget
{

public:
	DLLRebuildTarget(ParameterListInterface* aParameterListInterface);
	~DLLRebuildTarget();	
};

#endif // DLL_REBUILD_TARGET_H

