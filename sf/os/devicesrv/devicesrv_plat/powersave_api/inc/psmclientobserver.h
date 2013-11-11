/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  PSM Client Observer
*
*/



#ifndef PSMCLIENTOBSERVER_H
#define PSMCLIENTOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <psmtypes.h>

// CLASS DECLARATION

/**
 * PSM Client Observer for Power Save Mode change notification
 *
 *  @lib psmclient.lib
 */
class MPsmClientObserver
    {
public:

    /**
     * This method is called when Power Save Mode is changed.
     *
     * @param aMode Mode that is currently active
     */
    virtual void PowerSaveModeChanged( const TPsmsrvMode aMode ) = 0;

    /**
     * This method is called when there is an error during power save mode change
     * 
     * @param aError System wide error code. 
     *               KErrNoMemory if memory allocation failed, 
     *               KErrAlreadyExists if the requested mode is already in use, 
     *               KErrArgument if the requested mode is not supported.
     *               KErrCompletion if mode change done only partially, i.e. some 
     *               actions are not done for certain reason.
     */
    virtual void PowerSaveModeChangeError( const TInt aError ) = 0;

    };

#endif //PSMCLIENTOBSERVER_H

// End of File


