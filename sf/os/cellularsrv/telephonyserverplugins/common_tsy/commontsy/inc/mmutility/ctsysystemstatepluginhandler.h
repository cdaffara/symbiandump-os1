/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CTSYSYSTEMSTATEPLUGINHANDLER_H
#define CTSYSYSTEMSTATEPLUGINHANDLER_H

#include <e32base.h>
#include <et_tsy.h>

#include <ctsy/serviceapi/cmmutility.h>
#include <ctsy/pluginapi/cmmdatapackage.h>

class CCtsySystemStatePluginService;
class CCtsySystemStatePluginHandler;

class TCtsySsmCallbackData 
	{
public:
	CMmDataPackage	iNumberPackage;	// emergency call number	
	TTsyReqHandle	iReqHandle;	
	};
	
/*	
* Abstract class for 
* SsmPluginCallback()
*/
//MCtsySsmPluginCallback
class MCtsySsmPluginCallback
	{	
public: // Interface
	// aResult -- result returned by SSM
	virtual void SsmPluginCallback(TInt aResult, TCtsySsmCallbackData& aCallbackData) = 0; // called by CCtsySystemStatePlugin, after answer from SSM		
	};




/*	
* Plugin handler: encapsulates the plugin loading and unloading and
* delegates requests onto plugin implementation	
*/
NONSHARABLE_CLASS(CCtsySystemStatePluginHandler) : public CBase
	{
public:
	static CCtsySystemStatePluginHandler* NewL();
	static CCtsySystemStatePluginHandler* NewLC();
	virtual ~CCtsySystemStatePluginHandler();
//	void RegisterSsmPluginCallback(MCtsySsmPluginCallback* aSsmPluginCallback);

private:
	 CCtsySystemStatePluginHandler();
     void ConstructL();
     
public:        
	TInt Connect();
		
	// asynchronous	version	
	// aSsmPluginCallback -- callback to call on completion of ActivateRfForEmergencyCall
	void ActivateRfForEmergencyCall(MCtsySsmPluginCallback* aSsmPluginCallback, TCtsySsmCallbackData& aCallbackData);
	
	// asynchronous	version
	// aSsmPluginCallback -- callback to call on completion of DeactivateRfAfterEmergencyCall	
	void DeactivateRfAfterEmergencyCall();
	
    void Close();
private:
	CCtsySystemStatePluginService* iSystemStatePluginService;                            
	};

class CCtsySystemStatePluginService : public CBase
	{
public:
    inline static CCtsySystemStatePluginService* NewL();
    inline virtual ~CCtsySystemStatePluginService();		    
    virtual TInt Connect() = 0;
	virtual void ActivateRfForEmergencyCall(MCtsySsmPluginCallback* aSsmPluginCallback, TCtsySsmCallbackData& aCallbackData) = 0;
	virtual void DeactivateRfAfterEmergencyCall() = 0;
	virtual void Close() = 0;
    
protected:	
	TUid	iDestructorIDKey;
	};

#include "ctsysystemstatepluginservice.inl"
	
#endif
