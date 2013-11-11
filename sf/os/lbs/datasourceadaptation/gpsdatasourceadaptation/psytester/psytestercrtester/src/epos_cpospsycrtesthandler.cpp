// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <centralrepository.h> 
#include <ecom/ecom.h>

#include <lbs/positioningpluginfwinternalcrkeys.h>

#include "epos_cpospsycrtesthandler.h"
#include "epos_cpospsycrlog.h"


// Constants
_LIT(KPermission, " does not have proper capabilities in the Central Repository");

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosPSYCRTestHandler::CPosPSYCRTestHandler()
	{
    }

// EPOC default constructor can leave.
void CPosPSYCRTestHandler::ConstructL()
    {
    	iLog = CPosPSYCRLog::NewL();
	}

// Two-phased constructor.
CPosPSYCRTestHandler* CPosPSYCRTestHandler::NewL()
    {
	    CPosPSYCRTestHandler* self = new (ELeave) CPosPSYCRTestHandler;
	    CleanupStack::PushL(self);
	    self->ConstructL();
	    CleanupStack::Pop(self);
	    return self;
    }

// Destructor
CPosPSYCRTestHandler::~CPosPSYCRTestHandler()
    {
    	delete iLog;
    }

// ---------------------------------------------------------
// CPosPSYTestHandler::StartPSYCRTesterL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPSYCRTestHandler::StartPSYCRTesterL(
    TUid aUid)
    {

        iLog->OpenLogL(aUid);
        TestCRValueCapabilitiesL(aUid);
        iLog->CloseLog();
     } 


// ---------------------------------------------------------
// CPosPSYTestHandler::ReadCRValuesL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//

void CPosPSYCRTestHandler::TestCRValueCapabilitiesL(
	TUid aUid)
	{
		TInt errors = 0;
		
		TInt ret = -1;
		
	    // Open the PSY CR
    	CRepository *centralRep = NULL;
    	TRAPD(error, centralRep = CRepository::NewL(aUid));
    	
    	if(!error && centralRep)
    	{
    		// Try reading each and every value in the CR
    		// If we are unable to read any of the vaues,
    		// We should log an error message in the log file
    	    		    		
    		//+ Interface Version
    		TInt interfaceVersion;
    		
    		ret = centralRep->Get(KPSYInterfaceVersion, interfaceVersion);
    		
    		if(KErrPermissionDenied == ret)
    		{
    			_LIT(KInterfaceVersion, "Interface Version");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KInterfaceVersion);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}
    		
    		//+ Priority
    		TInt priority;
    		
    		ret = centralRep->Get(KPSYPriority, priority);
    		
    		if(KErrPermissionDenied == ret)
    		{
    			_LIT(KPriority, "Module Priority");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KPriority);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}
    		
    		//+ Availability Status
    		TInt status;
    		ret = centralRep->Get(KPSYStatus, status);
    		
    		if(KErrPermissionDenied == ret)
    		{
    			_LIT(KStatus, "Status (Availability)");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KStatus);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}
    		
			//+ TimeToFirstFix
			TReal timeToFirstFix;
			ret = centralRep->Get(KPSYTimeToFirstFix, timeToFirstFix);
			
    		if(KErrPermissionDenied == ret)
    		{
    			_LIT(KTimetoFirstFix, "Time to First Fix");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KTimetoFirstFix);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}
			
			
			//+ TimeToNextFix
			TReal timeToNextFix;
			ret = centralRep->Get(KPSYTimeToNextFix, timeToNextFix);
			
			if(KErrPermissionDenied == ret)
    		{
    			_LIT(KTimetoFix, "Time to Fix");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KTimetoFix);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}
				
		    
		    //+ Horizontal Accuray
			TReal64 horizontalAccuracy;
			ret = centralRep->Get(KPSYHorizontalAccuracy, horizontalAccuracy);
			
			if(KErrPermissionDenied == ret)
    		{
    			_LIT(KHorizAccuracy, "Horizontal Accuracy");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KHorizAccuracy);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}
				
			//+ Vertical Accuracy
			TReal64 verticalAccuracy;
			ret = centralRep->Get(KPSYVerticalAccuracy, verticalAccuracy);
			
			if(KErrPermissionDenied == ret)
    		{
    			_LIT(KVerticalAccuracy, "Vertical Accuracy");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KVerticalAccuracy);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}
			//+ Cost Indicator
			TInt costIndicator;
			ret = centralRep->Get(KPSYCostIndicator, costIndicator);
			
			if(KErrPermissionDenied == ret)
    		{
    			_LIT(KCostIndicator, "Cost Indicator");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KCostIndicator);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}

			//+ Power Consumption
			TInt powerConsumption;
			ret = centralRep->Get(KPSYPowerConsumption, powerConsumption);
			
			if(KErrPermissionDenied == ret)
    		{
    			_LIT(KPowerConsumption, "Power Consumption");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KPowerConsumption);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}
    		
			//+ Technology Type
			TInt technologyType;
			ret = centralRep->Get(KPSYTechnologyType, technologyType);
			
			if(KErrPermissionDenied == ret)
    		{
    			_LIT(KTechnologyType, "Technology Type");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KTechnologyType);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}

			//+ Device Location
			TInt deviceLocation;
			ret = centralRep->Get(KPSYDeviceLocation, deviceLocation);
			
			if(KErrPermissionDenied == ret)
    		{
    			_LIT(KDeviceLocation, "Device Location");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KDeviceLocation);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}
    		
			//+ Capabilities
			TInt capabilities;
			ret = centralRep->Get(KPSYCapabilities, capabilities);
			
			if(KErrPermissionDenied == ret)
    		{
    			_LIT(KCapabilities, "Capabilities");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KCapabilities);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}

			//+ Classes Supported
			TInt classesSupported;
			ret = centralRep->Get(KPSYClassesSupported, classesSupported);
			
			if(KErrPermissionDenied == ret)
    		{
    			_LIT(KClassesSupported, "Classes Supported");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KClassesSupported);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}

			//+ Visibility
			TInt visibility;
			ret = centralRep->Get(KPSYVisibility, visibility);
			
			if(KErrPermissionDenied == ret)
    		{
    			_LIT(KVisibility, "Visibility");
    			errors ++;
    			TBuf <150> msg;
    			msg.Append(KVisibility);
    			msg.Append(KPermission);
    			iLog->AppendErrorMsgL(msg);
    		}

				
    	}
    	
    	if(0 == errors)
    	{
    		TBuf <150> msg;
    		_LIT(KInfoMsg, "All the PSY Central Repository values are protected with proper capabilities");
    		msg.Append(KInfoMsg);
    		iLog->AppendInfoMsgL(msg);
    	}
		//+ delete centralRep
	    delete centralRep; 
	}

//  End of File
