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
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef MMFFORMATSTANDARDCUSTOMINTERFACES_H
#define MMFFORMATSTANDARDCUSTOMINTERFACES_H

#include <e32std.h>

/**
UID associated with the custom interface MMMFFormatCustomInterfaceBlockLength
*/
const TUid KUidCustomInterfaceMmfDecodeBlockLength = {0x10273837};

/**
Custom interface class to allow CMMFDecode plugin block lengths to be queried
*/
class MMMFDecodeCustomInterfaceBlockLength
	{	
public:
	
	/**
	Get the block length of the CMMFDecode derived class

	@return  The block length
	*/
	virtual TUint FileBlockLength() = 0;
	};


#endif
