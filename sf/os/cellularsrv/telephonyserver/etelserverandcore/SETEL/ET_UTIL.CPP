// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32svr.h>
#include "ET_SSTD.H"
#include "etelsize.h"


GLDEF_C void Fault(TEtelFault aFault)
//
// Panic the server
//
	{
	User::Panic(_L("Etel Fault"),aFault);
	}

GLDEF_C void PanicClient(TEtelPanic aPanic,const RMessage2& aMessage)
//
// Panic the client on server side
//
	{
	_LIT(KETelClientFault,"Etel Server");
	aMessage.Panic(KETelClientFault,aPanic);
	}


