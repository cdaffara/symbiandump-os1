// Copyright (c) 1994-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Shells for window server window functions.
// 
//

#include <e32std.h>
#include "../SERVER/w32cmd.h"
#include "CLIENT.H"
#include "w32comm.h"

RWindowTreeNode::RWindowTreeNode()
/** Protected default constructor.

This creates a sessionless, uninitialised window tree node handle. This class 
is not for user derivation; however derived classes form part of the Window 
Server API.

Because RWindowTreeNode and its derived classes are lightweight handles, if 
your class contains an object of such a class, the object should be an inline 
member rather than a pointer member.

This default constructor allows you to instantiate such an inline member before 
the class that contains it has created a window server session. If you do this, 
you will need to call the RWindowTreeNode constructor that takes an RWsSession 
parameter before you can use the member, because RWindowTreeNode-derived objects 
must have a reference to a window server session in order to be valid. */
	{}

RWindowTreeNode::RWindowTreeNode(RWsSession &aWs) : MWsClientClass(aWs.iBuffer)
/** Protected constructor which creates an uninitialised window tree node handle 
within a server session. 

This class is not for user derivation; however derived classes form part of 
the standard Window Server API, and are constructed with a public 
constructor with the same signature as this one.

@param aWs Window server session. */
	{}

RWindowBase::RWindowBase() : RWindowTreeNode()
/** Protected default constructor; creates an uninitialised, sessionless window 
handle.

Handles to server-side objects must be created in a session in order to be 
operational; this constructor is merely a convenience to allow the handle 
to be stored as a data member. See RWindowTreeNode::RWindowTreeNode() for 
details of how the complete setup of a handle field may be deferred until 
the window server session is known. */
	{}

RWindowBase::RWindowBase(RWsSession &aWs) : RWindowTreeNode(aWs)
/** Protected constructor; creates an uninitialised window handle within a session.

@param aWs Window server session. */
	{}

RDrawableWindow::RDrawableWindow() : RWindowBase()
/** Protected default constructor which creates a sessionless, uninitialised drawable-window 
handle.

Handles to server-side objects must be created in a session in order to be 
operational; this constructor is merely a convenience to allow the handle 
to be stored as a data member. See RWindowTreeNode::RWindowTreeNode() for 
details of how the complete setup of a handle field may be deferred until 
the window server session is known */
	{}

RDrawableWindow::RDrawableWindow(RWsSession &aWs) : RWindowBase(aWs)
/** Protected default constructor which creates an initialised drawable-window 
handle within a server session.

@param aWs Window server session. */
	{}

EXPORT_C RWindow::RWindow() : RDrawableWindow()
/** Default constructor which creates a sessionless, uninitialised window handle. 

Handles to server-side objects must be created in a session in order to be 
operational; this constructor is merely a convenience to allow the handle 
to be stored as a data member. See RWindowTreeNode::RWindowTreeNode() for 
details of how the complete setup of a handle field may be deferred until 
the window server session is known. */
	{}

EXPORT_C RWindow::RWindow(RWsSession &aWs) : RDrawableWindow(aWs)
/** Constructor which creates an initialised window handle within a server session.

@param aWs Window server session to use. */
	{}

EXPORT_C RBackedUpWindow::RBackedUpWindow() : RDrawableWindow()
/** Default C++ constructor which creates a sessionless backed-up window handle.

Handles to server-side objects must be created in a session in order to be 
operational; this constructor is merely a convenience to allow the handle 
to be stored as a data member. See RWindowTreeNode::RWindowTreeNode() for 
details of how the complete setup of a handle field may be deferred until 
the window server session is known. */
	{}

EXPORT_C RBackedUpWindow::RBackedUpWindow(RWsSession &aWs) : RDrawableWindow(aWs)
/** Constructor which creates an uninitialised backed-up window handle within a 
session.

@param aWs The window server session that owns the window. */
	{}

EXPORT_C RBlankWindow::RBlankWindow() : RWindowBase()
/** Default C++ constructor which creates an invalid blank-window handle. 

See RWindowTreeNode::RWindowTreeNode() for details of how the complete setup 
of a handle field may be deferred until the window server session is known. */
	{}

EXPORT_C RBlankWindow::RBlankWindow(RWsSession &aWs) : RWindowBase(aWs)
/** Default C++ constructor which creates a valid but uninitialised blank-window 
handle. 

This constructor does not create a window in the window server: client programs 
must do this by calling RBlankWindow::Construct() before any operations can 
be carried out on the window.

@param aWs The window server session that owns the window. */
	{}

EXPORT_C RWindowGroup::RWindowGroup() : RWindowTreeNode()
/** Creates a sessionless, uninitialised window group handle.

Handles to server-side objects must be created in a session in order to be 
operational; this constructor is merely a convenience to allow the handle 
to be stored as a data member. See RWindowTreeNode::RWindowTreeNode() for 
details of how the complete setup of a handle field may be deferred until 
the window server session is known. */
	{}

EXPORT_C RWindowGroup::RWindowGroup(RWsSession &aWs) : RWindowTreeNode(aWs)
/** Creates an initialised window group handle within a server session.

@param aWs The window server session owning the window group. */
	{}

EXPORT_C void RWindowTreeNode::Close()
/** Closes the node. 

This function should be called on all windows once they are no longer needed. 
It causes the window server to destroy the server-side window, and frees client-side 
resources owned by the window.

Note: When Close() is called on a parent window, its children are disconnected 
from the window tree and are hence removed from the screen. However, any client-side 
resources owned by its children are not freed. To free these resources, Close() 
(or Destroy()) must be called on all its children individually. */
	{
	if (iBuffer && iWsHandle)
		Write(EWsWinOpFree);
	iWsHandle=NULL;
	}

EXPORT_C void RWindowTreeNode::Destroy()
/** Closes and deletes the node. 

This function calls Close() followed by delete on the window. Use this function 
only when the window is allocated in its own heap cell. */
	{
	Close();
	delete this;
	}

EXPORT_C TUint32 RWindowTreeNode::ClientHandle() const
/** Gets the window's client handle

The return value is the client's integer handle that was passed as an argument
to the window's Construct() function: see RWindow::Construct()
for a description of the client handle.

This function always causes a flush of the window server buffer.

@return Handle ID for the window.
@see RWindow::Construct() */
	{
	return(WriteReply(EWsWinOpClientHandle));
	}

EXPORT_C TUint32 RWindowTreeNode::Parent() const
/** Gets the node's parent. 

The return value is the client's integer handle that was passed as an argument 
to the parent window tree node's Construct() function: see RWindow::Construct() 
for a description of the client handle.
If called on a window group, this function returns 0, as window groups have 
no parent.
 
This function always causes a flush of the window server buffer.

@return Handle ID for the parent, or zero for window groups. 
@see Child() 
@see RWindow::Construct() */
	{
	return(WriteReply(EWsWinOpParent));
	}

EXPORT_C TUint32 RWindowTreeNode::PrevSibling() const
/** Gets the node before this one in the sibling list. 

The return value is the client handle that was passed in the previous sibling window's 
Construct() function: see RWindow::Construct() for a description of the client handle.
 
This function always causes a flush of the window server buffer.

@return Handle ID for the previous sibling, or zero if no previous sibling exists. 
@see NextSibling() 
@see RWindow::Construct() */
	{
	return(WriteReply(EWsWinOpPrevSibling));
	}

EXPORT_C TUint32 RWindowTreeNode::NextSibling() const
/** Gets the next window after this one in its sibling list. 

The return value is the client handle that was passed in the next sibling 
window's Construct() function: see RWindow::Construct() for a description of the 
client handle.
 
This function always causes a flush of the window server buffer.

@return Window handle of next sibling, or 0 if no next sibling exists. 
@see PrevSibling()
@see Child()
@see Parent() */
	{
	return(WriteReply(EWsWinOpNextSibling));
	}

EXPORT_C TUint32 RWindowTreeNode::Child() const
/** Gets the first child of the node.
 
This function always causes a flush of the window server buffer.

@return The client handle of the child node that currently has ordinal position 
0. This is 0 if there isn't a child. */
	{
	return(WriteReply(EWsWinOpChild));
	}

EXPORT_C void RWindowTreeNode::__DbgTestInvariant() const
/** In debug builds, this function always causes a flush of the window 
server buffer. */
	{
#if defined(_DEBUG)
	if (WriteReply(EWsWinOpTestInvariant))
		User::Invariant();
#endif
	}

EXPORT_C void RWindowTreeNode::SetOrdinalPosition(TInt aPos)
/** Sets the ordinal position of a window. 

A window's ordinal position is relative to its siblings with the same 
ordinal priority. The ordinal priority of displayable windows (in other words, 
not window groups) is almost always zero (the default). To set the ordinal priority 
as well as the ordinal position, use the other overload of this function. 

The lower the ordinal position, the nearer the window will be to the front of the z-order. 
The frontmost window has ordinal position zero. Specifying a negative value has the effect 
of sending the window to the back of the z-order.

Note: If this window is a window group in a chain, then all other window groups in the chain will be moved also.
When this function is called on Group Window with aPos set to KOrdinalPositionSwitchToOwningWindow then the 
function doesn't change the ordinal position of the group window, if instead it has focus then the window group 
that would come to the forground if it died will be moved to the foreground.

@param aPos The window's new ordinal position. The lower the ordinal, the closer to the 
front of the z-order the window will be. Specifying any negative value however, sends 
the window to the back of the z-order. */
	{
	WriteInt(aPos,EWsWinOpSetOrdinalPosition);
	}

EXPORT_C void RWindowTreeNode::SetOrdinalPosition(TInt aPos,TInt aOrdinalPriority)
/** Sets the ordinal position and ordinal priority of a window. 

Ordinal priority is a number assigned to a window that determines its position in the z-order. 
For sibling windows or group windows, the higher the priority, the closer it will be to the 
front. Ordinal priority overrides ordinal position, so that the ordinal position 
is only taken into account for sibling windows or window groups with the same ordinal priority. 
For a description of ordinal position, see the other overload of this function.

Most windows have an ordinal priority of zero. Only window groups are normally 
given non-default ordinal priorities.

To set priority of KPasswordWindowGroupPriority or greater on a window group, client will require 
SwEvent capability. If client does not have SwEvent capability then priority will be reduced 
to KPasswordWindowGroupPriority-1. This function doesn't return an error thus the client cannot 
tell if the priority is reduced, however, there is another function that can be used if the client 
does require an error, this is RWindowGroup::SetOrdinalPositionErr.

Note: If this window is a window group in a chain, then all other window groups in the chain will be moved also. 
And further they will all have their ordinal priority set to the specified value.
When this function is called on Group Window with aPos set to KOrdinalPositionSwitchToOwningWindow then the 
function doesn't change the ordinal position of the group window, if instead it has focus then the window group 
that would come to the forground if it died will be moved to the foreground.

@param aPos The window's new ordinal position. The lower the ordinal, the closer to the 
front of the z-order the window will be. Specifying any negative value however, sends 
the window to the back of the z-order.
@param aOrdinalPriority The window's new ordinal priority. */
	{
	TWsWinCmdOrdinalPos ordinalPos;
	ordinalPos.pos=aPos;
	ordinalPos.ordinalPriority=aOrdinalPriority;
	Write(&ordinalPos,sizeof(ordinalPos),EWsWinOpSetOrdinalPositionPri);
	}

EXPORT_C TInt RWindowTreeNode::OrdinalPriority() const
/** Gets the ordinal priority of the specified window.

This function was added for FEPs that need to know the priority in their dialogues. 

This function and RWsSession::GetWindowGroupOrdinalPriority() may return different 
values because this function isn't subject to any ordinal priority adjustment, 
while the window group ordinal priority may be.
 
This function always causes a flush of the window server buffer.

@return The ordinal priority of the specified window */
	{
	return(WriteReply(EWsWinOpOrdinalPriority));
	}

EXPORT_C TInt RWindowTreeNode::OrdinalPosition() const
/** Gets the current ordinal position of the window tree node. 

The ordinal position returned is the window's position amongst windows with 
the same parent (an with the same priority). Displayable windows almost always 
have the same priority, but window groups might typically have different priorities. 
If a window group's ordinal position among window groups of all priorities 
is required, use FullOrdinalPosition().

Note: all group windows (across all clients) have the same parent.
 
This function always causes a flush of the window server buffer.

@return The window's ordinal position. */
	{
	return(WriteReply(EWsWinOpOrdinalPosition));
	}

EXPORT_C TInt RWindowTreeNode::FullOrdinalPosition() const
/** Get the current full ordinal position of a window. 

This function normally returns a useful value only when called on a window 
group, because only window groups are normally given different priorities. 
For other types of window the value returned is usually the same as that returned 
by OrdinalPosition().
 
This function always causes a flush of the window server buffer.

@return The window's full ordinal position. */
	{
	return(WriteReply(EWsWinOpFullOrdinalPosition));
	}

/**Get the screen number that a window is located on

@return The screen number that the window is located on
 */
EXPORT_C TInt RWindowTreeNode::ScreenNumber() const
	{
	return(WriteReply(EWsWinOpScreenNumber));
	}

EXPORT_C TInt RWindowTreeNode::WindowGroupId() const
/** Returns the window group Id of the parent window group

If the window is a window group it will return it's Id. If it is not it scans 
up the window tree to find the group window from which this window is descended 
and returns it's Id.

This function always causes a flush of the window server buffer.

@return The window group Id of the window group from which this window is descended. */
	{
	return(WriteReply(EWsWinOpWindowGroupId));
	}

EXPORT_C TInt RWindowTreeNode::EnableOnEvents(TEventControl aCircumstances)
/** Requests notification of 'on' events. 'On' events are of type EEventSwitchOn.
 
This function always causes a flush of the window server buffer.

@param aCircumstances The circumstances in which 'on' events are to be reported. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see DisableOnEvents() */
	{
	return(WriteReplyInt(aCircumstances,EWsWinOpEnableOnEvents));
	}

EXPORT_C void RWindowTreeNode::DisableOnEvents()
/** Cancels notification of 'on' events. 

This function instructs the server to stop reporting 'on' events to this window. 
If the window server has not previously been instructed to report 'on' events, 
this method has no effect (i.e. the default is that windows do not get the 
events).

@see EnableOnEvents() */
	{
	if (iWsHandle)
		Write(EWsWinOpDisableOnEvents);
	}

EXPORT_C TInt RWindowTreeNode::EnableGroupChangeEvents()
/** Requests notification of group-change events.

Use this function to instruct the window server to report group-change events 
to this window. These events will typically be of interest to a shell or similar 
application, for example to notify it that it should update its list of running 
applications. Window group changed events are of type EEventWindowGroupsChanged.
 
This function always causes a flush of the window server buffer.

@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see DisableGroupChangeEvents() */
	{
	return(WriteReply(EWsWinOpEnableGroupChangeEvents));
	}

EXPORT_C void RWindowTreeNode::DisableGroupChangeEvents()
/** Cancels notification of group changed events. 

Use this function to instruct the server to stop reporting window group changed 
events to this window. If the window server has not previously been instructed 
to report window group changed events, this function has no effect (i.e. the 
default is that windows do not get the events).

@see EnableGroupChangeEvents() */
	{
	if (iWsHandle)
		Write(EWsWinOpDisableGroupChangeEvents);
	}

