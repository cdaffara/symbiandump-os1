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

#ifndef __SUSPLUGINFRAME_H__
#define __SUSPLUGINFRAME_H__

#include <e32base.h>

class MSsmUtility;

/**
 Frame to contain MSsmUtility objects.
 
 @released
 @internalComponent
 */
class CSusPluginFrame : public CBase
  	{
public:
	static CSusPluginFrame* NewL(TLibraryFunction aNewLFunc, TInt32 aNewLOrdinal);
	~CSusPluginFrame();
	void  SetLibrary(RLibrary& aLibrary); // take ownership
	TFileName FileName() const;
	TInt NewLOrdinal() const;

	//MSsmUtility
	void InitializeL();
	void StartL();
	void Release();
	
private:
	CSusPluginFrame();
	void ConstructL(TLibraryFunction aNewLFunc, TInt32 aNewLOrdinal);
	
private:
	MSsmUtility* iPlugin;
	RLibrary iLibrary;
	TInt32 iNewLOrdinal;
	};
	
#endif
