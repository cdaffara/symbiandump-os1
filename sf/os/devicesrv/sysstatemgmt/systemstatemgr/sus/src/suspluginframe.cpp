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
//

#include <ssm/ssmutility.h>
#include "suspluginframe.h"
#include "ssmdebug.h"
#include "suspanic.h"

/**
 */
typedef MSsmUtility* (*TFuncNewL)(void);

/**
 */
CSusPluginFrame::CSusPluginFrame()
 	{
 	}

/**
 */
CSusPluginFrame* CSusPluginFrame::NewL(TLibraryFunction aNewLFunc, TInt32 aNewLOrdinal)
 	{
 	CSusPluginFrame* self = new (ELeave) CSusPluginFrame();
 	CleanupStack::PushL(self);
 	self->ConstructL(aNewLFunc, aNewLOrdinal);
 	CleanupStack::Pop(self);
 	return self;
 	}

/**
*/
void CSusPluginFrame::ConstructL(TLibraryFunction aNewLFunc, TInt32 aNewLOrdinal)
 	{
 	TFuncNewL newL = reinterpret_cast<TFuncNewL>( aNewLFunc );
 	iPlugin = newL();
 	iNewLOrdinal = aNewLOrdinal;
 	SSMLOGLEAVEIFNULL(iPlugin);
 	}

/**
 */
 CSusPluginFrame::~CSusPluginFrame() 
  	{
  	Release();
  	iPlugin = NULL;
 	if(KNullHandle != iLibrary.Handle())
 		{
 		//release the dll
		iLibrary.Close(); 
 		}
  	}

/**
 */	
void CSusPluginFrame::SetLibrary(RLibrary& aLibrary)
	{
	iLibrary = aLibrary;
	}

/**
 */	
void CSusPluginFrame::InitializeL()
	{
	__ASSERT_ALWAYS(iPlugin, User::Panic(KPanicSsmSus, EPluginFrameError1));
 	iPlugin->InitializeL();
	}

/**
 */	
void CSusPluginFrame::StartL()
	{
	__ASSERT_ALWAYS(iPlugin, User::Panic(KPanicSsmSus, EPluginFrameError2));
 	iPlugin->StartL();
	}

/**
 */	
void CSusPluginFrame::Release()
	{
	if(iPlugin)
 		{
 		iPlugin->Release();
 		iPlugin = NULL;
 		}
	}

/**
 */
TFileName CSusPluginFrame::FileName() const
	{
	return iLibrary.FileName();
	}

TInt CSusPluginFrame::NewLOrdinal() const
	{
	return iNewLOrdinal;
	}