EXPORT_C TInt RWindowTreeNode::EnableFocusChangeEvents()
/** Enables focus changed events.

After this function is called, the EEventFocusGroupChanged event is delivered 
to the window server message queue every time the focus window group changes. 
The handle of the event is set to the client handle of the window that this 
function is called on.
 
This function always causes a flush of the window server buffer.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see DisableFocusChangeEvents() */
	{
	return(WriteReply(EWsWinOpEnableFocusChangeEvents));
	}

EXPORT_C void RWindowTreeNode::DisableFocusChangeEvents()
/** Disables delivery of focus changed events.

Use this function to instruct the server to stop reporting window group focus 
changed events to this window. If the window server has not previously been 
instructed to report window group changed events, this function has no effect 
(i.e. the default is that windows do not get the events).

@see EnableFocusChangeEvents() */
	{
	if (iWsHandle)
		Write(EWsWinOpDisableFocusChangeEvents);
	}

EXPORT_C TInt RWindowTreeNode::EnableGroupListChangeEvents()
/** Enables reporting of window group list change events.

The window group list is a list of all window groups and their z-order. Calling 
this function will cause notification events (of type EEventWindowGroupListChanged) 
for any change in the window group list: additions, removals and reorderings.

This function is useful when you need to know about changes to window groups 
beneath the focused one, for instance when the screen is showing windows from 
more than one window group at the same time.

The handle of the event is set to the client handle of the window that this 
function is called on.
 
This function always causes a flush of the window server buffer.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	return(WriteReply(EWsWinOpEnableGroupListChangeEvents));
	}

EXPORT_C void RWindowTreeNode::DisableGroupListChangeEvents()
/** Disables reporting of window group list change events.

This function instructs the window server to stop sending window group list 
change events to this window. If the window server has not previously been 
instructed to report window group list change events, this function has no 
effect (i.e. the default is that windows do not receive group list change 
events). */
	{
	if (iWsHandle)
		Write(EWsWinOpDisableGroupListChangeEvents);
	}

EXPORT_C TInt RWindowTreeNode::EnableVisibilityChangeEvents()
/** Enables reporting of window visibility change events.

The window visibility is based on whether or not any area of the window can be seen
on the screen.  This can be affected by SetVisible(), but also by other windows in
front of this one, and by the presence of transparent windows which it can be seen
through.  Calling this function will cause notification events (of type
EEventWindowVisibilityChanged) for any change in the window's visibility.

This function is useful when you are performing graphical processing such as animations
and would like to stop them while they cannot be seen, for efficiency reasons.

The handle of the event is set to the client handle of the window that this 
function is called on.
 
This function always causes a flush of the window server buffer.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{
	return(WriteReply(EWsWinOpEnableVisibilityChangeEvents));
	}

EXPORT_C void RWindowTreeNode::DisableVisibilityChangeEvents()
/** Disables reporting of window visibility change events.

This function instructs the window server to stop sending window visibility 
change events to this window. If the window server has not previously been 
instructed to report window visibility change events, this function has no 
effect (i.e. the default is that windows do not receive visibility change 
events). */
	{
	if (iWsHandle)
		Write(EWsWinOpDisableVisibilityChangeEvents);
	}

EXPORT_C TInt RWindowTreeNode::EnableErrorMessages(TEventControl aCircumstances)
/** Requests notification of error message events. 

Use this function to instruct the window server to report error message events 
(of type EEventErrorMessage).
 
This function always causes a flush of the window server buffer.

@param aCircumstances The circumstances in which error message events are 
to be reported. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see DisableErrorMessages() */
	{
	return(WriteReplyInt(aCircumstances,EWsWinOpEnableErrorMessages));
	}

EXPORT_C void RWindowTreeNode::DisableErrorMessages()
/** Cancels notification of error message events. 

Use this function to instruct the server to stop reporting error message events 
to this window. If the window server has not previously been instructed to 
report error message events, this function has no effect (i.e. the default 
is that windows do not get error messages).

@see EnableErrorMessages() */
	{
	if (iWsHandle)
		Write(EWsWinOpDisableErrorMessages);
	}

EXPORT_C TInt RWindowTreeNode::EnableModifierChangedEvents(TUint aModifierMask, TEventControl aCircumstances)
/** Requests notification of modifier changed events. 

Use this function to instruct the window server to report modifier changed 
events to this window. Values for the modifier keys are defined in TEventModifier. 
If more than one modifier key is to be monitored, their values should be combined 
using a bit-wise OR operation. Modifier changed events are of type EEventModifiersChanged.
 
This function always causes a flush of the window server buffer.

@param aModifierMask The modifiers to be reported. May be a combination of 
values defined in TEventModifier 
@param aCircumstances The circumstances in which modifier changed events are 
to be reported. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see DisableModifierChangedEvents() */
	{
	TWsWinCmdEnableModifierChangedEvents params(aModifierMask, aCircumstances);
	return(WriteReply(&params,sizeof(params),EWsWinOpEnableModifierChangedEvents));
	}

EXPORT_C void RWindowTreeNode::DisableModifierChangedEvents()
/** Cancels notification of modifier changed events. 

Use this function to instruct the server to stop reporting modifier changed 
events to this window. If the window server has not previously been instructed 
to report modifier changed events, this function has no effect (i.e. the default 
is that windows do not get the events).

@see EnableModifierChangedEvents() */
	{
	if (iWsHandle)
		Write(EWsWinOpDisableModifierChangedEvents);
	}

EXPORT_C TInt RWindowTreeNode::SetPointerCursor(TInt aCursorNumber)
/** Sets the pointer cursor from the system pointer cursor list. 

Use this function to set the current cursor to one contained in the system 
pointer cursor list. If the list does not contain a cursor with an index of 
aCursorNumber, the function will attempt to set the cursor to the default 
system pointer cursor, which has an index of 0 in the list.

The RWsSession class provides functions for setting and controlling the system 
pointer cursor list.
 
This function always causes a flush of the window server buffer.

@param aCursorNumber The cursor index in the system pointer cursor list. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see ClearPointerCursor() */
	{
	return(WriteReplyInt(aCursorNumber,EWsWinOpSetPointerCursor));
	}

EXPORT_C void RWindowTreeNode::SetCustomPointerCursor(const RWsPointerCursor &aPointerCursor)
/** Sets the pointer cursor to an application-defined cursor.

@param aPointerCursor The cursor to use. */
	{
	WriteInt(aPointerCursor.WsHandle(),EWsWinOpSetCustomPointerCursor);
	}

EXPORT_C void RWindowTreeNode::SetNonFading(TBool aNonFading)
/** Sets whether a window is non-fading.

When the non-fading flag is set the window will unfade if needed and remain 
unfaded until this flag is removed. This is useful for toolbars etc. which 
must never be faded.

@param aNonFading ETrue to set the non-fading flag, EFalse (the default) to 
re-set it. */
	{
	WriteInt(aNonFading,EWsWinOpSetNonFading);
	}

EXPORT_C void RWindowTreeNode::SetFaded(TBool aFaded,TFadeControl aIncludeChildren)
/** Sets the window as faded or unfaded.

This function allows a single window to be faded or unfaded. The function 
also allows the same action to be applied to all of the window's children.

Notes:

A redraw is required to un-fade a window because information is lost during 
fading (blank and backup windows deal with this themselves). Areas in shadow 
when the window is faded also require a redraw. 

While a window is faded, all drawing to that window will be adjusted appropriately 
by the window server.

@param aFaded ETrue to fade the window, EFalse to un-fade it. 
@param aIncludeChildren Fade control flags. These set whether fading/un-fading 
also apply to child windows. */
	{
	TWsWinCmdSetFaded params(aFaded,aIncludeChildren);
	Write(&params,sizeof(params),EWsWinOpSetFade);
	}

EXPORT_C void RWindowTreeNode::SetFaded(TBool aFaded,TFadeControl aIncludeChildren,TUint8 aBlackMap,TUint8 aWhiteMap)
/** Sets one or more windows as faded or unfaded, specifying a fading map. 

Fading is used to change the colour of a window to make other windows stand 
out. For example, you would fade all other windows when displaying a dialogue. 

Fading makes a window closer to white or closer to black.
Setting the fading map allows you to over-ride the default fading parameters 
set in RWsSession::SetDefaultFadingParameters(). 

The white and black fading values define the range over which colours are 
re-mapped when a window is faded. As the values get closer together, all colours 
in the faded window becomes more similar, creating the fading effect. 
When the numbers cross over (so that the black value is greater than the white 
value) the colours in the faded window start to invert, i.e. colours that 
were closer to white in the unfaded window are mapped to a darker colour when 
the window is faded.

The function also allows the fading action applied to this window to be applied 
to all of its children.

Notes:

Fading a window for a 2nd time will not change the fading map used. 

A redraw is required to un-fade a window because information is lost during 
fading. Note that blank (RBlankWindow) and backup (RBackedUpWindow) windows 
deal with this themselves. Areas in shadow when the window is faded also require 
a redraw. 

While a window is faded all drawing to that window will be adjusted appropriately 
by the window server.

@param aFaded ETrue to fade the window, EFalse to un-fade it. 
@param aIncludeChildren Fade control flags. This sets whether fading/un-fading 
is also to apply to all child windows. 
@param aBlackMap Black map fading parameter.
@param aWhiteMap White map fading parameter. 
@see RWsSession::SetDefaultFadingParameters()
@see CWindowGc::SetFadingParameters() */
	{
	TWsWinCmdSetFaded params(aFaded,aIncludeChildren,EFalse,aBlackMap,aWhiteMap);
	Write(&params,sizeof(params),EWsWinOpSetFade);
	}

EXPORT_C void RWindowTreeNode::ClearPointerCursor()
/** Clears pointer cursor settings.

These are the settings made by calling SetPointerCursor(). */
	{
	Write(EWsWinOpClearPointerCursor);
	}

/** Returns the window server session that is used to create this window handle. 

This functions returns null if the window handle is not initialised.

@see RWindowTreeNode(RWsSession &aWs)
*/
EXPORT_C RWsSession* RWindowTreeNode::Session() const
	{
	return iBuffer? iBuffer->Session() : NULL;
	}

/** @panic TW32Panic 17 in debug builds if called on an already constructed object.*/
TInt RWindowBase::construct(const RWindowTreeNode &parent,TUint32 aClientHandle, TInt aType, TDisplayMode aDisplayMode)
	{
	__ASSERT_DEBUG(iWsHandle == KNullHandle, Panic(EW32PanicGraphicDoubleConstruction));
	TWsClCmdCreateWindow createWindow;
	createWindow.parent=parent.WsHandle();
	createWindow.clientHandle=aClientHandle;
	createWindow.type=(TWinTypes)aType;
	createWindow.displayMode=aDisplayMode;
	TInt ret=iBuffer->WriteReplyWs(&createWindow,sizeof(createWindow),EWsClOpCreateWindow);
	if (ret<0)
		return(ret);
	iWsHandle=ret;
	return(KErrNone);
	}

EXPORT_C void RWindowBase::Activate()
/** Displays the window and enables it to receive events. 

Calling this method on a window causes it to receive a redraw event 
if it is a non-backed-up window, and should be called after a window has 
been constructed and initialised.

Windows are not displayed automatically when they are constructed. This allows 
them to be customised using SetPosition(), SetOrdinalPosition(), SetExtent(), 
etc., before they are displayed. */
	{
	Write(EWsWinOpActivate);
	}

EXPORT_C TPoint RWindowBase::Position() const
/** Gets a window's position relative to its parent.
 
This function always causes a flush of the window server buffer.

@return Position of this window's origin relative to the origin of its parent. */
	{
	TPckgBuf<TPoint> pntPkg;
  	WriteReplyP(&pntPkg,EWsWinOpPosition);
	return(pntPkg());
	}

EXPORT_C TPoint RWindowBase::AbsPosition() const
/** Gets a window's absolute position - ie the windows position relative 
to the current screen size mode.
 
This function always causes a flush of the window server buffer.

@return Position of this window's origin relative to the screen. */
	{
	TPckgBuf<TPoint> pntPkg;
  	WriteReplyP(&pntPkg,EWsWinOpAbsPosition);
	return(pntPkg());
	}

EXPORT_C TSize RWindowBase::Size() const
/** Gets the window's current size.
 
This function always causes a flush of the window server buffer.

@return Current size of window. */
	{
	TPckgBuf<TSize> sizePkg;
  	WriteReplyP(&sizePkg,EWsWinOpSize);
	return(sizePkg());
	}

/**
@internalAll
Disclaimer - this API is internal and is subject to change
@deprecated */
EXPORT_C TSize RWindowBase::SizeForEgl() const
    {
    return Size();
    }

EXPORT_C void RWindowBase::SetPosition(const TPoint &aPos)
/** Sets the position of a window relative to its parent. 

The co-ordinates given in aPos specify the position of the top left-hand 
corner of the window, relative to the top left-hand corner of its parent. 
A positive value indicates a direction to the right and down. Negative values 
are valid but will cause part of the window to be outside its parent's extent, 
and therefore clipped.

This function may be called at any time after the window's Construct() function: 
the window's position will change dynamically.

A window's position can also be set using the RWindow::SetExtent() and RWindowBase::SetExtentErr() 
functions.

Note: upon creation, a window's extent is the same as its parent’s. In other words 
it has the same origin and size. If the window’s parent is a group window, it is 
initialised to be full screen.

@param aPos The position of the window's origin, relative to its parent */
	{
	WritePoint(aPos,EWsWinOpSetPos);
	}

EXPORT_C TInt RWindowBase::SetSizeErr(const TSize &aSize)
/** Sets the size of a backed-up window. 

A window's size is not constrained by the size of its parent. However, its 
visible region is, and the child window's visible region will always be clipped 
to the parent's visible region.

Avoid using this function for a window known to be of type RBlankWindow or 
RWindow (i.e. not a backed-up window). Instead, use SetSize(), which is more 
efficient as it does not return a value. However, if the window is a backed-up 
window, or of unknown type, SetSizeErr() should be used, because 
setting the size of a backed-up window may cause an out-of-memory error.

This function may be called at any time after the window's Construct() function: 
the window's size will change dynamically.
 
This function always causes a flush of the window server buffer.

@param aSize Window size.
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see RWindow::SetSize()
@see RWindow::SetExtent()
@see RWindowBase::SetExtentErr() */
	{
	return(WriteReply(&aSize,sizeof(aSize),EWsWinOpSetSizeErr));
	}

