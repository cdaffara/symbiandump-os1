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

#ifndef PDRSTD_H
#define PDRSTD_H

#include <e32std.h>

/**
@internalComponent
*/
enum TPdrStorePanic
	{
	EPdrBandIndexNegative,
	EPdrHeightIndexOutOfRange,
	EPdrModelIndexOutOfRange,
	EPdrModelUidNotFound,
	EPdrDirectoryIndexOutOfRange,
	EPdrFileIndexOutOfRange,
	EPdrPrinterDeviceExists,
	EPdrPrinterDeviceDoesNotExist,
	
	EPdrControlDoesNotExist,
	EPdrIncorrectFontType,
	EPdrNotPrinting,
	EPdrNoFontInUse,
	EPdrRequiresPrinterPort,
	EPdrBadResourceString,

	EPdrScanLineIndexOutOfRange,
	EPdrBandIndexOutOfRange,
	EPdrPageIndexOutOfRange,
	EPdrEncodedScanLineTooShort,

	EPdrControlAlreadyExists,
	EPdrPageSpecNotSet
	};

/**
@internalComponent
*/
GLREF_C void Panic(TPdrStorePanic aPanic);

#endif
