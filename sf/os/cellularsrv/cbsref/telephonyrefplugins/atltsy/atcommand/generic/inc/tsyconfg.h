// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// @file tsconfig.h
// This contains CTsyConfig which manage to get the modem settings from CommDB.
// 

#ifndef __TSYCONFG_H
#define __TSYCONFG_H

// system include
#include <e32base.h>
#include <d32comm.h>
#include <etelmm.h>
#include <commsdattypesv1_1.h>

// use name space
using namespace CommsDat;

enum TConfigType
	{
	EConfigTypePreInit,
	EConfigTypeInit,
	EConfigTypeConnect,
	EConfigTypeHangUp,
	EConfigTypeFull,
	EConfigTypeQuickInit,
	EConfigTypeDDBugWorkAroundStart,
	EConfigTypeDDBugWorkAroundEnd
	};

/** 
 * CTsyConfig Class
 * Creating the class will load the current modem settings.
 * They may be refreshed by calling ReloadSettings.
 * The ModemName, PortDriverName, CommPortName and String functions 
 * will return descriptors to the relevant settings taken from 
 * the current modem's settings.
 * The CommConfig function will return a reference to the Configuration 
 * structure in the current modem's settings. 
 * All file closing, tidying and memory deallocation should be done 
 * in the ~CTsyConfig destructor.
 */
class CTsyConfig : public CBase
	{
public:
	/**
	 * Factory function.
	 * 
	 * @return A newly created CTsyConfig Object
	 */
	static CTsyConfig* NewL();
	
	/**
	 * Virtual destructor
	 * 
	 */
	~CTsyConfig();
    
	/**
	 * Set current table view.
	 * Try to get current table view by 10 times.
	 */
	void SetCurrentTableViewL();
	
	/**
	 * Get the field value by the name from modem Bearer record table in commdb.
	 * 
	 * @param aStringTag The field name
	 * @param aString The field value
	 */
	TInt ConfigModemStringL(const TDesC& aStringTag, TDes8& aString);
	
	/**
	 * Get the field value by the name from modem Bearer record table in commdb.
	 * 
	 * @param aStringTag The field name
	 * @param aString The field value
	 */
	TInt ConfigModemStringL(const TDesC& aStringTag, TDes16& aString);
	
	/**
	 * Gets the port config settings and masks out appropriate handshaking settings
	 * according to whether state is about to initialise, just connected or about to hang up
	 */ 
	TInt PortConfig(TCommConfig& aConfigPckg, TConfigType aConfigType);
	
	/**
	 * Stores the Speaker Preferences settings internally. These settings are stored on construction
	 * of the CTsyConfig object.
	 * @param aSpeakerSetting Speaker Preference setting
	 */
	void GetSpeakerSettingPref(RCall::TMonitorSpeakerControl& aSpeakerSetting);
	
	/**
	 * Stores the Speaker Volume Preferences settings internally. These settings are stored on construction
	 * of the CTsyConfig object.
	 * @param aSpeakerVolume Speaker Volume setting
	 */
	void GetSpeakerVolumePref(RCall::TMonitorSpeakerVolume& aSpeakerVolume);
	
	/**
	 * Stores the Dial Tone Preferences settings internally. These settings are stored on construction
	 * of the CTsyConfig object.
	 * @param aWaitForDialTonePref Wait for Dial Tone Preference setting
	 */
	void GetWaitForDialTonePref(RCall::TWaitForDialTone& aWaitForDialTonePref);
	
	/**
	 * Opens a handle to CCommsDatabase	and positions the view on the default modem specified 
	 * in the current Connected Modem record.
	 * Note that the TSY does not keep the handle to the CCommsDatabase opened all the time, 
	 * therefore this method is used to open the handle only when needed. The methods using 
	 * this method are also responsible for calling ResetCurrentTableViews() to close the
	 * handle to the CCommsDatabase and to clean up the table views.
	 */
	void GetCurrentTableViewsL();
	
	/**
	 * Gets some commonly used settings from the Location and Modem Table Views and stores them
	 * internally as these are not likely to change. 
	 * If some of these settings are not found in the table view then a predefined default values are
	 * used.
	 */
	void GetLocationModemSettingsL();
	
	/**
	 * Closes the handle to CCommsDatabase and clears the view on the default modem specified 
	 * in the current Connected Modem record. This method needs to be called after 
	 * GetCurrentTableViewsL().
	 */
	void ResetCurrentTableViews();
private:
	/**
	 * Constructor 
	 */
	CTsyConfig();
	
	/**
	 * 2nd constructor
	 * 
	 */
	void ConstructL();
	
	/*
	 * Scan through the table for records containing Phonetsy.tsy
	 * Stop at the first instance of such a record and return the id
	 * 
	 * @param aBearerId The Bearer ID in modem bearer table
	 */
 	void GetModemBearerIdL(TUint32& aBearerId);  	
 	/*
 	 * Scan through the table for records containing MM.TSY
 	 * Stop at the first instance of such a record and return the id
 	 *
 	 * @param aBearerId The Bearer ID in modem bearer table
 	 * @param aLocationId 
 	 */
 	void GetLocationIdL(const TUint32& aBearerId, TUint32& aLocationId); 

private:
	/**
	 * Store the modem configuration for connecting with AT Modem
	 * 
	 */
	TCommConfigV01 iConfig;
	
	/**
	 * The CommDB session
	 * 
	 * Own
	 */
	CMDBSession* iDbSession;
	
	/**
	 * Store the IAP number for the selected modem
	 * 
	 */
	TUint32 iModemBearer;
	
	/**
	 * Location ID
	 * 
	 */
	TUint32 iLocationId;
	
	/**
	 * An integer value that indicates the time to pause after using the dial-ot code
	 * 
	 */
	TInt iInterval;
	
	/**
	 * Monitor speaker control setting flags
	 * 
	 */
	RCall::TMonitorSpeakerControl iSpeakerSetting;
	
	/**
	 * Monitor speaker volume control setting flags
	 * 
	 */
	RCall::TMonitorSpeakerVolume iSpeakerVolume;
	
	/**
	 * Wait for dial tone flags
	 * 
	 */
	RCall::TWaitForDialTone iWaitForDialTonePref;
	};

#endif
