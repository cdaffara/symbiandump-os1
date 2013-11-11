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

#include <graphics/displaycontrol.h>
#include <graphics/displaymapping.h>
#include <graphics/testscreencapture.h>
#include <w32std.h>


class CWsScreenDevice::CScrDevExtension : public CBase, public MDisplayControl, public MDisplayMapping, public MTestScreenCapture, public MWsClientClass
{
public:
	CScrDevExtension(RWsBuffer *aBuffer,TInt32 aWsHandle);
	~CScrDevExtension();
//create or access the 1:1 extension instance behind the screen device
	TInt RepeatableConstruct();
//get a specific interface
	void*	GetInterface(TUint aId);
//Access pre-existing data members
	CFbsTypefaceStore* TypefaceStore();
	void  SetTypefaceStore(CFbsTypefaceStore*);
//implementing MDisplayControlBase 
	virtual TInt NumberOfResolutions() const;
	virtual TInt GetResolutions(RArray<TResolution>& aResolutions) const;
	virtual void GetConfiguration(TDisplayConfiguration& aConfig) const;
	virtual TInt SetConfiguration(const TDisplayConfiguration& aConfig);
	virtual TInt PreferredDisplayVersion() const;
//implementing MDisplayControl
	virtual TBool DisplayChangeEventsEnabled() const;
	virtual void EnableDisplayChangeEvents(TBool aEnable);
//implementing MDisplayMapping
	virtual TInt MapCoordinates(TCoordinateSpace aSourceSpace, const TRect& aSource, TCoordinateSpace aTargetSpace, TRect& aTarget) const;
	virtual void GetMaximumWindowExtent(TRect& aExtent) const;
	virtual void GetMaximumSurfaceSize(TSize& aPixels, TSize& aTwips) const;
	virtual void GetDisplayExtentOfWindow(const RWindowBase& aWindow, TRect& aExtent) const;
//implementing MTestScreenCapture
	virtual TInt TranslateExtent(const TRect& aInitial, TRect& aTarget) const;
	virtual TInt GetCompositedSize(TSize& aSize) const;
	virtual TInt ComposeScreen(const CFbsBitmap& aBitmap) const;
private:
	//function used by GetResolutions
	TInt FillResolutionArray(TInt aNumOfRes, RArray<TResolution>& aResolutions) const;
protected:
	MWsClientClass* WriteClient() const;
	TInt	iSupportedExtensionsBits;
	CFbsTypefaceStore* iTypefaceStore;
};

#endif /*SCRDEVEXTENSION_H_*/
