// Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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

// Capture key & hot key classes
//
#include <e32std.h>
#include "server.h"
#include "windowgroup.h"
#include "EVENT.H"
#include "inifile.h"

_LIT(KWsProtectedKey, "PROTECTEDKEY");
_LIT(KWsProtectedWindowName, "PROTECTEDKEYWINDOWNAME");


/*CWsCaptureKey*/

CWsCaptureKey::CWsCaptureKey(CWsWindowGroup *aGroupWin) : CWsObject(aGroupWin?aGroupWin->WsOwner():NULL,WS_HANDLE_CAPTURE_KEY), iWindowGroup(aGroupWin)
	{}

CWsCaptureKey::~CWsCaptureKey()
	{
	TWindowServerEvent::CancelCaptureKey(ECaptureTypeKey, this);
	}

/**
Convert a window server key capture command to a capture request for the
key routing plug-in.

@param	aCaptureKey		Input capture command
@param	aRequest		Output capture request
*/
void CWsCaptureKey::CmdToRequest(const TWsWinCmdCaptureKey &aCaptureKey, TKeyCaptureRequest &aRequest)
	{
	aRequest.iType = ECaptureTypeKey;
	aRequest.iModifierMask = aCaptureKey.modifierMask;
	aRequest.iModifiers = aCaptureKey.modifiers;
	aRequest.iInputCode = aCaptureKey.key;
	aRequest.iOutputCode = aCaptureKey.key;
	aRequest.iPriority = aCaptureKey.priority;
	aRequest.iWindowGroup = iWindowGroup;
	aRequest.iWindowGroupId = iWindowGroup ? iWindowGroup->Identifier() : 0;
	aRequest.iAppUid = iWsOwner ? TUid::Uid(iWsOwner->SecureId().iId) : KNullUid;
	aRequest.iHandle = this;
	}

/**
Check for protected key in a capture command

@param	aWindowGroup	Window Group of capture request
@param	aCaptureKey		Key capture command

@leave KErrPermissionDenied		Capture key is protected
*/
void CheckProtectedKeyL(CWsWindowGroup* aWindowGroup,const TWsWinCmdCaptureKey &aCaptureKey)
	{
	//The key specified in the WSINI file with the keyword: PROTECTEDKEY can only be captured 
	//by a group window with name specified with the PROTECTEDKEYWINDOWNAME keyword.
	TInt protectedKey;
	if(WsIniFile->FindVar(KWsProtectedKey,protectedKey))
		{
		if (aCaptureKey.key == static_cast<TUint>(protectedKey))
			{
			if (aWindowGroup->GroupName()==NULL)
				{
				User::Leave(KErrPermissionDenied);
				}
				
			TPtrC wsProtectedWindowName;
			WsIniFile->FindVar(KWsProtectedWindowName,wsProtectedWindowName);
			if (aWindowGroup->GroupName()->Find(wsProtectedWindowName)==KErrNotFound)
				{
				User::Leave(KErrPermissionDenied);
				}
			}
		}
	}

/**
Construct a capture object for normal key events and make a capture request
to the key routing plug-in.

@param	aCaptureKey		Key capture command from RWindowGroup::CaptureKey(),
						RWsSession::SetHotKey() or default hot key settings.
*/
void CWsCaptureKey::ConstructL(const TWsWinCmdCaptureKey &aCaptureKey)
	{
	CheckProtectedKeyL(iWindowGroup, aCaptureKey);
	NewObjL();

	TKeyCaptureRequest request;
	CmdToRequest(aCaptureKey, request);
	TWindowServerEvent::AddCaptureKeyL(request);
	}

/**
Make a capture request update for normal key events to the key routing plug-in.

@param	aCaptureKey		Key capture command from CWsHotKey::SetL()

Note: this function is used only to disable hot key capture requests or to
reset them to their defaults.
*/
void CWsCaptureKey::SetL(const TWsWinCmdCaptureKey &aCaptureKey)
	{
	TKeyCaptureRequest request;
	CmdToRequest(aCaptureKey, request);
	TWindowServerEvent::UpdateCaptureKeyL(request);
	}

