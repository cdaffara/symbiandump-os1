// GuiConsApplication.cpp
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

#include "GuiConsDocument.h"
#include "GuiConsApplication.h"
#include <fshell/common.mmh>

const TUid KUidGuiConsApp = { FSHELL_UID_S60_GUICONS };


CApaDocument* CGuiConsApplication::CreateDocumentL()
	{
	return (static_cast<CApaDocument*> ( CGuiConsDocument::NewL( *this ) ) );
	}

TUid CGuiConsApplication::AppDllUid() const
	{
	return KUidGuiConsApp;
	}
