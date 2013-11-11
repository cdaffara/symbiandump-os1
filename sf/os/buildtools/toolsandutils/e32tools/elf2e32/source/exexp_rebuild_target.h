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
// Declaration of Class ExExpRebuildTarget of the elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef EXEXP_REBUILD_TARGET_H
#define EXEXP_REBUILD_TARGET_H

#include "export_type_rebuild_target.h"

/**
This class is derived from the class ExportTypeRebuildTarget and handles exexp target rebuild

@internalComponent
@released
*/
class ExExpRebuildTarget : public ExportTypeRebuildTarget
{

public:
	ExExpRebuildTarget(ParameterListInterface* aParameterListInterface);
	~ExExpRebuildTarget();
	bool ImageIsDll();
	bool AllowDllData();
};


#endif // EXEXP_REBUILD_TARGET_H

