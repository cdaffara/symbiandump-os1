// GuiConsApplication.h
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

#ifndef __GUICONSAPPLICATION_H__
#define __GUICONSAPPLICATION_H__

#include <aknapp.h>

class CGuiConsApplication : public CAknApplication
	{
public:
	TUid AppDllUid() const;
protected:
	CApaDocument* CreateDocumentL();
	};

#endif // __GUICONSAPPLICATION_H__
