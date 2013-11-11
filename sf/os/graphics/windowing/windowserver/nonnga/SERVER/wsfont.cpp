// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// FONT related functions and the font cache
// 
//

#include "wsfont.h"

CWsFontCache * CWsFontCache::iSelf = NULL;
TDblQue<CWsFbsFont> * CWsFontCache::iList = NULL;

CWsFontCache::CWsFontCache()
	{
	}

CWsFontCache::~CWsFontCache()
	{
	}

void CWsFontCache::CreateInstanceL()
	{
	// The list MUST exist before creation of the cache since the cache
	// contains an array of fonts. When the array is created the fonts add themselves
	// to the list.
	// The list is ordered from newest to oldest font used.
	iList = new (ELeave) TDblQue<CWsFbsFont>(_FOFF(CWsFbsFont,iLink));
	CleanupStack::PushL(iList);
	iSelf = new (ELeave) CWsFontCache;
	CleanupStack::Pop(iList);
	}

void CWsFontCache::DestroyInstance()
	{
	// The cache has to be destroyed first since the fonts are
	// destroyed when the cache's array if destroyed. The fonts 
	// remove themselves from the list
	delete iSelf;
	iSelf = NULL;
	delete iList;
	iList = NULL;

	}

void CWsFontCache::ReleaseFont(CWsFbsFont *&aFont)
	{
	if (aFont)
		{
		WS_ASSERT_DEBUG(aFont->iCount>0, EWsPanicFontCacheCount);
		// decrease usage count
		aFont->iCount--;
		// when the font reaches zero then the font entry can be reused later
		aFont = NULL;
		}
	}

TBool CWsFontCache::UseFont(CWsFbsFont *&aFont, TInt aHandle)
	{
	ReleaseFont(aFont);
	CWsFbsFont *font = NULL;

	// The list is ordered from newest to oldest font used.
	TDblQueIter<CWsFbsFont> iter(*iList);
	
	
	// Search for the font from newest to oldest font
	while ((font = iter++) != NULL)
		{
		// if the font handle already exists use existing font
		if (font->Handle() == aHandle)
			{
			break;
			}
		}

	// Font not found in cache so find an unused entry
	if (!font)
		{
		// Starting search from oldest to newest font unsed
		iter.SetToLast();
		while((font = iter--) != NULL)
			{
			// if you find an unused font then use that font entry
			if (font->iCount == 0)
				{
				break;
				}
			}
		// if an unused font is not found then all fonts are being used and 
		// the font can not be created
		if (!font)
			{
			return(ETrue);	// No room in cache
			}
		// release and reuse the font entry to create a new font
		font->Reset();
		if (font->Duplicate(aHandle) != KErrNone)
			{
			return(EFalse);	// Bad handle, (indicated by aFont=NULL)
			}
		}
	// put font at the font of the list as the font is the newest font
	font->iLink.Deque();
	iList->AddFirst(*font);
	// increase usage count
	font->iCount++;
	aFont = font;
	return(EFalse);
	}

TDblQue<CWsFbsFont>& CWsFontCache::List()
	{
	return *Instance()->iList;
	}
	
CWsFbsFont::CWsFbsFont()
	{
	// add itself to the font cache
	CWsFontCache::List().AddLast(*this);
	}

CWsFbsFont::~CWsFbsFont()
	{
	// remove itself from the font cache
	iLink.Deque();
	WS_ASSERT_DEBUG(iCount==0, EWsPanicFontCacheCount);
	}
