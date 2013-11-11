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
// THIS FILE MUST BE RENAMED TO THE NAME OF OF YOUR SERVER MAIN ENTRY POINT
// DO NOT EDIT THE FILE CONTENTS
// 
//
	
	
#include "e32base.h"
#include "csecureasbase.h"
#include "tserverstartparams.h"

// sample E32Main
// all servers should use this E32main(!), but in a renamed file.
// Derived classes make use of the abstract method CSecureAsBase::CreateServerLC(const TServerStartParams& aParams)
// For an implmenter that is your "E32Main"

TInt E32Main()
	{
	TServerStartParams params;
	TPtr ptr(reinterpret_cast<TText*>(&params), 0, sizeof(TServerStartParams)/sizeof(TText16));
	User::CommandLine(ptr);
	TInt err = CSecureASBase::ServerMain(params);
	return err;
	}
