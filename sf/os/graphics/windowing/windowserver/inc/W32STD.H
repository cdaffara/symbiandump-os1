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
// Standard window server header file
// 
//

#ifndef __W32STD_H__
#define __W32STD_H__

#ifndef __FNTSTORE_H__
#include <fntstore.h>
#endif
#ifndef __BITDEV_H__
#include <bitdev.h>
#endif
#ifndef __BITSTD_H__
#include <bitstd.h>
#endif
#include <e32keys.h>
#include <textcursor.h>
#include <pointerevent.h>
#include <advancedpointerevent.h>
#include <sizemode.h>
#include <babitflags.h>
#include <wspublishandsubscribedata.h>

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <graphics/windowserverconstants.h>
#include <graphics/pointereventdata.h>
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

_LIT(KWSERVThreadName,"Wserv");

class RWindowBase;
class RWindow;
class RWsBuffer;
class MWsObjectProvider;
class RWsDrawableSource;
class TSizeMode;

/** Screen mode enforcement flags.

Screen mode enforcement ensures that windows must meet certain requirements
if they are to be displayed. When the screen mode changes, window groups that
are incorrectly set up, according to these requirements, are locked out. The
windows are notified of the change, and will be displayed if they are updated
(according to the current enforcement requirement) to match the new screen
mode.

@publishedAll
@released
@see CWsScreenDevice::ScreenModeEnforcement()
@see CWsScreenDevice::SetScreenModeEnforcement() */
enum TScreenModeEnforcement
	{
	/** No enforcement.

	All windows that are the children of window groups will be displayed, irrespective
	of the screen mode.

	This is not properly supported and is provided for testing purposes. */
	ESizeEnforcementNone,
	/** Pixels and rotation enforcement.

	Group windows must be set up for the correct rotation and be working in the
	correct size in pixels to be displayed. */
	ESizeEnforcementPixelsAndRotation,
	/** Pixels and Twips enforcement.

	Group windows must have the correct rotation and be working in the correct
	size in pixels and twips to be displayed.

	This setting might be used if the display pixels are not square, and would
	distort fonts when rotated. */
	ESizeEnforcementPixelsTwipsAndRotation,
	};

struct TPixelsAndRotation
/** Pixels and rotation struct.

This is used to define a particular screen mode's screen size in pixels,
and its rotation.

@publishedAll
@released
@see CWsScreenDevice::GetScreenModeSizeAndRotation() */
	{
	/** The screen size, for a given screen mode, in pixels. */
	TSize iPixelSize;
	/** The current screen orientation. */
	CFbsBitGc::TGraphicsOrientation iRotation;
	};

struct TPixelsTwipsAndRotation
/** Pixels, twips and rotation struct.

This is used to define a particular screen mode's screen size in twips
and pixels, and its rotation.

@publishedAll
@released
@see CWsScreenDevice::GetScreenModeSizeAndRotation() */
	{
	/** The screen size, for a given screen mode, in pixels. */
	TSize iPixelSize;
	/** The screen size, for a given screen mode, in twips. */
	TSize iTwipsSize;
	/** The screen rotation. */
	CFbsBitGc::TGraphicsOrientation iRotation;
	};

/** Log message text length.

This defines the length of the log text message buffer TLogMessageText,
which is used in RWsSession::LogMessage().

@publishedAll
@released */
enum {
	/** The length of the log message text buffer in characters. */
	KLogMessageLength=0x80
	};

/** Log message text buffer.

This is used in RWsSession::LogMessage().

@publishedAll
@released */
typedef TBuf<KLogMessageLength> TLogMessageText;

/** Password window group priority.

This priority is assigned to the password window group when the window server is displaying the password window.
This is the highest priority, which ensures that the machine password window is in front of all other windows -
hence password tests cannot be bypassed.

@publishedAll
@deprecated */
enum
	{
	 /** Password window group priority. */
	KPasswordWindowGroupPriority=1000
	};

/** Switch ordinal position to owning window.

This enum can be specified as the 2nd parameter to RWsSession::SetWindowGroupOrdinalPosition()
or as the 1st parameter to RWindowTreeNode::SetOrdinalPosition() (if this is being called on an
RWindowGroup).

When called with this value, the functions don't change the ordinal position of the group window
in question, but rather bring to the front the window that would come to the front if that group
window were to die with keyboard focus.

@publishedAll
@released
@see RWsSession::SetWindowGroupOrdinalPosition()
@see RWindowTreeNode::SetOrdinalPosition() */
enum {
	/** Switch ordinal position to owning window. */
	KOrdinalPositionSwitchToOwningWindow=0x80000000
	};

/** Window corner types.

Corners may be specified square or stepped, with varying numbers of pixels
removed to make the stepping. Picturing a corner as the top left corner of
a window, the numbers denote the number of pixels removed from the top row
to give corners.

@publishedAll
@released
@see TCornerFlags
@see RWindowBase::SetCornerType() */
enum TCornerType
// Bottom 16 bits available for corner type
	{
	/** Square corner. */
	EWindowCornerSquare,
	/** Corner pixel removed. */
	EWindowCorner1,
	/** Two pixel step.

	3 pixels are removed: the corner pixel and a single pixel from each side. */
	EWindowCorner2,
	/** Three pixel step.

	5 pixels are removed: the corner pixel, the two pixels next to it and the
	2 pixels above or below it. */
	EWindowCorner3,
	/** Five pixel step.

	12 pixels are removed: the corner pixel, the four pixels next to it, the
	four pixels above or below it, the pixel which was diagonal to the corner
	pixel, and the pixels to both sides of that pixel. */
	EWindowCorner5,
	EWindowCornerRegion,		// Private
	/** Corner type mask.

	This value is used by the window server, and may be used by developers, to
	separate TCornerType and TCornerFlags values from a TInt which contains both
	types. */
	ECornerTypeMask=0xFFFF
	};

/** Corner flags.

These are used by RWindowBase::SetCornerType() to set which corners of a window
are not to have corner types applied to them.

@publishedAll
@released
@see TCornerType */
enum TCornerFlags
// Top 16 bits available for corner flags
	{
	/** Do not apply corner shape to top left corner. */
	EWindowCornerNotTL=0x10000,
	/** Do not apply corner shape to top right corner. */
	EWindowCornerNotTR=0x20000,
	/** Do not apply corner shape to bottom left corner. */
	EWindowCornerNotBL=0x40000,
	/** Do not apply corner shape to bottom right corner. */
	EWindowCornerNotBR=0x80000,
	};

/** Window backup type flags.

@publishedAll
@released
@see RWindowBase::EnableBackup() */
enum TWindowBackupType
	{
	/** Backup the area behind the current window only.

	This is the default behaviour for the RWindowBase::EnableBackup() function. */
	EWindowBackupAreaBehind=0x0001,
	/** Backup the entire screen.

	This is provided for windows that require the rest
	of the screen to fade when they are displayed. When the rest of the screen
	should become unfaded, the window server redraws the whole screen from the
	backed up bitmap. */
	EWindowBackupFullScreen=0x0002,
	};

/** Types of standard events.

These are events that the window server passes to its clients, as opposed
to events that the base passes to the window server.

@publishedAll
@released
@see CClickMaker */
enum TEventCode
	{
	/** Null event.

	This can be sent, but should be ignored by clients. */
	EEventNull,
	/** Key event.

	This is the event that is sent when a character has been received from the
	keyboard.

	If an EEventKey event is associated with an EEventKeyDown or EEventKeyUp
	event (typically EEventKeyDown), the EEventKey event occurs after the
	EEventKeyDown/EEventKeyUp event.

	In practice, the only keys potentially likely to have their EEventKey event
	generated on the up rather than the down are modifier keys. */
	EEventKey,
	/** Key up event.

	If an EEventKey event is associated with an EEventKeyUp event (which is
	rarely the case), the EEventKey event occurs after the EEventKeyUp event. */
	EEventKeyUp,
	/** Key down event.

	If an EEventKey event is associated with an EEventKeyDown event (which
	is typically the case), the EEventKey event occurs after the EEventKeyDown event. */
	EEventKeyDown,
	/** Modifier changed event.

	This is an event generated by the window server when
	the state of one of the modifier keys changes.
	It is not reported unless explicitly requested by a window.

	@see RWindowTreeNode::EnableModifierChangedEvents(). */
	EEventModifiersChanged,
	/** Pointer event.

	This event is sent when the user presses or releases a pointer button (or
	the equivalent action, depending on the type of pointing device), drags the
	pointer, moves it or uses the pointer to switch on the device. */
	EEventPointer,			//5
	/** Pointer enter event.

	This occurs when the user moves the pointer into a window with a pointer button
	pressed (or equivalent action depending on the type of pointing device). If
	move events are being generated, this event also occurs when the user moves
	the pointer into the window. */
	EEventPointerEnter,
	/** Pointer exit event.

	Occurs when the user moves the pointer out of a window with a pointer button
	pressed (or equivalent action depending on the type of pointing device). If
	move events are being generated, this event also occurs when the user moves
	the pointer out of the window. */
	EEventPointerExit,
	/** Pointer move buffer ready event.

	Occurs when the pointer move buffer is ready to be retrieved by the client. */
	EEventPointerBufferReady,
	/** Occurs as a duplicate of each pointer event if a window sets pointer capture
	with the TCaptureFlagDragDrop flag set. */
	EEventDragDrop,
	/** Focus lost event.

	Occurs when a window group loses keyboard focus. */
	EEventFocusLost,		//10
	/** Focus gained event.

	Occurs when a window group gains keyboard focus. */
	EEventFocusGained,
	/** On event.

	This event type is not reported unless explicitly requested by a window.

	@see RWindowTreeNode::EnableOnEvents(). */
	EEventSwitchOn,
	/** Password event.

	Occurs when the window server enters password mode. It is sent to the group
	window of the currently active password window.

	This is the window server mode where the user is required to enter a password
	before any further actions can be performed.

	@deprecated	*/
	EEventPassword,
	/** Window group changed event. This occurs whenever a window group is destroyed,
	and whenever a window group's name changes

	This event type is not reported unless explicitly requested by a window.

	@see RWindowTreeNode::EnableGroupChangeEvents(). */
	EEventWindowGroupsChanged,
	/** Error event.

	Occurs when an error occurs. See TWsErrorMessage::TErrorCategory for the types
	of errors.

	This event type is not reported unless explicitly requested by a window.

	@see RWindowTreeNode::EnableErrorMessages(). */
	EEventErrorMessage,		//15
	/** Message ready event.

	Occurs when a session sends a message to this window group using RWsSession::SendMessageToWindowGroup(). */
	EEventMessageReady,
	EEventMarkInvalid,	// For internal use only
	/** Off event.

	This is issued when an off event is received by the window server from the
	base.

	If for some reason the event can't be delivered, or there is no-one to deliver
	it to, then a call to the base is made to power down the processor.

	This event is only delivered if explicitly requested using RWsSession:RequestOffEvent(). */
	EEventSwitchOff,
	/** Event issued to off-event requesting windows when the off key is pressed. */
	EEventKeySwitchOff,
	/** Screen size mode change event.

	This is issued when the screen size mode has changed, for instance when
	the cover on a phone that supports screen flipping is opened or closed. */
	EEventScreenDeviceChanged, //20
	/** Event sent whenever the window group with focus changes.

	Requested by RWindowTreeNode::EnableFocusChangeEvents(). */
	EEventFocusGroupChanged,
	/** Case opened event.

	This event is sent to those windows that have requested EEventSwitchOn
	events. Unlike with EEventSwitchOn events, the screen will not be switched
	on first. */
	EEventCaseOpened,
	/** Case closed event.

	This event is sent to those windows that have requested EEventSwitchOff
	events.

	Unlike EEventSwitchOff events, which make a call to the base to power down
	the processor if for some reason the event can't be delivered (or there is
	no-one to deliver it to), failure to deliver case closed events has no repercussions. */
	EEventCaseClosed,
	/** Window group list change event.

	The window group list is a list of all window groups and their z-order. This
	event indicates any change in the window group list: additions, removals and
	reorderings.

	Notification of this event is requested by calling RWindowTreeNode::EnableGroupListChangeEvents(). */
	EEventWindowGroupListChanged,
	/** The visibility of a window has changed

	This is sent to windows when they change from visible to invisible, or visa versa, usually due
	to another window obscuring them.

	Notification of this event is requested by calling RWindowTreeNode::EnableVisibilityChangeEvents(). */
	EEventWindowVisibilityChanged,
#ifdef SYMBIAN_PROCESS_MONITORING_AND_STARTUP
	/** Restart event.
	
	This is issued when an restart event is received by the window server from the 
	base. This event is also an off event, because it might power-cycle the device.
	
	If for some reason the event can't be delivered, or there is no-one to deliver 
	it to, then a call to the base is made to power down the processor.
	
	This event is only delivered if explicitly requested using RWsSession:RequestOffEvent(). */
	EEventRestartSystem,
#endif
	/** The display state or configuration has changed
	
	Either change of the current resolution list (state change) or current resolution/background
	(mode change) will trigger this event.
	
	Notification of this event is requested by calling MDisplayControl::EnableDisplayChangeEvents() 
	 */
	EEventDisplayChanged = EEventWindowVisibilityChanged+2,
	//Codes for events only passed into Key Click DLL's
	/** Repeating key event.

	This is only sent to a key click plug-in DLL (if one is present) to indicate
	a repeating key event.

	@see CClickMaker */
	EEventKeyRepeat=100,
	EEventGroupWindowOpen,
	EEventGroupWindowClose,
	EEventWindowClose,
	//Codes for events only passed into anim dlls
	/** Direct screen access begin
	This is only sent to anim dlls (if they register to be notified). It indicates that
	the number of direct screen access sessions has increased from zero to one.*/
	EEventDirectScreenAccessBegin=200,
	/** Direct screen access end
	This is only sent to anim dlls (if they register to be notified). It indicates that
	the number of direct screen access sessions has decreased from one to zero.*/
	EEventDirectScreenAccessEnd,
	/** Event to signal the starting or stopping of the wserv heartbeat timer
	This is only sent to anim dlls (if they register to be notified). */
	EEventHeartbeatTimerStateChange,

	//The range 900-999 is reserved for UI Framework events
	/** 900-909 WSERV protects with PowerMgmt */
	EEventPowerMgmt = 900,
	EEventReserved = 910,

	//Event codes from EEventUser upwards may be used for non-wserv events.
	//No event codes below this should be defined except by the window server

	/** User defined event.

	The client can use this and all higher values to define their own
	events. These events can be sent between windows of the same client or windows
	of different clients.

	@see RWs::SendEventToWindowGroup(). */
	EEventUser=1000,

	};


