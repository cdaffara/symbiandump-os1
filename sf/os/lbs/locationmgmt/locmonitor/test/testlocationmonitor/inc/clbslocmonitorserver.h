/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CLBSLOCMONITORSERVER_H
#define CLBSLOCMONITORSERVER_H

#include "tserverstartparams.h"
#include "csecureasbase.h"
#include "csecureserverbase.h"
#include "csecuresessionbase.h"
#include "csecuresessionsubsessionbase.h"
#include "csubsessionbase.h"
#include "messageutils.h"
#include "lbslogger.h"
#include "clbslocmonitorrequesthandler.h"
#include "clbslocmonitorareainfofinder.h"
#include "clbslocmonitornetworkinfofinder.h"
#include "clbslocmonitorposlistener.h"
#include "lbsrootapi.h"
#include "lbsprocesssupervisor.h"
#include "mlbsconversioncompleteobserver.h"
#include "clbslocmonitorconversionhandler.h"
#include "clbslocmonitorpluginresolver.h"



class CSession2;

/**
Observer class for Location Monitor Server

@internalTechnology
@released
*/
class MLocMonServerDestructObserver
    {
public:
    //Pure virtual implemented by classes which inherit from MLocMonServerDestructObserver
    virtual void LocMonServerDestructed() = 0;
    };


/**
This class implements MCreateServerImpl.

A class that defines a call to CreateServerLC
is required by LBS' ServerFramwork. Its only purpose
is to provide a means to launch the creation of the
real server class (CLbsLocMonitorServer)

@see MCreateServerImpl
@see CLbsLocMonitorServer
@internalComponent
@released
*/
class CLbsLocMonitorCreator : public CBase, public MCreateServerImpl
	{
public:
	virtual void CreateServerLC(TServerStartParams& aParams);	
	};
	
/**
This class defines the Location Monitor server.

It derives from CSecureServerBase to make use of the functionality
provided by LBS' ServerFramework for server, session and subsession 
creation and destruction.

It derives from MLbsEavesDropperObserver to receive positions
observed by the CLbsEavesDropper on any of LBS' internal position buses.

It derives from MLbsAreaInfoObserver to current receive area information
from different sources (e.g. from CLbsCurrentCellInfoFinder)

@see MCreateServerImpl
@internalComponent
@released
*/
class CLbsLocMonitorServer : public CSecureServerBase,
							 public MLbsProcessCloseDown,
							 public MLbsConversionCompleteObserver
	{
	
	enum TAreaInfoFinder
		{
		ENetworkInfoFinder = 0
		};
	
public:

	CLbsLocMonitorServer(TInt aPriority, const CPolicyServer::TPolicy& aSecurityPolicy);
	~CLbsLocMonitorServer();
	void ConstructL(const TDesC& aServerName);

	CSession2* DoNewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

	TVersion GetServerVersion() const;

	// Methods called by the server subsessions to gain access
	// to database for writting and reading positions and area info.
	CLbsLocMonitorRequestHandler& ReadRequestHandler() const;
	
	CLbsLocMonitorConversionHandler* ConversionHandlerL( TUid aConversionPluginId );
	
	CLbsLocMonitorConversionHandler* ConversionHandler( const RMessage2& aMessage );
	
	CLbsLocMonitorPluginResolver* PluginResolver();
	
	void SetLocMonServerDestructObserverL(const MLocMonServerDestructObserver& aDestructObserver);
	void UnsetLocMonServerDestructObserver(const MLocMonServerDestructObserver& aDestructObserver);
	
public: // From MLbsProcessCloseDown
    void OnProcessCloseDown();	
    
public: // From MLbsConversionCompleteObserver
    void HandleConversionComplete( CLbsLocMonitorConversionHandler*
                                   aConversionHandler );
	
protected:

	// From CSecureServerBase (from CPolicyServer)
	CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);
	TBool FindRootProcess();

protected:

	TVersion  iVersion;

	// Sources of area information.
	// Currently only one element in the array as only cell-information is sought.
	RPointerArray<CLbsLocMonitorAreaInfoFinder>  iAreaInfoFinders;

	CLbsLocMonitorPosListener*	iPosListener;
	
	CLbsLocMonitorRequestHandler* iRequestHandler;

	CLbsCloseDownRequestDetector* iCloseDownRequestDetector;
	
	RPointerArray<CLbsLocMonitorConversionHandler> iConversionHandlerArray;
	
	CLbsLocMonitorPluginResolver* iPluginResolver;
	RPointerArray<MLocMonServerDestructObserver> iDestructionObservers;
	};
	
#endif //CLBSLOCMONITORSERVER_H
