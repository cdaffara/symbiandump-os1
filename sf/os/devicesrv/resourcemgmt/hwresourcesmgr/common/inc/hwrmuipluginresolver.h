// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of class CHwrmUiPluginResolver. It encapsulates the logic to select which UI Plugin to load
// 
//

/**
 @file
*/

#ifndef HWRMUIPLUGINRESOLVER_H
#define HWRMUIPLUGINRESOLVER_H

#include <e32base.h>
#include <f32file.h>

NONSHARABLE_CLASS(CHwrmUiPluginResolver) : public CBase
	{
public:	
	IMPORT_C static CHwrmUiPluginResolver* NewL();
	IMPORT_C static CHwrmUiPluginResolver* NewLC();
	IMPORT_C ~CHwrmUiPluginResolver();

	IMPORT_C TInt GetDesiredPluginL(TUid& aImplUid);

private:
	CHwrmUiPluginResolver();
	void ConstructL();
	
	TBool IsTestFilePresent(const TDesC& aFilePath);
	void GetTestFilePath(TDes& aFilePath);
	
private:
	RFs iFs;
	};

#endif
