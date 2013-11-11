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
//


#include "amastarter.h"
#include <ssm/ssmloadamastart.h>
#include "ssmdebug.h"

/*
loadamastart.dll will load amastart.dll.
loadamastart.dll is common symbian component which is used to
remove the dependency on MW layer.
 * 
 */

_LIT( KAmaStartDLL, "loadamastart.dll" );
typedef CSsmLoadAmaStart* (*TFuncCreateAmaStartL)(void);

CAmaStarter* CAmaStarter::NewL(const TUid aDscId)
	{
	CAmaStarter* self = new(ELeave) CAmaStarter(aDscId);
	return self;
	}

void CAmaStarter::Execute(TRequestStatus& aCallerStatus)
	{
	TRAPD(err, DoExecuteL());
	
	TRequestStatus* statusValue = &aCallerStatus;
	User::RequestComplete(statusValue, err); 
	}

/*
All the AMAs are loaded from the DSC that corresponds to the iDscId
*/
void CAmaStarter::DoExecuteL()
	{
	LoadAmaStartLibraryL();
	iAmaStart->StartL(iDscId); // even if StartL doesn't leave, it does not mean all AMAs have been started correctly
	}

CAmaStarter::~CAmaStarter()
	{
	delete iAmaStart;
    iAmaStartLib.Close();
	}

CAmaStarter::CAmaStarter(TUid aDscId) 
	:iDscId(aDscId)
	{
	}

void CAmaStarter::Release() 
	{
	delete this;
	}

/**
Load loadamastart.dll
*/
void CAmaStarter::LoadAmaStartLibraryL()
    {
    SSMLOGLEAVEIFERROR(iAmaStartLib.Load(KAmaStartDLL));
    TFuncCreateAmaStartL amaStart = reinterpret_cast<TFuncCreateAmaStartL>(iAmaStartLib.Lookup(1));
    SSMLOGLEAVEIFTRUE(amaStart == NULL, KErrArgument);
    iAmaStart = amaStart();
    SSMLOGLEAVEIFNULL(iAmaStart);
    }

