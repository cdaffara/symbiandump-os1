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


#ifndef __AMASTARTER_H__
#define __AMASTARTER_H__

#include <e32base.h>

#include "StartupCommand.h"

class CSsmLoadAmaStart;

/*
Provides mechanism for starting AMAs from a DSC
@internalComponent
@deprecated
*/
class CAmaStarter : public CBase,  public MStartupCommand
	{
public:
	static CAmaStarter* NewL(const TUid aDscId);
	
	void Execute(TRequestStatus& aCallerStatus);
	
	~CAmaStarter();
	
	void Release();
	
private:
	CAmaStarter(TUid aDscId);
	void DoExecuteL();
	void LoadAmaStartLibraryL();
	
private:
    CSsmLoadAmaStart* iAmaStart;
    RLibrary iAmaStartLib;
	TUid iDscId;
	};

#endif // __AMASTARTER_H__
