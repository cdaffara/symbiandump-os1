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
 @file
 @test
 @internalComponent
*/
  

#include "utils.h"
#include "panic.h"

/*************************************************************************************
 class TRnd
 */

/**
Math::Rand() degenerates to approx seed*3.3+38 for the first numbers generated 
with a low seed, which isn't very random. To illustrate: The random numbers generated 
by the seeds {0..9} are {38, 41, 45, 48, 51, 54, 58, 61, 64, 68}.
*/
const TInt64 KMinSeed = 1000000000;

TInt64 TRnd::iSeed = KMinSeed; // Need to change CComparison::StartL() to use this value

CExecutionContext::CLogMediator* TRnd::iMediator 			= NULL;
CExecutionContext* 				 TRnd::iExecutionContext	= NULL;

TInt64 TRnd::Seed()
	{
	return iSeed;
	}

void TRnd::SetSeed(TInt64 aSeed)
	{
	iSeed = aSeed;
	}

void TRnd::SetLogMediator(CExecutionContext::CLogMediator* aMediator)
	{
	iMediator = aMediator;
	}

void TRnd::SetExecutionContext(CExecutionContext* aExecutionContext)
	{
	iExecutionContext = aExecutionContext;
	}


TInt TRnd::GenRand(TInt x)
	{
	TInt res = Math::Rand( iSeed );
	if (x)
		{
		res = res % x;
		}
	return res;
	}

TInt TRnd::rnd(TInt x)
	{
	TInt res;
	
	switch ( iExecutionContext->ContextMode() )
	{
	case CExecutionContext::ECtxPlayback:	
		res = iMediator->PlaybackInt();
		break;
		
	case CExecutionContext::ECtxRandomAndRecord:
		res = GenRand(x);
		iMediator->RecordInt( res );
		break;
		
	case CExecutionContext::ECtxRandom:		// fallthrough
	default:
		res = GenRand(x);
		break;
	}
	
	return res; 
	}

/*************************************************************************************
 class CExecutionContext
 */
TInt CExecutionContext::CLogMediator::PlaybackInt()
	{
	
	TInt res = 0;
	
	if ( !iStalled )
		{
	
		TBool failed = ETrue;
		
		TBuf8<1024> logRec;
	
		TInt readRes;
		
		readRes = iLog.Read( logRec, 3 ); // 'Num'
	
		
		
		if ( readRes == KErrNone && logRec == _L8("Num") )
			{
			iLog.Read( logRec, 16 ); // EntryNo, 16 characters	
			TLex8 strLex;		
			strLex.Assign( logRec );		
			TInt64 readVal64 = 0;		
			readRes = strLex.Val( readVal64, EDecimal );		
			if ( KErrNone == readRes && readVal64 == iEntryNo )
				{
							
				readRes = iLog.Read( logRec, 3 ); // ' = '
				
				if ( readRes == KErrNone && logRec == _L8(" = ")  )
					{
	
					readRes = iLog.Read( logRec, 13 ); // value 12 + eol
					strLex.Assign( logRec );		
					
					TUint32 readVal32;
	
					readRes = strLex.Val( readVal32, EDecimal );		
					
					if ( KErrNone == readRes )
						{  // finally we got a number
						
						res = (TInt) readVal32;
						
						failed = EFalse;
						}
					
					}							
				}
			
			}
		
		if ( failed )
			{
			iExecutionContext.MediatorEmptied( this );
			res = 0;
			iStalled = ETrue;
			}
		
		}
	
	iEntryNo++;
	
	return res;
	}

void CExecutionContext::CLogMediator::RecordInt(TInt aIntToBeRecorded)
	{
	if ( !iStalled )
		{
		TBuf8<1024> logRec;
		logRec.Format( _L8("Num%016Ld = %012d\n"), iEntryNo, aIntToBeRecorded );
		iLog.Write( logRec );	
		}
	
	iEntryNo++;
	}

CExecutionContext::CLogMediator::CLogMediator(CExecutionContext&	aExecutionContext):
	iExecutionContext( aExecutionContext )
	{ }

CExecutionContext::CLogMediator::~CLogMediator()
	{
	iLog.Close();
	iExecutionContext.MediatorDestroyed ( this );
	}

void CExecutionContext::CLogMediator::ConstructL( RFs& aFs, const TDesC& aFileName )
	{
	TInt fileErr;
	
	switch ( iExecutionContext.ContextMode() )
	{
	case CExecutionContext::ECtxPlayback:
		fileErr = iLog.Open( aFs, aFileName, EFileRead );
		break;
	
	case CExecutionContext::ECtxRandomAndRecord:
		fileErr = iLog.Create( aFs, aFileName, EFileWrite );
		if(fileErr == KErrAlreadyExists)
			{
			fileErr = iLog.Replace( aFs, aFileName, EFileWrite );		
			}
		else if(fileErr == KErrPathNotFound)
			{
			User::LeaveIfError(aFs.CreatePrivatePath(EDriveC));
			fileErr = iLog.Create( aFs, aFileName, EFileWrite );
			}
		__ASSERT_ALWAYS(KErrNone == fileErr, User::Panic(_L("t_stress utils.cpp"), EPanic2));
		break;
	
	default:										
		fileErr = KErrGeneral;
		break;
	}
	
	User::LeaveIfError( fileErr );
	
	}
		
