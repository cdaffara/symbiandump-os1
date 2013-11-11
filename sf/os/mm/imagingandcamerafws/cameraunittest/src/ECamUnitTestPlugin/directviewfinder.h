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

#ifndef DIRECTVIEWFINDER_H
#define DIRECTVIEWFINDER_H

#include <ecam/ecamplugin.h>
#include <ecam/ecaminfoplugin.h>

#include "ECamUnitTestPlugin.h"

#include <ecam/mcameradirectviewfinder.h>

class CCamDirectViewFinder : public CBase, public MCameraDirectViewFinder
	{
	
public:	
	static CCamDirectViewFinder* NewL(CCamUnitTestPlugin& aOwner);
	
	void PauseViewFinderDirectL();
	void ResumeViewFinderDirectL();
	CCamera::CCameraDirectViewFinder::TViewFinderState ViewFinderState() const;
	
	void Release();
		
private:
	CCamDirectViewFinder(CCamUnitTestPlugin& aOwner);
	~CCamDirectViewFinder();

private:
	CCamUnitTestPlugin& iOwner;
	CCamera::CCameraDirectViewFinder::TViewFinderState iVFState;

	};
	
#endif // DIRECTVIEWFINDER_H
