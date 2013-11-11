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
 @publishedAll
 @released
*/
#ifndef  ECAMDIRECTVIEWFINDER_H
#define  ECAMDIRECTVIEWFINDER_H

#include <e32base.h>
#include <ecam.h>
#include <ecam/ecamdirectviewfinderuids.hrh>

class MCameraDirectViewFinder;

/** 
UID used to identify the CCameraDirectViewFinder API.

@see CCamera::CCameraDirectViewFinder
*/
static const TUid KECamDirectViewFinderUid 	= {KECamDirectViewFinderUidValue};

/** 
CCamera direct view finder class is used to provide support for Direct View Finder functionalities like pause/resume.

@note This class is not intended for sub-classing and used to standardise existing
       varieties of implementations.

@note   This class features are supposed to be meant for the direct viewfinder started using CCamera methods.

@note   If the class methods leave, the output type parameter value is not guaranteed to be valid.

@publishedAll
@released
*/
class CCamera::CCameraDirectViewFinder : public CBase
	{
	/* so can use internal factory functions etc. */
	friend class CCamera;

public:
   
	/** Specifies direct viewfinder state. */ 
	enum TViewFinderState
		{
		/** View Finder is activated */  
		EViewFinderActive, 	 
		/** View Finder has been paused */
		EViewFinderPause,	 
		/** View Finder has been stopped or hasn't yet started. Default state. */
		EViewFinderInActive
		};
	
public:

	IMPORT_C static CCameraDirectViewFinder* NewL(CCamera& aOwner);

    IMPORT_C void PauseViewFinderDirectL();
	IMPORT_C void ResumeViewFinderDirectL();
	IMPORT_C TViewFinderState ViewFinderState() const;
	
	IMPORT_C ~CCameraDirectViewFinder();
    
private:
	CCameraDirectViewFinder(CCamera& aOwner);
	void ConstructL();
	
	void SetImplHandle();
	
private:	
	CCamera&                	iOwner;
	MCameraDirectViewFinder* 	iImpl;  // not owned
   };		
    
#endif // ECAMDIRECTVIEWFINDER_H
