// viewbase.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef VIEWBASE_H_
#define VIEWBASE_H_

#include <e32base.h>

#include "common.h"

class CConsoleBase;
class CFedBufferBase;

//Main View class inherited by any views able to show data on the screen, either backed by a file buffer or not
class CFedViewBase : public CActive, public MViewController, public MSharedCacheClient, public MDeferredClose, public MKeyConsumer
{
protected:
	CFedViewBase(MConsoleProvider& aConsoleProvider, TInt aPriority = CActive::EPriorityStandard)
		: CActive(aPriority), iConsoleProvider(aConsoleProvider), iConsole(aConsoleProvider.Console()) {}

public:
	void StoreWindow(const TWindow& aWindow) {iWindow = aWindow;}
	virtual CFedBufferBase& Buffer() = 0;

protected:
	MConsoleProvider& iConsoleProvider;
	CConsoleBase& iConsole;
	TWindow iWindow;
};

#endif /*VIEWBASE_H_*/