EXPORT_C TInt RWindowBase::SetExtentErr(const TPoint &pos,const TSize &size)
/** Sets a backed-up window's extent, relative to its parent, and returns an error 
code from the server. 

See SetPosition() and SetSizeErr() for a description of the rules applying 
to aPoint and aSize respectively.

Avoid using this function for a window of type RBlankWindow or RWindow (in other words, 
not a backed-up window). Instead, use SetExtent(), which is more efficient 
as it does not return a value. However, if the window is a backed-up window, 
or of unknown type, SetExtentErr() should be used, because setting 
the extent of a backed-up window may cause an out-of-memory error.

This function may be called at any time after the window's Construct() function: 
the window's extent will change dynamically.
 
This function always causes a flush of the window server buffer.

@param pos The position of the window's origin, relative to its parent. 
@param size Window size. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	TWsWinCmdSetExtent setExtent(pos,size);
	return(WriteReply(&setExtent,sizeof(setExtent),EWsWinOpSetExtentErr));
	}

EXPORT_C TPoint RWindowBase::InquireOffset(const RWindowTreeNode &aWindow) const
/** Enquires the offset between this and another window. 

A positive value indicates a position to the right and down from aWindow, 
a negative value indicates a position to the left and up.
 
This function always causes a flush of the window server buffer.

@param aWindow Another window tree node. 
@return The offset of this window relative to aWindow. */
	{
	TPckgBuf<TPoint> pkgPoint;
	TUint32 handle=aWindow.WsHandle();
	WriteReplyP(&handle,sizeof(handle),&pkgPoint,EWsWinOpInquireOffset);
	return(pkgPoint());
	}

EXPORT_C void RWindowBase::PointerFilter(TUint32 aFilterMask, TUint32 aFilter)
/** Sets the filter which controls which pointer events are sent to the client 
session. 

A pointer filter can be defined for each window separately, and changed dynamically. 
The default behaviour when a window is created is that move, drag, enter and 
exit events are filtered out and not delivered to the client.

@param aFilterMask Bitwise OR of values from TPointerFilter masking event 
types which will be updated. 
@param aFilter Bits containing new values for the masked event types. A 1 bit 
causes the corresponding event to be filtered out, a 0 bit lets through the 
corresponding event. 
@see TPointerFilter */
	{
	TWsWinCmdPointerFilter params;
	params.mask=aFilterMask;
	params.flags=aFilter;
	Write(&params,sizeof(params),EWsWinOpPointerFilter);
	}

EXPORT_C void RWindowBase::SetPointerCapture(TInt aFlags)
/** Sets the pointer capture state.

A window which has called this function can capture events that would otherwise 
go to other windows. Normally, pointer events are sent to the window at 
the co-ordinates where the event occurs. Capturing only works on windows which 
are behind the capturing window in the z order. 

Capture can be enabled or disabled. If capturing is enabled a window will, 
by default, capture events only from windows in the same window group. A flag 
can be specified to allow it to capture events across all window groups.

Another flag can be used to specify drag-drop capture. This causes a drag-drop 
message to be sent to the window within which the pen was lifted. Drag-drop 
with a pen is a tap-drag-lift sequence.

Capture functionality is useful in situations where only the foreground window 
should receive events, e.g. in a modal dialogue.

@param aFlags Bitwise OR of flag values from TCaptureFlags. 
@see TCaptureFlags */
	{
	WriteInt(aFlags,EWsWinOpSetPointerCapture);		//The data is actually interpreted as a TUint
	}

EXPORT_C void RWindowBase::SetPointerGrab(TBool aState)
/** Allows or disallows pointer grabs in a window. 

If the pointer grab is set, any down event in this window will cause 
a pointer grab, terminated by the next corresponding up event. All pointer 
events up to and including the next up event will be sent to that window.

Pointer grab can be used for drag-and-drop or other situations when you want 
the events delivered to the same window even though the pen will be dragged 
outside that window. This function is typically called during window construction 
so that pointer grab is enabled for the lifetime of the window.

Pointer grab is disabled by default.

@param aState New state for the pointer grab setting. 
@see ClaimPointerGrab() */
	{
	WriteInt(aState,EWsWinOpSetPointerGrab);
	}

EXPORT_C TInt RWindowBase::ClaimPointerGrab(const TUint8, const TBool)
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(EFalse);
	return KErrNotSupported;
	}

EXPORT_C void RWindowBase::ClaimPointerGrab(TBool aSendUpEvent)
/** Claims the pointer grab from another window. 

If a pointer grab is already in effect in another window, a window can claim 
the pointer grab from that window by calling this function. All subsequent 
events will be delivered to this window, up to and including the next "up" 
event. This next up event terminates the pointer grab.

This function would typically be used where clicking in a window pops up another 
window, and where the popped-up window wishes to grab the pointer as though 
the original click had been in that window.

Note:

If aSendUpEvent is set to ETrue, the window losing the grab is immediately 
sent an up event, signalling the end of its pointer grab.

@param aSendUpEvent Whether to deliver an up event to the window that previously 
had the grab. 
@see SetPointerGrab() */
	{
	WriteInt(aSendUpEvent,EWsWinOpClaimPointerGrab);
	}

EXPORT_C void RWindowBase::SetPointerCapturePriority(TInt aPriority)
/** Sets the window's pointer capture priority.

To allow window gain a property that allows them to be clicked on even when they are behind a modal
window. The priority will be 0 by default. Windows that need to stop modal window in front of them, 
should set this value to positive. Window can only capture pointer events from another window 
of same group, if it has at least the same pointer capture priority.

@param aPriority Pointer capture priority */
	{
	WriteInt(aPriority,EWsWinOpSetPointerCapturePriority);
	}

EXPORT_C TInt RWindowBase::GetPointerCapturePriority() const
/** Gets the windows's pointer capture priority

@return Window's pointer capture priority 
@see SetPointerCapturePriority() */
	{
	return WriteReply(EWsWinOpGetPointerCapturePriority);
	}

EXPORT_C void RWindowBase::SetVisible(TBool aState)
/** Sets the window's visibility. 

This function can be called after the window has been created to dynamically 
change its visibility.

Notes:

When a window is invisible, it receives no events from the window server.

A window is invisible before it is activated, irrespective of the state of 
its visibility flag.

@param aState New value for the visibility. */
	{
	WriteInt(aState,EWsWinOpSetVisible);
	}

EXPORT_C void RWindowBase::SetShadowHeight(TInt aHeight)
/** Sets a window's shadow height.

This is the height that this window is above the window immediately behind 
it. The offset of the shadow's bottom right corner from the window's bottom 
right corner is the product of the window's shadow height and the system shadow 
vector. 

A shadow is only displayed on top-level windows.

@param aHeight Shadow height. The default is 0. */
	{
	WriteInt(aHeight,EWsWinOpSetShadowHeight);
	}

EXPORT_C void RWindowBase::SetShadowDisabled(TBool aState)
/** Enables or disables whether a window casts a shadow. 

By default, when a window is created, its shadow is not disabled. (But note 
that a shadow is only cast by top-level windows.)

@param aState ETrue if the shadow is to be disabled (not displayed). EFalse 
if the shadow is to be displayed */
	{
	WriteInt(aState,EWsWinOpShadowDisabled);
	}

EXPORT_C TInt RWindowBase::SetCornerType(TCornerType aCornerType, TInt aCornerFlags)
/** Sets the shape of a window's corners.
 
This function always causes a flush of the window server buffer.

@param aCornerType Corner type to apply. 
@param aCornerFlags Bitwise OR of flags indicating corners to exclude. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see TCornerFlags 
@deprecated */
	{
	TWsWinCmdSetCornerType params;
	params.type=aCornerType;
	params.flags=aCornerFlags;
	return(WriteReply(&params,sizeof(params),EWsWinOpSetCornerType));
	}

EXPORT_C TInt RWindowBase::SetShape(const TRegion &aRegion)
/** Sets a window's shape arbitrarily, if rectangular windows are not required.
 
This function always causes a flush of the window server buffer.

@param aRegion Region defining window shape. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@deprecated */
	{
	__ASSERT_DEBUG(!aRegion.CheckError(),Panic(EW32PanicInvalidRegion));
	const TInt regionCount=aRegion.Count();
	TPtrC8 ptrRect(reinterpret_cast<const TUint8*>(aRegion.RectangleList()),regionCount*sizeof(TRect));
	return(WriteReplyByProvidingRemoteReadAccess(&regionCount,sizeof(regionCount),&ptrRect,EWsWinOpSetShape));
	}

EXPORT_C TInt RWindowBase::SetRequiredDisplayMode(TDisplayMode aMode)
/** Sets the window's display mode. 

Each window can be given an individual display mode because some windows may 
require a greater number of different colours than others.

The default display mode of a window is dependent on the individual target 
device. The default mode may not reflect the full capabilities of the display. 
On some devices a simpler colour mode may be used as the default in order 
to save battery power. In these cases, this method may be used to request 
a different display mode on a per-window basis.

At any given time, the window server uses the highest display mode of the 
windows currently visible, and switches the hardware into that mode. Therefore, 
if an application contains a window with display mode EGray16, but only EGray4 
windows are visible, the screen hardware will use EGray4 until the EGray16 
window becomes visible.
 
This function always causes a flush of the window server buffer.

@param aMode The display mode: can have any TDisplayMode value except TDisplayMode::ERgb 
(since this isn't a mode that screen hardware can ever take). 
@return The display mode to which the window has been set. This is the closest 
display mode to aMode supported by the hardware. */
	{
	return(WriteReplyInt(aMode,EWsWinOpRequiredDisplayMode));
	}

EXPORT_C TDisplayMode RWindowBase::DisplayMode() const
/** Gets the window's current display mode.
 
This function always causes a flush of the window server buffer.

@return The window's current display mode. */
	{
	return((TDisplayMode)WriteReply(EWsWinOpGetDisplayMode));
	}

EXPORT_C void RWindowBase::EnableBackup(TUint aBackupType/*=EWindowBackupAreaBehind*/)
/** Requests that this window backs up all or part of the screen's contents. There 
are two backup possibilities:- just the area behind this window (the default), 
or the whole screen. Backing up the whole screen is useful for windows that 
require the rest of the screen to fade when they are displayed. In this case, 
the full screen backup is of the unfaded content of the screen.

It is possible to specify both types of backup at the same time. This may 
be used by fade behind windows that can be dragged across the screen, e.g. 
a dialog. This is done by calling this function with the parameter EWindowBackupAreaBehind|EWindowBackupFullScreen.

When backing up the whole screen, this function should be called before the 
window is activated, and before you call fade behind on it (RWindowBase::FadeBehind()).

Backing up a window is an optimisation feature that is honoured only if there 
is enough memory to do so, (it requires bitmaps and a region to be created 
and maintained). If there is not enough memory, all or part of the backup 
will be lost. In this case, a redraw will be issued at the relevant point 
just as if the window had not been backed up.

The backup bitmap is made and is claimed by the window not when EnableBackup() 
is called, but when the window becomes visible (this is normally when the 
window is activated).

Only one backed up window of the same type can exist at any one time (although 
the same window can have both types of backup at the same time). If a window 
requests a backup of type EWindowBackupAreaBehind, any window that has already 
claimed a backup of this type will lose it. If a window requests a backup 
of type EWindowBackupFullScreen, this request will fail if another window 
has already claimed a backup of this type.

@param aBackupType The type of backed up window. See the TWindowBackupType 
enum for possible values.
@see TWindowBackupType */
	{
	WriteInt(aBackupType,EWsWinOpEnableBackup);
	}

EXPORT_C void RWindowBase::RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32 aTime,const TRect &aRect)
/** Requests a pointer repeat event. 

This function instructs the window server to send a single pointer repeat event if 
the pointer state that caused the last event (e.g. EButton1Down) has not changed 
within aTime and the pointer has not moved outside aRect. Pointer repeat events 
are pointer events of type TPointerEvent::EButtonRepeat.

While the repeat is in operation all move and drag events within the rectangle 
are filtered out. The repeat is cancelled if the pointer moves outside the 
rectangle or generates any other pointer event.

A typical use of this function would be for a scrollbar, so that holding down 
the pointer results in a continuous scroll.

@param aTime Time interval before pointer repeat event should be sent. 
@param aRect Repeat event occurs only if pointer is within this rectangle. 
@see CancelPointerRepeatEventRequest() */
	{
	TWsWinCmdRequestPointerRepeatEvent params(aTime,aRect);
	Write(&params,sizeof(params),EWsWinOpRequestPointerRepeatEvent);
	}

EXPORT_C void RWindowBase::CancelPointerRepeatEventRequest()
/** Cancels a request for a pointer repeat event.

@see RequestPointerRepeatEvent() */
	{
	if (iWsHandle)
		Write(EWsWinOpCancelPointerRepeatEventRequest);
	}

EXPORT_C TInt RWindowBase::RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32 /*aTime*/,const TRect &/*aRect*/, const TUint8 /*aPointerNumber*/)
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	// No multipointer in WSERV NONNGA, so ignore pointer number by redirecting to appropriate API
	ASSERT(0);
	return KErrNotSupported;
	}

EXPORT_C TInt RWindowBase::CancelPointerRepeatEventRequest(const TUint8 /*aPointerNumber*/)
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	// No multipointer in WSERV NONNGA, so ignore pointer number by redirecting to appropriate API
	ASSERT(0);	
	return KErrNotSupported;
	}

EXPORT_C void RWindowBase::EnableAdvancedPointers()
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(0);	
	}

/**
@internalAll
Disclaimer - this API is internal and is subject to change
@deprecated */
EXPORT_C TInt RWindowBase::FixNativeOrientation()
    {
    return KErrNotSupported;
    }

EXPORT_C TInt RWindowBase::AllocPointerMoveBuffer(TInt aMaxNumPoints, TUint aFlags)
/** Allocates a buffer for storing pointer movements. 

The pointer move buffer is used by applications that need to process every 
single pointer move or drag event: for example, a freehand drawing application.

Normally, multiple drag events which the window server receives from the pointer 
device driver are translated into a single drag event. The single drag event 
incorporates all pointer events that occurred while the client was processing 
the previous pointer event. If the pointer move buffer is used, the window 
server stores all pointer events in a pointer buffer, and then delivers the 
entire buffer when it is full.

AllocPointerMoveBuffer() must be called before the pointer move buffer can 
be used. It would typically be called during window construction.

After the pointer move buffer has been allocated, the window server does not 
start putting pointer events into it until EnablePointerMoveBuffer() is called.

Note: move events are not available on all hardware.
 
This function always causes a flush of the window server buffer.

@param aMaxPoints Maximum number of pointer events the buffer can hold. This 
affects the frequency at which the buffer is flushed. 
@param aFlags Obsolete argument: set to zero always. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	TWsWinCmdAllocPointerMoveBuffer params;
	params.maxNumPoints=aMaxNumPoints;
	params.flags=aFlags;
	return(WriteReply(&params,sizeof(params),EWsWinOpAllocPointerMoveBuffer));
	}

EXPORT_C void RWindowBase::FreePointerMoveBuffer()
/** Frees the pointer move buffer. 

This function should be called on a window which calls AllocPointerMoveBuffer(). */
	{
	if (iWsHandle)
		Write(EWsWinOpFreePointerMoveBuffer);
	}

