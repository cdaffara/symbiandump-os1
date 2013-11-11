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
* Description:   This file contains definitions of GlobalAudioSettings.
*
*/




#ifndef GLOBALAUDIOSETTINGSIMPL_H
#define GLOBALAUDIOSETTINGSIMPL_H

#include <e32property.h>
#include <e32base.h>
#include <e32debug.h>
#include "GlobalAudioSettingsData.h"
#include <GlobalAudioSettings.h>
#include <MAudioClientsListObserver.h>

class MAudioSettingsObserver;
class CGlobalAudioSettings;
class CWarningTonesObserverAO;
class CMessagingTonesObserverAO;
class CSilentProfileObserverAO;
class CVibraObserverAO;
class CAudioClientsListManagerAO;
class CMessageToneObserverAO;
class CEmailToneObserverAO;
class CRingingTypeObserverAO;
class CVideoCallToneObserverAO;
class CRingingTone1ObserverAO;
class CRingingTone2ObserverAO;
class CKeypadToneObserverAO;
class CSilenceModeObserverAO;
/**
*  Defines functions that client uses to set phone profile settings.
*
*  @lib GlobalAudioSettings.lib
*  @since Series 60 3.2
*/
NONSHARABLE_CLASS(CGlobalAudioSettingsImpl): public CBase
    {
    public:
        /**
        * function for creating the GlobalAudioSettingsImpl
        *
        */
       static CGlobalAudioSettingsImpl* NewL(
                        CGlobalAudioSettings& aGlobalAudioSettings,
                        MAudioSettingsObserver& aAudioSettingsObserver);
        /**
        * Destructor.
        *
        */
        ~CGlobalAudioSettingsImpl();
        /**
        * Returns warning tones status.
        *
        * @since Series 60 3.2
        * @return TBool aEnable. ETrue if warning tones are enabled else EFalse.
        */
         TBool IsWarningTonesEnabled();
        /**
        * Returns message tones status.
        *
        * @since Series 60 3.2
        * @return TBool aEnable. ETrue if message tones are enabled else EFalse.
        */
        TBool IsMessageTonesEnabled();
        /**
        * Returns silent profile status.
        *
        * @since Series 60 3.2
        * @return TBool aEnable. ETrue if current profile is silent profile else EFalse.
        */
         TBool IsSilentProfileEnabled();
        /**
        * Returns vibra is enabled or not in the current profile status.
        *
        * @since Series 60 3.2
        * @return TBool aEnable. ETrue if current profile vibra active else EFalse.
        */
         TBool IsVibraEnabled();
        /**
        * Returns PublicSilence is enabled or not status.
        *
        * @since Series 60 3.2
        * @return TBool aEnable. ETrue if PublicSilence active else EFalse.
        */
         TBool IsPublicSilenceEnabled();
        /**
        * Returns Message Alert Tone 
        *
        * 
        * The filename selected as the message alert tone in the
        * current profile.
        * 
        *
        * 
        * @return TDesC& aMessageAlertToneName. z:\\resource\\No_Sound.wav is returned if sound is off,else selected filename 
        * is returned
        */
        
        
         TDesC& MessageAlertTone();
         /**
         * Returns Email Alert Tone status
         *
         * 
         * The filename selected as the email alert tone in the
         * current profile 
         * 
         *
         * 
         *  @return TBool aEnable. ETrue if current profile message tone is on else EFalse.
         */
         TBool IsMessageAlertToneEnabled();
         /**
        * Returns Email Alert Tone 
        *
        * 
        * The filename selected as the email alert tone in the
        * current profile 
        * 
        *
        * 
        * @return TDesC& aEmailAlertToneName. z:\\resource\\No_Sound.wav is returned if sound is off,else selected filename 
        * is returned
        */
         TDesC& EmailAlertTone();
          /**
         * Returns Email Alert Tone status
         *
         * 
         * The filename selected as the email alert tone in the
         * current profile 
         * 
         *
         * 
         *  @return TBool aEnable. ETrue if current profile email alert tone is on else EFalse.
         */
         TBool IsEmailAlertToneEnabled();
         /**
         * Returns the RingingType
         *
         * 
         * Ringing type set in the current profile
         * 
         * 
         *
         * 
         * @return TGASRingingType aRingingType. 
         */
         CGlobalAudioSettings::TGASRingingType RingingType();
         /**
         * Returns Videocall Alert Tone 
         *
         * 
         * The filename selected as the video call alert tone in the
         * current profile 
         * 
         *
         * 
         * @return TDesC& aVideoCallAlertToneName. z:\\resource\\No_Sound.wav is returned if sound is off,else selected filename 
         * is returned
         */
         TDesC& VideoCallAlertTone();
         /**
        * Returns Videocall Alert Tone status
        *
        * 
        * The filename selected as the video call alert tone in the
        * current profile 
        * 
        *
        * 
        * @return TBool aEnable. ETrue if current profile videocall alert tone is on else EFalse.
        */
         TBool IsVideoCallAlertToneEnabled();
         /**
        * Returns Ringing Alert Tone 
        *
        * 
        * The filename selected as the ringing alert tone in the
        * current profile 
        * 
        *
        * @return TDesC& aRingingAlertTone1Name. z:\\resource\\No_Sound.wav is returned if sound is off,else selected filename 
        * is returned
        * 
        */
         TDesC& RingingAlertTone1();
         /**
        * Returns Ringing Alert Tone1 status
        *
        * 
        * The filename selected as the ringing alert tone in the
        * current profile 
        * 
        *
        * 
        * @return TBool aEnable. ETrue if current profile ringing alert tone is on else EFalse.
        */
         TBool IsRingingAlertTone1Enabled();
         /**
        * Returns Ringing Alert Tone 2
        *
        * 
        * The filename selected as the ringing alert tone in the
        * current profile for the alternate line
        * 
        *
        * 
        * @return TDesC& aRingingAlertTone2Name. z:\\resource\\No_Sound.wav is returned if sound is off,else selected filename 
        * is returned
        */
         TDesC& RingingAlertTone2();
         /**
        * Returns Ringing Alert Tone 2
        *
        * 
        * The filename selected as the ringing alert tone in the
        * current profile for the alternate line
        * 
        *
        * 
        * @return TBool aEnable. ETrue if current profile ringing alert tone of alternate line is on else EFalse.
        */
         TBool IsRingingAlertTone2Enabled();
         /**
        * Returns Ringing Keypad Volume
        *
        * 
        * The key tone volume selected  in the
        * current profile 
        * 
        *
        * 
        * @return TGASKeypadVolume aKeypadToneVolume.returns EGASKeypadVolumeOff sound is off,else selected volume level 
        * is returned
        */
         CGlobalAudioSettings::TGASKeypadVolume KeyPadToneVolume();
         /**
         * Returns silent mode status.
         *
         * 
         * @return TBool aEnable. ETrue if the silent mode is enabled else EFalse.
         */
         TBool IsSilenceModeEnabled();
        /**
        * Registers audio clients list observer.
        *
        * When registered successfully, the observer will start receiveing
        * callback defined in MAudioClientsListObserver when audio clients list
        * changes.
        *
        * @since S60 3.2
        * @param MAudioClientsListObserver& aObserver. A reference to class
        *           implementing MAudioClientsListObserver interface.
        * @return TInt. KErrNone if successful. KErrAlreadyExists if the observer
        *           is already registered.
        */
        TInt RegisterAudioClientsListObserver(
                                    MAudioClientsListObserver& aObserver );
                
        /**
        * Unregisters audio clients list observer.
        *
        * When unregistered, the observer will no longer receive callback when
        * audio clients list changes.
        *
        * @since S60 3.2
        * @param MAudioClientsListObserver& aObserver. A reference to class
        *           implementing MAudioClientsListObserver interface.
        * @return TInt. KErrNone if successful. KErrNotFound if the observer
        *           is not registered.
        */
        TInt UnregisterAudioClientsListObserver(
                                    MAudioClientsListObserver& aObserver);

        /**
        * Gets audio clients list.
        *
        * On return of this function, the parameter aList will contain active
        * audio client application process ids.
        *
        * The parameter aList will be reset in the case
        *
        * @since S60 3.2
        * @param TAudioClientListType aFilter. Type of client list.
        * @param RArray<TProcessId>& aList. A reference to RArray.
        * @return TInt. KErrNone if successful. KErrBadName if
        *           aType is not one of the TAudioClientListType enumeration.
        *           Otherwise a systemwide error code.
        *           In case of error, the array aList will be reset.
        */
        TInt GetAudioClientsList(
                                CGlobalAudioSettings::TAudioClientListType aType,
                                RArray<TProcessId>& aList );

    
    private:
        CGlobalAudioSettingsImpl(CGlobalAudioSettings &aGlobalAudioSettings,
                                 MAudioSettingsObserver& aAudioSettingsObserver);
        /**
        * Second Phase Costructor for GlobalAudioSettingsImpl Instance
        * @since 3.2
        * @param void
        */
        void ConstructL();
            
    private:
        TGlobalAudioSettings iGlobalAudioSettingsData;
        CGlobalAudioSettings& iGlobalAudioSettings;
        MAudioSettingsObserver&   iAudioSettingsObserver;
        CWarningTonesObserverAO* iWarningTonesObserverAO;
        CMessagingTonesObserverAO* iMessagingTonesObserverAO;
        CSilentProfileObserverAO* iSilentProfileObserverAO;
        CVibraObserverAO* iVibraObserverAO;
        // Object managing Audio Clients List implementation
        CAudioClientsListManagerAO* iCAudioClientsListManagerAO;
        // Object managing Paused Clients List implementation
        CAudioClientsListManagerAO* iPausedClientsListManagerAO;              
        // List of Audio Clients List Observer objects
        RPointerArray<MAudioClientsListObserver> iAudioClientsListObserverArray;
        
        
        CMessageToneObserverAO* iMessageToneObserverAO;
        CEmailToneObserverAO* iEmailToneObserverAO;
        CRingingTypeObserverAO* iRingingTypeObserverAO;
        CVideoCallToneObserverAO* iVideoCallObserverAO;
        CRingingTone1ObserverAO* iRingingTone1ObserverAO;
        CRingingTone2ObserverAO* iRingingTone2ObserverAO;
        CKeypadToneObserverAO* iKeypadToneObserverAO;
        CSilenceModeObserverAO* iSilenceModeObserverAO;
    };


