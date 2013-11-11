// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Reference implementation of Key Event Routing plug-in

/**
@file
@internalTechnology
@prototype
*/

#include <e32cmn.h>
#include <e32debug.h>
#include "keyrouterimpl.h"
#include "keyaliases.h"

#ifndef KEYROUTER_TEST
// Temporary: to be made configurable
#define COMBINED_POWER_END_KEY
#endif

/**
Privileged application UIDs
*/
#ifdef KEYROUTER_TEST
const TInt KUidTAutoServer = 0x10205152;
const TInt KUidReservedApp = 0x102872e3;
#else
const TInt KUidPhoneApp = 0x100058b3;
const TInt KUidSysAp = 0x100058f3;
const TInt KUidAutoLock = 0x100059b5;
const TInt KUidHomeScreen = 0x20022f35;
const TInt KUidAknCapServer = 0x10207218;
#endif

#if defined(KEYROUTER_TEST) || defined(COMBINED_POWER_END_KEY)
/**
Key Capture Translation Table

The following scan codes and key codes are translated at capture-request time.

@note	The data in this table is for example/test purposes only.
*/
const TTranslationEntry KTranslations[] =
	{
		// Req. scancode,	Capture scancode,	Req. keycode,	Capture keycode
#ifndef KEYROUTER_TEST
		// On devices with a combined Power and End key, that key produces a
		// PowerOff code. Map capture requests for the End key to PowerOff.
		// At routing time, an End key event will automatically be delivered
		// to the capturing application.
		{ EStdKeyPhoneEnd,	EStdKeyPowerOff,	EKeyPhoneEnd,	EKeyPowerOff }
#else
		// Entry for TEvent test case GRAPHICS-WSERV-0751.
		// Capture requests for psuedo key Device0 are mapped to capture
		// the real key Device1.
		{ EStdKeyDevice0,	EStdKeyDevice1,		EKeyDevice0,	EKeyDevice1 }
#endif
	};

const TInt KNumTranslations = TABLE_SIZE(KTranslations);
#endif // defined(KEYROUTER_TEST) || defined(COMBINED_POWER_END_KEY)

/**
Restricted Key Table

The following keys may be captured only by the specified applications.
More than one application may be listed for a given key-type combination.
*/
const TRestrictedKeyEntry KRestrictedKeys[] =
	{
#ifndef KEYROUTER_TEST
		// Short Apps key events may only be captured by HomeScreen, SysApp,
		// AutoLock, Phone and AknCapServer.
		{ EStdKeyApplication,	ECaptureTypeKeyUpDown,	KUidHomeScreen },
		{ EKeyApplication,		ECaptureTypeKey,		KUidHomeScreen },
		{ EStdKeyApplication,	ECaptureTypeKeyUpDown,	KUidSysAp },
		{ EKeyApplication,		ECaptureTypeKey,		KUidSysAp },
		{ EStdKeyApplication,	ECaptureTypeKeyUpDown,	KUidAutoLock },
		{ EKeyApplication,		ECaptureTypeKey,		KUidAutoLock },
		{ EStdKeyApplication,	ECaptureTypeKeyUpDown,	KUidPhoneApp },
		{ EKeyApplication,		ECaptureTypeKey,		KUidPhoneApp },
		{ EStdKeyApplication,	ECaptureTypeKeyUpDown,	KUidAknCapServer },
		{ EKeyApplication,		ECaptureTypeKey,		KUidAknCapServer },

		// Long Apps key events may only be captured by SysAp (for launching
		// the task switcher), AutoLock and Phone.
		{ EKeyApplication,		ECaptureTypeLongKey,	KUidSysAp },
		{ EKeyApplication,		ECaptureTypeLongKey,	KUidAutoLock },
		{ EKeyApplication,		ECaptureTypeLongKey,	KUidPhoneApp },

		// Only SysAp and AutoLock are allowed to capture Power key events
		{ EKeyPowerOff,			ECaptureTypeKey,		KUidSysAp },
		{ EKeyPowerOff,			ECaptureTypeLongKey,	KUidSysAp },
		{ EStdKeyPowerOff,		ECaptureTypeKeyUpDown,	KUidSysAp },
		{ EKeyPowerOff,			ECaptureTypeKey,		KUidAutoLock },
		{ EKeyPowerOff,			ECaptureTypeLongKey,	KUidAutoLock },
		{ EStdKeyPowerOff,		ECaptureTypeKeyUpDown,	KUidAutoLock },

		// Only AutoLock is allowed to capture Lock key events
		{ EKeyLock,				ECaptureTypeKey,		KUidAutoLock },
		{ EKeyLock,				ECaptureTypeLongKey,	KUidAutoLock },
		{ EStdKeyLock,			ECaptureTypeKeyUpDown,	KUidAutoLock }
#else
		// The following entries are for test purposes only.
		// Test Case GRAPHICS-WSERV-0754. Only a reserved UID is allowed to
		// capture F20 key events.
		{ EKeyF20,				ECaptureTypeKey,		KUidReservedApp },
		{ EKeyF20,				ECaptureTypeLongKey,	KUidReservedApp },
		{ EStdKeyF20,			ECaptureTypeKeyUpDown,	KUidReservedApp },
		// Only the reserved UID and the TAuto tests are allowed to capture
		// F21 key events
		{ EKeyF21,				ECaptureTypeKey,		KUidReservedApp },
		{ EKeyF21,				ECaptureTypeLongKey,	KUidReservedApp },
		{ EStdKeyF21,			ECaptureTypeKeyUpDown,	KUidReservedApp },
		{ EKeyF21,				ECaptureTypeKey,		KUidTAutoServer },
		{ EKeyF21,				ECaptureTypeLongKey,	KUidTAutoServer },
		{ EStdKeyF21,			ECaptureTypeKeyUpDown,	KUidTAutoServer }
#endif
	};

