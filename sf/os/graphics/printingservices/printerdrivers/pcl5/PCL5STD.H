// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef PCL5STD_H
#define PCL5STD_H

#include <e32std.h>

/**
@internalComponent
*/
enum TPcl5Panic
	{
	EPcl5NoFontInUse,
	EPcl5ControlAlreadyExists,
	EPcl5PageSpecNotSet,
	EPcl5ControlDoesNotExist,
	EPcl5RequiresPrinterPort
	};

/**
@internalComponent
*/
GLREF_C void Panic(TPcl5Panic aPanic);

#endif
