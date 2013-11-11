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
//

#include "Graphics/WSGRAPHICDRAWER.H"
#include <Graphics/WSGRAPHICDRAWERINTERFACE.H>
#include "Graphics/wsgraphicdrawerinternal.h"

// TGraphicDrawerId \\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C TInt TGraphicDrawerId::Compare(const TGraphicDrawerId& aOther) const
/** Compares another ID with this for equality.
@param aOther the ID to be compared with this.
@return 0 if identical, -1 if this is less than aOther and 1 if this is greater than aOther
*/	{
	// id is expected to differ most 99.99% of the time
	if(iId < aOther.iId)
		{
		return -1;
		}
	else if(iId > aOther.iId)
		{
		return 1;
		}
	// else we have to compare the iIsUid flag too; again, expect it to be a match 99.99% of these times
	else if(iIsUid == aOther.iIsUid)
		{
		return 0;
		}
	// collisions of id but not iIsUid are going to be really really rare
	else if(iIsUid)
		{
		return 1;
		}
	else
		{
		return -1;
		}
	}
	
EXPORT_C TInt TGraphicDrawerId::Compare(const TGraphicDrawerId& aFirst,const TGraphicDrawerId& aSecond)
/** Compares two IDs for equality.
@param aFirst the ID to be compared with aSecond
@param aSecond the ID to be compared with aFirst
@return 0 if identical, -1 if aFirst is less than aSecond and 1 if aFirst is greater than aSecond
*/	{
	return aFirst.Compare(aSecond);
	}

// MWsAnimationScheduler \\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C void MWsAnimationScheduler::Animate(MWsScreen& aScreen)
/** Calls the screen's OnAnimation() callback
	@param aScreen the screen to be called back
*/	{
	aScreen.OnAnimation();
	}

EXPORT_C void MWsAnimationScheduler::Animate(MWsScreen& aScreen, TRequestStatus* aFinished)
/** This method is only here to keep compatibility with NGA Window Server.
	Please use Animate(MWsScreen&) instead!
	@internalTechnology
*/	{
	aScreen.OnAnimation();

	if(aFinished)
		{
		*aFinished = KRequestPending;
		User::RequestComplete(aFinished, KErrNone);
		}
	}

EXPORT_C void MWsAnimationScheduler::Redraw(MWsScreen& aScreen)
/** Calls the screen's Redraw() method
	@param aScreen the screen to be redrawn
*/	{
	aScreen.Redraw();
	}

EXPORT_C TBool MWsAnimationScheduler::RedrawInvalid(MWsScreen& aScreen,const TArray<TGraphicDrawerId>& aInvalid)
/** Calls the screen's RedrawInvalid() method
	@param aScreen the screen to be signalled to redraw any affected areas
	@param the list of graphic IDs that are invalid
	@return whether the screen drew anything
*/	{
	return aScreen.RedrawInvalid(aInvalid);
	}
	
// MWsObjectProvider \\\\\\\\\\\\\\\\\\\\\\\\
	
EXPORT_C TAny* MWsObjectProvider::ResolveObjectInterface(TUint /*aTypeId*/)
	{
	return NULL;
	}

// MWsGraphicDrawerEnvironment \\\\\\\\\\\\\\\\\\\\\\\\

