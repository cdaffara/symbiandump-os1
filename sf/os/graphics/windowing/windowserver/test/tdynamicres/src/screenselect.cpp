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

/**
 @file
*/

#include <e32std.h>
#include <w32std.h>
#include "teflogextensions.h"
#include "screenselect.h"
#include "globalsettings.h"

//Note that this class now just acts as a stub to the CGlobalSettings::SetScreenNoL() method and data, so both are equivalent.

CScreenSelect::CScreenSelect(TInt aScreenNumber, const TDesC& aStepName)
	{
	SetTestStepName(aStepName);
	iScreenNumber = aScreenNumber;
	}

CScreenSelect::~CScreenSelect()
	{
	}


TInt CScreenSelect::Number()
	{
	return	TGlobalSettings::Instance().iScreen;
	}


TVerdict CScreenSelect::doTestStepL()
	{
	SetTestStepError(EFail);

	CGlobalSettings* globalSettings=new (ELeave) CGlobalSettings;
	
	globalSettings->SetLoggerL(Logger());
	TRAPD(err,globalSettings->SetScreenNoL( iScreenNumber));
	
	if (err==KErrNone)
		{
		SetTestStepError(EPass);
		return EPass;
		}
	else
		{
		return EFail;
		}
	}

