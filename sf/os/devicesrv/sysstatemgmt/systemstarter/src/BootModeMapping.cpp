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

#include "BootModeMapping.h"
#include <f32file.h>

/**
Given a boot mode this method will provide the associated resource file name. 
 
@param aBootMode TUint specifying which boot mode a resource file name is requested for.
@param aResourceFileName Descriptor to hold the resource file name.
@return KErrNone  
*/

void TBootModeMapping::GetResourceFileName(TUint aBootMode, TDes& aResourceFileName, RFs& aFs) const
	{
	// the file name format string consists of Z: (drive name), %s for private path (\private\__SID__\), 
	// and file name (SSCForStartupMode%d.rsc, where %d is aBootMode)
	_LIT(KBootModeFileName, "Z:%SSSCForStartupMode%d.rsc");

	TPath privatePath;
	aFs.PrivatePath(privatePath);
	aResourceFileName.Format(KBootModeFileName, &privatePath, aBootMode);
	}
