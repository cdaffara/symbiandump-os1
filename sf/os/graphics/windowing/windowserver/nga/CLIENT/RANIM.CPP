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
// Shells Animate DLL
// 
//

#include <e32std.h>
#include "../SERVER/w32cmd.h"
#include "w32comm.h"
#include "CLIENT.H"

EXPORT_C RAnimDll::RAnimDll()
/** Default constructor. 

RAnimDll is a lightweight class which can be allocated as a non-pointer member. 
This constructor is provided to allow such a member to be instantiated while 
its owner is constructed.

Note that an RAnimDll constructed without a reference to a window server session 
is invalid. */
	{}

EXPORT_C RAnimDll::RAnimDll(RWsSession &aWs) : MWsClientClass(aWs.iBuffer)
/** Valid constructor. 

To be useable, an animation DLL should always be constructed with a reference 
to a window server session as argument.

@param aWs Window server session to use. */
	{}

EXPORT_C RAnimDll::~RAnimDll()
/** Empty virtual destructor. 

Destroy() should be used to instead of this function to destroy both this 
object and the server-side CAnimDll object.

@see Destroy()
@see Close() */
	{}

EXPORT_C void RAnimDll::Close()
/** Closes an animation DLL. 

Use this function to close a polymorphic DLL which was previously loaded on 
the server. Cleans up and frees all resources belonging to the DLL but does 
not delete it. Closing is in accordance with the RLibrary mechanism.

Load() and Close() are symmetrical operations. A DLL can be re-loaded after 
a close has been called on it.

To both close and delete a previously loaded DLL, use Destroy(). */
	{
	if (iBuffer && iWsHandle)
		Write(EWsAnimDllOpFree);
	iWsHandle=NULL;
	}

EXPORT_C void RAnimDll::Destroy()
/** Closes and deletes a previously loaded polymorphic DLL. 

This function is equivalent to calling this->Close() followed by delete this, 
in accordance with the RLibrary mechanism provided for managing polymorphic 
DLLs. */
	{
	Close();
	delete this;
	}

EXPORT_C TInt RAnimDll::Load(const TDesC &aFileName)
/** Instructs the server to load an animation DLL. 

Use this function to pass the name of a polymorphic DLL, the animation DLL, 
for loading on the server side. Loading is in accordance with the RLibrary 
mechanism. The DLL must conform to the interface defined by the window server, 
otherwise an error will result. For requirements, see the description of CAnimDll.

Load() and Close() are symmetrical operations. A DLL can be re-loaded after 
a close has been called on it.

This function always causes a flush of the window server buffer.

@param aFileName Filename of the DLL to load. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@panic TW32Panic 17 in debug builds if called on an already constructed object.*/
	{
	 __ASSERT_DEBUG(iWsHandle == KNullHandle, Panic(EW32PanicGraphicDoubleConstruction));
	TWsClCmdLoadAnimDll load;
	load.length=aFileName.Length();
	TInt ret;
	ret=iBuffer->WriteReplyWs(&load,sizeof(load),aFileName.Ptr(),aFileName.Size(),EWsClOpCreateAnimDll);
	if (ret<0)
		return(ret);
	iWsHandle=ret;
	return(KErrNone);
	}

TInt RAnimDll::CommandReply(TInt aHandle, TInt aOpcode, const TDesC8& aArgs, const TIpcArgs* aIpcArgs)
	{
	TInt params[2];
	params[0]=aHandle;
	params[1]=aOpcode;
	return(WriteReply(&params,sizeof(params),aArgs.Ptr(),aArgs.Length(),EWsAnimDllOpCommandReply,aIpcArgs));
	}

TInt RAnimDll::CommandReply(TInt aHandle, TInt aOpcode, const TIpcArgs* aIpcArgs)
	{
	TInt params[2];
	params[0]=aHandle;
	params[1]=aOpcode;
	return(WriteReply(&params,sizeof(params),EWsAnimDllOpCommandReply,aIpcArgs));
	}

void RAnimDll::Command(TInt aHandle, TInt aOpcode, const TPtrC8 &aArgs)
	{
	TInt params[2];
	params[0]=aHandle;
	params[1]=aOpcode;
	Write(&params,sizeof(params),aArgs.Ptr(),aArgs.Length(),EWsAnimDllOpCommand);
	}

void RAnimDll::Command(TInt aHandle, TInt aOpcode)
	{
	TInt params[2];
	params[0]=aHandle;
	params[1]=aOpcode;
	Write(&params,sizeof(params),EWsAnimDllOpCommand);
	}

