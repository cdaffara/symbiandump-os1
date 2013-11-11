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
// IPC implementation of client side code
// 
//

#include "../SERVER/w32cmd.h"
#include "w32comm.h"
#include <e32std.h>
#include <w32std.h>
#include "CLIENT.H"
#include "graphics/windowserverconstants.h"
#include "rtfxeffect.h"
#include <wspublishandsubscribedata.h>

const TInt KMaxWSERVMessagesSlot=-1;

class RWsCustomTextCursor : public RWsSpriteBase
	{
	public:
		RWsCustomTextCursor(RWsSession aWs, TInt aHandle) : RWsSpriteBase(aWs)
			{ iWsHandle = aHandle; }
	};


EXPORT_C RWsSession::RWsSession()
/** Default C++ constructor.

Constructs an uninitialised window server session. Note that it does not establish 
a connection to the window server - this must be done explicitly by calling 
the session's Connect() function. Before Connect() is called, no corresponding 
session object exists in the server, and the RWsSession contains no meaningful 
handle. */
	{}

void RWsSession::connectL()
	{
	iBuffer=new(ELeave) RWsBuffer(this);
	iBuffer->SetBufferSizeL(RWsBuffer::EDefBufferSize);
	User::LeaveIfError(CreateSession(KWSERVServerName,Version(),KMaxWSERVMessagesSlot));
	iWsHandle=User::LeaveIfError(SendReceive(EWservMessInit,TIpcArgs()));
	}

EXPORT_C TInt RWsSession::Connect()
/** Connects the client session to the window server.

Connect() should be the first function called on an RWsSession object after 
it is created. The function establishes a connection to the window server, 
creating a corresponding session object in the server. Each session has one 
and only one connection to the server. Attempting to call Connect() when 
a connection has already been made will cause a panic.

After a connection has been successfully established, all events are delivered 
to the client application through the RWsSession object.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TInt ret;

	if (iBuffer!=NULL)
		Panic(EW32PanicReConnect);
 	if ((ret=RFbsSession::Connect())==KErrNone)
		{
		TRAP(ret,connectL());
		if (ret!=KErrNone)
			{
			if (!iBuffer)
				RFbsSession::Disconnect();
			Close();
			}
		else
			iBuffer->SetCallBack();
		}
	return(ret);
	}

EXPORT_C TInt RWsSession::Connect(RFs& aFileServer)
/** Connects the client session to the window server using pre constructed file server
session.

Connect() should be the first function called on an RWsSession object after 
it is created. The function establishes a connection to the window server, 
creating a corresponding session object in the server. Each session has one 
and only one connection to the server. Attempting to call Connect() when 
a connection has already been made will cause a panic.

After a connection has been successfully established, all events are delivered 
to the client application through the RWsSession object.

@param aFileServer A fully constructed file server session
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TInt ret;

	if (iBuffer!=NULL)
		Panic(EW32PanicReConnect);
 	if ((ret=RFbsSession::Connect(aFileServer))==KErrNone)
		{
		TRAP(ret,connectL());
		if (ret!=KErrNone)
			{
			if (!iBuffer)
				RFbsSession::Disconnect();
			Close();
			}
		else
			iBuffer->SetCallBack();
		}
	return(ret);
	}

EXPORT_C void RWsSession::Close()
/** Closes the window server session. 

This function cleans up all resources in the RWsSession and disconnects it 
from the server. Prior to disconnecting from the window server, the client-side 
window server buffer is destroyed without being flushed. This function should 
be called when the RWsSession is no longer needed - normally just before 
it is destroyed. */
	{
	if (iBuffer)
		{
		__ASSERT_ALWAYS(iBuffer->iDirectAcessCount==0,Panic(EW32PanicDirectMisuse));
		iBuffer->CancelCallBack();
		iBuffer->Destroy();
		iBuffer=NULL;
		RFbsSession::Disconnect();
		}
	RSessionBase::Close();
	}

// Private function(s)

TInt RWsSession::DoSyncMsgBuf(const TIpcArgs& aIpcArgs)
	{
	return (SendReceive(EWservMessSyncMsgBuf,aIpcArgs));
	}
	
TInt RWsSession::DoFlush(const TIpcArgs& aIpcArgs)
	{
	return (SendReceive(EWservMessCommandBuffer,aIpcArgs));
	}

EXPORT_C TVersion RWsSession::Version() const
/** Gets the window server version.

@return Window server version containing major and minor version numbers, 
and build number. */
	{

	TVersion v(KWservMajorVersionNumber,KWservMinorVersionNumber,KWservBuildVersionNumber);
	return(v);
	}

TInt RWsSession::doSetHotKey(TInt aOpcode, TInt aType, TUint aKeycode, TUint aModifierMask, TUint aModifiers)
	{
	TWsClCmdSetHotKey setHotKey;

	setHotKey.type=aType;
	setHotKey.keycode=(TUint16)aKeycode;
	setHotKey.modifiers=aModifiers;
	setHotKey.modifierMask=aModifierMask;
	return(WriteReply(&setHotKey,sizeof(setHotKey),aOpcode));
	}

EXPORT_C TInt RWsSession::SetHotKey(THotKey aType, TUint aKeycode, TUint aModifierMask, TUint aModifiers)
/** Sets the hot keys.

Hot keys allow standard functions to be performed by application-defined key 
combinations. 

This function maps any key press (with optional modifiers) to one of the hot 
keys defined in THotKey. More than one key combination may be mapped to each 
hot key: a new mapping is added each time the function is called.

Modifier key states are defined in TEventModifier. The modifiers that you 
want to be in a particular state should be specified in aModifierMask and 
the ones of these you want to be set should be specified in aModifiers. For 
example, if you want to capture FN-A and you want the SHIFT modifier unset, 
but you don't care about the state of the other modifiers then set both the 
flags for SHIFT and FN in aModiferMask and only set FN in aModifiers.

Note: default hotkey settings exist, but this function can be used for customisation. 
Typically it might be be used by a shell application or other application 
that controls system-wide settings.

This function always causes a flush of the window server buffer.

@param aType The hot key to be mapped
@param aKeyCode The keycode to be mapped to the hot key
@param aModifierMask Modifier keys to test for a match
@param aModifiers Modifier keys to be tested for "on" state 
@return KErrNone value if successful, otherwise another of the system-wide 
error codes.
@capability SwEvent */
	{
	return(doSetHotKey(EWsClOpSetHotKey, aType, aKeycode, aModifierMask, aModifiers));
	}

EXPORT_C TInt RWsSession::ClearHotKeys(THotKey aType)
/** Clears all mappings for the specified hotkey, including the default mapping.

Hotkeys allow standard functions to be performed by application-defined key 
combinations. 

This function always causes a flush of the window server buffer.

@param aType The hot key to be cleared 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see SetHotKey()
@see RestoreDefaultHotKey()
@capability SwEvent */
	{
	return(WriteReplyInt(aType,EWsClOpClearHotKeys));
	}

EXPORT_C TInt RWsSession::RestoreDefaultHotKey(THotKey aType)
/** Restores the default mapping for a hot key.

The function clears current mappings for a hot key and restores the default 
mapping. See THotKey for the default.

This function always causes a flush of the window server buffer.

@param aType The hot key to restore to its default value 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see ClearHotKeys() */
	{
	return(WriteReplyInt(aType,EWsClOpRestoreDefaultHotKey));
	}

EXPORT_C void RWsSession::ComputeMode(TComputeMode aMode)
/** Sets the mode used to control process priorities.

The default window server behaviour is that the application that owns the 
window with keyboard focus gets foreground process priority (EPriorityForeground) 
while all other clients get background priority (EPriorityBackground). This 
function can be used to over-ride this default behaviour, as discussed in 
TComputeMode.

Note:

Unlike real Symbian phones, the Emulator runs on a single process. As a result, 
on the Emulator this function sets the priority of individual threads rather 
than of processes. The values used for thread priorities are EPriorityLess 
instead of EPriorityBackground, and EPriorityNormal instead of EPriorityForeground.

@param aMode The compute mode. */
	{
	WriteInt(aMode,EWsClOpComputeMode);
	}

/**
This method has been deprecated. Shadowing of a window is no longer supported. 
Calling it has no effect.
@param aVector Ignored.
@deprecated
*/
EXPORT_C void RWsSession::SetShadowVector(const TPoint& /*aVector*/)
	{
	}

/**
This method has been deprecated. Shadowing of a window is no longer supported.
Calling it has no effect.
@return TPoint(0, 0)
@deprecated
*/
EXPORT_C TPoint RWsSession::ShadowVector() const
	{
	return TPoint();
	}

void RWsSession::doReadEvent(TRequestStatus *aStat, TInt aOpcode)
	{
	*aStat=KRequestPending;
	if (iBuffer)
		{
		iBuffer->Flush(); //ignore error since this flushing should not return any error
		}
	__ASSERT_DEBUG(EWservMessAsynchronousService>=aOpcode, Assert(EW32AssertIllegalOpcode));
	const TInt function=EWservMessAsynchronousService|aOpcode;
	SendReceive(function,TIpcArgs(),*aStat);
	}

EXPORT_C void RWsSession::EventReady(TRequestStatus* aStat)
/** Requests standard events from the window server.

Standard events include all events except redraws and priority key events.

The client application will typically handle the completed request using the 
RunL() function of an active object, and in this case the request status aStat 
should be the iStatus member of that CActive object.

Notes:

- The active object runs when an event is waiting. You should call GetEvent() 
in the RunL() function to get the event.

- You should not call this function again until you have either called GetEvent() 
or EventReadyCancel().

- Because this function is asynchronous, there is no guarantee that the Window 
Server will process the request before the function returns. However, on single 
core systems it is unusual for this function to return before the Window Server 
has processed the request, because the client generally runs in a lower priority 
thread than the Window Server. You should therefore expect the use of this 
function to give rise to different behaviour between single and multicore systems.

@param aStat Request status. On successful completion contains KErrNone, otherwise 
another of the system-wide error codes.
@see CActive
@see GetEvent() */
	{
	doReadEvent(aStat,EWsClOpEventReady);
	}

EXPORT_C void RWsSession::RedrawReady(TRequestStatus* aStat)
/** Requests redraw events from the window server.

Typically, a client will create an active object for redraw events with a 
lower priority than the active objects for standard events. The client will 
then typically handle completed redraw requests in the active object's RunL() 
function.

As in EventReady(), the request status aStat should be used as the iStatus 
member of an active object. When a redraw event occurs the active object's 
RunL() function is called. The redraw event can be obtained by calling 
GetRedraw() in the RunL().

Notes:

- You should not call this function again until you have either called 
GetRedraw() or RedrawReadyCancel().

- Because this function is asynchronous, there is no guarantee that the Window 
Server will process the request before the function returns. However, on single 
core systems it is unusual for this function to return before the Window Server 
has processed the request, because the client generally runs in a lower priority 
thread than the Window Server. You should therefore expect the use of this 
function to give rise to different behaviour between single and multicore systems.

@param aStat The request status. On successful completion contains KErrNone, 
otherwise another of the system-wide error codes. 
@see RedrawReadyCancel()
@see GetRedraw()
@see CActive */
	{
	doReadEvent(aStat,EWsClOpRedrawReady);
	}
	
void RWsSession::GraphicMessageReady(TRequestStatus *aStat)
	{
	doReadEvent(aStat,EWsClOpGraphicMessageReady);
	}
	
void RWsSession::GetGraphicMessage(TDes8& aData) const
	{
	WriteReplyP(TWriteDescriptorType(&aData),EWsClOpGetGraphicMessage);
	}
	
void RWsSession::GraphicMessageCancel()
	{
	WriteReply(EWsClOpGraphicMessageCancel);
	}

void RWsSession::GraphicAbortMessage(TInt aError)
	{
	WriteReplyInt(aError, EWsClOpGraphicAbortMessage);
	}

TInt RWsSession::GraphicFetchHeaderMessage()
	{
	return WriteReply(EWsClOpGraphicFetchHeaderMessage);
	}
	
