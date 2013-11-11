/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/**
@file
@internalComponent
@released
*/

#ifndef CLBSLOCMONITORSESSION_H
#define CLBSLOCMONITORSESSION_H

#include <e32property.h>
#include "tserverstartparams.h"
#include "csecureasbase.h"
#include "csecureserverbase.h"
#include "csecuresessionbase.h"
#include "csecuresessionsubsessionbase.h"
#include "csubsessionbase.h"
#include "messageutils.h"

/**
Client side session of the Location Monitor server.

It derives from the class CSecureSessionSubSessionBase (from Server Framework)
which means that it is a session with support for subsessions (not a subsession).

*/
class CLbsLocMonitorSession : public CSecureSessionSubSessionBase
	{

public:

	void CreateL();				   
	void ServiceMessageL(const RMessage2& aMessage);
	void ServiceMessageError(const RMessage2& aMessage, const TInt aError);

private:

	void HandleDatabaseWipeoutL(const RMessage2& aMessage);

protected:
	// property
	TVersion iVersion;
	};

#endif //CLBSLOCMONITORSESSION_H
