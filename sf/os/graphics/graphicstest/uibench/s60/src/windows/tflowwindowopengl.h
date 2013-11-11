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

#ifndef TFLOWWINDOWOPENGL_H
#define TFLOWWINDOWOPENGL_H

#include <e32std.h>
#include <e32base.h>

#include "twindow.h"
#include "eglenvironment.h"
#include "model.h"


/**
 *  Renders and draws OpenGL ES scenes to the screen.
 * 
 */
class CTFlowWindowOpenGL : public CTWindow
	{
public:
	virtual ~CTFlowWindowOpenGL();
	static CTWindow* NewL(RWsSession &aWs,
			const RWindowTreeNode &aParent, 
			const TPoint& aStartingPoint,
			const TSize& aWindowSize);

	virtual void RenderL();
	
protected:
	CTFlowWindowOpenGL(const TPoint& aStartingPoint, const TSize& aWindowSize);
	void ConstructL(RWsSession &aWs, const RWindowTreeNode &aParent);
	void SetResolutionL(TInt aResolution);

private:
	CEglEnvironment* iEglEnvironment;
	CModel* iModel;
	TInt iTime;
	};

#endif // TFLOWWINDOWOPENGL_H
