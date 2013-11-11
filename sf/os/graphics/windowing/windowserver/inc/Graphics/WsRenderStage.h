// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Interface for window server render stages.
// 
//

#ifndef __WSRENDERSTAGE_H__
#define __WSRENDERSTAGE_H__

#include <e32base.h>
#include <graphics/wsgraphicdrawerinterface.h>

class CFbsBitGc;

/**
This API declares the window server interface to the Render Stage Pipeline.
@publishedPartner
@prototype
*/
class CWsRenderStage : public CBase, public MWsObjectProvider
	{
private:
	class CPimpl;
	
public: // pure virtuals
#ifdef SYMBIAN_GRAPHICS_GCE
	/**
	Signal to the render stage to perform intitialisation prior to a batch
	of draw ops
	@param	aRegion	The dirty region for which drawing will be issued.
					The caller guarantees that same dirty region applies
					across a matched set of Begin and End calls.  The
					render stage may pass a different dirty region to
					child render stages, but it must guarantee the same
					dirty region applies across a matched set of Begin and
					End calls.
	*/
	virtual void Begin(const TRegion* aRegion) = 0;
	
	/**
	Completes a pass of this render stage.
	@param aFinished If not NULL, then this is signalled when the next
					(down stream) render stage is ready to receive further
					updates.
	@note 			It is valid to call MWsAnimationScheduler::Animate() again,
					before aFinished has been signalled, but the render stage
					pipeline may not be ready to receive updates at that time.
	*/
	virtual void End(TRequestStatus* aFinished) = 0;
#else //SYMBIAN_GRAPHICS_GCE
	/**
	Returns the GC to use for drawing in this render stage:
	*/
	virtual CFbsBitGc * Begin() = 0;
	
	/**
	Completes a pass of this render stage:
	*/
	virtual void End() = 0;
#endif //SYMBIAN_GRAPHICS_GCE

public:
	/** Destructor */
	IMPORT_C virtual ~CWsRenderStage();
	
	/**
	Returns the next render stage in the pipeline:
	*/
	IMPORT_C virtual CWsRenderStage * Next();

	/**
	Sets the next render stage in the pipeline:
	*/
	IMPORT_C virtual void SetNext(CWsRenderStage * aNext);
	
	/**
	Returns extended functionality.
	@param	aTypeId	interface extension type specifier
	@return an interface implementation, if the interface ID is implemented by
			the render stage, otherwise passes the call on to the next render
			stage in the pipeline.  The last render stage returns NULL if no 
			render stage in the pipeline implements the specified extension.
	*/
	IMPORT_C virtual TAny * ResolveObjectInterface(TUint aTypeId);

protected:	
	/** Constructor */
	IMPORT_C CWsRenderStage();
	
	/**
	Second phase construction.  Must be called from derived class construction sequence.
	*/
	IMPORT_C void BaseConstructL();
	
private:
	CWsRenderStage(const CWsRenderStage &); // no implementation
	CWsRenderStage & operator=(const CWsRenderStage &); // no implementation

private:
	CPimpl * iPimpl;
	};

#endif //__WSRENDERSTAGE_H__
