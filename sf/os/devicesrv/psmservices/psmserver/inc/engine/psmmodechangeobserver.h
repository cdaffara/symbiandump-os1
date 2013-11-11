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
* Description:  PSM mode change observer
*
*/



#ifndef PSMMODECHANGEOBSERVER_H
#define PSMMODECHANGEOBSERVER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 * Observer for Power Save Mode change completion.
 *
 * For PSM Servers internal use
 *
 * @since S60 5.0
 */
class MPsmModeChangeObserver
    {
public:

    /**
     * This method is called when PSM change is completely done.
     *
     * @param aInfo Information of PSM mode change
     */
    virtual void NotifyPsmModeChangeComplete( const TInt aInfo ) = 0;

    };

#endif //PSMMODECHANGEOBSERVER_H

// End of File


