// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef CTSMALLWINDOWOPENVG_H
#define CTSMALLWINDOWOPENVG_H


#include "twindow.h"
#include "eglenvironment.h"

#include <e32std.h>
#include <e32base.h>
#include <EGL/egl.h>
#include <VG/openvg.h>
#include <VG/vgu.h>

// CLASS DECLARATION

/**
 *  CTSmallWindow
 * 
 */
class CTSmallWindowOpenVG : public CTWindow
	{
public:
	virtual ~CTSmallWindowOpenVG();
    static CTWindow* NewL(RWsSession &aWs,
            const RWindowTreeNode &aParent, 
            const TPoint& aStartingPoint,
            const TSize& aWindowSize);

    virtual void RenderL();
		
private:
	CTSmallWindowOpenVG(const TPoint& aStartingPoint, const TSize& aWindowSize);
	void ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent);

private:
	EGLDisplay iDisplay;
	EGLSurface iSurface;
	EGLContext iContextVG;
	
    VGfloat iCurrentRotation;
    VGPaint iStrokePaint;
    VGPaint iFillPaint;
    VGPath  iPath;

    TInt iTime;
	};

#endif // CTSMALLWINDOWOPENVG_H
