// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the Header file for Class StdExeTarget of the elf2e32 tool
// @internalComponent
// @released
// 
//


#ifndef STDEXE_TARGET_H
#define STDEXE_TARGET_H

#include "export_type_fb_target.h"

/**
This class is derived from the base class ExportTypeFBTarget and is responsible for
creation of STDEXE Target.

@internalComponent
@released
*/
class StdExeTarget : public ExportTypeFBTarget
{

public:
	StdExeTarget(ParameterListInterface* aParameterListInterface);
	virtual ~StdExeTarget();
	bool ImageIsDll();
	bool AllowDllData();
	void GenerateOutput();
	bool WarnForNewExports();
};



#endif // STDEXE_TARGET_H


