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

#include <bautils.h>
#include <f32file.h>
#include "gceavailable.h"

/**
Always returns ETrue.

@return ETrue.
*/

EXPORT_C TBool GCEAvailable()
	{
	RFs fs;
	fs.Connect();
	TBool exists = BaflUtils::FileExists(fs, _L("z:\\resource\\videorenderer\\videorenderer.rsc"));
	fs.Close();
	return exists;
	}

