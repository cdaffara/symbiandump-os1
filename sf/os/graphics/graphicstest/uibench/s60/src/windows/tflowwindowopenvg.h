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

#ifndef TFLOWWINDOWOPENVG_H
#define TFLOWWINDOWOPENVG_H

#include "twindow.h"
#include "eglrendering.h"

#include <e32std.h>
#include <e32base.h>


/**
 *  Renders and draws OpenVG scenes to the screen.
 * 
 */
class CTFlowWindowOpenVG : public CTWindow
	{
public:
	virtual ~CTFlowWindowOpenVG();
	static CTWindow* NewL(RWsSession &aWs,
			const RWindowTreeNode &aParent, 
			const TPoint& aStartingPoint,
			const TSize& aWindowSize);

	virtual void RenderL();

protected:
	CTFlowWindowOpenVG(const TPoint& aStartingPoint, const TSize& aWindowSize);
	void ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent);
	
private:
	CEGLRendering* iVGRendering;	
	TInt iInterleaveCounter;
	};

#endif // TFLOWWINDOWOPENVG_H
