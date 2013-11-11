/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * CTVOutConfigForComposite class declaration.
 *
 */

#ifndef TVOUTCONFIGFORCOMPOSITE_H
#define TVOUTCONFIGFORCOMPOSITE_H

// INCLUDES
#include <e32base.h>
#include <tvoutconfigdef.h>
#include "myasynconeshottimercallback.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CTvOutConfig;
class MFSMForBody;
class CRepository;
class CMyAsyncOneShotTimer;
class TTvSettings;

// CLASS DECLARATION

/**
 *  TV Out Configurer for Composite Cable Status FSM.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib none.
 *  @since TB 9.2
 */
NONSHARABLE_CLASS( CTVOutConfigForComposite ): public CActive,
public MMyAsyncOneShotTimerCallback
    {
public: // Constructors and destructor

    /**
     * Symbian two phased constructor.
     * 
     * @since S60 TB9.2
     * @param aFSMBody A FSM.
     * @return CTVOutConfigForComposite*
     */
    static CTVOutConfigForComposite* NewL( MFSMForBody& aFSM );

    /**
     * Destructor.
     */
    virtual ~CTVOutConfigForComposite();

public: // New methods


    /**
     * Set list of available Tv-configurations.
     * 
     * @since TB9.2
     * @param   aSettingAfterCableConnected Tells whether parameter setting
     *          is requested after cable was connected. If not true then
     *          setting is requested because of a changed value of a parameter.
     * @return Symbian error code.
     */
    void SetConfig( TBool aSettingAfterCableConnected );
    
    /**
     * Listen for setting changes in TV Out Configuration.
     * 
     * @since TB9.2
     * @param  None.
     * @return void.
     *           
     */
    void ListenSettingsChanges();
    
    /**
     * Get pointer of CTvOutConfig.
     * 
     * @since TB9.2
     * @param  None.
     * @return CTvOutConfig* Pointer to CTvOutConfig.
     */
    CTvOutConfig* GetTvOutConfig();
    

    /**
     *  Enables Tv-out. Latest SetConfig() settings are used.
     *
     * @since TB9.2
     * @param  None.
     * @return void.
     */
    void Enable();

    /**
     * Disables Tv-out.
     * 
     * @since TB9.2
     * @param  aAnalogConfigs  Available Analog Tv configurations.
     * @param  None.
     * @return void.
     */
    void Disable();

	TBool UpdateOverscanValues();
	
private:

    /**
     * Initializes the driver with the values from the Central Repository.
     * @aBootTime indicates, if method is called during phone boot
     * @return KErrNone if all went well, system wide error code otherwise.
     */
    TInt InitializeDriverL();

    /**
     * Return the current value of the tv system.
     * @return setting value
     */
    TInt TvSystemL();

    /**
     * Return the current value of the aspect ratio.
     * @return setting value
     */
    TInt AspectRatioL();

    /**
     * Return the current value of the flicker filter.
     * @return setting value.
     */
    TInt FlickerFilterL();

    /**
     * Switching value from 1 to 0 and back
     */
    void ConvertFlickerFilter( TInt& aValue );

    /**
     * Converting aspect ratio value for display driver
     */
    void ConvertAspectRatio( TInt& aValue );

    /**
     * Converting tv system value for display driver
     */
    void ConvertTvSystem( TInt& aValue );
    
    /**
     * Resolving the Pixel Aspect Ratio denominator and numerator
     */
    void UpdatePixelAspectRatio( TTvSettings& aSettings );

private: // Constructors

    /**
     * C++ constructor.
     * @param aFSMBody A FSM.
     */
    CTVOutConfigForComposite( MFSMForBody& aFSMForBody );

    /**
     * Symbian 2nd phase constructor.
     * 
     */
    void ConstructL();

private:
    // Functions from base classes

    /**
     * RunL()
     */
    void RunL();

    /**
     * DoCancel()
     */
    void DoCancel();

    /**
     * From MMyAsyncOneShotTimerCallback
     * This is called when one shot asyncronous timer has elapsed.
     * 
     * @since TB9.2
     * @param  None.
     * @return void.
     *           
     */
    virtual void OneShotTimeout();

private:
    // Functions from base classes


private:
    // Data

    enum
        {
        EWaitTimeInMsForTvOutConfigBusyServer = 50000
        };

    enum TOpt
        {
        EOptUndefined = 0,
        EOptEnableTv,
        EOptDisableTv,
        EOptIdle,
        EOptSetting,
        EOptStartListenForSettingsChanges,
        EOptListenForSettingsChanges,
        EOptDisableTVBeforeSetting
        };

    // A FSM pointer.
    // Not own.
    MFSMForBody& iFSM;

    // Handle to TV-out config
    // Own
    CTvOutConfig* iTvOutConfig;

    // Handle to the Central Repository.
    // Own
    CRepository* iRepository;

    // Next operation
    TOpt iNextOpt;

    // Tells whether parameter setting is requested after
    // cable was connected. If not true then setting is requested
    // because of a changed value of a parameter.
    TBool iSettingAfterCableConnected;

    // iTvConfig->Disable() called already once
    // before trying iTvConfig->Setting() again.
    TBool iDisabledOnceBeforeSetting;

    // Asynchronous timer 
    CMyAsyncOneShotTimer* iMyAsyncOneShotTimer;

    // Time out reason and next opt after timer has 
    // elapsed
    TOpt iTimeOutReason;
	
    // Horizontal overscan in pixels (1% == 100)
    TInt iHOverscan;

    // Vertical overscan in pixels (1% == 100)
    TInt iVOverscan;
    };

#endif // TVOUTCONFIGFORCOMPOSITE_H
