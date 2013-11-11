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

#include "tdirectgdieglcontent_server.h"
#include "tdirectgdieglcontent_cube.h"
#include "tdirectgdieglcontent_clientserver.h"
#include "tdisplaymode_mapping.h"
#include <e32debug.h>
#include <fbs.h>
#include <graphics/sgimage_sw.h>

/**
Static constructor.
*/
CEglContent* CEglContent::NewL()
	{
	CEglContent* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

CEglContent* CEglContent::NewLC()
	{
	CEglContent* self = new(ELeave) CEglContent();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
1st phase constructor
*/
CEglContent::CEglContent() : CTimer(CActive::EPriorityStandard), iMode(ESync)
	{
	CActiveScheduler::Add(this);
	}

/**
2nd phase constructor
*/
void CEglContent::ConstructL()
	{
	CTimer::ConstructL();

	iCube = CGLCube::NewL(EUidPixelFormatXRGB_8888, TSize(200, 200));
	}

/**
Destructor
*/
CEglContent::~CEglContent()
	{
	Cancel();

	delete iCube;
	}

/**
Handles an active object's request completion event.
*/
void CEglContent::RunL()
	{
	if(iStatus.Int() == KErrNone)
		{
		RenderNextFrame();
		iFrame = iFrame % KEglContentAsyncMaxFrames;

		if(iMode == EAsync)
			{
			// issue request for next frame
			After(KEglContentDelay);
			}
		}
	}

/**
Render next frame of animation.
Function returns when rendering is finished. Animation is set to next frame.
*/
void CEglContent::RenderNextFrame()
	{
	iCube->Render(iFrame);
	iLastFrame = iFrame;
	iFrame++;
	}

/**
Set rendering mode to synchronous or asynchronous
@param aMode Rendering mode to set.
*/
void CEglContent::SetMode(TMode aMode)
	{
	if(aMode == iMode)
		return;

	iMode = aMode;

	// reset mode
	if(aMode == ESync)
		{
		// cancel request for next frame
		Cancel();
		iFrame = 0;
		}
	else if(aMode == EAsync)
		{
		// render init frame
		iFrame = 0;
		RenderNextFrame();
		// issue request for next frame
		After(KEglContentDelay);
		}
	else // EAsyncDebug
		{
		// render init frame
		iFrame = 0;
		RenderNextFrame();
		}
	}

/**
Get image id of current frame. Current image to render is switch to next.
@param aId Reference to drawable id class to store image id.
@return number of frame stored in image
*/
TInt CEglContent::GetImage(TSgDrawableId& aId)
	{
	if(iMode == ESync)
		{
		// if rendering mode is synchrounous, we need to render current frame
		RenderNextFrame();
		}

	iCube->GetImage(aId);

	if(iMode == EAsyncDebug)
		{	
		// Added this as a panic can occur if After() is called while the server is active. 
		// Before this was added the server could get stuck in an active state
		// occasionally when running the tests in SW mode.
		if (IsActive())					
			Cancel();
					
		// if rendering mode is asynchrounous debug, we start rendering next frame (only one) immediately
		After(0);
		}

	return iLastFrame;
	}

/**
Static constructor
*/
CServer2* CEglContentServer::NewLC()
	{
	CEglContentServer* self = new(ELeave) CEglContentServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
1st phase constructor
*/
CEglContentServer::CEglContentServer() : CServer2(0, ESharableSessions)
	{
	}

/**
2nd phase construction - ensure the content renderer and server objects are running
*/
void CEglContentServer::ConstructL()
	{
	StartL(KEglContentServerName);
	iContent = CEglContent::NewL();
	}

/**
Destructor.
*/
CEglContentServer::~CEglContentServer()
	{
	delete iContent;
	}

/**
Cretae a new client session.
*/
CSession2* CEglContentServer::NewSessionL(const TVersion&, const RMessage2&) const
	{
	return new(ELeave) CEglContentSession();
	}

/*
A new session is being created
*/
void CEglContentServer::AddSession()
	{
	++iSessionCount;
	}

/*
A session is being destroyed
*/
void CEglContentServer::DropSession()
	{
	--iSessionCount;
	}

/**
Get image in synchronous mode.
@param aId Reference to drawable id class to store image id.
*/
void CEglContentServer::GetSyncImage(TSgDrawableId& aId)
	{
	if(iContent)
		{
		iContent->SetMode(CEglContent::ESync);
		iContent->GetImage(aId);
		}
	}

/**
Get image in asynchronous mode.
@param aId Reference to drawable id class to store image id.
@return number of frame stored in image
*/
TInt CEglContentServer::GetAsyncImage(TSgDrawableId& aId)
	{
	if(iContent)
		{
		iContent->SetMode(CEglContent::EAsync);
		return iContent->GetImage(aId);
		}
	return -1;
	}

/**
Get image in asynchronous debug mode.
@param aId Reference to drawable id class to store image id.
@return number of frame stored in image
*/
TInt CEglContentServer::GetAsyncImageDebug(TSgDrawableId& aId)
	{
	if(iContent)
		{
		iContent->SetMode(CEglContent::EAsyncDebug);
		return iContent->GetImage(aId);
		}
	return -1;
	}

/**
1st phase constructor
*/
CEglContentSession::CEglContentSession()
	{
	}

/**
2nd phase construct for sessions - called by the CServer framework
*/
void CEglContentSession::Create()
	{
	Server().AddSession();
	}

/**
Destructor.
*/
CEglContentSession::~CEglContentSession()
	{
	Server().DropSession();
	}

/**
Get server object.
@return a reference to server object
*/
CEglContentServer& CEglContentSession::Server()
	{
	return *static_cast<CEglContentServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

/**
Handle a client request.
Leaving is handled by CEglContentSession::ServiceError() which reports
the error code to the client
*/
void CEglContentSession::ServiceL(const RMessage2& aMessage)
	{
	switch(aMessage.Function())
		{
		case ETerminateServer:
			{
			aMessage.Complete(KErrNone);
			CActiveScheduler::Stop();
			break;
			}
		case EGetSyncImage:
			{
			// Get the current image synchronously, the frame
			// number is not returned as the client will already know the frame number
			// as it controls when a frame is drawn
			TSgDrawableId id;
			Server().GetSyncImage(id);
			TPckg<TSgDrawableId> idPckg(id);
			aMessage.Write(0, idPckg);
			aMessage.Complete(KErrNone);
			break;
			}
		case EGetAsyncImage:
			{
			// Get the current image and it's frame number, the drawing is
			// asynchronous so the client needs to know which one this as it cannot tell otherwise
			TSgDrawableId id;
			TInt fnum = Server().GetAsyncImage(id);
			TPckg<TSgDrawableId> idPckg(id);
			aMessage.Write(0, idPckg);
			TPckg<TInt> fnumPckg(fnum);
			aMessage.Write(1, fnumPckg);
			aMessage.Complete(KErrNone);
			break;
			}
		case EGetAsyncImageDebug:
			{
			// Get the current image and it's frame number, the drawing is
			// asynchronous so the client needs to know which one this as it cannot tell otherwise
			TSgDrawableId id;
			TInt fnum = Server().GetAsyncImageDebug(id);			
			TPckg<TSgDrawableId> idPckg(id);
			aMessage.Write(0, idPckg);
			TPckg<TInt> fnumPckg(fnum);
			aMessage.Write(1, fnumPckg);
			aMessage.Complete(KErrNone);
			break;
			}
		default:
			{
			PanicClient(aMessage, EPanicIllegalFunction);
			}
		}
	}

/**
Handle an error from CEglContentSession::ServiceL()
*/
void CEglContentSession::ServiceError(const RMessage2& aMessage, TInt aError)
	{
	if(aError == KErrBadDescriptor)
		PanicClient(aMessage, EPanicBadDescriptor);
	CSession2::ServiceError(aMessage, aError);
	}


/**
RMessage::Panic() also completes the message.
*/
void PanicClient(const RMessagePtr2& aMessage, TEglContentPanic aPanic)
	{
	aMessage.Panic(KEglContentServerName, aPanic);
	}

/**
Perform all server initialisation, in particular creation of the
scheduler and server and then run the scheduler
*/
static void RunServerL()
	{
	// naming the server thread after the server helps to debug panics
	User::LeaveIfError(RThread::RenameMe(KEglContentServerName));
	//
	// create and install the active scheduler we need
	CActiveScheduler* s = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(s);
	CActiveScheduler::Install(s);
	//
	// create the server (leave it on the cleanup stack)
	CEglContentServer::NewLC();
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous(KErrNone);
	//
	// Ready to run
	CActiveScheduler::Start();
	//
	// Cleanup the server and scheduler
	CleanupStack::PopAndDestroy(2);
	}

/**
Entry point of server executable.
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	RDebug::Print(_L("%S started"), &KEglContentServerName);
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TInt r = KErrNoMemory;
	if(cleanup)
		{
		TRAP(r, RunServerL());
		delete cleanup;
		}
	RDebug::Print(_L("%S terminated"), &KEglContentServerName);
	__UHEAP_MARKEND;
	return r;
	}