/** Window server hot keys.

@publishedAll
@released
@see RWsSession::SetHotKey() */
enum THotKey
	{
	/** Enables logging of all messages to and from the window server.

	Note that the required type of logging must have been specified in the wsini.ini
	file (using the LOG keyword), and the appropriate logging DLL must be available.

	Default key mapping: \<Ctrl\>\<Alt\>\<Shift\>E */
	EHotKeyEnableLogging,
	/** Always disables window server logging, if active. Does nothing otherwise.

	Default key mapping: \<Ctrl\>\<Alt\>\<Shift\>D */
	EHotKeyDisableLogging,
	/** Dumps a list of all windows to the log. (If logging is disabled, it is temporarily
	enabled in order to do this.)

	Default key mapping: \<Ctrl\>\<Alt\>\<Shift\>W */
	EHotKeyStateDump,
	/** Kills the foreground application.

	Default key mapping: \<Ctrl\>\<Alt\>\<Shift\>K */
	EHotKeyOfDeath,
	/** Shuts down the window server.

	Be cautious! This may mean resetting the machine to re-start the window server.

	Default key mapping: release (not available), debug (\<Ctrl\>\<Alt\>\<Shift\>X). */
	EHotKeyShutDown,
	/** Dumps a list of cells allocated on the window server's heap to the log. (If
	logging is disabled, it is temporarily enabled in order to do this.)

	Note that logging requires that the type of logging has been set up in the
	wsini.ini file, and that the appropriate logging DLL is available.

	Default key mapping: \<Ctrl\>\<Alt\>\<Shift\>H */
	EHotKeyHeapDump,
	/** Increases the LCD contrast.

	Default key mapping: EKeyIncContrast. Note that this value is from an enum
	in e32keys.h. The contrast wraps around when it reaches the maximum. */
	EHotKeyIncContrast,
	/** Decreases the LCD contrast.

	Default key mapping: EKeyDecContrast. Note that this value is from an enum
	in e32keys.h. The contrast wraps around when it reaches the minimum. */
	EHotKeyDecContrast,
	/** Switches the machine off.

	Default key mapping: EKeyOff. Note that this value is from an enum in e32keys.h. */
	EHotKeyOff,
	/** Switches the backlight on.

	Default key mapping: EKeyBacklightOn. Note that this value is from an enum
	in e32keys.h. */
	EHotKeyBacklightOn,
	/** Switches the backlight off.

	Default key mapping: EKeyBacklightOff. Note that this value is from an enum
	in e32keys.h. */
	EHotKeyBacklightOff,
	/** Toggles the backlight.

	Default key mapping: EKeyBacklightToggle. Note that this value is from an
	enum in e32keys.h. */
	EHotKeyBacklightToggle,
	/** Switches to screen size 0.

	This, and the following 3 keys are used to switch screen sizes on real hardware,
	for instance when the cover is closed on a phone that supports screen flipping. */
	EHotKeyScreenDimension0,
	/** Switches to screen size 1.

	This might be generated when the cover is opened on a phone that supports screen
	flipping. */
	EHotKeyScreenDimension1,
	/** Switches to screen size 2. */
	EHotKeyScreenDimension2,
	/** Switches to screen size 3. */
	EHotKeyScreenDimension3,
	/** Cycles the display though its possible sizes.

	This is used only for debugging.

	A device may have several screen sizes, each with a default orientation. For
	example a phone that supports screen flipping will have different display
	sizes when the cover is opened and closed.

	Default key mapping: debug : \<Ctrl\>\<Alt\>\<Shift\> U. Release : none. */
	EHotKeyCycleDisplaySize,
	/** Cycles the screen orientation through the specified rotations for the particular
	size mode you are in.

	For example, phones that support screen flipping may
	use this feature for changing between right and left handed use.

	For rectangular display modes you can only specify 2 orientations, 180 degrees
	apart. For square modes you can specify 4 rotations (90 degrees) or however
	many you want.

	Specification of the list of rotations takes place in the WSINI.INI file.

	Default key mapping: debug : \<Ctrl\>\<Alt\>\<Shift\> O. Release : none. */
	EHotKeyCycleOrientation,
	/** Increases the screen's brightness.

	The brightness wraps around to the minimum
	value after it has reached the maximum. */
	EHotKeyIncBrightness,
	/** Decreases the screen's brightness.

	The brightness wraps around to the maximum value after it has reached the minimum. */
	EHotKeyDecBrightness,

	/** Cycle focus screen from one to another in multiple screen environment. Start
	from current focused screen, switch to next the screen, and wraps around when it
	reaches the last screen.

	Default key mapping: \<Ctrl\>\<Alt\>\<Shift\> I. */
	EHotKeyCycleFocusScreen,

	/** Value for first hot key.

	Used with EHotKeyLastKeyType to make it easy to write a for loop that steps
	through all the different key values. */
	EHotKeyFirstKeyType=EHotKeyEnableLogging,		//Must always be set to the first one
	/** Value for last hot key.

	Used with EHotKeyFirstKeyType to make it easy to write a for loop that steps
	through all the different key values. */
	EHotKeyLastKeyType=EHotKeyCycleFocusScreen,		//Must always be set to the last one
	};


/** Password mode flags.

@publishedAll
@deprecated */
enum TPasswordMode
	{
	/** Releases ownership of password handling.

	This allows another window to become the password window. A client will be
	panicked with code 46 if it sets this mode when it is not the current password
	window. */
	EPasswordCancel,
	/** No password handling is performed by this window, but no other window
	can become the password window. */
	EPasswordNone,
	/** Password mode is enabled only once a day.

	When enabled, password mode requires the user to enter a password
	before any further actions can be performed. */
	EPasswordOnceADay,
	/** Password mode is enabled every time the machine is switched on. */
	EPasswordAlways,
	/** Equivalent to setting the password mode to EPasswordAlways and triggering an
	instant password check. */
	EPasswordAlwaysTriggerNow,
	/** Equivalent to setting the password mode to EPasswordOnceADay and triggering
	an instant password check. */
	EPasswordOnceADayTriggerNow,
	};
enum TPriorities {
           /**
           Defines the value EAllPriorities.
           */
           EAllPriorities=KMinTInt,
           };

/** Sprite flags.

These can be combined using a bit-wise OR operation.

@publishedAll
@released
@see RWsPointerCursor::Construct()
@see RWsSprite::Construct() */
enum TSpriteFlags
	{
	/** Flash sprite on and off.

	All flashing is done on the same timer, including the text cursor. */
	ESpriteFlash=0x1,
	/** This flag no longer has any effect.
	
	If you want a sprite to appear on top of all windows, you can create a floating sprite
	by specifying a RWindowGroup as parent to the sprite.  */
	ESpriteNoChildClip=0x2,
	/** The sprite's appearance will not change when it is on a shadowed part of the
	screen.

	(Typically this is used for sprites attached to window groups so that
	they are perceived to be floating above the windows). */
	ESpriteNoShadows=0x4
	};

struct TSpriteMember
/** Sprite member.

This structure is used to define the parameters of a sprite member, which
can be added to a sprite or pointer cursor.
@publishedAll
@released */
	{
	/** The bitmap to be displayed in the sprite or pointer cursor during the time
	interval specified by iInterval.

	If NULL, the sprite or pointer cursor will be invisible for the time specified
	by iInterval. */
	CFbsBitmap *iBitmap;
	/** The bitmap mask.

	This defines which areas of iBitmap are drawn to the screen.
	It enables the bitmap to have a non-rectangular shape on the screen. Areas
	which are not drawn to, retain the appearance of the window underneath the
	sprite or cursor.

	The bits in iBitmap are AND'ed with those in iMaskBitmap before being drawn
	to the screen. In practice, the mask bitmap is normally white in those areas
	drawn to by iBitmap, and black in all other areas.

	Note:

	This bitmap should be in the same mode as iBitmap, otherwise the masking
	out operation becomes inefficient. */
	CFbsBitmap *iMaskBitmap;
	/** EFalse if the mask bitmap is not to be inverted (the default) or ETrue if it
	is to be inverted. */
	TBool iInvertMask;
	/** Specifies the drawing mode to use when drawing iBitmap if the mask is NULL. */
	CGraphicsContext::TDrawMode iDrawMode;
	/** Offset from the sprite's central position.

	Specifies an offset between the origin of iBitmap and the sprite or pointer
	cursor's origin. (The origin is the top left corner of the bitmap.) */
	TPoint iOffset;
	/** Time interval for which iBitmap is displayed.

	Note that if only one sprite member is added to a sprite or pointer cursor,
	it will be displayed continuously, regardless of the value of iInterval. */
	TTimeIntervalMicroSeconds32 iInterval;
	};

/** Event reporting control.

This enumeration defines flags which can be used to control how events are
reported.

@publishedAll
@released
@see RWindowTreeNode::EnableOnEvents()
@see RWindowTreeNode::EnableModifierChangedEvents()
@see RWindowTreeNode::EnableErrorMessages() */
enum TEventControl
	{
	/** Requested events are sent in all circumstances. */
	EEventControlAlways,
	/** Events are delivered only when the requesting window's window group has keyboard
	focus. */
	EEventControlOnlyWithKeyboardFocus,
	/** Events are delivered only when the requesting window or any of its children
	are visible on the screen. */
	EEventControlOnlyWhenVisible
	};

/** Pointer event filter types.

These flags are used by RWindowBase::PointerFilter() to define which pointer
events are filtered out by the window server and not sent to the client session.
They also allow the window server to simulate a move event prior to each pen
down event.

@publishedAll
@released
@see RWindowBase::PointerFilter() */
enum TPointerFilter
	{
	//Basic Types
	/** Filters out both pointer enter and pointer exit events for this window. */
	EPointerFilterEnterExit=0x1,
	/** Filters out pointer-move events for this window. */
	EPointerFilterMove=0x2,
	/** Filters out pointer-drag events for this window. */
	EPointerFilterDrag=0x4,
	/** Simulates a pointer move event before each pen down event.

	This makes the pen appear more like a mouse, with simulated movement between
	two pointer events. You can only get these events while you are in pen mode. */
	EPointerGenerateSimulatedMove=0x8,
	//Combinations of the above
	/** Filters move and drag-pointer events from the event stream and simulates the
	move event before each pen down event.

	This enum is useful when you want to get simulated move events. Use it as shown
	below to turn on simulated moves:

	@code
	win->PointerFilter(EPointerMoveEvents, EPointerGenerateSimulateMoves)
	@endcode
	@see RWindowBase::PointerFilter() */
	EPointerMoveEvents=EPointerFilterMove|EPointerGenerateSimulatedMove,
	};

struct TKeyEvent
/** Key event details.

When processing a TKeyEvent, the TStdScanCode in iScanCode should usually
be ignored in favour of the TKeyCode in iCode. Using iScanCode would bypass
the keyboard mapping and any FEP that happens to be installed. The exceptions
to this general rule are games where the positions of the keys are more important
than their translations, and FEPs that are implementing keyboard maps themselves.
In these cases, if the iCode is used rather than iScanCode to determine the
key pressed, there will be two unfortunate consequences. Firstly, the low-level
keyboard mapping might re-arrange the mapping that you are trying to impose.
Secondly, you will subvert the CTRL+number method of entering Unicode literals.

@publishedAll
@released */
	{
	/** The character code generated for an EEventKey, or 0 for a down or up event.

	Key codes for special keys are defined in TKeyCode. */
	TUint iCode;
	/** The scan code of the key that caused the event.

	Standard scan codes are defined in TStdScanCode. */
	TInt iScanCode;
	/** State of modifier keys and pointing device. Modifier keys are defined in TEventModifier. */
	TUint iModifiers;
	/** Count of auto repeats generated.

	0 means an event without repeats. 1 or more means "this many auto repeat events".
	It is normal to ignore this value and treat it as a single event. */
	TInt iRepeats;
	};

struct TModifiersChangedEvent
/** Details of changed modifier keys.

@publishedAll
@released
@see TWsEvent::ModifiersChanged() */
	{
	/** Contains a set bit for any modifier which has changed. Modifiers are defined
	in TEventModifier. */
	TUint iChangedModifiers;
	/** Current state of all modifiers, combined using a bitwise-OR. Modifiers are
	defined in TEventModifier. */
	TUint iModifiers;
	};

struct TWsVisibilityChangedEvent
/** Visibility changed events.

These events are generated in response to a window being obscured, or being fully or partially
revealed after being obscured.
@publishedAll
@released
@see RWindowTreeNode::EnableVisibilityChangeEvents() */
		{
	enum
		{
		/** Some or all of the window is visible, either directly or through a transparent window.
		@deprecated Use EPartiallyVisible instead. */
		ECanBeSeen = 0x01,
		/** None of the window is visible, either directly or through a transparent window.
		@deprecated Use ENotVisible instead. */
		ECantBeSeen = 0x02,
		};
	enum
		{
		/** Some (or all) of the window is visible, either directly or through a transparent window. */
		EPartiallyVisible = 0x01,
		/** None of the window is visible, either directly or through a transparent window. */
		ENotVisible = 0x02,
		/** All of the window is directly visible. */
		EFullyVisible = 0x04,
		};
	/** A combination of one or more visibility event flags. */
	TUint iFlags;
	};

struct TWsDisplayChangedEvent
/** Display changed events.

These events are generated by attaching, detaching the display device, changing the resolution list or change
current resolution or backgound (change the current configuration).
@publishedAll
@released
@see MDisplayControl::EnableDisplayChangeEvents() */
	{
	/**
	 Number of display that has changed, causing this event. This is
	 also known as the screen number, and is zero-based.
	
	 @see CWsScreenDevice::Construct(TInt)
	 */
	TInt iDisplayNumber;
	/**
	 Opaque value that changes when the current display configuration
	 has changed.
	 
	 Compare values in subsequent events to determine whether the
	 current resolution has changed since the last event.
	 */
	TInt iConfigurationChangeId;
	/**
	 Opaque value that changes when the resolution list has changed.
	 
	 Compare values in subsequent events to determine whether the list
	 of available resolutions has changed since the last event.
	 
	 @see MDisplayControlBase::GetResolutions
	 */
	TInt iResolutionListChangeId;	
	};

struct TWsErrorMessage
/** Error event details.

@publishedAll
@released
@see RWindowTreeNode::EnableErrorMessages() */
	{
	/** Error event types */
	enum TErrorCategory
		{
		/** An error that occurs while the window server is calculating a drawing region
		for a particular window.

		When the window server can't calculate a drawing region due to lack of memory
		it goes into a mode where it draws the whole of a window. Any window that
		is covering this window is also told to redraw. */
		EDrawingRegion,
		/** An error occured while trying to turn the backlight on or off.

		This would usually only happen when the batteries are low. */
		EBackLight,
		/** An error occured while trying to enable logging. */
		ELogging,
		/** An error that occured while trying to change the contrast. */
		EContrast,
		};
	/** The error category. */
	TErrorCategory iErrorCategory;
	/** The error code */
	TUint iError;
	};

