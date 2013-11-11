/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Charging Context Controller for checking desired conditions
*                and acting according them in order to ease device's usability
*                during charging.   
*
*/



#ifndef C_CACCSRVCHARGINGCONTEXTCONTROLLER_H
#define C_CACCSRVCHARGINGCONTEXTCONTROLLER_H


#include <sensrvchannelfinder.h>
#include <sensrvorientationsensor.h>
#include <sensrvdatalistener.h>


#include "ChargingContextFSMBody.h"
#include "FiniteStateMachine.h"
#include "AccSrvCentralRepositoryWatch.h"
#include "AccSrvPSIntPropertyWatch.h"
#include "AccSrvResetInactivityTimeTimer.h"

#ifdef __MODULE_TEST__
const TUid KModuleTestUid = { 0x01700000 };
const TUint KModuleTestKeyOrientation = 0x1;
const TUint KModuleTestKeyCharger = 0x2;
const TUint KModuleTestKeyResult = 0x3;
#endif // __MODULE_TEST__


/**
 *  Charging Context Controller for checking desired conditions
 *  and acting according them in order to ease device's usability
 *  during charging. 
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib AccServer.exe
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( CAccSrvChargingContextController ): public CBase, 
    public MAccSrvPSIntPropertyObserver, 
    public MResetActivityTimeTimerObserver,
    public MSensrvDataListener
    {

    friend class CChargingContextFSMBody;
    friend class CChargingContextStateWFCharger;
    friend class CChargingContextStateCharging;
    friend class CChargingContextStateMaintainActivity;
    
public:


    /**
     * Two-phased constructor.
     * @param none.
     */
    IMPORT_C static CAccSrvChargingContextController* NewL();
    

    /**
    * Destructor.
    */
    virtual ~CAccSrvChargingContextController();


// from base class MAccSrvPSIntPropertyObserver


    /**
     * From MAccSrvPSIntPropertyObserver.
     * Function is called by property watcher every time property's value changes.
     *
     * @since S60 5.1
     * @param aCategory The UID that identifies the property category.
     * @param aKey The property sub-key, i.e. the key that identifies the specific property within the category. 
     * @param aNewStatus New property status value. 
     * @return none
     */
    virtual void PSPropertyChanged( 
            TUid    aCategory,
            TInt    aKey,
            TInt aNewStatus );

    
    /**
     * From MAccSrvPSIntPropertyObserver.
     * Function is called by property watcher when property is not found anymore.
     *
     * @since S60 5.1
     * @param aCategory The UID that identifies the property category.
     * @param aKey The property sub-key, i.e. the key that identifies the specific property within the category. 
     * @return none
     */
    virtual void PSPropertyDeleted(
            TUid    aCategory,
            TInt    aKey );


// from base class MResetActivityTimeTimerObserver


    /**
     * From MResetActivityTimeTimerObserver.
     * Function is called by timer watcher every time when timer elapses.
     *
     * @since S60 5.1
     * @param none. 
     * @return none
     */
    virtual void InactivityTimerTimeout();


// from base class MSenssrvDataListener

    /**
     * From MSenssrvDataListener.
     * Indicates data is available in the receiving buffer. A client can read 
     * the data through GetData()-function in the related channel object. Data 
     * is valid until the data received notification occurs again.
     *
     * @since S60 5.1
     * @param[in] aChannel Reference to the related channel object
     * @param[in] aCount Data object count in receiving buffer.
     * @param[in] aDataLost Number of lost data items. Data loss can occur if
     *     the client does not request new data from server fast enough. This 
     *     can happen
     *     when system is under heavy load and the client process has lower
     *     priority than sensor server process. If data loss is a problem, 
     *     consider using higher object count in data listening, which will reduce 
     *     number of expensive IPC calls used in data transmission.
     */
    void DataReceived( CSensrvChannel& aChannel, 
                               TInt aCount, 
                               TInt aDataLost );

    
    /**
     * From MSenssrvDataListener.
     * Data listening failed. 
     * If error was fatal, channel has also been closed and sensor server session 
     * terminated. If error was minor, some data has potentially been lost.
     *
     * @since S60 5.1
     * @param[in] aChannel Reference to the related channel object
     * @param[in] aError Error code.
     */
    void DataError(
            CSensrvChannel& aChannel, 
            TSensrvErrorSeverity aError );
		
		/**
     * From MSenssrvDataListener.
     *
     * @since S60 5.1
     * @param[in] aInterfaceUid
     * @param[in] aInterface
     */
	void GetDataListenerInterfaceL( TUid aInterfaceUid, 
                                    TAny*& aInterface );

    void MaintainActivity();
    
    /**
     * Get the id of current state of FSM.
     *
     * @since S60 5.1
     * @param none. 
     * @return TFSMState Current state identification.
     */
    IMPORT_C TFSMState CurrentStateID();
    
    /**
      * Get the name of current state of FSM.
     *
     * @since S60 5.1
     * @param none. 
     * @return TPtrC Name of the current state.
     */
    IMPORT_C TPtrC CurrentStateName();

