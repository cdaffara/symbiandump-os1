// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

 
#if !defined (__BOOT_MODE_MAPPING_H__)
#define __BOOT_MODE_MAPPING_H__

#include <e32base.h>

class RFs;

/** Serves to associate a boot mode with a specific resource file.
@internalComponent
@deprecated
*/
class TBootModeMapping
	{
public:
void GetResourceFileName(TUint aBootMode, TDes& aResourceFileName, RFs& aFs) const;
	};

#endif // __BOOT_MODE_MAPPING_H__