class TWsRedrawEvent
/** Window server redraw event.

All redraw events generated by the window server are delivered to the client
in an object of this type. The class provides a handle to the window which
generated the redraw event, and the rectangle which should be redrawn.

@publishedAll
@released
@see RWsSession::GetRedraw() */
	{
public:
	inline TUint Handle() const;
	inline TRect Rect() const;
public:
	void SetHandle(TUint aHandle);
	void SetRect(TRect aRect);
protected:
	TUint iHandle;	/**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	TRect iRect;	/**< Rectangle to redraw.  WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	};

class TWsPriorityKeyEvent
/** Priority key events.

These events are configured using RWindowGroup::AddPriorityKey(), and obtained
by the client using the functions RWsSession::PriorityKeyReady() and RWsSession::GetPriorityKey().

@publishedAll
@released */
	{
public:
	inline TKeyEvent *Key() const;
	inline TUint Handle() const;
	inline void SetHandle(TUint aHandle);
protected:
	TUint iHandle;	/**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	TUint8 iEventData[sizeof(TKeyEvent)];	/**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	};

class TWsEvent
/** Window server event.

All events generated by the window server, except redraw events and priority
key events, are delivered to the client in a TWsEvent. The client obtains
the TWsEvent by calling RWsSession::GetEvent(). The type of data contained
in the TWsEvent depends on the type of event. The client can inquire the event
type using Type(), and then retrieve the appropriate type of data.

@publishedAll
@released */
	{
public:
	/** Event data size. */
	enum
		{
		/**
		This is the maximum number of bytes of data that can be returned by EventData().
		Note: this value is 32 bytes.*/
		EWsEventDataSize=sizeof(TAdvancedPointerEvent)
		};
public:
	inline TWsEvent();
	inline TAdvancedPointerEvent* Pointer() const;
	inline TKeyEvent* Key() const;
	inline TModifiersChangedEvent* ModifiersChanged() const;
	inline TWsVisibilityChangedEvent* VisibilityChanged();
	inline const TWsVisibilityChangedEvent* VisibilityChanged() const;
	inline TWsDisplayChangedEvent* DisplayChanged();
	inline const TWsDisplayChangedEvent* DisplayChanged() const;
	inline TWsErrorMessage* ErrorMessage() const;
	inline TUint8* EventData() const;
	inline TInt Type() const;
	inline TUint Handle() const;
	inline TTime Time() const;
	inline void SetType(TInt aType);
	inline void SetHandle(TUint aHandle);
	inline void SetTimeNow();
	inline TInt* Int() const;
	IMPORT_C void InitAdvancedPointerEvent(TPointerEvent::TType aType, TUint aModifiers, const TPoint3D &aPoint3D, TUint8 aPointerNumber);
	IMPORT_C void SetPointerNumber(TUint8 aPointerNumber);
	IMPORT_C void SetPointerZ(TInt aZ);
protected:
	TInt iType;	/**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	TUint iHandle;	/**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	TTime iTime;	/**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	TUint8 iEventData[EWsEventDataSize];	/**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */	
	};


class RWsPointerCursor;

/** Pointer cursor mode flags.

These flags define the current cursor mode, and hence which sprite to use
for the pointer cursor.

@publishedAll
@released
@see RWsSession::SetPointerCursorMode()
@see RWsSession::PointerCursorMode() */
enum TPointerCursorMode
	{
	/** Don't have a pointer cursor. */
	EPointerCursorNone,
	/** The default system pointer cursor is used whenever there is a pointer cursor. */
	EPointerCursorFixed,
	/** The default system pointer cursor is used unless the window the pointer cursor
	is above specifies one, in which case that cursor is used. */
	EPointerCursorNormal,
	/** There is no pointer cursor unless the pointer is above a window that has specified
	one. */
	EPointerCursorWindow,
	/** First pointer cursor mode.

	This is used in test code for iterating though available modes. */
	EPointerCursorFirstMode=EPointerCursorNone,
	/** The final pointer cursor mode.

	This is used in test code for iterating though available modes. It will be
	updated if more modes are added. */
	EPointerCursorLastMode=EPointerCursorWindow,
	};

/** Flags which customise a long key press capture request made by a window group.

These flags are passed to RWindowGroup::CaptureLongKey().

Two things can be configured using this enum: the normal key event that is
generated immediately, and the long key event that is generated after a short
time interval. The normal key event can be sent immediately (on key down)
to the application with keyboard focus, or, if the key is not held down long
enough to generate a long key event, when the key is released. The long key
event can be customised to generate a single key event only, or to auto repeat.

@publishedAll
@released
@see RWindowGroup::CaptureLongKey() */
enum TLongCaptureFlags
	{
	/** If specified, the normal ("short") key event is sent immediately (on key down)
	to the application with keyboard focus. If not specified, the normal key event
	is sent when the key is released (if the key is not held down long enough
	to generate a long key event). */
	ELongCaptureShortEventImmediately=0x01,
	/** If specified, causes the long key press to automatically repeat. If not specified,
	the long key press will generate a single key event only. */
	ELongCaptureRepeatEvents=0x02,
	/** Equivalent to ELongCaptureShortEventImmediately being set and the other flags
	unset. */
	ELongCaptureNormal=ELongCaptureShortEventImmediately,
	/** Equivalent to the first two values being unset. */
	ELongCaptureWaitShort=0,
	};


class TIpcArgs;
class TReadDescriptorType;
class TWriteDescriptorType;
class CWindowGc;
class RWsSession;

class TWsGraphicId
/** Identifies an abstract artwork

This is a piece of artwork that might be owned and authored by another process.
A client can draw a piece of artwork, but that doesn't confer the ability to
access the pixel representation of that artwork directly.

@publishedAll
@released */
	{
public:
	enum TUninitialized
		{
		EUninitialized
		};
	TWsGraphicId(TUninitialized): iFlags(0) {}
	IMPORT_C TWsGraphicId(TUid aUid);
	IMPORT_C TWsGraphicId(TInt aId);
	IMPORT_C TWsGraphicId(const TWsGraphicId& aCopy);
	IMPORT_C TUid Uid() const;
	IMPORT_C TBool IsUid() const;
	IMPORT_C void Set(TUid aUid);
	IMPORT_C TInt Id() const;
	IMPORT_C TBool IsId() const;
	IMPORT_C void Set(TInt aId);
	IMPORT_C TInt Compare(const TWsGraphicId& aOther) const;
private:
	friend class CWsGraphic;
	TUint iFlags;
	TInt iId;
	TInt iWsGraphicIdSpare[2];
	};

class CWsGraphic: public CBase
/** Client representation of a window-server-side piece of artwork owned by this Client

@publishedAll
@released
*/
	{
protected:
	IMPORT_C CWsGraphic();
public:
	IMPORT_C ~CWsGraphic();
protected:
	// 2nd phase construction
	IMPORT_C void BaseConstructL(TUid aUid,TUid aType,const TDesC8& aData);
	IMPORT_C void BaseConstructL(TUid aType,const TDesC8& aData);
	IMPORT_C void BaseConstructL(const TWsGraphicId& aReplace,TUid aType,const TDesC8& aData);
	// sharing between clients
	IMPORT_C virtual TInt ShareGlobally();
	IMPORT_C virtual TInt Share(TSecureId aClientId);
	IMPORT_C virtual TInt UnShare(TSecureId aClientId);
	IMPORT_C virtual TInt UnShareGlobally();
public:
	IMPORT_C const TWsGraphicId& Id() const;
	IMPORT_C TBool IsActive() const;
	IMPORT_C void Destroy();
protected:
	IMPORT_C void SendMessage(const TDesC8& aData) const;
	IMPORT_C TInt SendSynchronMessage(const TDesC8& aData) const;
	IMPORT_C TInt Flush() const;
	IMPORT_C void SetGraphicExtension(MWsObjectProvider* aExt);
	IMPORT_C RWsSession& Session();
	
private:
	// events from wserv, to be implemented by derived classes
	/**
	Handles message from this graphic artwork peer on the server.
	@param aData Message from peer CWsGraphicDrawer.
	*/
	virtual void HandleMessage(const TDesC8& aData) = 0;
	
	/**
	Handles custom operation when this graphic artwork is replaced. When this method
	is invoked, the replaced peer CWsGraphicDrawer on the server has been destroyed.	
	*/
	virtual void OnReplace() = 0;
	IMPORT_C virtual void OnClientClose();
private:
	void BaseConstructL();
	IMPORT_C virtual TInt CWsGraphic_Reserved1();
	IMPORT_C virtual TInt CWsGraphic_Reserved2();
	IMPORT_C virtual TInt CWsGraphic_Reserved3();
public:
	class CManager;
private:
	friend class CManager;
	class CPimpl;
	friend class CPimpl;
	CPimpl* iPimpl;
	TInt iWsGraphicSpare[2];
};

class MWsClientClass
/** Base class for all classes whose objects are clients of the window server.

@publishedAll
@released */
	{
	friend class RTFXEffect;
public:
	MWsClientClass();
	MWsClientClass(RWsBuffer *aBuffer);
	inline TInt WsHandle() const;
protected:
	void Write(const TAny *aData1,TInt aLength1,TInt aLength2,TUint aOpcode) const;
	void Write(const TAny *aData, TInt aLength,const TAny *aData2, TInt aLength2,TUint aOpcode,const TIpcArgs* aIpcArgs=NULL) const;
	void Write(const TAny *aBuf, TInt aBufLen,TUint aOpcode,const TIpcArgs* aIpcArgs=NULL) const;
	void Write(TUint aOpcode) const;
	void WriteInt(TInt aInt, TUint aOpcode) const;
	void WriteRect(const TRect &aRect, TUint aOpcode) const;
	void WritePoint(const TPoint &aPoint, TUint aOpcode) const;
	void WriteSize(const TSize &aSize, TUint aOpcode) const;
	TInt WriteReply(TUint aOpcode,const TIpcArgs* aIpcArgs=NULL) const;
	TInt WriteReplyInt(TInt aInt, TUint aOpcode,const TIpcArgs* aIpcArgs=NULL) const;
	TInt WriteReply(const TAny *aBuf, TInt aBufLen,TUint aOpcode,const TIpcArgs* aIpcArgs=NULL) const;
	TInt WriteReply(const TAny *aBuf, TInt aBufLen,const TAny *aBuf2, TInt aBuflen2,TUint aOpcode,const TIpcArgs* aIpcArgs=NULL) const;
	TInt WriteReplyP(const TWriteDescriptorType& aReplyPackage,TUint aOpcode) const;
	TInt WriteReplyIntP(TInt aInt, const TWriteDescriptorType& aReplyPackage,TUint aOpcode) const;
	TInt WriteReplyP(const TAny *aBuf, TInt aBufLen, const TWriteDescriptorType& aReplyPackage,TUint aOpcode) const;
	TInt WriteReplyP(const TAny *aBuf, TInt aBufLen,const TAny *aBuf2, TInt aBuflen2, const TWriteDescriptorType& aReplyPackage,TUint aOpcode) const;
	TInt WriteReplyByProvidingRemoteReadAccess(const TAny* aBuf, TInt aBufLen,const TReadDescriptorType& aRemoteReadBuffer,TUint aOpcode) const;
	void AddToBitmapArray(const TInt aBitmapHandle)const;
	void AsyncRequest(TRequestStatus& aStatus, TUint aOpcode) const;

    TBool WindowSizeCacheEnabled() const;
    void MarkWindowSizeCacheDirty();
    void RefreshWindowSizeCache(const TSize& aNewSize) const;
    TInt CachedWindowSize(TSize& aSize) const;
    void DestroyWindowSizeCacheEntry();

private:
    void AppendData(const TAny *aData,TInt aLength,TBool aFinished);
	
protected:
	TInt32 iWsHandle;	/**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	RWsBuffer *iBuffer;	/**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	};


class RWindowTreeNode;
class CWsScreenDevice;
class CWsBitmap;
class RAnimDll;
class RWsSpriteBase;
class RDirectScreenAccess;
class RSoundPlugIn;


class RWsSession : public MWsClientClass, public RSessionBase
/** Window server session.

The session between the client and the window server can be used to mediate
asynchronous events, for client interface control and for system control.
A description of each of these capabilities is given below.

Mediating asynchronous events:

Primarily, the session mediates asynchronous events to the user. Three event
streams are available: the standard event stream which all applications must
use; the redraw event stream which must be used by all applications except
those which exclusively use backed-up windows; and the priority key event
stream which may be used for abort keys and the like for specialist applications.

All these events are mediated as standard asynchronous services. Typical window
server client programs encapsulate each service they require in an active
object whose RunL() identifies the event and calls the appropriate member
function of a class associated with an application framework, or a window.

Client interface control:

The client's interface with the window server has several aspects, each of
which is controlled through the window server session.

Flushing defines how requests to the window server are handled.

System control:

Many system-wide settings may be controlled through the window server session.
Typically, these functions are only used by the system shell and its associated
sessions/applications. They include: auto-repeat and double-click, querying
all window groups in the system, setting the default shadow vector, setting
the system pointer cursors, counting resources used by the window server
(this is only useful for debugging checks), getting and setting the state of
the modifier keys (for instance Shift and Ctrl), setting the window server
background colour, getting and setting thresholds for window server generated
pointer events.

@publishedAll
@released */
	{
	friend class RWsBuffer;
	friend class RWindowTreeNode;
	friend class CWsScreenDevice;
	friend class CWsBitmap;
	friend class RAnimDll;
	friend class RWsSpriteBase;
	friend class RDirectScreenAccess;
	friend class RSoundPlugIn;
	friend class CWsGraphic;
	friend class RWsDrawableSource;
public:
	/** Compute mode flags.

	When a window group takes focus or loses it, the window server can boost its
	client's thread or process priority to provide a better response to the user.
	How it alters the priority is determined by the current compute mode of the
	client.

	@see ComputeMode() */
	enum TComputeMode
		{
		/** Client priority is permanently set to its current level. It is not altered
		or set by the windowing system if the focus changes.

		Thus if ComputeMode() is called with this flag when a client is in the foreground,
		it will subsequently have foreground priority even if it is in the background. */
		EPriorityControlDisabled,
		/** Client process's priority is always set to EPriorityBackground. */
		EPriorityControlComputeOn,
		/** Client process's priority is set to EPriorityForeground when the window group
		takes focus, and set to EPriorityBackground when it loses focus.

		This is the default behaviour. */
		EPriorityControlComputeOff
		};
	enum {ESystemInfoArraySize=16};
	/** Window server logging commands passed to LogCommand().

	@see RWsSession::LogCommand() */
	enum TLoggingCommand
		{
		/** Enables logging. */
		ELoggingEnable,
		/** Disables logging. */
		ELoggingDisable,
		/** Logs the current status of all the windows in the tree, even if logging is
		not currently enabled. */
		ELoggingStatusDump,		//Window Tree Dump
		/** Logs information about the window server's heap size and usage, even if logging
		is not currently enabled. */
		ELoggingHeapDump,
		};

	struct TWindowGroupChainInfo
		{
		/*Stores id and Parent id window group

		This is used to define chained window groups's id and it's parent id.
		An array of these structures is returned by some of the varients of the function WindowGroupList.
		@publishedAll
		@released */
		TInt iId;
		TInt iParentId;
		};

	struct SSystemInfo
	/** WARNING: Struct for internal use ONLY.  Compatibility is not guaranteed in future releases. */ 
		{
		TInt iInfo[ESystemInfoArraySize];
		};
	/** The vertical alignment of a custom text cursor sprite.

	One of these values must be set, see SetCustomTextCursor(). This value also
	determines which part of each sprite member bitmap is clipped when the cursor's
	TTextCursor::EFlagClipVertical flag is set. */
	enum TCustomTextCursorAlignment
		{
		/** The y-origin of each sprite member bitmap is aligned with the top of the line. */
		ECustomTextCursorAlignTop,
		/** The y-origin of each sprite member bitmap is aligned with the baseline of the
		line. */
		ECustomTextCursorAlignBaseline,
		/** The y-origin of each sprite member bitmap is aligned with the bottom of the
		line. */
		ECustomTextCursorAlignBottom
		};
	
//
public:
	IMPORT_C RWsSession();
	IMPORT_C TInt Connect();
	IMPORT_C TInt Connect(RFs& aFileServer);
	IMPORT_C void Close();
	IMPORT_C TVersion Version() const;

	IMPORT_C TInt SetHotKey(THotKey aType, TUint aKeyCode, TUint aModifierMask, TUint aModifier);
	IMPORT_C TInt ClearHotKeys(THotKey aType);
	IMPORT_C TInt RestoreDefaultHotKey(THotKey aType);
//
	IMPORT_C void EventReady(TRequestStatus *aStat);
	IMPORT_C void EventReadyCancel();
	IMPORT_C void GetEvent(TWsEvent &aEvent) const;
	IMPORT_C void PurgePointerEvents();
//
	IMPORT_C void RedrawReady(TRequestStatus *aStat);
	IMPORT_C void RedrawReadyCancel();
	IMPORT_C void GetRedraw(TWsRedrawEvent &aEvent);
//
	IMPORT_C void PriorityKeyReady(TRequestStatus *aStat);
	IMPORT_C void PriorityKeyReadyCancel();
	IMPORT_C void GetPriorityKey(TWsPriorityKeyEvent &aEvent) const;
//
	IMPORT_C void Flush();
	IMPORT_C TBool SetAutoFlush(TBool aState);
	IMPORT_C TInt SetKeyboardRepeatRate(const TTimeIntervalMicroSeconds32 &aInitialTime, const TTimeIntervalMicroSeconds32 &aTime);
	IMPORT_C void GetKeyboardRepeatRate(TTimeIntervalMicroSeconds32 &aInitialTime, TTimeIntervalMicroSeconds32 &aTime) const;
	IMPORT_C void GetDoubleClickSettings(TTimeIntervalMicroSeconds32 &aInterval, TInt &aDistance) const;
	IMPORT_C TInt SetDoubleClick(const TTimeIntervalMicroSeconds32 &aInterval, TInt aDistance);
//
	IMPORT_C TInt NumWindowGroups() const;
	IMPORT_C TInt WindowGroupList(CArrayFixFlat<TInt> *aWindowList) const;
	IMPORT_C TInt WindowGroupList(RArray<TWindowGroupChainInfo>* aWindowList) const;
	IMPORT_C TInt NumWindowGroups(TInt aPriority) const;
	IMPORT_C TInt WindowGroupList(TInt aPriority, CArrayFixFlat<TInt> *aWindowList) const;
	IMPORT_C TInt WindowGroupList(TInt aPriority, RArray<TWindowGroupChainInfo>* aWindowList) const;
	IMPORT_C TInt GetFocusWindowGroup() const;
	IMPORT_C TInt GetDefaultOwningWindow() const;
	IMPORT_C TInt SetWindowGroupOrdinalPosition(TInt aIdentifier, TInt aPosition);
	IMPORT_C TInt GetWindowGroupClientThreadId(TInt aIdentifier, TThreadId &aThreadId) const;
	IMPORT_C TInt GetWindowGroupHandle(TInt aIdentifier) const;
	IMPORT_C TInt GetWindowGroupOrdinalPriority(TInt aIdentifier) const;
	IMPORT_C TInt SendEventToWindowGroup(TInt aIdentifier, const TWsEvent &aEvent);
	IMPORT_C TInt SendEventToAllWindowGroups(const TWsEvent &aEvent);
	IMPORT_C TInt SendEventToAllWindowGroups(TInt aPriority, const TWsEvent &aEvent);
	IMPORT_C TInt SendEventToOneWindowGroupsPerClient(const TWsEvent &aEvent);
	IMPORT_C TInt GetWindowGroupNameFromIdentifier(TInt aIdentifier, TDes &aWindowName) const;
	IMPORT_C TInt FindWindowGroupIdentifier(TInt aPreviousIdentifier,const TDesC& aMatch,TInt aOffset=0) const;
	IMPORT_C TInt FindWindowGroupIdentifier(TInt aPreviousIdentifier,TThreadId aThreadId) const;
	IMPORT_C TInt SendMessageToWindowGroup(TInt aIdentifier, TUid aUid, const TDesC8 &aParams);
	IMPORT_C TInt SendMessageToAllWindowGroups(TUid aUid, const TDesC8& aParams);
	IMPORT_C TInt SendMessageToAllWindowGroups(TInt aPriority, TUid aUid, const TDesC8& aParams);
	IMPORT_C TInt FetchMessage(TUid &aUid, TPtr8 &aParams, const TWsEvent& aMessageEvent) const;
//
	IMPORT_C void SetShadowVector(const TPoint &aVector);
	IMPORT_C TPoint ShadowVector() const;
//
	IMPORT_C void SetBackgroundColor(TRgb aColor);
	IMPORT_C TRgb GetBackgroundColor() const;
//
	IMPORT_C TInt RegisterSurface(TInt aScreenNumber, const TSurfaceId& aSurface);
	IMPORT_C void UnregisterSurface(TInt aScreenNumber, const TSurfaceId& aSurface);
	IMPORT_C TInt PreferredSurfaceConfigurationSize() const;
//
	IMPORT_C TInt SetSystemPointerCursor(const RWsPointerCursor &aPointerCursor,TInt aCursorNumber);
	IMPORT_C void ClearSystemPointerCursor(TInt aCursorNumber);
	IMPORT_C TInt ClaimSystemPointerCursorList();
	IMPORT_C void FreeSystemPointerCursorList();
//
	IMPORT_C TInt SetCustomTextCursor(TInt aIdentifier, const TArray<TSpriteMember>& aSpriteMemberArray, TUint aSpriteFlags, TCustomTextCursorAlignment aAlignment);
//
	IMPORT_C TInt ResourceCount() const;
	IMPORT_C void PasswordEntered();
	IMPORT_C void ComputeMode(TComputeMode aMode);
//
	IMPORT_C TInt HeapCount() const;
	IMPORT_C TInt DebugInfo(TInt aFunction, TInt aParam=0) const;
	IMPORT_C TInt DebugInfo(TInt aFunction, TDes8& aReturnBuf, TInt aParam=0) const;
	IMPORT_C void HeapSetFail(TInt aTAllocFail,TInt aValue);
	IMPORT_C void HeapSetBurstFail(TInt aTAllocFail,TInt aRate,TInt aBurst);
	IMPORT_C TInt SetModifierState(TEventModifier aModifier,TModifierState aState);
	IMPORT_C TInt GetModifierState() const;
//
	IMPORT_C TInt RequestOffEvents(TBool aOn,RWindowTreeNode *aWin=NULL);
	IMPORT_C TDisplayMode GetDefModeMaxNumColors(TInt& aColor,TInt& aGray) const;
	IMPORT_C TInt GetColorModeList(CArrayFixFlat<TInt> *aModeList) const;
//
	IMPORT_C void SetPointerCursorArea(const TRect& aArea);
	IMPORT_C void SetPointerCursorArea(TInt aScreenSizeMode,const TRect& aArea);
	IMPORT_C TRect PointerCursorArea() const;
	IMPORT_C TRect PointerCursorArea(TInt aScreenSizeMode) const;
	IMPORT_C void SetPointerCursorMode(TPointerCursorMode aMode);
	IMPORT_C TInt SetClientCursorMode(TPointerCursorMode aMode);
	IMPORT_C TPointerCursorMode PointerCursorMode() const;
	IMPORT_C void SetDefaultSystemPointerCursor(TInt aCursorNumber);
	IMPORT_C void ClearDefaultSystemPointerCursor();
	IMPORT_C TInt SetPointerCursorPosition(const TPoint& aPosition);
	IMPORT_C TPoint PointerCursorPosition() const;

	IMPORT_C void SetDefaultFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap);
	IMPORT_C void PrepareForSwitchOff();
	IMPORT_C void SetBufferSizeL(TInt aBufSize);
	IMPORT_C void SetMaxBufferSizeL(TInt aMaxBufSize);
	IMPORT_C TInt SetSystemFaded(TBool aFaded);
	IMPORT_C TInt SetSystemFaded(TBool aFaded,TUint8 aBlackMap,TUint8 aWhiteMap);

// Functions for multiple screens
	IMPORT_C TInt SetFocusScreen(TInt aScreenNumber);
	IMPORT_C TInt GetFocusScreen() const;
	IMPORT_C void ClearAllRedrawStores();
	IMPORT_C TInt NumWindowGroups(TInt aScreenNumber,TInt aPriority) const;
	IMPORT_C TInt NumberOfScreens() const;
	IMPORT_C TInt WindowGroupList(CArrayFixFlat<TInt>* aWindowList,TInt aScreenNumber,TInt aPriority=EAllPriorities) const;
	IMPORT_C TInt GetFocusWindowGroup(TInt aScreenNumber) const;
	IMPORT_C TInt GetDefaultOwningWindow(TInt aScreenNumber) const;
	IMPORT_C TDisplayMode GetDefModeMaxNumColors(TInt aScreenNumber,TInt& aColor,TInt& aGray) const;
	IMPORT_C TInt GetColorModeList(TInt aScreenNumber,CArrayFixFlat<TInt>* aModeList) const;
#if defined(__WINS__)
// Function for WINS behaviour only
	IMPORT_C void SetRemoveKeyCode(TBool aRemove);
	IMPORT_C void SimulateXyInputType(TInt aInputType);		//Only for testing WSERV
#endif
//
	IMPORT_C void SimulateRawEvent(TRawEvent aEvent);
	IMPORT_C void SimulateKeyEvent(TKeyEvent aEvent);
	IMPORT_C void LogCommand(TLoggingCommand aCommand);
	IMPORT_C void LogMessage(const TLogMessageText &aMessage);

// Functions for test code use only
	IMPORT_C void SystemInfo(TInt &aSystemInfoNumber, SSystemInfo &aSystemInfo);
	IMPORT_C void TestWrite(TInt aHandle,TInt aOpcode,const TAny *aData, TInt aLength);
	IMPORT_C void TestWriteReply(TInt aHandle,TInt aOpcode,const TAny *aData, TInt aLength);
	IMPORT_C void TestWriteReplyP(TInt aHandle,TInt aOpcode,const TAny *aData,TInt aLength,TDes8 *aReplyPackage);
	IMPORT_C TInt TestWriteReplyByProvidingRemoteReadAccess(TInt aHandle,TInt aOpcode,const TDesC8& aData,const TDesC8& aRemoteReadBuffer);
	IMPORT_C TInt TestWriteReplyByProvidingRemoteReadAccess(TInt aHandle,TInt aOpcode,const TDesC8& aData,const TDesC16& aRemoteReadBuffer);
	
	IMPORT_C TInt Finish();
	IMPORT_C void SyncMsgBuf();

// Getters and setters for pointer event's thresholds
	IMPORT_C TInt SetCloseProximityThresholds(TInt aEnterCloseProximityThreshold, TInt aExitCloseProximityThreshold);
	IMPORT_C TInt GetEnterCloseProximityThreshold() const;
	IMPORT_C TInt GetExitCloseProximityThreshold() const;
	IMPORT_C TInt SetHighPressureThresholds(TInt aEnterHighPressureThreshold, TInt aExitHighPressureThreshold);
	IMPORT_C TInt GetEnterHighPressureThreshold() const;
	IMPORT_C TInt GetExitHighPressureThreshold() const;
//
    IMPORT_C void EnableWindowSizeCacheL();
	IMPORT_C void SendEffectCommand(TInt aTfxCmd, const TDesC8& aTfxCmdData=KNullDesC8);

// Registering/Unregistering of TFX effect, to allow animations to be associated with each transition
	IMPORT_C void RegisterEffect(TInt aAction, TInt aPurpose, const TFileName& aResourceDir, const TFileName& aFilenameOutgoing, const TFileName& aFilenameIncoming, TUint aAppUid=0, TBitFlags aFlags=0);
	IMPORT_C void UnregisterEffect(TInt aAction, TInt aPurpose, TUint aAppUid=0);
	IMPORT_C void UnregisterAllEffects();
	IMPORT_C void OverrideEffects(TInt aAction, TInt aPurpose, const TFileName& aResourceDir, const TFileName& aFilenameOutgoing, const TFileName& aFilenameIncoming, TBitFlags aFlags=0);
	
//Application tells the window server the orientation of rendering it intends to use
	IMPORT_C void IndicateAppOrientation(TRenderOrientation aOrientation);
	
// functions not exported, used by CWsGraphic
	void GraphicMessageReady(TRequestStatus *aStat);
	void GetGraphicMessage(TDes8& aData) const;
	void GraphicMessageCancel();
	void GraphicAbortMessage(TInt aError);
	TInt GraphicFetchHeaderMessage();
private:
	TInt doWindowGroupList(TInt aPriority, RArray<TWindowGroupChainInfo>* aWindowListCh, TInt aNumOpcode, TInt aListOpcode) const;
	TInt doWindowGroupList(TInt aScreenNumber, TInt aPriority, CArrayFixFlat<TInt>* aWindowListId, TInt aNumOpcode, TInt aListOpcode) const;
	TInt doSetHotKey(TInt aOpcode, TInt aType, TUint aKeycode, TUint aModifierMask, TUint aModifiers);
	void doReadEvent(TRequestStatus *aStat, TInt aOpcode);

private:
	void connectL();
	TInt DoFlush(const TIpcArgs& aIpcArgs);
	TInt DoSyncMsgBuf(const TIpcArgs& aIpcArgs);
	void DirectAcessActivation(TBool aIsNowActive);
	};

class RWindowGroup;
class RWsSprite;


class RWindowTreeNode : public MWsClientClass
/** A handle to a node in the server-side window tree.

This handle class is used as the base for both window groups and the windows
within them; it is not intended for user derivation, and may not be directly
constructed: use one of the standard derived classes instead.

@publishedAll
@released */
	{
	friend class RWindowGroup;
	friend class RWindowBase;
	friend class CWindowGc;
	friend class RAnimDll;
	friend class RWsSprite;
public:
	/** Fade control flags.

	These flags define whether fading is applied to all child windows when SetFaded()
	is called on a window. */
	enum TFadeControl
		{
		/** Include all child windows when fading/un-fading. */
		EFadeIncludeChildren=ETrue,
		/** Fade only the parent window.

		Note that a panic will occur if this value is used when SetFaded() is called
		on a window group. */
		EFadeWindowOnly=EFalse,
		};
protected:
	RWindowTreeNode();
	RWindowTreeNode(RWsSession &aWs);
public:
	IMPORT_C void Close();
	IMPORT_C void Destroy();
	IMPORT_C TUint32 ClientHandle() const;
	IMPORT_C TUint32 Parent() const;
	IMPORT_C TUint32 PrevSibling() const;
	IMPORT_C TUint32 NextSibling() const;
	IMPORT_C TUint32 Child() const;
	IMPORT_C TInt OrdinalPriority() const;
	IMPORT_C TInt OrdinalPosition() const;
	IMPORT_C TInt FullOrdinalPosition() const;
	IMPORT_C void SetOrdinalPosition(TInt aPos);
	IMPORT_C void SetOrdinalPosition(TInt aPos,TInt aOrdinalPriority);
	IMPORT_C TInt ScreenNumber() const;	
	IMPORT_C TInt WindowGroupId() const;
	IMPORT_C TInt SetPointerCursor(TInt aCursorNumber);
	IMPORT_C void SetCustomPointerCursor(const RWsPointerCursor &aPointerCursor);
	IMPORT_C TInt EnableOnEvents(TEventControl aCircumstances=EEventControlOnlyWithKeyboardFocus);
	IMPORT_C void DisableOnEvents();
	IMPORT_C TInt EnableGroupChangeEvents();
	IMPORT_C void DisableGroupChangeEvents();
	IMPORT_C TInt EnableFocusChangeEvents();
	IMPORT_C void DisableFocusChangeEvents();
	IMPORT_C TInt EnableGroupListChangeEvents();
	IMPORT_C void DisableGroupListChangeEvents();
	IMPORT_C TInt EnableVisibilityChangeEvents();
	IMPORT_C void DisableVisibilityChangeEvents();
	IMPORT_C TInt EnableErrorMessages(TEventControl aCircumstances);
	IMPORT_C void DisableErrorMessages();
	IMPORT_C TInt EnableModifierChangedEvents(TUint aModifierMask, TEventControl aCircumstances);
	IMPORT_C void DisableModifierChangedEvents();
	IMPORT_C void SetNonFading(TBool aNonFading);
	IMPORT_C void SetFaded(TBool aFaded,TFadeControl aIncludeChildren);
	IMPORT_C void SetFaded(TBool aFaded,TFadeControl aIncludeChildren,TUint8 aBlackMap,TUint8 aWhiteMap);
	IMPORT_C void ClearPointerCursor();
	IMPORT_C RWsSession* Session() const;
protected:
	__DECLARE_TEST;
	};


class TSurfaceId;
class TSurfaceConfiguration;

class RWindowBase : public RWindowTreeNode
/** Client-side handle to a server-side window.

The server-side windows of handles derived from this class can be displayed
on the device's screen, and associated with pointer events.

This class is not for user derivation; however derived classes form part of
the Window Server API.

@publishedAll
@released */
	{
public:
	/** Capture behaviour flags.

	@see SetPointerCapture()
	@see EEventDragDrop */
	enum TCaptureFlags
		{
// Flags, these can be combined to customise pointer capture
		/** If set, capture is enabled, disabled otherwise. */
		TCaptureFlagEnabled=0x01,
		/** If set, drag-drop events are sent to the window in which the pointer was lifted.
		Drag-drop is a tap-drag-lift sequence. */
		TCaptureFlagDragDrop=0x02,
		/** Capture events from windows belonging to any group, not just the group of the
		window on which the flag was set */
		TCaptureFlagAllGroups=0x04,
// States, made up of a set of flags
		/** Equivalent to disabling all the above flags. */
		TCaptureDisabled=0,
		/** Equivalent to TCaptureFlagEnabled. */
		TCaptureEnabled=TCaptureFlagEnabled,
		/** Equivalent to the OR of the first three flags. */
		TCaptureDragDrop=TCaptureFlagEnabled|TCaptureFlagDragDrop|TCaptureFlagAllGroups,
		};
protected:
	RWindowBase();
	RWindowBase(RWsSession &aWs);
public:
	IMPORT_C void Activate();
	IMPORT_C void SetPosition(const TPoint &point);
	IMPORT_C TInt SetSizeErr(const TSize &size);
	IMPORT_C TInt SetExtentErr(const TPoint &point,const TSize &size);
	IMPORT_C TSize Size() const;
	IMPORT_C TPoint InquireOffset(const RWindowTreeNode &aWindow) const;
	IMPORT_C void PointerFilter(TUint32 aFilterMask, TUint32 aFilter);
	IMPORT_C void SetPointerGrab(TBool aState);
	IMPORT_C void ClaimPointerGrab(TBool aSendUpEvent=ETrue);
	IMPORT_C TInt ClaimPointerGrab(const TUint8 aPointerNumber, const TBool aSendUpEvent=ETrue);
	IMPORT_C void SetPointerCapture(TInt aFlags);
	IMPORT_C void SetPointerCapturePriority(TInt aPriority);
	IMPORT_C TInt GetPointerCapturePriority() const;
	IMPORT_C void SetVisible(TBool aState);
	IMPORT_C void SetShadowHeight(TInt aHeight);
	IMPORT_C void SetShadowDisabled(TBool aState);
	IMPORT_C TPoint Position() const;
	IMPORT_C TPoint AbsPosition() const;
	IMPORT_C TInt SetCornerType(TCornerType aCornerType, TInt aCornerFlags=0);
	IMPORT_C TInt SetShape(const TRegion &aRegion);
	IMPORT_C TInt SetRequiredDisplayMode(TDisplayMode aMode);
	IMPORT_C TDisplayMode DisplayMode() const;
	IMPORT_C void EnableBackup(TUint aBackupType=EWindowBackupAreaBehind);
	IMPORT_C void RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32 aTime,const TRect &aRect);
	IMPORT_C void CancelPointerRepeatEventRequest();
	IMPORT_C TInt RequestPointerRepeatEvent(TTimeIntervalMicroSeconds32 aTime,const TRect &aRect, const TUint8 aPointerNumber);	//< @prototype
	IMPORT_C TInt CancelPointerRepeatEventRequest(const TUint8 aPointerNumber);	//< @prototype
	IMPORT_C TInt AllocPointerMoveBuffer(TInt aMaxPoints, TUint aFlags);
	IMPORT_C void FreePointerMoveBuffer();
	IMPORT_C void EnablePointerMoveBuffer();
	IMPORT_C void DisablePointerMoveBuffer();
	IMPORT_C TInt RetrievePointerMoveBuffer(TDes8 &aBuf) const;
	IMPORT_C void DiscardPointerMoveBuffer();
	IMPORT_C TInt AddKeyRect(const TRect &aRect, TInt aScanCode, TBool aActivatedByPointerSwitchOn);
	IMPORT_C void RemoveAllKeyRects();
	IMPORT_C TInt PasswordWindow(TPasswordMode aPasswordMode);
	IMPORT_C void FadeBehind(TBool aFade);
	IMPORT_C TBool IsFaded() const;
	IMPORT_C TBool IsNonFading() const;
	IMPORT_C TInt MoveToGroup(TInt aIdentifier);
	IMPORT_C TInt SetBackgroundSurface(const TSurfaceId& aSurface);
	IMPORT_C TInt SetBackgroundSurface(const TSurfaceConfiguration& aConfiguration, TBool aTriggerRedraw);
	IMPORT_C void RemoveBackgroundSurface(TBool aTriggerRedraw);
	IMPORT_C TInt GetBackgroundSurface(TSurfaceConfiguration& aConfiguration) const;
	IMPORT_C void SetSurfaceTransparency(TBool aSurfaceTransparency);
	IMPORT_C TRgb KeyColor() const;
	IMPORT_C void EnableAdvancedPointers();
	IMPORT_C void SetPurpose(TInt aPurpose);
	IMPORT_C void SendEffectCommand(TInt aTfxCmd, const TDesC8& aTfxCmdData=KNullDesC8);
	IMPORT_C void OverrideEffects(TInt aAction, const TFileName& aResourceDir, const TFileName& aFilenameOutgoing, const TFileName& aFilenameIncoming, TBitFlags aFlags=0);
    IMPORT_C TInt FixNativeOrientation();
    IMPORT_C TSize SizeForEgl() const;
protected:
	TInt construct(const RWindowTreeNode &parent,TUint32 aHandle, TInt aType, TDisplayMode aDisplayMode);
	};


class RBlankWindow : public RWindowBase
/** Blank window.

A blank window may have its extent and colour changed, but nothing else may
be drawn to it. Blank windows provide no functionality that is not provided
by the RWindow class. However, they are useful when the application requires
a window that cannot be drawn to, for example a container window.

@publishedAll
@released */
	{
public:
	IMPORT_C RBlankWindow();
	IMPORT_C RBlankWindow(RWsSession &aWs);
	IMPORT_C TInt Construct(const RWindowTreeNode &parent, TUint32 aHandle);
	IMPORT_C void SetColor(TRgb aColor);
	IMPORT_C void SetColor();
	IMPORT_C void SetSize(const TSize &size);
	IMPORT_C void SetExtent(const TPoint &point,const TSize &size);
	};


class RDrawableWindow : public RWindowBase
/** Handle to a drawable window.

This class cannot be constructed directly.
The two concrete derived classes, RWindow and RBackedUpWindow, allow
drawing but use different protocols because RWindow relies on the client to
redraw invalid areas.

@publishedAll
@released */
	{
protected:
	RDrawableWindow();
	RDrawableWindow(RWsSession &aWs);
public:
	IMPORT_C void Scroll(const TPoint &aOffset);
	IMPORT_C void Scroll(const TRect &aClipRect, const TPoint &aOffset);
	IMPORT_C void Scroll(const TPoint &aOffset, const TRect &aRect);
	IMPORT_C void Scroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect);
	IMPORT_C TRect GetDrawRect() const;
protected:
	void SetDrawRect(const TRect &aRect);
private:
	void doScroll(const TRect &aClipRect, const TPoint &aOffset, const TRect &aRect, TInt aOpcode);

	TRect iDrawRect;
	};