void RAnimDll::AsyncCommandReply(TRequestStatus& aStatus, TInt aOpcode, TIpcArgs& aIpcArgs)
	{
	__ASSERT_DEBUG((aOpcode&EWservMessAnimDllAsyncCommand)==0, Assert(EW32AssertIllegalOpcode));
	aIpcArgs.Set(KAsyncMessageSlotAnimDllHandle,iWsHandle);
	iBuffer->Flush(); // needs to happen first so that things occur in their correct order, and because this asynchronous function uses a different mechanism, i.e. assigns different meanings to, say, the first slot in the TIpcArgs
	iBuffer->Session()->SendReceive((aOpcode|EWservMessAnimDllAsyncCommand),aIpcArgs,aStatus);
	}

TInt RAnimDll::CreateInstance(TInt32& aHandle, const MWsClientClass &aDevice, TInt aType, const TDesC8 &aArgs, TInt aOpcode, const TIpcArgs* aIpcArgs)
	{
	TInt params[2];
	params[0]=aDevice.WsHandle();
	params[1]=aType;
	const TInt returnValue=WriteReply(&params,sizeof(params),aArgs.Ptr(),aArgs.Length(),aOpcode,aIpcArgs);
	if (returnValue>=0)
		{
		aHandle=returnValue;
		return KErrNone;
		}
	return returnValue;
	}

void RAnimDll::DestroyInstance(TInt aHandle)
	{
	WriteReplyInt(aHandle,EWsAnimDllOpDestroyInstance);
	}

EXPORT_C RAnim::RAnim() : iHandle(0)
/** Default constructor. Developers should use the other constructor overload. */
	{}

EXPORT_C RAnim::RAnim(RAnimDll &aDll) : iHandle(0),iAnimDll(&aDll)
/** Protected C++ constructor.

This constructor should be used to construct an animation object from a given 
animation DLL. The DLL must be loaded first, see the discussion of RAnimDll.

@param aDll The animation DLL. */
	{}

EXPORT_C RAnim::~RAnim()
/** Empty virtual destructor. */
	{}

EXPORT_C TInt RAnim::Construct(const RWindowBase &aDevice, TInt aType, const TDesC8 &aParams)
/** Completes construction of the object based on a window device, and creates 
the server-side animation system. 

Construction information is passed to the server side via aType and aParams. 
The server then passes the information to the function CreateInstanceL() inside 
the Anim DLL.

This function always causes a flush of the window server buffer.

@param aDevice A window device. 
@param aType The type of this animation object, as understood by CAnimDll::CreateInstanceL(). 
@param aParams Packaged arguments which will be passed to the server side object 
to tell it how to construct itself or initialise itself. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see CAnim::ConstructL()
@see CAnimDll::CreateInstanceL() */
	{
	return iAnimDll->CreateInstance(iHandle, aDevice, aType, aParams, EWsAnimDllOpCreateInstance, NULL);
	}

EXPORT_C TInt RAnim::Construct(const RWindowBase &aDevice, TInt aType, const TDesC8 &aParams, const TIpcArgs& aIpcArgs)
/** Completes construction of the object based on a window device, and creates 
the server-side animation system. 

Construction information is passed to the server side via aType and aParams. 
The server then passes the information to the function CreateInstanceL() inside 
the Anim DLL.

This function always causes a flush of the window server buffer.

@param aDevice A window device. 
@param aType The type of this animation object, as understood by CAnimDll::CreateInstanceL(). 
@param aParams Packaged arguments which will be passed to the server side object 
to tell it how to construct itself or initialise itself. 
@param aIpcArgs Inter-process communication arguments which will be passed to the server side 
object. Panics if the first "slot" is set to anything other than TIpcArgs::ENothing.
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see CAnim::ConstructL()
@see CAnimDll::CreateInstanceL() */
	{
	return iAnimDll->CreateInstance(iHandle, aDevice, aType, aParams, EWsAnimDllOpCreateInstance, &aIpcArgs);
	}

EXPORT_C TInt RAnim::Construct(const RWsSprite &aDevice, TInt aType, const TDesC8 &aParams)
/** Completes construction of the Anim DLL based on a sprite. 

Construction information is passed to the server side via aType and aParams. 
The server then passes the information to the function CreateInstanceL() inside 
the Anim DLL.

This function always causes a flush of the window server buffer.

@param aDevice A sprite. 
@param aType The type of this animation object, as understood by CAnimDll::CreateInstanceL(). 
@param aParams Packaged arguments which will be passed to the server side object 
to tell it how to construct itself or initialise itself. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see CAnim::ConstructL()
@see CAnimDll::CreateInstanceL() */
	{
	return iAnimDll->CreateInstance(iHandle, aDevice, aType, aParams, EWsAnimDllOpCreateInstanceSprite, NULL);
	}

