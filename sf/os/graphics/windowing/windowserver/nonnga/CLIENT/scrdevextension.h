// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SCRDEVEXTENSION_H_
#define SCRDEVEXTENSION_H_

#include <w32std.h>
#include <graphics/testscreencapture.h>


class CWsScreenDevice::CScrDevExtension : public CBase, public MTestScreenCapture, public MWsClientClass
{
public:
	CScrDevExtension(RWsBuffer *aBuffer,TInt32 aWsHandle);
	~CScrDevExtension();
//get a specific interface
	void*	GetInterface(TUint aId);
//Access pre-existing data members
	CFbsTypefaceStore* TypefaceStore();
	void  SetTypefaceStore(CFbsTypefaceStore*);
	virtual TInt TranslateExtent(const TRect& aInitial, TRect& aTarget) const;
	virtual TInt GetCompositedSize(TSize& aSize) const;
	virtual TInt ComposeScreen(const CFbsBitmap& aBitmap) const;

protected:
	TInt	iSupportedExtensionsBits;
	CFbsTypefaceStore* iTypefaceStore;
};

#endif /*SCRDEVEXTENSION_H_*/
