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



#ifndef CPOSMODULESENGINE_H
#define CPOSMODULESENGINE_H

#include <e32base.h>
#include <f32file.h>
#include <lbscommon.h>
#include <centralrepository.h>
#include <ecom/ecom.h>

#include "epos_cposmodulessettings.h"
#include "epos_cposmodulescenrepmanager.h"

class MPosModulesObserver;
class CPosModulesNotifier;
class CPosModuleUpdate;
class CPosModuleIdList;
class CPosPluginsList;
class CPosPluginProperties;

/**
*  The engine which contains info on all installed positioning modules.
*
*  This class is used for accessing central repository and ECom.
*
*  This class contains methods for getting and setting lists for
*  the priorities, availability and cost. Also it supports getting
*  module information and synchronizing the Central Repository
*  with ECom.
*
*  A client can listen for location settings changes by using the method
*  @StartObserveL().
*
*/
NONSHARABLE_CLASS( CPosModulesEngine ) : 
    public CActive,
    public MPosCenrepObserver
    {
    public:

        /**
        * Two-phased constructor.
        * @return A new instance of this class.
        */
        static CPosModulesEngine* NewL();
        virtual ~CPosModulesEngine();

    public:

        /** Starts listening for changes settings.
         *  @param aObserver Change events are notified to this observer.
         *  @panic "LocationSettings"-1 If listening already started. */
        void SetObserverL( MPosModulesObserver& aObserver );

        /** Stops listening for the changes in settings. */
        void RemoveObserver();

        /**
        * Fills parameter with all information about PSY. Leaves
        * if cannot find some information parts (ECom, Cenrep, Priority)
        * @param[in] aModuleId UID of the interested module
        * @param[out] aInfo target container
        */
        void GetModuleInfoL(
        	TPositionModuleId aModuleId,
        	TPositionModuleInfo& aInfo);

        /**
        * Retrieves a list of the existing modules in priority order and leaves
        * it in cleanup stack. Client takes ownership of returned object.
        *
        * Modules marked as invisible are not included in this list.
        *
        * @return List of visible modules
        * @see ModuleIdListL()
        */
        CPosModuleIdList* GetPriorityListLC();

        /**
        * Changes the priority of a module. The priority of the other modules
        * will be shifted so that each module has a unique priority between 0
        * and number of modules - 1. 0 is the highest priority.
        *
        * Note that modules with visibility attribute set to 0 have no priority.
        *
        * @param aModuleId The ID of the module to set priority for.
        * @param aNewPriority The new priority for the module. Priority must be
        *   a number between 0 and number of modules - 1.
        *
        * @leave KErrLocked Several clients are accessing location settings simultaneously.
        * @leave KErrNotFound No module with specified ID is found in priority list.
        * @leave KErrArgument Priority value is out of bounds.
        */
        void SetModulePriorityL( TPositionModuleId aModuleId, TInt aNewPriority );

        /**
        * Changes the priority of a module. The priority of the other modules
        * will be shifted so that each module has a unique priority between 0
        * and number of modules - 1. 0 is the highest priority.
        *
        * @param aOldPriority The module's old priority. Priority must be a
        *   number between 0 and number of modules - 1.
        * @param aNewPriority The new priority for the module. Priority must be
        *   a number between 0 and number of modules - 1.
        *
        * @leave KErrLocked Several clients are accessing location settings simultaneously.
        * @leave KErrArgument Priority value is out of bounds.
        */
        void SetModulePriorityL( TInt aOldPriority, TInt aNewPriority );

        /**
        * Changes the priority of modules. The modules priorities will be set
        * to reflect the module order in the list.
        *
        * @param[in] aModuleIdList The module list including the new priority order
        *   List must have the same set of module IDs as would be returned by
        *   ModuleIdListL() .
        *
        * @leave KErrLocked Several clients are accessing location settings simultaneously.
        * @leave KErrArgument Module ID list is missing some module IDs or
        *                     has module IDs not found in the system.
        */
        void  SetModulePrioritiesL( const CPosModuleIdList& aModuleIdList );

        /**
        * Retrieves the priority of a positioning module. 0 is the highest
        * priority.
        *
        * Note that modules with visibility attribute set to 0 have no priority.
        *
        * @param aModuleId The ID of the module to retrieve priority for.
        * @return The module priority.
        * @leave KErrNotFound No module with specified ID is found in priority list.
        */
        TInt PriorityL( TPositionModuleId aModuleId );

        /**
        * Updates changeable attributes of a positioning module.
        *
        * @param[in] aModuleId The ID of the module, which attributes are going to be updated.
        * @param[in] aUpdate The attributes to update.
        *
        * @leave KErrLocked Several clients are accessing location settings simultaneously.
        * @leave KErrNotFound No module with given ID is installed.
        */
        void UpdateModuleL( TPositionModuleId aModuleId, const CPosModuleUpdate& aUpdate );

        /** Verifies if a module is installed in the system.
         *
         *  @param aModuleId The ID of the module to check for existence
         *  @return ETrue if module exists, EFalse otherwise
         *
         */
        TBool IsModuleInstalled( TPositionModuleId aModuleId );

        /** Returns visibility attribute of a module.
         *
         *  @param aModuleId The ID of the module,
         *                   which visibility attribute is to be checked.
         *  @return ETrue if module exists and visible, EFalse otherwise.
         *
         *  @leave KErrNotFound No module with given ID is found in the system.
         *  @see IsModuleInstalledL()
         */
        TBool IsModuleVisibleL( TPositionModuleId aModuleId );

    protected:  // From CActive

        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

    protected:  // From MPosCenrepObserver

        void HandleRepoChangeL();

    private:

        CPosModulesEngine();
        void ConstructL();

        // Prohibit copy constructor
        CPosModulesEngine( const CPosModulesEngine& );
        // Prohibit assigment operator
        CPosModulesEngine& operator= ( const CPosModulesEngine& );

	private:

        void SynchronizeL();

        /** Reads current settings from cenrep */
		CPosModulesSettings* ReadSettingsL();
		/** Returns iCurrentSettings or reads from cenrep */
        const CPosModulesSettings& SettingsCacheL();
        /** Destroys settings cache */
        void ResetSettingsCache();

		void ListenEcomEvents();

		void ResortModuleL(
			TPositionModuleId aModuleId,
			CPosModulesSettings& aSettings );

        TBool IsLeftBetterThanRight(
			TPositionModuleId aLeftModuleId,
			TPositionModuleId aRightModuleId,
			CPosModulesSettings& aSettings );

		void FillModuleAttributes(
			const CPosPluginProperties& aPlugin,
			CPosModulesSettings::TPosModuleAttributes& aModule );

        void NotifyObserver( const RArray<TPosModulesEvent>& aEvents );

        void DoUpdateSettingsL(
        	TPositionModuleId aModuleId, 
        	const CPosModuleUpdate& aUpdate,
        	CPosModulesSettings& aSettings );
        
        void RemoveUninstalledPluginsL( CPosModulesSettings& aSettings );
        void RemoveUninstalledPluginsL( CPosPostInstalledList& aPostInstalled );
        
        TBool NewRomBasedPluginsFound( CPosModulesSettings& aSettings );

        void AddNewPluginsL( 
    		CPosModulesSettings& aSettings, 
    		CPosPostInstalledList& aPostInstalledList );
			
		void UpdateLastWorkingGPSPsyKeyL();
    private:

        RFs                         iFs;
        REComSession*				iEcom;

        CPosModulesCenRepManager*   iCenRepManager;
        CPosPluginsList*            iPlugins;

		/** Pointer to observer object. Not owned. */
        MPosModulesObserver*    iObserver;
        /** Own copy of settings. Used when client reading settings to avoid calling CenRep. */
        CPosModulesSettings*	iSettingsCache;
        /** Observer's copy of settings. Used when listening for changes for comparison. */
        CPosModulesSettings*	iPreviousSettings;
    };

#endif      // CPOSMODULESENGINE_H
