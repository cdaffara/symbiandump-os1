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
// A simple program which uses the statically linked dll  "eulibd1b.dll"
// 
//

// standard example header
#include "CommonFramework.h"
#include "InvariantStaticDLL.h"
#include "VariantStaticDLL.h"

_LIT(KTxt1,"statically linked DLL example \n\n");

#ifdef FEATURE_VARIANT_DEFAULT
	_LIT(KTxt2,"Hello from variant_default!");
#endif

#ifdef FEATURE_VARIANT_A
	_LIT(KTxt2,"Hello from variant_a!");
#endif

#ifdef FEATURE_VARIANT_B
	_LIT(KTxt2,"Hello from variant_b!");
#endif

_LIT(KTxtNewLine,"\n");

LOCAL_C void doExampleL()
    {
	console->Printf(KTxt1);

	CInvMessenger* myInvMessage = CInvMessenger::NewLC(*console, KTxt2);
	myInvMessage->ShowMessage();

	console->Printf(KTxtNewLine);

	CVarMessenger* myVarMessage = CVarMessenger::NewLC(*console, KTxt2);
	myVarMessage->ShowMessage();

	console->Printf(KTxtNewLine);
	CleanupStack::PopAndDestroy(); 
	}


