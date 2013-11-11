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
#include "server.h"
#include "tcursor.h"
#include "windowgroup.h"
#include "wstop.h"
#include "panics.h"
#include "offscreenbitmap.h"
#include "EVENT.H"
#include "graphics/windowserverconstants.h"

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

		if (pos != iPos || size != iSize || iType != type ||
			flags != iFlags || clipRect != iClipRect || color != iColor ||
			customTextCursor != iCustomTextCursor || win != iWin)
			{
			// There is a change in the cursor.
			changed = ETrue;
			}

		if (iInternalFlags&EHasFocus && changed)
			{
			TCursorSprite::Hide();
			}

		iPos = pos;
		iSize = size;
		iType = type;
		iFlags= flags;
		iClipRect = clipRect;
		iColor = color;
		iCustomTextCursor = customTextCursor;
		iWin = win;
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

void RWsTextCursor::doDraw(CFbsBitGc* aGc, const TRegion& aRegion)
	{
	TRegionFix<1> justInCase;
	const TRegion *pr= &aRegion;
	if (aRegion.CheckError())
		{
		justInCase.AddRect(iWin->AbsRect());
		pr= &justInCase;
		}
	if (!pr->IsEmpty())
		{
		aGc->SetUserDisplayMode(iWin->DisplayMode());
		aGc->SetDitherOrigin(iWin->Origin());
		aGc->SetDrawMode(CGraphicsContext::EDrawModeXOR);
		switch (iType)
			{
			case TTextCursor::ETypeRectangle:
				{
				aGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
				aGc->SetPenStyle(CGraphicsContext::ENullPen);
				aGc->SetBrushColor(iColor);
				}
				break;
			case TTextCursor::ETypeHollowRectangle:
				{
				aGc->SetBrushStyle(CGraphicsContext::ENullBrush);
				aGc->SetPenStyle(CGraphicsContext::ESolidPen);
				aGc->SetPenColor(iColor);
				}
				break;
			default:
				WS_PANIC_ALWAYS(EWsPanicInvalidCursorType);
			}
		aGc->SetClippingRegion(pr);
		aGc->DrawRect(RectRelativeToScreen());
		aGc->SetUserDisplayMode(ENone);

		TWindowServerEvent::NotifyScreenDrawingEvent(pr);
		}
	}

void RWsTextCursor::Draw(CFbsBitGc* aGc, const TRegion& aRegion)
	{
	iDrawRegion.Copy(iWin->VisibleRegion());
	if (iFlags&ETextCursorFlagClipped)
		{
		TRect rect(iClipRect);
		rect.Move(iWin->Origin());
		iDrawRegion.ClipRect(rect);
		}

	// Need to clip against a possible recent screen size change.
	iDrawRegion.ClipRect(iWin->Screen()->DrawDevice()->SizeInPixels());


	RWsRegion tmpRegion;
	tmpRegion.Intersection(iDrawRegion, aRegion);
	if (tmpRegion.CheckError())
		doDraw(aGc, iDrawRegion);
	else
		{
		if (!tmpRegion.IsEmpty())
			{
			doDraw(aGc, tmpRegion);
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
	iGroupWin->Screen()->ScheduleAnimation(RectRelativeToScreen(), 0, 0, 0);
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
		Hide();
		SetCurrentCursor(aFocus, aWin);
		}
	}

void TCursorSprite::SetCurrentCursor(RWsTextCursor* aFocus, CWsClientWindow* aWin)
	{
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

