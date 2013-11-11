// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Definition of base class for all objects
// 
//

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <e32std.h>
#include <e32base.h>
#include "w32cmd.h"
#include "screen.h"

class CWsClient;
class CWsRootWindow;

/** 
CWsObject is the base class for the server-side objects that are visible to the client.

The client refers to these objects via a handle that is passed to it by the server.
The association between handles and CWsObject instances is stored by the TWsObject class.

Derived classes must implement the CommandL() function so that it handles the requests
the client sent.

Each client session has a list of all its CWsObject instances. When creating an instance of 
a class derived from CWsObject the NewObjL() function must be called to add the new 
object to the list. The CWsObject destructor takes care of removing the object from the list.

@see TWsObject
@see CWsClient
@internalComponent
@released
*/
class CWsObject : public CBase
	{
public:
	CWsObject(CWsClient* aOwner,WH_HANDLES aType);
	~CWsObject();
	virtual void CommandL(TInt aOpcode, const TAny *aCmdData)=0;
	virtual void CloseObject();
	TInt LogHandle() const;
	void NewObjL();
	void RemoveFromIndex();
	inline WH_HANDLES Type() const;
	inline CWsClient *WsOwner() const;
	inline void SetWsOwner(CWsClient *aOwner);
	void OwnerPanic(TClientPanic aPanic) const;
protected:
	void SetReply(TInt aReply);
	
private:
	WH_HANDLES iType;
protected:
	/** Each client has a list of all its CWsObject instances. The CWsObject 
	has a pointer to its owner so that it can update the list when it is created and
	when it is destroyed. */
	CWsClient *iWsOwner;
	};

class CWsScreenObject : public CWsObject
	{
public:
	inline CWsScreenObject(CWsClient *aOwner,WH_HANDLES aType,CScreen* aScreen);
	inline CScreen* Screen() const;
	inline CWsRootWindow* RootWindow() const;

protected:
	CScreen* iScreen;
	};


//
// inlines			//
//

//
// CWsObject
//
inline CWsClient *CWsObject::WsOwner() const
	{return(iWsOwner);}
inline void CWsObject::SetWsOwner(CWsClient *aOwner)
	{iWsOwner=aOwner;}
inline WH_HANDLES CWsObject::Type() const
	{return(iType);}
//
// CWsScreenObject
//
inline CWsScreenObject::CWsScreenObject(CWsClient *aOwner,WH_HANDLES aType,CScreen* aScreen) : CWsObject(aOwner,aType), iScreen(aScreen)
	{}
inline CScreen* CWsScreenObject::Screen() const
	{return iScreen;}
inline CWsRootWindow* CWsScreenObject::RootWindow() const
	{return iScreen->RootWindow();}
#endif
