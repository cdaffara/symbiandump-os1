// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file CSelectionRequest.cpp
*/

#include <nifman.h>
#include <comms-infras/nifagt.h>
#include <es_connpref.h>
#include "CSelectionRequest.h"
#include "CNetworkController.h"
#include "NetConPanic.h"
#include "NetConLog.h"
#include "NetConError.h"

CSelectionRequest* CSelectionRequest::NewL(MNetConEnv* aController, MNetworkControllerObserver* aObserver, TConnStartType aStartType, const TConnPref& aPrefs, TInt aConnectionAttempt, TInt aLastConnectionError)
/**
Factory function

Creates a new CSelectionRequest object

@param aController pointer to the Network Controller
@param aObserver the object that initiated this request
@param aStartType how this request was initiated either Implicit(due to RSocket/RHostResolver calls) or Explicit(due to an RConnection call)
@param aPrefs connection preferences to be used when selecting an appropriate Agent
@param aConnectionAttempt indicates the current connection attempt
@param aLastConnectionError if this is not the first connection attempt then the error of the last attempt
@exception leaves if object cannot be constructed
@return the new CSelectionRequest object
*/
 	{

	CSelectionRequest* self = new(ELeave) CSelectionRequest(aController, aObserver, aStartType, aPrefs, aConnectionAttempt, aLastConnectionError);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CSelectionRequest::~CSelectionRequest()
/**
Destructor
*/
	{

	if(iOverrides)
		delete iOverrides;

	if(iAgentName)
		delete iAgentName;
	}

CSelectionRequest::CSelectionRequest(MNetConEnv* aController, MNetworkControllerObserver* aObserver, TConnStartType aStartType, const TConnPref& aPrefs, TInt aConnectionAttempt, TInt aLastConnectionError)
: CNetConRequestBase(aController, aObserver, NULL), iConnStartType(aStartType), iConnPrefs(aPrefs), iConnectionAttempt(aConnectionAttempt), iLastConnectionError(aLastConnectionError)
/**
Constructor

@param aDatabase pointer to the object used to access CommDb
@param aDialogPrc pointer to the object used to access the dialog processor
@param aController pointer to the Network Controller
@param aObserver the object that initiated this request
@param aStartType how this request was initiated either Implicit(due to RSocket/RHostResolver calls) or Explicit(due to an RConnection call)
@param aPrefs connection preferences to be used when selecting an appropriate Agent
@param aConnectionAttempt indicates the current connection attempt
@param aLastConnectionError if this is not the first connection attempt then the error of the last attempt
*/
	{ }

void CSelectionRequest::ConstructL()
/**
2nd phase of construction

@exception leaves if 2nd phase construction leaves
*/
	{
	CNetConRequestBase::ConstructL();

	// construct a HBufC in which to store the name of the agent
	const TUint8 KInitialAgentNameLength = 20;
	iAgentName = HBufC::NewL(KInitialAgentNameLength);
	_LIT(KUndefined, "Undefined");
	SetAgentNameL(KUndefined());
	}

void CSelectionRequest::SetAgentNameL(const TDesC& aName)
/**
Replace the value of iAgentName allocating more memory if needed

@param aName the new name of the Agent
@exception leaves with KErrNoMemory if allocation fails
*/
	{
	ASSERT(iAgentName);

	// see if we need to allocate more memory for the agent name
	if (aName.Length() > iAgentName->Length())
		{
		iAgentName = iAgentName->ReAllocL(aName.Length());
		}

	// replace agent name
	*iAgentName = aName;
	}

void CSelectionRequest::StartRequest()
/**
Start processing this request
*/
	{
	LOG (
		NetConLog::Printf(_L("\tRequest type = Selection"));
		NetConLog::Printf(_L("\tConnection attempt = %d"), iConnectionAttempt);
		NetConLog::Printf(_L("\tConnection type = %d"), iConnStartType);
		)

	// set overrides for this request in the databse
	TRAPD(err, iDatabase->SetOverridesL(iOverrides);
               iDatabase->RequestNotificationOfServiceChangeL(ipServiceChangeObserver));

	if(err!=KErrNone)
		{
		LOG( NetConLog::Printf(_L("\tError %d encountered during start of IAP selection routine - is CommDb setup correctly?"), err); )
		RequestComplete(err);
		return;
		}

	TRAP(err, StartSelectL());
	if(err!=KErrNone)
		{
		LOG( NetConLog::Printf(_L("\tError %d encountered during start of IAP selection routine - is CommDb setup correctly?"), err); )
		RequestComplete(err);
		}
	}

void CSelectionRequest::RequestComplete(TInt aError)
/**
Complete this request with a given error
 
@param aError the error with which to complete the request
*/
	{

	LOG( NetConLog::Printf(_L("\tRequest Complete with error %d"), aError); )

	iController->RequestComplete(this, aError);
	}

const TDesC& CSelectionRequest::AgentName() const
/**
Retrieves the name of the agent that was selected
*/
	{
	if(!iAgentName)
		{
		return KNullDesC();
		}
	else
		{
		return *iAgentName;
		}
	}

const TPckgBuf<TSoIfConnectionInfo>& CSelectionRequest::AgentConnectionInfo() const
/**
Retrieves the connection information of the agent that was selected
*/
	{
	return iAgentConnInfo;
	}

const TConnStartType& CSelectionRequest::ConnectionStartType() const
/**
@return the connection start type - i.e. either implicit or explicit
*/
	{
	
	return iConnStartType;
	}

const TConnPref& CSelectionRequest::ConnPrefs() const
/**
@return the connection preferences associated with this request
*/
	{

	return iConnPrefs;
	}

void CSelectionRequest::StartSelectL()
/**
Start the selection process

Read current connection preferences and settings from the database then start the selection process. 

@exception leaves if database access leaves
*/
 	{

	// clear any existing overrides in the database
	iDatabase->SetOverridesL(NULL);

	// check that we are within the maximum number of connection attempts specified by CommDb
	TInt maxConnectionAttempts = iDatabase->GetConnectionAttempts();
	
	User::LeaveIfError(maxConnectionAttempts);

	// allow the connection preferences to override the max number of connection attempts in CommDb
	if(iConnPrefs.ExtensionId() == TConnPref::EConnPrefCommDbMulti)
		{
		TInt preferences = TCommDbMultiConnPref::Cast(iConnPrefs).ConnectionAttempts();
		maxConnectionAttempts = preferences; 
		}

	if(iConnectionAttempt > maxConnectionAttempts)
		{
		LOG( NetConLog::Printf(_L("\tError - maximum number of connection attempts (%d) reached"), maxConnectionAttempts); )
		RequestComplete(KErrOverflow);
		return;
		}

	// extract a TCommDbConnPref from iConnPrefs
	TCommDbConnPref commDbPref;
	User::LeaveIfError(ExtractCommDbConnPref(iConnPrefs, commDbPref));

	// set connection attempt
	iSettings.iRank = iConnectionAttempt;

	// if direction is not specified then default to outgoing
	if(commDbPref.Direction() == ECommDbConnectionDirectionUnknown)
		{
		commDbPref.SetDirection(ECommDbConnectionDirectionOutgoing);
		}

	// extract connection direction
	iSettings.iDirection = commDbPref.Direction();

	TUint32 overidenIap = commDbPref.IapId();

	// Wnsure that a valid IAP exists in the 
	TUint32 preferredIap(overidenIap);
	LOG( NetConLog::Printf(_L("\tpreferredIap=%d"), preferredIap); )

	if (overidenIap == 0)
		{
		iDatabase->GetPreferedIapL(preferredIap, iSettings.iDirection, iSettings.iRank);
		LOG( NetConLog::Printf(_L("\tnew preferredIap=%d"), preferredIap); )
		}

	// If this IAP doesn't exist, we must prompt the user
	if((preferredIap == 0) || !iDatabase->DoesIapExistL(preferredIap))
		{
		LOG( NetConLog::Printf(_L("\tActive settings for IAP have been set to prompt, since the IAP doesn't exist"));)
		commDbPref.SetDialogPreference(ECommDbDialogPrefPrompt);

		// IAP is invalid, override it in the active settings to allow prompt to continue
		iDatabase->GetFirstValidIapL(preferredIap);
		commDbPref.SetIapId(preferredIap);
		}

	// convert the connection preferences into some appropriate override settings
	iOverrides = MapPrefsToOverridesL(commDbPref, iSettings);

	if(iOverrides)
		{
		//set these overrides in the database
		iDatabase->SetOverridesL(iOverrides);
		}

	iDatabase->GetCurrentSettingsL(iSettings, iSettings.iDirection, iSettings.iRank);

	if((iConnStartType == EConnStartImplicit) && iController->ImplicitConnectionAgentName())
		{
		// there is already an implicit connection - try to match the preferences
		TCommDbConnPref implicitPref;
		User::LeaveIfError(ExtractCommDbConnPref(iController->ImplicitConnectionPrefs(), implicitPref));
		if(commDbPref == implicitPref)
			{
			// This is an implicit request for an outgoing connection and there
			// has already been another implicit request - just return the name
			// of the existing agent
			LOG( NetConLog::Printf(_L("\tReturning existing agent for implicit connection: '%S'"), iController->ImplicitConnectionAgentName()); )
			SetAgentNameL(*iController->ImplicitConnectionAgentName());
			RequestComplete(KErrNone);
			}
		else
			{
			LOG( NetConLog::Printf(_L("\tImplicit connection already exists - cannot start a new one with conflicting preferences!")); )
			RequestComplete(KErrInUse);
			}
		return;
		}

	// store the completed preferences as iConnPrefs
	// this is so that if the CNetworkController asks for the prefs used for the
	// selected connection we will return the accurate prefs (with the default
	// values filled in)
	iConnPrefs = commDbPref;

	// for some reason we have to write these settings back to the database, otherwise
	// when dialog prompt is set to warn we get an error - why?
	iDatabase->SetCurrentSettingsL(iSettings);

	// note that we no-longer display the "Select Modem & Location" dialog boxes
	// since the IAP points to the modem and location

	// read service settings from database
	iDatabase->GetServiceSettingsL(iSettings);

	LOG (
		NetConLog::Printf(_L("\tPreferred Service Type = '%S'"), &iSettings.iServiceType);
		NetConLog::Printf(_L("\tPreferred Service Id = %d"), iSettings.iServiceId);
		NetConLog::Printf(_L("\tPreferred Bearer Set = %d"), iSettings.iBearerSet);
		)

	// check for bearer availability
	iDoBearerAvailability = !(iSettings.iDialogPref==ECommDbDialogPrefWarn || iSettings.iDialogPref==ECommDbDialogPrefDoNotPrompt);
	if (iDoBearerAvailability)
		{
		// Bearer availability check is only performed if prompting
		// It is only used to help populate the dialogue with available bearers
		//
		// Warning: that the results of the CheckBearerAvailability are 
		// only used in the case of ECommDbDialogPrefPromptIfWrongMode
		// so it might be possible to remove this check and speed things up
		// 
		LOG( NetConLog::Printf(_L("\tRequesting bearer availability")); )
		iController->CheckBearerAvailability(EFalse);  // EFalse indicates that this is not a reconnection
		}
	else
		{
		// Not prompting so do best effort and allow all bearers
		// Allow all bearers if bearer availability check is not done
		LOG( NetConLog::Printf(_L("\tSkipping bearer availability check")); )
		SetAvailableBearers(KCommDbBearerCSD | KCommDbBearerLAN | KCommDbBearerVirtual);
		}
	}

TInt CSelectionRequest::ExtractCommDbConnPref(const TConnPref& aBasePref, TCommDbConnPref& aCommDbPref)
/**
Extract a single TCommDbConnPref from the given TConnPref

@param aBasePref the TConnPref
@param aCommDbPref on return contains the TCommDbConnPref extracted from the TConnPref
@return KErrNone if the TCommDbConnPref could be extracted, otherwise KErrArgument
*/
	{
	TInt err(KErrNone);

	// if the TConnPref argument is a valid CommDb connection preference then cast it to one
	switch(aBasePref.ExtensionId())
		{
		case TConnPref::EConnPrefCommDb:
			// cast down to a TCommDbConnPref
			aCommDbPref = TCommDbConnPref::Cast(aBasePref);
			break;

		case TConnPref::EConnPrefCommDbMulti:
			{
			// cast down to a TCommDbMultiConnPref
			TCommDbMultiConnPref multiPrefs = TCommDbMultiConnPref::Cast(aBasePref);

			// check the number of connection attempts
			if(iConnectionAttempt > multiPrefs.ConnectionAttempts())
				{
				LOG( NetConLog::Printf(_L("\tError - maximum number of connection attempts (%d) reached"), multiPrefs.ConnectionAttempts()); )
				err = KErrOverflow;
				}
			else
				{
				// connection attempt is ok, try to retrieve the connection preference for this attempt
				err = multiPrefs.GetPreference(iConnectionAttempt, aCommDbPref);
				}
			
			break;
			}

		case TConnPref::EConnPrefSnap:
			err = KErrNotSupported;
			break;
			
		case TConnPref::EConnPrefIdList:
			{
			const TCommIdList& pref = static_cast<const TCommIdList&>(aBasePref);
			// check the number of connection attempts
			if(iConnectionAttempt > pref.Count())
				{
				LOG( NetConLog::Printf(_L("\tError - the last AP (%d) reached"), pref.Count()); )
				err = KErrOverflow;
				}
			else
				{
				// connection attempt is ok, try to retrieve the connection preference for this attempt
				TInt iap = pref.Get(iConnectionAttempt-1);
				aCommDbPref.SetIapId(iap);
				aCommDbPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
				//otherwise keep the default TCommDbConnPref class settings
				}
			break;
			}
			
		case TConnPref::EConnPrefUnknown:
			// a TConnPref - just ignore and use the default values from the connection preferences table
			break;

		default:
			// some sort of unrecognised connection preference type - error the caller
			err = KErrArgument;

		}

	return err;
	}

CStoreableOverrideSettings* CSelectionRequest::MapPrefsToOverridesL(TCommDbConnPref& aPrefs, const TConnectionSettings& aSettings)
/**
Convert user's connection preferences into an override setting

@param aPrefs a set of connection preferences passed in by the user
@param aSettings the current default connection preference settings
@return override settings matching the supplied connection preferences or NULL if the TCommDbConnPref is empty
@exception leaves if OOM or database access fails
*/
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref iapPref;

	// set rank to this connection attempt
	iapPref.iRanking = aSettings.iRank;

	// always use direction given in prefs
	iapPref.iDirection = aSettings.iDirection;

	// if specified, use the settings given in the connection
	// preferences - otherwise read the default values from CommDb
	TBool useOverrides(EFalse);

	if(aPrefs.DialogPreference() != ECommDbDialogPrefUnknown)
		{
		LOG( NetConLog::Printf(_L("\tDialog Preference overriden to %d"), aPrefs.DialogPreference()); )

		iapPref.iDialogPref = aPrefs.DialogPreference();
		useOverrides = ETrue;
		}
	else
		{
		// use current connection preference dialog setting from database
		iapPref.iDialogPref = aSettings.iDialogPref;
		aPrefs.SetDialogPreference(aSettings.iDialogPref);
		}

	if(aPrefs.BearerSet() != KCommDbBearerUnknown)
		{
		LOG( NetConLog::Printf(_L("\tBearer Set overriden to %d"), aPrefs.BearerSet()); )

		iapPref.iBearer.iBearerSet = aPrefs.BearerSet();
		useOverrides = ETrue;
		}
	else
		{
		// use current connection preference BearerSet setting from database
		iapPref.iBearer.iBearerSet = aSettings.iBearerSet;
		aPrefs.SetBearerSet(aSettings.iBearerSet);
		}
	
	if(aPrefs.IapId() != 0)
		{
		LOG( NetConLog::Printf(_L("\tIAP ID overriden to %d"), aPrefs.IapId()); )
		
		iapPref.iBearer.iIapId = aPrefs.IapId();
		useOverrides = ETrue;

		}
	else
		{
		// use current connection preference IAP setting from database
		iapPref.iBearer.iIapId = aSettings.iIAPId;
		aPrefs.SetIapId(aSettings.iIAPId);
		}

	CStoreableOverrideSettings* overrides = NULL;
	if (useOverrides)
		{
		overrides = CStoreableOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
		CleanupStack::PushL(overrides);
		User::LeaveIfError(overrides->SetConnectionPreferenceOverride(iapPref));
		CleanupStack::Pop(); // overrides
		}

	return overrides;
	}

