/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef __HWRMLightTestF_APPUI_H__
#define __HWRMLightTestF_APPUI_H__

#include <eikappui.h>
#include "HWRMLightTestF.hrh"
#include "tc_log.h"
#include "testcasecontroller.h"


class CHWRMLightTestFAppView;

/*! 
  @class CHWRMLightTestFAppUi
  
  @discussion An instance of class CHWRMLightTestFAppUi is the UserInterface part of the AVKON
  application framework for the HWRMLightTestF example application
  * * Nokia Core OS *
  */
class CHWRMLightTestFAppUi : public CEikAppUi, public MHwrmTestCaseObserver
    {
public:
    ~CHWRMLightTestFAppUi();
	CHWRMLightTestFAppUi();
	
private:
    void ConstructL();
     
private: // from CAknAppUi
  	void HandleCommandL(TInt aCommand);

private: // from MHwrmTestCaseObserver
	void InfoMessage(const TDesC& aMessage);
	void LogMessage(const TDesC& aMessage);
	void DebugMessage(const TDesC& aMessage);
	
private:
	CHwrmTestCaseController* iTestCaseController;	
    CHWRMLightTestFAppView* iAppView;
    Log iLog;
    };


#endif // __HWRMLightTestF_APPUI_H__