void CWsCaptureKey::CommandL(TInt , const TAny *)
	{
	}


/*CWsCaptureKeyUpsAndDowns*/

CWsCaptureKeyUpsAndDowns::CWsCaptureKeyUpsAndDowns(CWsWindowGroup *aGroupWin) : CWsObject(aGroupWin?aGroupWin->WsOwner():NULL,WS_HANDLE_CAPTURE_KEY_UPDOWNS), iWindowGroup(aGroupWin)
	{}

CWsCaptureKeyUpsAndDowns::~CWsCaptureKeyUpsAndDowns()
	{
	TWindowServerEvent::CancelCaptureKey(ECaptureTypeKeyUpDown, this);
	}

/**
Construct a capture object for up/down key events and make a capture request
to the key routing plug-in.

@param	aCaptureKey		Key capture command from
						RWindowGroup::CaptureKeyUpAndDowns().
*/
void CWsCaptureKeyUpsAndDowns::ConstructL(const TWsWinCmdCaptureKey &aCaptureKey)
	{
	CheckProtectedKeyL(iWindowGroup, aCaptureKey);
	NewObjL();

	TKeyCaptureRequest request;
	request.iType = ECaptureTypeKeyUpDown;
	request.iInputCode = aCaptureKey.key;
	request.iOutputCode = aCaptureKey.key;
	request.iModifiers = aCaptureKey.modifiers;
	request.iModifierMask = aCaptureKey.modifierMask;
	request.iPriority = aCaptureKey.priority;
	request.iWindowGroup = iWindowGroup;
	request.iWindowGroupId = iWindowGroup ? iWindowGroup->Identifier() : 0;
	request.iAppUid = iWsOwner ? TUid::Uid(iWsOwner->SecureId().iId) : KNullUid;
	request.iHandle = this;
	TWindowServerEvent::AddCaptureKeyL(request);
	}

void CWsCaptureKeyUpsAndDowns::CommandL(TInt , const TAny *)
	{
	}


/*CWsCaptureLongKey*/

CWsCaptureLongKey::CWsCaptureLongKey(CWsWindowGroup *aGroupWin) 
	:CWsObject(aGroupWin?aGroupWin->WsOwner():NULL,WS_HANDLE_CAPTURE_LONG_KEY), iWindowGroup(aGroupWin)
	{}

CWsCaptureLongKey::~CWsCaptureLongKey()
	{
	TWindowServerEvent::CancelCaptureKey(ECaptureTypeLongKey, this);
	}

/**
Construct a capture object for long key events and make a capture request
to the key routing plug-in.

@param	aCaptureKey		Key capture command from RWindowGroup::CaptureLongKey()
*/
void CWsCaptureLongKey::ConstructL(const TWsWinCmdCaptureLongKey &aCaptureKey)
	{
	NewObjL();
	iFlags = aCaptureKey.flags;
	iDelay = aCaptureKey.delay;
	if (iDelay.Int() < 0)
		{
		TTimeIntervalMicroSeconds32 time;
		CKeyboardRepeat::GetRepeatTime(iDelay, time);
		}

	TKeyCaptureRequest request;
	request.iType = ECaptureTypeLongKey;
	request.iInputCode = aCaptureKey.inputKey;
	request.iOutputCode = aCaptureKey.outputKey;
	request.iModifiers = aCaptureKey.modifiers;
	request.iModifierMask = aCaptureKey.modifierMask;
	request.iPriority = aCaptureKey.priority;
	request.iWindowGroup = iWindowGroup;
	request.iWindowGroupId = iWindowGroup ? iWindowGroup->Identifier() : 0;
	request.iAppUid = iWsOwner ? TUid::Uid(iWsOwner->SecureId().iId) : KNullUid;
	request.iHandle = this;
	TWindowServerEvent::AddCaptureKeyL(request);
	}

void CWsCaptureLongKey::CommandL(TInt , const TAny *)
	{
	}
