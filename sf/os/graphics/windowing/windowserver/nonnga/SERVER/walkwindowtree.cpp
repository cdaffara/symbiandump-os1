// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "walkwindowtree.h"
#include "cliwin.h"
#include "rootwin.h"
#include "offscreenbitmap.h"
#include "ANIM.H"
#include "tcursor.h"
#include "pointer.h"

TWalkWindowTreeFocusChanged::TWalkWindowTreeFocusChanged(TBool aNewFocusState) :
	iNewFocusState(aNewFocusState)
	{
	}

TBool TWalkWindowTreeFocusChanged::DoIt(CWsWindow *aWin)
//
// Walk all windows that have had their focus state changed
//
	{
	aWin->FocusChanged(iNewFocusState);
	return(EFalse);
	}

TResumableWalkWindowTreeFindInvalid::TResumableWalkWindowTreeFindInvalid(CWsWindowRedraw** aResult) :
	iResult(aResult)
	{
	}

TBool TResumableWalkWindowTreeFindInvalid::DoIt(CWsWindow* aWin)
//
// Find a window with an invalid area
//
	{
	WS_ASSERT_DEBUG(aWin->WinType()==EWinTypeClient, EWsPanicWindowType);
	CWsWindowRedraw *redraw=((CWsClientWindow *)aWin)->Redraw();
	if (redraw->NeedsRedraw()>0)
		{
		*iResult=redraw;
		return(ETrue);
		}
	return(EFalse);
	}

TWalkWindowTreeDisconnect::TWalkWindowTreeDisconnect(RWsTextCursor *aCursor) :
	iTextCursor(aCursor)
	{}

TBool TWalkWindowTreeDisconnect::DoIt(CWsWindow *aWin)
//
// Disconnect a window
//
	{
	if (aWin->WinType()==EWinTypeClient)
		{
		CWsClientWindow *win=(CWsClientWindow *)aWin;
		win->iRedraw->WindowClosing();
		win->DeactivateAllSprites();

		if (iTextCursor)
			iTextCursor->WindowDisconnected(win);
		CWsAnim::WindowClosing(win->iAnimList);	// Destroy any animated objects attached to this window
		WsPointer::WindowDisconected(aWin);

		win->iParent=NULL;
		win->iSibling=NULL;
		win->iChild=NULL;
		win->iFlags&=~EFlagActive;
		win->ResetHiddenFlag();
		}
	return(EFalse);
	}
	
TWalkWindowTreeRegionBase::TWalkWindowTreeRegionBase(RWsRegion *aRegion, TTranslucentBehaviour aTranslucentBehaviour) :
	iTranslucentBehaviour(aTranslucentBehaviour), iRegion(aRegion), iSubRegion(NULL)
	{}

TBool TWalkWindowTreeRegionBase::DoIt(CWsWindow *aWin)
	{
	if (aWin->IsVisible())
		{
		DoIt2(aWin);
		if (aWin->WinType()!=EWinTypeRoot)
			{
			STACK_REGION tmp;
			switch(iTranslucentBehaviour)
				{
				case EDontWalkTranslucent:
					static_cast<CWsClientWindow *>(aWin)->SetClippedBaseArea(tmp);
					iRegion->SubRegion(tmp,iSubRegion);
					break;
				case EWalkTranslucent:
					static_cast<CWsClientWindow *>(aWin)->SetClippedBaseArea(*iSubRegion);
					iSubRegion->Intersect(*iRegion);
					if (iSubRegion->Count() > 0)
						{
						static_cast<CWsClientWindow *>(aWin)->SetOpaqueClippedBaseArea(tmp);
						iRegion->SubRegion(tmp);
						}
					break;
				}
			tmp.Close();
			}
		else if(iSubRegion)
			{
			iSubRegion->Copy(*iRegion);
			}
		if (iSubRegion && (iSubRegion->Count()>0 || iSubRegion->CheckError()))
			{
			if (DoIt3(aWin))
				return ETrue;
			iSubRegion->Clear();
			}
		}
	return(iRegion->IsEmpty());
	}
