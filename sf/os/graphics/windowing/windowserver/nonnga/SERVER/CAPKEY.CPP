// Copyright (c) 1995-2009 Nokia Corporation and/or its subsidiary(-ies).
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

TPriQue<CWsCaptureKeyUpsAndDowns> CWsCaptureKeyUpsAndDowns::iCaptureKeysUpsAndDowns(_FOFF(CWsCaptureKeyUpsAndDowns,iLink));
TPriQue<CWsCaptureLongKey> CWsCaptureLongKey::iCaptureLongKeys(_FOFF(CWsCaptureLongKey,iLink));


/*CWsCaptureKey*/

CWsCaptureKey::CWsCaptureKey(CWsWindowGroup *aGroupWin) : CWsObject(aGroupWin?aGroupWin->WsOwner():NULL,WS_HANDLE_CAPTURE_KEY), iWindowGroup(aGroupWin)
	{}

CWsCaptureKey::~CWsCaptureKey()
	{
	TWindowServerEvent::CancelCaptureKey((TUint32)this);
	}

void CWsCaptureKey::CmdToParams(const TWsWinCmdCaptureKey &aCaptureKey, TCaptureKey &aParams)
	{
	aParams.iModifiers.iMask=aCaptureKey.modifierMask;
	aParams.iModifiers.iValue=aCaptureKey.modifiers;
	aParams.iKeyCodePattern.iKeyCode=(TInt16)aCaptureKey.key;
	aParams.iKeyCodePattern.iPattern=EMatchKey;
	aParams.iKeyCodePattern.iFiller=STATIC_CAST(TUint8,aCaptureKey.priority);
	aParams.iApp=(TUint32)iWindowGroup;
	aParams.iHandle=(TUint32)this;
	}

void CheckProtectedKeyL(CWsWindowGroup* aWindowGroup,const TWsWinCmdCaptureKey &aCaptureKey)
	{
	//The key specified in the WSINI file with the keyword: PROTECTEDKEY can only be captured 
	//by a group window with name specified with the PROTECTEDKEYWINDOWNAME keyword.
	TInt protectedKey;
	if(WsIniFile->FindVar(KWsProtectedKey,protectedKey))
		{
		if (aCaptureKey.key == (TUint)protectedKey)
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

void CWsCaptureKey::ConstructL(const TWsWinCmdCaptureKey &aCaptureKey)
	{
	CheckProtectedKeyL(iWindowGroup, aCaptureKey);
	NewObjL();
	TCaptureKey params;
	CmdToParams(aCaptureKey, params);
	TWindowServerEvent::AddCaptureKeyL(params);
	}

void CWsCaptureKey::SetL(const TWsWinCmdCaptureKey &aCaptureKey)
	{
	TCaptureKey params;
	CmdToParams(aCaptureKey, params);
	TWindowServerEvent::SetCaptureKey((TUint32)this, params);
	}

void CWsCaptureKey::CommandL(TInt , const TAny *)
	{
	}


/*CWsCaptureKeyUpsAndDowns*/

CWsCaptureKeyUpsAndDowns::CWsCaptureKeyUpsAndDowns(CWsWindowGroup *aGroupWin) : CWsObject(aGroupWin?aGroupWin->WsOwner():NULL,WS_HANDLE_CAPTURE_KEY_UPDOWNS), iWindowGroup(aGroupWin)
	{}

CWsCaptureKeyUpsAndDowns::~CWsCaptureKeyUpsAndDowns()
	{
	iLink.Deque();
	}

void CWsCaptureKeyUpsAndDowns::ConstructL(const TWsWinCmdCaptureKey &aCaptureKey)
	{
	CheckProtectedKeyL(iWindowGroup, aCaptureKey);
	NewObjL();
	iModifierMask=aCaptureKey.modifierMask;
	iModifierValue=aCaptureKey.modifiers;
	iScanCode=aCaptureKey.key;
	iLink.iPriority=aCaptureKey.priority + 1;
	iCaptureKeysUpsAndDowns.Add(*this);
	--iLink.iPriority;
	}

void CWsCaptureKeyUpsAndDowns::CommandL(TInt , const TAny *)
	{
	}

CWsWindowGroup *CWsCaptureKeyUpsAndDowns::CheckForCapture(TUint aScanCode, TUint aModifiers)
	{
	TDblQueIter<CWsCaptureKeyUpsAndDowns> iter(iCaptureKeysUpsAndDowns);
	CWsCaptureKeyUpsAndDowns* cap;
	while ((cap=iter++)!=NULL)
		{
		if (cap->iScanCode==aScanCode && (aModifiers&cap->iModifierMask)==cap->iModifierValue)
			return(cap->iWindowGroup);
		}
	return NULL;
	}


/*CWsCaptureLongKey*/

CWsCaptureLongKey::CWsCaptureLongKey(CWsWindowGroup *aGroupWin) 
	:CWsObject(aGroupWin?aGroupWin->WsOwner():NULL,WS_HANDLE_CAPTURE_LONG_KEY), iWindowGroup(aGroupWin)
	{}

CWsCaptureLongKey::~CWsCaptureLongKey()
	{
	iLink.Deque();
	}

void CWsCaptureLongKey::ConstructL(const TWsWinCmdCaptureLongKey &aCaptureKey)
	{
	NewObjL();
	iData=aCaptureKey;
	if (iData.delay.Int()<0)
		{
		TTimeIntervalMicroSeconds32 time;
		CKeyboardRepeat::GetRepeatTime(iData.delay,time);
		}
	iLink.iPriority=iData.priority + 1;
	iCaptureLongKeys.Add(*this);
	--iLink.iPriority;
	}

void CWsCaptureLongKey::CommandL(TInt , const TAny *)
	{
	}

CWsCaptureLongKey* CWsCaptureLongKey::CheckForCapture(TUint aKeyCode, TInt aModifiers)
	{
	TDblQueIter<CWsCaptureLongKey> iter(iCaptureLongKeys);
	CWsCaptureLongKey* longCapture;
	while ((longCapture=iter++)!=NULL)
		{
		if (aKeyCode==longCapture->iData.inputKey && (aModifiers&longCapture->iData.modifierMask)==longCapture->iData.modifiers)
			return longCapture;
		}
	return NULL;
	}
