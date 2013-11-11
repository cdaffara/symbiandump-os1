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
// The text cursor
// 
//

#include <e32std.h>
#include <graphics/wscursor.h>
#include "server.h"
#include "tcursor.h"
#include "windowgroup.h"
#include "wstop.h"
#include "panics.h"
#include "EVENT.H"
#include "graphics/windowserverconstants.h"

static const TInt64 KFlashRate(500000); // duration cursor is ON or OFF

void RWsTextCursor::ConstructL(CWsWindowGroup *aGroupWin)
	{
	iInternalFlags = 0;
	iGroupWin=aGroupWin;
	iCustomTextCursor = NULL;
	}

void RWsTextCursor::Close()
	{
	iDrawRegion.Close();
	Cancel();
	}

void RWsTextCursor::SetL(const TWsWinCmdSetTextCursor &aSet, TBool aClipped)
	{
	if (aSet.cursor.iType < TTextCursor::ETypeFirst ||
	        (aSet.cursor.iType > TTextCursor::ETypeLast &&
		 aSet.cursor.iType <= TTextCursor::ETypeLastBasic) ||
		(aSet.cursor.iFlags&static_cast<TUint>(ETextCursorPrivateFlags)))
		{
		Cancel();
		iGroupWin->OwnerPanic(EWservPanicInvalidTextCursor);
		}
	else
		{
		CWsClientWindow* win = NULL;
		iGroupWin->WsOwner()->HandleToClientWindow(aSet.window, &win);

		// Check window is a child of the group window
		CWsWindowBase* searchWin = NULL;
		for(searchWin=win; searchWin->WinType()!=EWinTypeGroup; searchWin=searchWin->BaseParent())
			{}
		if (iGroupWin != searchWin)
			{
			Cancel();
			iGroupWin->OwnerPanic(EWservPanicWindow);
			}

		TPoint pos(aSet.pos.iX, aSet.pos.iY-aSet.cursor.iAscent);
		TSize size(aSet.cursor.iWidth, aSet.cursor.iHeight);
		TUint flags = aSet.cursor.iFlags;
		TInt type = aSet.cursor.iType;
		TRect clipRect = iClipRect;
		TRgb color = aSet.cursor.iColor;
		CWsCustomTextCursor* customTextCursor = iCustomTextCursor;
		TBool changed = EFalse;

		TPoint clipOrigo;
		TSize clipSize;

		if (type > TTextCursor::ETypeLastBasic)
			{
			changed = ETrue;

			customTextCursor = CWsClient::FindCustomTextCursor(type);
			if (!customTextCursor)
				{
				Cancel();
				iGroupWin->OwnerPanic(EWservPanicNoCustomTextCursor);
				return;
				}
			
			if( !customTextCursor->HasSpriteMember() )
				{
				iGroupWin->OwnerPanic(EWservPanicNoSpriteMember);
				return;
				}
			
			TInt yAdjust=0;
			switch (customTextCursor->Alignment())
				{
				case RWsSession::ECustomTextCursorAlignTop:
					break;
				case RWsSession::ECustomTextCursorAlignBaseline:
					yAdjust = aSet.cursor.iAscent-1;
					break;
				case RWsSession::ECustomTextCursorAlignBottom:
					yAdjust = aSet.cursor.iHeight-1;
					break;
				default:
					Cancel();
					iGroupWin->OwnerPanic(EWservPanicCustomTextCursorAlign);
					return;
				}
			pos.iY += yAdjust;
			// Start with a clipping rect to be the whole window
			// relative cursor pos and shrink down to what we want
			clipOrigo = -pos;
			clipSize = win->Size();
			if (flags & TTextCursor::EFlagClipHorizontal)
				{
				clipOrigo.iX = 0;
				clipSize.iWidth = size.iWidth;
				}
			if (flags & TTextCursor::EFlagClipVertical)
				{
				clipOrigo.iY = -yAdjust;
				clipSize.iHeight = aSet.cursor.iHeight;
				}
			}
		else
			{
			customTextCursor = NULL;
			}

		if (aClipped)
			{
			flags|=ETextCursorFlagClipped;
			clipRect=aSet.rect;
			}

		TPoint absPos(pos.iX,pos.iY);
		absPos=absPos+win->Origin();
		if (pos != iPos || absPos != iAbsPos || size != iSize || iType != type ||
			flags != iFlags || clipRect != iClipRect || color != iColor ||
			customTextCursor != iCustomTextCursor || win != iWin)
			{
			// There is a change in the cursor.
			changed = ETrue;
			}

		if (iInternalFlags&EHasFocus && changed)
			{
			if ((win != iWin && !iCustomTextCursor) || (customTextCursor && !iCustomTextCursor))
				ReleaseNode();
			TCursorSprite::Hide();
			}

		UpdateAttributes(pos, absPos, size, type, flags, clipRect, color, customTextCursor, win);

		if (customTextCursor && iInternalFlags&EHasFocus)
			{
			customTextCursor->CompleteL(win, !(flags&TTextCursor::EFlagNoFlash), flags & (TTextCursor::EFlagClipHorizontal | TTextCursor::EFlagClipVertical), clipOrigo, clipSize);
			customTextCursor->SetPositionNoRedraw(pos);
			}

		if (iInternalFlags&EHasFocus && changed)
			{
			TCursorSprite::SetCurrentCursor(this, win);
			}
		}
	}
