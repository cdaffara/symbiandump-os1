// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of the class that manages the keyclick plugin
// 
//

#include "KEYCLICK.H"

#include "server.h"
#include "W32CLICK.H"
#include "panics.h"
#include "inifile.h"
#include "advancedpointereventhelper.h"

CClickMaker* CClick::iHandler=NULL;
TBool CClick::iIsChangeable=ETrue;
TBool CClick::iKeyClickOveride=EFalse;
TBool CClick::iKeyClickEnabled=ETrue;
TBool CClick::iPenClickEnabled=ETrue;
RLibrary CClick::iPlugIn;
TBool CClick::iIsLoaded=EFalse;

const TUid KClickPlugInDllUid={0x10004F63};
static _LIT_SECURITY_POLICY_C1(KSecurityPolicy_WriteDeviceData,ECapabilityWriteDeviceData);


/*static part*/

void CClick::InitStaticsL()
	{
	_LIT(KClickPlugin,"KEYCLICKPLUGIN");
	TPtrC plugInName;
	if (WsIniFile->FindVar(KClickPlugin,plugInName))
		LoadNewLibraryL(plugInName);
	_LIT(KClickFixed,"KEYCLICKPLUGINFIXED");
	iIsChangeable=!WsIniFile->FindVar(KClickFixed);
	}

void CClick::DeleteStatics()
	{
	if (iIsLoaded)
		{
		delete iHandler;
		iHandler=NULL;
		iPlugIn.Close();
		}
	}

void CClick::KeyEvent(TEventCode aType,const TKeyEvent& aEvent)
	{
	WS_ASSERT_DEBUG(iHandler, EWsPanicClickPluginNotLoaded);
	if (iKeyClickEnabled && !iKeyClickOveride)
		{
		TRAP_IGNORE(iHandler->KeyEvent(aType,aEvent)); // TRAP leaves in case the plugin is badly behaved
		}
	}

void CClick::PointerEvent(const TPoint& aScreenPos,const TAdvancedPointerEvent& aEvent)
	{
	WS_ASSERT_DEBUG(iHandler, EWsPanicClickPluginNotLoaded);
	if (iPenClickEnabled)
		{
		TAdvancedPointerEvent event;
		TAdvancedPointerEventHelper::Copy(aEvent,event);
		event.iParentPosition=aScreenPos;
		TRAP_IGNORE(iHandler->PointerEvent(event)); // TRAP leaves in case the plugin is badly behaved
		}
	}

void CClick::OtherEvent(TInt aType,TAny* aParam)
	{
	WS_ASSERT_DEBUG(iHandler, EWsPanicClickPluginNotLoaded);
	if (aType!=EEventPointer || iPenClickEnabled)
		TRAP_IGNORE(iHandler->OtherEvent(aType,aParam)); // TRAP leaves in case the plugin is badly behaved
	}

void CClick::LoadNewLibraryL(const TDesC &aDllName)
	{
	WS_ASSERT_DEBUG(iIsChangeable, EWsPanicChangeClickPlugin);
	const TUidType uidType(KDllUid,KClickPlugInDllUid);
	RLibrary plugIn;
	User::LeaveIfError(plugIn.Load(aDllName,uidType));
	CleanupClosePushL(plugIn);
	CreateCClickHandler function;
	CClickMaker* handler;
	function=(CreateCClickHandler)User::LeaveIfNull((TAny*)plugIn.Lookup(1));	//Can only cast function pointer with C-style casts
	handler=(*function)();
	if (handler)
		{
		Unload();
		CleanupStack::Pop(&plugIn);
		iPlugIn=plugIn;
		iHandler=handler;
		iIsLoaded=ETrue;
		}
	else
		CleanupStack::PopAndDestroy(&plugIn);
	}

/*object part*/

void CClick::ConstructL(const TUid& aUid)
	{
	NewObjL();
	iThirdUid=aUid;
	}

void CClick::CommandL(TInt aOpcode,const TAny* aCmdData)
	{
	TWsClickCmdUnion pData;
	pData.any=aCmdData;
	switch(aOpcode)
		{
		case EWsClickOpFree:
			delete this;
			break;
		case EWsClickOpIsLoaded:
			{
			TUint reply=0;
			if (IsHandler())
				reply|=EClickLoaded;
			if (iIsChangeable)
				reply|=EClickLoadable;
			SetReply(reply);
			}
			break;
		case EWsClickOpUnLoad:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RSoundPlugIn::UnLoad API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			if (iIsChangeable)
				Unload();
			else
				SetReply(KErrNotSupported);
			}
			break;
		case EWsClickOpLoad:
			{
			if(!KSecurityPolicy_WriteDeviceData().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RSoundPlugIn::Load API")))
				{
				User::Leave(KErrPermissionDenied);
				}
			if (iIsChangeable)
				LoadNewLibraryL(iWsOwner->BufferTPtr((TText*)(pData.Int+1),*pData.Int));
			else 
				SetReply(KErrNotSupported);
			}
			break;
		case EWsClickOpSetKeyClick:
			{
			if(KSecurityPolicy_WriteDeviceData().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RSoundPlugIn::SetKeyClick API")))
				{
				iKeyClickEnabled=*pData.Bool;
				}
			}
			break;
		case EWsClickOpSetPenClick:
			{
			if(KSecurityPolicy_WriteDeviceData().CheckPolicy(iWsOwner->ClientMessage(),__PLATSEC_DIAGNOSTIC_STRING("Capability check failed for RSoundPlugIn::SetPenClick API")))
				{
				iPenClickEnabled=*pData.Bool;
				}
			}
			break;
		case EWsClickOpKeyClickEnabled:
			SetReply(iKeyClickEnabled);
			break;
		case EWsClickOpPenClickEnabled:
			SetReply(iPenClickEnabled);
			break;
		case EWsClickOpCommandReply:
			{
			TInt reply=RSoundPlugIn::ESoundWrongPlugIn;
			if (iHandler && iThirdUid==ThirdUid() && iThirdUid!=TUid::Null())
				TRAP(reply, reply = iHandler->CommandReplyL(*pData.Int,(TAny*)(pData.Int+1)));
			SetReply(reply);
			}
			break;
		default:
			OwnerPanic(EWservPanicOpcode);
			break;
		}
	}
