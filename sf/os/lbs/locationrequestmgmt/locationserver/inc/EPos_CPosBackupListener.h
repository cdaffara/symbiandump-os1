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



#ifndef CPOSBACKUPLISTENER_H
#define CPOSBACKUPLISTENER_H

// INCLUDES
#include <e32base.h>
#include <e32property.h>
#include <connect/sbdefs.h>

// CLASS DECLARATION

/**
 * Backup and Restore operation callback interface
 */
class MPosBackupEventCallback
    {
    public:
        /*
        * Called by iBackupListener when a backup or restore 
        * operation either starts.
        */
        virtual void HandleBackupOperationEventL() = 0;
    };

/**
 *  Class to handle backup and restore events
 */
class CPosBackupListener : public CActive
    {
    private:    // Constructor
        CPosBackupListener();

    public:     // Destructor
        ~CPosBackupListener();
        static CPosBackupListener* NewL();

    public:     // New functions
        void Start(MPosBackupEventCallback& aCallback);
        TBool IsBackupOperationOngoing();
        TInt GetCurrentValue(conn::TBURPartType& aState, conn::TBackupIncType& aBackupType);

    private:
        void ConstructL();
        CPosBackupListener( const CPosBackupListener& );
        CPosBackupListener& operator= ( const CPosBackupListener& );

    protected:  // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();

    private:    // Data
        RProperty                   iBackupProperty;
        MPosBackupEventCallback*    iCallback;
    };

#endif // CPOSBACKUPLISTENER_H

// End of File