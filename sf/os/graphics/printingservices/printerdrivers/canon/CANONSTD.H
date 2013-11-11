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

#ifndef CANNSTD_H
#define CANNSTD_H

#include <e32std.h>

/**
@internalComponent
*/
enum TCanonPanic
	{
	ECanonNoFontInUse,
	ECanonControlAlreadyExists,
	ECanonPageSpecNotSet,
	ECanonControlDoesNotExist,
	ECanonRequiresPrinterPort
	};

/**
@internalComponent
*/
GLREF_C void Panic(TCanonPanic aPanic);

#endif
