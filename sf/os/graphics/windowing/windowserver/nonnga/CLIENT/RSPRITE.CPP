// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Client side sprite code
// 
//

#include <e32std.h>
#include "../SERVER/w32cmd.h"
#include "CLIENT.H"

TCmdSpriteMember::TCmdSpriteMember(const TSpriteMember &aSpriteMember)
	{
	if (aSpriteMember.iBitmap)
		{
		iBitmap=aSpriteMember.iBitmap->Handle();
		iMaskBitmap=(aSpriteMember.iMaskBitmap) ? aSpriteMember.iMaskBitmap->Handle() : 0;
		iInvertMask=aSpriteMember.iInvertMask;
		iDrawMode=aSpriteMember.iDrawMode;
		iOffset=aSpriteMember.iOffset;
		}
	else
		Mem::FillZ(this,sizeof(*this));
	iInterval=aSpriteMember.iInterval;
	}

//
// RWsSpriteBase
//

EXPORT_C RWsSpriteBase::RWsSpriteBase()
/** Protected constructor. 

This prevents objects of this type being directly created. */
	{}

EXPORT_C RWsSpriteBase::RWsSpriteBase(RWsSession &aWs) : MWsClientClass(aWs.iBuffer)
/** Protected constructor with a window server session. 

This prevents objects of this type being directly created.

@param aWs The window server session owning the sprite. */
	{}

EXPORT_C TInt RWsSpriteBase::UpdateMember(TInt aIndex, const TSpriteMember &aMemberData)
/** Replaces one of a sprite's members. 

This allows the appearance of the sprite to be modified after it has been 
activated. 

You must call this function if you have changed the size of a member. After 
calling it, the sprite starts showing member zero again.

This function always causes a flush of the window server buffer.

@param aIndex Index of the sprite member. Sprite members are indexed in the 
order they were added to the sprite, beginning with 0. 
@param aMemberData New data for the sprite member. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	TWsSpriteCmdUpdateMember params(aIndex,aMemberData);
	return(WriteReply(&params,sizeof(params),EWsSpriteOpUpdateMember2));
	}

EXPORT_C void RWsSpriteBase::UpdateMember(TInt aIndex)
/** Updates the displayed sprite, taking into account any change to the content 
of the bitmaps for the specified member.

The appearance of a sprite can be changed after it has been activated by drawing 
to the appropriate TSpriteMember::iBitmap member, and then calling this function. 
Alternatively, a new sprite member can be created, containing a new bitmap, 
and the sprite can be updated to contain this new member by calling the other 
UpdateMember() overload.

You should only use this function if you have changed the content of 
the bitmap or mask. If you have changed the size of the bitmap then you must 
use the other overload.

You only need to call this function if you want the content of the screen 
to update immediately. If you don't call this function then the appearance 
of a member will update automatically next time that member is drawn to the 
screen.

@param aIndex Index of the sprite member. Sprite members are indexed in the 
order they were added to the sprite, beginning with 0. */
	{
	WriteInt(aIndex, EWsSpriteOpUpdateMember);
	}

EXPORT_C void RWsSpriteBase::Close()
/** Destroys the sprite or pointer cursor in the window server, and frees client-side 
resources used by the sprite. 

Any application which constructs an RWsSprite or RWsPointerCursor should call 
this function when the sprite or cursor is no longer needed.

Note that this function does not free resources used by sprite members. */
	{
	if (iBuffer && iWsHandle)
		Write(EWsSpriteOpFree);
	iWsHandle=NULL;
	}

EXPORT_C TInt RWsSpriteBase::Activate()
/** Activates the sprite or pointer cursor.

Sprites are displayed on the screen when they are activated; pointer cursors 
are not displayed until they are set into a window using either RWindowTreeNode::SetCustomPointerCursor() 
or RWindowTreeNode::SetPointerCursor(). 

This function should not be called until the sprite or pointer cursor has 
been fully initialised, and its sprite members have been added.

Before it can call this function, a sprite must be fully constructed using 
the RWsSprite::RWsSprite(RWsSession& aWs) and RWsSprite::Construct() functions, 
and must have at least one sprite member. Otherwise, a panic will occur. Similarly, 
a pointer cursor must have at least one sprite member and must call RWsPointerCursor::RWsPointerCursor(RWsSession& 
aWs) and RWsPointerCursor::Construct().
 
This function always causes a flush of the window server buffer.

@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	return(WriteReply(EWsSpriteOpActivate));
	}

EXPORT_C TInt RWsSpriteBase::AppendMember(const TSpriteMember &aMemberData)
/** Adds a sprite member to a sprite or pointer cursor. 

Sprite members are displayed by the sprite in the order in which they were 
added to the sprite. This function can be called before and after the sprite 
has been activated.

The position of the bitmap can be changed in relation to the sprite's origin 
using the iOffset member of aSpriteList.
 
This function always causes a flush of the window server buffer.

@param aMemberData The sprite member to add.
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	TCmdSpriteMember params(aMemberData);
	return(WriteReply(&params,sizeof(params),EWsSpriteOpAppendMember));
	}

//
// RWsSprite
//

EXPORT_C RWsSprite::RWsSprite() : RWsSpriteBase()
/** Default C++ constructor. 

This allows classes that contain an RWsSprite to be constructed before an 
RWsSession exists.

Note: do not use this version of the constructor on its own. Before an RWsSprite 
object can be used it must be constructed using the RWsSprite(RWsSession) 
constructor. An example of this might be as follows:

@code
RWsSprite iSprite;
iSprite=RWsSprite(iWsSession);
@endcode */
	{}

