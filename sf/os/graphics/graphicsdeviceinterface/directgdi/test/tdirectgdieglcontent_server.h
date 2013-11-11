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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef __TDIRECTGDIEGLCONTENT_SERVER_H__
#define __TDIRECTGDIEGLCONTENT_SERVER_H__

#include <e32base.h>
#include <pixelformats.h>
#include <graphics/sgimage.h>
#include <graphics/sgimagecollection.h>
#include <GLES/egl.h>

/**
Egl content panic codes
*/
enum TEglContentPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	EPanicAlreadyReceiving
	};

void PanicClient(const RMessagePtr2& aMessage, TEglContentPanic aPanic);

const TInt KEglContentDelay = 33333; // frame rate: 30 fps

class CGLCube;

/**
This class contains the egl content renderer.
CEglContent is a wrapper for CGLCube and works in two modes:
	- Synchronous – when requested for a TsgDrawableId object CEglContent 
		it returns it from CGLCube and the frame number is increased.  
		Thus in the next request the next frame is obtained. 
		Nothing is rendered until the new request occurs.
	- Asynchronous – in this mode CEglContent forces GLCube to render 
		frames at 30 fps frequency. On the request last rendered a frame is returned 
		and generation is continued in the background.
	- Asynchronous debug - start rendering the next frame (only one) immediately
*/
class CEglContent : public CTimer
	{
public:
	/** Available rendering modes */
	enum TMode
		{
		ESync, // Rendering is synchronised with client
		EAsync, // Rendering is asynchronous with fixed rate
		EAsyncDebug // Rendering is debug asynchronous with fixed rate
		};
public:
	static CEglContent* NewL();
	static CEglContent* NewLC();
	virtual ~CEglContent();

	void SetMode(TMode aMode);
	TInt GetImage(TSgDrawableId& aId);

private:
	CEglContent();
	void ConstructL();
	void RenderNextFrame();

private: // from CTimer
	void RunL();

private:
	CGLCube* iCube;
	/** Rendering mode (sync or async) */
	TMode iMode;
	/** Frame number to be rendered */
	TInt iFrame;
	/** Last rendered frame number */
	TInt iLastFrame;
	};

/**
This class contains egl content server.
Implements server and manages CEglContent object.
*/
class CEglContentServer : public CServer2
	{
public:
	static CServer2* NewLC();
	void AddSession();
	void DropSession();
	void GetSyncImage(TSgDrawableId& aId);
	TInt GetAsyncImage(TSgDrawableId& aId);
	TInt GetAsyncImageDebug(TSgDrawableId& aId);
private:
	CEglContentServer();
	~CEglContentServer();
	void ConstructL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
private:
	TInt iSessionCount;
	CEglContent* iContent;
    };

/**
This class contains server side of egl content server session.
*/
class CEglContentSession : public CSession2
	{
public:
	CEglContentSession();
	void Create();
private:
	~CEglContentSession();
	CEglContentServer& Server();
	void ServiceL(const RMessage2& aMessage);
	void ServiceError(const RMessage2& aMessage, TInt aError);
	};

#endif
