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

#ifndef CTSYSYSTEMSTATEPLUGIN_H
#define CTSYSYSTEMSTATEPLUGIN_H

#include <e32base.h>
#include "ctsysystemstatepluginhandler.h"


#include <ssm/ssmadaptationcli.h>



class CChangeRfForEmergencyCall;

/*		
*/
class CCtsySystemStatePlugin : public CCtsySystemStatePluginService
	{	
friend class CChangeRfForEmergencyCall;

public:
	static CCtsySystemStatePlugin* NewL();	
	~CCtsySystemStatePlugin();	

protected: //From CCtsySystemStatePluginService
	TInt Connect();
	void ActivateRfForEmergencyCall(MCtsySsmPluginCallback* aSsmPluginCallback, TCtsySsmCallbackData& aCallbackData);
	void DeactivateRfAfterEmergencyCall();
	void Close();

protected:	
	CCtsySystemStatePlugin();
	void ConstructL();
	
	// Active object for do RF state changing 
	CChangeRfForEmergencyCall* iAoChangeRfForEmergencyCall;	
	RSsmEmergencyCallRfAdaptation iSsmEmergencyCallRf;

	TBool iConnected;

	};
	
	/*
	Active object for changing Rf State
	*/
	class CChangeRfForEmergencyCall : public CActive
		{
	public:			
		enum TBusyType
        {
        ENotBusy,
        EActivateBusy,
        EDeactivateBusy
        };
		 	
	public:
		CChangeRfForEmergencyCall(CCtsySystemStatePlugin& iCtsySystemStatePlugin,
								  RSsmEmergencyCallRfAdaptation& aSsmEmergencyCallRf);
		~CChangeRfForEmergencyCall();

		void ActivateRfForEmergencyCall(MCtsySsmPluginCallback* aSsmPluginCallback, TCtsySsmCallbackData& aCallbackData);
		void DeactivateRfAfterEmergencyCall();
		
		// indicate if there is any request to CTSY
		TBool Busy();
		
		// from CActive
	private:	
		void RunL();
		void DoCancel();
	
	private: // data
		// pointer to interface to call in RunL
		MCtsySsmPluginCallback* iSsmPluginCallback;	 
	
		// As RSsmEmergencyCallRfAdaptation allows
		//only one request is allowed to be outstanding for each connected session.
		// we have to keep it in mind.
		TBusyType iBusy; 
	
		CCtsySystemStatePlugin&			iCtsySystemStatePlugin;
		RSsmEmergencyCallRfAdaptation&	iSsmEmergencyCallRf;		
		
		// data to pass into callback
		TCtsySsmCallbackData iCallbackData;
		};


	
	
#endif