EXPORT_C void RWindowBase::EnablePointerMoveBuffer()
/** Enables the pointer move buffer for receiving pointer move events. 

This function instructs the window server to begin putting pointer events 
into the pointer move buffer. AllocPointerMoveBuffer() must have previously 
been called, or a panic will occur.

As soon as the pointer buffer has at least one point in it, the window server 
sends an event of type EEventPointerBufferReady to the client, and the events 
can be retrieved from the pointer move buffer using RetrievePointerMoveBuffer().

Note: pointer move, drag and enter/exit events are not delivered to a window 
by default. An application using the pointer move buffer should use PointerFilter() 
to request that these events be delivered.

@see DisablePointerMoveBuffer() */
	{
	Write(EWsWinOpEnablePointerMoveBuffer);
	}

EXPORT_C void RWindowBase::DisablePointerMoveBuffer()
/** Instructs the window server to stop adding pointer events to the pointer move 
buffer. */
	{
	Write(EWsWinOpDisablePointerMoveBuffer);
	}

EXPORT_C TInt RWindowBase::RetrievePointerMoveBuffer(TDes8 &aBuf) const
/** Retrieves events from the pointer move buffer. 

Use this function to get pointer events from the pointer move buffer. This 
function should be called when an event has occurred of type EEventPointerBufferReady 
(defined in TEventCode).
 
This function always causes a flush of the window server buffer.

@param aBuf Buffer to store events retrieved from pointer move buffer 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	TInt maxPoints=aBuf.MaxSize()/sizeof(TPoint);
	return(WriteReplyP(&maxPoints,sizeof(maxPoints),&aBuf,EWsWinOpRetrievePointerMoveBuffer));
	}

EXPORT_C void RWindowBase::DiscardPointerMoveBuffer()
/** Discards all events in the pointer move buffer. 

The window server subsequently continues to put new pointer events into the 
pointer move buffer as usual (if the buffer is enabled). */
	{
	Write(EWsWinOpDiscardPointerMoveBuffer);
	}

EXPORT_C TInt RWindowBase::AddKeyRect(const TRect &aRect, TInt aScanCode, TBool aActivatedByPointerSwitchOn)
/** Adds an on-screen key rectangle. 

This function configures an area of the screen, given by aRect, to act as 
an on-screen key. Any subsequent pointer events within this area will be translated 
by the window server into key events with a scan code of aScanCode.

aActivatedByPointerSwitchOn indicates whether or not to generate a key event 
as a result of a pointer event that acts to switch the machine on.

Before v7.0, calling this function stopped the window from receiving pointer 
events (they were received as key events) and pointer events outside the specified 
key rectangle were discarded. From v7.0, pointer events outside the key rectangles 
are not discarded and may be handled.
 
This function always causes a flush of the window server buffer.

@param aRect Rectangle to act as an on-screen key, relative to the window 
origin 
@param aScanCode Scan code of key events generated by this on-screen key 
@param aActivatedByPointerSwitchOn If ETrue, a switch-on pointer event will 
produce a key event. If EFalse, a switch-on pointer event will not produce 
a key event. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see RemoveAllKeyRects() */
	{
	TWsWinCmdAddKeyRect params(aRect, aScanCode, aActivatedByPointerSwitchOn);
	return(WriteReply(&params,sizeof(params),EWsWinOpAddKeyRect));
	}

EXPORT_C void RWindowBase::RemoveAllKeyRects()
/** Removes all the on-screen keys that have been added to this window.

After this function has been called, all pointer events are delivered to the window, 
reversing the effect of AddKeyRect().

@see AddKeyRect() */
	{
	Write(EWsWinOpRemoveAllKeyRects);
	}

EXPORT_C TInt RWindowBase::PasswordWindow(TPasswordMode aPasswordMode)
/** Makes this window the password window.

Only one password window can exist concurrently. Another window may take over 
as the password window if either (a) the current password window calls this 
function with aPasswordMode=EPasswordCancel, or (b) the current password window 
is destroyed.
 
This function always causes a flush of the window server buffer.

@param aPasswordMode The type of password handling required. 
@return KErrNone if successful, KErrInUse if this function is called when another 
password window already exists, otherwise another of the system-wide error 
codes. 

@deprecated */
	{
	return(WriteReplyInt(aPasswordMode,EWsWinOpPasswordWindow));
	}

EXPORT_C void RWindowBase::FadeBehind(TBool aFade)
/** Sets whether or not all windows behind the current window, in the same window 
group, should be faded or unfaded. 

This function can be used to fade all windows used by an application when 
a dialogue is displayed.

Fading works on a count basis. Fading increases the fade count, while unfading 
decreases it. If the fade count is greater than zero the window will be drawn 
faded. Only when it drops back to zero will it stop being faded.

This functionality is used to support nested dialogues. When bringing up a 
dialogue the rest of the application windows are faded. If an option is selected 
to launch another dialogue, the original dialogue is faded (fade count 1) 
and the remaining windows have their fade count increased to 2. When the dialogue 
is closed the fade count is reduced by one, which displays the original dialogue, 
but the remaining windows remain faded. They are only displayed when the other 
dialogue is closed and their fade count is reduced to zero.

Note:

Information is lost when a window is faded, so a redraw is required to restore 
the window content when it is unfaded (blank and backup windows deal with 
this themselves). A redraw is also required for the areas that were in shadow 
when the window was faded, since the shadowing also causes information loss. 
While a window is faded all drawing to that window will be adjusted appropriately 
by the window server.

@param aFade ETrue to increase the fade count of all windows behind the current 
window (within the current window group), EFalse to reduce the fade count. 
@see RWindowTreeNode::SetFaded()
@see RWindowTreeNode::SetNonFading() */
	{
	WriteInt(aFade,EWsWinOpFadeBehind);
	}

EXPORT_C TBool RWindowBase::IsFaded() const
/** Tests whether the current window is faded.
 
This function always causes a flush of the window server buffer.

@return ETrue if the current window is faded, otherwise EFalse. */
	{
	return WriteReply(EWsWinOpGetIsFaded);
	}

EXPORT_C TBool RWindowBase::IsNonFading() const
/** Tests whether the current window is non-fading.
 
This function always causes a flush of the window server buffer.

@return ETrue if the current window is non-fading, otherwise EFalse. 
@see RWindowTreeNode::SetNonFading() */
	{
	return WriteReply(EWsWinOpGetIsNonFading);
	}

EXPORT_C TInt RWindowBase::MoveToGroup(TInt aIdentifier)
/** Moves this window to another window group. 

This function allows a window with a window group as its immediate parent 
to be moved from one window group to another one. The two window groups must 
be owned by the same client. The new parent window group is specified by its 
identifier.
 
This function always causes a flush of the window server buffer.

@param aIdentifier The identifier of the target window group. This is the 
value returned by RWindowGroup::Identifier(). 
@return KErrNone if successful, otherwise another of the system-wide error 
codes. An error is returned if an attempt is made to move the window between 
window groups in different clients. */
	{
	return WriteReplyInt(aIdentifier,EWsWinOpMoveToGroup);
	}

/** This sets the background of the window to be the given surface.

The surface will be stretched or compressed to fill the extent of the window.
The background is updated on screen when the window is next redrawn.

When the window is moved, the surface will move with it. If the window is
resized, the surface will be similarly scaled. If a 1-1 pixel mapping is
required and the window size changes, the function will need to be called
again with a different TSurfaceId for a surface with the new size of the
window.

The key color to use in chroma key composition mode is defined by the system,
for all TSurfaceId background windows to use. In alpha composition mode,
transparent black is used. The composition mode is determined by the screen
display mode: if the mode supports an alpha channel, alpha composition is used;
otherwise chroma key composition is used.

@param aSurface  The surface to act as the background of the window
@return KErrNone on success or a system-wide error code.
@pre aSurface has been initialized.
@pre The window is either an RWindow or an RBlankWindow, or the client is
panicked with the code EWservPanicDrawable.
@pre The surface must be compatible with being composited on the screen this
window appears on; otherwise the client thread is panicked with code
EWservPanicIncompatibleSurface.
@post The window has its background set to be the surface.
@post The window is opaque.
@post The base area of the window is the full extent of the window.
@post A GCE surface layer has been created to associate the surface with a
location on screen.
@post The surface's content is in an undefined state, but the surface remains
initialized.
@post This function always causes a flush of the window server buffer.

@publishedPartner
@prototype
*/
EXPORT_C TInt RWindowBase::SetBackgroundSurface(const TSurfaceId& aSurface)
	{
	(void) aSurface;	//avoid unreferenced warning
	return KErrNotSupported;
	}


/**
This sets a surface to appear in front of the window's background within a 
given area of that window. 

Any rendering performed by CWindowGc operations will appear in front of surface 
for the window. The TSurfaceConfiguration object contains the Surface ID and 
allows various surface presentation attributes to be specified. This describes 
the mapping from surface co-ordinates to screen co-ordinates, allowing for scaling, 
cropping, and rotation. 

For details on the attributes see TSurfaceConfiguration.

The composition mode is determined by the screen display mode: if the mode supports an 
alpha channel, alpha composition is used; otherwise chroma key composition is used. 
In chroma key composition mode, the key color used is defined by the system, for all 
TSurfaceId background windows to use. In alpha composition mode, transparent 
black is used.

If the same surface ID is already set as the window background surface, then only 
the configuration parameters will be updated. 

If the window already has a background surface (that is not same as the new surface) 
or then that surface will be removed and the new 
background surface will be set. The Surface ID will be registered while attached 
to this window. This is in addition to any call to RWsSession::RegisterSurface.

@param aConfiguration The set of configuration that applies to the surface.
@param aTriggerRedraw If set causes WServ to repaint any affected portions of the display.
@return KErrNone on success or any system-wide error code
	- KErrNotSupported if surface support is not available
	- KErrNoMemory If a memory allocation fault occurs
	- KErrArgument If the surface ID is not accepted by the GCE 
@pre The window is either a RWindow or an RBlankWindow, or the client is panicked 
with the code EWservPanicDrawable.
@pre The surface is opaque; otherwise results are not defined.
@pre All members of the TSurfaceConfiguration recognised by the server must have valid 
values. If not, the client is panicked with code EWservPanicInvalidSurfaceConfiguration.
@pre The surface must not be the UI surface; otherwise the client thread is panicked 
with code EWservPanicInvalidSurface.
@pre The surface must not be the null surface ID; otherwise the client thread is 
panicked with code EWservPanicInvalidSurface.
@post The window has a new background surface set to be within the given area. Outside 
the area, the window’s background color will be visible.
@post The window must be redrawn before the surface becomes visible, and the surface's 
content will be visible after the composition and refresh. Composition of the surface 
will be automatically triggered if required. The aTriggerRedraw flush parameter will 
cause this redraw, or the client should cause a redraw after this call.
@post This function always causes a flush of the WServ session buffer.
@see RemoveBackgroundSurface
@see GetBackgroundSurface

@publishedPartner
@prototype
*/
EXPORT_C TInt RWindowBase::SetBackgroundSurface(const TSurfaceConfiguration& aConfiguration, TBool aTriggerRedraw)
	{
	(void) aConfiguration;	//avoid unreferenced warning
	(void) aTriggerRedraw;	//avoid unreferenced warning
	return KErrNotSupported;
	}


/**
This removes any background surface that has been set to the window.

The surface ID registration associated with the window will be released. 
This is independent of any outstanding calls to RWsSession::RegisterSurface, 
which should be completed with a corresponding call to UnregisterSurface.

The aTriggerRedraw parameter triggers a redraw of at least the affected 
areas of the window at the end of this method. 

@param aTriggerRedraw If set causes WServ to repaint any affected portions of the display.
@post Any background surface associated with this window has been removed. The 
appearance on screen may not change until the window is redrawn and the next 
refresh occurs. The aTriggerRedraw parameter can be set to trigger this redrawing.
@post This function does not explicitly force a flush of the WServ session buffer. 
Internal reference counting will keep the Surface ID "live" until the client code 
has released any handles and provisioners, and WServ processes the buffered remove 
command, and the final frame containing this surface has finished being displayed.
@see SetBackgroundSurface
@see GetBackgroundSurface

@publishedPartner
@prototype
*/
EXPORT_C void RWindowBase::RemoveBackgroundSurface(TBool aTriggerRedraw)
	{
	(void) aTriggerRedraw;	//avoid unreferenced warning
	}


/**
Retrieves a copy of the current configuration for background surface attached to the window.

The client must present a properly initialized TSurfaceConfiguration on entry – in 
particular the size field must be valid before the call. The server will then fill 
the object with the available data.

If the server supports more fields it will truncate the returned data to the size the 
client has requested. If the server has fewer fields it will set the returned size 
field to the lower value. 

Note that the returned attribute values will only be guaranteed equivalent to the 
input values, not exact copies, as the values may be calculated from internal flags 
rather than reported from a cached exact copy.

@param aConfiguration
	- On entry the Size field specifies the maximum size of the object.
	- On return fields up to this size are filled in.
@return KErrNone on success or any system-wide error code
	- KErrNotFound The window does not have a background surface attached.
	- KErrNoMemory If a memory allocation fault occurs.
@pre The window is either a RWindow or an RBlankWindow, or the client is panicked 
with the code EWservPanicDrawable.
@pre The window has not been set as transparent, or the client is panicked with 
the code EWservPanicTransparencyMisuse.
@pre The Size member of the configuration must be an acceptable value. The size 
must match the Size() member of a defined TSurfaceConfiguration variant, or be 
larger than all variants known to the server. If not, the client is panicked with 
code EWservPanicInvalidSurfaceConfiguration.
@post aConfiguration object filled to specified size.
@post This function always causes a flush of the WServ session buffer.
@see SetBackgroundSurface
@see RemoveBackgroundSurface

@publishedPartner
@prototype
*/
EXPORT_C TInt RWindowBase::GetBackgroundSurface(TSurfaceConfiguration& aConfiguration) const
	{
	(void) aConfiguration;	//avoid unreferenced warning
	return KErrNotSupported;
	}

/**
Not supported in non-NGA
 */
EXPORT_C void RWindowBase::SetSurfaceTransparency(TBool aSurfaceTransparency)
    {
    (void) aSurfaceTransparency;
    }

/** This function returns the key color for the window, if used.

In chroma key composition mode, this will be an opaque color; in alpha
composition mode, it will be transparent black, i.e. TRgb(0, 0, 0, 0). If the
window does not have a surface or overlay background, the return value is
transparent black.

The value of a chroma key color depends on the background type. Windows with
surface backgrounds return the system defined key color, whereas windows with
overlay backgrounds return the color passed in.

If the return value is opaque, the client should take care to avoid using the
color when rendering further content in the window, since it may cause that
content to become invisible.

@return The key color used in this window, or transparent black.
@post This function always causes a flush of the window server buffer.

@publishedPartner
@prototype
*/
EXPORT_C TRgb RWindowBase::KeyColor() const
	{
	return KRgbBlack;
	}

/**	Dummy implementation in order to preserve compatibility with WSERV NGA.
	@internalComponent*/
EXPORT_C void RWindowBase::SetPurpose(TInt /*aPurpose*/)
	{
	ASSERT(0);
	}

/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */
EXPORT_C void RWindowBase::SendEffectCommand(TInt /*aTfxCmd*/,const TDesC8& /*aTfxCmdData*/)
    {
	ASSERT(0);
    }

