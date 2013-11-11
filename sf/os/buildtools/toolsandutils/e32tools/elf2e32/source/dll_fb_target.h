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
// Implementation of the Header file for DLL First build Target Type
// @internalComponent
// @released
// 
//

#ifndef DLLFB_TARGET_H
#define DLLFB_TARGET_H

#include "export_type_fb_target.h"

/**
This class is derived from the base class ExportTypeFBTarget and is responsible for creation of
DLL Target.It passes the input ELF file to the ELFfile Handler to get the Symbol list
and then passes the Symbol List to the DSOHandler to generate the DSO file and then the E32 image.

@internalComponent
@released
*/
class DLLFBTarget : public ExportTypeFBTarget
{

public:
	DLLFBTarget(ParameterListInterface* aParameterListInterface);
	~DLLFBTarget();
};


#endif // DLLFB_TARGET_H
