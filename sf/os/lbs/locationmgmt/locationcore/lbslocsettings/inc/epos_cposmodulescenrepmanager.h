/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef EPOS_CPOSMODULESCENREPMANAGER_H_
#define EPOS_CPOSMODULESCENREPMANAGER_H_

#include <e32base.h>

class CRepository;
class CPosModulesSettings;
class CPosPostInstalledList;

/** Observer interface for listening for changes in Central Repository */
class MPosCenrepObserver
    {
    public:
        virtual void HandleRepoChangeL() = 0;
    };

/** Helper class for tasks related to central repository. It reads settings from,
 *  writes to appropriate cenrep keys and listens for changes, informing
 *  observer MPosCenrepObserver. */
NONSHARABLE_CLASS( CPosModulesCenRepManager ) : public CActive
{
	public:

        /** Two-phased constructor. */
        static CPosModulesCenRepManager* NewL();

        /** Destructor. */
        virtual ~CPosModulesCenRepManager();
        
        /** Resets factory keys copying their values to working keys, 
         *  if working priority list is empty. 
         *  Used to prepare repository after backup from old implementation
         *  of this component. */
        void InitWorkingAndResetFactoryKeysL();

        /** Reads current module settings from Central Repository
         * (priority list, inivisible list and version working keys ) */
		CPosModulesSettings* LoadSettingsL();

        /** Reads factory module settings from Central Repository
         * (priority list, inivisible list and version factory keys ) */
		CPosModulesSettings* LoadFactorySettingsL();

        /**
        * Updates Central Repository with current state of modules settings.
         * (priority list, inivisible list and version working keys ) */
		void SaveSettingsL( const CPosModulesSettings& aSettings );

        /** Reads lists of post-installed modules from Central Repository */
		CPosPostInstalledList* LoadPostInstalledListL();

        /** Updates lists of post-installed modules in Central Repository */
		void SavePostInstalledListL( const CPosPostInstalledList& aIdList );

        /** Starts transaction and pushes rollback to cleanup stack */
    	void StartTransactionLC();
        /** Commits current transaction and pops rollback from cleanup stack 
         *  @return 
         *  - KErrNone if transaction succeeded, transaction is ok
         *  - KErrLocked if other client was faster, transaction should be repeated
         *  - other system error, return to client */
    	TInt CommitTransactionX();
        
        /** Cancels running transcation and pops rollback from cleanup stack */
        void CancelTransactionX();

        /** Starts listening for changes in Central Repository keys.
         *  @param aObserver Called whenever location settings keys
         *                   are modified in Central Repository. */
        void StartListeningL( MPosCenrepObserver& aObserver );

        /** Stops listening for the changes in Central Repository keys. */
        void StopListening();

    protected:  // From CActive

        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

	private:
		CPosModulesCenRepManager();
		void ConstructL();
		
		CPosModulesSettings* LoadSettingsL(
				TInt32 aPrioritiesKey, 
				TInt32 aInvisibleKey,
				TInt32 aVersionKey );
				
		void InitWorkingKeysL();
		void ResetFactoryKeysL();

		HBufC* ReadKeyLC( TUint32 aKeyId );
		void WriteKeyL( TUint32 aKeyId, const TDesC& aValue );

        void ListenCenrepEvents();

	private:
	    /** Own repository instance */
		CRepository* 			iRepository;
		/** Pointer to observer. Not owned. */
        MPosCenrepObserver*     iObserver;
};

#endif /*EPOS_CPOSMODULESCENREPMANAGER_H_*/
