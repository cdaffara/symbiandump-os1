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
// Definition of class that needs to be provided by a Key Click plugin
// 
//

#ifndef __W32CLICK_H__
#define __W32CLICK_H__

#ifndef __E32STD_H__
#include <e32std.h>
#endif
#ifndef __E32BASE_H__
#include <e32base.h>
#endif
#ifndef __W32STD_H__
#include <w32std.h>
#endif

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <graphics/pointereventdata.h>
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

class CClickMaker: public CBase
/** Key or pointer click plug-in provider interface.

This class should be implemented by a plug-in DLL in order to produce a sound 
when a key is pressed, a key auto repeats or when the screen is tapped with 
a pointer. When any of these events occur, the window server informs the plug-in 
DLL, which can then make the sound.

The details of the event are also passed to the plug-in so that the sound 
can be tailored to the precise event, for example clicking on different keys on the 
keypad or on different parts of the screen could make different sounds.

The name of the initial plug-in to use can be specified in the wsini.ini configuration 
file using the KEYCLICKPLUGIN or KEYCLICKPLUGINFIXED keywords. When the operating 
system boots, wsini.ini is read for the name of this plug-in. If a plug-in 
name exists, the window server will try to load it. At a later time, any client 
of the window server can request a new plug-in to be loaded or the current 
one to be unloaded, using the RSoundPlugIn class.

The plug-in is a polymorphic DLL which implements the CClickMaker interface. 
Its first UID is KDynamicLibraryUidValue and its second UID is 0x10004F63. 
Its first exported function should create an object of the CClickMaker sub-class 
and should have the following signature:

EXPORT_C CClickMaker* CreateClickMakerL() 

@publishedAll 
@released 
@see RSoundPlugIn */
	{
public:
	/** This function is called by the window server whenever there is a key event, 
	to generate the sound.
	
	It must be implemented by the key or pointer click plug-in.
	
	If the sound cannot be made, the function should continue without leaving.
	
	@param aType The type of key event: EEventKey, EEventKeyUp, EEventKeyDown 
	or EEventKeyRepeat.
	@param aEvent The key event details. */
	virtual void KeyEvent(TEventCode aType,const TKeyEvent& aEvent)=0;

	/** This function is called by the window server whenever there is a pointer event, 
	to generate the sound.
	
	It must be implemented by the key or pointer click plug-in.
	
	If the sound cannot be made, the function should continue without leaving.
	
	The iParentPosition data member of aEvent is not the position of the pointer 
	event relative to origin of the parent window. Instead it is the position 
	on the screen. This is because the parent window has no meaning inside the 
	plug-in as it does to the window server client and also knowledge 
	of the screen position may be useful to the plug-in.
	
	On devices where these features are supported, aEvent will contain pointer number, 
	proximity of the pointer to the screen and/or pressure applied by the pointer to the screen.
	In order to retrieve this information, implementation of this method should
	use TPointerEvent::AdvancedPointerEvent().
	
	@param aEvent The pointer event details. 
	@see TPointerEvent::AdvancedPointerEvent() */
	virtual void PointerEvent(const TPointerEvent& aEvent)=0;

	/** This function is intended for future expansion of the interface, in case it 
	needs to support sounds for other types of event.

	Currently it is called by the window server, with several values for aType.
	For each of these aParam will need to be cast to a different class type 
	to get the data:
	EEventPointer: cast to TPointerEventData*
	EEventScreenDeviceChanged: TClickMakerData*
	EEventGroupWindowOpen: TGroupWindowOpenData*
	EEventGroupWindowClose: TInt (the identifier of the window group being closed)
	EEventWindowClose: TWindowCloseData*

	@param aType One of the above listed values, in future may be used with other values.
	@param aParam See above. */
	virtual void OtherEvent(TInt aType,TAny* aParam=NULL)=0;

	/** This function may be implemented by the key or pointer click plug-in to enable 
	the plug-in to communicate with the window server client.
	
	If this is not required, the implementation may simply return zero.
	
	It is called by RSoundPlugIn::CommandReply() which returns the value returned 
	by this function.
	
	The return value can be generated either by returning it from the function 
	or by leaving with the value. In either case, the client will get back the 
	value concerned.
	
	@param aOpcode Opcode understood by both the window server client and the 
	plug-in DLL.
	@param aArgs Arguments packaged by the window server client.
	@return A value passed back to the client. This may be KErrNone or another 
	of the system-wide error codes. */
	virtual TInt CommandReplyL(TInt aOpcode, TAny *aArgs)=0;
	};

struct TClickMakerData
/**
Passed to a Key Click Plug-in using the function CClickMaker::OtherEvent when the 
aType value is EEventScreenDeviceChanged.

@publishedAll 
@released 
*/
	{
	TInt screenDeviceMode;
	};

class TGroupWindowOpenData
/**
Passed to a Key Click Plug-in using the function CClickMaker::OtherEvent when the 
aType value is EEventGroupWindowOpen.

@publishedAll 
@released 
*/
	{
public:
	/**
	The Window Group Identifier of the window being created.
	*/
	TInt iIdentifier;
	/**
	A number unique to the client creating the window group - allows the Plug-in to tell 
	different clients apart.
	*/
	TUint iClient;
	/**
	The number of Window Groups currently owned by that client (not including the one being created).
	*/
	TInt iNumClientWindowGroups;
	};

class TWindowCloseData
/**
Passed to a Key Click Plug-in using the function CClickMaker::OtherEvent when the aType value 
is EEventWindowClose.

@publishedAll 
@released 
*/
	{
public:
	/**
	The client handle of the window being closed.
	*/
	TUint32 iClientHandle;
	/**
	The Window Group Identifier of the window group that is a parent (or grandparent etc.)
	of the window being closed, or 0 if the window has been orphaned.
	*/
	TInt iWindowGroupId;
	};

#endif
