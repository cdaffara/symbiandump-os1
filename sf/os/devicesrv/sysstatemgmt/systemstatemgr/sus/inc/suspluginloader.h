// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SUSPLUGINLOADER_H__
#define __SUSPLUGINLOADER_H__

#include <e32std.h>

class TSsmSupInfo;
class CSusPluginFrame;

/**
 Static functions to load Sup DLL files and create the contained MSsmUtility objects.
 
 @released
 @internalComponent
 */
class SusPluginLoader
	{
public:
	static void LoadDllFileLC(RLibrary& aLibrary, const TSsmSupInfo& aSupInfo);
	static CSusPluginFrame* CreatePluginLC(RLibrary& aLibrary, TInt aNewLOrdinal);
	
private:
	static void DoLoadDllFileLC(RLibrary& aLibrary, const TDesC& aFileName, TUid aExpectedUid3);
	static CSusPluginFrame* DoCreatePluginL(const RLibrary& aLibrary, TInt aNewLOrdinal);
	};

#endif