TBool TWalkWindowTreeRegionBase::DoIt3(CWsWindow*)
	{return EFalse;}

TWalkWindowTreeSchedule::TWalkWindowTreeSchedule() :
	TWalkWindowTreeBase(),
	iHead(0)
	{
	}

CWsWindow * TWalkWindowTreeSchedule::HeadWindow() const
	{
	return iHead;
	}

TWalkWindowTreeScheduleRegions::TWalkWindowTreeScheduleRegions(RWsRegion *aRegion, const TRegion& aTopLayer) :
	TWalkWindowTreeSchedule(),
	iRegion(aRegion),
	iTopLayer(aTopLayer),
	iScheduledRegionsOk(ETrue)
	{
	}

// This is similar to TWalkWindowTreeRegionBase::DoIt
TBool TWalkWindowTreeScheduleRegions::DoIt(CWsWindow *aWin)
	{
	WS_ASSERT_DEBUG((aWin != iHead), EWsPanicScheduledRedraw);
	if (aWin->IsVisible())
		{
		// Calculate the region we care about for this window:
		STACK_REGION region;
		if (aWin->WinType()==EWinTypeRoot)
			{
			region.Copy(*iRegion);
			}
		else
			{
			static_cast<CWsClientWindow *>(aWin)->SetClippedBaseArea(region);
			region.Intersect(*iRegion);
			}
		// If there is a region we care about, remember the window:
		// NOTE: Even if there are no redraw segments (ReadyToDraw is false) the window should
		//   be scheduled if it has some animations which will be redrawn via PostDrawWindow (cf def131912)
		if (!region.IsEmpty() && (aWin->ReadyToDraw() || aWin->HasAnimation() || aWin->HasSprite()) )
			{
			// Add window to linked list:
			aWin->SetNextScheduled(iHead);
			iHead = aWin;
			// Set the window scheduled region to something appropriate:
			if (iScheduledRegionsOk)
				{
				if (region.CheckError())
					{
					iScheduledRegionsOk = EFalse;
					}
				else
					{
					iScheduledRegionsOk = aWin->SetScheduledRegion(region);
					}
				}
			}
		if (aWin->WinType()!=EWinTypeRoot)
			{
			// Remove the opaque part from our working region:
			STACK_REGION opaqueRegion;
			static_cast<CWsClientWindow *>(aWin)->SetOpaqueClippedBaseArea(opaqueRegion);
			iRegion->SubRegion(opaqueRegion);
			opaqueRegion.Close();
			
			// Where we were drawing transparent and doing top layer only, remove
			// that bit too:
			if (!iTopLayer.IsEmpty())
				{
				region.Intersect(iTopLayer);
				iRegion->SubRegion(region);
				}
			}
		region.Close();
		}

	return(iRegion->IsEmpty() || !iScheduledRegionsOk);
	}

const TRegion * TWalkWindowTreeScheduleRegions::Region(const CWsWindow* aWin) const
	{
	WS_ASSERT_DEBUG(iScheduledRegionsOk, EWsPanicScheduledRedraw);
	return aWin->ScheduledRegion();
	}

TBool TWalkWindowTreeScheduleRegions::ScheduledRegionsOk() const
	{
	return iScheduledRegionsOk;
	}

TWalkWindowTreeScheduleFallback::TWalkWindowTreeScheduleFallback(CScreen::CFallbackMap * aFallbackMap) :
	TWalkWindowTreeSchedule(),
	iFallbackMap(aFallbackMap)
	{
	}