EXPORT_C void RWsSession::PriorityKeyReady(TRequestStatus *aStat)
/** Requests priority key events from the window server. 

Typically, an client will create an active object for priority key events 
with a higher priority than the active objects for standard events. The client 
will then normally handle completed priority key requests in the active object's 
RunL() function.

As in EventReady(), the request status argument should be the set to the iStatus 
member of CActive. When priority key events occur, they are obtained using 
GetPriorityKey().

Notes:

- You should not call this function again until you have either called 
GetPriorityKey() or PriorityKeyReadyCancel().

- Because this function is asynchronous, there is no guarantee that the Window 
Server will process the request before the function returns. However, on single 
core systems it is unusual for this function to return before the Window Server 
has processed the request, because the client generally runs in a lower priority 
thread than the Window Server. You should therefore expect the use of this 
function to give rise to different behaviour between single and multicore systems.

@param aStat Request status. On successful completion contains KErrNone, otherwise 
another of the system-wide error codes. 
@see PriorityKeyReadyCancel()
@see GetPriorityKey()
@see CActive */
	{
	doReadEvent(aStat,EWsClOpPriorityKeyReady);
	}

EXPORT_C void RWsSession::GetEvent(TWsEvent &aEvent) const
/** Gets a standard event from the session for processing.

The type of event returned by GetEvent() may be any of those listed in TEventCode. 
To access the data within an event, the event should be converted to the appropriate 
type, using functions provided by the TWsEvent class. TWsEvent also provides 
a function to find out the type of the event.

Notes:

It is possible that the returned event is of type EEventNull. Clients should 
normally ignore these events.

This function should only be called in response to notification that an event 
has occurred, otherwise the client will be panicked.

This function would normally be called in the RunL() function of an active 
object which completes with the EventReady() function's request status.

This function always causes a flush of the window server buffer.

@param aEvent On return, contains the event that occurred 
@see TEventCode
@see EventReady() */
	{
	TPckgBuf<TWsEvent> event;
	WriteReplyP(&event,EWsClOpGetEvent);
	aEvent=event();
	}

EXPORT_C void RWsSession::PurgePointerEvents()
/** Removes all pointer events waiting to be delivered to this session. 

The events are removed from the event queue without being processed. This 
might occur, for example, at application startup. */
	{
	Write(EWsClOpPurgePointerEvents);
	}

EXPORT_C void RWsSession::GetRedraw(TWsRedrawEvent &aEvent)
/** Gets the redraw event from the session.

This function is similar to GetEvent(), except that the event is returned 
as a TWsRedrawEvent, and hence there is no need to convert it from a TWsEvent.

The function should only be called after notification that a redraw is waiting.

It always causes a flush of the window server buffer.

@param aEvent On return, contains the redraw event that occurred 
@see RedrawReady()
@see GetEvent()
@see CActive */
	{
	TPckgBuf<TWsRedrawEvent> redraw;
	WriteReplyP(&redraw,EWsClOpGetRedraw);
	aEvent=redraw();
	}

EXPORT_C void RWsSession::GetPriorityKey(TWsPriorityKeyEvent &aEvent) const
/** Gets the completed priority key event from the window server session.

Priority key events are typically used for providing "Abort" or "Escape" keys 
for an application.

This function is similar to GetEvent(), except that it returns a TWsPriorityKeyEvent 
instead of a TWsEvent.

Note: this should only be called after notification that a priority key event has 
occurred.

This function always causes a flush of the window server buffer.

@param aEvent On return, contains the priority key event that occurred. 
@see PriorityKeyReady()
@see PriorityKeyReadyCancel() */
	{
	TPckgBuf<TWsPriorityKeyEvent> abortEvent;
	WriteReplyP(&abortEvent,EWsClOpGetPriorityKey);
	aEvent=abortEvent();
	}

EXPORT_C void RWsSession::EventReadyCancel()
/** Cancels a request for standard events from the window server.

This request was made using EventReady().

The client application will typically use an active object to handle standard 
events, and this function should be called from the active object's DoCancel() 
function.

This function always causes a flush of the window server buffer.

@see EventReady() */
	{
	if (iWsHandle)
		WriteReply(EWsClOpEventReadyCancel);
	}

EXPORT_C void RWsSession::RedrawReadyCancel()
/** Cancels a redraw event request. 

If active objects are used, this function should be called from the active 
object's DoCancel() function.

This function always causes a flush of the window server buffer.

@see RedrawReady() */
	{
	if (iWsHandle)
		WriteReply(EWsClOpRedrawReadyCancel);
	}

EXPORT_C void RWsSession::PriorityKeyReadyCancel()
/** Cancels a priority key event request.

If active objects are used, this function should be called from the active 
object's DoCancel() function.

This function always causes a flush of the window server buffer.

@see PriorityKeyReady()
@see CActive */
	{
	if (iWsHandle)
		WriteReply(EWsClOpPriorityKeyReadyCancel);
	}

EXPORT_C void RWsSession::Flush()
/** Sends all pending commands in the buffer to the window server. 

Delivering a command to the window server requires a context switch, and so 
it is more efficient to deliver several commands in one go. Hence all client 
commands are normally first placed into a buffer for delivery to the window 
server. 

The buffer is delivered when it gets full, or when a command that returns a value 
is called (there are a few exceptions to this), or when this function is called.

Note: this function is called when a prompt response is required from the window 
server, e.g. after doing some drawing.

@see RWsSession::SetAutoFlush()
@see RWsSession::SetBufferSizeL()
@see RWsSession::SetMaxBufferSizeL()
*/
	{
	if (iBuffer)
		iBuffer->Flush(NULL,ETrue);
	}

EXPORT_C TBool RWsSession::SetAutoFlush(TBool aState)
/** Sets a session's auto-flush state. 

If auto-flush is set to ETrue, the window server buffer is flushed immediately 
anything is put into it, instead of waiting until it becomes full. This setting 
is normally used only in a debugging environment. 

If the auto-flush state is EFalse, the window server buffer is flushed normally.

@param aState ETrue to set auto-flushing on, EFalse to disable auto-flushing. 
@return Previous auto-flush state

@see RWsSession::Flush()
@see RWsSession::SetBufferSizeL()
@see RWsSession::SetMaxBufferSizeL()
*/
	{
	return(iBuffer->SetAutoFlush(aState));
	}

EXPORT_C TInt RWsSession::NumWindowGroups() const
/** Gets the total number of window groups currently running in the window server. 

This includes all the groups running in all sessions.

This function always causes a flush of the window server buffer.

@return Total number of window groups running in the server */
	{
	return(WriteReply(EWsClOpNumWindowGroupsAllPriorities));
	}

EXPORT_C TInt RWsSession::NumWindowGroups(TInt aPriority) const
/** Gets the number of window groups of a given window group priority running in 
all sessions in the window server.

This function always causes a flush of the window server buffer.

@param aPriority Window group priority 
@return Number of window groups of priority aPriority */
	{
	return(WriteReplyInt(aPriority,EWsClOpNumWindowGroups));
	}

EXPORT_C TInt RWsSession::NumWindowGroups(TInt aScreenNumber,TInt aPriority) const
/** Gets the number of window groups of a given window group priority running on a specified screen

This function always causes a flush of the window server buffer.

@param aScreenNumber specifies the screen. 
@param aPriority Window group priority. EAllPriorities is the enum for all priorities 
@return Number of window groups of priority aPriority on the specified screen */
	{
	TWsClCmdNumWindowGroups params;
	params.screenNumber = aScreenNumber;
	params.priority = aPriority;
	return(WriteReply(&params,sizeof(params),EWsClOpNumWindowGroupsOnScreen));	
	}

TInt RWsSession::doWindowGroupList(TInt aScreenNumber, TInt aPriority, CArrayFixFlat<TInt>* aWindowList, TInt aNumOpcode, TInt aListOpcode) const
/** Gets the id of window groups of specified priority running on a specified screen.

This function always causes a flush of the window server buffer.

@param aScreenNumber specifies the screen.
@param aPriority Window group priority 
@param aWindowList List of identifiers
@return KErrNone if successful, otherwise another of the system-wide error 
codes.*/
	{
	TWsClCmdWindowGroupList params;
	params.priority = aPriority;		
	params.screenNumber = aScreenNumber;
	if(aScreenNumber!=KDummyScreenNumber)
		{
		TWsClCmdNumWindowGroups numWinGrp;
		numWinGrp.screenNumber = aScreenNumber;
		numWinGrp.priority = aPriority;	
		params.count=WriteReply(&numWinGrp,sizeof(numWinGrp),aNumOpcode);
		}
	else
		params.count=WriteReplyInt(aPriority,aNumOpcode);

	TRAPD(err,aWindowList->ResizeL(params.count));
	if (err!=KErrNone || params.count==0)
		return(err);
	TPtr8 listPtr(reinterpret_cast<TUint8*>(&(*aWindowList)[0]),params.count*sizeof((*aWindowList)[0]));
	TInt actualCount=WriteReplyP(&params, sizeof(params), &listPtr, aListOpcode);
	err=KErrNone;
	if (actualCount<0)
		{
		err=actualCount;
		actualCount=0;
		}
	if (actualCount<params.count)
		aWindowList->ResizeL(actualCount);	// Can't fail, only shrinking it
	return(err);
	}

TInt RWsSession::doWindowGroupList(TInt aPriority, RArray<TWindowGroupChainInfo>* aWindowListCh, TInt aNumOpcode, TInt aListOpcode) const
/** Gets the id of window groups and id of its parent window group of specified priority running in 
all sessions in the window server.

This function always causes a flush of the window server buffer.

@param aPriority Window group priority 
@param aWindowList List of identifiers
@return KErrNone if successful else KErrNoMemory if there is insufficient memory to create the array. 
This function will panic if aWindowListCh is Null.*/
	{
	__ASSERT_ALWAYS(aWindowListCh,Panic(EW32PanicNullArray));
	TWsClCmdWindowGroupList params;
	params.priority=aPriority;
	params.count=WriteReplyInt(aPriority,aNumOpcode);
	aWindowListCh->Reset();
	TUint8* WindowList=static_cast<TUint8*>(User::Alloc(params.count*sizeof(TWindowGroupChainInfo)));
	if(WindowList==NULL)
		{
		return KErrNoMemory;
		}	
	TPtr8 listPtr(WindowList,params.count*sizeof(TWindowGroupChainInfo));
	WriteReplyP(&params, sizeof(params), &listPtr, aListOpcode);
	new(aWindowListCh) RArray<TWindowGroupChainInfo>(sizeof(TWindowGroupChainInfo),(TWindowGroupChainInfo*)WindowList,params.count);
	return KErrNone;
	}

EXPORT_C TInt RWsSession::WindowGroupList(CArrayFixFlat<TInt>* aWindowList) const
/** Gets a list of identifiers of all window groups in all window server sessions.

An array buffer must be created to store the resultant list.

This function always causes a flush of the window server buffer.

@param aWindowList List of identifiers of all window groups in the server.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	return(doWindowGroupList(KDummyScreenNumber,0,aWindowList,EWsClOpNumWindowGroupsAllPriorities,EWsClOpWindowGroupListAllPriorities));
	}

EXPORT_C TInt RWsSession::WindowGroupList(CArrayFixFlat<TInt>* aWindowList,TInt aScreenNumber,TInt aPriority) const
/** Lists the number of window groups of a given window group priority running on a specified screen.

This function is the same as WindowGroupList() described above, but allows 
the application to restrict the list of window groups to those of a particular 
window group priority.

This function always causes a flush of the window server buffer.

@param aScreenNumber specifies the screen
@param aPriority Window group priority . Enum for all priorities is EAllPriorities
@param aWindowList List of identifiers of all window groups on the specified screen with the given priority 
aPriority.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	return(doWindowGroupList(aScreenNumber,aPriority,aWindowList,EWsClOpNumWindowGroupsOnScreen,EWsClOpWindowGroupList));	
	}

EXPORT_C TInt RWsSession::WindowGroupList(RArray<TWindowGroupChainInfo>* aWindowList) const
/** Gets a list of identifier of window group and parent identifier of window group of
 all window groups in all window server sessions.

An array buffer must be created to store the resultant list.

This function always causes a flush of the window server buffer.

@param aWindowList List of identifiers of all window groups in the server.
@return KErrNone if successful otherwise KErrNoMemory if there is insufficient memory to create the array,
Panics if aWindowList is NULL. */
	{
	return(doWindowGroupList(0,aWindowList,EWsClOpNumWindowGroupsAllPriorities,EWsClOpWindowGroupListAndChainAllPriorities));
	}