/** Transparency policies

These policies are to control the behaviour of a transparent window.
@publishedAll
@released
@see RWindow::SetTransparencyPolicy() */
enum TWsTransparencyPolicy {
	/** Default behaviour. */
	ETransparencyDefault,
	/** Freeze the content of underneath the transparent window to prevent redraw everytime
	the overlying window changes. */
	ETransparencyFreezeUnder,
	};

class RWindow : public RDrawableWindow
/** Handle to a standard window.

An RWindow is a handle to a server-side window which can be displayed and
drawn to, and whose redraws are performed by the application.

@publishedAll
@released */
	{
public:
	IMPORT_C RWindow();
	IMPORT_C RWindow(RWsSession &aWs);
	IMPORT_C TInt Construct(const RWindowTreeNode &parent,TUint32 aHandle);
	IMPORT_C void BeginRedraw();
	IMPORT_C void BeginRedraw(const TRect &aRect);
	IMPORT_C void EndRedraw();
	IMPORT_C void Invalidate();
	IMPORT_C void Invalidate(const TRect &aRect);
	IMPORT_C void GetInvalidRegion(RRegion &aRegion) const;
	IMPORT_C void SetBackgroundColor(TRgb aColor);
	IMPORT_C void SetBackgroundColor();
	IMPORT_C void SetSize(const TSize &size);
	IMPORT_C void SetExtent(const TPoint &point,const TSize &size);
	IMPORT_C void HandleTransparencyUpdate();
	IMPORT_C TInt SetTransparencyFactor(const TRgb& aTransparencyFactor);
	IMPORT_C TInt SetTransparencyBitmap(const CFbsBitmap& aTransparencyBitmap);
	IMPORT_C TInt SetTransparencyWsBitmap(const CWsBitmap& aTransparencyBitmap);
	IMPORT_C void SetNonTransparent();
	IMPORT_C void EnableRedrawStore(TBool aEnabled);
	IMPORT_C TInt SetTransparencyAlphaChannel();
	IMPORT_C TInt SetTransparentRegion(const TRegion& aRegion);
	IMPORT_C TInt SetTransparencyPolicy(TWsTransparencyPolicy aPolicy);
	IMPORT_C TBool IsRedrawStoreEnabled() const;
	IMPORT_C void EnableOSB(TBool);
	IMPORT_C void ClearRedrawStore();
	};


