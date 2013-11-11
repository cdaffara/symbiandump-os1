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

#ifndef EGLENVIRONMENT_H_
#define EGLENVIRONMENT_H_


#include <GLES/egl.h>
#include <w32std.h>

class CEglGraphicsOutput;


static const TInt KMaxConfigs = 1024; // Maximal number of Configurations

/*
 The EGL Environment class wraps all the stuff you have to do to initialise 
 the EGL, and owns all the EGL objects. It acts as a factory for 
 the CEglGraphicsOutput object.
*/ 
class CEglEnvironment : public CBase
	{
public:
	static CEglEnvironment* NewL(RWindow& aWindow);
	~CEglEnvironment();
	void DrawToWindow() const;
	EGLDisplay Display() const;
	EGLSurface Surface() const;
	
private:
	CEglEnvironment(RWindow& aWindow);
	void ConstructL();
	
	
private:    
	RWindow& iWindow;
	
	EGLDisplay iDisplay;
	EGLSurface iSurface;
	EGLContext iContext;
	EGLConfig iConfig[KMaxConfigs];
	};

#endif /* EGLENVIRONMENT_H_ */
