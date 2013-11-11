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

#include "VariantStaticDLL.h"
#include <e32uid.h>

// construct/destruct

EXPORT_C CVarMessenger* CVarMessenger::NewLC(CConsoleBase& aConsole, const TDesC& aString)
	{
	CVarMessenger* self=new (ELeave) CVarMessenger(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL(aString);
	return self;
	}

CVarMessenger::~CVarMessenger() // destruct - virtual, so no export
	{
	delete iString;
	}

// useful functions

EXPORT_C void CVarMessenger::ShowMessage()
	{
#ifdef FEATURE_VARIANT_A
	_LIT(KFormat1,"FEATURE VARIANT A %S\n");
#else
	_LIT(KFormat1,"FEATURE VARIANT ~A %S\n");
#endif
#ifdef FEATURE_VARIANT_B
	_LIT(KFormat2,"FEATURE VARIANT B %S\n");
#else
	_LIT(KFormat2,"FEATURE VARIANT ~B %S\n");
#endif
	iConsole.Printf(KFormat1, iString); // notify completion
	iConsole.Printf(KFormat2, iString); // notify completion
	}

// constructor support
// don't export these, because used only by functions in this DLL, eg our NewLC()

CVarMessenger::CVarMessenger(CConsoleBase& aConsole) // first-phase C++ constructor
	: iConsole(aConsole)
	{
	}

void CVarMessenger::ConstructL(const TDesC& aString) // second-phase constructor
	{
	iString=aString.AllocL(); // copy given string into own descriptor
    }


GLDEF_C TInt E32Dll(TInt /*aReason*/)
// DLL entry point
	{
	return(KErrNone);
	}