class RBackedUpWindow : public RDrawableWindow
/** Client-side handle to a drawable window with a backup bitmap.

A backed-up window is associated with a backup bitmap which retains the contents
of the window. When an area of the window is invalidated, the window server
uses the bitmap directly to update the window without requiring an application
redraw. The backup bitmap is owned by the font and bitmap server and resides
on the font and bitmap server's shared heap, which is accessible to all client
programs: functions are provided to allow the client application to access
this bitmap directly and perform updates to the window under application control.

@publishedAll
@deprecated
*/
	{
public:
	IMPORT_C RBackedUpWindow();
	IMPORT_C RBackedUpWindow(RWsSession &aWs);
	IMPORT_C TInt Construct(const RWindowTreeNode &parent,TDisplayMode aDisplayMode, TUint32 aHandle);
	IMPORT_C TInt BitmapHandle() const;
	IMPORT_C void UpdateScreen();
	IMPORT_C void UpdateScreen(const TRegion &aRegion);
	IMPORT_C void UpdateBackupBitmap();
	IMPORT_C void MaintainBackup();
	};


class RWindowGroup : public RWindowTreeNode
/** Client-side handle to a server-side window group.

@publishedAll
@released */
	{
public:
	IMPORT_C RWindowGroup();
	IMPORT_C RWindowGroup(RWsSession &aWs);
	IMPORT_C TInt Construct(TUint32 aClientHandle);
	IMPORT_C TInt Construct(TUint32 aClientHandle,CWsScreenDevice* aScreenDevice);
	IMPORT_C TInt Construct(TUint32 aClientHandle,TBool aIsFocusable,CWsScreenDevice* aScreenDevice);
	IMPORT_C TInt Construct(TUint32 aClientHandle,TBool aIsFocusable);
	IMPORT_C TInt ConstructChildApp(TInt aIdOfParentWindowGroup,TUint32 aClientHandle);
	IMPORT_C TInt ConstructChildApp(TInt aIdOfParentWindowGroup,TUint32 aClientHandle,TBool aIsFocusable);
	IMPORT_C void AllowProcessToCreateChildWindowGroups(TUid aProcessSID);
	IMPORT_C void EnableReceiptOfFocus(TBool aState);
	IMPORT_C void AutoForeground(TBool aState);
	IMPORT_C void SetOrdinalPriorityAdjust(TInt aAdjust);
	IMPORT_C TInt SetOrdinalPositionErr(TInt aPos,TInt aOrdinalPriority);
	IMPORT_C TInt32 CaptureKey(TUint aKeycode, TUint aModifierMask, TUint aModifier);
	IMPORT_C TInt32 CaptureKey(TUint aKeycode, TUint aModifierMask, TUint aModifier, TInt aPriority);
	IMPORT_C void CancelCaptureKey(TInt32 aCaptureKey);
	IMPORT_C TInt32 CaptureKeyUpAndDowns(TUint aScanCode, TUint aModifierMask, TUint aModifier);
	IMPORT_C TInt32 CaptureKeyUpAndDowns(TUint aScanCode, TUint aModifierMask, TUint aModifier, TInt aPriority);
	IMPORT_C void CancelCaptureKeyUpAndDowns(TInt32 aCaptureKey);
	IMPORT_C TInt32 CaptureLongKey(TUint aInputKeyCode,TUint aOutputKeyCode,TUint aModifierMask,TUint aModifier
																										,TInt aPriority,TUint aFlags);
	IMPORT_C TInt32 CaptureLongKey(TTimeIntervalMicroSeconds32 aRepeatTime,TUint aInputKeyCode,TUint aOutputKeyCode
																	,TUint aModifierMask,TUint aModifier,TInt aPriority,TUint aFlags);
	IMPORT_C void CancelCaptureLongKey(TInt32 aCaptureKey);
	IMPORT_C TInt AddPriorityKey(TUint aKeycode, TUint aModifierMask, TUint aModifier);
	IMPORT_C void RemovePriorityKey(TUint aKeycode, TUint aModifierMask, TUint aModifier);
	IMPORT_C void SetTextCursor(RWindowBase &aWin, const TPoint &aPos, const TTextCursor &aCursor);
	IMPORT_C void SetTextCursor(RWindowBase &aWin, const TPoint &aPos, const TTextCursor &aCursor, const TRect &aClipRect);
	IMPORT_C void CancelTextCursor();
	IMPORT_C void SetOwningWindowGroup(TInt aIdentifier);
	IMPORT_C void DefaultOwningWindow();
	IMPORT_C TInt SetName(const TDesC &aName);
	IMPORT_C TInt Name(TDes &aWindowName) const;
	IMPORT_C TInt Identifier() const;
	IMPORT_C void DisableKeyClick(TBool aState);
	IMPORT_C TInt EnableScreenChangeEvents();
	IMPORT_C void DisableScreenChangeEvents();
	IMPORT_C void SimulatePointerEvent(TRawEvent aEvent);
	IMPORT_C void SimulateAdvancedPointerEvent(TRawEvent aEvent);
	IMPORT_C TInt ClearChildGroup();
	IMPORT_C TInt SetChildGroup(TInt aId);
private:
	TInt32 doCaptureKey(TUint aKey, TUint aModifierMask, TUint aModifiers, TInt aPriority, TInt aOpcode);
	void doCancelCaptureKey(TInt32 aCaptureKeyHandle, TInt aOpcode);
	TInt Construct(TInt aIdOfParentWindowGroup, TUint32 aClientHandle, TBool aIsFocusable, CWsScreenDevice* aScreenDevice);
	};


class CWsBitmap : public CFbsBitmap, public MWsClientClass
/** Window server bitmap.

This is a bitmap to which the window server already has a handle. Functions
which take a window server bitmap are faster than equivalent functions which
take a CFbsBitmap.

@publishedAll
@released */
	{
public:
	IMPORT_C CWsBitmap();
	IMPORT_C CWsBitmap(RWsSession &aWs);
	IMPORT_C ~CWsBitmap();
	IMPORT_C TInt Create(const TSize& aSizeInPixels,TDisplayMode aDispMode);
	IMPORT_C TInt Duplicate(TInt aHandle);
	IMPORT_C TInt Load(const TDesC& aFileName,TInt32 aId,TBool aShareIfLoaded=ETrue);
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void Reset();
private:
	TInt createWsBitmap(TInt aErr);
	};

class RWsGraphicMsgBuf;

class TWsGraphicMsgFixedBase
/** Base class for fixed-size messages to be attached to CWindowGc::DrawWsGraphic commands
@publishedAll
@released
*/	{
public:
	IMPORT_C TPtrC8 Pckg() const;
	IMPORT_C TUid TypeId() const;
	IMPORT_C TInt Size() const;
protected:
	IMPORT_C TWsGraphicMsgFixedBase(TUid aTypeId,TInt aSizeOfDerived);
private:
	const TUid iTypeId;
	const TInt iSize;
	};

class RWsGraphicMsgBuf: private RBuf8
/** Utility class for building data buffers to be attached to CWindowGc::DrawWsGraphic commands
Useful when there is more than one piece of data to be attached to the draw command
@publishedAll
@released */
	{
public:
	IMPORT_C RWsGraphicMsgBuf();
	using RBuf8::Close;
	using RBuf8::CleanupClosePushL;
	IMPORT_C TInt Append(TUid aTypeId,const TDesC8& aData);
	IMPORT_C TInt Append(TUid aTypeId,const TDesC16& aData);
	IMPORT_C TInt Append(TUid aTypeId,TInt aLen,TPtr8& aPtr);
	IMPORT_C TInt Append(const TWsGraphicMsgFixedBase& aMsg);
	IMPORT_C void Remove(TInt aIndex);
	IMPORT_C TInt Count() const;
	IMPORT_C TUid TypeId(TInt aIndex) const;
	IMPORT_C TPtrC8 Data(TInt aIndex) const;
	IMPORT_C TPtr8 Data(TInt aIndex);
	IMPORT_C void GetFixedMsg(TWsGraphicMsgFixedBase& aMsg,TInt aIndex) const;
	IMPORT_C const TDesC8& Pckg() const;
private:
	TInt ExpandForAppend(TInt aDataLen);
	void WriteHeader(TUid aUid,TInt aLen);
	TInt IntAt(TInt aOfs) const;
private:
	TInt iRMsgBufSpare[2];
	};