/**
Application Priority Table

The following applications have fixed priorities for capture of the associated
key. The priority specified in this table overrides the priority in the capture
request. When any other application captures a key listed here, the priority is
forced to zero.

@note	If a key should be never be delivered to any other application, then
		it should be placed in KRestrictedKeys[] instead.
*/
const TAppPriorityEntry KAppPriorityKeys[] =
	{
#ifndef KEYROUTER_TEST
		// The Phone app has priority for capture of Send key events
		{ EStdKeyPhoneSend,	ECaptureTypeKeyUpDown,	KUidPhoneApp, 1 },
		{ EKeyPhoneSend,	ECaptureTypeKey,		KUidPhoneApp, 1 },
		{ EKeyPhoneSend,	ECaptureTypeLongKey,	KUidPhoneApp, 1 },

		// The Phone app has the highest priority for capture of End key
		// events. Note: any applications that capture these events apart
		// from those listed here (e.g. other telephony apps) will effectively
		// have priority 0.
		{ EStdKeyPhoneEnd,	ECaptureTypeKeyUpDown,	KUidPhoneApp, 1 },
		{ EKeyPhoneEnd,		ECaptureTypeKey,		KUidPhoneApp, 1 },
		{ EKeyPhoneEnd,		ECaptureTypeLongKey,	KUidPhoneApp, 1 },

		// The Home Screen app has low priority for capture of End key events
		{ EStdKeyPhoneEnd,	ECaptureTypeKeyUpDown,	KUidHomeScreen, -1 },
		{ EKeyPhoneEnd,		ECaptureTypeKey,		KUidHomeScreen, -1 },
		{ EKeyPhoneEnd,		ECaptureTypeLongKey,	KUidHomeScreen, -1 },

		// SysAp has the lowest priority for capture of End key events
		{ EStdKeyPhoneEnd,	ECaptureTypeKeyUpDown,	KUidSysAp, -2 },
		{ EKeyPhoneEnd,		ECaptureTypeKey,		KUidSysAp, -2 },
		{ EKeyPhoneEnd,		ECaptureTypeLongKey,	KUidSysAp, -2 },

#ifdef COMBINED_POWER_END_KEY
		// SysAp has the same priority for capture of Power key events as
		// it does for End key events. This is necessary to maintain correct
		// relative priorities when there is a combined Power and End key.
		{ EStdKeyPowerOff,	ECaptureTypeKeyUpDown,	KUidSysAp, -2 },
		{ EKeyPowerOff,		ECaptureTypeKey,		KUidSysAp, -2 },
		{ EKeyPowerOff,		ECaptureTypeLongKey,	KUidSysAp, -2 }
#endif

#else // KEYROUTER_TEST
		// The following entries are for test purposes only.
		// Test Case GRAPHICS-WSERV-0757. TAuto tests have priority for
		// capture of F22 key events.
		{ EStdKeyF22,		ECaptureTypeKeyUpDown,	KUidTAutoServer, 1 },
		{ EKeyF22,			ECaptureTypeKey,		KUidTAutoServer, 1 },
		{ EKeyF22,			ECaptureTypeLongKey,	KUidTAutoServer, 1 }
#endif // KEYROUTER_TEST
	};