EXPORT_C TInt RWsSession::WindowGroupList(TInt aPriority,CArrayFixFlat<TInt>* aWindowList) const
/** Lists the number of window groups of a given window group priority running 
in all window server sessions.

This function is the same as WindowGroupList() described above, but allows 
the application to restrict the list of window groups to those of a particular 
window group priority.

This function always causes a flush of the window server buffer.

@param aPriority Window group priority 
@param aWindowList List of identifiers of all window groups in the server of priority 
aPriority.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	return(doWindowGroupList(KDummyScreenNumber,aPriority,aWindowList,EWsClOpNumWindowGroups,EWsClOpWindowGroupList));
	}

EXPORT_C TInt RWsSession::WindowGroupList(TInt aPriority, RArray<TWindowGroupChainInfo>* aWindowList) const
/** Lists the number of window groups of a given window group priority running 
in all window server sessions.

This function is the same as WindowGroupList() described above, but allows 
the application to restrict the list of window groups to those of a particular 
window group priority.

This function always causes a flush of the window server buffer.

@param aPriority Window group priority 
@param aWindowList List of identifiers of all window groups in the server of priority 
aPriority.
@return KErrNone if successful otherwise KErrNoMemory if there is insufficient memory to create the array, 
Panics if aWindowList is NULL. */	
	{
	return(doWindowGroupList(aPriority,aWindowList,EWsClOpNumWindowGroups,EWsClOpWindowGroupListAndChain));
	}

EXPORT_C TInt RWsSession::GetDefaultOwningWindow() const
/** Gets the identifier of the current default owning window group.

This function always causes a flush of the window server buffer.

@return Identifier of current default owning window group. Returns 0 if there 
isn't one. */
	{
	return GetDefaultOwningWindow(KDummyScreenNumber);
	}

EXPORT_C TInt RWsSession::GetDefaultOwningWindow(TInt aScreenNumber) const
/** Gets the identifier of the current default owning window group on a specified screen.

This function always causes a flush of the window server buffer.

@param aScreenNumber specifies the screen.
@return Identifier of current default owning window group on the specified screen. Returns 0 if there 
isn't one. */
	{
	return(WriteReplyInt(aScreenNumber,EWsClOpGetDefaultOwningWindow));	
	}

EXPORT_C TInt RWsSession::GetFocusWindowGroup() const
/** Gets the identifier of the window group that currently has the keyboard focus.

Note: this might not necessarily be the front-most window group, as window groups 
can disable keyboard focus.

This function always causes a flush of the window server buffer.

@return Identifier of window group with keyboard focus. */
	{
	return GetFocusWindowGroup(KDummyScreenNumber);
	}

EXPORT_C TInt RWsSession::GetFocusWindowGroup(TInt aScreenNumber) const
/** Gets the identifier of the window group on a specified screen that currently has the keyboard focus.

Note: this might not necessarily be the front-most window group, as window groups 
can disable keyboard focus.

This function always causes a flush of the window server buffer.

@param aScreenNumber specifies the screen.
@return Identifier of window group with keyboard focus. */
	{
	return WriteReplyInt(aScreenNumber,EWsClOpGetFocusWindowGroup);	
	}

EXPORT_C TInt RWsSession::SetWindowGroupOrdinalPosition(TInt aIdentifier, TInt aPosition)
/** Sets the ordinal position of a window group.

This function allows the caller to change the ordinal position of an existing 
window group. It would typically be used by a shell application.

This function always causes a flush of the window server buffer.

@param aIdentifier The window group. 
@param aPosition Ordinal position for the window group. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TWsClCmdSetWindowGroupOrdinalPosition params(aIdentifier,aPosition);
	return(WriteReply(&params, sizeof(params),EWsClOpSetWindowGroupOrdinalPosition));
	}

EXPORT_C TInt RWsSession::GetWindowGroupClientThreadId(TInt aIdentifier, TThreadId &aThreadId) const
/** Gets the thread ID of the client that owns the window group specified by the 
window group identifier.

This function always causes a flush of the window server buffer.

@param aIdentifier The window group identifier. 
@param aThreadId On return, contains the thread ID. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TPtr8 ptr(reinterpret_cast<TUint8*>(&aThreadId),sizeof(aThreadId));
	return(WriteReplyIntP(aIdentifier,&ptr,EWsClOpGetWindowGroupClientThreadId));
	}

EXPORT_C TInt RWsSession::GetWindowGroupHandle(TInt aIdentifier) const
/** Gets the handle of the window specified by the window group identifier. 

This is the handle that was passed as an argument to RWindowGroup::Construct().

This function always causes a flush of the window server buffer.

@param aIdentifier The window group identifier.
@return Handle of the window group */
	{
	return(WriteReplyInt(aIdentifier,EWsClOpGetWindowGroupHandle));
	}

EXPORT_C TInt RWsSession::GetWindowGroupOrdinalPriority(TInt aIdentifier) const
/** Gets a window group's priority.

This function always causes a flush of the window server buffer.

@param aIdentifier The window group identifier. 
@return The window group priority. */
	{
	return(WriteReplyInt(aIdentifier,EWsClOpGetWindowGroupOrdinalPriority));
	}

EXPORT_C TInt RWsSession::SendEventToWindowGroup(TInt aIdentifier, const TWsEvent &aEvent)
/** Sends an event to a window group.

This function always causes a flush of the window server buffer.

@param aIdentifier Window group identifier.
@param aEvent Event to send to the window group.
@return KErrNone if successful, KErrPermissionDenied if the client does not have 
the required capability, KErrNoMemory if there is not enough room to add the 
event to the event queue and otherwise another of the system-wide error codes.
@capability SwEvent Required when aEvent.Type() < EEventUser unless the event 
is for a window group owned by this session.

@deprecated */
	{
	TWsClCmdSendEventToWindowGroup params(aIdentifier,aEvent);
	return(WriteReply(&params, sizeof(params),EWsClOpSendEventToWindowGroup));
	}

EXPORT_C TInt RWsSession::SendEventToAllWindowGroups(const TWsEvent &aEvent)
/** Sends the specified event to all existing window groups.

This function always causes a flush of the window server buffer.

@param aEvent The event to be sent to all window groups. 
@return KErrNone if successful, KErrPermissionDenied if the client does not have 
the required capability, KErrNoMemory if there is not enough room to add the 
event to all the required event queues (although it may have been added to some 
of them) and otherwise another of the system-wide error codes.
@capability SwEvent Required when aEvent.Type() < EEventUser

@deprecated */
	{
	TWsClCmdSendEventToWindowGroup params(0,aEvent);
	return(WriteReply(&params, sizeof(params),EWsClOpSendEventToAllWindowGroup));
	}

EXPORT_C TInt RWsSession::SendEventToAllWindowGroups(TInt aPriority, const TWsEvent &aEvent)
/** Sends the specified event to all window groups with the specified priority.

This function always causes a flush of the window server buffer.

@param aPriority The priority which window groups must have to be sent the 
message.
@param aEvent The event to be sent to the specified window groups.
@return KErrNone if successful, KErrPermissionDenied if the client does not have 
the required capability, KErrNoMemory if there is not enough room to add the 
event to all the required event queues (although it may have been added to some 
of them) and otherwise another of the system-wide error codes.
@capability SwEvent Required when aEvent.Type() < EEventUser

@deprecated  */
	{
	TWsClCmdSendEventToWindowGroup params(aPriority,aEvent);
	return(WriteReply(&params, sizeof(params),EWsClOpSendEventToAllWindowGroupPriority));
	}

EXPORT_C TInt RWsSession::SendEventToOneWindowGroupsPerClient(const TWsEvent &aEvent)
/** This function always causes a flush of the window server buffer.
@capability SwEvent Required when aEvent.Type() < EEventUser 

@deprecated  */
	{
	TWsClCmdSendEventToWindowGroup params(0,aEvent);
	return(WriteReply(&params, sizeof(params),EWsClOpSendEventToOneWindowGroupPerClient));
	}

EXPORT_C TInt RWsSession::GetWindowGroupNameFromIdentifier(TInt aIdentifier, TDes &aWindowName) const
/** Gets the name of a window group from its identifier. 

Using the list of identifiers returned by WindowGroupList(), it is possible 
to get the names of all window groups in the system. Note that window names 
are a zero length string by default.

Note that the window group name must have been previously set using RWindowGroup::SetName() 
to contain a meaningful value.

This function always causes a flush of the window server buffer.

@param aIdentifier The identifier of the window group whose name is to be 
inquired.
@param aWindowName On return, contains the window group name.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TWsClCmdGetWindowGroupNameFromIdentifier params(aIdentifier,aWindowName.MaxLength());
	return(WriteReplyP(&params,sizeof(params),&aWindowName,EWsClOpGetWindowGroupNameFromIdentifier));
	}

EXPORT_C TInt RWsSession::FindWindowGroupIdentifier(TInt aPreviousIdentifier,const TDesC& aMatch,TInt aOffset) const
/** Gets all window groups whose names match a given string, which can contain 
wildcards.

An example use of this function might be to find all the currently 
running instances of a particular application, assuming that the window group 
name contains the application name. An optional argument, aOffset, specifies 
the number of characters to be ignored at the beginning of the window group 
name. As several window group names may match the given string, and the function 
can return only one at a time, there is an argument, aPreviousIdentifier, 
which gives the identifier for the previous match that was returned. In other 
words, it means, "get me the next match after this one." The first time the 
function is called, give 0 as the previous identifier.

Matching is done using TDesC::MatchF(), which does a folded match. Wildcards 
'*' and '?' can be used to denote one or more characters and exactly one character, 
respectively. Windows are searched in front to back order.

This function always causes a flush of the window server buffer.

@param aPreviousIdentifier Identifier of the last window group returned. If 
the value passed is not a valid identifier, the function returns KErrNotFound. 
@param aMatch String to match window group name against: may contain wildcards 
@param aOffset The first aOffset characters of the window group name are ignored 
when doing the match.
@return The next window group, after the one identified by aPreviousIdentifier, 
whose name matches aMatch. KErrNotFound if no window group matched or aPreviousIdentifier 
was not a valid identifier. */
	{
	TWsClCmdFindWindowGroupIdentifier params(aPreviousIdentifier,aOffset, aMatch.Length());
	return(WriteReply(&params,sizeof(params),aMatch.Ptr(),aMatch.Size(),EWsClOpFindWindowGroupIdentifier));
	}

EXPORT_C TInt RWsSession::FindWindowGroupIdentifier(TInt aPreviousIdentifier,TThreadId aThreadId) const
/** Gets the identifiers of window groups belonging to a client which is owned 
by a thread with the specified thread ID.

The thread may own more than one window group, so the identifier returned 
is the one after aPreviousIdentifier. The first time the function is called, 
use 0 for the previous identifier.

This function always causes a flush of the window server buffer.

@param aPreviousIdentifier Identifier returned by previous call 
@param aThreadId Thread owning the window group or groups. 
@return Next window group identifier after the one identified by aPreviousIdentifier */
	{
	TWsClCmdFindWindowGroupIdentifierThread params(aPreviousIdentifier, aThreadId);
	return(WriteReply(&params,sizeof(params),EWsClOpFindWindowGroupIdentifierThread));
	}

EXPORT_C TInt RWsSession::SendMessageToWindowGroup(TInt aIdentifier, TUid aUid, const TDesC8 &aParams)
/** Sends a message to a window group. 

The window group will then receive an event of type EEventMessageReady notifying 
it that a message has been received. The window group can belong to this or 
another session.

In order to receive messages sent using this function you will need to implement 
the MCoeMessageObserver interface which is defined in the UI Control Framework API.

This function always causes a flush of the window server buffer.

@param aIdentifier The identifier of the window group to receive the message. 
@param aUid A UID which uniquely identifies the session sending the message. 
@param aParams The message data. An unlimited amount of data can be passed 
in this argument. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see MCoeMessageObserver 

@deprecated  */
	{
	TWsClCmdSendMessageToWindowGroup params(aIdentifier, aUid, aParams.Length(),&aParams);
	return(WriteReplyByProvidingRemoteReadAccess(&params,sizeof(params),&aParams,EWsClOpSendMessageToWindowGroup));
	}

