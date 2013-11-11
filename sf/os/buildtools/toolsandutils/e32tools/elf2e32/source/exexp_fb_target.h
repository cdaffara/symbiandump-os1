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
// Implementation of the Header file for Class Exexp_FB_Target of the elf2e32 tool
// @internalComponent
// @released
// 
//


#ifndef EXEXP_FB_TARGET_H
#define EXEXP_FB_TARGET_H

#include "export_type_fb_target.h"

/**
This class is derived from the base class ExportTypeFBTarget and is responsible for
creation of EXEXP Target.

@internalComponent
@released
*/
class ExexpFBTarget : public ExportTypeFBTarget
{

public:
	ExexpFBTarget(ParameterListInterface* aParameterListInterface);
	virtual ~ExexpFBTarget();
	bool ImageIsDll();
	bool AllowDllData();

};



#endif // EXEXP_FB_TARGET_H