EXPORT_C RWsSprite::RWsSprite(RWsSession &aWs) : RWsSpriteBase(aWs)
/** Constructs a sprite with a window server session. 

Initialisation must be completed using the Construct() function before the 
sprite can be activated using RWsSpriteBase::Activate().

@param aWs The window server session owning the sprite. */
	{}

EXPORT_C TInt RWsSprite::Construct(RWindowTreeNode &aWindow, const TPoint &aPos, TInt aFlags)
/** Completes the construction of a sprite. 

This function must be called before a sprite is activated using RWsSpriteBase::Activate().
 
It always causes a flush of the window server buffer.

@param aWindow The window in which the sprite is displayed.
@param aPos The position of the sprite's origin relative to aWindow's origin. 
The origin is the top left corner of the window. 
@param aFlags Any one of the TSpriteFlags values, or a combination of the flags, 
using a bit-wise OR operation.
@return KErrNone if successful, otherwise one of the system-wide error codes. 
@panic TW32Panic 17 in debug builds if called on an already constructed object.
@see TSpriteFlags */
	{
	__ASSERT_DEBUG(iWsHandle == KNullHandle, Panic(EW32PanicGraphicDoubleConstruction));
	TWsClCmdCreateSprite params(aWindow.WsHandle(),aPos,aFlags);
	TInt ret;
	if ((ret=iBuffer->WriteReplyWs(&params,sizeof(params),EWsClOpCreateSprite))>=0)
		{
		iWsHandle=ret;
		ret=KErrNone;
		}
	return(ret);
	}

EXPORT_C void RWsSprite::SetPosition(const TPoint &aPos)
/** Sets the sprite's position. 

This function can be called before or after the sprite has been activated. 

Note: the sprite's initial position is set when the sprite is constructed (see Construct()).

@param aPos Position of the sprite's origin relative to the origin of the 
window that owns it. The origin is the top left corner of the window. */
	{
	WritePoint(aPos,EWsSpriteOpSetPosition);
	}

//
// RWsPointerCursor
//

EXPORT_C RWsPointerCursor::RWsPointerCursor() : RWsSpriteBase()
/** Default C++ constructor. 

Use this constructor to allow classes that contain an RWsPointerCursor 
to be constructed before an RWsSession exists.

Note: do not use this version of the constructor on its own. 
Before an RWsPointerCursor object can be used, it must be constructed 
using the RWsPointerCursor(RWsSession) constructor. An example of this 
might be as follows:

@code
RWsPointerCursor pointerCursor;
pointerCursor=RWsPointerCursor(iWsSession);
@endcode */
	{}

EXPORT_C RWsPointerCursor::RWsPointerCursor(RWsSession &aWs) : RWsSpriteBase(aWs)
/** Constructs a pointer cursor initialised with a window server session. 

Initialisation must be completed using the Construct() function before the 
sprite can be activated using RWsSpriteBase::Activate().

Once initialisation is complete, the pointer cursor can be passed as an argument 
to RWsSession::SetSystemPointerCursor() or RWindowTreeNode::SetCustomPointerCursor().

@param aWs The window server session owning the pointer cursor. */
	{}

EXPORT_C TInt RWsPointerCursor::Construct(TInt aFlags)
/** Completes pointer cursor construction. 

This function must be called before the pointer cursor is activated.
 
It always causes a flush of the window server buffer.

@param aFlags ESpriteFlash to flash the sprite on and off or 0 for a non-flashing 
cursor. Note that pointer cursors always behave as if ESpriteNoChildClip and 
ESpriteNoShadows are set. 
@return KErrNone if successful, otherwise one of the system-wide error codes. */
	{
	__ASSERT_DEBUG(iWsHandle == KNullHandle, Panic(EW32PanicGraphicDoubleConstruction));	
	TWsClCmdCreatePointerCursor params;
	params.flags=aFlags;
	TInt ret;
	if ((ret=iBuffer->WriteReplyWs(&params,sizeof(params),EWsClOpCreatePointerCursor))>=0)
		{
		iWsHandle=ret;
		ret=KErrNone;
		}
	return(ret);
	}
