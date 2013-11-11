/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*      Active object for getting notification when there is enough 
*      free disk space to regain the original size of the reserve file.
*
*
*/


#ifndef SHARED_DATA_FILE_SYSTEM_NOTIFIER_H
#define SHARED_DATA_FILE_SYSTEM_NOTIFIER_H

// INCLUDES
#include <e32base.h>
#include <s32file.h>

// FORWARD DECLARATIONS
class CDosServer;
class CDosSharedDataBase;

// CLASS DECLARATION

/**
* Active object for getting notification when there is enough free disk space
* to regain the original size of the reserve file.
*/
NONSHARABLE_CLASS(CSharedDataFileSystemNotifier) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor
        */        
        static CSharedDataFileSystemNotifier* NewL( CDosServer& aServer );

        ~CSharedDataFileSystemNotifier();

    public:

        void Start( TInt aTreshold, RFs& aFs, CDosSharedDataBase* sdBase );
        CDosServer& iServer;

    protected:  // From CActive

        void RunL();
        void DoCancel();

    private:

        /**
        * C++ default constructors.
        */        
        CSharedDataFileSystemNotifier( CDosServer& aServer );

    private:    // Data

        RFs iFs;
        CDosSharedDataBase* iSDBase;
    };

#endif      // SHARED_DATA_FILE_SYSTEM_NOTIFIER_H

// End of File
