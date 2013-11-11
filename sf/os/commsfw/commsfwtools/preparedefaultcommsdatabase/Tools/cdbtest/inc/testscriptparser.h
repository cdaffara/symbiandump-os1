// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file testscriptparser.h
 @internalComponent
*/


#ifndef TESTSCRIPTPARSER_H
#define TESTSCRIPTPARSER_H

#include <f32file.h>

class CTestLog;

/**
  @internalComponent
*/
class CTestScriptParser : public CBase
    {
    public:
    	
    	enum TScriptAction
    	    {
    	    ENoAction,
    	    ERunProgram,
    	    ERunTestStep,
    	    EPrint,
    	    ELoadTestSuite
    	    };
    	
      virtual ~CTestScriptParser() {}

      static CTestScriptParser* FactoryLC(TDesC& aFile, CTestLog* aLog);

      void  OpenScriptFileL();
      TBool GetNextTestStepL(TScriptAction &runAction, TDes &params);
  
    protected:
    	CTestScriptParser(TDesC& aFile, CTestLog* aLog);
      void Exit(TInt aErr);
        
      TDesC&    iFile;
      CTestLog* iLog;
      RFile     iCfg;
    	TPtr      iPtr;
    	TLex	    iLex;
    };

   

#endif