NONSHARABLE_CLASS(CWarningTonesObserverAO) : public CActive
	{
public:
    // Ist Phase constr and Destr
	static CWarningTonesObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
	~CWarningTonesObserverAO();
	void Subscribe();

protected:
	// From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	// Constr and IInd phase constr
	CWarningTonesObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
	void ConstructL();
	CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iWarningTonesProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
	};

NONSHARABLE_CLASS(CMessagingTonesObserverAO) : public CActive
	{
public:
    // Ist Phase constr and Destr
	static CMessagingTonesObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
	~CMessagingTonesObserverAO();
	void Subscribe();

protected:
	// From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	// Constr and IInd phase constr
	CMessagingTonesObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
	void ConstructL();
	CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iMessagingTonesProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
	};
	
NONSHARABLE_CLASS( CSilentProfileObserverAO) : public CActive
	{
public:
    // Ist Phase constr and Destr
	static CSilentProfileObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
	~CSilentProfileObserverAO();
	void Subscribe();

protected:
	// From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	// Constr and IInd phase constr
	CSilentProfileObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
	void ConstructL();
	CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iSilentProfileProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
	};

NONSHARABLE_CLASS(CVibraObserverAO) : public CActive
	{
public:
    // Ist Phase constr and Destr
	static CVibraObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
	~CVibraObserverAO();
	void Subscribe();

protected:
	// From CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	// Constr and IInd phase constr
	CVibraObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
	void ConstructL();
	CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iVibraProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
	};


