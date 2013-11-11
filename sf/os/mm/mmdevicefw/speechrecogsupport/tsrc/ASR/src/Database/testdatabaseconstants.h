// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// sddatabase.h
// 
//

#ifndef __TESTDATABASECONSTANTS_H__
#define __TESTDATABASECONSTANTS_H__

#include "Database/sddatabase.h"

_LIT(KFileNameTestDatabase, "C:\\data\\speechrecognition.db");

// some constants to create a dummy database to test
const TInt KNumParameters = 2;

const TInt KNumGrammars = 2;
const TInt KNumLexicons = 2;
const TInt KNumModelBanks = 2;

const TInt KNumRules = 3;
const TInt KNumPronunciations = 3;
const TInt KNumModels = 3;


const TGrammarID KGrammarIDs[KNumGrammars] = {1 ,2 };
const TLexiconID KLexiconIDs[KNumLexicons] = {3 ,4 };
const TModelBankID KModelBankIDs[KNumModelBanks] = {5 ,6 };

// Test rules for the dummy database
const CSDDatabase::TRuleData KRuleIDs[KNumGrammars][KNumRules] = {{{7,3,13},{8,3,15},{9,4,18}},
													  {{10,4,17},{11,4,16},{12,3,15}}};
									
// Test pronunciations for the dummy database				  
const CSDDatabase::TPronunciationData KPronunciationIDs[KNumLexicons][KNumPronunciations] = {{{13,5,19},{14,5,20},{15,6,22}},
																			  {{16,6,23},{17,5,20},{18,5,19}}};

// Test Models for the dummy database				  
const CSDDatabase::TModelData KModelIDs[KNumModelBanks][KNumModels] = {{{19, 2000},{20,3000}, {21, 5000}},
	{{22, 3000},{23, 4000},{24,7000}}};


#define KMaxAvailableResults 2
#define KNumParameters 2

// intially stored parameters for the dummy database
const TInt KParameterIDs[KNumParameters] = {1,2};
const TInt KParameterValues[KNumParameters] = {101,222};

const TGrammarID KResultsGrammarID[KMaxAvailableResults] = {2,1};
const TRuleID KResultsRuleID[KMaxAvailableResults] = {11,9};

const TUid KUidTestASRClient = {0x101};
const TUid KUidTestASRClient2 = {0x102};

const TInt KIndex0 = 0;
const TInt KIndex1 = 1;
const TInt KIndex2 = 2;

// IDs that do not exist in the database defined above
const TLexiconID KBadLexiconID = 100000;
const TGrammarID KBadGrammarID = 100001;
const TModelID KBadModelID = 100002;
const TRuleID KBadRuleID = 100003;
const TModelBankID KBadModelBankID = 100004; 
const TPronunciationID KBadPronunciationID = 100002;

const TInt KBadMode = 1000; // an invalid enum value for TRecognitionMode

const TInt KNumberOfModelsInFreeStorage = 0;
const TInt KUtteranceDuration = 100;


#endif