#ifdef KEYROUTER_TEST
/**
Blocked key table.

The following keys are not routed by default.

@note	The data in this table is for example/test purposes only.
		Since long key events are never routed by default, there is no need
		to list them here.
*/
const TBlockedKeyEntry KBlockedKeys[] =
	{
		// Entries for TEvent test case GRAPHICS-WSERV-0760
		{ EStdKeyDevice3,	ECaptureTypeKeyUpDown },
		{ EKeyDevice3,		ECaptureTypeKey }
	};

const TInt KNumBlockedKeys = TABLE_SIZE(KBlockedKeys);
#endif // KEYROUTER_TEST

const TInt KNumRestrictedKeys = TABLE_SIZE(KRestrictedKeys);
const TInt KNumAppPriorityKeys = TABLE_SIZE(KAppPriorityKeys);
const TInt KCaptureKeyArrayGranularity = 5;

/**
Create and return an instance of CKeyEventRouter

@return	Pointer to new router instance
*/
EXPORT_C CKeyEventRouter* CKeyEventRouter::NewL()
	{
	return new(ELeave) CKeyEventRouterImpl;
	}

CKeyEventRouterImpl::CKeyEventRouterImpl()
: iCaptureKeys(KCaptureKeyArrayGranularity, _FOFF(TKeyCaptureRequest, iHandle))
	{
	}

CKeyEventRouterImpl::~CKeyEventRouterImpl()
	{
	iCaptureKeys.Close();
	}

/**
Add a new key capture request

@param	aRequest	Capture request details

@see CKeyEventRouter::AddCaptureKeyL
*/
void CKeyEventRouterImpl::AddCaptureKeyL(const TKeyCaptureRequest& aRequest)
	{
	CheckCaptureKeyL(aRequest);
	iCaptureKeys.InsertL(aRequest, 0);
	ProcessAppPriorities(iCaptureKeys[0]);
	TranslateCaptureKey(aRequest.iType, iCaptureKeys[0].iInputCode);
	}

   
/**
Update an existing key capture request

@param	aRequest	Updated capture request details

@note	This function is used only for window server hotkeys, hence key/scan
		code translation and processing of special application priorities are
		omitted.

@see CKeyEventRouter::UpdateCaptureKeyL
*/
void CKeyEventRouterImpl::UpdateCaptureKeyL(const TKeyCaptureRequest& aRequest)
	{
	ASSERT(!aRequest.iWindowGroup);
	CheckCaptureKeyL(aRequest);

	TInt index = iCaptureKeys.Find(aRequest);
	ASSERT(index != KErrNotFound);

	if (index != KErrNotFound)
		{
		iCaptureKeys[index] = aRequest;
		}
	}

/**
Cancel a key capture request

@param	aType		Capture type
@param	aHandle		Opaque handle of request to be cancelled

@see CKeyEventRouter::CancelCaptureKey

Note: aType is unused in this implementation, but is present to permit
implementations that use separate lists for each of the three capture types.
*/
void CKeyEventRouterImpl::CancelCaptureKey(TKeyCaptureType /*aType*/, TAny* aHandle)
	{
	TKeyCaptureRequest request;
	request.iHandle = aHandle;

	TInt index = iCaptureKeys.Find(request);
	if (index != KErrNotFound)
		{
		iCaptureKeys.Remove(index);
		}
	}

