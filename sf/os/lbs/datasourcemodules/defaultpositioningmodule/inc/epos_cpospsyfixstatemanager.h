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



#ifndef CPOSPSYFIXSTATEMANAGER_H
#define CPOSPSYFIXSTATEMANAGER_H

#include <e32base.h>
#include <lbs.h>

#include "epos_mposfixstatetimerlistener.h"

class MPosPsyFixStateListener;
class CPosFixStateTimer;
class CPosConstManager;
class CPosPsyListHandler;


/**
* The CPosPsyFixStateManager manages fix state of loaded PSYs.
*/
class CPosPsyFixStateManager : public CBase, public MPosFixStateTimerListener
    {
    public: //Definitions
        /*
        * The enumeration defines PSY fix state
        */
        enum TPsyFixState
            {
            EPsyFixStateUnknown = 0,    ///PSY fix state unkown
            EPsyFixStateYes,            ///PSY is able to give a fix
            EPsyFixStateNo              ///PSY is not able to give a fix
            };

    public: // Constructors and destructor
              
        /**
        * Get an instance of CPosPsyFixStateManager. 
        *
        * This function must be called in pair with
        * ReleaseInstance() .
        *
        * @return The new instance of this class.
        */
        static CPosPsyFixStateManager* GetInstanceL();

        /**
        * Release the instance of CPosPsyFixStateManager.
        *
        * This function must be called in pair with
        * GetInstanceL()
        */
        void ReleaseInstance();

        /**
        * Add a listener.
        *
        * @param aListener Pointer to a new listener
        */
        void AddListenerL( MPosPsyFixStateListener* aListener );
        
        /**
        * Remove a listner.
        * 
        * @param aListener Pointer to the listener.
        */
        void RemoveListener( MPosPsyFixStateListener* aListener );
        
        /**
        * Inform PSY fix state manager that a PSY is loaded.
        * The PSY will be added to PSY list and the fix state
        * of the PSY is maintained by the PSY fix state
        * manager.
        *
        * Client does not need to inform the PSY fix state 
        * manager if a PSY is unloaded. 
        *
        * @param aPsyId Id of the loaded PSY.
        */
        void PsyLoadedL(
            TPositionModuleId aPsyId );
        
        /**
        * Get fix state of a specific PSY.
        *
        * If the PSY has not been laoded, then 
        * EPsyFixStateUnknow will be returned.
        *
        * @param aPsyId Id of the PSY
        * @return Fix state of the PSY
        */
        TPsyFixState GetPsyFixState(
            TPositionModuleId aPsyId );
        
        /**
        * Notify PSY fix state manager that a PSY is used. 
        * 
        * When function SetPsyFixState is called, the location 
        * request has been completed. And PSY is not used any more.
        * 
        * @param aPsyId The ID of the PSY
        * @param aPreviousDeviceStatus The device status before the PSY
        * is used.
        */
        void PsyUsed( 
            TPositionModuleId aPsyId,
            TPositionModuleStatus::TDeviceStatus aPreviousDeviceStatus );
        
        /**
        * Set fix state to a specific PSY. 
        *
        * If the PSY has not been loaded before, this 
        * function does nothing.
        *
        * @param aPsyId The Id of the PSY.
        * @param aLRErr Location request complete error code
        */
        void SetPsyFixState( 
            TPositionModuleId aPsyId, 
            TInt aLRErr );
            
        /**
        * Set fix state of external GPS PSY. This function is used by
        * CPosExternalGpsMonitor
        *
        * @param aPsyId The Id of the PSY.
        * @param aLRErr Location request complete error code
        */
        void SetExtGpsPsyFixState(
            TPositionModuleId aPsyId, 
            TInt aLRErr );
            
    public: //From base class
        /**
        * From MPosFixStateTimerListener
        */
        virtual void TimeshiftTimerTimeout( 
            TPositionModuleId aPsyId );
            
        /**
        * From MPosFixStateTimerListener
        */
        virtual void UnknownTimerTimeout(
            TPositionModuleId aPsyId );

    private:
        /**
        * PSY and fix state structure
        */
        struct TPsyAndFixState
            {
            TPositionModuleId iPsyId; //The Id of the PSY
            TPsyFixState iPsyState; //The state of the PSY
            TInt iLocationRequestCount; //The location request count
            CPosFixStateTimer* iTimer;//Fix state timer
            };

    private:

        /**
        * C++ default constructor.
        */
        CPosPsyFixStateManager();

        /**
        * EPOC constructor.
        */
        void ConstructL();
        
        /**
        * Destructor.
        */
        ~CPosPsyFixStateManager();
        
        /**
        * Find index of the PSY in PSY state array. This function
        * returns KErrNotFound if the PSY is not found in the 
        * PSY state array. Otherwise the index is returned.
        */
        TInt PsyIndex(
            TPositionModuleId aPsyId );
            
        /**
        * Start timeshift timer
        */
        void StartTimeshiftTimer(
            TPsyAndFixState& aFixState,
            TPositionModuleStatus::TDeviceStatus aPreviousDeviceStatus );
            
        /**
        * Notify listener that PSY fix state is changed
        *
        * @param aPsyID The ID of the PSY
        * @param aPreviousFixState Previous fix state
        */
        void NotifyListener(
            TPositionModuleId aPsyId,
            TPsyFixState aPreviousFixState );
            
        // By default, prohibit copy constructor
        CPosPsyFixStateManager(const CPosPsyFixStateManager&);
        // Prohibit assigment operator
        CPosPsyFixStateManager& operator= (const CPosPsyFixStateManager&);

    private: // Data
        //Reference count
        TInt iRefCount;
        
        //List handler to get TTFF and TTNF
        CPosPsyListHandler* iListHandler;
        
        //Const manager
        CPosConstManager* iConstManager;

        //Listner array
        RPointerArray< MPosPsyFixStateListener > iListenerArray;

        //Psy fix state array
        RArray< TPsyAndFixState > iPsyStateArray;
        
    };

#endif      // CPOSPSYFIXSTATEMANAGER_H  
            
// End of File
