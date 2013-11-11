/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CPOSPSYLISTHANDLER_H
#define CPOSPSYLISTHANDLER_H

#include <e32base.h>
#include <lbs.h>
#include "epos_mposmodulesettingsmanager.h"
#include "epos_mposmodulesettingslistener.h"
#include "epos_mpospsylistlistener.h"
#include "epos_mpospsyfixstatelistener.h"

class MPosModuleSettingsManager;
class CPosPsyFixStateManager;
class CPosConstManager;

/**
* The CPosPsyListHandler manages dynamic list that is used by Default
* Proxy to make location requests.
*/
class CPosPsyListHandler : 
        public CBase, 
        public MPosModuleSettingsListener,
        public MPosPsyFixStateListener
    {
    public: // Constructors and destructor
              
        /**
        * Get an instance of CPosPsyListHandler.    
        *
        * This function must be called in pair with 
        * ReleaseInstance()
        *
        * @return The new instance of this class.
        */
        static CPosPsyListHandler* GetInstanceL();
        
        /**
        * Release the instance of CPosPsyListHandler
        *
        * This function must be called in pair with
        * GetInstanceL()
        */
        void ReleaseInstance();

        /**
        * Set module settings manager. If the manager is already set,
        * this function does nothing.
        * 
        * @param aManager Reference to module settings manager.
        */
        void SetModuleSettingsManagerL(
            MPosModuleSettingsManager& aManager );
        
        /**
        * Destructor.
        */
        ~CPosPsyListHandler();

    public: // Functions from base classes
        /**
        * Add a PSY list change listener
        * 
        * @param aListener Pointer to the PSY list change listener
        */
        void AddListenerL(
            MPosPsyListListener* aListener );
            
        /**
        * Remove a PSY list change listener.
        *
        * @param aListener Pointer to the PSY list change listener
        */
        void RemoveListener(
            MPosPsyListListener* aListener );
        
        /**
        * Get module list.
        *
        * @return aPsyList The PSY list in priority order.
        * the aPsyList will be emptied wether this function 
        * succeed or not.
        */
        void GetPsyListL(
            RArray< TPositionModuleId >& aPsyList );
            
        /**
        * Get external GPS PSY module list.
        *
        * @return aPsyList The PSY list contains all external
        * GPS modules. The list will be emptyied first.
        */
        void GetExtGpsPsyListL(
            RArray< TPositionModuleId >& aPsyList );
            
        /**
        * Check if first GPS PSY is located on external device.
        *
        * If there is no GPS PSY available in the list, this 
        * function will return ETrue
        */
        TBool IsFirstGpsPsyExternal() const;
            
        /**
        * Check if the PSY is based on Network technology.
        */
        TBool IsModuleNetworkBased(
            TPositionModuleId aModuleId ) const;
            
        /**
        * Check if the requested class is supported by the PSY
        */
        TBool IsClassSupported(
            TUint32 aClassType,
            TPositionModuleId aModuleId ) const;
            
        /**
        * Get TTFF and TTNF value of a specific PSY.
        *
        * @return KErrNone if successful. If the module is not found
        * then KErrNotFound is returned.
        */
        TInt GetTtffAndTtnf(
            TPositionModuleId aModuleId,
            TTimeIntervalMicroSeconds& aTtff,
            TTimeIntervalMicroSeconds& aTtnf ) const;
            
    protected: //From base class
        /*
        * From MPosModulesObserver.
        * Called when changes are made in the modules settings
        */
        virtual void HandleModuleSettingsChangedL(
            TPosModulesEvent aEvent );
            
        /**
        * From MPosPsyFixStateListener
        */
        virtual void PsyFixStateChanged( 
            TPositionModuleId aModuleId,
            CPosPsyFixStateManager::TPsyFixState aFixState ); 
            

    private:

        /**
        * C++ default constructor.
        */
        CPosPsyListHandler();

        /**
        * EPOC constructor.
        */
        void ConstructL();
        
        /**
        * Update module list
        */
        void UpdateModuleListL();
        
        /**
        * Find module in module info array.
        * This function returns KErrNotFound if module is not 
        * found in the array.
        */
        TInt ModuleIndex( TPositionModuleId aModuleId ) const;
        
        /**
        * Notify listener that PSY list is changed
        */
        void NotifyListeners( 
            const TPosPsyListChangeEvent& aEvent );

        /**
        * Check if the module is GPS type of technology
        */
        TBool IsGpsTechnology( 
            const TPositionModuleInfo& aModuleInfo ) const;
            
        /**
        * Move PSY to the top of PSY list. If aId is KPositionNullModuleId,
        * nothing will be done. Internal GPS PSY will not be moved if the 
        * top GPS PSY on external device and fix state is Yes.
        *
        * @return ETrue if the list is modified. Otherwise returns EFalse.
        */
        TBool MovePsyToTop( TPositionModuleId aId );

        // By default, prohibit copy constructor
        CPosPsyListHandler(const CPosPsyListHandler&);
        // Prohibit assigment operator
        CPosPsyListHandler& operator= (const CPosPsyListHandler&);
        
    private: // Data
        //Reference count
        TInt iRefCount;
        
        //Fix state manager
        CPosPsyFixStateManager* iFixStateManager;
        
        //Const manager
        CPosConstManager* iConstManager;

        //Listener array
        RPointerArray < MPosPsyListListener > iListenerArray;

        //modules
        MPosModuleSettingsManager* iModulesSettings;
        
        //Module info array
        RArray< TPositionModuleInfo > iModuleInfoArray;
        
    };

#endif      // CPOSPSYLISTHANDLER_H  
            
// End of File
