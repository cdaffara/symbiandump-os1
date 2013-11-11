/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MPOSPSYVIEWMANAGER_H
#define MPOSPSYVIEWMANAGER_H

// CLASS DECLARATION

/**
*  Implement this interface in a log class
*/
class MPosPSYViewManager
    {
    public: // New functions
        
        /**
        * Switches to a new view
        * @param aViewId The view to switch to.
        */
        virtual void SwitchViewL(
		/* IN */    const TUid& aViewId
        ) = 0;

    };

#endif      // MPOSPSYVIEWMANAGER_H
            
// End of File