NONSHARABLE_CLASS(CRingingTypeObserverAO) : public CActive
    {
public:
    // Ist Phase constr and Destr
    static CRingingTypeObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
    ~CRingingTypeObserverAO();
    void Subscribe();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    // Constr and IInd phase constr
    CRingingTypeObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
    void ConstructL();
    CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iRingingTypeProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
    };

NONSHARABLE_CLASS(CMessageToneObserverAO) : public CActive
    {
public:
    // Ist Phase constr and Destr
    static CMessageToneObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
    ~CMessageToneObserverAO();
    void Subscribe();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    // Constr and IInd phase constr
    CMessageToneObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
    void ConstructL();
    CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iMessageToneNameProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
    };

NONSHARABLE_CLASS(CEmailToneObserverAO) : public CActive
    {
public:
    // Ist Phase constr and Destr
    static CEmailToneObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
    ~CEmailToneObserverAO();
    void Subscribe();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    // Constr and IInd phase constr
    CEmailToneObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
    void ConstructL();
    CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iEmailToneNameProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
    };

NONSHARABLE_CLASS(CVideoCallToneObserverAO) : public CActive
    {
public:
    // Ist Phase constr and Destr
    static CVideoCallToneObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
    ~CVideoCallToneObserverAO();
    void Subscribe();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    // Constr and IInd phase constr
    CVideoCallToneObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
    void ConstructL();
    CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iVideoCallToneNameProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
    };

