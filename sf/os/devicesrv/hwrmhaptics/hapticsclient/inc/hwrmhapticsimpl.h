/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file of haptics client implementation.
*
*/


#ifndef C_HWRMHAPTICSIMPL_H
#define C_HWRMHAPTICSIMPL_H

#include <hwrmhaptics.h>
#include "hwrmhapticssession.h"

class CHWRMHapticsIVTDataCache;
class MHWRMHapticsPacketizer;
class CHWRMHapticsStatusObserver;
class CHWRMHapticsEffectDataGetter;

/**
*  Haptics client side implementation.
*
*  @lib hwrmhapticsclient.lib
*  @since S60 5.1
*/
NONSHARABLE_CLASS( CHWRMHapticsImpl ) : public CHWRMHaptics
    { 
public:
    
    /**
     * C++ constructor.
     */
    CHWRMHapticsImpl();

    /**
     * Second phase construction.
     * 
     * @param aHapticsCallback @see CHWRMHaptics
     * @param aActuatorCallback @see CHWRMHaptics
     */
    void ConstructL( MHWRMHapticsObserver* aHapticsCallback, 
                     MHWRMHapticsActuatorObserver* aActuatorCallback );
                     
    /**
     * Second phase construction, asynchronous.
     * 
     * @param aHapticsCallback @see CHWRMHaptics
     * @param aActuatorCallback @see CHWRMHaptics
     * @param aStatus @see CHWRMHaptics
     */
    void ConstructL( MHWRMHapticsObserver* aHapticsCallback, 
                     MHWRMHapticsActuatorObserver* aActuatorCallback,
                     TRequestStatus& aStatus );

    /**
     * Destructor.
     */
    virtual ~CHWRMHapticsImpl();

public: // Functions from base classes

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void OpenActuatorL( THWRMLogicalActuators aActuator );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt SupportedActuators( TUint32& aActuators );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void ReserveHapticsL();

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void ReserveHapticsL( TBool aForceNoCCoeEnv );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void ReleaseHaptics();

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual MHWRMHapticsObserver::THWRMHapticsStatus HapticsStatus() const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt SetDeviceProperty( TInt aDevicePropertyType, 
                                    TInt aDevicePropertyValue );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt SetDeviceProperty( TInt aDevicePropertyType, 
                                    const TDesC8& aDevicePropertyValue );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetDeviceProperty( TInt aDevicePropertyType, 
                                    TInt& aDevicePropertyValue );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetDeviceProperty( TInt aDevicePropertyType, 
                                    TDes8& aDevicePropertyValue );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetDeviceCapability( TInt aDeviceCapabilityType, 
                                      TInt& aDeviceCapabilityValue );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetDeviceCapability( TInt aDeviceCapabilityType, 
                                      TDes8& aDeviceCapabilityValue );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetEffectState( TInt aEffectHandle, TInt& aEffectState );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt CreateStreamingEffect( TInt& aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt PlayStreamingSample( TInt aEffectHandle, 
                                      const TDesC8& aStreamingSample );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void PlayStreamingSample( TInt aEffectHandle, 
                                      const TDesC8& aStreamingSample, 
                                      TRequestStatus& aStatus );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt PlayStreamingSampleWithOffset( 
                                    TInt aEffectHandle, 
                                    const TDesC8& aStreamingSample, 
                                    TInt aOffsetTime );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void PlayStreamingSampleWithOffset( 
                                    TInt aEffectHandle, 
                                    const TDesC8& aStreamingSample, 
                                    TInt aOffsetTime, 
                                    TRequestStatus& aStatus );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt DestroyStreamingEffect( TInt aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt ModifyPlayingMagSweepEffect( 
        TInt aEffectHandle, 
        const THWRMHapticsMagSweepEffect& aEffect );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void ModifyPlayingMagSweepEffect( 
        TInt aEffectHandle, 
        const THWRMHapticsMagSweepEffect& aEffect, 
        TRequestStatus& aStatus );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt ModifyPlayingPeriodicEffect( 
        TInt aEffectHandle, 
        const THWRMHapticsPeriodicEffect& aEffect );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void ModifyPlayingPeriodicEffect( 
        TInt aEffectHandle,
        const THWRMHapticsPeriodicEffect& aEffect, 
        TRequestStatus& aStatus );
    
    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt LoadEffectData( const TDesC8& aData, TInt& aFileHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt DeleteEffectData( TInt aFileHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt DeleteAllEffectData();

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt PlayEffect( TInt aFileHandle, 
                             TInt aEffectIndex, 
                             TInt& aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void PlayEffect( TInt aFileHandle, 
                             TInt aEffectIndex, 
                             TInt& aEffectHandle, 
                             TRequestStatus& aStatus );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt PlayEffectRepeat( TInt aFileHandle, 
                                   TInt aEffectIndex, 
                                   TUint8 aRepeat, 
                                   TInt& aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void PlayEffectRepeat( TInt aFileHandle, 
                                   TInt aEffectIndex, 
                                   TUint8 aRepeat, 
                                   TInt& aEffectHandle, 
                                   TRequestStatus& aStatus );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt PlayEffect( const TDesC8& aData, 
                             TInt aEffectIndex, 
                             TInt& aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void PlayEffect( const TDesC8& aData,
                             TInt aEffectIndex, 
                             TInt& aEffectHandle, 
                             TRequestStatus& aStatus );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt PlayEffectRepeat( const TDesC8& aData, 
                                   TInt aEffectIndex, 
                                   TUint8 aRepeat, 
                                   TInt& aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void PlayEffectRepeat( const TDesC8& aData, 
                                   TInt aEffectIndex, 
                                   TUint8 aRepeat, 
                                   TInt& aEffectHandle, 
                                   TRequestStatus& aStatus );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt PlayMagSweepEffect( 
        const THWRMHapticsMagSweepEffect& aEffect, 
        TInt& aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void PlayMagSweepEffect( 
        const THWRMHapticsMagSweepEffect& aEffect,
        TInt& aEffectHandle, 
        TRequestStatus& aStatus );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt PlayPeriodicEffect( 
        const THWRMHapticsPeriodicEffect& aEffect,
        TInt& aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual void PlayPeriodicEffect( 
        const THWRMHapticsPeriodicEffect& aEffect,
        TInt& aEffectHandle,
        TRequestStatus& aStatus );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt PausePlayingEffect( TInt aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt ResumePausedEffect( TInt aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt StopPlayingEffect( TInt aEffectHandle );

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt StopAllPlayingEffects();

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetEffectCount ( TInt aFileHandle, 
                                  TInt& aCount ) const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetEffectDuration ( TInt aFileHandle,
                                     TInt aEffectIndex,
                                     TInt& aEffectDuration ) const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetEffectIndexFromName ( 
        TInt aFileHandle,
        const TDesC8& aEffectName,
        TInt& aEffectIndex ) const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetEffectType( TInt aFileHandle,
                                TInt aEffectIndex,
                                TInt& aEffectType ) const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetEffectName( TInt aFileHandle,
                                TInt aEffectIndex,
                                TDes8& aEffectName ) const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetMagSweepEffectDefinition( 
        TInt aFileHandle,
        TInt aEffectIndex,
        THWRMHapticsMagSweepEffect& aEffect ) const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt GetPeriodicEffectDefinition( 
        TInt aFileHandle,
        TInt aEffectIndex,
        THWRMHapticsPeriodicEffect& aEffect ) const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt InfiniteRepeat() const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt InfiniteDuration() const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt MaxEffectNameLength() const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt MaxDeviceNameLength() const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt MaxCapabilityStringLength() const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt MaxPropertyStringLength() const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt MaxStreamingSampleSize() const;

    /**
     * From base class CHWRMHaptics.
     * @see CHWRMHaptics
     */
    virtual TInt DefaultDevicePriority() const;

private:

    /**
     * Sends haptics reservation command to haptics server, if
     * not reserved already.
     *
     * @param aForceNoCCoeEnv @see ReserveHapticsL
     * 
     * @return KErrNone if successful, otherwise a System wide error code.
     */
    TInt DoReserveHaptics( TBool aForceNoCCoeEnv );

    /**
     * Sends haptics release command to haptics server, if
     * haptics is reserved.
     */
    void DoReleaseHaptics();

    /**
     * Resets the internal iEffectHandlePckg package with the given value.
     * 
     * The packages a used in asynchronous IPC-commands, so that the value 
     * stays available after the call returns (i.e. local variables cannot 
     * be used).
     *
     * @param aNewValue Value for the new instance of the package.
     */
    void ResetIntPckg( TInt& aNewValue );

    /**
     * Finalizes the construction.
     *
     * @param aHapticsCallback @see CHWRMHaptics
     * @param aActuatorCallback @see CHWRMHaptics
     */
    void FinalizeL( MHWRMHapticsObserver* aHapticsCallback,
                    MHWRMHapticsActuatorObserver* aActuatorCallback );

private:

    /**
     * Wrapper for client side session object. Handles also 
     * server startup.
     */
    RHWRMHapticsSession iClient;
    
    /**
     * Message packetizer. Owned.
     */
    MHWRMHapticsPacketizer* iPacketizer;

    /**
     * Haptics status observer pointer. Owned.
     */
    CHWRMHapticsStatusObserver* iStatusObserver;
    
    /**
     * Indicates whether or not haptics is reserved for this client.
     */
    TBool iReserved;

    /**
     * Whether or not to use automatic reservation/release
     * on focus gain/lost.
     */
    TBool iAutoReserve;

    /**
     * Request data buffer for sending commands to server.
     */
    RBuf8 iReqData;

    /**
     * Device handle received, when an actuator is opened.
     */
    TInt iDeviceHandle;
    
    /**
     * The type of the currently open actuator. If none has been
     * opened successfully, the type is EHWRMLogicalActuatorLast.
     */
    THWRMLogicalActuators iOpenedActuator;

    /**
     * Package used to receive effect handle in asynchronous calls
     * to server.
     */
    TPckg<TInt> iEffectHandlePckg;

    /**
     * IVT-data cache.
     * Owned.
     */
    CHWRMHapticsIVTDataCache* iIVTDataCache;
    
    /**
     * Effect data getter.
     * Owned.
     */
    CHWRMHapticsEffectDataGetter* iEffectDataGetter;
    };
    
#endif // C_HWRMHAPTICSIMPL_H   
            
// End of File
