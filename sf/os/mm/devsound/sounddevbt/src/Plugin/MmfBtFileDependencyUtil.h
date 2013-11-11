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

#ifndef MMFBTFILEDEPENDENCYUTIL_H
#define MMFBTFILEDEPENDENCYUTIL_H

#include <w32std.h>
#include <f32file.h>

_LIT(KSoundDevBtFileName, "C:\\SoundDevBt.txt");

/**
This class is for checking the existence of a file
@internalComponent
*/
class FileDependencyUtil
	{
public:
	static void CheckFileDependencyL();
	};

#endif
