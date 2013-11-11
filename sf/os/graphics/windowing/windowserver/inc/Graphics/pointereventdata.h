// Copyright (c) 1994-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Data passed to a Key Click Plug-in using the function CClickMaker::OtherEvent when the 
// aType value is EEventPointer.
//

#ifndef POINTEREVENTDATA_H
#define POINTEREVENTDATA_H

/**
Passed to a Key Click Plug-in using the function CClickMaker::OtherEvent when the 
aType value is EEventPointer.
This includes information about the window the pointer event will be sent to.
This is the normally the window being clicked on by the user, but pointer capturing 
and grabbing may affect this.

@publishedPartner
@released 
*/
class TPointerEventData
	{
public:
	enum TSource
		/**
		A list of locations that WSERV receives events from
		*/
		{
		/** The source is not specified. */
		EUnspecified,
		/** The event came from the kernel. */
		EKernel,
		/** The event came from a client API. */
		EClient,
		/** The event came from an Anim DLL. */
		EAnimDLL,
		};
public:
	/**
	The version number of the data passed with EEventPointer, current always 0.
	*/
	TInt iVersion;
	/**
	The screen position of pointer event.
	*/
	TPoint iCurrentPos;
	/**
	The full pointer event data previously passed to the CClickMaker::PointerEvent function,
	except that the iParentPosition will be the actual parent position, when previously passed
	to the plug-in this value was the screen position.
	*/
	TAdvancedPointerEvent iPointerEvent;
	/**
	The client handle of the window or zero if the window is not a client window.
	*/
	TUint32 iClientHandle;
	/**
	The current top left of the window on the screen.
	*/
	TPoint iWindowOrigin;
	/**
	The Window Group Identifier of the window group that is a parent (or grand parent etc.)
	of the window the event is sent to or zero if the window is not a client window.
	*/
	TInt iWindowGroupId;
	/**
	The source that WSERV recieves the event from,
	currently set to EUnspecified as this is for future expansion.
	*/
	TSource iSource;
	};

#endif // POINTEREVENTDATA_H
