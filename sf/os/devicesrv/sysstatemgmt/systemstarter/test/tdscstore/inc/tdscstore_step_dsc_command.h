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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code  
*/

#if (!defined __TDSCSTORE_STEP_DSC_COMMANDS_H__)
#define __TDSCSTORE_STEP_DSC_COMMANDS_H__

#include <dscstore.h>
#include <dscitem.h>
#include "dscdatabase.h"

#include "tdscstore_step_base.h"

/**
Folder to use as a flag to avoid (if required) the connection to ddbb in the doTestStepPreambleL
NOTE: 	This folder name has to match the one used in the script files when we want
		to skip the conntection
*/
_LIT(KTestAvoidConnectionInPreamble, "c:\\tdscstore_avoid_connection_in_preamble_token\\");


/**
Literal used as names for test case.
*/
_LIT(KCmdOpenDsc,"OpenDscDatabase");
_LIT(KCmdOpenCloseDsc,"OpenCloseDscDatabase");
_LIT(KCmdConnectedDsc ,"ConnectedDsc");
_LIT(KCmdCreateDsc,"CreateDsc");
_LIT(KCmdDeleteDsc,"DeleteDsc");
_LIT(KCmdDscExist,"DscExist");
_LIT(KCmdCreateDscItem,"CreateDscItem");
_LIT(KCmdUpdateDscItem,"UpdateDscItem");
_LIT(KCmdDeleteDscItem,"DeleteDscItem");
_LIT(KCmdCompareDscItems,"CompareDscItems");
_LIT(KCmdDscItemExist,"DscItemExist");
_LIT(KCmdReadDscItem,"ReadDscItem");
_LIT(KCmdEnumOpenDsc,"EnumOpenDsc");
_LIT(KCmdEnumDscItems,"EnumDscItems");
_LIT(KCmdEnumReadNextDscItem,"EnumReadNextDscItem");
_LIT(KCmdDscItemCount,"DscItemCount");
_LIT(KCmdGetDscDescriptions,"GetDscDescriptions");
_LIT( KCmdCheckMaxStringLength, "CheckMaxQueryStringLength" );

/**
CAppfwkDscStoreTestStepDscCommand encapsulates the functionality required 
to operate on DscStore (creating/modifying/deletion/enumerate dsc and dscitems).
*/
class CAppfwkDscStoreTestStepDscCommand : public CAppfwkDscStoreTestStepBase
	{
public:	
	CAppfwkDscStoreTestStepDscCommand(const TDesC& aStepName);
	~CAppfwkDscStoreTestStepDscCommand();
	
	//from CTestStep
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();	
	void SetHeapFailNext(TInt aFailureIndex);		
	TBool CheckForHeapFailNext();
	
private:
	void OpenDsc();
	void CloseDsc();
	void ConnectedDsc();
	void CreateDsc();
	void DeleteDsc();
	TBool DscExist();
	void CreateDscItem();
	void DeleteDscItem();
	void UpdateDscItem();
	TBool CompareDscItemsL();
	void DscItemCount();
	TBool DscItemExists();
	CDscItem* ReadDscItem();
	TInt EnumerateItemsL();	
	TInt EnumerateReadNextL();
	CDscItem* ConstructDscItemL(const TDesC& aParamKeyName);
	void PrintDscItemValues(const CDscItem& aDscItem);	
	void EnumOpenDscL();
	void GetDscDescriptionsL();
	void CheckMaxStringLength();
	
private:
	RDscStore	iDscClient;
	TUid		iDscUid;
	TInt		iErrVal;
	TBool		iOOMSwitch;
	TBool		iOOMLoopDone;
	TBuf<3>		iIniAccessLogValue;	
	};

#endif


