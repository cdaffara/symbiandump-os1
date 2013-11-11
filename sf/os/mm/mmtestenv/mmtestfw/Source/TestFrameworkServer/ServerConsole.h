// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SERVERCONSOLE_H__
#define __SERVERCONSOLE_H__

#include <e32std.h>
#include <e32base.h>
#include <e32cons.h>

#include <testframework.h>

/**
 *
 * Active console for test input.
 * To be used where manual control of testing is required
 *
 * @xxxx
 *
 */
class CServerConsole : public CBase
	{
public:
	static CServerConsole* NewL(const TDesC& aName);
	~CServerConsole();

	void SetInstructionsL(const TDesC& aInstructions);

	// accessor
	CConsoleBase* Console() const;

private:
	CServerConsole();
	void ConstructL(const TDesC& aName);

private:
	CConsoleBase* iConsole;
	TRectBuf iWindow;
	HBufC* iWindowName;
	HBufC* iInstructions;
	};

#endif // __SERVERCONSOLE_H__