void RWsTextCursor::UpdateAttributes(TPoint aPos, TPoint aAbsPos, TSize aSize, TInt aType, TUint aFlags, TRect aClipRect, TRgb aColor, CWsCustomTextCursor* aCustomTextCursor, CWsClientWindow* aWin)
	{
	if (aPos != iPos || aSize != iSize || aAbsPos != iAbsPos)
		{
		iPos = aPos;
        iAbsPos = aAbsPos;
		iSize = aSize;
		WS_ASSERT_DEBUG(iGroupWin->Screen(),EWsPanicNoScreen);
		MWsWindowTreeObserver* const windowTreeObserver = iGroupWin->Screen()->WindowTreeObserver();
		if (windowTreeObserver && iInternalFlags&EHasFocus && iInternalFlags&EActiveNode)
			windowTreeObserver->NodeExtentChanged(*this, RectRelativeToScreen());
		}

	if (aType != iType)
		{
		iType = aType;
		NotifyObserver(MWsWindowTreeObserver::ECursorType);
		}

	if (aClipRect != iClipRect)
		{
		iClipRect = aClipRect; // must update clip rect before sending clip rect set/unset notification
		if ((aFlags&ETextCursorFlagClipped) && (iFlags&ETextCursorFlagClipped))
			NotifyObserver(MWsWindowTreeObserver::ECursorClipRect); // clip rect changed
		}

	if (aFlags != iFlags)
		{
		TBool sendFlagChanged = EFalse;
		if ((aFlags&ETextCursorFlagClipped) != (iFlags&ETextCursorFlagClipped))
			{
			if (iInternalFlags&EHasFocus && iInternalFlags&EActiveNode)
				{
				// We can't send flag changed till iFlags has been updated, as otherwise plugins responding to
				// the flag changed notification by calling ClipRect() may get the wrong rect
				sendFlagChanged = ETrue;
				}
			}
		const TBool userFlagsChanged((aFlags&ETextCursorUserFlags) != (iFlags&ETextCursorUserFlags)); 
		iFlags = aFlags;
		if (userFlagsChanged)
			NotifyObserver(MWsWindowTreeObserver::ECursorFlags);
		if (sendFlagChanged)
			{
			WS_ASSERT_DEBUG(iGroupWin->Screen(),EWsPanicNoScreen);
			MWsWindowTreeObserver* const windowTreeObserver = iGroupWin->Screen()->WindowTreeObserver();
			if (windowTreeObserver)
				windowTreeObserver->FlagChanged(*this, MWsWindowTreeObserver::ECursorClipRectSet, !!(iFlags&ETextCursorFlagClipped)); // clip rect set/unset
			}
		}

	if (aColor != iColor)
		{
		iColor = aColor;
		NotifyObserver(MWsWindowTreeObserver::ECursorColor);
		}
	iCustomTextCursor = aCustomTextCursor;
	iWin = aWin;
	}

void RWsTextCursor::NotifyObserver(MWsWindowTreeObserver::TAttributes aAttribute) const
	{
	if (iInternalFlags&EHasFocus && iInternalFlags&EActiveNode)
		{
		WS_ASSERT_DEBUG(iGroupWin->Screen(),EWsPanicNoScreen);
		MWsWindowTreeObserver* const windowTreeObserver = iGroupWin->Screen()->WindowTreeObserver();
		if (windowTreeObserver)
			windowTreeObserver->AttributeChanged(*this, aAttribute);
		}
	}

void RWsTextCursor::CreateNode()
	{
	WS_ASSERT_DEBUG(iGroupWin->Screen(),EWsPanicNoScreen);
	MWsWindowTreeObserver* const windowTreeObserver = iGroupWin->Screen()->WindowTreeObserver();
	if (windowTreeObserver && !(iInternalFlags&EActiveNode))
		{
		iInternalFlags |= EActiveNode;
		windowTreeObserver->NodeCreated(*this, iWin);
		windowTreeObserver->NodeExtentChanged(*this, RectRelativeToScreen());
		if (iFlags&ETextCursorFlagClipped)
			windowTreeObserver->FlagChanged(*this, MWsWindowTreeObserver::ECursorClipRectSet, ETrue);
		windowTreeObserver->NodeActivated(*this);
		}
	}

