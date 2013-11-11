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




/**
 @file
 @internalComponent
 @released
*/

#ifndef POSCOMMONPRIVACYRESOURCES_H
#define POSCOMMONPRIVACYRESOURCES_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CPosDialogCtrl;
class CPosTrace;

// DATA TYPES
class CPosCommonPrivacyResources : public CBase
    {
    public: // Data
        CPosDialogCtrl*         iDialogCtrl;
        TInt                    iLastAssignedRequestId;
#ifdef _DEBUG
        CPosTrace*              iTrace;
#endif
    };

// CLASS DECLARATION

/**
*  Singleton representation for common resources in the privacy server.
*/
class PosCommonPrivacyResources
    {
    public:  // New functions

        /**
        * Installs the holder for common resources in thread local storage.
        * @return KErrNone if ok, otherwise a system wide error code.
        */
        static TInt Install();

        /**
        * Checks if the PosCommonPrivacyResources is installed.
        * @return ETrue if installed, otherwise EFalse.
        */
        static TBool Installed();

        /**
        * Uninstalls the holder for common resources. The common resources are
        * not destroyed by this call.
        */
        static void Uninstall();

        /**
        * Gets a handle to the global dialog controller.
        * @return The global dialog controller.
        */
        static CPosDialogCtrl* DialogCtrl();

        /**
        * Sets the global dialog controller.
        * @param aDialogCtrl The global dialog controller.
        */
        static void SetDialogCtrl(
            /* IN  */   CPosDialogCtrl* aDialogCtrl);

        /**
        * Get a new unique request id for a new CPosReqLogEntry.
        *
        * @return The new unique request id.
        */
        static TInt GetNewRequestId();

#ifdef _DEBUG
        static void SetupTraceL();
        static void TearDownTrace();

        static void Trace(
            /* IN  */ const TDesC& aDescription,
            /* IN  */ const TDesC& aFilename,
            /* IN  */ const TInt aLineNumber
            );
#endif
    };

#endif      // POSCOMMONPRIVACYRESOURCES_H

// End of File


