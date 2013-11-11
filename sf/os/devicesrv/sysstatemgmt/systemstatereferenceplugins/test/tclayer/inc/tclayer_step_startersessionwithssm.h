// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//Check set state functionality of RStarterSession class. 
//Check reset network behaviour of RStarterSession class.
//Check rfstatusswp transition.
//Check IsRTCTimeValid of RStarterSession class.

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/


#ifndef __TCLAYER_STEP_STARTERSESSIONWITHSSM_H__
#define __TCLAYER_STEP_STARTERSESSIONWITHSSM_H__

#include <ssm/ssmstatemanager.h>
#include <ssm/starterclient.h>
#include <ssm/ssmsystemwideproperty.h>

#include <test/testexecutestepbase.h>
#include <test/datawrapper.h>

class CCLayerTestWrapperWithSsm : public CDataWrapper
	{
public:
    ~CCLayerTestWrapperWithSsm();
	static CCLayerTestWrapperWithSsm* NewL();
	virtual TBool   DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
    virtual TAny*   GetObject() { return iObject; }
    inline virtual void SetObjectL(TAny* aObject)
        {
        DestroyData();
        iObject = static_cast<TInt*> (aObject);
        }
        
    inline virtual void DisownObjectL() 
        {
        iObject = NULL;
        }

    void DestroyData()
        {
        delete iObject;
        iObject=NULL;
        }
                
    inline virtual TCleanupOperation CleanupOperation()
        {
        return CleanupOperation;
        }
    
protected:
    void ConstructL();
    
private:
    CCLayerTestWrapperWithSsm();
    
    static void CleanupOperation(TAny* aAny)
        {
        TInt* number = static_cast<TInt*>(aAny);
        delete number;
        }
    
    void DefineAndRegisterPropertyL();
   	void DoCommandProcessL(const TTEFFunction& aCommand, const TTEFSectionName& aSection);

	void doTestRFSwpTransitionNotAllowedCmdL(const TDesC& aSection);
	void doTestSetStateCmdL(const TDesC& aSection);
	void doTestRequestResetNetworkCmdL();
	void doTestIsRTCTimeValidCmdL(const TDesC& aSection);
		
private:
    RStarterSession iStarterSession;
    RSsmSystemWideProperty iSwp;
    TInt* iObject; //Owned by framework
    };

#endif // __TCLAYER_STEP_SETSTATE_H__