EXPORT_C TInt RWsSession::SendMessageToAllWindowGroups(TUid aUid, const TDesC8& aParams)
/** Sends a message to all window groups.

In order to receive messages sent using this function you will need to implement 
the MCoeMessageObserver interface which is defined in the UI Control Framework 
API.

This function always causes a flush of the window server buffer.

@param aUid A UID which uniquely identifies the session sending the message. 
@param aParams The message data. An unlimited amount of data can be passed 
in this argument. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 

@deprecated  */
	{
	TWsClCmdSendMessageToWindowGroup params(0, aUid, aParams.Length(),&aParams);
	return(WriteReplyByProvidingRemoteReadAccess(&params,sizeof(params),&aParams,EWsClOpSendMessageToAllWindowGroups));
	}

EXPORT_C TInt RWsSession::SendMessageToAllWindowGroups(TInt aPriority, TUid aUid, const TDesC8& aParams)
/** Sends a message to all window groups with the specified priority.

In order to receive messages sent using this function you will need to implement 
the MCoeMessageObserver interface which is defined in the UI Control Framework 
API.

This function always causes a flush of the window server buffer.

@param aPriority The priority which window groups must have to be sent the 
message. 
@param aUid A UID which uniquely identifies the session sending the message. 
@param aParams The message data. An unlimited amount of data can be passed 
in this argument. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 

@deprecated  */
	{
	TWsClCmdSendMessageToWindowGroup params(aPriority, aUid, aParams.Length(),&aParams);
	return(WriteReplyByProvidingRemoteReadAccess(&params,sizeof(params),&aParams,EWsClOpSendMessageToAllWindowGroupsPriority));
	}


/** Called by the client in response to an EEventMessageReady event to receive the message data.

This function always causes a flush of the window server buffer.

@param aUid A UID which uniquely identifies the session sending the message.
@param aParams The message data. An unlimited amount of data can be passed 
in this argument.
@param aMessageEvent The event sent from the server to the client to identify the message.
@return KErrNone if successful, KErrNoMemory if there is insufficient memory for
the message data, otherwise another of the system-wide error codes.
*/
EXPORT_C TInt RWsSession::FetchMessage(TUid& aUid, TPtr8& aParams, const TWsEvent& aMessageEvent) const
	{
	const SEventMessageReady& eventMessageReady=*(SEventMessageReady*)aMessageEvent.EventData();
	TUint8* ptr=(TUint8*)User::Alloc(eventMessageReady.iMessageParametersSize);
	if (ptr==NULL)
		{
		aParams.Set(NULL, 0, 0);
		return KErrNoMemory;
		}
	aUid=eventMessageReady.iMessageUid;
	aParams.Set(ptr, eventMessageReady.iMessageParametersSize, eventMessageReady.iMessageParametersSize);
	TWsClCmdFetchMessage outGoingParams(eventMessageReady.iWindowGroupIdentifier);
	const TInt error=WriteReplyP(&outGoingParams, sizeof(outGoingParams), &aParams, EWsClOpFetchMessage); // must be called even if eventMessageReady.iMessageParametersSize==0
	if (error!=KErrNone)
		{
		delete ptr;
		aParams.Set(NULL, 0, 0);
		}
	return error;
	}

EXPORT_C TInt RWsSession::SetKeyboardRepeatRate(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime)
/** Sets the system-wide keyboard repeat rate. 

This is the rate at which keyboard events are generated when a key is held 
down.

The default settings for the keyboard repeat rate are 0.3 seconds for the 
initial delay, and 0.1 seconds for the interval between subsequent repeats. 
However, since the settings are system-wide, these will not necessarily be 
the current settings when an application is launched: the settings may have 
been over-ridden by another module.

This function always causes a flush of the window server buffer.

@param aInitialTime Time before first repeat key event 
@param aTime Time between subsequent repeat key events 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see GetKeyboardRepeatRate()
@capability WriteDeviceData */
	{
	TWsClCmdSetKeyboardRepeatRate repeat(aInitialTime,aTime);
	return(WriteReply(&repeat,sizeof(repeat),EWsClOpSetKeyboardRepeatRate));
	}

EXPORT_C void RWsSession::GetKeyboardRepeatRate(TTimeIntervalMicroSeconds32 &aInitialTime, TTimeIntervalMicroSeconds32 &aTime) const
/** Gets the current system-wide settings for the keyboard repeat rate.

This function always causes a flush of the window server buffer.

@param aInitialTime Time before first repeat key event 
@param aTime Time between subsequent repeat key events 
@see SetKeyboardRepeatRate() */
	{
	TPckgBuf<SKeyRepeatSettings> settings;
	WriteReplyP(&settings,EWsClOpGetKeyboardRepeatRate);
	aInitialTime=settings().iInitialTime;
	aTime=settings().iTime;
	}

EXPORT_C TInt RWsSession::SetDoubleClick(const TTimeIntervalMicroSeconds32 &aInterval, TInt aDistance)
/** Sets the system-wide double click settings.

Double click distance is measured, in pixels, as the sum of the X distance 
moved and the Y distance moved between clicks. For example: a first click 
at 10, 20 and a second click at 13,19 gives a distance of (13-10)+(21-20) = 4.

This function always causes a flush of the window server buffer.

@param aInterval Maximum interval between clicks that constitutes a double 
click 
@param aDistance Maximum distance between clicks that constitutes a double 
click
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see GetDoubleClickSettings()
@capability WriteDeviceData */
	{
	TWsClCmdSetDoubleClick dblClick(aInterval,aDistance);
	return(WriteReply(&dblClick,sizeof(dblClick),EWsClOpSetDoubleClick));
	}

EXPORT_C void RWsSession::GetDoubleClickSettings(TTimeIntervalMicroSeconds32 &aInterval, TInt &aDistance) const
/** Gets the current system-wide settings for pointer double clicks.

Double click distances are measured in pixels as the sum of the X distance 
moved and the Y distance moved between clicks. For example: a first click 
at 10, 20 and a second click at 13,19 gives a distance of (13-10)+(21-20) = 4.

This function always causes a flush of the window server buffer.

@param aInterval Maximum interval between clicks that constitutes a double 
click 
@param aDistance Maximum distance between clicks that constitutes a double 
click 
@see SetDoubleClick() */
	{
	TPckgBuf<SDoubleClickSettings> settings;
	WriteReplyP(&settings,EWsClOpGetDoubleClickSettings);
	aInterval=settings().iInterval;
	aDistance=settings().iDistance;
	}

EXPORT_C void RWsSession::SetBackgroundColor(TRgb aColor)
/** Sets the background colour for the window server. 

This background can only be seen in areas of the display that have no windows 
on them: so for many applications it will never be seen. It affects no 
other windows.

@param aColor Background colour */
	{
	WriteInt(aColor.Internal(),EWsClOpSetBackgroundColor);
	}

EXPORT_C TRgb RWsSession::GetBackgroundColor() const
/** Gets the window server's background colour.

This function always causes a flush of the window server buffer.

@return Background colour */
	{
	TRgb col;
	col.SetInternal((TUint32)WriteReply(EWsClOpGetBackgroundColor));
	return col;
	}

EXPORT_C TInt RWsSession::RegisterSurface(TInt aScreenNumber, const TSurfaceId& aSurface)
/**
This function registers a surface for use in composition on the screen associated 
with this device within this session.

A surface may be registered as a separate step before it is added as a background surface 
for a window created in the same session and that is displayed on this screen. This then 
allows content to be provisioned before the surface is displayed for the first time, and 
to be kept "live" while not assigned to any window.

A surface can be successfully registered in multiple sessions and on multiple screens, but 
only once for a given combination of session and screen.

The client should call UnregisterSurface when finished with the surface registered using 
this method. The surface will be automatically unregistered if necessary when the session closes.

@param aScreenNumber The screen to which to register.
@param aSurface The surface to be registered.
@return KErrNone on success or a system-wide error code
	- KErrNoMemory if registration fails due to low memory.
	- KErrInUse if the surface ID is already registered for this session and screen.
@pre aSurface has been initialized and is compatible with being composited on this device's 
screen; otherwise the client thread is panicked with code EWservPanicIncompatibleSurface.
@pre aScreenNumber is an acceptable screen number. Otherwise the client thread is panicked 
with code EWservPanicScreenNumber.
@post The surface has been successfully registered for use in composition on this device's screen.
@post The surface’s content is in an undefined state, but the surface remains initialized.
@post This function always causes a flush of the WServ session buffer.
@see UnregisterSurface

@publishedPartner
@prototype
*/
	{
	TWsClCmdSurfaceRegister surfaceRegister(aScreenNumber,aSurface);
	return(WriteReply(&surfaceRegister,sizeof(surfaceRegister),EWsClOpRegisterSurface));
	}

EXPORT_C void RWsSession::UnregisterSurface(TInt aScreenNumber, const TSurfaceId& aSurface)
/**
This function removes the surface from the session's register of surfaces that are used in 
composition on the screen associated with this device.

Calling this function with a surface that is not currently explicitly registered on this screen 
in this session by RegisterSurface() will have no effect.

Calling this function while the surface is still assigned to a window will have no immediate 
effect. However, when the surface is unassigned from the window, and is not held by another 
session it will then be automatically unregistered.

An unregistered surface can be re-registered again, if necessary.

This function does not explicitly force a flush of the WServ session buffer. Internal reference 
counting will keep the Surface ID "live" until the client code has released any handles and 
provisioners, and WServ processes the buffered remove command, and the final frame containing 
this surface has finished being displayed.

@param aScreenNumber The screen to which to unregister.
@param aSurface The surface to be unregistered.
@pre aSurface has been initialized; otherwise the client thread is panicked with 
code EWservPanicIncompatibleSurface.
@pre aScreenNumber is an acceptable screen number. Otherwise the client thread is 
panicked with code EWservPanicScreenNumber.
@post If no window is using the surface, then it is unregistered on this screen in this session.
@see RegisterSurface

@publishedPartner
@prototype
*/
	{
	TWsClCmdSurfaceRegister surfaceRegister(aScreenNumber,aSurface);
	Write(&surfaceRegister,sizeof(surfaceRegister),EWsClOpUnregisterSurface);
	}

EXPORT_C TInt RWsSession::PreferredSurfaceConfigurationSize() const
/**
Returns the window server's preferred size for the TSurfaceConfiguration object, used 
for RWindow::SetBackgroundSurface.

Client code is permitted to present any defined version of the TSurfaceConfiguration 
class, distinguished by its size. If smaller, earlier versions are presented, the server 
will substitute the stated default values. If later, larger, structures are presented to 
the server then the additional data will be ignored.

However, by using this method, the client can fine-tune its use of the interface, avoiding 
generating attribute data that may not be supported, or reduce the options presented to users.

@return The preferred size in bytes
	- Should match one of the TSurfaceConfiguration classes defined in the client's header 
	file, or be larger than them all, indicating a version newer that the client is compiled for.
	- If the server does not support surface configuration 
		- Return KErrNotSupported.

@publishedPartner
@prototype
*/
	{
	return sizeof(TSurfaceConfiguration);
	}

EXPORT_C TInt RWsSession::SetSystemPointerCursor(const RWsPointerCursor &aPointerCursor,TInt aCursorNumber)
/** Sets a cursor in the system pointer cursor list. 

To gain access to the list, the client must first call ClaimSystemPointerCursorList().

This function always causes a flush of the window server buffer.

@param aPointerCursor Pointer cursor to set in the list 
@param aCursorNumber Cursor number in the list 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	TWsClCmdSetSystemPointerCursor setpc;
	setpc.handle=aPointerCursor.WsHandle();
	setpc.number=aCursorNumber;
	return(WriteReply(&setpc,sizeof(setpc),EWsClOpSetSystemPointerCursor));
	}

EXPORT_C void RWsSession::ClearSystemPointerCursor(TInt aCursorNumber)
/** Clears a system pointer cursor from the list. 

Before calling this function, the client must first gain access to the list 
by calling ClaimSystemPointerCursorList().

@param aCursorNumber Cursor number to clear */
	{
	WriteInt(aCursorNumber,EWsClOpClearSystemPointerCursor);
	}

