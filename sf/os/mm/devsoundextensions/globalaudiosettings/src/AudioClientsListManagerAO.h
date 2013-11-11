/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   This file contains definitions of Audio Clients List Manager.
*
*/




#ifndef AUDIOCLIENTSLISTMANAGERAO_H
#define AUDIOCLIENTSLISTMANAGERAO_H

#include <e32base.h>
#include <GlobalAudioSettings.h>
#include <e32property.h>

/**
*  Defines audio clients list manager class.
*
*  @lib GlobalAudioSettings.lib
*  @since Series 60 3.2
*/
class CAudioClientsListManagerAO: public CActive
    {
    public:
        // 2 Phase constructor
        static CAudioClientsListManagerAO* NewL(
                                            CGlobalAudioSettings& aGlobalAudioSettings,
                                            RPointerArray<MAudioClientsListObserver>& aArray,
                                            TUint32 aKey );
        // Destructor
        ~CAudioClientsListManagerAO();
        // Starts monitoring audio clients list
        TInt MonitorStart();
        // Stops monitoring audio clients list
        TInt MonitorStop();
        // Returns the audio clients list
        TInt GetAudioClientsList(
                    CGlobalAudioSettings::TAudioClientListType aType,
                    RArray<TProcessId>& aList );

    protected:
        // From CActive begins
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);
        // From CActive ends
    
    private:
        // Constructor
         CAudioClientsListManagerAO(CGlobalAudioSettings& aGlobalAudioSettings,
                					RPointerArray<MAudioClientsListObserver>& aArray,
                					TUint32 aKey );
        // 2nd phase constructor
        void ConstructL();

    private:
        // Handle to property
        RProperty iActiveAudioClients;
        // Reference to CGlobalAudioSettings
        CGlobalAudioSettings& iGlobalAudioSettings;
        // List of Audio Clients List Observer objects
        RPointerArray<MAudioClientsListObserver>& iAudioClientsListObserverArray;        
        //P&S key
        TUint32 iKey;
    };

#endif  // AUDIOCLIENTSLISTMANAGERAO_H

// End of file
