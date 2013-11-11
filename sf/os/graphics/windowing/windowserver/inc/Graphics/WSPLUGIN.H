// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Server-side base-classes for graphic drawer plugins
// 
//

#ifndef __WSPLUGIN_H__
#define __WSPLUGIN_H__

#include <e32base.h>
#include <e32std.h>

#include <graphics/wsgraphicdrawerinterface.h>

/**
Window Server interface to the Window Server plugin API.

@publishedPartner
@prototype
*/
class CWsPlugin : public CBase, public MWsObjectProvider
	{
private:
	class CPimpl;
	friend class CPimpl;

public:
	/**
	This function should be overriden by all derived classes.  The first
	call the implementation of this function should make is to BaseConstructL().
	@param aEnv 	the environment this drawer exists in
	@param aData 	arbitrary data for constructing this instance, sent from
					the client.
	*/
	virtual void ConstructL(MWsGraphicDrawerEnvironment& aEnv,const TDesC8& aData) = 0;
	IMPORT_C ~CWsPlugin();
	IMPORT_C virtual const TDesC & PluginName() const;

	static TInt DtorIDKeyOffset() { return _FOFF(CWsPlugin,iDtor_ID_Key); }
	
protected:
	IMPORT_C CWsPlugin();
	IMPORT_C void BaseConstructL(MWsGraphicDrawerEnvironment& aEnv);
	IMPORT_C MWsGraphicDrawerEnvironment& Env();
	IMPORT_C const MWsGraphicDrawerEnvironment& Env() const;

private:
	CWsPlugin(const CWsPlugin &); // no implementation
	CWsPlugin & operator=(const CWsPlugin &); // no implementation
	
private:
	TUid iDtor_ID_Key;
	CPimpl* iPimpl;
	};


#endif //#ifndef __WSPLUGIN_H__
