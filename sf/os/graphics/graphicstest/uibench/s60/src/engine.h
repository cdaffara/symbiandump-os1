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

#ifndef ENGINE_H_
#define ENGINE_H_


#include <coedef.h>
#include <w32std.h>

class MEngine
	{
public:
	virtual void ActivateL() = 0;
	virtual void Deactivate() = 0;
	virtual void Step()= 0 ;
	virtual TBool IsPending() = 0;
	virtual TInt GetSpeed() = 0;
	
	virtual TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent) = 0;
	virtual void Refresh() = 0;
	
	inline virtual ~MEngine() {}; // needed, otherwise destructor of derived class is not called
	};

#endif 
