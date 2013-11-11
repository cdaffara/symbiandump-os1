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
// Key Event Routing Plug-in API

/**
@file
@publishedPartner
@prototype
*/

#ifndef WSKEYROUTER_H
#define WSKEYROUTER_H

#include <e32base.h>
#include <w32std.h>

/**
Interface Uid
*/
const TUid KKeyRouterPluginUid = { 0x102872e1 };

/**
Key Capture Type
*/
enum TKeyCaptureType
	{
	ECaptureTypeKey = 0,
	ECaptureTypeLongKey = 1,
	ECaptureTypeKeyUpDown = 2
	};

/**
Key Capture Request
*/
struct TKeyCaptureRequest
	{
	/** Capture type */
	TKeyCaptureType	iType;

	/** Keycode or scancode to be captured */
	TUint			iInputCode;

	/** Output keycode or scancode. When iInputCode is captured, RouteKey()
		places iOutputCode in TKeyEventRouterOutput.iKeyEvent */
	TUint			iOutputCode;

	/** Bitmask of modifier keys of interest. Key events are captured only
		when the modifier keys specified by iModifierMask are in the states
		specified by iModifiers */
	TUint			iModifierMask;

	/** Bitmask of modifier key states */
	TUint			iModifiers;

	/** Opaque handle for this request */
	TAny*			iHandle;

	/** Opaque handle to window group through which request was made */
	TAny*			iWindowGroup;

	/** Identifier of the window group through which request was made */
	TInt			iWindowGroupId;

	/** UID of application that made the capture request */
	TUid			iAppUid;

	/** Capture priority for this request. If more than one window group has
		requested capture for the same key event, the one with the highest
		priority will capture it (unless overridden by application specific
		rules). */
	TInt			iPriority;

	/** Reserved for future use */
	TInt			iReserved[2];
	};

/**
Input parameters for RouteKey()
*/
struct TKeyEventRouterInput
	{
	inline TKeyEventRouterInput(TKeyCaptureType aType, const TKeyEvent& aKeyEvent, TAny* aFocusWindowGroup, TUid aFocusAppUid);

	/** Capture type */
	TKeyCaptureType		iType;

	/** Input key event */
	TKeyEvent			iKeyEvent;

	/** Opaque handle to current focussed window group */
	TAny*				iFocusWindowGroup;

	/** UID of client application with current focussed window group */
	TUid				iFocusAppUid;

	/** Reserved for future use */
	TInt				iReserved[2];
	};

/**
Result codes for RouteKey()
*/
enum TKeyEventRouterResult
	{
	/** Key routed, no capture */
	ERouted = 0,

	/** Key captured and routed */
	ECaptured = 1,

	/** Key consumed, do not deliver event */
	EConsumed = 2
	};

/**
Output parameters for RouteKey()
*/
struct TKeyEventRouterOutput
	{
	/** Result code */
	TKeyEventRouterResult	iResult;

	/** Output key event as translated by plug-in. Key code may be set by
		RWindowGroup::CaptureLongKey() via TKeyCaptureRequest.iOutputCode **/
	TKeyEvent				iKeyEvent;

	/** Opaque handle to destination window group or NULL if captured by WServ
		(hotkey). Plug-in must set this to either the window group from the
		matching capture request or to TKeyEventRouterInput.iFocusWindowGroup */
	TAny*					iWindowGroup;

	/** Opaque handle from matching capture request or NULL if none */
	TAny*					iCaptureHandle;

	/** Reserved for future use */
	TInt					iReserved[2];
	};

/**
Key Event Router Interface

This class is implemented by a plug-in DLL in order to perform customised
routing of window server key events.

The Key Event Routing plug-in is a polymorphic DLL that implements the
CKeyEventRouter interface. Its UID1 and UID2 must be KDynamicLibraryUid and
KKeyRouterPluginUid respectively. UID3 identifies a particular implementation
of the plug-in. The first and only exported function should create and return
an object of the CKeyEventRouter sub-class.
*/
class CKeyEventRouter : public CBase
	{
public:
	/**
	Create and return an instance of CKeyEventRouter

	@return	Pointer to new router instance
	*/
	IMPORT_C static CKeyEventRouter* NewL();

	/**
	Add a new key capture request

	@param	aRequest	Capture request details
	*/
	virtual void AddCaptureKeyL(const TKeyCaptureRequest& aRequest) = 0;
	   
	/**
	Update an existing key capture request

	@param	aRequest	Updated capture request details
	*/
	virtual void UpdateCaptureKeyL(const TKeyCaptureRequest& aRequest) = 0;

	/**
	Cancel a key capture request

	@param	aType		Capture type
	@param	aHandle		Opaque handle of request to be cancelled
	*/
	virtual void CancelCaptureKey(TKeyCaptureType aType, TAny* aHandle) = 0;

	/**
	Route the key event described by aInput and return its destination
	in iOutput.

	@param	aInput		Input data with key event to be routed
	@param	aOutput		Output key event and routing results
	*/
	virtual void RouteKey(const TKeyEventRouterInput& aInput,
						  TKeyEventRouterOutput& aOutput) = 0;

	/**
	Reserved for future use
	*/
private:
	virtual void Reserved1() {};
	virtual void Reserved2() {};
	};

inline TKeyEventRouterInput::TKeyEventRouterInput(TKeyCaptureType aType, const TKeyEvent& aKeyEvent, TAny* aFocusWindowGroup, TUid aFocusAppUid) : iType(aType), iKeyEvent(aKeyEvent), iFocusWindowGroup(aFocusWindowGroup), iFocusAppUid(aFocusAppUid)
	{
	}

#endif // WSKEYROUTER_H