/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */
EXPORT_C void RWindowBase::OverrideEffects(TInt /*aAction*/, const TFileName& /*aResourceDir*/, const TFileName& /*aFilenameOutgoing*/, const TFileName& /*aFilenameIncoming*/, TBitFlags /*aFlags*/)
    {
	ASSERT(0);
    }

//////////////////////////// RDrawableWindow ////////////////////////////////

void RDrawableWindow::doScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect, TInt aOpcode)
	{
	TWsWinCmdScroll scroll(aClipRect,aOffset,aRect);
	Write(&scroll,sizeof(scroll),aOpcode);
	}

EXPORT_C void RDrawableWindow::Scroll(const TPoint &aOffset)
/** Scrolls the window contents by an offset. 

All parts of an RWindow are either drawn with content copied from another 
part of the window or are invalidated. Areas are invalidated when the source 
content of the scroll would be outside the window or obscured behind another 
window. The areas that are invalidated are not blanked off.

For example, if the window is scrolled towards the right, then there is no 
content (from outside the window) to move into area on the left hand side. 
This area is invalidated. Similarly, no content can be scrolled out from under 
an obscuring window, so the area from beneath the obscuring window is also 
invalidated.

Note that for an RBackedUpWindow, the contents of areas obscured by other windows 
are stored. In this case the window contents are scrolled out from "underneath" 
the obscuring window. In the example above the area on the left was invalidated 
but for this type of window the area simply contains its old pre-scroll contents.

@param aOffset Scroll offset, in pixels. Positive values cause the window 
contents to move downwards/right. Negative values cause contents to move upwards/left. */
	{
	doScroll(TRect(), aOffset, TRect(), EWsWinOpScroll);
	}

EXPORT_C void RDrawableWindow::Scroll(const TRect &aClipRect,const TPoint &aOffset)
/** Scrolls the contents of a clip rectangle, independently of the other contents 
of the window.

This function behaves in exactly the same way as the single parameter overload, 
except that the scroll region is a clipping rectangle rather than the whole window. 
All parts of the clipping rectangle are either drawn with content copied from 
another part of the rectangle or are invalidated.

The RBackedUpWindow behaviour is also similar to the behaviour in the function 
above.

@param aClipRect Rectangle to which scrolling is to be clipped 
@param aOffset Scroll offset, in pixels. Positive values cause the window contents 
to move downwards and right. Negative values cause contents to move upwards 
and left. */
	{
	doScroll(aClipRect, aOffset, TRect(), EWsWinOpScrollClip);
	}

EXPORT_C void RDrawableWindow::Scroll(const TPoint &aOffset, const TRect &aRect)
/** Scrolls a rectangle within a window.

This function effectively moves the specified rectangle by the given offset 
with respect to the window. The destination rectangle may overlap and cover 
the old rectangle.

Note that if the source of this rectangle is an invalid area, ie it is obscured 
or lies outside the window, then this area in the destination rectangle will 
be invalidated.

@param aOffset Scroll offset, in pixels. Positive values cause the window 
contents to move downwards and right. Negative values cause contents to move 
upwards and left. 
@param aRect The source rectangle for the scroll. */
	{
	doScroll(TRect(), aOffset, aRect, EWsWinOpScrollRect);
	}

EXPORT_C void RDrawableWindow::Scroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect)
/** Scrolls the contents of a source rectangle within a clipping rectangle.

The source rectangle is effectively copied from one position to another, with 
both the source and destination clipped to the clipping rectangle. Hence if 
the source and destination rectangles are inside the clipping region then 
this function behaves exactly like the Scroll(const TPoint &aOffset, const TRect &aRect) 
overload. However if the source or destination for the scrolled rectangle intersect 
the clipping rectangle then the function behaves similarly to the 
Scroll(const TRect &aClipRect,const TPoint &aOffset) overload, with regards to 
invalidated regions etc.

@param aClipRect Rectangle to which scrolling is to be clipped.
@param aOffset Scroll offset, in pixels. Positive values cause the window contents 
to move downwards and right. Negative values cause contents to move upwards 
and left. 
@param aRect Source rectangle for the scroll. */
	{
	doScroll(aClipRect, aOffset, aRect, EWsWinOpScrollClipRect);
	}

/**Protected system function.

Sets the current redraw reference rectangle that is being drawn. 
This is called between a BeingRedraw() / EndRedraw() pair by the system.

@param aRect Rectangle reference area that is currently being drawn.
@see GetDrawRect() */
void RDrawableWindow::SetDrawRect(const TRect &aRect)
	{
	iDrawRect = aRect;
	}

/**Obtains the current rectangle being drawn to this window, during a window redraw cycle.

This function can be called between a BeginRedraw() / EndRedraw() pair to obtain 
the current window redraw reference rectangle.

@return The current reference rectangle that is being redrawn*/
EXPORT_C TRect RDrawableWindow::GetDrawRect() const
	{
	return iDrawRect;
	}

EXPORT_C void RWindow::BeginRedraw(const TRect &aRect)
/** Begins the redraw of a rectangle within the window's invalid region.

This method tells the window server that the window is about to respond to the 
last redraw event by redrawing the specified rectangle. This causes the window 
server to clear the rectangle, and remove it from the invalid region. 

After the redraw is complete the window should call EndRedraw().

Note:

When handling a redraw event, this rectangle would typically be the rectangle 
returned by TWsRedrawEvent::Rect().

The redraw is clipped to the area that is validated, i.e. the intersection 
of the rectangle with the previously invalid region.

If you only validate part of the rectangle given in the redraw event then, 
after EndRedraw() is called, drawing will be clipped to the visible area which 
is not invalid. This is because drawing (that is non-redrawing) is always 
clipped to the visible region less the invalid region. You will get another 
message telling you to redraw the area that is still invalid. 

@param aRect The rectangle to be redrawn. 
@see EndRedraw() */
	{
	WriteRect(aRect,EWsWinOpBeginRedraw);
	SetDrawRect(aRect);
	}

EXPORT_C void RWindow::BeginRedraw()
/** Begins redrawing the window's invalid region. 

This method tells the window server that the window is about to respond to 
the last redraw event by redrawing the entire invalid region. This causes 
the window server to validate the entire invalid region. 

After the redraw is complete the entire region that was previously invalid 
is validated. The window should then call EndRedraw().

Note: the redraw is clipped to the region that was previously invalid.

This function always causes a flush of the window server buffer.

@see EndRedraw() */
	{
	Write(EWsWinOpBeginRedrawFull);
	SetDrawRect(TRect(Size()));
	}

EXPORT_C void RWindow::EndRedraw()
/** Ends the current redraw. 

This function should be called when redrawing is complete. */
	{
	Write(EWsWinOpEndRedraw);
	SetDrawRect(TRect::EUninitialized);
	}

EXPORT_C void RWindow::Invalidate(const TRect &aRect)
/** Invalidates an area within the window. 

This function invalidates the specified rectangle, which causes the window 
to get a redraw message. This allows an application-initiated redraw of a 
specified rectangle.

@param aRect Area to invalidate. */
	{
	WriteRect(aRect,EWsWinOpInvalidate);
	}

EXPORT_C void RWindow::Invalidate()
/** Invalidates the entire window. 

This function causes the window to get a redraw message specifying its entire 
visible area, allowing an application-initiated redraw. */
	{
	Write(EWsWinOpInvalidateFull);
	}

EXPORT_C void RWindow::GetInvalidRegion(RRegion &aRegion) const
/** Gets the invalid region.

Note: if there is not enough memory to create the region, the region's error flag 
will be set.
 
This function always causes a flush of the window server buffer.

@param aRegion On return, contains the invalid region. */
	{
	aRegion.Clear();
retry:
	TInt count=WriteReply(EWsWinOpGetInvalidRegionCount);
	if (count>0)
		{
		HBufC8 *rectBuf=NULL;
		TRAPD(err,rectBuf=HBufC8::NewMaxL(count*sizeof(TRect)));
		if (err==KErrNone)
			{
			TPtr8 des=rectBuf->Des();
			if (WriteReplyP(&count,sizeof(count),&des,EWsWinOpGetInvalidRegion))
				{
				User::Free(rectBuf);
				goto retry;
				}
			for(TInt index=0;index<count;index++)
				aRegion.AddRect(((TRect *)rectBuf->Ptr())[index]);
			User::Free(rectBuf);
			}
		else
			aRegion.ForceError();
		}
	}

EXPORT_C void RWindow::SetBackgroundColor(TRgb aColor)
/** Sets the background colour used for clearing in server-initiated redraws. 

The window will be cleared to its background colour when a window server-initiated 
redraw occurs. Background colour can be changed dynamically after a window 
has been created and activated, however, the new background colour will not 
be visible until the window has been redrawn.

@param aColor Background colour to be used during redraws. */
	{
	WriteInt(aColor.Internal(),EWsWinOpSetBackgroundColor);
	}

EXPORT_C void RWindow::SetBackgroundColor()
/** Sets the background colour used for clearing in server-initiated redraws to 
none. 

The window will not be cleared to its background colour when a window server-initiated 
redraw occurs. 

For a window on which SetTransparencyAlphaChannel() has been called, this function means
that the transparent window has no background.

If a client calls this function for an opaque window, it is their responsibility to provide
opaque drawing to every pixel in the window (for example, a background bitmap). Otherwise,
undefined behaviour will result. */
	{
	Write(EWsWinOpSetNoBackgroundColor);
	}

EXPORT_C TInt RWindow::Construct(const RWindowTreeNode &parent,TUint32 aClientHandle)
/** Completes the construction of the window handle. 

This method should be called after the RWindow() constructor, before any other 
functions are performed on the window. It creates a window in the window server 
corresponding to the RWindow object. The window is initialised to inherit 
the size and extent of its parent window, given by the first parameter. If its 
parent is a group window then it will be full screen.
 
This function always causes a flush of the window server buffer.

@param parent The window's parent. 
@param aClientHandle Client handle for the window. This is an integer value 
chosen by the client that must be unique within the current server session. The 
usual way of doing this is to cast the address of the object that owns the window 
to a TUint32; this allows event handlers which are given a window handle to obtain 
a reference to the window an event is intended for. For example, CCoeControl uses 
this technique when it constructs a window. Note that in GUI applications, every 
window is created and owned by a control. Therefore it is rare for 3rd party code 
to ever need to call a window's Construct() function directly.

@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	return(construct(parent,aClientHandle,EWinRedraw,ENone));
	}

EXPORT_C void RWindow::SetSize(const TSize &size)
/** Sets the size of a window. 

This function may be called at any time after the window's Construct() function: 
the window's size will change dynamically.

If the window size is increased, any new area will be cleared to the background 
colour and a redraw event will be generated for it.

@param size The window size. */
	{
	WriteSize(size,EWsWinOpSetSize);
	}

EXPORT_C void RWindow::SetExtent(const TPoint &pos,const TSize &size)
/** Sets the size and position of a window. 

This function may be called at any time after the window's Construct() function: 
the window's extent will change dynamically.

If the window size is increased, any new area will be cleared to the background 
colour and a redraw event will be generated for it.

@param pos The position of the window's origin, relative to its parent. 
@param size The window size. 
@see RBackedUpWindow */
	{
	TWsWinCmdSetExtent setExtent(pos,size);
	Write(&setExtent,sizeof(setExtent),EWsWinOpSetExtent);
	}

/** Enables or Disables the storing of redraw operations that do not 
intersect the viewport for a window.

The window server stores operations required to redraw a window in a redraw 
store. Calling this function with ETrue causes all such drawing operations 
to be stored. The EnableRedrawStore(EFalse) call serves as a hint to store 
only the subset of draw commands which intersect the viewport.

@param aEnabled Indicates whether the redraw store should be enabled or disabled. */
EXPORT_C void RWindow::EnableRedrawStore(TBool aEnabled)
	{
	WriteInt(aEnabled, EWsWinOpStoreDrawCommands);
	}

/** Enables/Disables the WSERV to use its OSB
@param bool value that dertermines whether to trun OSB on or off
*/
EXPORT_C void RWindow::EnableOSB(TBool aStatus)
	{
	if(aStatus)
		{
		Write(EWsWinOpEnableOSB);
		}
	else
		{
		Write(EWsWinOpDisableOSB);
		}
	}

/** Clears the draw commands that are stored for this window from the redraw store.
	Windows that have had their redraw store cleared are not ready to draw again until 
	a new set of draw commands enclosed by BeginRedraw/EndRedraw have been issued.
*/	
EXPORT_C void RWindow::ClearRedrawStore()
	{
	Write(EWsWinOpClearRedrawStore);
	}
	
EXPORT_C void RWindowGroup::SetOrdinalPriorityAdjust(TInt aAdjust)
/** Sets the window group's priority adjust value. 

This function is primarily designed for sessions that own multiple window 
groups. After this function has been called by a window group, that window 
group's priority will be adjusted by the amount given by aAdjust whenever 
another window group owned by the same session gains keyboard focus.

When the session loses focus, the priority returns to its original value.

Note: This is ignored for window groups in chains.

@param aAdjust Value to be added to window group's existing priority. */
	{
	WriteInt(aAdjust,EWsWinOpSetOrdinalPriorityAdjust);
	}

EXPORT_C TInt RWindowGroup::SetOrdinalPositionErr(TInt aPos,TInt aOrdinalPriority)
/** Sets the ordinal position and ordinal priority of a window.
 
Ordinal priority is a number assigned to a window that determines its position in the z-order. 
For a description of ordinal priority, see the RWindowTreeNode::SetOrdinalPosition function.
To set a priority of KPasswordWindowGroupPriority or greater, client will require 
SwEvent capability. If client does not have SwEvent capability then it will return error 
code.

Note: If this window is a window group in a chain, then all other window groups in the chain will be moved also. 
And further they will all have their ordinal priority set to the specified value.
When this function is called on Window with aPos set to KOrdinalPositionSwitchToOwningWindow then the function 
doesn't change the ordinal position of the window, if instead it has focus then the window that would come to 
the forground if it died will be moved to the foreground.

This function always causes a flush of the window server buffer.

@param aPos The window's new ordinal position. The lower the ordinal, the closer to the 
front of the z-order the window will be. Specifying any negative value however, sends 
the window to the back of the z-order.
@param aOrdinalPriority The window's new ordinal priority.
@return	if not successful, one of the system-wide error codes. */
	{
	TWsWinCmdOrdinalPos ordinalPos;
	ordinalPos.pos=aPos;
	ordinalPos.ordinalPriority=aOrdinalPriority;
	return(WriteReply(&ordinalPos,sizeof(ordinalPos),EWsWinOpSetOrdinalPositionErr));
	}

/**
Window transparency based on the use of a separate window mask bitmap has been 
deprecated. Calling this method method has no effect. For window transparency 
see the the support for window alpha channel.

@see RWindow::SetTransparencyAlphaChannel() 
@deprecated
*/
EXPORT_C void RWindow::HandleTransparencyUpdate()
	{
	// deprecated. this call does nothing. always return KErrNone.
	Write(EWsWinOpHandleTransparencyUpdate);
	}

