// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testutils.h"
	
	
/*
Convert the pixel format enum to a string
@param aMode The display mode
@return On return, it contains the pixel format string
 */
TPtrC TestUtils::DisplayModeToString(const TDisplayMode aMode)
	{
	return KDisplayModeNames[static_cast<TInt>(aMode)];
	}