/**
Route the key event described by aInput and return its destination in iOutput

@param	aInput		Input data with key event to be routed
@param	aOutput		Output key event and routing results

@see CKeyEventRouter::RouteKey
*/
void CKeyEventRouterImpl::RouteKey(const TKeyEventRouterInput& aInput, TKeyEventRouterOutput& aOutput)
	{
	TUint inputCode = (aInput.iType == ECaptureTypeKeyUpDown) ?
						aInput.iKeyEvent.iScanCode : aInput.iKeyEvent.iCode;
	TInt priority = KMinTInt;
	TInt captureCount = iCaptureKeys.Count();
	TKeyCaptureRequest* matchRequest = NULL;

	// Find the highest priority matching capture request. If there are
	// multiple entries with the same priority then use the first one, i.e.
	// the most recent request.
	for (TInt index = 0; index < captureCount; index++)
		{
		TKeyCaptureRequest& request = iCaptureKeys[index];
		if (request.iType == aInput.iType &&
			request.iInputCode == inputCode &&
		    (aInput.iKeyEvent.iModifiers & request.iModifierMask) == request.iModifiers &&
			(request.iPriority > priority))
			{
			matchRequest = &request;
			priority = request.iPriority;
			}
		}

	if (matchRequest)
		{
		// Found matching capture request. Route the key event to the window
		// group that made the capture request.
		aOutput.iWindowGroup = matchRequest->iWindowGroup;
		if (aInput.iType == ECaptureTypeKeyUpDown)
			{
			aOutput.iKeyEvent.iScanCode = matchRequest->iOutputCode;
			aOutput.iKeyEvent.iCode = aInput.iKeyEvent.iCode;
#ifdef COMBINED_POWER_END_KEY
			// When routing to SysAp on devices with a combined Power and End
			// key, deliver this event as PowerOff if the HomeScreen currently
			// has focus, otherwise as End. (Note that when the HomeScreen app
			// is showing the application library, it will capture the End key
			// itself with higher priority.)
			if (matchRequest->iAppUid.iUid == KUidSysAp &&
				aInput.iKeyEvent.iScanCode == EStdKeyPowerOff)
				{
				aOutput.iKeyEvent.iScanCode =
					(aInput.iFocusAppUid.iUid == KUidHomeScreen) ?
					EStdKeyPowerOff : EStdKeyPhoneEnd;
				}
#endif
			}
		else
			{
			aOutput.iKeyEvent.iScanCode = aInput.iKeyEvent.iScanCode;
			aOutput.iKeyEvent.iCode = matchRequest->iOutputCode;
#ifdef COMBINED_POWER_END_KEY
			if (matchRequest->iAppUid.iUid == KUidSysAp &&
				aInput.iKeyEvent.iCode == EKeyPowerOff)
				{
				aOutput.iKeyEvent.iCode =
					(aInput.iFocusAppUid.iUid == KUidHomeScreen) ?
					EKeyPowerOff : EKeyPhoneEnd;
				}
#endif
			}
		aOutput.iKeyEvent.iModifiers = aInput.iKeyEvent.iModifiers;
		aOutput.iCaptureHandle = matchRequest->iHandle;
		aOutput.iResult = ECaptured;
		}
#ifdef KEYROUTER_TEST
	else if (IsKeyBlocked(aInput.iType, inputCode))
		{
		// No matching capture request and key is blocked. Do not route.
		aOutput.iResult = EConsumed;
		}
#endif
	else
		{
		// No matching capture request. Route the key event to the current
		// focussed window group.
		aOutput.iWindowGroup = aInput.iFocusWindowGroup;
		aOutput.iKeyEvent = aInput.iKeyEvent;
		aOutput.iCaptureHandle = NULL;
		aOutput.iResult = ERouted;
		}
	}

/**
Check that capture request arguments are sane and capture is allowed

@param	aRequest	Capture request details

@leave	KErrArgument if modifier state contains bits that are not in the
		modifier mask or modifier mask contains EModifierLongKey or priority
		is KMinTInt.
		KErrPermissionDenied if key is restricted to capture by a specific
		application UID and the UID of the requesting app does not match.

@note	Requests with a priority of KMinTint would never be captured: this was
		also true of the previous implementation in ektran.dll but was ignored.
		Client code must use the appropriate API function to capture short or
		long key events, so including EModifierLongKey in the modifier mask is
		not allowed. (Hotkeys are excepted since a mask of 0xffffffff is used
		to disable them).
*/
void CKeyEventRouterImpl::CheckCaptureKeyL(const TKeyCaptureRequest& aRequest)
	{
	if ((aRequest.iModifiers & ~aRequest.iModifierMask) != 0 ||
		(aRequest.iModifierMask & EModifierLongKey) != 0 && aRequest.iWindowGroup != NULL ||
		aRequest.iPriority == KMinTInt)
		{
		User::Leave(KErrArgument);
		}

	if (aRequest.iWindowGroup != NULL && IsRestrictedKey(aRequest))
		{
		User::Leave(KErrPermissionDenied);
		}
	}

