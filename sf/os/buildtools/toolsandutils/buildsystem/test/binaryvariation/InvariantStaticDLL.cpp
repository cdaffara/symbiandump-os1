// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This program creates a dll.
// 
//

#include "InvariantStaticDLL.h"
#include <e32uid.h>

// construct/destruct

EXPORT_C CInvMessenger* CInvMessenger::NewLC(CConsoleBase& aConsole, const TDesC& aString)
	{
	CInvMessenger* self=new (ELeave) CInvMessenger(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL(aString);
	return self;
	}

CInvMessenger::~CInvMessenger() // destruct - virtual, so no export
	{
	delete iString;
	}

// useful functions

EXPORT_C void CInvMessenger::ShowMessage()
	{
	_LIT(KFormat1,"FEATURE INVARIANT %S\n");
	iConsole.Printf(KFormat1, iString); // notify completion
	}

// constructor support
// don't export these, because used only by functions in this DLL, eg our NewLC()

CInvMessenger::CInvMessenger(CConsoleBase& aConsole) // first-phase C++ constructor
	: iConsole(aConsole)
	{
	}

void CInvMessenger::ConstructL(const TDesC& aString) // second-phase constructor
	{
	iString=aString.AllocL(); // copy given string into own descriptor
    }


GLDEF_C TInt E32Dll(TInt /*aReason*/)
// DLL entry point
	{
	return(KErrNone);
	}