class CWindowGc : public CBitmapContext, public MWsClientClass
/** Window graphics context.

Most window graphics context drawing functions map to equivalent CFbsBitGc
functions. They are implemented on the screen with any co-ordinates being
relative to the top left corner of the window. However extra clipping is applied.
The drawing will always be clipped to the visible part of the window. In addition
it will be clipped to the non-invalid part if you are not doing a redraw and
to the region being validated if you are doing a redraw.

Note:

In general, server side functions which encounter conditions which would normally
cause a leave, do not leave but instead return an error value indicating the
leave condition. In this way the leave can be handled on the appropriate side
of the client/server boundary. For example, a client can choose to wrap server
calls with User::LeaveIfError(), which causes a leave with the specified
error.

The above advice is true of many functions in this class, and in its derived
classes.

@publishedAll
@released
@see CGraphicsContext */
	{
	friend class CWsScreenDevice;
public:
	/** Defines possible clockwise rotation values.
	
	WARNING: Enum for internal and partner use ONLY.  Compatibility is not guaranteed in future releases.

	@prototype
	*/
	enum TGraphicsRotation
		{
		/** No rotation. */
		EGraphicsRotationNone,
		/** A 90 degree rotation. */
		EGraphicsRotation90,
		/** A 180 degree rotation. */
		EGraphicsRotation180,
		/** A 270 degree rotation. */
		EGraphicsRotation270
		};	

	IMPORT_C CWindowGc(CWsScreenDevice *aDevice);
	IMPORT_C virtual ~CWindowGc();
	IMPORT_C virtual TInt Construct();
	IMPORT_C virtual void Activate(RDrawableWindow &aDevice);
	IMPORT_C virtual void Deactivate();
//====================FROM CGraphicsContext.H===============================
	IMPORT_C virtual CGraphicsDevice* Device() const;
	IMPORT_C virtual void SetOrigin(const TPoint &aPoint=TPoint(0,0));
	IMPORT_C virtual void SetDrawMode(TDrawMode aDrawingMode);
	IMPORT_C virtual void SetClippingRect(const TRect& aRect);
	IMPORT_C virtual void CancelClippingRect();
	IMPORT_C virtual void Reset();

	IMPORT_C virtual void UseFont(const CFont *aFont);
	IMPORT_C virtual void DiscardFont();
	IMPORT_C virtual void SetUnderlineStyle(TFontUnderline aUnderlineStyle);
	IMPORT_C virtual void SetStrikethroughStyle(TFontStrikethrough aStrikethroughStyle);
// Used to do justified text properly
	IMPORT_C virtual void SetWordJustification(TInt aExcessWidth,TInt aNumGaps);
// Used primarily to get accurate WYSIWYG
	IMPORT_C virtual void SetCharJustification(TInt aExcessWidth,TInt aNumChars);

	IMPORT_C virtual void SetPenColor(const TRgb &aColor);
	IMPORT_C virtual void SetPenStyle(TPenStyle aPenStyle);
	IMPORT_C virtual void SetPenSize(const TSize& aSize);

	IMPORT_C virtual void SetBrushColor(const TRgb &aColor);
	IMPORT_C virtual void SetBrushStyle(TBrushStyle aBrushStyle);
	IMPORT_C virtual void SetBrushOrigin(const TPoint &aOrigin);
	IMPORT_C virtual void UseBrushPattern(const CFbsBitmap *aDevice);
	IMPORT_C virtual void DiscardBrushPattern();
//
// Move the internal position, as used by DrawLineTo & DrawLineBy, and set by MoveTo,
// MoveBy, DrawLine, DrawLineTo, DrawLineBy and DrawPolyline
	IMPORT_C virtual void MoveTo(const TPoint &aPoint);
	IMPORT_C virtual void MoveBy(const TPoint &aPoint);
	IMPORT_C virtual void Plot(const TPoint &aPoint);
//
//  Line drawing subject to pen color, width and style and draw mode
	IMPORT_C virtual void DrawArc(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd);
	IMPORT_C virtual void DrawLine(const TPoint &aPoint1,const TPoint &aPoint2);
	IMPORT_C virtual void DrawLineTo(const TPoint &aPoint);
	IMPORT_C virtual void DrawLineBy(const TPoint &aPoint);
	IMPORT_C virtual void DrawPolyLine(const CArrayFix<TPoint> *aPointList);
	IMPORT_C virtual void DrawPolyLine(const TPoint* aPointList,TInt aNumPoints);
//
// Filled outlines
// Outlines subject to pen color, width and style and draw mode
// (set pen to ENullPen for no outline)
// Fill subject to brush style (color, hash or pattern) and origin and
// drawing mode (set brush to ENullBrush for no fill)
	IMPORT_C virtual void DrawPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd);
	IMPORT_C virtual void DrawEllipse(const TRect &aRect);
	IMPORT_C virtual void DrawRect(const TRect &aRect);
	IMPORT_C virtual void DrawRoundRect(const TRect &aRect,const TSize &aEllipse);
	IMPORT_C virtual TInt DrawPolygon(const CArrayFix<TPoint> *aPointList,TFillRule aFillRule=EAlternate);
	IMPORT_C virtual TInt DrawPolygon(const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule=EAlternate);
//
// Uses the bitmap's Twips size and does a "stretch" blit in general
	IMPORT_C virtual void DrawBitmap(const TPoint &aTopLeft, const CFbsBitmap *aDevice);
	IMPORT_C virtual void DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice);
	IMPORT_C virtual void DrawBitmap(const TRect &aDestRect, const CFbsBitmap *aDevice, const TRect &aSourceRect);
	IMPORT_C virtual void DrawBitmapMasked(const TRect& aDestRect, const CFbsBitmap* aBitmap, const TRect& aSourceRect, const CFbsBitmap* aMaskBitmap, TBool aInvertMask);
	IMPORT_C virtual void DrawBitmapMasked(const TRect& aDestRect, const CWsBitmap* aBitmap, const TRect& aSourceRect, const CWsBitmap* aMaskBitmap, TBool aInvertMask);

// Required as not all DrawText and DrawTextVertical functions are implemented in CWindowGc
	using CBitmapContext::DrawText;
	using CBitmapContext::DrawTextVertical;
//
// Text drawing subject to drawing mode
// Subject to used font, pen color, drawing mode,
// word and char justification
	IMPORT_C virtual void DrawText(const TDesC &aBuf,const TPoint &aPos);
//
// Subject to same as above plus brush for background
// (set brush to ENullBrush for no effect on background)
	IMPORT_C virtual void DrawText(const TDesC &aBuf,const TRect &aBox,TInt aBaselineOffset,TTextAlign aHoriz=ELeft,TInt aLeftMrg=0);
//================Extra functions from CBitmapContext==============
	IMPORT_C virtual void Clear();
	IMPORT_C virtual void Clear(const TRect &aRect);
	IMPORT_C virtual void CopyRect(const TPoint &anOffset,const TRect &aRect);
	IMPORT_C virtual void BitBlt(const TPoint &aPos, const CFbsBitmap *aBitmap);
	IMPORT_C virtual void BitBlt(const TPoint &aDestination, const CFbsBitmap *aBitmap, const TRect &aSource);
	IMPORT_C virtual void BitBltMasked(const TPoint& aPoint,const CFbsBitmap* aBitmap,const TRect& aSourceRect,const CFbsBitmap* aMaskBitmap,TBool aInvertMask);
	IMPORT_C virtual void BitBlt(const TPoint &aPoint, const CWsBitmap *aBitmap);
	IMPORT_C virtual void BitBlt(const TPoint &aDestination, const CWsBitmap *aBitmap, const TRect &aSource);
	IMPORT_C virtual void BitBltMasked(const TPoint& aPoint,const CWsBitmap *aBitmap,const TRect& aSourceRect,const CWsBitmap *aMaskBitmap,TBool aInvertMask);
	IMPORT_C virtual void MapColors(const TRect& aRect,const TRgb* aColors,TInt aNumPairs=2,TBool aMapForwards=ETrue);

	IMPORT_C virtual void DrawTextVertical(const TDesC& aText,const TPoint& aPos,TBool aUp);
	IMPORT_C virtual void DrawTextVertical(const TDesC& aText,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
//=================Extra functions specific to wserv GDI==============
	IMPORT_C virtual void SetDitherOrigin(const TPoint& aPoint);
	IMPORT_C virtual TInt SetClippingRegion(const TRegion &aRegion);
	IMPORT_C virtual void CancelClippingRegion();
	IMPORT_C virtual void SetOpaque(TBool aDrawOpaque=ETrue);
//=================Functions also supplied by CFbsBitGc==============
	IMPORT_C virtual void SetFaded(TBool aFaded);
	IMPORT_C virtual void SetFadingParameters(TUint8 aBlackMap,TUint8 aWhiteMap);
	IMPORT_C virtual TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CFbsBitmap* aSrcBmp, const TRect& aSrcRect, const CFbsBitmap* aAlphaBmp, const TPoint& aAlphaPt);
	IMPORT_C virtual TInt AlphaBlendBitmaps(const TPoint& aDestPt, const CWsBitmap* aSrcBmp, const TRect& aSrcRect, const CWsBitmap* aAlphaBmp, const TPoint& aAlphaPt);
//
	IMPORT_C TAny* Interface(TUid aInterfaceId);
	IMPORT_C const TAny* Interface(TUid aInterfaceId) const;
	
protected:	
	IMPORT_C TInt APIExtension(TUid aUid, TAny*& aOutput, TAny* aInput);
	
private:	

	IMPORT_C void Reserved_CGraphicsContext_2();

	IMPORT_C void Reserved_CBitmapContext_1();
	IMPORT_C void Reserved_CBitmapContext_2();
	IMPORT_C void Reserved_CBitmapContext_3();

public:
	IMPORT_C virtual void DrawWsGraphic(const TWsGraphicId& aId,const TRect& aDestRect);
	IMPORT_C virtual void DrawWsGraphic(const TWsGraphicId& aId,const TRect& aDestRect,const TDesC8& aData);
private:
	IMPORT_C virtual void Reserved_CWindowGc_3();
	IMPORT_C virtual void Reserved_CWindowGc_4();
	IMPORT_C virtual void Reserved_CWindowGc_5();

//=============================================================
private: // Private code
	TRgb Color(TInt aOpcode)const;
	void SetJustification(TInt aExcessWidth,TInt aNumGaps, TInt aOpcode);
	void DrawArcOrPie(const TRect &aRect,const TPoint &aStart,const TPoint &aEnd, TInt aOpcode);
	void doDrawPolyLine(const CArrayFix<TPoint> *aPointArray, const TPoint* aPointList,TInt aNumPoints);
	TInt doDrawPolygon(const CArrayFix<TPoint> *aPointArray,const TPoint* aPointList,TInt aNumPoints,TFillRule aFillRule);
	void WriteTextPos(TInt aOpcode,TInt aOpcodePtr,const TPoint &aPos,const TDesC &aBuf) const;
	void WriteTextCommand(TAny *aCmd, TInt aLen,const TDesC &aBuf,TInt aOpcode,TInt aOpcodePtr) const;
	void WriteTextCommand(TAny *aCmd, TInt aLen,const TDesC8 &aBuf,TInt aOpcode,TInt aOpcodePtr) const;
	TInt APIExGetUnderlineMetrics(TAny*& aOutput);
	TInt APIExSetShadowColor(TAny* aShadowColor);
	TInt APIExGetShadowColor(TAny*& aOutput);
	// New DrawText API's that take in context
	TInt APIExDrawText(const TDesC &aBuf,const TTextParameters* aParam,const TPoint &aPos);
	TInt APIExDrawText(const TDesC &aBuf,const TTextParameters* aParam,const TRect &aBox,TInt aBaselineOffset,TTextAlign aHoriz=ELeft,TInt aLeftMrg=0);
	TInt APIExDrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TPoint& aPos,TBool aUp);
	TInt APIExDrawTextVertical(const TDesC& aText,const TTextParameters* aParam,const TRect& aBox,TInt aBaselineOffset,TBool aUp,TTextAlign aVert=ELeft,TInt aMargin=0);
	TInt APIExInterface(TAny*& aInterface, TUid aInterfaceId);

	void DrawResource(const TPoint& aPos, const RWsDrawableSource& aSource, TGraphicsRotation aRotation=EGraphicsRotationNone);
	void DrawResource(const TRect& aDestRect, const RWsDrawableSource& aSource, TGraphicsRotation aRotation=EGraphicsRotationNone);
	void DrawResource(const TRect& aDestRect, const RWsDrawableSource& aSource, const TRect& aSrcRect, TGraphicsRotation aRotation=EGraphicsRotationNone);
	void DrawResource(const TRect& aDestRect, const RWsDrawableSource& aSource, const TDesC8& aParam);
	
private: // Private data
	class CPimpl;
	CPimpl* iPimpl;
	CWsScreenDevice *iDevice;
	};


