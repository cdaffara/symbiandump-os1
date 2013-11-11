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
// Declaration of Class ExportTypeRebuildTarget of the elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef EXPORT_TYPE_REBUILD_TARGET_H
#define EXPORT_TYPE_REBUILD_TARGET_H

#include "export_type_target.h"

class DefFile;

/**
class ExportTypeRebuildTarget is dervied from ExportTypeTarget and handles export type  target rebuilding

@internalComponent
@released
*/
class ExportTypeRebuildTarget : public ExportTypeTarget
{

public:
	ExportTypeRebuildTarget(ParameterListInterface* aParameterListInterface);
	~ExportTypeRebuildTarget();

	void ProcessExports();

protected:
	DefFile *iDefFile;

};


#endif // EXPORT_TYPE_REBUILD_TARGET_H

