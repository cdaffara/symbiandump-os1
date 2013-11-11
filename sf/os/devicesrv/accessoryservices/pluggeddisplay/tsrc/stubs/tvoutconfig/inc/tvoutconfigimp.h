/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This file provides the headers used in
*                TVOutConfig Dummy Implementation.
*
*/


#ifndef __TVOUTCONFIGIMP_H__
#define __TVOUTCONFIGIMP_H__

//- Include Files  ----------------------------------------------------------

#include <e32base.h>
#include <e32def.h>
#include <AccessoryServer.h>
#include <AccessoryControl.h>
#include "tvoutconfig.h"
#include "tvoutstubeventsender.h"

_LIT(KTvOutConfigGlobalChunkName, "TvOutConfigGlobalChunk");
_LIT(KTvOutConfigGlobalSemaphoreName, "TvOutConfigGlobalSemaphore");

//- Class Definitions -------------------------------------------------------

class CTVOutStubEventListener;

struct STvOutConfigSharedData
    {
    // Assign constant tag 
    IMPORT_C void TagAssign();
    
    // Check tag and binary sizes for
    // binary compatibility between shared data
    // users
    IMPORT_C void TagCheckL();

    // Zero load data, not tag at the end.
    IMPORT_C void ZeroLoadDataNotTag();

    /**
     * Stores Enable() status
     *
     */
    TBool iEnabled;

    /**
     * Stores HDMI connection status
     *
     */
    TBool iConnected;

    /**
     * Stores HDCP support status
     *
     */
    TBool iHdcpSupported;

    /**
     * Stores Copy protection status
     *
     */
    TBool iCopyProtected;

    /**
     * Stores HDMI configuration set status
     *
     */
    TBool iConfigSet;

    /**
     * Stores HDMI configuration set status
     *
     */
    TBool iHdmiConfigSet;
    
    // Config set error can be read from here once
    // and reseted immedialtely after read
    TInt iConfigSetHdmiDviError;

    // Config set error can be read from here once
    // and reseted immedialtely after read
    TInt iConfigSetTvError;

    enum
    {
    EMaxNumberOfConfigs = 30
    };
    
    
    struct 
        {
        TInt iCount;
        TTvSettings iConfigs[ EMaxNumberOfConfigs ]; 
        } iAnalog;
    
    struct 
        {
        TInt iCount;
        THdmiDviTimings iConfigs[ EMaxNumberOfConfigs ];
        } iHdmi;
    
    THdmiDviTimings iSetHdmiDviSettings;
    
    TTvSettings iTvSettings;
    
		TBool iCVideoConnected;
		TInt iPreviousEvent;
    
    // Let the following two be the last fields. It is used to 
    // check compatibility between creator of this chunk (structure)
    // and other users
    struct
        {
        TUint iSizeOfThisStruct;
        TUint8   iTag[ 10 ];
        } iBinaryProtection;    
    };