EXPORT_C TInt RWsSession::ClaimSystemPointerCursorList()
/** Claims the system pointer cursor list.

You must call this function before you can call SetSystemPointerCursor() or 
ClearSystemPointerCursor().

This function always causes a flush of the window server buffer.

@return KErrNone if successful, KErrInUse if another client is already using 
the system pointer cursor list, otherwise another of the system-wide error 
codes. 
@see FreeSystemPointerCursorList()
@capability WriteDeviceData */
	{
	return(WriteReply(EWsClOpClaimSystemPointerCursorList));
	}

EXPORT_C void RWsSession::FreeSystemPointerCursorList()
/** Releases the system pointer cursor list and deletes all the entries in it. 

A client should call this function when it no longer needs the system pointer 
cursor list. */
	{
	if (iWsHandle)
		Write(EWsClOpFreeSystemPointerCursorList);
	}

EXPORT_C TInt RWsSession::SetCustomTextCursor(TInt aIdentifier, const TArray<TSpriteMember>& aSpriteMemberArray, TUint aSpriteFlags, TCustomTextCursorAlignment aAlignment)
/** Adds a custom text cursor to the server's list of cursors.

After adding a custom text cursor, it can be selected for use by calling 
RWindowGroup::SetTextCursor().

Note that once added, custom text cursors cannot be removed.

This function always causes a flush of the window server buffer.

@param aIdentifier The unique identifier for the cursor.
@param aSpriteMemberArray An array defining the bitmap(s) that make up the 
cursor. Typically, this array will contain a single element for a static, 
non-animated cursor.
@param aSpriteFlags Flags affecting the sprite behaviour. For possible values 
see TSpriteFlags.
@param aAlignment The vertical alignment of the cursor sprite.
@return KErrNone if successful, KErrAlreadyExists if a custom cursor with the 
specified identifier already exists, or another of the standard system wide 
error codes. */
	{
	// Create the cursor
	TWsClCmdCustomTextCursorData params;
	params.identifier = aIdentifier;
	params.flags = aSpriteFlags;
	params.alignment = aAlignment;
	const TInt handle = iBuffer->WriteReplyWs(&params, sizeof(params), EWsClOpStartSetCustomTextCursor);
	if (handle < 0)
		{
		return handle; // Failed to create
		}

	RWsCustomTextCursor cursor(*this, handle);
	TInt err = KErrNone;
	for (TInt ii = 0; ii < aSpriteMemberArray.Count(); ii++)
		{
		err = cursor.AppendMember(aSpriteMemberArray[ii]);
		if (err != KErrNone)
			{
			break;
			}
		}
	cursor.Close();
	err = iBuffer->WriteReplyWs(&err, sizeof(err), EWsClOpCompleteSetCustomTextCursor);
	return err;
	}

EXPORT_C TInt RWsSession::SetModifierState(TEventModifier aModifier,TModifierState aState)
/** Sets the state of the modifier keys. 

This function is typically used for permanent modifier states such as Caps 
Lock or Num Lock, but other possible uses include on-screen function key simulation, 
or the implementation of a Shift Lock key.

This function always causes a flush of the window server buffer.

@param aModifier Modifier to set. 
@param aState Modifier state.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see GetModifierState()
@capability WriteDeviceData */
	{
	TWsClCmdSetModifierState params;
	params.modifier=aModifier;
	params.state=aState;
	return(WriteReply(&params,sizeof(params),EWsClOpSetModifierState));
	}

EXPORT_C TInt RWsSession::GetModifierState() const
/** Gets the state of the modifier keys.

The state of each modifier key (defined in TEventModifier) is returned in 
a bitmask.

This function always causes a flush of the window server buffer.

@return Modifier state 
@see TEventModifier */
	{
	return(WriteReply(EWsClOpGetModifierState));
	}

EXPORT_C TInt RWsSession::HeapCount() const
/** Gets the heap count.

This function calls RHeap::Count() on the window server's heap, after throwing 
away all the temporary objects allocated for each window.

This function always causes a flush of the window server buffer.

@return The window server's heap count. */
	{
	return(WriteReply(EWsClOpHeapCount));
	}

EXPORT_C TInt RWsSession::DebugInfo(TInt aFunction, TInt aParam) const
	{
	TWsClCmdDebugInfo params(aFunction,aParam);
	return(WriteReply(&params,sizeof(params),EWsClOpDebugInfo));
	}

EXPORT_C TInt RWsSession::DebugInfo(TInt aFunction, TDes8& aReturnBuf, TInt aParam) const
	{
	TWsClCmdDebugInfo params(aFunction,aParam);
	return(WriteReplyP(&params,sizeof(params),TWriteDescriptorType(&aReturnBuf),EWsClOpDebugInfoReplyBuf));
	}

EXPORT_C TInt RWsSession::ResourceCount() const
/** Gets the number of objects that the server has allocated for that client.

This function can be used to check that the client has correctly cleaned up 
all of its objects.

It always causes a flush of the window server buffer.

@return The number of resources allocated to the client. */
	{
	return(iWsHandle ? WriteReply(EWsClOpResourceCount) : 0);
	}

EXPORT_C void RWsSession::PasswordEntered()
/** Disables the window server password mode.

This function must be called by the session which owns the password window 
when the correct machine password has been entered. 

@deprecated */
	{
	Write(EWsClOpPasswordEntered);
	}

EXPORT_C void RWsSession::HeapSetFail(TInt aTAllocFail,TInt aValue)
/** Sets the heap failure mode in the window server.

The release version of the base does not support simulated heap failure functionality, 
and the result of this function is additional error messages. In the debug 
version the clients are notified of the simulated failure and handle it. See 
RHeap::__DbgSetAllocFail() for more information.

Note:

It is unlikely, but possible to create a ROM with a mixture of Release and 
Debug versions of the Base and Window Server DLLs, which results in different 
behaviour to that described above. If you run a debug Window Server with a 
release version of the Base, then calling this function will result in neither 
extra error messages (e.g. EDrawingRegion) nor simulated heap failures. However 
if you have a release Window Server with a debug Base then you will get both 
simulated heap failures and the extra error messages.

This function always causes a flush of the window server buffer.

@param aTAllocFail A value from the RHeap::TAllocFail enumeration which 
indicates how to simulate heap allocation failure. 
@param aValue The rate of failure; when aType is RHeap::EDeterministic, heap 
allocation fails every aRate attempt 
@see RHeap::__DbgSetAllocFail()
@capability WriteDeviceData */
	{
	TWsClCmdHeapSetFail params;
	params.type=(RHeap::TAllocFail)aTAllocFail;
	params.value=aValue;
	params.burst=-1;
	WriteReply(&params,sizeof(params),EWsClOpHeapSetFail);
	}

EXPORT_C void RWsSession::HeapSetBurstFail(TInt aTAllocFail,TInt aRate,TInt aBurst)
/** Sets the heap failure burst mode in the window server.

The release version of the base does not support simulated heap failure functionality, 
and the result of this function is additional error messages. In the debug 
version the clients are notified of the simulated failure and handle it. See 
RHeap::__DbgSetBurstAllocFail() for more information.

Note:

It is unlikely, but possible to create a ROM with a mixture of Release and 
Debug versions of the Base and Window Server DLLs, which results in different 
behaviour to that described above. If you run a debug Window Server with a 
release version of the Base, then calling this function will result in neither 
extra error messages (e.g. EDrawingRegion) nor simulated heap failures. However 
if you have a release Window Server with a debug Base then you will get both 
simulated heap failures and the extra error messages.

This function always causes a flush of the window server buffer.

@param aTAllocFail A value from the RHeap::TAllocFail enumeration which 
indicates how to simulate heap allocation failure. 
@param aRate The rate of failure; when aType is RHeap::EDeterministic, heap 
allocation fails every aRate attempt.
@param aBurst The number of consecutive allocations that should fail.
@see RHeap::__DbgSetBurstAllocFail()
@capability WriteDeviceData */
	{
	TWsClCmdHeapSetFail params;
	params.type=(RHeap::TAllocFail)aTAllocFail;
	params.value=aRate;
	params.burst=aBurst;
	WriteReply(&params,sizeof(params),EWsClOpHeapSetFail);
	}

EXPORT_C TInt RWsSession::RequestOffEvents(TBool aOn,RWindowTreeNode *aWin/*=NULL*/)
/** Requests the window server to send OFF events to a window. 

After calling this function, the window server sends OFF events to the window 
when an event occurs which requires power down, rather than handling powering 
down itself.

Notes:

Any client can ask for OFF events, but only one window in the system can be 
set to receive them. If this function is called when another window is set 
to receive OFF events then the client will be panicked. The exception is the 
shell, which is allowed to take receipt of OFF events from other clients.

The window server identifies the shell client by comparing the process name 
of the client with the process name of the shell. Only the first client created 
by the shell is guaranteed to have the extra shell client privileges.

If the shell dies or terminates just before the action requiring power down 
happens then the window server will handle it rather than passing it on to 
the shell.

The window server has a queue of messages that it is waiting to send to clients. 
If the shell's client's queue is full and the window server cannot make room 
for the OFF message then it will power down the machine itself.

This function always causes a flush of the window server buffer.

@param aOn ETrue to get the window server to send EEventShellSwitchOff messages 
to the shell (rather than powering down). EFalse makes the window server switch 
back to powering down the machine itself. 
@param aWin The handle to the window or window group of the shell to which 
the message is to be sent. This may be NULL only if aOn=EFalse, in other words, 
the window server is handling power down itself. If aOn=ETrue then this must not 
be NULL, or the Window Server will panic the shell. Note that as far as the window 
server is concerned the handle must exist when this function is called.
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@panic WSERV 51 aOn is true, but no window was specified (aWin is NULL). 
@capability PowerMgmt */
	{
	TWsClCmdOffEventsToShell OffEventsToShell(aOn,(aWin ? aWin->WsHandle():0));
	return WriteReply(&OffEventsToShell,sizeof(OffEventsToShell),EWsClOpSendOffEventsToShell);
	}

EXPORT_C TDisplayMode RWsSession::GetDefModeMaxNumColors(TInt& aColor,TInt& aGray) const
/** Gets the number of colours available in the richest supported colour mode, the 
number of greys available in the richest grey mode, and returns the default 
display mode.

This function always causes a flush of the window server buffer.

@param aColor The number of colours in the richest supported colour mode. 
@param aGray The number of greys in the richest supported grey mode. 
@return The default display mode. */
	{
	return GetDefModeMaxNumColors(KDummyScreenNumber,aColor,aGray);
	}

EXPORT_C TDisplayMode RWsSession::GetDefModeMaxNumColors(TInt aScreenNumber,TInt& aColor,TInt& aGray) const
/** Gets the number of colours available in the richest supported colour mode, the 
number of greys available in the richest grey mode, and returns the default 
display mode, on the specified screen.

This function always causes a flush of the window server buffer.

@param aScreenNumber specifies the screen.
@param aColor The number of colours in the richest supported colour mode. 
@param aGray The number of greys in the richest supported grey mode. 
@return The default display mode. */
	{
	TPckgBuf<SDefModeMaxNumColors> colors;	
	WriteReplyIntP(aScreenNumber,&colors,EWsClOpGetDefModeMaxNumColors);
	aColor=colors().iColors;
	aGray=colors().iGrays;
	return colors().iDisplayMode;
	}

#define MODE_TO_FLAG(x) 1<<(x-1)

LOCAL_C void HandleColorMode(CArrayFix<TInt>* aModeList,TUint aModeBits, TInt& aNumberOfModes, TInt aMode)
	{
	if (aModeBits&(MODE_TO_FLAG(aMode)))
		{
		if (aModeList!=NULL)
			{
			(*aModeList)[aNumberOfModes]=aMode;
			}
		++aNumberOfModes;
		}
	}