NONSHARABLE_CLASS(CRingingTone1ObserverAO) : public CActive
    {
public:
    // Ist Phase constr and Destr
    static CRingingTone1ObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
    ~CRingingTone1ObserverAO();
    void Subscribe();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    // Constr and IInd phase constr
    CRingingTone1ObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
    void ConstructL();
    CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iRingingTone1NameProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
    };

NONSHARABLE_CLASS(CRingingTone2ObserverAO) : public CActive
    {
public:
    // Ist Phase constr and Destr
    static CRingingTone2ObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
    ~CRingingTone2ObserverAO();
    void Subscribe();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    // Constr and IInd phase constr
    CRingingTone2ObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
    void ConstructL();
    CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iRingingTone2NameProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
    };

NONSHARABLE_CLASS(CKeypadToneObserverAO) : public CActive
    {
public:
    // Ist Phase constr and Destr
    static CKeypadToneObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
    ~CKeypadToneObserverAO();
    void Subscribe();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    // Constr and IInd phase constr
    CKeypadToneObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
    void ConstructL();
    CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iKeypadToneVolumeProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
    };

NONSHARABLE_CLASS(CSilenceModeObserverAO) : public CActive
    {
public:
    // Ist Phase constr and Destr
    static CSilenceModeObserverAO* NewL(CGlobalAudioSettings &aGlobalAudioSettings,
                                         MAudioSettingsObserver& aAudioSettingsObserver,
                                         TGlobalAudioSettings& aGlobalAudioSettingsData);
    ~CSilenceModeObserverAO();
    void Subscribe();

protected:
    // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    // Constr and IInd phase constr
    CSilenceModeObserverAO(CGlobalAudioSettings &aGlobalAudioSettings,
                                                 MAudioSettingsObserver& aAudioSettingsObserver,
                                                 TGlobalAudioSettings& aGlobalAudioSettingsData);
    void ConstructL();
    CGlobalAudioSettings& iGlobalAudioSettings;
    MAudioSettingsObserver&   iAudioSettingsObserver;
    RProperty iSilenceModeProperty;
    TGlobalAudioSettings& iGlobalAudioSettingsData;
    };
#endif // GLOBALAUDIOSETTINGSIMPL_H

// End of file