/**
The support for window transparency factor has been deprecated. Calling this 
method has no effect. For window transparency see the the support for window 
alpha channel.

@param aTransparencyFactor Ignored.
@return	Always KErrNone.
@see RWindow::SetTransparencyAlphaChannel() 
@deprecated
*/
EXPORT_C TInt RWindow::SetTransparencyFactor(const TRgb& aTransparencyFactor)
	{
	// deprecated. this call does nothing. always return KErrNone.
	return WriteReplyInt(aTransparencyFactor.Internal(), EWsWinOpSetTransparencyFactor);
	}
	
/**
Window transparency based on the use of a separate window mask bitmap has been 
deprecated. Calling this method method has no effect. For window transparency 
see the the support for window alpha channel.

@param aTransparencyBitmap Ignored.
@return	Always KErrNone.
@see RWindow::SetTransparencyAlphaChannel() 
@deprecated
*/
EXPORT_C TInt RWindow::SetTransparencyBitmap(const CFbsBitmap& aTransparencyBitmap)
	{
	// deprecated. this call does nothing. always return KErrNone.
	return WriteReplyInt(aTransparencyBitmap.Handle(), EWsWinOpSetTransparencyBitmap);	
	}
	
/**
Window transparency based on the use of a separate window mask bitmap has been 
deprecated. Calling this method method has no effect. For window transparency 
see the the support for window alpha channel.

@param aTransparencyBitmap Ignored.
@return	Always KErrNone.
@see RWindow::SetTransparencyAlphaChannel() 
@deprecated
*/
EXPORT_C TInt RWindow::SetTransparencyWsBitmap(const CWsBitmap& aTransparencyBitmap)
	{
	// deprecated. this call does nothing. always return KErrNone.
	return WriteReplyInt(aTransparencyBitmap.WsHandle(), EWsWinOpSetTransparencyBitmapCWs);
	}	

/**
Window transparency based on the use of a separate window mask bitmap has been 
deprecated. Calling this method method has no effect. To guarantee a window being
opaque; see SetTransparentRegion().

@see RWindow::SetTransparentRegion()
@deprecated
*/
EXPORT_C void RWindow::SetNonTransparent()
	{}

/**
Enables the use of an alpha channel to control the window's transparency.

Note that the window's invalid area will be cleared using the window's 
background color before any drawing commence. Thus setting the background 
color to fully transparent is essential in most use cases.
   
@see RWindow::SetBackgroundColor()
@return	KErrNone if successful, otherwise one of the system-wide error codes. */
EXPORT_C TInt RWindow::SetTransparencyAlphaChannel()
	{
	return WriteReply(EWsWinOpSetTransparencyAlphaChannel);
	}

EXPORT_C TInt RWindow::SetTransparentRegion(const TRegion& aRegion)
/** Sets the user-defined transparent region of a window.

This will replace the default transparent region in a transparent window which is normally the full window 
base area.
The area outside of the transparent region will be treated by window server as being opaque. The client
is responsible for ensuring that opaque drawing is present in all pixels of this area. If the client 
fails to do this, undefined behaviour will result.
It is possible to make the transparent window completely opaque by passing an empty region. Passing a region
equal to the window base area or larger will revert the window to its default behaviour (i.e. full window
transparency).
No screen content changes will occur immediately as a result of calling this function.
 
This function applies to transparent window only and always causes a flush of the window server buffer.

@param aRegion User defined windows's transparent region. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see RWindow::SetTransparencyAlphaChannel() */
	{
	__ASSERT_ALWAYS(!aRegion.CheckError(),Panic(EW32PanicInvalidRegion));
	const TInt regionCount=aRegion.Count();
	TPtrC8 ptrRect(reinterpret_cast<const TUint8*>(aRegion.RectangleList()),regionCount*sizeof(TRect));
	return WriteReplyByProvidingRemoteReadAccess(&regionCount,sizeof(regionCount),&ptrRect,EWsWinOpSetTransparentRegion);
	}

EXPORT_C TInt RWindow::SetTransparencyPolicy(TWsTransparencyPolicy aPolicy)
/** Sets the transparency policy of a window.

No screen content changes will occur immediately as a result of calling this function.
 
This function applies to transparent window only.

@param aPolicy Transparent window policy. 
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@see TWsTransparencyPolicy */
	{
	return WriteReplyInt(aPolicy,EWsWinOpSetTransparencyPolicy);
	}


/** Returns whether the window uses a redraw store to store drawing commands.

The current WServ implementation always uses a redraw store. Therefore this function always returns ETrue.

This function always causes a flush of the window server buffer.

@return ETrue 
@see RWindow::EnableRedrawStore()
@deprecated */
EXPORT_C TBool RWindow::IsRedrawStoreEnabled() const
	{
	return WriteReply(EWsWinOpIsRedrawStoreEnabled);
	}

EXPORT_C TInt RWindowGroup::Construct(TUint32 aClientHandle)
/** Completes construction of a window group. 

Construction must be complete before any other window group methods can be 
called.

This function always causes a flush of the window server buffer.

@param aClientHandle The client's handle for the window. See RWindow::Construct() 
for a description of the client handle.
@return KErrNone if successful, otherwise one of the system-wide error codes, the most likely 
of which is KErrNoMemory. 
@see RWindow::Construct() */
	{
	return(ConstructChildApp(0,aClientHandle,ETrue));
	}

EXPORT_C TInt RWindowGroup::Construct(TUint32 aClientHandle,CWsScreenDevice* aScreenDevice) 
/** Completes construction of a window group on a screen. 

Construction must be complete before any other window group methods can be 
called.

This function always causes a flush of the window server buffer.

@param aScreenDevice A screen device is used to specify on which screen the window should be. The screen device must exist for as long as the window 
is in use. Different screen devices for two different window groups in an application allow the windows to have different screen size modes and WSERV 
will automatically switch from one to the other when there is a screen size mode change.
@param aClientHandle The client's handle for the window. See RWindow::Construct() 
for a description of the client handle.
@return KErrNone if successful, otherwise one of the system-wide error codes, the most likely 
of which is KErrNoMemory. 
@see RWindow::Construct() */
	{
	return (Construct(0,aClientHandle,ETrue,aScreenDevice));
	}

EXPORT_C TInt RWindowGroup::Construct(TUint32 aClientHandle,TBool aIsFocusable,CWsScreenDevice* aScreenDevice)
/** Completes construction of a window group on a screen. 

Construction must be complete before any other window group methods can be called.

The purpose of this Construct function for RWindowGroup is that it allows chains of window groups to be created on a specific screen. When one of 
the window groups in a chain is moved with SetOrdinalPosition then all window groups in that chain will be moved to be consecutative, 
with the parent being at the back in the Z-order. The purpose of this feature is to allow embedding of applications.

Note: The term parent should not be confused with the paremeter used in the function RWindow::Construct.  There it means that this window 
is a child of that window in the tree.  Here is just means that the two window groups involved will have an extra association.
Note: This window group will be given the same ordinal priority as it's parent window group.

This function always causes a flush of the window server buffer.

@param aScreenDevice A screen device is used to specify on which screen the window should be. The screen device must exist for as long as the window 
is in use. Different screen devices for two different window groups in an application allow the windows to have different screen size modes and WSERV 
will automatically switch from one to the other when there is a screen size mode change.
@param aClientHandle The client's handle for the window. See RWindow::Construct() 
for a description of the client handle.
@param aIdOfParentWindowGroup The Identifier of the parent window group.
@return KErrNone if successful, otherwise one of the system-wide error codes, the most likely of which is KErrNoMemory. 
It will return KErrPermissionDenied if the requested parent has not allowed this window group to be its child, 
and it will return KErrInUse if the parent already has a child.
@see RWindow::Construct() */
	{
	return(Construct(0,aClientHandle,aIsFocusable,aScreenDevice));	
	}
	
EXPORT_C TInt RWindowGroup::Construct(TUint32 aClientHandle,TBool aIsFocusable)
/** Completes the construction of a window group, setting its initial focus state. 

Construction must be complete before any other window group methods can be 
called.

aInitialFocusState can be specified in order to prevent a window group from 
automatically taking the keyboard focus when it is created. If specified, 
it sets the initial focus state of the window group. If a window group has 
a focus state of ETrue (the default), it can receive keyboard focus; if EFalse, 
it is prevented from receiving keyboard focus until this setting is changed. 
If a window group is constructed without specifying an initial focus state, 
keyboard focus will be enabled by default and the window group will receive 
keyboard focus automatically when it is created. To prevent this, set aInitialFocusState 
to EFalse.

This function always causes a flush of the window server buffer.

@param aClientHandle The client's integer handle for the window. See RWindow::Construct() 
for a description of the client handle.
@param aIsFocusable Whether the window is focusable. Set ETrue for it to 
be able to receive focus, EFalse otherwise. 
@return KErrNone if successful, otherwise one of the system-wide error codes, 
the most likely of which is KErrNoMemory.
@see EnableReceiptOfFocus() 
@see RWindow::Construct() */
	{
	return(ConstructChildApp(0,aClientHandle,aIsFocusable));
	}

EXPORT_C TInt RWindowGroup::ConstructChildApp(TInt aIdOfParentWindowGroup,TUint32 aClientHandle)
/** Completes construction of a window group. 

Construction must be complete before any other window group methods can be called.

The purpose of this Construct function for RWindowGroup is that it allows chains of window groups to be created. When one of 
the window groups in a chain is moved with SetOrdinalPosition then all window groups in that chain will be moved to be consecutative, 
with the parent being at the back in the Z-order. The purpose of this feature is to allow embedding of applications.

Note: The term parent should not be confused with the paremeter used in the function RWindow::Construct.  There it means that this window 
is a child of that window in the tree.  Here is just means that the two window groups involved will have an extra association.
Note: This window group will be given the same ordinal priority as it's parent window group.

This function always causes a flush of the window server buffer.

@param aClientHandle The client's handle for the window. See RWindow::Construct() 
for a description of the client handle.
@param aIdOfParentWindowGroup The Identifier of the parent window group.
@return KErrNone if successful, otherwise one of the system-wide error codes, the most likely of which is KErrNoMemory. 
It will return KErrPermissionDenied if the requested parent has not allowed this window group to be its child, 
and it will return KErrInUse if the parent already has a child.
@see RWindow::Construct() */
	{
	return(ConstructChildApp(aIdOfParentWindowGroup,aClientHandle,ETrue));
	}

TInt RWindowGroup::Construct(TInt aIdOfParentWindowGroup,TUint32 aClientHandle,TBool aIsFocusable, CWsScreenDevice* aScreenDevice)
/** Completes construction of a window group on a screen. 

Construction must be complete before any other window group methods can be called.

This function always causes a flush of the window server buffer.

@param aScreenDevice A screen device is used to specify on which screen the window should be. The screen device must exist for as long as the window 
is in use. Different screen devices for two different window groups in an application allow the windows to have different screen size modes and WSERV 
will automatically switch from one to the other when there is a screen size mode change.
@param aClientHandle The client's handle for the window. See RWindow::Construct() for a description of the client handle.
@param aIdOfParentWindowGroup The Identifier of the parent window group.
@return KErrNone if successful, otherwise one of the system-wide error codes, the most likely of which is KErrNoMemory. 
It will return KErrPermissionDenied if the requested parent has not allowed this window group to be its child, 
and it will return KErrInUse if the parent already has a child.
@param aIsFocusable Whether the window is focusable. Set ETrue for it to be able to receive focus, EFalse otherwise. 
@panic TW32Panic 17 in debug builds if called on an already constructed object.
@see RWindow::Construct() */
	{
	__ASSERT_DEBUG(iWsHandle == KNullHandle, Panic(EW32PanicGraphicDoubleConstruction));
	TWsClCmdCreateWindowGroup create;
	create.clientHandle=aClientHandle;
	create.focus=aIsFocusable;
	create.parentId=aIdOfParentWindowGroup;
	create.screenDeviceHandle = (aScreenDevice) ? aScreenDevice->WsHandle() : 0;
	
	TInt ret;
	if ((ret=iBuffer->WriteReplyWs(&create,sizeof(create),EWsClOpCreateWindowGroup))<0)
		{
		return ret;
		}
	iWsHandle=ret;
	return KErrNone;
	}

EXPORT_C TInt RWindowGroup::ConstructChildApp(TInt aIdOfParentWindowGroup,TUint32 aClientHandle,TBool aIsFocusable)
/** Completes construction of a window group. 

Construction must be complete before any other window group methods can be called.

This Construct function of RWindowGroup allows both the creation of the group window in a chain and for and for it not to recieve focus.  

This function always causes a flush of the window server buffer.

@param aClientHandle The client's handle for the window. See RWindow::Construct() 
for a description of the client handle.
@param aIdOfParentWindowGroup The Identifier of the parent window group.
@return KErrNone if successful, otherwise one of the system-wide error codes, the most likely of which is KErrNoMemory. 
It will return KErrPermissionDenied if the requested parent has not allowed this window group to be its child, 
and it will return KErrInUse if the parent already has a child.
@param aIsFocusable Whether the window is focusable. Set ETrue for it to be able to receive focus, EFalse otherwise. 
@see RWindow::Construct() */
	{
	return (Construct(aIdOfParentWindowGroup,aClientHandle,aIsFocusable,NULL));
	}
    
EXPORT_C void RWindowGroup::AllowProcessToCreateChildWindowGroups(TUid aProcessSID)
/** Allows a Window Group in the specified process to be be a child of this one

This function will need to be called to allow another window group to become a child window group of this one.

This function always causes a flush of the window server buffer.

@param aProcessSID This is the process security Id of the process that will be allowed to make child window groups*/
	{
	WriteReplyInt(aProcessSID.iUid,EWsWinOpAllowChildWindowGroup);		//Use WriteReply to cause it to flush
	}

EXPORT_C void RWindowGroup::EnableReceiptOfFocus(TBool aState)
/** Enables or disables receipt of keyboard focus.

The front most focusable window has keyboard focus. This function sets whether 
or not the window can receive keyboard focus.

@param aState Whether this window group can accept keyboard focus. */
	{
	WriteInt(aState,EWsWinOpReceiveFocus);
	}

EXPORT_C void RWindowGroup::AutoForeground(TBool aState)
/** Sets or disables auto-foreground behaviour. 

If this behaviour is set for a window, it will automatically be given ordinal 
position zero whenever a pointer event of type EButton1Down occurs in any 
of its child windows. This will cause it to be brought to the foreground, 
unless another window group exists with a higher priority.

A window group that is moved to the foreground will automatically get keyboard 
focus unless it has explicitly had receipt-of-focus disabled or there is a 
focusable window group of higher priority. See EnableReceiptOfFocus().

@param aState Whether this group should be automatically brought to the foreground 
on an EButton1Down. */
	{
	WriteInt(aState,EWsWinOpAutoForeground);
	}

TInt32 RWindowGroup::doCaptureKey(TUint aKey, TUint aModifierMask, TUint aModifiers, TInt aPriority, TInt aOpcode)
	{
	TWsWinCmdCaptureKey captureKey;

	captureKey.key=aKey;
	captureKey.modifiers=aModifiers;
	captureKey.modifierMask=aModifierMask;
	captureKey.priority=aPriority;

	return(WriteReply(&captureKey,sizeof(captureKey),aOpcode));
	}

