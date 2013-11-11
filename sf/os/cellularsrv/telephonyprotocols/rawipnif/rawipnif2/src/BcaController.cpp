// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the interface to Bca & flow control.
// 
//

/**
 @file
*/


#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "BcaControllerTraces.h"
#endif

#include <e32uid.h>
#include <nifmbuf.h>

#include "Constants.h"
#include "BcaController.h"

CBcaController::CBcaController(CRawIP2Flow& aRawIPFlow)
/**
 * Constructor. Performs standard active object initialisation.
 *
 * @param aRawIPFlow Reference to the RawIp2Flow
 *
 */
	: CActive(EPriorityStandard), 	  
	  iMBca(NULL),	  
	  iState(EIdling),
	  iRawIPFlow(aRawIPFlow),
	  iUpperControl(NULL),
	  iUpperDataReceiver(NULL),	 
	  iBCAProvisionConfig(NULL),
	  iBcaParams(NULL)
	{
	CActiveScheduler::Add(this);
	}
	 
CBcaController* CBcaController::NewL(CRawIP2Flow& aRawIPFlow)
/**
 * Two-phase constructor. Creates a new CBcaController object, performs 
 * second-phase construction, then returns it.
 *
 * @param aRawIPFlow Reference to the RawIp2Flow
 * @return A newly constructed CBcaController object
 */
	{
	CBcaController* self = new (ELeave) CBcaController(aRawIPFlow);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CBcaController::ConstructL()
/**
 * Second-phase constructor. Creates all the state objects it owns.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_CONSTRUCTL_1, "CBcaController::ConstructL");
	
	}
  
 
CBcaController::~CBcaController()
/**
 * Destructor.
 */
	{
	Cancel();
	if(iMBca)
		{
		iMBca->Release();	
		}
	delete iBcaParams;		
	// Library will be Closed when iBcaDll is destroyed.
	}

void CBcaController::RunL()
/**
 *  AO async callback method. Called after request is completed. 
 *  
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RUNL_1, "CBcaControl::RunL() called");
	switch (iState)
		{
		//in this state, Ioctl is called to set IAP ID, check the result of
		// Ioctl, then either set the BCA stack with another Ioctl call, 
		// open the BCA (if there's no BCA stack to set), or stop the NIF.
		case EIdling:
			{
			if(iStatus == KErrNone || iStatus == KErrNotSupported)
				{
				if(iStatus == KErrNotSupported)
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RUNL_2, "This BCA does not support IAPID set");
					}
				else
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RUNL_3, "This BCA supports IAPID set");
					}
				
				TPtrC bcaStack = iBCAProvisionConfig->GetBCAStack();
				if(bcaStack.Length())
					{
					TBuf8<KMaxName> remainingBcaStack8;
					remainingBcaStack8.Copy(bcaStack);
					iMBca->Control(iStatus, KBcaOptLevelGeneric,KBCASetBcaStack,remainingBcaStack8);
					}
				else
					{
					TRequestStatus* statusPtr=&iStatus;
					User::RequestComplete(statusPtr,KErrNone);
					}
				iState = EIAPSet;
				SetActive();	
				}
			else
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RUNL_4, "ERROR in BCA IAPID set = %d", iStatus.Int());
				Stop(iStatus.Int());
				}
			
			break;
			}			
		//in this case, we receive the result of Ioctl call to set Bca Stack.
		// Check the result of Ioctl, then Open and start the Bca or stop the NIF
		case EIAPSet:
			{
			if(iStatus == KErrNotSupported || iStatus == KErrNone)
				{
				if(iStatus == KErrNotSupported)
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RUNL_5, "This BCA does not support BCA stacking");
					}
				else
					{
					OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RUNL_6, "This BCA supports BCA stacking");
					}
				iBcaParams = new(ELeave) MBca2::TBcaParams(const_cast<CBCAProvision*>(iBCAProvisionConfig)->GetCommsPond(), iBCAProvisionConfig->GetPortName());
			
				TInt aErr = iMBca->Open(*iUpperControl,*iUpperDataReceiver,*iBcaParams);
				if ( aErr != KErrNone)
					{					
					OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RUNL_7, "ERROR in BCA Open = %d", aErr);
					Stop(iStatus.Int());
					iState = EIdling;
					}
				else
					{
					iMBca->Start();					
					}
				}
			else
				{
				OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RUNL_8, "ERROR in BCA stack set = %d", iStatus.Int());
				Stop(iStatus.Int());
				}
			break;
			}
		// Wrong state.
		default:
			{
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RUNL_9, "ERROR CBcaControl::RunL(): Unknown state");
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_RUNL_10, "PANIC: %S %d", KNifName, KBcaUnkownState);
			User::Panic(KNifName, KBcaUnkownState);
			break;
			}
		}

	}
	
void CBcaController::DoCancel()
/**
 *	cancel active request. 
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_DOCANCEL_1, "CBcaControl::DoCancel called.");
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_DOCANCEL_2, "iState value is %d", iState);
	switch (iState)
		{
		case EIdling:
		case EIAPSet:
			if(iMBca)
				{
				iMBca->CancelControl();
				}
			iState = EIdling;
			break;
		default:
			OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_DOCANCEL_3, "ERROR CBcaControl::DoCancel(): Unknown state");
			OstTraceDefExt2(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_DOCANCEL_4, "PANIC: %S %d", KNifName, KBcaUnkownState);
			User::Panic(KNifName, KBcaUnkownState);
			break;
		}
	}
	
void CBcaController::StartLoadL(const CBCAProvision* aBCAProvisionConfig,MUpperControl* aControl, 
		MUpperDataReceiver* aData)
/**
 *  This method loads the C32BCA library and uses Ioctl to set the Bca iIapId.
 *  
 * @param aBCAProvisionConfig pointer to CBCAProvision for extracting the pond
 * @param aControl pointer to BasebandChannelAdaptation2::MUpperControl passed to the MBca2 to pass control information up the stack
 * * @param aData pointer to BasebandChannelAdaptation2::MUpperDataReceiver passed to the MBca2 to push packets up the stack
 * @return none 
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_STARTLOADL_1, "CBcaControl::StartLoad");

	iBCAProvisionConfig = aBCAProvisionConfig;
	iUpperControl = aControl;
	iUpperDataReceiver = aData;
	// Loads Bca Dll and creates a Bca instance;
	User::LeaveIfError(iBcaDll.iObj.Load(iBCAProvisionConfig->GetBCAName()));
	
	TNewBca2FactoryL newBca2FactoryProcL = (TNewBca2FactoryL)iBcaDll.iObj.Lookup(1);
	if (NULL == newBca2FactoryProcL)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_STARTLOADL_2, "Library entry point found error %d", KErrBadLibraryEntryPoint);
		User::Leave(KErrBadLibraryEntryPoint);	
		}
	
	MBca2Factory* bcaFactory = (*newBca2FactoryProcL)();

	if(!bcaFactory)
		{
		OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_STARTLOADL_3, "BcaFactory creation error %d", KErrCompletion);
		User::Leave(KErrCompletion);	
		}
	CleanupReleasePushL(*bcaFactory);
	
	iMBca = bcaFactory->NewBcaL();
	CleanupStack::PopAndDestroy(bcaFactory);
	
	TPckg<TUint32> aOpt(iBCAProvisionConfig->GetIAPid());
	iMBca->Control(iStatus,KBcaOptLevelGeneric,KBCASetIapId,aOpt);
	
	iState = EIdling;
	SetActive();
	}

void CBcaController::Stop(TInt aError)
/**
 *  Used to shutdown this module. This will cancel all the outstanding 
 *  requests on the active objects owned by this module and shutdown.
 */
	{
	OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_STOP_1, "CBcaController::Stop is called.");

	if(iMBca)
		{
		iMBca->Close();
		}
	else 
		{
		OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_STOP_2, "CBcaController::Stop Bca is not initialized. Bring the link layer down");
		iRawIPFlow.LinkLayerDown(aError);
		}
  	}


/** Panic function for RawIpNif 

* @param aPanic panic code */
void Panic(TRawIP2NifPanic aPanic)
	{
	OstTraceDef1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CBCACONTROLLER_PANIC_1, "Panic code for RawIpNif = %d", aPanic);
	User::Panic(KNifName,aPanic);

	}