EXPORT_C TInt RAnim::Construct(const RWsSprite &aDevice, TInt aType, const TDesC8 &aParams, const TIpcArgs& aIpcArgs)
/** Completes construction of the Anim DLL based on a sprite. 

Construction information is passed to the server side via aType and aParams. 
The server then passes the information to the function CreateInstanceL() inside 
the Anim DLL.

This function always causes a flush of the window server buffer.

@param aDevice A sprite. 
@param aType The type of this animation object, as understood by CAnimDll::CreateInstanceL(). 
@param aParams Packaged arguments which will be passed to the server side object 
to tell it how to construct itself or initialise itself. 
@param aIpcArgs Inter-process communication arguments which will be passed to the server side 
object. Panics if the first "slot" is set to anything other than TIpcArgs::ENothing.
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see CAnim::ConstructL()
@see CAnimDll::CreateInstanceL() */
	{
	return iAnimDll->CreateInstance(iHandle, aDevice, aType, aParams, EWsAnimDllOpCreateInstanceSprite, &aIpcArgs);
	}

EXPORT_C TInt RAnim::CommandReply(TInt aOpcode)
/** Sends a command to the server-side CAnim instance, and waits for a response. 

This function executes synchronously and returns the code returned by the 
server-side method CAnim::CommandReplyL(). The request is not buffered.

@param aOpcode An opcode meaningful to the server-side CAnim-derived class. 
@return A value defined by the animation writer. The value may, in some cases, 
be defined to be an error code.

This function always causes a flush of the window server buffer. 

@see Command()
@see AsyncCommandReply()
@see CAnim::CommandReplyL() */
	{
	return(iAnimDll->CommandReply(iHandle,aOpcode));
	}

EXPORT_C TInt RAnim::CommandReply(TInt aOpcode, const TPtrC8 &aArgs)
/** Sends a command and its arguments to the server-side CAnim instance, and waits 
for a response. 

The packaged command arguments are passed to the matching server side function, 
where the behaviour should be implemented. The request is not buffered. The 
function executes synchronously and returns the code returned by the server-side 
method CAnim::CommandReplyL().

This function always causes a flush of the window server buffer. 

@param aOpcode An opcode meaningful to the server-side CAnim-derived class. 
@param aArgs Packaged arguments which will be passed to the server side object. 
@return A value defined by the animation writer. The value may, in some cases, 
be defined to be an error code. 
@see Command()
@see AsyncCommandReply()
@see CAnim::CommandReplyL() */
	{
	return(iAnimDll->CommandReply(iHandle,aOpcode, aArgs));
	}

EXPORT_C TInt RAnim::CommandReply(TInt aOpcode, const TDesC8& aArgs, const TIpcArgs& aIpcArgs)
/** Sends a command and its arguments to the server-side CAnim instance, and waits 
for a response. 

The IPC (inter-process communication) arguments are passed to the matching server side function, 
where the behaviour should be implemented. The request is not buffered. The 
function executes synchronously and returns the code returned by the server-side 
method CAnim::CommandReplyL(). The first slot of the TIpcArgs parameter is reserved for 
internal use. The second slot is used to receive an additional reply from the server-side CAnim.

This function always causes a flush of the window server buffer. 

@param aOpcode An opcode meaningful to the server-side CAnim-derived class. 
@param aArgs Packaged arguments which will be passed to the server side object. 
@param aIpcArgs Inter-process communication arguments which will be passed to the server side 
object. Panics if the first "slot" is set to anything other than TIpcArgs::ENothing.
@return A value defined by the animation writer. The value may, in some cases, 
be defined to be an error code. 
@see Command()
@see AsyncCommandReply()
@see CAnim::CommandReplyL() */
	{
	return(iAnimDll->CommandReply(iHandle,aOpcode,aArgs,&aIpcArgs));
	}