/**
Inherited implementation of CTvOutConfig
*/
class CTvOutConfigImp : public CTvOutConfig,
                        public MTVOutStubEventSender
    {
	/**
	 * From TvOutConfig
	 *
	 */
    public:
        CTvOutConfigImp();
        virtual ~CTvOutConfigImp();

        virtual TInt StandByFigureMaxSizeInBytes( TUint& aMaxFigureSize );
        virtual TInt StandByFigure( const TStandByFigure& aStandByFigure );
        virtual TInt Enable();
        virtual TInt Disable();
        virtual TInt OnOffListener( TRequestStatus& aListenerRequest );
        virtual TInt OnOffListenerCancel();
        virtual TBool Enabled();

        virtual TInt GetStandByFigure( TStandByFigure& aStandByFigure );

        virtual TInt HdmiCableListener( TRequestStatus& aListenerRequest );
        virtual TInt HdmiCableListenerCancel();

        virtual TInt GetNumberOfHdmiModes( TUint& aNumberOfModes );
        virtual TInt GetSupportedHdmiMode( TUint aModeNumber, TSupportedHdmiDviMode& aReadMode );
        virtual TInt GetTvHwCapabilities( TTvSettings::TOutput aConnector, THwCapabilities& aReadCapabilities );

        virtual TInt SetConfig( const TTvSettings& aTvSettings );
        virtual TInt SetConfig(const THdmiDviTimings& aTvSettings );
        virtual TInt GetConfig( TTvSettings& aTvSettings );
        virtual TInt GetConfig(THdmiDviTimings& aTvSettings);

        virtual TBool HdmiCableConnected();
        
        virtual TInt CopyProtectionStatusListener(TRequestStatus& aListenerRequest ); //JoVa
        virtual TInt CopyProtectionStatusCancel();
        virtual TBool CopyProtectionStatus();
        virtual TInt CopyProtection(TBool aCopyProtectEnabled );

        virtual TInt SetAvailableTvConfigList( const RArray<TTvSettings>& aAnalogConfigs, const RArray<THdmiDviTimings>& aHdmiConfigs );
        virtual TInt GetAvailableTvConfigList( RArray<TTvSettings>& aAnalogConfigs, RArray<THdmiDviTimings>& aHdmiConfigs );
        virtual TInt AvailableTvConfigListListener( TRequestStatus& aListenerRequest);
        virtual TInt AvailableTvConfigListListenerCancel();
        virtual TInt SettingsChangedListener( TRequestStatus& aListenerRequest );
        virtual TInt SettingsChangedListenerCancel();

        virtual void ConstructL();

	/**
	 * From MTVOutStubEventSender
	 *
	 */
    public:
		virtual void SendEvent( const TUid aCategory, const TInt aEvent, const TInt aArg );

    private:

		void EventHdmiCableConnect( const TInt aArg );
		void EventAnalogCableConnect( const TInt aArg );

	    void EventCopyProtectionStatus( const TInt aArg );
	    
	    void EventSettingChange( const TInt aArg = 0 );


		/**
		 * Cable connection status listener
		 *
		 */
		CTVOutStubEventListener* iCableConnectStatusListener;
		
        /**
         * Copy protection status listener
         *
         */
        CTVOutStubEventListener* iCopyProtectStatusListener;

        /**
         * Setting changes listener
         *
         */
		CTVOutStubEventListener* iSettingChangeListener;
		
        /**
         * Stores HDMI cable listener request
         *
         */
		TRequestStatus* iHdmiCableListenerStatus;

        /**
         * Copy Protection status listener's request status pointer
         *
         */
		TRequestStatus* iCopyProtectListenerStatus;
        
        /**
         * Setting changes listener's request status pointer
         *
         */
		TRequestStatus* iSettingsChangedListenerStatus;
		
		// Let the retrial in case of enable/disable succeed after
		// first attempt is unsuccessfull with error code KErrServerBusy
		TBool iServerBusyReturnedOnce;

		// Shared configuration
		STvOutConfigSharedData* iConfig;

		// Semaphore handle
		RSemaphore iConfigSemaphore;
		
		// Chunk handle
	    RChunk iConfigurationChunk;
	    
	    // Semaphore handle
	    RSemaphore iConfigSemaphoreForPSInput;
	    
	    // Is this instance responsible of updating shared data in case
	    // PS key value has changed.
	    TBool iPSInputResponsibilityGot;

		// Supported HDMI modes
		RArray<TSupportedHdmiDviMode> iAllDviMode;
		RArray<TSupportedHdmiDviMode> iSupportedMode;
		
	    /** Accessory server client */
	    RAccessoryServer iAccServer;
	    
	    /** Accessory control */
	    RAccessoryControl iAccControl;
		
	    /** C-Video accessory generic ID */
	    TAccPolGenericID iCVideoGenericID;
		
		/**
		 * Cable connection status listener
		 *
		 */
		CTVOutStubEventListener* iAnalogCableConnectStatusListener;
	};

#endif //__TVOUTCONFIGIMP_H__

// End of File
