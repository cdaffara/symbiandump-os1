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
// @file globlephonemanager.h
// This contains CGlobalPhonemanager which manage the global phone status and call info manager.
// 

#ifndef __GLOBALPHONEMANAGER_H__
#define __GLOBALPHONEMANAGER_H__

// system include
#include <etel.h>
#include <etelmm.h>
#include <etelqos.h>
#include <ctsy/ltsy/mltsydispatchphonebookinterface.h>

// user include
#include <cmmutility.h>
#include "cltsypacketserviceshandler.h"

/**
 *  Define the prort access type
 */
enum TPortAccess
	{
	EPortAccessAllowed,
	EPortAccessDenied
	};

/**
 * Define the phone status for phone initialization
 */ 
enum TPhoneInitStatus
	{
	EPhoneNotInitialised,
	EPhoneInitialising,
	EPhoneInitialiseCancelling,
	EPhoneInitialised
	};

/**
 * Define the singnal info
 * 
 */ 
struct TTsySignalInfo
	{
	TInt iSignalStrength;
	TInt iBar;
	};

/**
 * Define the storage type
 */
typedef TBuf8<2> TStorageType;

/**
 * Define the global phone status
 */
struct TPhoneStatusTSY
	{
	RPhone::TModemDetection iModemDetected;
	RPhone::TMode iMode;
	TUint iDataAndFaxFlags;
	TPhoneInitStatus iInitStatus;
	TPortAccess iPortAccess;
	TInt iWaitForCarrierTime;
	TBool iDataPortLoaned;
	TUint iSupportedPhoneBookStorageFlag;
	TInt iSupportedPhoneBookCount;
	TStorageType iLastAccessedPhoneBook;
	TBool iNetworkChanged;
	RMobilePhone::TMobilePhoneRegistrationStatus iRegistrationStatus;
	RMobilePhone::TMobilePhoneLocationAreaV1 iLocationArea;
	RMobilePhone::TMobilePhoneNetworkInfoV5 iCurrentNetwork;
	TTsySignalInfo iSignalInfo;
	RMobilePhone::TMobilePhoneBatteryInfoV1 iBatteryInfo;
	RMobilePhone::TMobilePhoneNITZ iMobilePhoneNITZ;
	RPacketService::TRegistrationStatus iGprsRegistrationStatus;
	RPacketService::TStatus iPacketStatus;
	TInt iGprsMaxNumContexts;
	TRfStateInfo iRfStateInfo;
	RMobilePhone::TMobilePhoneNetworkSelectionV1  iLocalNetworkSel;
	RMobilePhone::TMobilePhoneIdentityV1 iPhoneId;
	};

// forward class declarations
class CTsyConfig;
class CAtManager;
class CCommEngine;
class CRequestBase;
class CLtsyCallInformationManager;

/**
 * This class is a shared global phone manager across all the handlers.
 * 
 * This class provide the functions which store the phone status, modem status and access the call info manager.
 * 
 */
class CGlobalPhonemanager : public CBase
	{
public:
	/**
	 * Factory function
	 */
	static CGlobalPhonemanager* NewL();
	
	/**
	 * Factory function
	 */
	static CGlobalPhonemanager* NewLC();
	
	/**
	 * virtual destructor
	 */
	virtual ~CGlobalPhonemanager();
	
public:
	/**
	 * Check current phone status before writing an AT command
	 */
	TInt CheckGlobalPhoneStatus();
	
	/**
	 * Set current Active IPC Request
	 */
	void SetActiveRequest(CRequestBase* aRequestBase);
	
	/**
	 * Before writing an AT command to baseband, the last active Request must be deleted in order to avoiding the  
	 */
	void DeleteLastActiveRequest();
	
	/**
	 * Get Call informantion manager reference
	 * 
	 */
	CLtsyCallInformationManager& GetCallInfoManager();
	
	/**
	 * Get current active IPC request
	 */
	CRequestBase* CurrentActiveRequest();
private:
	/**
	 * Constructor
	 * 
	 */
	CGlobalPhonemanager();
	
	/**
	 * 2nd Constructor
	 * 
	 */
	void ConstructL();
	
	/**
	 * Initialize Phone Manager
	 * 
	 */
	void InitL();
public:
	/**
	 * CommDB Engine
	 */
	CTsyConfig *iConfiguration;
	
	/**
	 * Comm Engine
	 * Own
	 */
	CCommEngine *iCommEngine;
	
	/**
	 * AT Manager
	 * Own
	 */
	CAtManager *iAtManager;	
	
	/**
	 * Global phone status
	 */
	TPhoneStatusTSY iPhoneStatus;
	
	/**
	 * This member define if or not there is unfinished IPC active
	 */
	TBool iEventSignalActive;

	/**
	 * Pkt Context list
	 */
	RPointerArray<TContextInfomation> iContextList;
	/**
 	 * Phonebook store info
 	 */
	DispatcherPhonebook::TPhonebookStoreInfoV1  iPhonebookStoreInfo;
	/**
	 * Get the total storage items from CAtSmsStoreInfo
	 */
	TInt iStoreTotal; 
private:
	/**
	 * Current active Ipc Request
	 */
	CRequestBase* iActiveIpcRequest;
	
	/**
	 * The call information manager
	 * Own
	 */
	CLtsyCallInformationManager* iCallInfoManager;
	};

#endif /*__GLOBALPHONEMANAGER_H__*/
