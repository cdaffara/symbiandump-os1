// GuiConsDocument.h
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

#ifndef __GUICONSDOCUMENT_H__
#define __GUICONSDOCUMENT_H__

#include <akndoc.h>

class CGuiConsAppUi;
class CEikApplication;

class CGuiConsDocument : public CAknDocument
	{
public:
	static CGuiConsDocument* NewL( CEikApplication& aApp );
	static CGuiConsDocument* NewLC( CEikApplication& aApp );
	virtual ~CGuiConsDocument();
public:
	CEikAppUi* CreateAppUiL();
private:
	void ConstructL();
	CGuiConsDocument( CEikApplication& aApp );
	};

#endif // __GUICONSDOCUMENT_H__

