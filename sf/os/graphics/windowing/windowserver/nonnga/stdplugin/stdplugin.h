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
// STDPLUGIN.CPP
// 
//

#ifndef __STDPLUGIN_H__
#define __STDPLUGIN_H__

#include <Graphics/WSGRAPHICDRAWERINTERFACE.H>

/**
The window server itself does not (and must not) understand any of these values.  These are 
used internally within the standard set of plugins provided in the DLL so that they are able
to communicate with one another.
*/
enum WsStdPluginIds
	{
	EWsFbsBitGcImpl = 0x2001B70A,
	};
	
/**
This interface is used internally by the standard plugin implementations to communicate with each
other.  It works on the principle that they understand the value it returns and what to cast it to.
*/
class MWsFbsBitGcImpl : public MWsObjectProvider
	{
public:
	virtual CFbsBitGc* WsFbsBitGcImpl();
	};

#endif //__STDPLUGIN_H__