// This is similar to TWalkWindowTreeRegionBase::DoIt
TBool TWalkWindowTreeScheduleFallback::DoIt(CWsWindow *aWin)
	{
	WS_ASSERT_DEBUG((aWin != iHead), EWsPanicScheduledRedraw);
	if (aWin->IsVisible())
		{
		if (aWin == aWin->RootWindow())
			{
			aWin->SetNextScheduled(iHead);
			return ETrue;
			}
		else
			{
			TBool addWindow = EFalse;
			CWsClientWindow* cliWin = static_cast<CWsClientWindow *>(aWin);
			if (cliWin->IsTranslucent())
				{
				addWindow = ETrue; // costs more to work out than it is worth
				const TRegion * opaque = cliWin->GetUserOpaqueRegion();
				if (opaque && !opaque->CheckError())
					iFallbackMap->FillRegion(*opaque);
				}
			else
				{
				addWindow = iFallbackMap->FillRegion(*cliWin->BaseArea());
				}
			if (addWindow)
				{
				aWin->SetNextScheduled(iHead);
				iHead = aWin;
				}
			}
		}

	return(iFallbackMap->Count() < 1);
	}

const TRegion * TWalkWindowTreeScheduleFallback::Region(const CWsWindow* aWin) const
	{
	if (aWin == aWin->RootWindow())
		return iFallbackMap->Region();
	else
		{
		const CWsClientWindow* win = static_cast<const CWsClientWindow *>(aWin);
		const TRegion* region = win->VisibleRegionIfValid();
		if (!region)
			region = win->BaseArea();
		return region;
		}
	}

TWalkWindowTreeIsObscured::TWalkWindowTreeIsObscured(TBool &aResult) :
	iResult(&aResult)
	{
	aResult=ETrue;
	}

TBool TWalkWindowTreeIsObscured::DoIt(CWsWindow *aWin)
	{
	if (!aWin->VisibleRegion().IsEmpty())
		{
		*iResult=EFalse;
		return(ETrue);
		}
	return(EFalse);
	}

TWalkWindowTreeSetNonFading::TWalkWindowTreeSetNonFading(TBool aNonFading) :
	iNonFading(aNonFading)
	{}
TBool TWalkWindowTreeSetNonFading::DoIt(CWsWindow *aWin)
	{
	aWin->SetNonFading(iNonFading); 
	return EFalse;
	}

TWalkWindowTreeSetFaded::TWalkWindowTreeSetFaded(TBool aFaded,CWsWindowBase* aWin,TUint8 aBlackMap,TUint8 aWhiteMap) :
	iBlackMap(aBlackMap), iWhiteMap(aWhiteMap), iFaded(aFaded), iGroup(aWin->WinGroup())
	{
	}
	
TBool TWalkWindowTreeSetFaded::DoIt(CWsWindow *aWin)
	{
	if (aWin->WinGroup()!=iGroup)
		return ETrue;
	((CWsClientWindow*)aWin)->SetFaded(iFaded,iBlackMap,iWhiteMap); 
	return EFalse;
	}

TWalkWindowTreePurgeEvents::TWalkWindowTreePurgeEvents()
	{}
TBool TWalkWindowTreePurgeEvents::DoIt(CWsWindow *aWin)
	{
	aWin->PurgeEvents();
	return EFalse;
	}

TWalkWindowTreeCalcInvalidGraphics::TWalkWindowTreeCalcInvalidGraphics(RWsRegion *aRegion,TRegion &aDirty,const TArray<TGraphicDrawerId>& aInvalid):
	TWalkWindowTreeRegionBase(aRegion, EWalkTranslucent),
	iDirty(aDirty),
	iInvalid(aInvalid)
	{
	}

void TWalkWindowTreeCalcInvalidGraphics::DestroyRegions()
	{
	if(iSubRegion)
		{
		iSubRegion->Close();
		}
	delete iSubRegion;
	iSubRegion = NULL;	
	iDirty.Clear();
	}	

void TWalkWindowTreeCalcInvalidGraphics::CalcInvalid(CScreen& aScreen)
	{
	if(aScreen.RootWindow())
		{
		aScreen.RootWindow()->WalkWindowTree(*this,EWalkChildren);
		if(iRegion->CheckError())
			{
			iDirty.ForceError();
			}
		}
	}