CExecutionContext::CLogMediator* CExecutionContext::CLogMediator::NewLC(CExecutionContext& aExecutionContext, RFs& aFs, const TDesC& aFileName )
	{
	CExecutionContext::CLogMediator* self = new (ELeave) CExecutionContext::CLogMediator( aExecutionContext );
	CleanupStack::PushL ( self );
	self->ConstructL( aFs, aFileName );
	return self; 
	}
		
CExecutionContext* CExecutionContext::NewL(CExecutionContext::TExecutionMode aExecutionMode, CTestExecWatchCat&	aWatchCat)
	{
	CExecutionContext* self = new (ELeave) CExecutionContext( aExecutionMode, aWatchCat );
	CleanupStack::PushL ( self );
	self->ConstructL( );
	CleanupStack::Pop( self );
	return self;
	}
	
CExecutionContext::TExecutionMode CExecutionContext::ContextMode()
	{
	return iContextMode;
	}

CExecutionContext::CLogMediator&	CExecutionContext::CreateLogMediatorL(const TDesC& aLogName)
	{
	CExecutionContext::CLogMediator* mediator = CExecutionContext::CLogMediator::NewLC( *this, iFs, aLogName );
	iMediators.AppendL( mediator );
	CleanupStack::Pop(mediator);
	return *mediator;
	}

void CExecutionContext::MediatorDestroyed(CLogMediator* aMediator)
	{
	TInt pos = iMediators.Find( aMediator );
	iMediators.Remove ( pos );
	}

void CExecutionContext::MediatorEmptied(CLogMediator* aMediator)
	{
	(void)aMediator;
	iWatchCat.ExecutionContextRunOut( this );
	}


void CExecutionContext::ConstructL()
	{
	TInt conResult = iFs.Connect();
	User::LeaveIfError( conResult );	
	}
	

CExecutionContext::CExecutionContext(CExecutionContext::TExecutionMode aExecutionMode, CTestExecWatchCat&	aWatchCat):
	iWatchCat(aWatchCat),
	iContextMode(aExecutionMode)
	{ } // empty

CExecutionContext::~CExecutionContext()
	{
	while ( iMediators.Count() > 0 )
		{
			delete iMediators[0]; // this will remove the mediator from the list
		}
	
	iMediators.Close();
	iFs.Close();
	}

/*************************************************************************************
 class CTestExecWatchCat
 */
CTestExecWatchCat* CTestExecWatchCat::NewL(CExecutionContext::TExecutionMode aExecutionMode)
	{
	CTestExecWatchCat* self = new (ELeave) CTestExecWatchCat();
	CleanupStack::PushL( self );	
	self->ConstructL( aExecutionMode );
	
	// finally hook into TRnd
	TRnd::SetExecutionContext(self->iExecutionContext);
	
	CleanupStack::Pop(self); 
	return self;
	}

void CTestExecWatchCat::ConstructL( CExecutionContext::TExecutionMode aExecutionMode )
	{
	iExecutionContext = CExecutionContext::NewL( aExecutionMode, *this );
	}


CTestExecWatchCat::CTestExecWatchCat()
	{ } // empty

CTestExecWatchCat::~CTestExecWatchCat()
	{
	delete iExecutionContext;
	}
	
void CTestExecWatchCat::ExecutionContextRunOut(CExecutionContext* /*aContext*/)
	{ }

/**
 Sets the path where the logging file will be created, then constructs the mediator
 */
void CTestExecWatchCat::SetLoggingPathL(const TDesC& aPath)
	{
	RBuf path;
	path.CleanupClosePushL();
	path.CreateL(aPath.Length() + 16);
	if (aPath.Length())
		{
		path.Copy(aPath);
		if (aPath[aPath.Length()-1] != '\\')
			{
			path.Append('\\');
			}
		}
	path.Append(KLogFileName);
	// now construct log mediator
	CExecutionContext::CLogMediator& mediator = iExecutionContext->CreateLogMediatorL( path );
	TRnd::SetLogMediator(&mediator);
	CleanupStack::PopAndDestroy(&path);
	}

/*************************************************************************************
 class TTickUtils
 */
TUint32 TTickUtils::CalcTickDelta(TUint32 tick1, TUint32 tick2)
	{
	TInt32 res;
	if ( tick1 > tick2 )
		{
		res = tick1 - tick2;
		}
	else
		{
		res = tick2 - tick1;		
		}
	return res;
	}
