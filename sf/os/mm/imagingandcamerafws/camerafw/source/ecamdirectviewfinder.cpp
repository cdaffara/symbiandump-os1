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

#include <ecam/ecamdirectviewfinder.h>
#include <ecam/ecamdirectviewfinderuids.hrh>
#include <ecam/mcameradirectviewfinder.h>

/**
Factory function for creating the CCameraDirectViewFinder object.

@param aCamera 
	   a reference to a CCamera object providing the settings.

@return a pointer to a fully constructed CCameraDirectViewFinder object.

@leave KErrNoMemory Out of memory Or any other system-wide error code.
*/
EXPORT_C CCamera::CCameraDirectViewFinder* CCamera::CCameraDirectViewFinder::NewL(CCamera& aCamera)
	{
	CCamera::CCameraDirectViewFinder* self = new (ELeave)CCamera::CCameraDirectViewFinder(aCamera); 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self; 
	}
	
void CCamera::CCameraDirectViewFinder::ConstructL()
	{
	SetImplHandle();
	if(iImpl == NULL)
		{
		User::LeaveIfError(KErrNotSupported);		
		}
	}
	
/**	
CCameraDirectViewFinder Constructor.

@param aOwner
       a reference to a CCamera object providing the settings. 
*/
CCamera::CCameraDirectViewFinder::CCameraDirectViewFinder(CCamera& aOwner): iOwner(aOwner), iImpl(NULL)
	{
	}
	
/** 
Pauses the direct viewfinder   

@leave KErrNoMemory: Out of memory. 

@note   This will pause the direct viewfinder which is started using the CCamera method.

@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect)
@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect)
*/
EXPORT_C void CCamera::CCameraDirectViewFinder::PauseViewFinderDirectL() 
	{
	iImpl->PauseViewFinderDirectL();
	}
	
/** 
Resumes the direct viewfinder   

@leave KErrNoMemory Out of memory.

@note   This will resume the direct viewfinder which is started using the CCamera method.
	
@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect)
@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect)
*/
EXPORT_C void CCamera::CCameraDirectViewFinder::ResumeViewFinderDirectL()
	{
	iImpl->ResumeViewFinderDirectL();
	}

/** 
Gets the current view finder state.

@return  Retrieves the current direct view finder state.

@note   This will retrieve the state of the direct viewfinder which is started using the CCamera method.
	
@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect)
@see	CCamera::StartViewFinderDirectL(RWsSession& aWs,CWsScreenDevice& aScreenDevice,RWindowBase& aWindow,TRect& aScreenRect,TRect& aClipRect)
*/
EXPORT_C CCamera::CCameraDirectViewFinder::TViewFinderState CCamera::CCameraDirectViewFinder::ViewFinderState() const
	{
	return iImpl->ViewFinderState();
	}
	
/**
Destructor
*/	
EXPORT_C CCamera::CCameraDirectViewFinder::~CCameraDirectViewFinder()
	{
	if (iImpl != NULL)
		{
		iImpl->Release();
		}
	}	
	
/** 
Gets the handle to the implementer's MCameraDirectViewFinder derived class.
*/	
void CCamera::CCameraDirectViewFinder::SetImplHandle()
	{
	if(iImpl == NULL)
		{
		iImpl = static_cast<MCameraDirectViewFinder*>(iOwner.CustomInterface(KECamMCameraDirectViewFinderUid));
		}
	}
