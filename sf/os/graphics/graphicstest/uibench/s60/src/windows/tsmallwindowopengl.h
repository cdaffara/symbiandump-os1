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

#ifndef CTSMALLWINDOWOPENGL_H
#define CTSMALLWINDOWOPENGL_H

#include "tflowwindowopengl.h"

// CLASS DECLARATION

/**
 *  CTSmallWindow
 * 
 */
class CTSmallWindowOpenGL : public CTFlowWindowOpenGL
	{
public:
    virtual ~CTSmallWindowOpenGL();
    static CTWindow* NewL(RWsSession &aWs,
            const RWindowTreeNode &aParent, 
            const TPoint& aStartingPoint,
            const TSize& aWindowSize);

protected:
    CTSmallWindowOpenGL(const TPoint& aStartingPoint, const TSize& aWindowSize);
    void ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent);
    
	};

#endif // CTSMALLWINDOWOPENGL_H
