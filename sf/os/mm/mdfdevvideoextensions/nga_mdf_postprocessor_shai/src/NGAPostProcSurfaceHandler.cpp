/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

 
#include <e32base.h>
#include <graphics/surface.h> //TSurfaceId
#include <graphics/surfacemanager.h> //RSurfaceManager
#include "MdfRDebug.h"

#include "NGAPostProcSurfaceHandler.h"

//**************************************************

CNGAPostProcSurfaceHandler* CNGAPostProcSurfaceHandler::NewL() 
{ 
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler:NewL() ++"));

    CNGAPostProcSurfaceHandler* self = new (ELeave) CNGAPostProcSurfaceHandler; 
    CleanupStack::PushL(self);
    self->ConstructL(); 
    CleanupStack::Pop();

   PP_DEBUG(_L("CNGAPostProcSurfaceHandler:NewL() --"));
    return self; 
}

void CNGAPostProcSurfaceHandler::ConstructL() 
{ 
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:ConstructL() ++"), this);
    iSurfaceManager = new (ELeave) RSurfaceManager();
    User::LeaveIfError(iSurfaceManager->Open());
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:ConstructL() --"), this);
}

CNGAPostProcSurfaceHandler::CNGAPostProcSurfaceHandler() 
:iSurfaceManager(NULL),
 iSurfaceAttributes(iSurfaceAttributesBuf())
{
}

CNGAPostProcSurfaceHandler::~CNGAPostProcSurfaceHandler()
{
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:~() ++"), this);
   if (iSurfaceManager)
   {
    iSurfaceManager->Close();
    delete iSurfaceManager;
    iSurfaceManager = NULL;
   }
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:~() --"), this);
}

TInt CNGAPostProcSurfaceHandler::CreateSurface(
							const RSurfaceManager::TSurfaceCreationAttributes & aAttributes, 
							TSurfaceId& aSurfaceId, const RChunk& aChunk) 
{ 
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:CreateSurface() ++"), this);
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:CreateSurface() --"), this);
   

   iSurfaceAttributes = aAttributes;
   return(iSurfaceManager->CreateSurface(iSurfaceAttributesBuf, aSurfaceId, aChunk));
}
TInt CNGAPostProcSurfaceHandler::CreateSurface(
							const RSurfaceManager::TSurfaceCreationAttributesBuf & aAttributes, 
							TSurfaceId& aSurfaceId) 
{ 
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:CreateSurface() ++"), this);
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:CreateSurface() --"), this);
    return(iSurfaceManager->CreateSurface(aAttributes, aSurfaceId));
}


TInt CNGAPostProcSurfaceHandler::OpenSurface(const TSurfaceId& aSurfaceId) 
{ 
 PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:OpenSurface() ++"), this);
	PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:OpenSurface() --"), this);
	return(iSurfaceManager->OpenSurface(aSurfaceId));
    
}

TInt CNGAPostProcSurfaceHandler::MapSurface(const TSurfaceId& aSurfaceId, RChunk& aHandle) 
{ 
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:MapSurface() ++"), this);
	PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:MapSurface() --"), this);
	return(iSurfaceManager->MapSurface(aSurfaceId, aHandle));
}

TInt CNGAPostProcSurfaceHandler::SurfaceInfo(const TSurfaceId& aSurfaceId, 
											RSurfaceManager::TInfoBuf& aInfo) 
{
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:SurfaceInfo() ++"), this);
	PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:SurfaceInfo() --"), this);
	return(iSurfaceManager->SurfaceInfo(aSurfaceId, aInfo));
}

TInt CNGAPostProcSurfaceHandler::DestroySurface(const TSurfaceId& aSurfaceId) 
{ 
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:DestroySurface() ++"), this);
	PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:DestroySurface() --"), this);
	return(iSurfaceManager->CloseSurface(aSurfaceId));
}

TInt CNGAPostProcSurfaceHandler::GetBufferOffset(const TSurfaceId& aSurfaceId, TInt aBuffer, TInt& aOffset) 
{ 
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:GetBufferOffset() aBuffer = %d++"), this, aBuffer);
	PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:GetBufferOffset() --"), this);
	return(iSurfaceManager->GetBufferOffset(aSurfaceId, aBuffer, aOffset));
}

TInt CNGAPostProcSurfaceHandler::AddSurfaceHint(const TSurfaceId& aSurfaceId,const RSurfaceManager::THintPair& aHint)
{
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:AddSurfaceHint() ++"), this);
	PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:AddSurfaceHint() --"), this);
	return(iSurfaceManager->AddSurfaceHint(aSurfaceId, aHint));
}

TInt CNGAPostProcSurfaceHandler::SetSurfaceHint(const TSurfaceId& aSurfaceId,const RSurfaceManager::THintPair& aHint)
{
   PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:SetSurfaceHint() ++"), this);
	PP_DEBUG(_L("CNGAPostProcSurfaceHandler[%x]:SetSurfaceHint() --"), this);
	return(iSurfaceManager->SetSurfaceHint(aSurfaceId, aHint));
}

