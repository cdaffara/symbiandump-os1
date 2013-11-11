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
* Description: Declaration of RestartSys class.
*
*/

#ifndef RESTARTSYS_H
#define RESTARTSYS_H

#include <e32base.h>

/**
* Class implementing system restart.
*/
class RestartSys
	{
public:

    /**
     * System restart with reason EUnknownReset.
     *
     * @return KErrNone if operation succeeded
     */
	IMPORT_C static TInt RestartSystem();

    /**
     * System restart with reason.
     *
     * @param aStartupMode Requested startup mode
     * @return KErrNone if operation succeeded
     */
	IMPORT_C static TInt RestartSystem( TInt aStartupMode );
	};

#endif // RESTARTSYS_H