/**
Check if the requested key capture is restricted by application UID

@param	aRequest	Capture request details

@return	ETrue if the key-type combination in aRequest is listed in
		KRestrictedKeys[] but the requesting application UID does not match
		any entry in the table; otherwise EFalse.
*/
TBool CKeyEventRouterImpl::IsRestrictedKey(const TKeyCaptureRequest& aRequest)
	{
	TBool foundKey = EFalse;

	for (const TRestrictedKeyEntry* entry = KRestrictedKeys; entry < &KRestrictedKeys[KNumRestrictedKeys]; entry++)
		{
		if (entry->iCode == aRequest.iInputCode &&
			entry->iType == aRequest.iType)
			{
			foundKey = ETrue;
			if (entry->iAppUidValue == aRequest.iAppUid.iUid)
				{
				return EFalse;
				}
			}
		}

	return foundKey;
	}

/**
Process special application priorities

@param	aRequest	Capture request details. If the key-type combination is
					listed in KAppPriorityKeys[], then the capture priority is
					replaced with the configured priority for the requesting
					application, or with zero if the application UID does not
					match any entry in the table.
*/
void CKeyEventRouterImpl::ProcessAppPriorities(TKeyCaptureRequest& aRequest)
	{
	TBool foundKey = EFalse;

	for (const TAppPriorityEntry* entry = KAppPriorityKeys; entry < &KAppPriorityKeys[KNumAppPriorityKeys]; entry++)
		{
		if (entry->iCode == aRequest.iInputCode &&
			entry->iType == aRequest.iType)
			{
			foundKey = ETrue;
			if (entry->iAppUidValue == aRequest.iAppUid.iUid)
				{
				aRequest.iPriority = entry->iAppPriority;
				return;
				}
			}
		}

	if (foundKey)
		{
		aRequest.iPriority = 0;
		}
	}

#ifdef KEYROUTER_TEST
/**
Check if the specified key is blocked from default routing

@param	aType		Key capture type
@param	aCode		Scan code or key code

@return	ETrue if key is blocked, otherwise EFalse.
*/
TBool CKeyEventRouterImpl::IsKeyBlocked(TKeyCaptureType aType, TUint aCode)
	{
	for (const TBlockedKeyEntry* entry = KBlockedKeys; entry < &KBlockedKeys[KNumBlockedKeys]; entry++)
		{
		if (entry->iCode == aCode && entry->iType == aType)
			{
			return ETrue;
			}
		}

		return EFalse;
	}
#endif // KEYROUTER_TEST

/**
Translate the scan or key code of a capture request

@param	aType	Key capture type
@param	aCode	Key code or scan code, updated to translated value (if any)

@note	This function is used to translate the input key or scan code of a
		capture request on addition to the capture list. When a key event that
		matches the list entry is routed, the output code will be taken from
		TKeyCaptureRequest.iOutputCode and hence automatically translated again
		(typically back to the original input code of the request, unless the
		input and output codes were different). For example, a request to
		capture key code A will be translated to key code B in the capture list.
		When RouteKey() processes an event with key code B and selects that
		list entry, it will output key code A for delivery to the application.
*/
void CKeyEventRouterImpl::TranslateCaptureKey(TKeyCaptureType aType, TUint& aCode)
	{
#if defined(KEYROUTER_TEST) || defined(COMBINED_POWER_END_KEY)
	for (const TTranslationEntry* entry = KTranslations; entry < &KTranslations[KNumTranslations]; entry++)
		{
		if (aType == ECaptureTypeKeyUpDown)
			{
			if (aCode == entry->iRequestScanCode)
				{
				aCode = entry->iCaptureScanCode;
				}
			}
		else
			{
			if (aCode == entry->iRequestKeyCode)
				{
				aCode = entry->iCaptureKeyCode;
				}
			}
		}
#endif
	}
