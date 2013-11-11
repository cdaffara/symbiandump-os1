// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef _TE_SMSPRTBASE_H_
#define _TE_SMSPRTBASE_H_

#include <commsdattypesv1_1.h>
#include <e32base.h>
#include <es_sock.h>
#include <f32file.h>
#include <e32test.h>
#include <c32comm.h>
#include <simtsy.h>
#include <gsmumsg.h>
#include <gsmubuf.h>
#include <gsmuset.h>
#include <smsustrm.h>
#include <es_wsms.h>
#include <testconfigfileparser.h>
#include <test/testexecuteserverbase.h>

#include "smsstacktestcase.h"
#include "TE_smsprt_alphabet.h"
#include "smsstacktestutilities.h"
#include "smsstackbaseteststeps.h"
#include "smsstacktestconsts.h"
#include "smspdudb.h"
#include "smspproc.h"
#include "attributes.h"

using namespace CommsDat; 

/**
 *  Test Step base case for all encoding tests. 
 */
class CSmsPrtEncodingTestStep : public CSmsBaseTestStep
	{
public:
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	void GetExpectedPDUCountAndUDUnitsFreeL(TInt aOctetsCount, 
			                                TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
			                                TSmsEncoding aEncodingExpected,
				                            TInt& aExpectedPDUs,
				                            TInt& aExpectedFreeUDUnitsInLastPDU) const;
	void GetExpectedEncodingInfoL(TDesC& aDes,
								  TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
				                  TInt& aExpectedUnconvertibleChars,
								  TInt& aExpectedFirstUnconvertibleChar) const;
	void GetExpectedEncodingInfoL(TDesC& aDes,
								  TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
								  TSmsEncoding aEncoding,
				                  TInt& aExpectedUnconvertibleChars,
				                  TInt& aExpectedDowngradedChars,
				                  TInt& aExpectedAlternativeEncodingChars,
								  TInt& aExpectedFirstUnconvertibleChar) const;
	
	void DoEncodingTestL(RSocket aSocket,
						 const TDesC& aTestName,
		                 TInt aTestSequences,
						 RArray<TPtrC>& aTestCharSetNames,
						 TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
						 TSmsEncoding aEncodingToUse,
						 TSmsEncoding aEncodingExpected);

	void DoEncodingTestL(RSocket aSocket,
	                     const TDesC& aTestName,
	                     TInt aTestCharacters,
	                     RArray<TPtrC>& aTestCharSetNames,
	                     TInt aAdditionalTestCharacters,
	                     RArray<TPtrC>& aAdditionalTestCharSetNames,
	                     TSmsDataCodingScheme::TSmsAlphabet aAlphabet,
	                     TSmsEncoding aEncodingToUse,
	                     TSmsEncoding aEncodingExpected);
	
	TInt PerformCharacterTestsL(CSmsMessage* aSmsMessage, 
	        TDesC16& aOriginalPtr,
	        TSmsDataCodingScheme::TSmsAlphabet& aAlphabet,
            TSmsEncoding aEncodingToUse,
            TSmsEncoding aEncodingExpected,
            TInt aOctetsCount,
            TBool aExtendedCharAdded
            );
	
protected:
	CSmsPrtTestAlphabet* iTestAlphabet;
    RArray<TPtrC> iCharSets;
    RArray<TPtrC> iAdditionalCharSets;
};

#endif // _TE_SMSPRTBASE_H_
