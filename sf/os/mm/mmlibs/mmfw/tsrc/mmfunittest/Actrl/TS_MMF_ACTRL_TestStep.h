// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//
#if (!defined __ACTRL_TESTSTEP_H__)
#define __ACTRL_TESTSTEP_H__

#include <e32std.h>
#include <e32base.h>
#include <mmf/common/mmfcontroller.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfdes.h>
#include <mmf/plugin/mmfcontrollerimplementationuids.hrh>
#include <mmf/common/mmfstandardcustomcommands.h>

class CTestSuite;
class CTestSuiteActrl;

/**
 *
 * CTestStepActrl, which is the base class for all 
 * the Actrl suite test steps, providing common functionality.
 *
 * @lib "TSU_MMF_actrl_01.lib"
 *
 */
class CTestStepActrl : public CTestStep
{
public:
	 CTestStepActrl();
	~CTestStepActrl();

	 virtual TVerdict DoTestStepPreambleL(void);
	 virtual TVerdict DoTestStepPostambleL(void);
	 void SetSuite( CTestSuiteActrl* aPtr );
	 CTestSuiteActrl* GetSuite();
protected:
	 //[ check if the event status is pending ]
	TBool IsTimeOut( TRequestStatus& aEventStatus );
	 //[ cancel received events ]
	void  CancelReceivedEvents();
	 //[ is event sought the one we received ] 
	TBool IsSoughtEvent( TMMFEvent& aExpectedEvent, TMMFEventPckg& receivedEvent );
	 // [ search for expected event ]
	 TVerdict SearchForEvent( TMMFEvent& aExpectedEvent );

	void SetReadFileNameL( const TDesC& aPathNameAndExtn );
	void SetWriteFileNameL( const TDesC& aPathNameAndExtn );
	inline TMMFFileConfig& ReadFileConfig() { return iFileConfigRead; }
	inline TMMFFileConfig& WriteFileConfig() { return iFileConfigWrite; }
	 
	void SetReadDescriptor( TPtr8* aDescriptor );
	void SetWriteDescriptor( TPtr8* aDescriptor );
	inline TMMFDescriptorConfig& ReadDescConfig() { return iDescConfigRead; }
	inline TMMFDescriptorConfig& WriteDescConfig() { return iDescConfigWrite; }
	 
protected:
	 CTestSuiteActrl*     iActrlSuite; // test suite
	 RMMFController       iController; // audio controller proxy
	 RFs                  iFs        ; // 
	 RTimer               iTimer     ; // escape timer for waiting on events  
	 TMMFPrioritySettings iPriority  ; // priority settings

private:	 
	 TMMFFileConfig			iFileConfigRead; // audio input file 
	 TMMFFileConfig			iFileConfigWrite;// audio output file 
	 TMMFDescriptorConfig	iDescConfigRead; // audio input desc 
	 TMMFDescriptorConfig	iDescConfigWrite;// audio output desc 
};

#endif /* __ACTRL_TESTSTEP_H__ */

