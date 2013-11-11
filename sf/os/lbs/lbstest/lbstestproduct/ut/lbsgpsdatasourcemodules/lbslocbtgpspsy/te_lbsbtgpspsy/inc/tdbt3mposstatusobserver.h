/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Tt3MPosStatusObserver declaration
*
*/



/**
 @file tdbt3mposstatusobserver.h
 @internalTechnology
*/

#ifndef _TT3MPOSSTATUSOBSERVER_H_
#define _TT3MPOSSTATUSOBSERVER_H_

#include <lbscommon.h>

#include "EPos_MPosStatusObserver.h"    // HACK!

NONSHARABLE_CLASS(Tt3MPosStatusObserver): public MPosStatusObserver
    {
    public:
        Tt3MPosStatusObserver();

    public:  // From MPosStatusObserver

        /**
        * Reports that the positioner status has changed.
        *
        * @param aImplementationUid The UID of the CPositioner implementation 
        *   which is the originator of the status updates.
        * @param aStatus The new status.
        */
        void ReportStatus(const TPositionModuleId& aImplementationUid,const TPositionModuleStatus& aStatus);

    };  


#endif

// End of file