void CSelectionRequest::SetAvailableBearers(TUint32 aBearerSet)
/**
Callback from the Network Controller when the bearer availability check is complete

@param aBearerSet the set of available bearers
*/
	{
	LOG( NetConLog::Printf(_L("\tAvailable Bearer Set = %d"), aBearerSet); )

	// is the selected service available?
	TBool available;
	if( (iSettings.iDialogPref == ECommDbDialogPrefPrompt) || 
	    (iSettings.iDialogPref == ECommDbDialogPrefPromptIfWrongMode))
		{
		/* The ServiceAvailable check is not necessary in prompt mode, since the user hasn't had a chance yet
		   to select the IAP. */
		available = ETrue;
		}
	else
		{
		available = SelectedServiceAvailable(aBearerSet);
		}

	TRAPD(err, SelectIapL(available, aBearerSet));
	if(err!=KErrNone)
		{
		LOG_DETAILED( NetConLog::Printf(_L("SelectionRequest %x\tSelectIapL left with %d"), this, err); )
		RequestComplete(err);
		}
	}

TBool CSelectionRequest::ServiceAvailableInAlternateMode(TUint32& aBearerSet, const TUint32 aAvailableBearerSet)
/**
The selected service is not available in this network mode - see if it is available in an alternate mode

If the selected service is GPRS but the network mode is CDMA (or vice versa) then prompt the user
to select an appropriate IAP.

@param aBearerSet on entry the set of selected bearers, on exit the set of bearers to pass to the dialog box
@param aAvailableBearerSet the set of available bearers
@return ETrue if an alternate bearer is available in another network mode, otherwise EFalse
*/
	{

	// read network mode from database
	RMobilePhone::TMobilePhoneNetworkMode networkMode(RMobilePhone::ENetworkModeUnknown);
	networkMode = iDatabase->NetworkMode();

	switch (networkMode)
		{
		case RMobilePhone::ENetworkModeGsm:
		default:
			LOG( NetConLog::Printf(_L("\tPacket data network mode is unknown")); )
			break;
		}

	// we couldn't switch from GPRS to CDMA or
	// vice-versa so see if CSD is available
	if(aAvailableBearerSet & KCommDbBearerCSD)
		{
		LOG( NetConLog::Printf(_L("\tPSD selected but only CSD available")); )
		aBearerSet = KCommDbBearerCSD;
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

void CSelectionRequest::SelectIapL(const TBool aSelectedServiceAvailable, const TUint32 aAvailableBearerSet)
/**
Select which Internet Access Point to connect to

@param aSelectedServiceAvailable whether the bearer required by the service is available
@param aAvailableBearerSet the set of available bearers
@exception leaves if database access leaves or if no service is available
*/
	{

	// set up connection preferences
	TConnectionPrefs prefs;
	prefs.iRank = iSettings.iRank;
	prefs.iDirection = iSettings.iDirection;
	prefs.iBearerSet = iSettings.iBearerSet;

	TBool serviceAvailable(aSelectedServiceAvailable);
	TInt error(KErrNotFound);

	switch (iSettings.iDialogPref)
		{
		case ECommDbDialogPrefWarn:
			LOG( NetConLog::Printf(_L("\tDialog box preference set to Warn")); )
				
			if(serviceAvailable)
				{
				// display IAP warning dialog box
				// will call back to MDPOWarnComplete() when finished
				TBuf<KCommsDbSvrMaxFieldLength> iapName;
				iDatabase->GetDesL(TPtrC(IAP),TPtrC(COMMDB_NAME), iapName);
				WarnNewConnection(prefs, &iapName, NULL, 0);
				}
			break;
		case ECommDbDialogPrefDoNotPrompt:
			LOG( NetConLog::Printf(_L("\tDialog box preference set to Do Not Prompt")); )
			
			if(serviceAvailable)
				{
				SelectIAPCompleteL(iSettings);
				}
			break;
		case ECommDbDialogPrefPromptIfWrongMode:
			LOG( NetConLog::Printf(_L("\tDialog box preference set to Prompt If Wrong Mode")); )
		
			if(serviceAvailable)
				{
				SelectIAPCompleteL(iSettings);
				}
			else
				{
				serviceAvailable = ServiceAvailableInAlternateMode(prefs.iBearerSet, aAvailableBearerSet);
				if(serviceAvailable)
					{
					// display IAP selection dialog box
					// will call back to MDPOSelectComplete() when finished
					// Warning:  prefs does not contain the aAvailableBearerSet
					// so the dialogue may be presented with unavailable Bearers
					SelectConnection(prefs, iLastConnectionError);
					}
				else
					{
					error = KErrNetConNoGPRSNetwork;
					}
				}
			break;
		case ECommDbDialogPrefPrompt:
		case ECommDbDialogPrefUnknown:
		default:
			LOG ( 
				if(iSettings.iDialogPref == ECommDbDialogPrefPrompt)
					NetConLog::Printf(_L("\tDialog box preference set to Prompt"));
				else
					{
					if (iSettings.iDialogPref == ECommDbDialogPrefUnknown)
						NetConLog::Printf(_L("\tDialog box preference set to Unknown"));
					else
						NetConLog::Printf(_L("\tDialog box preference not set"));
					}
				)

			if(serviceAvailable)
				{
				// display IAP selection dialog box
				// will call back to MDPOSelectComplete() when finished
				// Warning:  prefs does not contain the aAvailableBearerSet
				// so the dialogue may be presented with unavailable Bearers
				SelectConnection(prefs);
				}
			break;
		}

	if (!serviceAvailable)
		{
		if(iDoBearerAvailability)
			{
			iController->CancelBearerAvailabilityCheck();
			}
		User::Leave(error);
		}
	}

void CSelectionRequest::SelectAgentL()
/**
Decide whether to create a new agent or return an existing one

Check to see if there is already a connection to the
selected IAP and if so use the existing CNifAgentBase
Otherwise start a new Agent to the IAP

@exception leaves if database access or memory allocation
leaves or if signal strength is not sufficient
*/
	{
	if(iDoBearerAvailability)
		{
		if (iSettings.iBearerType.Compare(TPtrC(MODEM_BEARER))==0)
			{
			// connection is not NTRAS so check that the received signal strength is still ok
			User::LeaveIfError(iController->RequestSecondPhaseAvailability());
			}
		else
			{
			iController->CancelBearerAvailabilityCheck();
			}
		}

	CNifAgentBase* agent=NULL;

	TInt findErr = iController->FindExistingAgentForSelection(agent, iDatabase);

	if(findErr==KErrNone)
		{
		// found an Agent already connected to this IAP
		// retrieve the unique name of the agent and 
		// complete the request
		TNifAgentInfo info;
		agent->Info(info);
		LOG( NetConLog::Printf(_L("\tFound existing agent '%S' already connected to IAP %d"), &(info.iName), iSettings.iIAPId); )
		SetAgentNameL(info.iName);
		RequestComplete(KErrNone);
		return;
		}

	// otherwise - no existing connection to IAP - try to load a new agent
	LOG( NetConLog::Printf(_L("\tNo existing agent connected to IAP %d"), iSettings.iIAPId); )

	// read name of Agent from the bearer table
	TFileName agentName;
	iDatabase->GetDesL(iSettings.iBearerType, TPtrC(AGENT_NAME), agentName);

	// ask Nifman to create a new Agent, instantiating a new instance of an
	// existing agent if needed (note that Nifman maintains ownership of the agent)
	LOG( NetConLog::Printf(_L("\tCreating a new instance of agent '%S' for connection to IAP %d"), &agentName, iSettings.iIAPId); )
	agent = Nif::CreateAgentL(agentName, ETrue);

	// read the Agent's name
	TNifAgentInfo info;
	agent->Info(info);
	SetAgentNameL(info.iName);
	LOG( NetConLog::Printf(_L("\tAgent '%S' created for connection to IAP %d"), iAgentName, iSettings.iIAPId); )

	if(iOverrides)
		{
		// set overrides in the Agent's database - ownership of the overrides is transferred to the agent
		agent->SetOverridesL(iOverrides);
		iOverrides = NULL;
		}

	// set connection settings in Agent's database
	agent->SetConnectionSettingsL(iSettings);

	// read the Network ID of the IAP
	TUint32 networkId(0);
	iDatabase->GetIntL(TPtrC(IAP), TPtrC(IAP_NETWORK), networkId);

	// add the agent to the corresponding CNetwork object
	iController->AddAgentToNetworkL(agent, networkId);

	// store Agent Connection Info (IAP ID and Network ID)
	iAgentConnInfo().iIAPId = iSettings.iIAPId;
	iAgentConnInfo().iNetworkId = networkId;

	// If this is the implicit connection and it has a commdb connection preference 
	if ((iConnStartType == EConnStartImplicit) &&
		(iConnPrefs.ExtensionId() == TConnPref::EConnPrefCommDb))
		{
		TCommDbConnPref* commdbPref = (TCommDbConnPref*) &iConnPrefs; // cast down to a TCommDbConnPref

		if (commdbPref->IapId() == 0)
			{
			commdbPref->SetIapId(iSettings.iIAPId);
			}
		}

	RequestComplete(KErrNone);
	}

void CSelectionRequest::SelectIAPCompleteL(const TConnectionSettings& aSettings)
/**
IAP Selection is complete

At this point the selection is complete.  The Modem, Location and IAP have
been selected so load the appropriate Agent and set these settings in the
Agent.

@param aSettings The set of (now complete) connection settings
@exception leaves if database access or loading the Agent leaves
*/
 	{
 	 	
	if((iSettings.iDialogPref == ECommDbDialogPrefPrompt ||
		iSettings.iDialogPref == ECommDbDialogPrefPromptIfWrongMode)
		&& iSettings.iIAPId != aSettings.iIAPId)
		{
		// update stored connection preferences to reflect user's choice
		TCommDbConnPref commDbPref(TCommDbConnPref::Cast(iConnPrefs));
		commDbPref.SetIapId(aSettings.iIAPId);
		commDbPref.SetBearerSet(aSettings.iBearerSet);
		iConnPrefs = commDbPref;
		iSettings.iIAPId = aSettings.iIAPId;

		// Convert the new connection preferences into some appropriate override settings
		CStoreableOverrideSettings* overrides;
		overrides = MapPrefsToOverridesL(commDbPref, iSettings);
		if (overrides)
			{
			CleanupStack::PushL(overrides);
			iDatabase->SetOverridesL(overrides);
			CleanupStack::Pop();		// overrides
			}
		else
			{
			iDatabase->SetOverridesL(overrides);
			}

		if (iOverrides)
			{
			delete iOverrides;
			}

		iOverrides = overrides;
		}
	else
		{
		// Store selected IAP
		iSettings.iIAPId = aSettings.iIAPId;
		}

	// write the selected settings to the DbAccess object and refresh the service settings
	iDatabase->SetCurrentSettingsL(iSettings);
	iDatabase->GetCurrentSettingsL(iSettings, iSettings.iDirection, iSettings.iRank);
	iDatabase->GetServiceSettingsL(iSettings);

	LOG (
		NetConLog::Printf(_L("\tSelected IAP Id = %d"), iSettings.iIAPId);
		NetConLog::Printf(_L("\tSelected Location Id = %d"), iSettings.iLocationId);
		NetConLog::Printf(_L("\tSelected Bearer Type = '%S'"), &iSettings.iBearerType);
		NetConLog::Printf(_L("\tSelected Bearer Id = %d"), iSettings.iBearerId);
		NetConLog::Printf(_L("\tSelected Service Type = '%S'"), &iSettings.iServiceType);
		NetConLog::Printf(_L("\tSelected Service Id = %d"), iSettings.iServiceId);
		NetConLog::Printf(_L("\tSelected Bearer Set = %d"), iSettings.iBearerSet);
		)

	SelectAgentL();
	}

void CSelectionRequest::MDPOWarnComplete(TInt aError, TBool aResponse)
/**
Callback from the dialog processor when the "Warn about New Connection" dialog box has completed

@param aError KErrCancel if the user selected "Cancel", otherwise KErrNone
@param aReponse ETrue if the user selected "OK" otherwise EFalse
*/
	{

	if (aError == KErrNone)
		{
		if (aResponse)
			{
			// User did not cancel the connection
			// Find or Load an Agent based on selection
			TRAPD(err, SelectIAPCompleteL(iSettings));
			if(err!=KErrNone)
				{
				LOG_DETAILED( NetConLog::Printf(_L("\tError %d loading agent"), err); )
				RequestComplete(err);
				}
			}
		else
			{
			// User cancelled the connection
			aError = KErrCancel;
			}
		}

	if(aError!=KErrNone)
		{
		iController->CancelBearerAvailabilityCheck();
		RequestComplete(aError);
		return;
		}

	}

void CSelectionRequest::MDPOSelectComplete(TInt aError, const TConnectionSettings& aSettings)
/**
Callback from the dialog processor when the "Select IAP" dialog box has completed

@param aError KErrNone if the dialog box complete ok, otherwise KErrCancel
@param aSettings if there was no error will contain the CommDb ID of the selected IAP
*/
	{

	if(aError!=KErrNone)
		{
		iController->CancelBearerAvailabilityCheck();
		RequestComplete(aError);
		return;
		}

	// Find or Load an Agent based on user selection
	// Assume that the selected bearer is available its an edge case when
	// the selected bearer is not available or has now failed 
	TRAPD(err, SelectIAPCompleteL(aSettings));
	if(err!=KErrNone)
		{
		LOG_DETAILED( NetConLog::Printf(_L("SelectionRequest %x\tSelectIAPCompleteL left with error %d"), this, err); )
		RequestComplete(err);
		}
	}

