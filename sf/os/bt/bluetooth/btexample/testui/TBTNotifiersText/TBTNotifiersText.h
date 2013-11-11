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

#ifndef TBTNOTIFIERSTEXT_H
#define TBTNOTIFIERSTEXT_H

//  Include Files

#include <e32base.h>
#include <btmanclient.h>

//  Function Prototypes

GLDEF_C TInt E32Main();

class CBTNotifierConsole : public CActive
	{
public:
	static CBTNotifierConsole* NewL();
	static CBTNotifierConsole* NewLC();

	void RunL();
	void DoCancel();

	~CBTNotifierConsole();

private:
	CBTNotifierConsole();
	void ConstructL();
	void RequestKey();
	void DrawMenu();
	
	void TestLegacyPinL();
	void TestAuthorisationL();
	void TestDeviceSearchL();
	void TestNumericComparisonL();
	void TestPasskeyL();
	void TestPinL();
	
private:
	CConsoleBase* iConsole;
	};

#endif  // TBTNOTIFIERSTEXT_H

