// GuiCons.cpp
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

#include <eikstart.h>
#include "GuiConsApplication.h"


LOCAL_C CApaApplication* NewApplication()
	{
	return new CGuiConsApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}