class CWsScreenDevice : public CBitmapDevice, public MWsClientClass
/** Software device screen.

An object of this class can be used to enquire or set parameters of the software
screen, and determine the physical limits of the device.

CWsScreenDevice gives access to the list of screen modes. These are the combinations
of screen rotation and screen size supported by the device. One of them is
selected as the current screen mode. The possible screen modes are initialised
in wsini.ini.

@publishedAll
@released
@see CFbsScreenDevice */
	{
public:
	/**	Whether or not to include sprites when comparing areas of the screen*/
	enum TSpriteInCompare
		{
		/** This enum can be specified as the 3rd parameter to CWsScreenDevice::RectCompare(const TRect &aRect1,
		const TRect &aRect2,TUint aFlags).
		When called with the value ERemoveSprite if there are any sprites visible in the areas of the screen being compared then these
		will not be included in the comparison.
		The function CWsScreenDevice::RectCompare(const TRect &aRect1,const TRect &aRect2) will not include
		sprites in the comparison.
		*/
		ERemoveSprite=0,
		/**	This enum can be specified as the 3rd parameter to CWsScreenDevice::RectCompare(const TRect &aRect1,
		const TRect &aRect2,TUint aFlags).
		When called with the value EIncludeSprite if there are any sprites visible in the areas of the screen being compared then these
		will be included in the comparison.*/
		EIncludeSprite=1,
		/**	This enum can be specified as the 3rd parameter to CWsScreenDevice::RectCompare(const TRect &aRect1,
		const TRect &aRect2,TUint aFlags). 
		When called with the value EIncludeTextCursor if there are any text cursors visible in the areas of the screen being compared then these 
		will be included in the comparison.*/
		EIncludeTextCursor=2
		};
public:
	IMPORT_C CWsScreenDevice();
	IMPORT_C CWsScreenDevice(RWsSession &aWs);
	IMPORT_C ~CWsScreenDevice();
	IMPORT_C TInt Construct();
	IMPORT_C TInt Construct( TInt aDefaultScreenNumber ) ;
	IMPORT_C TAny* GetInterface(TUint aInterfaceId);
	inline const TAny* GetInterface(TUint aInterfaceId) const;
	IMPORT_C TBool IsModeDynamic(TInt aSizeMode) const;
	IMPORT_C TBool IsCurrentModeDynamic() const;
//==== From CGraphicsDevice ====//
	IMPORT_C TDisplayMode DisplayMode() const;
	IMPORT_C TSize SizeInPixels() const;
	IMPORT_C TSize SizeInTwips() const;
	IMPORT_C TInt HorizontalTwipsToPixels(TInt aTwips) const;
	IMPORT_C TInt VerticalTwipsToPixels(TInt aTwips) const;
	IMPORT_C TInt HorizontalPixelsToTwips(TInt aPixels) const;
	IMPORT_C TInt VerticalPixelsToTwips(TInt aPixels) const;
	IMPORT_C TRect PointerRect() const;
	IMPORT_C TInt CreateContext(CGraphicsContext *&aGc);
	inline TInt CreateContext(CWindowGc*& aGc);
	IMPORT_C virtual TInt GetNearestFontInTwips(CFont*& aFont,const TFontSpec& aFontSpec);
	IMPORT_C virtual TInt GetNearestFontToDesignHeightInTwips(CFont*& aFont,const TFontSpec& aFontSpec);
	IMPORT_C virtual TInt GetNearestFontToMaxHeightInTwips(CFont*& aFont,const TFontSpec& aFontSpec,TInt aMaxHeight);
	IMPORT_C TInt NumTypefaces() const;
	IMPORT_C void TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const;
	IMPORT_C TInt FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const;
// ==== From CBitmapDevice ====//
	IMPORT_C void GetPixel(TRgb& aColor,const TPoint& aPixel) const;
	IMPORT_C void GetScanLine(TDes8& aScanLine,const TPoint& aStartPixel,TInt aPixelLength, TDisplayMode aDispMode) const;
	IMPORT_C TInt AddFile(const TDesC& aName,TInt& aId);
	IMPORT_C void RemoveFile(TInt aId=0);
	IMPORT_C virtual TInt GetNearestFontInPixels(CFont*& aFont,const TFontSpec& aFontSpec);
	IMPORT_C virtual TInt GetNearestFontToDesignHeightInPixels(CFont*& aFont,const TFontSpec& aFontSpec);
	IMPORT_C virtual TInt GetNearestFontToMaxHeightInPixels(CFont*& aFont,const TFontSpec& aFontSpec,TInt aMaxHeight);
	IMPORT_C TInt FontHeightInPixels(TInt aTypefaceIndex,TInt aHeightIndex) const;
	IMPORT_C void ReleaseFont(CFont* aFont);
//===== From CFbsScreenDevice ====//
	IMPORT_C void PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const;
	IMPORT_C void SetPalette(CPalette* aPalette);
	IMPORT_C TInt GetPalette(CPalette*& aPalette) const;
//===== Extra functions ====//
	IMPORT_C TInt SetCustomPalette(const CPalette* aPalette);
	IMPORT_C TInt GetFontById(CFont*& aFont,TUid aUid,const TAlgStyle& aAlgStyle);
	IMPORT_C TBool RectCompare(const TRect &aRect1,const TRect &aRect2) const;
	IMPORT_C TBool RectCompare(const TRect& aRect1,const TRect &aRect2,TUint aFlags) const;
	IMPORT_C TInt CopyScreenToBitmap(const CFbsBitmap *aBitmap) const;
	IMPORT_C TInt CopyScreenToBitmap(const CFbsBitmap *aBitmap, const TRect &aRect) const;
	IMPORT_C TInt SetBackLight(TBool aBackLight);
//===== Screen Rotation functions ====//
	IMPORT_C void SetScreenSizeAndRotation(const TPixelsTwipsAndRotation &aSizeAndRotation);
	IMPORT_C void GetDefaultScreenSizeAndRotation(TPixelsTwipsAndRotation &aSizeAndRotation) const;
	IMPORT_C void SetScreenSizeAndRotation(const TPixelsAndRotation &aSizeAndRotation);
	IMPORT_C void GetDefaultScreenSizeAndRotation(TPixelsAndRotation &aSizeAndRotation) const;
	IMPORT_C TScreenModeEnforcement ScreenModeEnforcement() const;
	IMPORT_C void SetScreenModeEnforcement(TScreenModeEnforcement aMode) const;
	IMPORT_C void GetScreenModeSizeAndRotation(TInt aMode, TPixelsTwipsAndRotation &aSizeAndRotation) const;
	IMPORT_C void GetScreenModeSizeAndRotation(TInt aMode, TPixelsAndRotation &aSizeAndRotation) const;
	IMPORT_C void SetCurrentRotations(TInt aMode, CFbsBitGc::TGraphicsOrientation aRotation) const;
	IMPORT_C TInt GetRotationsList(TInt aMode, CArrayFixFlat<TInt> *aRotationList) const;
	IMPORT_C TInt NumScreenModes() const;
	IMPORT_C void SetScreenMode(TInt aMode);
	IMPORT_C TInt CurrentScreenMode() const;
	IMPORT_C void SetAppScreenMode(TInt aMode);
	IMPORT_C TPoint GetDefaultScreenModeOrigin() const;
	IMPORT_C TPoint GetScreenModeOrigin(TInt aMode) const;
	IMPORT_C TSize GetCurrentScreenModeScale() const;
	IMPORT_C TSize GetScreenModeScale(TInt aMode) const;
	IMPORT_C TPoint GetCurrentScreenModeScaledOrigin() const;
	IMPORT_C TPoint GetScreenModeScaledOrigin(TInt aMode) const;
	IMPORT_C TInt GetScreenSizeModeList(RArray<TInt>* aModeList) const;
	IMPORT_C TDisplayMode GetScreenModeDisplayMode(const TInt &aMode) const;
//===== Used for testing purpose only ====//
	IMPORT_C TSizeMode GetCurrentScreenModeAttributes() const;
	IMPORT_C void SetCurrentScreenModeAttributes(const TSizeMode &aModeAtt);
	IMPORT_C TInt GetScreenNumber() const;
private:
	CFbsTypefaceStore* TypeFaceStore()const;
	TSize DisplaySizeInPixels()const;
	TSize PhysicalScreenSizeInTwips()const;
private: // Private data
	class CScrDevExtension;				//Implements extension interfaces, including and replacing typeface store
	CScrDevExtension* iExtension;
	TSize iPhysicalScreenSizeInTwips;
	TSize iDisplaySizeInPixels;
	friend class CWindowGc;
	};


class RWsSpriteBase : public MWsClientClass
// Client side class base class for sprites and pointer cursors
/** Sprite base class.

Unless stated otherwise, references to sprites in the documentation for this class also
apply to pointer cursors.

@publishedAll
@released
@see RWsSprite
@see RWsPointerCursor */
	{
protected:
	IMPORT_C RWsSpriteBase();
	IMPORT_C RWsSpriteBase(RWsSession &aWs);
public:
	IMPORT_C TInt Activate();
	IMPORT_C TInt AppendMember(const TSpriteMember &aMemberData);
	IMPORT_C void UpdateMember(TInt aIndex);
	IMPORT_C TInt UpdateMember(TInt aIndex, const TSpriteMember &aMemberData);
	IMPORT_C void Close();
	};


class RWsSprite : public RWsSpriteBase
/** Sprite.

This class defines functions to create and initialise a sprite, and to set
its position.

@publishedAll
@released */
	{
public:
	IMPORT_C RWsSprite();
	IMPORT_C RWsSprite(RWsSession &aWs);
	IMPORT_C TInt Construct(RWindowTreeNode &aWindow, const TPoint &aPos, TInt aFlags);
	IMPORT_C void SetPosition(const TPoint &aPos);
	};


class RWsPointerCursor : public RWsSpriteBase
/** Pointer cursor.

The pointer cursor is created in a similar way to a sprite. After creating it,
one or more sprite members containing the bitmaps to be displayed are added.

After a pointer cursor has been created and activated, it does not become
visible until the application calls RWindowTreeNode::SetPointerCursor() or
RWindowTreeNode::SetCustomPointerCursor(). The pointer cursor's origin automatically
tracks the position of the pointing device, or position of emulated single pointer if there
are more pointers in the system (see documentation of RWindowBase::EnableAdvancedPointers()
for more details), and the origin forms the pointer cursor's "hot spot", i.e., the point 
whose co-ordinates are sent to the client if a pointer event occurs. 
If the pointer cursor's bitmap needs to extend to the left or upwards from the pointer 
position, its sprite member should be given a negative offset in TSpriteMember::iOffset.

Note:

Pointer cursors are rarely used in pure pen architectures, but they are supported
for mouse or tablet use.

@see RWindowBase::EnableAdvancedPointers()

@publishedAll
@released */
	{
public:
	IMPORT_C RWsPointerCursor();
	IMPORT_C RWsPointerCursor(RWsSession &aWs);
	IMPORT_C TInt Construct(TInt aFlags);
	};


class RAnim;


class RAnimDll : public MWsClientClass
/** Client-side interface to the server-side animation DLL.

This is the client side class matching the server side CAnimDLL class. It
provides functions to create, load, close and destroy a server-side CAnimDll
object.

@publishedAll
@released
@see CreateCAnimDllL()
@see CAnimDll */
	{
public:
	IMPORT_C RAnimDll();
	IMPORT_C virtual ~RAnimDll();
	IMPORT_C RAnimDll(RWsSession &aWs);
	IMPORT_C TInt Load(const TDesC &aFileName);
	IMPORT_C void Destroy();
	IMPORT_C virtual void Close();
private:
	TInt CommandReply(TInt aHandle, TInt aOpcode, const TIpcArgs* aIpcArgs=NULL);
	TInt CommandReply(TInt aHandle, TInt aOpcode, const TDesC8& aArgs, const TIpcArgs* aIpcArgs=NULL);
	void Command(TInt aHandle, TInt aOpcode, const TPtrC8 &aArgs);
	void Command(TInt aHandle, TInt aOpcode);
	void AsyncCommandReply(TRequestStatus& aStatus, TInt aOpcode, TIpcArgs& aIpcArgs);
	TInt CreateInstance(TInt32& aHandle, const MWsClientClass &aDevice, TInt aType, const TDesC8 &aArgs, TInt aOpcode, const TIpcArgs* aIpcArgs);
	void DestroyInstance(TInt aHandle);
private:
	friend class RAnim;
	};


class RAnim
/** Client-side handle to a server-side animation class.

This class should be sub-classed to provide a client side interface to the
server side animation DLL. The server side animation DLL is derived from CAnim.

@publishedAll
@released */
	{
public:
	IMPORT_C virtual ~RAnim();
	IMPORT_C virtual void Close();
	IMPORT_C void Destroy();
protected:
	IMPORT_C RAnim();
	IMPORT_C RAnim(RAnimDll &aDll);
	IMPORT_C TInt Construct(const RWindowBase &aDevice, TInt aType, const TDesC8 &aParams);
	IMPORT_C TInt Construct(const RWindowBase &aDevice, TInt aType, const TDesC8 &aParams, const TIpcArgs& aIpcArgs);
	IMPORT_C TInt Construct(const RWsSprite &aDevice, TInt aType, const TDesC8 &aParams);
	IMPORT_C TInt Construct(const RWsSprite &aDevice, TInt aType, const TDesC8 &aParams, const TIpcArgs& aIpcArgs);
	IMPORT_C TInt CommandReply(TInt aOpcode);
	IMPORT_C TInt CommandReply(TInt aOpcode, const TPtrC8 &aArgs);
	IMPORT_C TInt CommandReply(TInt aOpcode, const TDesC8& aArgs, const TIpcArgs& aIpcArgs);
	IMPORT_C void Command(TInt aOpcode, const TPtrC8 &aArgs);
	IMPORT_C void Command(TInt aOpcode);
	IMPORT_C void AsyncCommandReply(TRequestStatus& aRequestStatus,TInt aOpcode, const TIpcArgs& aIpcArgs);
private:
	TInt32 iHandle;
	RAnimDll *iAnimDll;
	};

class CDsaMsgQueue;

class RDirectScreenAccess : public MWsClientClass
/** The interface between an application that directly accesses the screen and
the window server.

Note that most applications that need to access the screen directly should
use CDirectScreenAccess instead. RDirectScreenAccess only needs to be used
directly by applications without access to an active scheduler.

@publishedAll
@released */
	{
public:
	/** Provides the reason why direct screen access must terminate. This enum is used
	in the MAbortDirectScreenAccess::AbortNow() and MDirectScreenAccess::Restart()
	functions.

	The first value (ETerminateCancel) indicates that direct screen access is
	being terminated by the application. The final three values indicate that
	direct screen access is being terminated by the window server. Note that
	for users of CDirectScreenAccess, the termination code is not important
	because these issues are dealt with by CDirectScreenAccess::StartL(). */
	enum TTerminationReasons
		{
		/** The application has finished direct screen access. */
		ETerminateCancel,
		/** A window is about to come in front of a part of the area that is being used
		for direct screen access. */
		ETerminateRegion,
		/** The screen's color depth (as enumerated by TDisplayMode) is about to change. */
		ETerminateScreenMode,
		/** The current screen mode (the combination of screen rotation and screen size)
		is about to change. */
		ETerminateRotation,
		};
	/** The priority of the active object that responds to notification that direct
	screen access must stop. */
	enum TPriority
		{
		/** A suggested value for the priority of the active object that responds to notification
		from the window server that direct screen access must stop. This is also the
		value used by CDirectScreenAccess for this purpose. */
		EPriorityVeryHigh=2000,
		};
public:
	IMPORT_C RDirectScreenAccess();
	IMPORT_C RDirectScreenAccess(RWsSession& aWs);
	IMPORT_C TInt Construct();
	IMPORT_C TInt Construct(TBool aRegionTrackingOnly);
	IMPORT_C TInt Request(RRegion*& aRegion,TRequestStatus& aStatus,const RWindowBase& aWindow);
	IMPORT_C void Completed();
	IMPORT_C void Cancel();
	IMPORT_C void Close();
private:
	RWsSession* iWs;
	CDsaMsgQueue* iMsgQueue;
	// To maintain BC we need the following
	RThread iWsThread;
	TRequestStatus* iDummyRequest;
	};


class MAbortDirectScreenAccess
/** The interface for terminating direct screen access.

An object of a class which implements this interface should be passed to CDirectScreenAccess::NewL().

@publishedAll
@released
@see MDirectScreenAccess */
	{
public:
	/** This function is called by the window server when direct screen access must
	stop (for example because a dialogue is moved in front of the area where direct
	screen access is taking place).

	In response to this, direct screen access must stop immediately. In simple cases,
	this will involve cancelling the active object that is driving the drawing to the
	screen.

	No attempt to call a Window Server API function can be made from
	AbortNow(), because then a temporary deadlock will occur. This is because WSERV
	is waiting to receive the client's acknowledgment that it has aborted, and so will
	not be able to service the call. As soon as the restriction no longer applies,
	the function Restart() will be called.

	@param aReason The reason why direct screen access was terminated. */
	virtual void AbortNow(RDirectScreenAccess::TTerminationReasons aReason)=0;
	};


class MDirectScreenAccess : public MAbortDirectScreenAccess
/** The interface for restarting direct screen access.

The Restart() function is called by the window server as soon as direct screen
access can resume. It is called after the window server has called AbortNow().

An object of the derived class is passed to CDirectScreenAccess::NewL().

@publishedAll
@released
@see CDirectScreenAccess::NewL() */
	{
public:
	/** This function is called by the window server as soon as direct screen access
	can resume.

	This function should call CDirectScreenAccess::StartL() within a trap harness.
	If this leaves, e.g. through lack of memory, direct screen access cannot be
	restarted. StartL() re-calculates the clipping region, so that if direct screen
	access was aborted because another window appeared in front of it, that window
	will not be overwritten when direct screen access resumes.

	In this function, you can resume calls to Window Server Client Side API functions.

	@param aReason Provides the reason why direct screen access was terminated. */
	virtual void Restart(RDirectScreenAccess::TTerminationReasons aReason)=0;
	};


class CDirectScreenAccess : public CActive
/** An active object used to start direct screen access.

Direct screen access is a way of drawing to the screen without using the window
server. As this avoids client-server communication, it is much faster, and
may be useful for games and video. Note that some interaction with the window
server is needed in order to prevent the application from drawing over other
application's data.

The object's (private) RunL() function is called by the window server in order
to abort direct screen access. This might occur when another window needs
to be displayed in front or when the window with direct screen access is moved.
The active object's priority is RDirectScreenAccess::EPriorityVeryHigh so
that direct screen access will be aborted as quickly as possible.

@publishedAll
@released */
	{
public:
	IMPORT_C static CDirectScreenAccess* NewL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWin,MDirectScreenAccess& aAbort);
	IMPORT_C static CDirectScreenAccess* NewL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWin,MDirectScreenAccess& aAbort,TBool aRegionTrackingOnly);
	~CDirectScreenAccess();
	IMPORT_C void StartL();
	inline CFbsBitGc* Gc();
	inline CFbsScreenDevice*& ScreenDevice();
	inline RRegion* DrawingRegion();
private:
	enum TFlags
		{
		EDirectCheckModeChange		=	0x01,
		EDirectCheckSizeModeChange	=	0x02,
		EDirectRegionTrackingOnly	=	0x04,
		};
private:
	inline CDirectScreenAccess(RWsSession& aWs,CWsScreenDevice* aScreenDevice,RWindowBase& aWindow,MDirectScreenAccess& aAbort);
	void ConstructL(RWsSession& aWs,TBool aRegionTrackingOnly);
	void CreateScreenObjectsL(TDisplayMode aCurrentMode);
	void UpdateSizeAndRotation(CFbsBitGc* aGc);
	static TInt Restart(TAny* aDirect);
	void Restart();
	//Pure virtual functions from CActive
	void DoCancel();
	void RunL();