void RWindowGroup::doCancelCaptureKey(TInt32 aCaptureKeyHandle, TInt aOpcode)
	{
	if (iWsHandle && aCaptureKeyHandle)
		WriteReplyInt(aCaptureKeyHandle,aOpcode);
	}

EXPORT_C TInt32 RWindowGroup::CaptureKey(TUint aKeyCode, TUint aModifierMask, TUint aModifiers)
/** Requests key capture.

This function instructs the window server to send key events (of type EEventKey) 
with the specified key code and modifier state to this window group, regardless 
of which window group currently has the keyboard focus. 

Key events for the key given by aKeyCode are captured only when the modifier keys 
specified by aModifierMask are in the states specified by aModifier. For example, 
calling  
@code
wsGroup->CaptureKey(EKeyDevice2, EModifierAlt|EModifierCtrl, EModifierCtrl); 
@endcode
will capture the EKeyDevice2 key only if Ctrl is pressed and Alt is not.

Normally, keyboard key presses result in three events being delivered to the client: 
EEventKeyDown, EEventKey and EEventKeyUp. However, CaptureKey() results in only 
the EEventKey being delivered to the window group that called it: the EEventKeyUp 
and EEventKeyDown events are sent to the window group that has focus. If a window 
group wishes to capture the EEventKeyUp and EEventKeyDown events as well as the 
EEventKey, it should call CaptureKeyUpAndDowns().

Note that a window group can call this function more than once (to request capture 
for more than one key), and more than one window group may have requested key 
capture. If multiple window groups have requested to capture the same key using 
this function, the key is sent to the window group that most recently requested 
the key capture.

This function always causes a flush of the window server buffer.

@param aKeyCode The key code for the key to be captured. Key codes for special 
keys are defined in TKeyCode. 
@param aModifierMask Bitmask that identifies the modifier keys of interest. 
Possible values are defined in TEventModifier.
@param aModifiers Bitmask that identifies which of the modifier keys in 
aModifierMask need to be set and which need to be unset. For example, see the 
description above. 
@return A handle identifying the capture key, or one of the system-wide error 
codes (if <0). KErrPermissionDenied indicates that the requested key cannot be 
captured by this window group, because it has been protected by another window group. 
For more information, see the PROTECTEDKEY parameter in wsini.ini. 
Handles should be kept in order to be passed to CancelCaptureKey() 
later. KErrPermissionDenied, if 
@capability SwEvent */
	{
	return(doCaptureKey(aKeyCode, aModifierMask, aModifiers, 0, EWsWinOpCaptureKey));
	}

EXPORT_C TInt32 RWindowGroup::CaptureKey(TUint aKeyCode, TUint aModifierMask, TUint aModifiers, TInt aPriority)
/** Requests key capture, with a priority.

This function instructs the window server to send key events (of type EEventKey) 
with the specified key code and modifier state to this window group, regardless 
of which window group currently has the keyboard focus. 

Key events for the key given by aKeyCode are captured only when the modifier keys 
specified by aModifierMask are in the states specified by aModifier. For example, 
calling  
@code
wsGroup->CaptureKey(EKeyDevice2, EModifierAlt|EModifierCtrl, EModifierCtrl, priority); 
@endcode
will capture the EKeyDevice2 key only if Ctrl is pressed and Alt is not.

Normally, keyboard key presses result in three events being delivered to the client: 
EEventKeyDown, EEventKey and EEventKeyUp. However, CaptureKey() results in only 
the EEventKey being delivered to the window group that called it: the EEventKeyUp 
and EEventKeyDown events are sent to the window group that has focus. If a window 
group wishes to capture the EEventKeyUp and EEventKeyDown events as well as the 
EEventKey, it should call CaptureKeyUpAndDowns().

Note that a window group can call this function more than once (to request capture 
for more than one key), and more than one window group may have requested key 
capture. If multiple window groups have requested to capture the same key at the 
same priority, the key is sent to the window group that most recently requested 
the key capture.

This function always causes a flush of the window server buffer.

@param aKeyCode The key code for the key to be captured. Key codes for special 
keys are defined in TKeyCode. 
@param aModifierMask Bitmask that identifies the modifier keys of interest. 
Possible values are defined in TEventModifier.
@param aModifiers Bitmask that identifies which of the modifier keys in 
aModifierMask need to be set and which need to be unset. For example, see the 
description above. 
@param aPriority A priority value - if more than one window group has requested 
capture for the same key event, the one with the highest priority will capture it.
@return A handle identifying the capture key, or one of the system-wide error 
codes (if <0). KErrPermissionDenied indicates that the requested key cannot be 
captured by this window group, because it has been protected by another window group. 
For more information, see the PROTECTEDKEY parameter in wsini.ini. Handles should be 
kept in order to be passed to CancelCaptureKey() later. 
@capability SwEvent */	{
	return(doCaptureKey(aKeyCode, aModifierMask, aModifiers, aPriority, EWsWinOpCaptureKey));
	}

EXPORT_C void RWindowGroup::CancelCaptureKey(TInt32 aHotKey)
/** Cancels a request for key capture. 

Use this function to cancel a request to capture a key previously made with 
CaptureKey(). If the value passed in aHotKey is not a valid capture key 
handle, this function will cause a panic.

This function always causes a flush of the window server buffer.

@param aHotKey The key for which the capture request is cancelled. */
	{
	doCancelCaptureKey(aHotKey, EWsWinOpCancelCaptureKey);
	}

EXPORT_C TInt32 RWindowGroup::CaptureKeyUpAndDowns(TUint aScanCode, TUint aModifierMask, TUint aModifiers)
/** Requests the capture of key-up and key-down events on behalf of a window group.

This function requests the window server to send EEventKeyUp and EEventKeyDown 
key events from the specified key to this window group, regardless of which 
window group currently has the keyboard focus. This contrasts with CaptureKey(), 
which causes the window server to send the EEventKey event.

Key events for the key given by aScanCode are captured only when the modifier 
keys specified by aModifierMask are in the states specified by aModifiers. 
See RWsSession::SetHotKey() for examples of how to set aModifierMask and aModifiers.

Note: in general, keyboard key presses result in three events being delivered to 
the client: EEventKeyDown, EEventKey and EEventKeyUp.

This function always causes a flush of the window server buffer.

@param aScanCode Scan code for the key to be captured. Scan codes are defined 
in TStdScanCode. 
@param aModifierMask Bitmask that identifies the modifier keys of interest. 
Possible values are defined in TEventModifier. 
@param aModifiers Bitmask that identifies which of the modifier keys in 
aModifierMask need to be set and which need to be unset. 
@return A handle identifying the capture key, or one of the system-wide error 
codes (if < 0). KErrPermissionDenied indicates that the requested key cannot be 
captured by this window group, because it has been protected by another window group. 
For more information, see the PROTECTEDKEY parameter in wsini.ini. Handles should be 
kept in order to be passed to CancelCaptureKeyUpAndDowns() later.
@capability SwEvent */
	{
	return(doCaptureKey(aScanCode, aModifierMask, aModifiers, 0, EWsWinOpCaptureKeyUpsAndDowns));
	}

EXPORT_C TInt32 RWindowGroup::CaptureKeyUpAndDowns(TUint aScanCode, TUint aModifierMask, TUint aModifiers, TInt aPriority)
/** Requests the capture of key-up and key-down events on behalf of a window group.
This function is identical to the other overload, except that it allows a priority to be specified.
This function always causes a flush of the window server buffer.
@param aScanCode Scan code for the key to be captured. Scan codes are defined 
in TStdScanCode. 
@param aModifierMask Bitmask that identifies the modifier keys of interest. 
Possible values are defined in TEventModifier. 
@param aModifiers Bitmask that identifies which of the modifier keys in 
aModifierMask need to be set and which need to be unset. 
@param aPriority A priority value - if more than one window group has requested 
capture for the same key event, the one with the highest priority will capture it.
@return A handle identifying the capture key, or one of the system-wide error 
codes (if < 0). KErrPermissionDenied indicates that the requested key cannot be captured by this 
window group, because it has been protected by another window group. For more information, see 
the PROTECTEDKEY parameter in wsini.ini. Handles should be kept in order to be passed to 
CancelCaptureKeyUpAndDowns() later.
@capability SwEvent */
	{
	return(doCaptureKey(aScanCode, aModifierMask, aModifiers, aPriority, EWsWinOpCaptureKeyUpsAndDowns));
	}

EXPORT_C void RWindowGroup::CancelCaptureKeyUpAndDowns(TInt32 aHotKey)
/** Cancels a capture request for a key up or key down event. 

Use this function to cancel a request to capture a key, previously made with 
CaptureKeyUpAndDowns(). If the value passed in aHotKey is not a valid 
capture key handle, this function will cause a panic.

This function always causes a flush of the window server buffer.

@param aHotKey Cancels the request to capture this key. */
	{
	doCancelCaptureKey(aHotKey, EWsWinOpCancelCaptureKeyUpsAndDowns);
	}

EXPORT_C TInt32 RWindowGroup::CaptureLongKey(TUint aInputKeyCode,TUint aOutputKeyCode,TUint aModifierMask,TUint aModifiers
																											,TInt aPriority,TUint aFlags)
/** Requests capture of long key presses.

This function causes the window server to send a long key event (which 
is generated when the key has been held down for the initial keyboard 
repeat time), to this window group, regardless of whether it currently has 
keyboard focus. The key that is output can have a different key code to the 
key that was captured.

Key events for the key given by aInputKeyCode are captured only when the modifier 
keys specified by aModifierMask are in the states specified by aModifiers.

A priority can be specified to resolve any conflicts that arise when multiple 
window groups attempt to capture the same long key event.

Normal key press behaviour (where no long key press capture requests have been 
made) is as follows. When a key is pressed, a key down event occurs, followed by 
one or more standard (short) key events, followed by a key up event. All of 
these key events are sent to the application in the foreground.

Key press behaviour when an application has made a long key press capture request 
is as follows. When the key is pressed and held down, a long key event is generated 
in addition to the events described above (although if a long key event occurs, 
then there will never be more than one standard key event) and this is sent to the 
application that made the request, even if it is not in the foreground. 

If that application also wants to capture the up and down key events then it needs to call 
CaptureKeyUpAndDowns(). If it wants to capture the standard key event, then it needs to 
call CaptureKey(). Depending on flags (enumerated in TLongCaptureFlags) specified when making 
the long key capture request, the standard key event can either be generated when the key is 
pressed, as it would be if no long key capture request had been made, or it can be generated 
when the key is released. In the latter case, the standard key event is only generated if the 
key was not held down long enough to generate a long key event.

This function always causes a flush of the window server buffer.

@param aInputKeyCode The key code for the key to be captured. Key codes for 
special keys are defined in TKeyCode.
@param aOutputKeyCode The key code that will be output.
@param aModifierMask Only the modifier keys in this mask are tested against 
the states specified in aModifier.
@param aModifiers The key is captured only when the modifier keys specified 
in aModifierMask match these states, where 1 = modifier set, and 0 = modifier 
not set. Modifier key states are defined in TEventModifier. 
@param aPriority If more than one window group has requested capture for the 
same long key event, the one with the highest priority will capture the event.
@param aFlags Configures the long key capture behaviour. See the TLongCaptureFlags 
enum.
@return Identifying value for the long key capture. For use with the CancelCaptureLongKey() 
function.
@see TLongCaptureFlags 
@capability SwEvent */
	{
	return(CaptureLongKey(TTimeIntervalMicroSeconds32(-1),aInputKeyCode,aOutputKeyCode,aModifierMask,aModifiers,aPriority,aFlags));
	}

EXPORT_C TInt32 RWindowGroup::CaptureLongKey(TTimeIntervalMicroSeconds32 aRepeatTime,TUint aInputKeyCode,TUint aOutputKeyCode
																	,TUint aModifierMask,TUint aModifiers,TInt aPriority,TUint aFlags)
/** Requests capture of long key presses.

This function causes the window server to send a long key event (which 
is generated when the key has been held down for the specified time), 
to this window group, regardless of whether it currently has 
keyboard focus.

For more information on this function, see the other CaptureLongKey() overload.

This function always causes a flush of the window server buffer.

@param aRepeatTime The time interval in microseconds between the initial 
key event and the first auto repeat.
@param aInputKeyCode The key code for the key to be captured. Key codes for 
special keys are defined in TKeyCode.
@param aOutputKeyCode The key code that will be output.
@param aModifierMask Only the modifier keys in this mask are tested against 
the states specified in aModifier.
@param aModifiers The key is captured only when the modifier keys specified 
in aModifierMask match these states, where 1 = modifier set, and 0 = modifier 
not set. Modifier key states are defined in TEventModifier. 
@param aPriority If more than one window group has requested capture for the 
same long key event, the one with the highest priority will capture the event.
@param aFlags Configures the long key capture behaviour. See the TLongCaptureFlags 
enum.
@return Identifying value for the long key capture. For use with the CancelCaptureLongKey() 
function.
@see TLongCaptureFlags
@capability SwEvent */
	{
	TWsWinCmdCaptureLongKey captureKey(aModifiers,aModifierMask,aInputKeyCode,aOutputKeyCode,aRepeatTime,aPriority,aFlags);
	return(WriteReply(&captureKey,sizeof(captureKey),EWsWinOpCaptureLongKey));
	}

EXPORT_C void RWindowGroup::CancelCaptureLongKey(TInt32 aCaptureKey)
/** Cancels a previous long key capture request.

This function always causes a flush of the window server buffer.

@param aCaptureKey The value returned by the previous call to CaptureLongKey(). 
Identifies which long key capture request to cancel. */
	{
	doCancelCaptureKey(aCaptureKey, EWsWinOpCancelCaptureLongKey);
	}

EXPORT_C TInt RWindowGroup::AddPriorityKey(TUint aKeycode, TUint aModifierMask, TUint aModifiers)
/** Adds a priority key. 

Priority key events are typically used for providing "Abort" or "Escape" keys 
for an application. For priority key events to occur they must first be configured 
using this function. Functions provided by RWsSession can then be used to 
get priority key events. Note that unlike other events, the Control Framework 
does not get priority key events for you.

Priority key events for the key given by aKeyCode are only delivered when 
the modifier keys specified by aModifierMask are in the states specified by 
aModifiers. See RWsSession::SetHotKey() for examples of how to use aModifierMask 
and aModifiers.

More than one priority key can be added for each keycode, each having a different 
set of modifier requirements.

Note: if you press a priority key while another is waiting to be sent to the client 
then the first key is lost.

This function always causes a flush of the window server buffer.

@param aKeycode The priority key to be added. 
@param aModifierMask Only the modifier keys in this mask are tested against 
the states specified in aModifiers. 
@param aModifiers Key is captured only when modifier keys specified in aModifierMask 
match these states, where 1 = modifier key on, and 0 = modifier key off. Modifier 
key states are defined in TEventModifier. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	TWsWinCmdPriorityKey priorityKey;

	priorityKey.keycode=aKeycode;
	priorityKey.modifiers=aModifiers;
	priorityKey.modifierMask=aModifierMask;
	return(WriteReply(&priorityKey,sizeof(priorityKey),EWsWinOpAddPriorityKey));
	}

EXPORT_C void RWindowGroup::RemovePriorityKey(TUint aKeycode, TUint aModifierMask, TUint aModifier)
/** Removes a priority key. 

Use this function to remove a priority key that was added using AddPriorityKey(). 
If the specified priority key does not exist, this function does nothing.

Note: all 3 parameters must match exactly for a successful removal.

@param aKeycode Key code for the priority key to be removed 
@param aModifierMask Modifier mask for the priority key to be removed 
@param aModifier Modifier states for the priority key to be removed */
	{
	if (iWsHandle)
		{
		TWsWinCmdPriorityKey priorityKey;

		priorityKey.keycode=aKeycode;
		priorityKey.modifiers=aModifier;
		priorityKey.modifierMask=aModifierMask;
		Write(&priorityKey,sizeof(priorityKey),EWsWinOpRemovePriorityKey);
		}
	}