LOCAL_C TInt DoGetColorModeList(CArrayFix<TInt>* aModeList,TUint aModeBits)
	{
	TInt numberOfModes=0;
	for (TInt mode=EGray2; mode<=EColor256; ++mode)
		{
		HandleColorMode(aModeList,aModeBits,numberOfModes,mode);
		}
	HandleColorMode(aModeList,aModeBits,numberOfModes,EColor4K);
	HandleColorMode(aModeList,aModeBits,numberOfModes,EColor64K);
	HandleColorMode(aModeList,aModeBits,numberOfModes,EColor16M);
	HandleColorMode(aModeList,aModeBits,numberOfModes,EColor16MU);
	HandleColorMode(aModeList,aModeBits,numberOfModes,EColor16MA);
	HandleColorMode(aModeList,aModeBits,numberOfModes,EColor16MAP);
	return numberOfModes;
	}

EXPORT_C TInt RWsSession::GetColorModeList(CArrayFixFlat<TInt> *aModeList) const
/** Gets the list of available colour modes.

Note that this function should usually be called within User::LeaveIfError(). 
The only time that an error can be generated is when the array gets resized 
to the number of display modes. Thus if you make the size of your array equal 
to the maximum number of display modes over all hardware (this is the number 
of different values in TDisplayMode) then this function will never leave.

This function always causes a flush of the window server buffer.

@param aModeList On return, contains a TDisplayMode entry for each of the 
available display modes. Note that the array's contents are deleted prior 
to filling with display mode information. 
@return The number of color modes if the parameter is passed NULL. Otherwise KErrNone or KErrNoMemory. */
	{
	return GetColorModeList(KDummyScreenNumber,aModeList);
	}

EXPORT_C TInt RWsSession::GetColorModeList(TInt aScreenNumber,CArrayFixFlat<TInt>* aModeList) const
/** Gets the list of available colour modes on a particular screen.

Note that this function should usually be called within User::LeaveIfError(). 
The only time that an error can be generated is when the array gets resized 
to the number of display modes. Thus if you make the size of your array equal 
to the maximum number of display modes over all hardware (this is the number 
of different values in TDisplayMode) then this function will never leave.

This function always causes a flush of the window server buffer.

@param aModeList On return, contains a TDisplayMode entry for each of the 
available display modes. Note that the array's contents are deleted prior 
to filling with display mode information.
@param aScreenNumber specifies the screen.  
@return The number of color modes if the parameter is passed NULL. Otherwise KErrNone or KErrNoMemory. */
	{
	const TUint modeList=STATIC_CAST(TUint,WriteReplyInt(aScreenNumber,EWsClOpGetColorModeList));	
	const TInt numberOfModes=DoGetColorModeList(NULL, modeList);
	if (aModeList==NULL)
		{
		return numberOfModes;
		}
	TRAPD(error,aModeList->ResizeL(numberOfModes));
	if (error!=KErrNone)
		{
		return error;
		}
	DoGetColorModeList(aModeList, modeList);
	return KErrNone;
	}

EXPORT_C void RWsSession::SetPointerCursorArea(const TRect& aArea)
/** 
@publishedPartner
@released

Sets the area of the screen in which the virtual cursor can be used while in 
relative mouse mode, for the first screen display mode. 

This function sets the area for the first screen mode - the one with index 
0, which in most devices will be the only screen mode. The other function 
overload can be used to set the screen area for other modes. The area is set 
and stored independently on each screen mode, so that it is not necessary 
to call this function again when switching back to the first screen mode.

The default area is the full digitiser area. When you set the area it will 
come into immediate affect, i.e. if necessary the current pointer position 
will be updated to be within the new area. 

Notes:

Relative mouse mode is where the events received from the base by window server 
are deltas from the last position of the pointer, as opposed to absolute co-ordinates.

This function is honoured even if there is a mouse or pen (e.g. on the emulator), 
by mapping the co-ordinates of where you click into the area set using this 
function. However the function does not restrict clicks outside of the 'drawing 
area' on the Emulator, to allow you to select items on the fascia.

@param aArea The area of the screen in which the virtual cursor can be used. 
@capability WriteDeviceData */
	{
	SetPointerCursorArea(0,aArea);
	}

EXPORT_C void RWsSession::SetPointerCursorArea(TInt aScreenSizeMode,const TRect& aArea)
/** Sets the area of the screen in which the virtual cursor can be used while in 
relative mouse mode, for a specified screen display mode. 

The default area is the full digitiser area for the given mode. When you set 
the area it will come into immediate affect, i.e. if necessary the current 
pointer position will be updated to be within the new area. 

The area is set and stored independently on each screen mode, so that it is 
not necessary to call this function again when switching back to a mode. 

Notes:

Relative mouse mode is where the events received from the base by window server 
are deltas from the last position of the pointer, as opposed to absolute co-ordinates.

The previous function overload may be used to set the screen area for only 
the first mode.

This function is honoured even if there is a mouse or pen (e.g. on the emulator), 
by mapping the co-ordinates of where you click into the area set using this 
function. However the function does not restrict clicks outside of the 'drawing 
area' on the Emulator, to allow you to select items on the fascia.

@param aScreenSizeMode The screen mode to which the new area applies. 
@param aArea The area of the screen, in the aScreenSizeMode screen mode, within 
which the virtual cursor can be used.
@capability WriteDeviceData */
	{
	TWsClCmdSetPointerCursorArea SetPointerCursorArea(aScreenSizeMode,aArea);
	Write(&SetPointerCursorArea,sizeof(SetPointerCursorArea),EWsClOpSetPointerCursorArea);
	}

EXPORT_C TRect RWsSession::PointerCursorArea() const
/** Gets the pointer cursor area for the first screen display mode.

This is the area of the screen in which the virtual cursor can be used while 
in relative mouse mode. While in pen or mouse mode the event co-ordinates 
are forced to be within this area unless you click outside the drawable area.

This function always causes a flush of the window server buffer.

@return The pointer cursor area for the first screen display mode.
@see SetPointerCursorArea() */
	{
	return PointerCursorArea(0);
	}

EXPORT_C TRect RWsSession::PointerCursorArea(TInt aScreenSizeMode) const
/** Gets the pointer cursor area for the specified screen display mode. 

This is the area of the screen in which the virtual cursor can be used while 
in relative mouse mode. While in pen or mouse mode the event co-ordinates 
are forced to be within this area unless you click outside the drawable area.

This function always causes a flush of the window server buffer.

@param aScreenSizeMode The screen mode for which the pointer cursor area is 
required. 
@return The pointer cursor area for the specified screen display mode. 
@see SetPointerCursorArea() */
	{
	TPckgBuf<TRect> rectPkg;
  	WriteReplyP(&aScreenSizeMode,sizeof(aScreenSizeMode),&rectPkg,EWsClOpPointerCursorArea);
	return(rectPkg());
	}

EXPORT_C void RWsSession::SetPointerCursorMode(TPointerCursorMode aMode)
/** Sets the current mode for the pointer cursor.

The mode determines which sprite is used for the pointer cursor at any point. 
The request is ignored unless the calling application is the application 
that currently has keyboard focus.
See TPointerCursorMode for more information.

@param aMode The new mode for the pointer cursor. 
@see TPointerCursorMode */
	{
	WriteInt(aMode,EWsClOpSetPointerCursorMode);
	}
	
EXPORT_C TInt RWsSession::SetClientCursorMode(TPointerCursorMode aMode)
/** Sets the current mode for the pointer cursor.

The mode determines which sprite is used for the pointer cursor at any point. 
See TPointerCursorMode for more information.

This function always causes a flush of the window server buffer.

@param aMode The new mode for the pointer cursor. 
@see TPointerCursorMode
@return KErrNone if successful, otherwise returns system wide errors. Returns
KErrPermissionDenied if calling thread lacks WriteDeviceData capability
@capability WriteDeviceData */
	{
	return(WriteReplyInt(aMode,EWsClOpSetClientCursorMode));
	}

EXPORT_C TPointerCursorMode RWsSession::PointerCursorMode() const
/** Gets the current mode for the pointer cursor.

The mode determines which sprite is used for the pointer cursor at any point.

This function always causes a flush of the window server buffer.

@return The current mode for the pointer cursor. */
	{
	return(STATIC_CAST(TPointerCursorMode,WriteReply(EWsClOpPointerCursorMode)));
	}

EXPORT_C void RWsSession::SetDefaultSystemPointerCursor(TInt aCursorNumber)
/** Sets the default system pointer cursor.

This function can only be called by the owner of the system pointer cursor 
list. By default the 0th entry in the pointer cursor list is assigned as the 
system pointer. The function allows any cursor from the list or even no cursor 
to be set as the system pointer cursor.

Note: ownership of the system pointer cursor list can be obtained by calling 
ClaimSystemPointerCursorList() when no-one else has ownership.

@param aCursorNumber The index of the new default system pointer cursor within 
the system cursor list. */
	{
	WriteInt(aCursorNumber,EWsClOpSetDefaultSystemPointerCursor);
	}

EXPORT_C void RWsSession::ClearDefaultSystemPointerCursor()
/** Clears the default system pointer cursor.

This sets the pointer to the current default system pointer cursor to NULL. 

@panic WSERV 42 If this function is called by a client other than the owner of the 
system pointer cursor list. */
	{
	Write(EWsClOpClearDefaultSystemPointerCursor);
	}

EXPORT_C TInt RWsSession::SetPointerCursorPosition(const TPoint& aPosition)
/** Sets the pointer cursor position.

This function allows an application to move the virtual cursor. It works in 
all modes, not just relative mouse mode.

Note: the function works in screen co-ordinates and honours the pointer cursor area 
exactly as pen presses do, i.e. only when they are in the drawing area 
on the Emulator.

This function always causes a flush of the window server buffer.

@param aPosition The new pointer cursor position.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability WriteDeviceData required, if the client calling the function doesn't have keyboard focus.
However, if the client have keyboard focus then he doesn't need any capability to call this function.  */
	{
	return(WriteReply(&aPosition,sizeof(aPosition),EWsClOpSetPointerCursorPosition));
	}

EXPORT_C TPoint RWsSession::PointerCursorPosition() const
/** Gets the pointer cursor position.

This function allows an application to determine the position of the virtual 
cursor.

It always causes a flush of the window server buffer.

Please note that on devices with multiple pointers (for example with multi-touch screen)
the pointer cursor's position will be equal to the last known position of the emulated pointer.
More information about the emulated pointer can be found in description of method
RWindowBase::EnableAdvancedPointers().

@return The position of the virtual cursor. 
@see RWindowBase::EnableAdvancedPointers() */
	{
	TPckgBuf<TPoint> pointPkg;
  	WriteReplyP(&pointPkg,EWsClOpPointerCursorPosition);
	return(pointPkg());
	}

EXPORT_C void RWsSession::SetDefaultFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap)
/**
@publishedPartner
@released

Sets the default fading parameters.

Fading is used to change the colour of windows to be either closer 
to white or closer to black, so that another window stands out. For example, 
when displaying a dialogue you could fade all visible windows, then unfade 
the dialog window. This function sets whether, and the amount by which, faded 
windows appear closer to white or closer to black.

The white and black mapping values define the range over which colours are 
re-mapped when a window is faded. If aBlackMap=0 and aWhiteMap=255 then the 
colours are mapped unchanged. As the two values get closer together, all colours 
in the faded window becomes more similar - creating the fading effect. When 
the numbers cross over (so that the black value is greater than the white 
value) the colours in the faded window start to invert - i.e. colours that 
were closer to white in the unfaded window are mapped to a darker colour in 
the faded window.

Changing the default will automatically apply to current graphics contexts 
but will not have any affect on windows that are already faded. 

Note: RWindowTreeNode::SetFaded(), CWindowGc::SetFaded() and RWsSession::SetSystemFaded() 
use these fading parameters, and in addition allow the default fading value 
to be overridden.

@param aBlackMap Black map fading parameter.
@param aWhiteMap White map fading parameter. 
@see RWindowTreeNode::SetFaded()
@see CWindowGc::SetFadingParameters() 
@capability WriteDeviceData */
	{
	WriteInt(WservEncoding::Encode8BitValues(aBlackMap,aWhiteMap),EWsClOpSetDefaultFadingParams);
	}

/**
@publishedPartner
@released

Prepares for switch off. 

This stops the window server heart beat timer if running.

@capability PowerMgmt
*/
EXPORT_C void RWsSession::PrepareForSwitchOff()
	{
	Write(EWsClOpPrepareForSwitchOff);
	}

