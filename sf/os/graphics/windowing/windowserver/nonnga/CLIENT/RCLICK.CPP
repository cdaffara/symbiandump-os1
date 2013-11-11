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
// Client side interface to the plugin
// 
//

#include <e32std.h>
#include "../SERVER/w32cmd.h"
#include "CLIENT.H"

EXPORT_C RSoundPlugIn::RSoundPlugIn()
/** Default constructor. */
	{}

EXPORT_C RSoundPlugIn::RSoundPlugIn(RWsSession& aWs) : MWsClientClass(aWs.iBuffer)
/** C++ constructor. 

After calling this function, Construct() must be called to complete construction.

@param aWs Connected session with the window server. */
	{}

EXPORT_C TInt RSoundPlugIn::Construct(TUid aUid)
/** Second phase constructor. 

Creates the server side resource and initialises the client's handle to it. 

This function always causes a flush of the window server buffer.

@param aUid Optional UID. This can be ignored unless you intend to use the CommandReply() 
function. If you do intend to use CommandReply(), this should be the plug-in DLL's third 
UID. CommandReply() will return the value ESoundWrongPlugIn if the plug-in DLL identified 
by this UID is not loaded.
@return KErrNone if the function was successful, otherwise one of the system wide error 
codes. 
@panic TW32Panic 17 in debug builds if called on an already constructed object.*/
	{
	__ASSERT_DEBUG(iWsHandle == KNullHandle, Panic(EW32PanicGraphicDoubleConstruction));
	TInt ret;
	if ((ret=iBuffer->WriteReplyWs(&aUid,sizeof(aUid),EWsClOpCreateClick))>=0)
		{
		iWsHandle=ret;
		ret=KErrNone;
		}
	return(ret);
	}

EXPORT_C void RSoundPlugIn::Close()
/** Sets the handle to zero and frees the resource owned by the server. */
	{
	if (iBuffer && iWsHandle)
		Write(EWsClickOpFree);
	iWsHandle=NULL;
	}

EXPORT_C void RSoundPlugIn::Destroy()
/** Closes (by calling Close()) and deletes the object. */
	{
	Close();
	delete this;
	}

EXPORT_C TBool RSoundPlugIn::IsLoaded(TBool& aIsChangeAble) const
/** Tests whether a key or pointer click plug-in DLL is currently loaded. 

If one is currently loaded, aIsChangeAble returns whether or not 
it can be unloaded. 

This function always causes a flush of the window server buffer.

@param aIsChangeAble If a plug-in is currently loaded, this returns ETrue if it 
can be unloaded and EFalse if it cannot. This depends on whether the 
KEYCLICKPLUGINFIXED keyword is specified in wsini.ini for the plug-in.
@return ETrue if a plug-in is currently loaded, EFalse if not. */
	{
	TUint reply=WriteReply(EWsClickOpIsLoaded);
	aIsChangeAble=reply&EClickLoadable;
	return reply&EClickLoaded;
	}

EXPORT_C TInt RSoundPlugIn::Unload()
/** Unloads the plug-in if one is currently loaded and if it can be unloaded. 

This function always causes a flush of the window server buffer.

@return KErrNone if the function was successful, KErrNotSupported if it could 
not be unloaded.
@capability WriteDeviceData */
	{
	return WriteReply(EWsClickOpUnLoad);
	}

EXPORT_C TInt RSoundPlugIn::Load(const TDesC& aFileName)
/** Loads a new plug-in, replacing the existing one, if any. 

This function always causes a flush of the window server buffer.

@param aFileName The filename of the plug-in DLL to load.
@return KErrNone if the function was successful. KErrNotSupported if the currently 
loaded plug-in could not be unloaded.
@capability WriteDeviceData */
	{
	TInt length=aFileName.Length();
	return WriteReply(&length,sizeof(length),aFileName.Ptr(),aFileName.Size(),EWsClickOpLoad);
	}

EXPORT_C void RSoundPlugIn::SetPenClick(TBool aEnabled)
/**
@publishedPartner
@released

Sets whether pointer clicks should be enabled or disabled.

By default, pointer clicks are enabled. 

@param aEnabled ETrue to enable pointer clicks, EFalse to disable them.
@capability WriteDeviceData */
	{
	WriteInt(aEnabled,EWsClickOpSetPenClick);
	}

EXPORT_C void RSoundPlugIn::SetKeyClick(TBool aEnabled)
/**
@publishedPartner
@released

Sets whether key clicks should be enabled or disabled.

By default, key clicks are enabled. 

@param aEnabled ETrue to enable key clicks, EFalse to disable them. 
@capability WriteDeviceData */
	{
	WriteInt(aEnabled,EWsClickOpSetKeyClick);
	}

EXPORT_C TBool RSoundPlugIn::PenClickEnabled() const
/** Tests whether pointer clicks are enabled, as set by SetPenClick(). 

This function always causes a flush of the window server buffer.

@return ETrue if pointer clicks are enabled, EFalse if they are disabled. */
	{
	return WriteReply(EWsClickOpPenClickEnabled);
	}

EXPORT_C TBool RSoundPlugIn::KeyClickEnabled() const
/** Tests whether key clicks are enabled, as set by SetKeyClick(). 

This function always causes a flush of the window server buffer.

@return ETrue if key clicks are enabled, EFalse if they are disabled. */
	{
	return WriteReply(EWsClickOpKeyClickEnabled);
	}

EXPORT_C TInt RSoundPlugIn::CommandReply(TInt aOpcode, const TPtrC8& aArgs)
/** Sends a command to the plug-in DLL and may receive a response.

If the correct plug-in is loaded, its implementation of  CommandReplyL() 
is called and its return code is returned by this function.

Specify an opcode of zero if you just want the identity of the plug-in DLL 
being used.

This function always causes a flush of the window server buffer.

@param aOpcode Opcode understood by both the window server client and the 
plug-in DLL. If an opcode of zero is specified, this is intercepted by the 
window server, and the third UID of the plug-in is returned. This allows 
the caller to identify the plug-in DLL being used.
@param aArgs Packaged arguments which are passed to the plug-in via the window 
server. 
@return KErrNone or another of the system error codes, as returned by the 
plug-in's CommandReplyL() implementation. ESoundWrongPlugIn is returned if no 
plug-in is loaded, or if the plug-in identified by the aUid parameter in Construct() 
is not loaded. */
	{
	return WriteReply(&aOpcode,sizeof(aOpcode),aArgs.Ptr(),aArgs.Length(),EWsClickOpCommandReply);
	}

