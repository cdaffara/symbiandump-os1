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
 @internalComponent
*/

#ifndef CONTROLLERINITIALISATIONPLUGINIMPL_H
#define CONTROLLERINITIALISATIONPLUGINIMPL_H

#include <bluetooth/hci/controllerinitialisationplugin.h>
#include <bluetooth/hci/controllerinitialisationinterface.h>
#include <bluetooth/hcicommandqueueclient.h>

class MCoreHci;

NONSHARABLE_CLASS(CControllerInitialisationPluginImpl) : public CControllerInitialisationPlugin,
														 public MControllerInitialisationInterface,
														 public MControllerInitialisationAbortExtensionInterface
	{
public:
	static CControllerInitialisationPluginImpl* NewL();
	~CControllerInitialisationPluginImpl();
	virtual TAny* Interface(TUid aUid);

protected:
	CControllerInitialisationPluginImpl();

private:
	void ConstructL();

	// MControllerInitialisationInterface
	virtual void MciiPreResetCommand();
	virtual void MciiPostResetCommand();
	virtual void MciiSetHCICommandQueue(MHCICommandQueue& aHCICommandQueue);
	virtual void MciiSetControllerInitialisationObserver(MControllerInitialisationObserver& aObserver);
	virtual void MciiSetCoreHci(MCoreHci& aCoreHci);

	// MControllerInitialisationAbortExtensionInterface
	virtual void MciaeiAbortInitialisation();
	
private:
	MControllerInitialisationObserver* iControllerInitialisationObserver;
	};

#endif // CONTROLLERINITIALISATIONPLUGINIMPL_H