void RWsTextCursor::ReleaseNode()
	{
	if (iInternalFlags&EActiveNode)
		{
		WS_ASSERT_DEBUG(iGroupWin->Screen(),EWsPanicNoScreen);
		MWsWindowTreeObserver* const windowTreeObserver = iGroupWin->Screen()->WindowTreeObserver();
		if (windowTreeObserver)
			{
			windowTreeObserver->NodeReleased(*this);
			iInternalFlags &= ~EActiveNode;
			}
		}
	}

void RWsTextCursor::SendState(MWsWindowTreeObserver& aWindowTreeObserver) const
	{
	if (iInternalFlags & EActiveNode)
		{
		aWindowTreeObserver.NodeCreated(*this, iWin);
		aWindowTreeObserver.NodeExtentChanged(*this, RectRelativeToScreen());
		if (iFlags&ETextCursorFlagClipped)
			aWindowTreeObserver.FlagChanged(*this, MWsWindowTreeObserver::ECursorClipRectSet, ETrue);
		aWindowTreeObserver.NodeActivated(*this);
		}
	}

void RWsTextCursor::Cancel()
	{
	if (iType!=TTextCursor::ETypeNone)
		{
		if (iInternalFlags&EHasFocus)
			TCursorSprite::SetFocus(NULL);
		iType=TTextCursor::ETypeNone;
		iWin=NULL;
		}
	}

void RWsTextCursor::Disable()
	{
	if (iWin)
		{
		TCursorSprite::Hide();
		}
	}

void RWsTextCursor::Enable()
	{
	if (iWin)
		{
		TCursorSprite::Reveal();
		}
	}

void RWsTextCursor::LostFocus()
	{
	TCursorSprite::SetFocus(NULL);
	iInternalFlags &= ~EHasFocus;
	}

void RWsTextCursor::ReceivedFocus()
	{
	iInternalFlags |= EHasFocus;
	if (iType!=TTextCursor::ETypeNone && iWin)
		{
		TCursorSprite::SetFocus(this,iWin);
		if (iCustomTextCursor)
			{
			iCustomTextCursor->SetPositionNoRedraw(iPos);
			}
		}
	}

TRect RWsTextCursor::RectRelativeToScreen() const
	{
	TRect rect;
	rect.iTl=iPos+iWin->Origin();
	rect.iBr=rect.iTl+iSize;
	return(rect);
	}

TRect RWsTextCursor::RectRelativeToWindow() const
	{
	TRect rect;
	rect.iTl=iPos;
	rect.iBr=rect.iTl+iSize;
	return rect;
	}

void RWsTextCursor::doDraw(const TRegion& aRegion)
	{
	TRegionFix<1> fallbackClipRegion;
	const TRegion *clipRegion= &aRegion;
	if (aRegion.CheckError())
		{
		fallbackClipRegion.AddRect(iWin->AbsRect());
		clipRegion= &fallbackClipRegion;
		}

	if(!clipRegion->IsEmpty())
		{
		MWsTextCursor::TTextCursorInfo renderStageCursorInfo(
			RectRelativeToWindow(),
			*clipRegion,
			iType, static_cast<MWsWindow *>(Win()), iColor
			);
		
		MWsTextCursor* textCursor = iWin->Screen()->RenderStageTextCursor();

		textCursor->DrawTextCursor(renderStageCursorInfo);

		TWindowServerEvent::NotifyScreenDrawingEvent(clipRegion);
		}
	}

void RWsTextCursor::Draw(const TRegion& aRegion)
	{
	iDrawRegion.Copy(iWin->VisibleRegion());
	if (iFlags&ETextCursorFlagClipped)
		{
		TRect rect(iClipRect);
		rect.Move(iWin->Origin());
		iDrawRegion.ClipRect(rect);
		}

	// Need to clip against a possible recent screen size change.
	iDrawRegion.ClipRect(iWin->Screen()->SizeInPixels());

	RWsRegion tmpRegion;
	tmpRegion.Intersection(iDrawRegion, aRegion);
	if (tmpRegion.CheckError())
		doDraw(iDrawRegion);
	else
		{
		if (!tmpRegion.IsEmpty())
			{
			doDraw(tmpRegion);
			}
		}
	tmpRegion.Close();
	}

void RWsTextCursor::WindowDisconnected(CWsWindow *aWindow)
	{
	if (iWin==aWindow)
		Cancel();
	}

TBool RWsTextCursor::IsStandardCursorActive()
	{
	return TCursorSprite::IsStandardCursorActive();
	}

