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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef  MCAMERADIRECTVIEWFINDER_H
#define  MCAMERADIRECTVIEWFINDER_H

#include <ecam/ecamdirectviewfinder.h>
#include <ecam/cameradirectviewfinderuidif.hrh>

/** This is the UID which is used to obtain the interface MCameraDirectViewFinder, via the 
CCamera::CustomInterface() call. */   
static const TUid KECamMCameraDirectViewFinderUid 			=  {KECamMCameraDirectViewFinderUidValue};

/** Mixin class for implementation of camera direct viewfinder control.
*/	
class MCameraDirectViewFinder
	{

public:
	
	/** 
	Releases the interface. 
	*/
	virtual void Release()=0;

	/** 
	Pauses the direct view finder   
	
	@leave KErrNoMemory: Out of memory or any other leave code 
	
	@note   This will pause the direct viewfinder which is started using the CCamera method.
	
	@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect)
	@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect)
	*/
	virtual void PauseViewFinderDirectL()=0;
	
	/** 
	Resumes the direct view finder  
	 
	@leave KErrNoMemory: Out of memory or any other leave code
	
	@note   This will resume the direct viewfinder which is started using the CCamera method.
	
	@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect)
	@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect)
	*/
	virtual void ResumeViewFinderDirectL()=0;
	
	/** 
	Gets the current view finder state.
	
	@return  Retrieves the current direct view finder state.
	
	@note   This will retrieve the state of the direct viewfinder which is started using the CCamera method.
	
	@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect)
	@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect)
	*/
	virtual CCamera::CCameraDirectViewFinder::TViewFinderState ViewFinderState() const =0;
  	};

#endif // MCAMERADIRECTVIEWFINDER_H
