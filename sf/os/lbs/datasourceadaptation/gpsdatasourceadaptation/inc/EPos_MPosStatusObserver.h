/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MPOSSTATUSOBSERVER_H
#define MPOSSTATUSOBSERVER_H

#include <lbscommon.h>

// FORWARD DECLARATIONS
class TPositionModuleStatus;

// CLASS DECLARATION

/**
* Interface implemented by the EPos server to listen to PSY status
*/
class MPosStatusObserver
    {
    public:  // New functions

        /**
        * Reports that the positioner status has changed.
        *
        * @param aImplementationUid The UID of the CPositioner implementation 
        *   which is the originator of the status updates.
        * @param aStatus The new status.
        */
        virtual void ReportStatus(
        /* IN  */       const TPositionModuleId& aImplementationUid,
        /* IN  */       const TPositionModuleStatus& aStatus
        ) = 0;

    };  

#endif  // MPOSSTATUSOBSERVER_H

// End of File