TBool TWalkWindowTreeCalcInvalidGraphics::CreateSubRegion()
	{
	iSubRegion=new RWsRegion;
	return iSubRegion!=NULL;
	}

TBool TWalkWindowTreeCalcInvalidGraphics::DoIt3(CWsWindow *aWin) 
	{
	if (!iDirty.CheckError() && aWin->Redraw() && aWin->Redraw()->Contains(iInvalid,aWin->VisibleRegion()))
		{
		STACK_REGION intersection;
		intersection.Intersection(*iSubRegion,aWin->VisibleRegion());
		iDirty.Union(intersection);
		intersection.Close();
		}
	return iDirty.CheckError();
	}
	
#if defined(_DEBUG)

TBool TWalkWindowTreeCheck::DoIt(CWsWindow *aWin)
	{
	if (aWin->WinType()==EWinTypeRoot)
		{
		WS_ASSERT_DEBUG(aWin->BaseParent()==NULL, EWsPanicWindowCheck);
		WS_ASSERT_DEBUG(aWin->NextSibling()==NULL, EWsPanicWindowCheck);
		}
	else
		{
		WS_ASSERT_DEBUG(aWin->WinType()==EWinTypeClient, EWsPanicWindowCheck);
		}
	if (aWin->BaseChild())
		{
		WS_ASSERT_DEBUG(aWin->BaseChild()->BaseParent()==aWin, EWsPanicWindowCheck);
		}
	if (aWin->NextSibling())
		{
		WS_ASSERT_DEBUG(aWin->NextSibling()->GetPrevSibling()==aWin, EWsPanicWindowCheck);
		}
	return(EFalse);
	}

TBool TWalkWindowTreeFindWithFlag::DoIt(CWsWindow *aWin)
	{
	if (aWin->iFlags & iFlag)
		{
		iFound = aWin;
		return ETrue;
		}
	return EFalse;
	}
	
#endif

#include "wnredraw.h"	
TWalkWindowTreeRedrawStoreSize::TWalkWindowTreeRedrawStoreSize() : iTotalSize(0)
	{
	}

TBool TWalkWindowTreeRedrawStoreSize::DoIt(CWsWindow *aWin)
	{
	iTotalSize += aWin->Redraw()->SizeInBytes();
	return EFalse;
	}


TBool TWalkWindowTreeFindByHandle::DoIt(CWsWindow *aWin)
	{
	if (aWin->ClientHandle() == iHandle)
		{
		iFound = aWin;
		return ETrue;
		}
	return EFalse;
	}

TWalkWindowTreeUpdateRegions::TWalkWindowTreeUpdateRegions(CScreen & aScreen) :
	iScreen(aScreen)
	{
	}

void TWalkWindowTreeUpdateRegions::Walk()
	{
	STACK_REGION floatingSpriteRgn;
	iScreen.SpriteManager()->CalcFloatingSpriteRgn( floatingSpriteRgn, iScreen.RootWindow()->AbsRect() );
	iVisible.AddRect(iScreen.RootWindow()->AbsRect());
	iTop.AddRect(iScreen.RootWindow()->AbsRect());
	iRemainsOfFadableScreen.AddRect( iScreen.RootWindow()->AbsRect() );
	iTop.SubRegion(floatingSpriteRgn);
	iScreen.RootWindow()->WalkWindowTree(*this, EWalkChildren);
	iTop.Close();
	iVisible.Close();	
	iRemainsOfFadableScreen.Close();
	floatingSpriteRgn.Close();
	}