EXPORT_C void RWindowGroup::SetTextCursor(RWindowBase &aWin, const TPoint &aPos, const TTextCursor &aCursor, const TRect &aClipRect)
/** Sets the text cursor and its clipping rectangle. 

Use this function to set a text cursor for this window group, or to change 
the existing text cursor's position or appearance.

The cursor is clipped to aClipRect. This allows, for example, the window 
to have a border region in which the cursor is not displayed.

@param aWin The text cursor is in this window, and is hence clipped to it 
and positioned relative to it. 
@param aPos Position of the text cursor's origin, relative to the origin of 
aWin. 
@param aCursor The cursor to set. This may be a standard rectangular cursor, 
of type TTextCursor::ETypeRectangle or TTextCursor::ETypeHollowRectangle, 
or it may be a custom cursor, in which case it should have previously been 
added to the window server using RWsSession::SetCustomTextCursor().
@param aClipRect The cursor is clipped to this rectangle. Rectangle co-ordinates 
are relative to the origin of aWin. */
	{
	TWsWinCmdSetTextCursor SetTextCursor(aWin.WsHandle(),aPos,aCursor,aClipRect);
	Write(&SetTextCursor,sizeof(SetTextCursor),EWsWinOpSetTextCursorClipped);
	}

EXPORT_C void RWindowGroup::SetTextCursor(RWindowBase &aWin, const TPoint &aPos, const TTextCursor &aCursor)
/** Sets the text cursor.

Use this function to set a text cursor for this window group, or to change 
the existing text cursor's position or appearance.

@param aWin The text cursor is in this window, and is hence clipped to it 
and positioned relative to it.
@param aPos Position of the text cursor's origin, relative to the origin of 
aWin.
@param aCursor The cursor to set. This may be a standard rectangular cursor, 
of type TTextCursor::ETypeRectangle or TTextCursor::ETypeHollowRectangle, 
or it may be a custom cursor, in which case it should have previously been 
added to the window server using RWsSession::SetCustomTextCursor(). */
	{
	TWsWinCmdSetTextCursor SetTextCursor(aWin.WsHandle(),aPos,aCursor);
	Write(&SetTextCursor,sizeof(SetTextCursor),EWsWinOpSetTextCursor);
	}

EXPORT_C void RWindowGroup::CancelTextCursor()
/** Removes the text cursor. 

Use this function to remove the current text cursor from this window group. 
This function does nothing if the cursor is currently in another window group. */
	{
	if (iWsHandle)
		Write(EWsWinOpCancelTextCursor);
	}

EXPORT_C TInt RWindowGroup::SetName(const TDesC &aName)
/** Sets the window group's name. 

Use this function to set the name of a window group. Window group names are 
arbitrary and can contain any data that can be stored in a descriptor 
of type TDesC.

Note: if this function is successful, a window group change event is sent to everything 
that is set up to receive these events.

This function always causes a flush of the window server buffer.

@param aName The name for the window group. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	const TInt nameLength=aName.Length();
	return WriteReplyByProvidingRemoteReadAccess(&nameLength,sizeof(TInt),&aName,EWsWinOpSetName);
	}

EXPORT_C TInt RWindowGroup::Name(TDes &aWindowName) const
/** Gets the window group's name, as set by SetName().

This function always causes a flush of the window server buffer.

@param aWindowName On return, contains the name of this window group. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	return(WriteReplyIntP(aWindowName.MaxLength(),&aWindowName,EWsWinOpName));
	}

EXPORT_C void RWindowGroup::SetOwningWindowGroup(TInt aIdentifier)
/** Sets the owning window group for this window group.

The owning window group is the group that is brought to the foreground when 
the window group which has keyboard focus (the foreground application) dies.

If the owning window group is not set, then the window group brought to the 
foreground when the application dies will be the default owning window group.

@param aIdentifier The window group's identifier. */
	{
	WriteInt(aIdentifier,EWsWinOpSetOwningWindowGroup);
	}

EXPORT_C void RWindowGroup::DefaultOwningWindow()
/** 
@publishedPartner
@released

Makes this window group the default owning window group.

The default owning window group is the group that is brought to the foreground 
when the window group which has keyboard focus (the foreground application) 
dies, if no other window has been set up as the owning group.

This window group will take over as the default owning window group even if 
another window group has previously called this function. However, if this 
window group is subsequently destroyed, the default owning window group will 
revert to being the previous window group.

@see SetOwningWindowGroup() 
@capability WriteDeviceData */
	{
	Write(EWsWinOpDefaultOwningWindow);
	}

EXPORT_C TInt RWindowGroup::Identifier() const
/** Gets the identifier of the window group.

This function always causes a flush of the window server buffer.

@return The window group identifier. */
	{
	return(WriteReply(EWsWinOpIdentifier));
	}

EXPORT_C void RWindowGroup::DisableKeyClick(TBool aState)
/** Disables key clicks.

If a window group calls this function with aState=ETrue, key clicks (i.e. 
the sound generated when keys are pressed) will be disabled whenever this 
window group has the keyboard focus. Key clicks for this window group can be 
re-enabled by calling this function with aState=EFalse. 

Note that this function doesn't do anything in v6.0 and v6.1.

@param aState If ETrue, no key clicks occur when this window group has keyboard focus. 
If EFalse, key clicks are enabled when this window group has keyboard focus. */
	{
	WriteInt(aState,EWsWinOpDisableKeyClick);
	}

EXPORT_C TInt RWindowGroup::EnableScreenChangeEvents()
/** Enables screen change event sending.

This function ensures that window groups are sent screen change events, which 
are sent, for example, when the cover on a phone that supports screen flipping 
is opened or closed.

Note that not getting screen change events is the default behaviour.

This function always causes a flush of the window server buffer.

@return KErrNone if successful, otherwise another of the system-wide error 
codes. 
@see DisableScreenChangeEvents() */
	{
	return(WriteReply(EWsWinOpEnableScreenChangeEvents));
	}

EXPORT_C void RWindowGroup::DisableScreenChangeEvents()
/** Disables screen change event sending.

This function prevents window groups from getting screen change events, which 
are sent, for example, when the cover on a phone that supports screen flipping is 
opened or closed.

Note that not getting screen change events is the default behaviour.

See EnableScreenChangeEvents(). */
	{
	if (iWsHandle)
		Write(EWsWinOpDisableScreenChangeEvents);
	}

EXPORT_C void RWindowGroup::SimulatePointerEvent(TRawEvent aEvent)
/** Simulates a pointer event.

This function sends a pointer event to the window as if it had come 
from the kernel, except that it will be sent to a window which is a child 
of the window group it is called on. 

Notes:

The function can be used to send a pointer event to an application when 
it is in the background.

The function is supported for testing purposes only.

@param aEvent The simulated raw event. */
	{
	Write(&aEvent,sizeof(aEvent),EWsWinOpSendPointerEvent);
	}

EXPORT_C void RWindowGroup::SimulateAdvancedPointerEvent(TRawEvent /*aEvent*/)
/** Dummy implementation in order to preserve compatibility with WSERV NGA.
    @internalComponent */ 
	{
	ASSERT(0);
	}

EXPORT_C TInt RWindowGroup::ClearChildGroup()
/** Clears all children of the current window group.

The window group chain is broken directly after the current window group. In the 
general case (consider clearing the child group of a window group in the middle of a 
long chain), this results in two distinct chains. In the special cases where either
the parent window group or the child window group ends up as the only member of a 
resultant chain, it is removed from that chain.

@return KErrArgument if trying to clear the child window group of a window group that
has no children; KErrNoMemory if, when splitting into 2 chains, there is insufficient
memory to create the second chain (in this case the child window groups are all cleared
from the current chain, then the error is returned). Otherwise KErrNone or one of the 
system-wide error codes.

@see SetChildGroup() */
	{
	return WriteReply(EWsWinOpClearChildGroup);
	}
	
EXPORT_C TInt RWindowGroup::SetChildGroup(TInt aId)
/** Sets a window group chain onto the current window group.

Appends all the window groups chained to the child window group onto the chain
containing the current window group. 

@param aId. ID of the child window group that is head of the chain

@return KErrArgument if any of these are true:
- child group referred to by aId does not exist
- the current window group already has a child 
- the window group requested to be the child is already a child of another window group
- the window group requested to the child is already the parent or grand parent etc. of the current window group
KErrNoMemory: If the group and it's child are not currently in a chain then a new chain needs to be created
which can fail due to lack of memory;
otherwise one of the system-wide error codes is returned.

@see ClearChildGroup() */
	{
	return WriteReplyInt(aId,EWsWinOpSetChildGroup);
	}

//
// Backed up window
//

EXPORT_C TInt RBackedUpWindow::Construct(const RWindowTreeNode &parent, TDisplayMode aDisplayMode, TUint32 aClientHandle)
/** Completes the construction of a backed up window. 

This method should be called after the RBackedUpWindow(RWsSession&) constructor, 
and before any other functions are performed on the window. It creates a window 
in the window server corresponding to the RBackedUpWindow object, and allocates 
a bitmap with which to perform the window backup.

Unlike non backed up windows, the size of a backed up window is not inherited 
from its parent. The window will be created with an initial size of zero, 
but this can be altered. The display mode must be specified because it determines 
the amount of memory required to store the backup bitmap.

This function always causes a flush of the window server buffer.

@param parent The window's parent. 
@param aDisplayMode The display mode for the window. 
@param aClientHandle The client's handle for the window. See RWindow::Construct() 
for a description of the client handle.
@return KErrNone if successful, otherwise one of the system-wide error codes, the 
most likely of which is KErrNoMemory. 
@see RWindowBase::SetSizeErr()
@see RWindowBase::SetExtentErr() 
@see RWindow::Construct() */
	{
	return(construct(parent,aClientHandle,EWinBackedUp,aDisplayMode));
	}

EXPORT_C TInt RBackedUpWindow::BitmapHandle() const
/** Gets a handle to the backup bitmap. 

This handle can then be passed to CFbsBitmap::Duplicate() to gain access 
to the bitmap. Once the bitmap has been obtained, it can be drawn to or read 
from by the application.

Notes: in most circumstances this function should be used only after MaintainBackup() 
has been called. Otherwise, the content of the backup bitmap will be unpredictable.

This function always causes a flush of the window server buffer.

@return The handle of the backup bitmap. */
	{
	return(WriteReply(EWsWinOpBitmapHandle));
	}

EXPORT_C void RBackedUpWindow::UpdateScreen(const TRegion &aRegion)
/** Copies a part of the backup bitmap to the on-screen bitmap. 

This function behaves in the same way as UpdateScreen(), but copies only the 
specified region of the backup bitmap to the window on screen.

This function always causes a flush of the window server buffer.

@param aRegion Area of the backup bitmap to copy. */
	{
	const TInt regionCount=aRegion.Count();
	TPtrC8 ptrRect(reinterpret_cast<const TUint8*>(aRegion.RectangleList()),regionCount*sizeof(TRect));
	WriteReplyByProvidingRemoteReadAccess(&regionCount,sizeof(regionCount),&ptrRect,EWsWinOpUpdateScreenRegion);
	}

EXPORT_C void RBackedUpWindow::UpdateScreen()
/** Copies the backup bitmap's image to the on-screen bitmap. 

This function should be used if the application draws directly to the backup 
bitmap. Any changes made to the backup bitmap will not be reflected on the 
screen until this function is called. */
	{
	Write(EWsWinOpUpdateScreen);
	}

EXPORT_C void RBackedUpWindow::UpdateBackupBitmap()
/** Copies to the backup bitmap any areas of the window which are not currently 
stored in the bitmap.

This method does not need to be called if the server has not been instructed 
to maintain the backup buffer constantly

@see MaintainBackup() */
	{
	Write(EWsWinOpUpdateBackupBitmap);
	}

EXPORT_C void RBackedUpWindow::MaintainBackup()
/** Tells the window server to start maintaining a backup bitmap with the entire 
window contents. 

By default, the window only backs up the non-visible and shadowed part of the 
window. Calling this function makes drawing to the window much faster.

Once this function has been called, it cannot be reversed. */
	{
	Write(EWsWinOpMaintainBackup);
	}

//
// Blank window
//

EXPORT_C TInt RBlankWindow::Construct(const RWindowTreeNode &parent, TUint32 aClientHandle)
/** Completes the construction of a valid blank-window handle. 

This function should be called after the RBlankWindow(RWsSession&) constructor, 
and before any other functions are performed on the window. It creates a window 
in the window server corresponding to the RBlankWindow object. The window 
is initialised to inherit the size and extent of its parent window, or to be full 
screen if its parent is a group window.

This function always causes a flush of the window server buffer.

@param parent The window's parent. 
@param aClientHandle Client handle for the window. See RWindow::Construct() for a 
description of the client handle. 
@return KErrNone if successful, otherwise one of the system-wide error codes, 
the most likely of which is KErrNoMemory.
@see TWsEvent::Handle()
@see TWsPriorityKeyEvent::Handle() 
@see RWindow::Construct() */
	{
	return(construct(parent,aClientHandle,EWinBlank,ENone));
	}

EXPORT_C void RBlankWindow::SetColor(TRgb aColor)
/** Sets the colour of the blank window.

@param aColor Colour for the window. */
	{
	WriteInt(aColor.Internal(),EWsWinOpSetColor);
	}

EXPORT_C void RBlankWindow::SetColor()
/** Sets the background colour used for clearing when the window becomes uncovered 
or visible to none. 

The window content will be left with whatever happened to be on the screen at the time. */
	{
	Write(EWsWinOpSetNoBackgroundColor);
	}

EXPORT_C void RBlankWindow::SetSize(const TSize &aSize)
/** Sets the size of the blank window.

@param aSize Size. */
	{
	WriteSize(aSize,EWsWinOpSetSize);
	}

EXPORT_C void RBlankWindow::SetExtent(const TPoint &pos,const TSize &size)
/** Sets the extent of the blank window.

@param pos The position of the window's origin, relative to its parent. 
@param size Size for the window. */
	{
	Write(&pos,sizeof(pos),&size,sizeof(size),EWsWinOpSetExtent);
	}