TBool RWsTextCursor::IsFlashing() const
	{
	return !(iFlags&TTextCursor::EFlagNoFlash);
	}

void RWsTextCursor::ScheduleReDrawNow()
	{
	if (!iGroupWin->Screen()->ChangeTracking())
		iGroupWin->Screen()->ScheduleAnimation(ETextCursor, RectRelativeToScreen(), 0, 0, 0, iWin);
	}

/** @see MWsWindowTreeNode */
MWsWindowTreeNode::TType RWsTextCursor::NodeType() const
	{
	return MWsWindowTreeNode::EWinTreeNodeStandardTextCursor; 
	}

/** @see MWsWindowTreeNode */
const MWsWindow* RWsTextCursor::Window() const
	{
	return NULL;
	}

/** @see MWsWindowTreeNode */
const MWsSprite* RWsTextCursor::Sprite() const
	{
	return NULL;
	}

/** @see MWsWindowTreeNode */
const MWsStandardTextCursor* RWsTextCursor::StandardTextCursor() const
	{
	return this;
	}

/** @see MWsWindowTreeNode */
const MWsWindowGroup* RWsTextCursor::WindowGroup() const
	{
	return static_cast<MWsWindowGroup*>(iGroupWin);
	}

/** @see MWsWindowTreeNode */
const MWsWindowTreeNode* RWsTextCursor::ParentNode() const
	{
	return iWin;
	}

/** @see MWsStandardTextCursor */
TInt RWsTextCursor::Type() const
	{
	return iType;
	}

/** @see MWsStandardTextCursor */
TRect RWsTextCursor::Rect() const
	{
	return RectRelativeToScreen();
	}

/** @see MWsStandardTextCursor */
TRect RWsTextCursor::ClipRect() const
	{
	if (iFlags&ETextCursorFlagClipped)
		{
		TRect clipRectRelativeToScreen(iClipRect);
		clipRectRelativeToScreen.Move(iWin->Origin());
		return clipRectRelativeToScreen;
		}
	else
		{
		return Rect();
		}
	}

/** @see MWsStandardTextCursor */
TUint RWsTextCursor::Flags() const
	{
	return iFlags&ETextCursorUserFlags;
	}

/** @see MWsStandardTextCursor */
TRgb RWsTextCursor::Color() const
	{
	return iColor;
	}

/** @see MWsStandardTextCursor */
TTimeIntervalMicroSeconds32 RWsTextCursor::FlashInterval() const
	{
	return iFlags&TTextCursor::EFlagNoFlash ? 0 : KFlashRate;
	}

TFlashState RWsTextCursor::CurrentCursorFlashState() const
	{
	if (IsFlashing())
		{
		return (CWsTop::CurrentFocusScreen()->Now().DateTime().MicroSecond()<KFlashRate)?EFlashOn:EFlashOff;
		}
	else
		{
		return EFlashOn;
		}
	}


// Cursor sprite handling

TBool TCursorSprite::iHidden=ETrue;
RWsTextCursor *TCursorSprite::iCurrentCursor=NULL;

//

// Hide / Reveal text cursors.
void TCursorSprite::Hide()
	{
	if (!iHidden && iCurrentCursor)
		{
		iHidden=ETrue;
		if (iCurrentCursor->iCustomTextCursor)
			{
			iCurrentCursor->iCustomTextCursor->Deactivate();
			}
		else
			{
			iCurrentCursor->ScheduleReDrawNow();
			}
		}
	}
	
void TCursorSprite::Reveal()
	{
	if(iHidden && iCurrentCursor)
		{
		iHidden=EFalse;
		if (iCurrentCursor->iCustomTextCursor)
			{
			iCurrentCursor->iCustomTextCursor->Activate();
			}
		else
			{
			iCurrentCursor->ScheduleReDrawNow();
			}
		}
	}

void TCursorSprite::SetFocus(RWsTextCursor* aFocus,CWsClientWindow* aWin/*=NULL*/)
	{
	if (iCurrentCursor!=aFocus)
		{
		if (iCurrentCursor)
			iCurrentCursor->ReleaseNode();
		Hide();
		SetCurrentCursor(aFocus, aWin);
		}
	}

void TCursorSprite::SetCurrentCursor(RWsTextCursor* aFocus, CWsClientWindow* aWin)
	{
	if (aFocus && !aFocus->iCustomTextCursor)
		aFocus->CreateNode();
	iCurrentCursor = aFocus;
	if (aWin && iCurrentCursor && iCurrentCursor->iCustomTextCursor)
		{
		iCurrentCursor->iCustomTextCursor->SetWindow(aWin);
		}
	Reveal();
	}

TBool TCursorSprite::IsStandardCursorActive()
	{
	return iCurrentCursor && !iCurrentCursor->iCustomTextCursor && !iHidden;
	}

