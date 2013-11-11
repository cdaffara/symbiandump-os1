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
* Declaration of CSsmDiskReserver class.
*
*/

#ifndef SSMDISKRESERVER_H
#define SSMDISKRESERVER_H

#include <e32base.h>
#include <e32property.h>
#include <shareddataclient.h>

/**
 *  Class implementing disk space reservation during boot.
 */
NONSHARABLE_CLASS( CSsmDiskReserver ) : public CBase
    {
public:

    /**
     * Two-phased constructor
     */
	static CSsmDiskReserver* NewL();

    /**
     * Destructor
     */
    virtual ~CSsmDiskReserver();

    /**
    * Waits for startup to finish.
    */
    void WaitStartupFinished();
    
private:

    /**
    * Second phase constructor.
    */
    void ConstructL();

private: // data

    /**
    * Property used to listen startup finish.
    */
    RProperty iProperty;

    /**
    * Session to shared data server.
    */
    RSharedDataClient iSharedData;
    };

#endif // SSMDISKRESERVER_H