protected:


private:


    /**
     * C++ default constructor.
     */
    CAccSrvChargingContextController();


    /**
     * By default Symbian 2nd phase constructor is private.
     * @param none.
     */
    void ConstructL();

    /**
     * Function reads the current device orientation value to shared
     * variable.
     *
     * @since S60 5.1
     * @param none. 
     * @return none
     */
    void GetCurrentOrientation();
    
    /**
     * Function reads the current autolock status to shared
     * variable.
     *
     * @since S60 5.1
     * @param none. 
     * @return none
     */
    void GetCurrentAutolockStatus();

    /**
     * Function reads the current charger status to shared
     * variable.
     *
     * @since S60 5.1
     * @param none. 
     * @return none
     */
    void GetCurrentChargerStatus();
    
    /**
     * Start orientation channel.
     * 
     * @since S60 5.1
     * @param None.
     * @return None.
     */
    void StartOrientationChannel();
    
    /**
     * Start orientation channel.
     * Leavable version.
     * 
     * @since S60 5.1
     * @param None.
     * @return None.
     */
    void StartOrientationChannelL();
    
    /**
     * Stop orientation channel.
     * 
     * @since S60 5.1
     * @param None.
     * @return None.
     */
    void StopOrientationChannel();
    
private: // data

    /**
     * Timer object needed to call User::ResetInactivityTime() 
     * frequently.
     * Own.
     */
    CAccSrvResetInactivityTimeTimer*    iResetInactivityTimeTimerPtr;
    
        
    /**
     * Object for watching charger's (status) property value changes
     * in Central Repository.
     * Own.
     */
    CAccSrvPSIntPropertyWatch*  iChargerStatusWatchPtr ;
    
       
    /**
     * Sensor server's channel finder object pointer.
     * Own.
     */
    CSensrvChannelFinder*   iChannelFinder;

    
    /**
     * Sensor server's channel finder object pointer.
     * Own.
     */    
    CSensrvChannel* iDisplayOrientationChannelPtr;
    
        
    /**
     * FSM Body pointer.
     * Owned. NewL() allocates, but ownwership shifted.
     */
    MFSMBody* iFSMBody;

    
    /**
     * Public Subscribe property value cahnged.
     */
    TBool   iPSPropertyChanged;

    
    /**
     * Last recorded orientation value.
     */    
    TSensrvOrientationData::TSensrvDeviceOrientation iCurrentDeviceOrientation;

      
    /**
     * Indicates that timer has recently elapsed.
     */    
    TBool iResetInactivityTimeTimerElapsed;
    
    /**
     * Last recorded charger status.
     */    
    TInt iCurrentChargerStatus;
    
    /**
     * Flag to indicate if the orientation channel started.
     */
    TBool iOrientationChannelStarted;

#ifdef __MODULE_TEST__
    CAccSrvPSIntPropertyWatch*  iModuleTestOrientationWatchPtr;
    CAccSrvPSIntPropertyWatch*  iModuleTestChargerWatchPtr;
#endif //__MODULE_TEST__ 

    };


#endif // C_CACCSRVCHARGINGCONTEXTCONTROLLER_H