#if defined(__WINS__)
EXPORT_C void RWsSession::SetRemoveKeyCode(TBool aRemove)
/** Sets whether to remove the top 16 bits of a key code (Emulator builds only).

This function allows the Emulator to use Windows to translate keypresses into 
the correct key code for each locale, rather than having to do the translation 
for each international keyboard itself. 

The top 16 bits of a keypress code contains the keycode that Windows would 
produce if the key had been pressed in a typical Windows program. If aRemove 
is EFalse the client can get these top 16 bits. If the value is non-zero 
the CKeyTranslator uses it rather than calculating it's own value. If aRemove 
is ETrue the window server strips the top 16 bits of the scan code before 
passing the value on to the client. 

Note: this function is intended for Java but it will be useful to any client who 
creates their own CKeyTranslator and processes keyups and downs.

@param aRemove ETrue to remove the code (default), EFalse to pass it to the 
client. */
	{
	WriteInt(aRemove,EWsClOpRemoveKeyCode);
	}

EXPORT_C void RWsSession::SimulateXyInputType(TInt aInputType)
	{
	WriteInt(aInputType,EWsClOpSimulateXyInput);
	}
#endif

EXPORT_C void RWsSession::LogCommand(TLoggingCommand aCommand)
/** Allows the window server client to enable or disable logging of window server 
events.

The type of logging that takes place (e.g. whether to file or to serial port) 
depends on the settings specified in the wsini.ini file.

Clients can also force a dump of the window tree or information about the 
window server's heap size and usage. 

For logging to work, the wsini.ini file has to specify the type of logging 
required and the DLLs for that type of logging must have been correctly installed. 
Otherwise, calling this function will have no effect.

@param aCommand The logging command. */
	{
	WriteInt(aCommand,EWsClOpLogCommand);
	}

EXPORT_C void RWsSession::LogMessage(const TLogMessageText &aMessage)
/** Adds a message to the window server debug log if one is currently in operation.

This function always causes a flush of the window server buffer.

@param aMessage The text added to the message log. */
	{
	TInt len=aMessage.Length();
	WriteReply(&len,sizeof(len),aMessage.Ptr(),aMessage.Size(),EWsClOpLogMessage);
	}

EXPORT_C void RWsSession::SimulateRawEvent(TRawEvent aEvent)
/** 
@internalAll

Simulates raw events.

For most purposes, RWsSession::SimulateKeyEvent() should be used instead to 
simulate key events because low-level scan-code up/down events are not meaningful 
to anything other than the keyboard to which they apply.

For example, the driver for an external keyboard should do its own conversion from 
raw scan-codes to higher-level character code key events and pass these to the 
window server using SimulateKeyEvent().

@param aEvent The raw event.
@capability SwEvent */
	{
	Write(&aEvent,sizeof(aEvent),EWsClOpRawEvent);
	}

EXPORT_C void RWsSession::SimulateKeyEvent(TKeyEvent aEvent)
/** 
@internalAll

Sends a simulated key event to the window server.

All the fields in TKeyEvent are honoured except iRepeats, which is overridden 
with zero.

@param aEvent The key event to be simulated.
@capability SwEvent */
	{
	Write(&aEvent,sizeof(aEvent),EWsClOpKeyEvent);
	}


EXPORT_C void RWsSession::SystemInfo(TInt &aSystemInfoNumber, SSystemInfo &aSystemInfo)
/** @internalComponent */
	{
	TPckgBuf<SSystemInfo> systemInfoPckg;
	WriteReplyP(&aSystemInfoNumber,sizeof(aSystemInfoNumber),&systemInfoPckg,EWsClOpSystemInfo);
	aSystemInfo=systemInfoPckg();
	}

void RWsSession::DirectAcessActivation(TBool aIsNowActive)
	{
	if (aIsNowActive)
		++iBuffer->iDirectAcessCount;
	else
		{
		--iBuffer->iDirectAcessCount;
		__ASSERT_DEBUG(iBuffer->iDirectAcessCount>=0,Assert(EW32AssertDirectMisuse));
		}
	}

EXPORT_C void RWsSession::TestWrite(TInt aHandle,TInt aOpcode,const TAny *pData, TInt aLength)
/** @internalComponent */
	{
	iBuffer->Write(aHandle,aOpcode,pData,aLength);
	}

EXPORT_C void RWsSession::TestWriteReply(TInt aHandle,TInt aOpcode,const TAny *pData, TInt aLength)
/** @internalComponent */
	{
	iBuffer->WriteReply(aHandle,aOpcode,pData,aLength);
	}

EXPORT_C void RWsSession::TestWriteReplyP(TInt aHandle,TInt aOpcode,const TAny *pData,TInt aLength,TDes8 *aReplyPackage)
/** @internalComponent */
	{
	iBuffer->WriteReplyP(aHandle,aOpcode,pData,aLength,aReplyPackage);
	}

EXPORT_C TInt RWsSession::TestWriteReplyByProvidingRemoteReadAccess(TInt aHandle,TInt aOpcode,const TDesC8& aData,const TDesC8& aRemoteReadBuffer)
/** @internalComponent */
	{
	return iBuffer->WriteReplyByProvidingRemoteReadAccess(aHandle,aOpcode,aData.Ptr(),aData.Length(),&aRemoteReadBuffer);
	}

EXPORT_C TInt RWsSession::TestWriteReplyByProvidingRemoteReadAccess(TInt aHandle,TInt aOpcode,const TDesC8& aData,const TDesC16& aRemoteReadBuffer)
/** @internalComponent */
	{
	return iBuffer->WriteReplyByProvidingRemoteReadAccess(aHandle,aOpcode,aData.Ptr(),aData.Length(),&aRemoteReadBuffer);
	}

/** Sets both the buffer size and maximum buffer size for queuing commands to send to the Windows Server.
The value should be at least the size of the largest message that will be sent, 
otherwise a panic of the client may occur.

The minimum possible buffer size is 640 and the maximum possible size is 16384 bytes.
The default size of 640 bytes is sufficient for most uses.

Larger buffers can reduce drawing flicker by allowing more drawing commands to be
collected in the buffer before being sent to the server.

Smaller buffers conserve system memory.

Can be used to set a minimum buffer size, sufficient for largest drawing command used,
before calling RWsSession::SetMaxBufferSizeL() to set a maximum buffer size for queuing commands.

@param aBufSize The desired buffer size in bytes, at least the size of largest message to be sent.
@leave KErrNoMemory Could not allocate the required memory.

@panic TW32Panic 5 Occurs during the session if a single drawing command is too big to fit in the buffer.

@see RWsSession::Flush()
@see RWsSession::SetAutoFlush()
@see RWsSession::SetMaxBufferSizeL()
*/
EXPORT_C void RWsSession::SetBufferSizeL(TInt aBufSize)
	{
	iBuffer->SetBufferSizeL(aBufSize);
	}

/** Sets the maximum size that the buffer for queuing commands to send to the Windows Server can expand to.

The minimum possible buffer size is 640 and the maximum possible size is 16384 bytes.

If the buffer size is larger than the new maximum it is reduced.

A minimum size is calculated to be one quarter the maximum size, but in any case at least 640 bytes.
If the buffer size is smaller than the calculated minimum it is expanded.

RWsSession::SetBufferSizeL() can be used to set a specific minimum size >640 bytes before setting a maximum size.
This is useful if you will send very large drawing commands.

After calling this function the buffer size will be between the specified maximum and calculated minimum sizes.

The algorithm for growing the buffer up to the maximum is chosen to balance the cost of expanding the buffer 
with the waste of an excessively large buffer that is never filled.

If the buffer becomes too full to add a new drawing command, and the buffer size is less than the maximum,
the buffer size will be expanded.
If the buffer is already at the maximum size, or the expansion fails due to low memory, 
the buffer will be emptied with RWsSession::Flush().
If there is not enough space now for the new command a panic occurs.

@param aMaxBufSize The desired maximum buffer size in bytes.
@leave KErrNoMemory Could not allocate the required minimum memory.

@panic TW32Panic 5 Occurs during the session if a single drawing command is too big to fit in the buffer.

@see RWsSession::Flush()
@see RWsSession::SetAutoFlush()
@see RWsSession::SetBufferSizeL()
*/
EXPORT_C void RWsSession::SetMaxBufferSizeL(TInt aMaxBufSize)
	{
	iBuffer->SetMaxBufferSizeL(aMaxBufSize);
	}

EXPORT_C TInt RWsSession::SetSystemFaded(TBool aFaded)
/** Sets all windows in the system as faded or unfaded, using the default fading 
parameters.

This function allows all windows that currently exist, not just those in a 
single window group, to be faded or unfaded.

Notes: The window server generates a redraw to un-fade a window, because information 
is lost during fading. Blank (RBlankWindow) and backup (RBackupWindow) windows 
deal with this themselves. Areas in shadow when the window is faded will also 
have redraw events generated for them by the window server. While a window 
is faded, all drawing to that window will be adjusted appropriately by the 
window server.

This function always causes a flush of the window server buffer.

@param aFaded ETrue to fade all windows, EFalse to un-fade all windows.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability WriteDeviceData */
	{
	TWsClCmdSetSystemFaded params(aFaded);
	return WriteReply(&params,sizeof(params),EWsClOpSetFaded);
	}

EXPORT_C TInt RWsSession::SetSystemFaded(TBool aFaded,TUint8 aBlackMap,TUint8 aWhiteMap)
/** Sets all windows in the system as faded or unfaded, overriding the default 
fading parameters (as set by SetDefaultFadingParameters()).

This function allows all windows that currently exist, not just those in the 
same window group, to be faded or unfaded.

Notes: Fading a window for a second time (that is fading it when it is already 
faded) will not change the fading map used. The window server generates a 
redraw to un-fade a window, because information is lost during fading. Blank 
(RBlankWindow) and backup (RBackupWindow) windows deal with this themselves. 
Areas in shadow when the window is faded will also have redraw events generated 
for them by the window server. While a window is faded, all drawing to that 
window will be adjusted appropriately by the window server.

This function always causes a flush of the window server buffer.

@param aFaded ETrue to fade all windows, EFalse to un-fade all windows.
@param aBlackMap Black map fading parameter.
@param aWhiteMap White map fading parameter.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@capability WriteDeviceData */
	{
	TWsClCmdSetSystemFaded params(aFaded,EFalse,aBlackMap,aWhiteMap);
	return WriteReply(&params,sizeof(params),EWsClOpSetFaded);
	}

EXPORT_C TInt RWsSession::SetFocusScreen(TInt aScreenNumber)
/** Set focus screen  
@param aScreenNumber The new focus screen.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	return WriteReplyInt(aScreenNumber,EWsClOpSetFocusScreen);
	}

EXPORT_C TInt RWsSession::GetFocusScreen() const
/** Get focus screen
@return The screen number of current focus screen */
	{
	return WriteReply(EWsClOpGetFocusScreen);
	}

EXPORT_C TInt RWsSession::NumberOfScreens() const
/** Number Of Screens
@return The number of screens on the phone */
	{
	return WriteReply(EWsClOpGetNumberOfScreens);	
	}
	
EXPORT_C void RWsSession::ClearAllRedrawStores()
/** Clear the redraw store for all windows in the system.
By default Windows will record the drawing commands used during a redraw and re-use them later if the window needs to be redrawn.
Calling this function will cause all these server-side stores to be thrown away, redraw requests will then be sent to all client-side windows. Visible windows will receive them first.

In most cases you should be using RWindow::ClearRedrawStore instead of this function. 

This function always causes a flush of the window server buffer.*/
	{
	Write(EWsClOpClearAllRedrawStores);
	}

EXPORT_C TInt RWsSession::Finish()
/** 
Blocks until all outstanding window server rendering has been completed.
@return KErrNone if successful 
*/
	{
	SyncMsgBuf();
	return SendReceive(EWservMessFinish);
	}

EXPORT_C void RWsSession::SyncMsgBuf()
/** 
Sends all pending commands in the buffer to the window server. 
*/
	{
	if (iBuffer)
		iBuffer->Flush();
	}