EXPORT_C void RAnim::Command(TInt aOpcode)
/** Sends a command to the server-side CAnim instance, and returns immediately. 

Commands issued by this function may be buffered before being passed to the 
server.

Server-side errors cannot be detected, so Command() should not be used for 
operations which could fail or which may leave. Use CommandReply() for those. 
Although the window server will not fail due to errors not being returned 
to the client side, client side behaviour will almost certainly not be correct 
if errors have been generated but not received.

Use of this function results in a server-side call to the equivalent CAnim::Command().

@param aOpcode An operation meaningful to the server-side CAnim object. 
@see CommandReply()
@see AsyncCommandReply()
@see CAnim::Command() */
	{
	iAnimDll->Command(iHandle,aOpcode);
	}

EXPORT_C void RAnim::Command(TInt aOpcode, const TPtrC8 &aArgs)
/** Sends a command and its arguments to the server-side CAnim instance, and returns 
immediately. 

Commands issued by this function may be buffered before being passed to the 
server.

Server-side errors cannot be detected, so Command() should not be used for 
operations which could fail or which may leave. Use CommandReply() for those. 
Although the window server will not fail due to errors not being returned 
to the client side, client side behaviour will almost certainly not be correct 
if errors have been generated but not received.

Use of this function results in a server-side call to the equivalent CAnim::Command().

@param aOpcode An operation meaningful to the server-side CAnim object. 
@param aArgs Packaged arguments which will be passed to the server side object. 

@see CommandReply()
@see AsyncCommandReply()
@see CAnim::Command() */
	{
	iAnimDll->Command(iHandle,aOpcode, aArgs);
	}

EXPORT_C void RAnim::AsyncCommandReply(TRequestStatus& aRequestStatus,TInt aOpcode, const TIpcArgs& aIpcArgs)
/** Sends a command and its arguments to the server-side CAnim instance asynchronously. 

The IPC (inter-process communication) arguments are passed to the CAnim-derived class' 
CommandReplyL function, where the behaviour should be implemented. The request is not 
buffered - rather the function executes asynchronously. The first and second slots of the 
TIpcArgs parameter are reserved for internal use. 

If the code calling this function is itself an API that is asynchronous (i.e. if the 
TRequestStatus passed into this function is simply a parameter to a higher-level API), then that 
higher-level API should typically provide a corresponding "Cancel" function that its own clients 
can use to cancel their asynchronous request - this would typically be implemented by the 
higher-level API by using RAnim::CommandReply.

@param aRequestStatus Set initially by the function to KRequestPending. Then when this 
asynchronous function at some potentially later point in time "completes", aRequestStatus 
is set to the completion code which is a value defined by the animation writer (i.e. the value 
that the server-side CAnim-derived class' CommandReplyL returns or leaves with). The value may, 
in some cases, be defined to be an error code. 
@param aOpcode An opcode meaningful to the server-side CAnim-derived class. 
@param aIpcArgs Inter-process communication arguments which will be passed to the server side 
object. Panics if either the first or second "slot" is set to anything other than 
TIpcArgs::ENothing.

@see CommandReply()
@see Command()
@see CAnim::CommandReplyL() */
	{
	TIpcArgs ipcArgs;
	ipcArgs=aIpcArgs; // GCC doesn't seem to like doing a copy constructor on TIpcArgs, probably because it's confused by all the templated-ness of TIpcArgs' constructors
	// check that the caller hasn't used the first or second slot (the first is set by the RAnimDll::AsyncCommandReply call below)
	ipcArgs.Set(KAsyncMessageSlotAnimDllHandle,TIpcArgs::ENothing);
	ipcArgs.Set(KAsyncMessageSlotAnimHandle,TIpcArgs::ENothing);
	__ASSERT_ALWAYS(Mem::Compare(REINTERPRET_CAST(const TUint8*, &ipcArgs), sizeof(TIpcArgs), REINTERPRET_CAST(const TUint8*, &aIpcArgs), sizeof(TIpcArgs))==0,Panic(EW32PanicUsingReservedIpcSlot));
	ipcArgs.Set(KAsyncMessageSlotAnimHandle,iHandle);
	iAnimDll->AsyncCommandReply(aRequestStatus, aOpcode, ipcArgs);
	}

EXPORT_C void RAnim::Close()
/** Sends the close command.

This frees resources belonging to an animation object. It would be called 
to release resources when the RAnim is owned in-line by another object (so 
that destruction is automatic). 

This function always causes a flush of the window server buffer. */
	{
	if (iHandle!=0)
		{
		iAnimDll->DestroyInstance(iHandle);
		iHandle=0;
		}
	}

EXPORT_C void RAnim::Destroy()
/** Closes and deletes the server-side animation object. 

This should be called on heap allocated objects. 

This function always causes a flush of the window server buffer. */
	{
	Close();
	delete this;
	}