TBool TWalkWindowTreeUpdateRegions::DoIt(CWsWindow * aWin)
	{
	if (aWin->IsVisible() && !iVisible.IsEmpty())
		{
		// Calculate the region we care about for this window:
		STACK_REGION newVisibleRegion;
		STACK_REGION newFadableRegion;
		if (aWin->WinType()==EWinTypeRoot)
			{
			newVisibleRegion.Copy(iVisible);
			}
		else
			{
			static_cast<CWsClientWindow *>(aWin)->SetClippedBaseArea(newVisibleRegion);
			newVisibleRegion.Intersect(iVisible);
			if (!aWin->IsTranslucent())
				{
				iVisible.SubRegion(newVisibleRegion);
				}
			else
				{
				STACK_REGION opaque;
				static_cast<CWsClientWindow *>(aWin)->SetOpaqueClippedBaseArea(opaque);
				iVisible.SubRegion(opaque);
				opaque.Close();
				}
			//If the window has been faded calculate what region actually needs fading 
			//(i.e. subtract what has already been faded)
			if ( aWin->FadeCount() && !aWin->IsNonFading() && aWin->IsVisible()  && !iRemainsOfFadableScreen.IsEmpty() )
				{
				newFadableRegion.Copy( newVisibleRegion );
				newFadableRegion.Intersect( iRemainsOfFadableScreen );
				}
			}
		aWin->SetVisibleRegion(newVisibleRegion, &iTop, newFadableRegion);
		iTop.SubRegion(newVisibleRegion);
		iRemainsOfFadableScreen.SubRegion( newFadableRegion ); // Subtract the new faded region
		newFadableRegion.Close();
		newVisibleRegion.Close();
		}
	else
		{
		if (!aWin->VisibleRegion().IsEmpty())
			{
			aWin->ClearVisibleRegion();
			}
		}
	return(EFalse);
	}

TWalkWindowTreeScheduleRedraws::TWalkWindowTreeScheduleRedraws():
	iScheduleRedrawFilter( ERedrawFilterNoFilter )
	{
	}

TWalkWindowTreeScheduleRedraws::TWalkWindowTreeScheduleRedraws( TUint32 aFilter ):
	iScheduleRedrawFilter( aFilter )
	{ }

TBool TWalkWindowTreeScheduleRedraws::DoIt(CWsWindow * aWin)
	{
	if (aWin->WinType() != EWinTypeClient || static_cast<CWsClientWindow *>(aWin)->HasBeenDrawnToScreen())
		{
		TBool ban = (iScheduleRedrawFilter & ERedrawFilterOmitDSA) && ( aWin->IsDSAHost() );
		if ( !ban )
			{
			aWin->Screen()->AddRedrawRegion(aWin->VisibleRegion());
			}
		}
	return EFalse;
	}

TWalkWindowTreeOffsetTransparentRegions::TWalkWindowTreeOffsetTransparentRegions(const TPoint& aOffset) :
	iOffset(aOffset)
	{
	}

TBool TWalkWindowTreeOffsetTransparentRegions::DoIt(CWsWindow * aWin)
	{
	if (aWin != aWin->RootWindow())
		static_cast<CWsClientWindow *>(aWin)->OffsetUserTransparentRegion(iOffset);
	return EFalse;
	}

TWalkWindowTreeRecalcOpaque::TWalkWindowTreeRecalcOpaque()
	{
	}

TBool TWalkWindowTreeRecalcOpaque::DoIt(CWsWindow * aWin)
	{
	if (aWin != aWin->RootWindow())
		static_cast<CWsClientWindow *>(aWin)->SetUserOpaqueRegion();
	return EFalse;
	}

TBool TWalkWindowTreeReactivateGcs::DoIt(CWsWindow *aWin)
	{
	if (aWin != aWin->RootWindow())
		static_cast<CWsClientWindow *>(aWin)->ReactivateGcs();
	return EFalse;
	}

TWalkWindowTreeScheduleFadeNoRedraw::TWalkWindowTreeScheduleFadeNoRedraw()
	{ } // empty

TBool TWalkWindowTreeScheduleFadeNoRedraw::DoIt(CWsWindow *aWin)
	{
	aWin->Screen()->ScheduleRegionUpdate( aWin->VisibleRegionIfValid() );
	return EFalse;	
	}