EXPORT_C TInt RWsSession::SetCloseProximityThresholds(TInt aEnterCloseProximityThreshold, TInt aExitCloseProximityThreshold)
/** Sets Z coordinate threshold values for TPointerEvent::EEnterCloseProximity 
and TPointerEvent::EExitCloseProximity events. 
As proximity occupies the negative range of Z coordinates, on most platforms
threshold values for these events should be specified as negative.

However, all possible Z coordinate values are accepted by this method
regardless of the fact that positive Z coordinate values are interpreted as 
pressure and only negative Z coordinate values are interpreted as proximity.
This means that if a particular platform supports an Up pointer state while a pointer is
touching the screen, it is possible to base the firing of 
EnterCloseProximity/ExitCloseProximity events on pressure readings by specifying 
positive threshold values.

EnterCloseProximity/ExitCloseProximity event generation can be switched off completely by 
specifying the following thresholds: aEnterCloseProximityThreshold = KMaxTInt, 
aExitCloseProximityThreshold = KMinTInt.

This method is supposed to be used only by the UI platform or device's configuration tool.

Modified thresholds will be persisted across system reboots in the following HAL
attributes: HALData::EPointer3DEnterCloseProximityThreshold, HALData::EPointer3DExitCloseProximityThreshold. 

If this method is never called on a particular device, the default threshold values defined by this device's 
manufacturer are used.

@param aEnterCloseProximityThreshold a new threshold for TPointerEvent::EEnterCloseProximity
                                     to be set, specified as Z coordinate value
@param aExitCloseProximityThreshold a new threshold for TPointerEvent::EExitCloseProximity
                                    to be set, specified as Z coordinate value
@return KErrNone if successful, 
        KErrNotSupported if the platform doesn't support threshold values for 
                         Close Proximity events,
        KErrArgument if aEnterCloseProximityThreshold is less than aExitCloseProximityThreshold;
                     when this error occurs, threshold values are not changed. 
@capability WriteDeviceData
@see TPointerEvent::EEnterCloseProximity
@see TPointerEvent::EExitCloseProximity
@see HALData::EPointer3DEnterCloseProximityThreshold
@see HALData::EPointer3DExitCloseProximityThreshold
@publishedPartner
@prototype To become released with WSERV NGA APIs */
	{
	TWsClCmdZThresholdPair params(aEnterCloseProximityThreshold, aExitCloseProximityThreshold);
	return WriteReply(&params,sizeof(params),EWsClOpSetCloseProximityThresholds);
	}

EXPORT_C TInt RWsSession::GetEnterCloseProximityThreshold() const
/**
@return Z coordinate threshold value for TPointerEvent::EEnterCloseProximity events.
@see TPointerEvent::EEnterCloseProximity
@publishedPartner To become publishedAll with WSERV NGA APIs
@prototype To become released with WSERV NGA APIs */
	{
	return WriteReply(EWsClOpGetEnterCloseProximityThreshold);	
	}

EXPORT_C TInt RWsSession::GetExitCloseProximityThreshold() const
/**
@return Z coordinate threshold value for TPointerEvent::EExitCloseProximity events
@see TPointerEvent::EExitCloseProximity
@publishedPartner To become publishedAll with WSERV NGA APIs
@prototype To become released with WSERV NGA APIs */
	{
	return WriteReply(EWsClOpGetExitCloseProximityThreshold);	
	}

EXPORT_C TInt RWsSession::SetHighPressureThresholds(TInt aEnterHighPressureThreshold, TInt aExitHighPressureThreshold)
/** Sets Z coordinate threshold values for TPointerEvent::EEnterHighPressure 
and TPointerEvent::EExitHighPressure events. 
As pressure occupies the positive range of Z coordinates, on most platforms
threshold values for these events should be specified as positive values.

However, all possible Z coordinate values are accepted by this method
regardless of the fact that only the positive Z coordinate values are interpreted as 
pressure and the negative Z coordinate values are interpreted as proximity.
This means that if a particular platform supports the Down pointer state while a pointer is
in proximity to the screen, it is possible to base the firing of 
EnterHighPressure/ExitHighPressure events on proximity readings by specifying 
negative threshold values.

EnterHighPressure/ExitHighPressure event generation can be switched off completely by 
specifying the following thresholds: aEnterHighPressureThreshold = KMaxTInt, 
aExitHighPressureThreshold = KMinTInt.

This method is supposed to be used only by the UI platform or device's configuration tool.

Modified thresholds will be persisted across system reboots in the following HAL
attributes: HALData::EPointer3DEnterHighPressureThreshold, HALData::EPointer3DExitHighPressureThreshold. 

If this method is never called on a particular device, the default threshold values defined by this device's 
manufacturer are used.

@param aEnterHighPressureThreshold a new threshold for TPointerEvent::EEnterHighPressure
                                   to be set, specified as Z coordinate value
@param aExitHighPressureThreshold a new threshold for TPointerEvent::EExitHighPressure
                                  to be set, specified as Z coordinate value
@return KErrNone if successful, 
        KErrNotSupported if the platform doesn't support threshold values for 
                         High Pressure events,
        KErrArgument if aEnterHighPressureThreshold is less than aExitHighPressureThreshold; 
                     when this error occurs, threshold values are not changed. 
@capability WriteDeviceData
@see TPointerEvent::EEnterHighPressure
@see TPointerEvent::EExitHighPressure
@see HALData::EPointer3DEnterHighPressureThreshold
@see HALData::EPointer3DExitHighPressureThreshold
@publishedPartner
@prototype To become released with WSERV NGA APIs */
	{
	TWsClCmdZThresholdPair params(aEnterHighPressureThreshold, aExitHighPressureThreshold);
	return WriteReply(&params,sizeof(params),EWsClOpSetHighPressureThresholds);
	}

EXPORT_C TInt RWsSession::GetEnterHighPressureThreshold() const
/**
@return Z coordinate threshold value for TPointerEvent::EEnterHighPressure events
@see TPointerEvent::EEnterHighPressure
@publishedPartner To become publishedAll with WSERV NGA APIs
@prototype To become released with WSERV NGA APIs */
	{
	return WriteReply(EWsClOpGetEnterHighPressureThreshold);	
	}

EXPORT_C TInt RWsSession::GetExitHighPressureThreshold() const
/**
@return Z coordinate threshold value for TPointerEvent::EExitHighPressure events
@see TPointerEvent::EExitHighPressure
@publishedPartner To become publishedAll with WSERV NGA APIs
@prototype To become released with WSERV NGA APIs */
	{
	return WriteReply(EWsClOpGetExitHighPressureThreshold);	
	}

EXPORT_C void RWsSession::EnableWindowSizeCacheL()
/**
Enables client side cache of window size to reduce client-server 
activity triggered by calls to RWindowBase::Size()
@leave KErrNoMemory Could not allocate the required memory.
@internalAll */
	{
	iBuffer->EnableWindowSizeCacheL();
	}

EXPORT_C void RWsSession::SendEffectCommand(TInt aTfxCmd,const TDesC8& aTfxCmdData)
/**
 Set the WServ session specific effect data or execute commands 
 to TFX Render Stage. The data or command passed by the client 
 API will be directed to MWsTfxApplication::SendEffectCommand.
 TFX Render Stage will accept only TFX application specific commands and data.
@param aTfxCmd TFX Render Stage command.
@param aTfxCmdData Structure related to the specified value of aTfxCmd, the default value is KNullDesC8.

@capability WriteDeviceData Only if aTfxCmd has value ETfxCmdEnableAllTransitions or ETfxCmdDisableAllTransitions

@publishedPartner
*/
	{
	__ASSERT_ALWAYS(aTfxCmdData.Length()<=KMaxWservStringSize, Panic(EW32PanicStringTooLong));
	TWsClCmdSendEffectCommand params(aTfxCmd,aTfxCmdData.Size(), NULL);
	Write(&params,sizeof(params),aTfxCmdData.Ptr(),aTfxCmdData.Size(),EWsClOpSendEffectCommand);
	}

EXPORT_C void RWsSession::RegisterEffect(TInt aAction, TInt aPurpose, const TFileName& aResourceDir, const TFileName& aFilenameOutgoing, const TFileName& aFilenameIncoming, TUint aAppUid, TBitFlags aFlags)
/**
Sets a specific animation for a particular transition effect.

Transition effect is represented by one of the values of enum TTfxTransitionActions.

An animation is specified by a filename and directory; the file will contain a description of the animation 
for that transition.  In fact each transition can have two animations associated with it, for example 
an App shut down could have one animation with the old application disappearing and then another animation 
for the App Launcher (or home screen) appearing.

Animation can be applied to all App's Transition effect or to a specfic App by providing its AppUid.

@param aAction Particular transition to register the animation for.
@param aPurpose The purpose of the window.
@param aResourceDir The name of the directory that contains the animation description files.
@param aFilenameOutgoing The file containing the description of the animation for the outgoing phase of the transition. 
						 Specify KNullDesC for no outgoing phase effect.
@param aFilenameIncoming The file containing the description of the animation for the incoming phase of the transition. 
						 Specify KNullDesC for no incoming phase effect.
@param aAppUid The Application UID this effect applies to. Set to zero to specify that all apps will use default effect.
@param aFlags Flag for the effect. Please see TTfxFlags for values this flag parameter can use.

@capability WriteDeviceData This API does nothing if the client does not possess required Capability

@publishedPartner
*/
	{
	RTFXEffect tfxEffect(iWsHandle, iBuffer);
	tfxEffect.RegisterTFXEffect(aAction, aPurpose, aResourceDir, aFilenameOutgoing, aFilenameIncoming, aAppUid, aFlags);
	}

EXPORT_C void RWsSession::UnregisterEffect(TInt aAction, TInt aPurpose, TUint aAppUid)
/**
Unregister already set animation for a particular transition effect

@param aAction Particular transition to unregister the animation for.
@param aPurpose The purpose of the window.
@param aAppUid The Application UID this effect applies to. Set to 0 to specify the default effect will be unregistered.

@capability WriteDeviceData This API does nothing if the client does not possess required Capability

@publishedPartner
*/
	{
	TWsClCmdUnRegisterEffect unregisterEffect(aAction, aPurpose, aAppUid);
	Write(&unregisterEffect, sizeof(unregisterEffect), EWsClOpUnregisterTFXEffect);
	}

EXPORT_C void RWsSession::UnregisterAllEffects()
/**
Unregister animation for all transition effects.

@capability WriteDeviceData This API does nothing if the client does not possess required Capability

@publishedPartner
*/
	{
	Write(EWsClOpUnregisterAllTFXEffect);
	}
	
EXPORT_C void RWsSession::OverrideEffects(TInt aAction, TInt aPurpose, const TFileName& aResourceDir, const TFileName& aFilenameOutgoing, const TFileName& aFilenameIncoming, TBitFlags aFlags)
/**
Overides the default animation for given app's transition effect by sent animation description.
Please refer RWsSession::RegisterEffect() comments for more information on animation description.

@param aAction The particular transition to set the animation for.
@param aPurpose This override only effects the window/layers owned by the application that have the specified purpose.
@param aResourceDir The name of the directory that contains the animation description files.
@param aFilenameOutgoing The file containing the description of the animation for the outgoing phase of the transition. 
						 Specify KNullDesC for no outgoing phase effect.
@param aFilenameIncoming The file containing the description of the animation for the incoming phase of the transition. 
						 Specify KNullDesC for no incoming phase effect.
@param aFlags Flag for the effect. Please see TTfxFlags for values this flag parameter can use.

@capability WriteDeviceData This API does nothing if the client does not possess required Capability

@publishedPartner
*/
	{
	RTFXEffect tfxEffect(iWsHandle, iBuffer);
	tfxEffect.OverrideTFXEffect(RTFXEffect::ETFXSession, aAction, aPurpose, aResourceDir, aFilenameOutgoing, aFilenameIncoming, aFlags);
	}

EXPORT_C void RWsSession::IndicateAppOrientation(TRenderOrientation aOrientation)
/**
Application informs window server the orientation of rendering it intends to use

@param aOrientation The orientation the application intends to use

@publishedPartner
*/
	{
	return(WriteInt(aOrientation,EWsClOpIndicateAppOrientation));
	}