private:
	CFbsBitGc* iGc;
	CFbsScreenDevice* iScreenDevice;
	RRegion* iDrawingRegion;
	RWindowBase& iWindow;
	CWsScreenDevice* iWsScreenDevice;
	MDirectScreenAccess& iAbort;
	RDirectScreenAccess iDirectAccess;
	RDirectScreenAccess::TTerminationReasons iReason;
	CIdle* iRestart;
	TSize iScreenSize;
	TBool iAborting;
	TUint iFlags;
	TInt iScreenNumber;
	};


class RSoundPlugIn : public MWsClientClass
/** Client interface to the key or pointer click plug-in DLL.

The plug-in DLL implements the CClickMaker class.

@publishedAll
@released
@see CClickMaker */
	{
public:
	/** A possible return value for CommandReply(). */
	enum
		{
		/** The value returned by CommandReply() if no plug-in is loaded,
		or if the plug-in identified by the aUid parameter in Construct() is not loaded.
		Its value is set to KMinTInt so that it should not clash with any other expected
		return value from this function. */
		ESoundWrongPlugIn=KMinTInt
		};
public:
	IMPORT_C RSoundPlugIn();
	IMPORT_C RSoundPlugIn(RWsSession &aWs);
	IMPORT_C TInt Construct(TUid aUid=KNullUid);
	IMPORT_C void Close();
	IMPORT_C void Destroy();
	IMPORT_C TBool IsLoaded(TBool& aIsChangeable) const;
	IMPORT_C TInt Unload();
	IMPORT_C TInt Load(const TDesC &aFileName);
	IMPORT_C void SetKeyClick(TBool aEnabled);
	IMPORT_C void SetPenClick(TBool aEnabled);
	IMPORT_C TBool KeyClickEnabled() const;
	IMPORT_C TBool PenClickEnabled() const;
	IMPORT_C TInt CommandReply(TInt aOpcode, const TPtrC8 &aArgs);
	};

inline TInt MWsClientClass::WsHandle() const
	/** Gets the server side handle for the object.

	@return The server-side handle for the object. */
	{return(iWsHandle);}

inline const TAny* CWsScreenDevice::GetInterface(TUint aInterfaceId) const
	{
	return const_cast<CWsScreenDevice*>(this)->GetInterface(aInterfaceId);
	}

inline TInt CWsScreenDevice::CreateContext(CWindowGc *&aGc)
	/** Creates a graphics context for this device.

	This function always causes a flush of the window server buffer.

	@param aGc On successful return, contains a new graphics context referring
	to this screen device.
	@return KErrNone if successful, otherwise one of the system-wide error codes.
	@see CGraphicsDevice::CreateContext() */
	{return(CreateContext((CGraphicsContext *&)aGc));}

inline TUint TWsRedrawEvent::Handle() const
	/** Gets the handle of the window which is the target of the redraw event.

	This is the handle that was passed to the window's Construct() function: typically
	it will be a pointer to the object owning the window, providing access to
	its member functions.

	@return The window's handle. */
	{return(iHandle);}

inline TRect TWsRedrawEvent::Rect() const
	/** Gets the rectangle to redraw.

	@return The rectangle that needs redrawing. Co-ordinates are relative to
	the window whose handle is given by Handle(). */
	{return(iRect);}

inline TWsEvent::TWsEvent()
	/** Constructor.  Zero Initialise Data
	@publishedAll
	@released */
	{
	iType=0;
	iHandle=0;
	iTime=0;
	Mem::FillZ(iEventData,EWsEventDataSize);
	}

inline TAdvancedPointerEvent* TWsEvent::Pointer() const
	/** Gets the pointer event.
	
	This method can be used to get information about the pointer event if Type()
	returns an event of type EEventPointer or EEventDragDrop.
	
	If the event has been received by the window without advanced pointer events enabled,
	this method will return a pointer to TPointerEvent with additional fields of
	TAdvancedPointerEvent being cleared to 0.
	
	@return Structure containing advanced pointer event data.
	@see TPointerEvent::AdvancedPointerEvent()
	@see RWindowBase::EnableAdvancedPointers() */
	{return((TAdvancedPointerEvent *)&iEventData);}

inline TInt TAdvancedPointerEvent::PointerNumber() const
/** Gets the pointer number of the pointer whose state change is described by this event.

  As soon as the pointer (for example a finger) is detected by the device's sensors,
  it is assigned a pointer number. Then all events related to this pointer always
  use the same pointer number. When the device loses track of the pointer,
  TPointerEvent::EOutOfRange is sent with its pointer number and the number is released
  - from this time it may be re-used to identify another pointer coming into range.  
  
  On a particular device, the pointer number is always an integer in the range 0 to 
  HALData::EPointerNumberOfPointers - 1. If the device doesn't provide value for
  this attribute, it is assumed to be 1.
  
  Please note that in order to receive events from multiple pointers in a window, the method
  RWindowBase::EnableAdvancedPointers() has to be called for this window's instance
  before it is activated. Otherwise this window will only receive TPointerEvents from one
  emulated pointer, which will always have pointer number 0.
  
  @return Pointer number of the pointer whose state change is described by this event.
  @see RWindowBase::EnableAdvancedPointers()
  @see HALData::EPointerNumberOfPointers */
	{
	return IsAdvancedPointerEvent() ? DoGetPointerNumber() : ENonAdvancedPointerPointerNumber;
	}

inline TInt TAdvancedPointerEvent::Proximity() const
/** Gets the proximity of the pointer to the screen's surface.
  Proximity units may vary between devices and may be non-linear.
  Returned value will be a negative integer as the maximum supported proximity
  range in Symbian OS is from KMinTInt to 0, where KMinTInt means the highest
  proximity that Symbian OS can support and 0 means touching the screen. 
  HALData attributes provide more information about proximity support on particular 
  device.
  
  On platforms without pointer proximity support, proximity is always assumed to be 0.
  
  @return Proximity of the pointer to the screen's surface.
  @see TAdvancedPointerEvent::Position3D()
  @see TAdvancedPointerEvent::ProximityAndPressure()
  @see HALData::EPointer3DMaxProximity
  @see HALData::EPointer3DProximityStep */
	{
	return IsAdvancedPointerEvent() ? DoGetProximity() : ENonAdvancedPointerZCoordinate;
	}

inline TInt TAdvancedPointerEvent::Pressure() const
/** Gets the pressure applied by the pointer to the screen. 
  Pressure units may vary between devices and may be non-linear.
  Returned value will be a positive integer as the maximum supported pressure range
  in Symbian OS is from 0 to KMaxTInt. HALData attributes provide more information
  about pressure support on particular device.
  
  On platforms without pointer pressure support, pressure is always assumed to be 0.
  
  @return Pressure applied by the pointer to the screen.
  @see TAdvancedPointerEvent::Pressure3D()
  @see TAdvancedPointerEvent::ProximityAndPressure()
  @see HALData::EPointer3DMaxPressure
  @see HALData::EPointer3DPressureStep */
	{
	return IsAdvancedPointerEvent() ? DoGetPressure() : ENonAdvancedPointerZCoordinate;
	}

inline TInt TAdvancedPointerEvent::ProximityAndPressure() const
/** Gets pressure applied by the pointer to the screen and proximity of the pointer
  to the screen as one value. This is possible because the pointer is never in proximity
  to the screen and pressing the screen at the same time.
  
  @return The value of proximity if the pointer is in proximity to the screen; proximity
          is always represented as negative TInt. The value of pressure if the pointer
          is touching the screen; pressure is always represented as positive TInt.
  @see TAdvancedPointerEvent::Proximity()
  @see TAdvancedPointerEvent::Pressure()
  @see TAdvancedPointerEvent::PositionAndPressure3D() */
	{return IsAdvancedPointerEvent() ? DoGetProximityAndPressure() : ENonAdvancedPointerZCoordinate;}

inline TPoint3D TAdvancedPointerEvent::PositionAndPressure3D() const
/**
  @return Pointer's X and Y coordinates plus combined proximity and pressure as
          Z coordinate. 
  @see TAdvancedPointerEvent::ProximityAndPressure() */
	{
	TPoint3D point3D;
	point3D.iX=iPosition.iX;
	point3D.iY=iPosition.iY;
	point3D.iZ=ProximityAndPressure();
	return point3D;
	}

inline TPoint3D TAdvancedPointerEvent::Position3D() const
/**
  @return Pointer's X and Y coordinates plus pointer's proximity to the screen as
          Z coordinate. Please note that returned Z coordinate will be always negative or 0.
  @see TAdvancedPointerEvent::Proximity() */
	{
	TPoint3D point3D;
	point3D.iX=iPosition.iX;
	point3D.iY=iPosition.iY;
	point3D.iZ=IsAdvancedPointerEvent() ? DoGetProximity() : ENonAdvancedPointerZCoordinate;
	return point3D;
	}

inline TPoint3D TAdvancedPointerEvent::Pressure3D() const
/**  
  @return Pointer's X and Y coordinates plus pressure applied by the pointer to the screen
          as Z coordinate. 
  @see TAdvancedPointerEvent::Pressure() */
	{
	TPoint3D point3D;
	point3D.iX=iPosition.iX;
	point3D.iY=iPosition.iY;
	point3D.iZ=IsAdvancedPointerEvent() ? DoGetPressure() : ENonAdvancedPointerZCoordinate;
	return point3D;
	}

inline TKeyEvent *TWsEvent::Key() const
	/** Gets the key event.

	This function can be used to get information about the key event if Type()
	returns an event of type EEventKey, EEventKeyUp or EEventKeyDown.

	@return Structure containing key event data */
	{return((TKeyEvent *)&iEventData);}

inline TWsVisibilityChangedEvent* TWsEvent::VisibilityChanged()
	/** Gets information about the visibility changed event.

	This function can be used to get information about the visibility changed event
	if Type() returns an event of type EEventWindowVisibilityChanged.

	@return Structure containing visibility changed event data */
	{ return reinterpret_cast<TWsVisibilityChangedEvent*>(iEventData); }

inline const TWsVisibilityChangedEvent* TWsEvent::VisibilityChanged() const
	/** Gets information about the visibility changed event.

	This function can be used to get information about the visibility changed event
	if Type() returns an event of type EEventWindowVisibilityChanged.

	@return Structure containing visibility changed event data */
	{ return reinterpret_cast<const TWsVisibilityChangedEvent*>(iEventData); }

inline TWsDisplayChangedEvent* TWsEvent::DisplayChanged()
	/** Gets information about the display changed event.

	This function can be used to get information about the display changed event
	if Type() returns an event of type EEventDisplayChanged.

	@return Structure containing display changed event data */
	{ return reinterpret_cast<TWsDisplayChangedEvent*>(iEventData); }

inline const TWsDisplayChangedEvent* TWsEvent::DisplayChanged() const
	/** Gets information about the display changed event.

	This function can be used to get information about the display changed event
	if Type() returns an event of type EEventDisplayChanged.

	@return Structure containing display changed event data */
	{ return reinterpret_cast<const TWsDisplayChangedEvent*>(iEventData); }

inline TModifiersChangedEvent *TWsEvent::ModifiersChanged() const
	/** Gets information about the modifier changed event.

	This function can be used to get information about the modifier changed event
	if Type() returns an event of type EEventModifiersChanged.

	@return Structure containing modifier changed event data */
	{return((TModifiersChangedEvent *)&iEventData);}

inline TWsErrorMessage *TWsEvent::ErrorMessage() const
	/** Gets an error event.

	This function can be used to get information about the error event if Type()
	returns an event of type EEventErrorMessage.

	@return The error event. */
	{return((TWsErrorMessage *)&iEventData);}

inline TUint8 *TWsEvent::EventData() const
	/** Gets information about the event.

	This function gives you direct access to the event data as a whole. The event
	can be a key or pointer event or it could be a new special event that only
	you and another application know about. You are limited to EWsEventDataSize
	bytes worth of data.

	@return A pointer to the event data. */
	{return((TUint8 *)&iEventData);}

inline TInt TWsEvent::Type() const
	/** Gets the type of event that occurred.

	@return The event type. Event types are defined in TEventCode. */
	{return(iType);}

inline TUint TWsEvent::Handle() const
	/** Gets the window handle.

	This is the handle that was passed to the window's Construct() function: typically
	it will be a pointer to the client-side object owning the window, providing
	access to its member functions.

	@return The window handle. */
	{return(iHandle);}

inline TTime TWsEvent::Time() const
	/** Gets the time when the event occurred.

	@return The time when the event occurred. */
	{return(iTime);}

inline void TWsEvent::SetType(TInt aType)
	/** Sets the event type.

	@param aType The event type. Event types are defined in TEventCode. */
	{iType=aType;}

inline void TWsEvent::SetHandle(TUint aHandle)
	/** Sets the event handle.

	This is the client handle of the window that is being sent the event.

	@param aHandle The event handle. */
	{iHandle=aHandle;}

inline void TWsEvent::SetTimeNow()
	/** Sets the event time to the current time. */
	{iTime.UniversalTime();}

inline TInt *TWsEvent::Int() const
	/** Gets the event data as a TInt.
	 
	 For TWsEvents of type EEventPointerEnter and EEventPointerExit this is the pointer number
	 of the pointer that entered/exited the window. Please note that on platforms that 
	 use the older Symbian OS, without multipointer support, pointer number is not initialized 
	 for EEventPointerEnter and EEventPointerExit and thus it is random.
	 
	 If the value of the attribute returned from HALData::EPointerNumberOfPointers is greater than 1,
	 then the system has multipointer support. 
	 
	 @see HALData::EPointerNumberOfPointers
	 @see HAL::Get(TAttribute,TInt&)
	*/
	{ return (TInt*)&iEventData; }

inline TUint TWsPriorityKeyEvent::Handle() const
	/** Gets the handle for the window group which added the priority key.

	This is the handle that was passed to the window group's Construct() function.
	Since most applications have only one window group, however, this function
	is rarely used.

	@return The window group handle. */
	{return(iHandle);}

inline TKeyEvent *TWsPriorityKeyEvent::Key() const
	/** Gets the priority key event.

	@return Structure containing priority key event data */
	{return((TKeyEvent *)&iEventData);}

inline void TWsPriorityKeyEvent::SetHandle(TUint aHandle)
	/** Sets the window group event handle.

	This is the client handle of the window that is being sent the event.

	@param aHandle The event handle. */
	{iHandle=aHandle;}

inline CDirectScreenAccess::CDirectScreenAccess(RWsSession& aWs,CWsScreenDevice* aScreenDevice,RWindowBase& aWindow,MDirectScreenAccess& aAbort)
		:CActive(RDirectScreenAccess::EPriorityVeryHigh), iWindow(aWindow), iAbort(aAbort), iDirectAccess(aWs)
		{iWsScreenDevice=aScreenDevice;}

inline CFbsBitGc* CDirectScreenAccess::Gc()
	/** Gets the graphics context for drawing to the screen.

	This is set up by calling StartL(). Its origin is set so that you should use
	window coordinates to specify which part of the screen to draw to and its
	clipping region is set to the visible part of the window.

	Code built to run on the Emulator must call CFbsScreenDevice::Update() in
	order to see the results of drawing to this graphics context, but this may
	not be required on all target hardware.

	@return The graphics context for drawing to the screen. */
	{ return iGc; }

inline CFbsScreenDevice*& CDirectScreenAccess::ScreenDevice()
	/** Gets the screen device to draw to.

	You should not use this screen device to create fonts because the object is
	deleted and recreated when direct screen access is aborted and restarted.
	Instead, create and use your own CFbsScreenDevice object to create fonts.

	@return The target screen device. */
	{ return iScreenDevice; }

inline RRegion* CDirectScreenAccess::DrawingRegion()
	/** Gets the clipping region to draw to.

	You must not draw outside of this region.

	The clipping region is calculated when StartL() is called, and is only updated
	if StartL() is called again.

	The region is specified in screen coordinates. This can be useful if you need
	to reapply the clipping region to the graphics context (CFbsBitGc::SetClippingRegion()).

	@return The clipping region to draw to. */
	{ return iDrawingRegion; }

#endif //__W32STD_H__
