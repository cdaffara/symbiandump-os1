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
// @file 
// @test
// @internalComponent - Internal Symbian test code
// 
//

#include "tss_step_base.h"

/**
 Find and kill all intances of the specified process name
*/
TInt CSsStepBase::FindAndKill( const TDesC& aProcessName )
	{

	TFullName searchTerm( aProcessName );
	StripExtension( searchTerm );
	searchTerm += _L("*");
	
	TFindProcess find( searchTerm );
	TFullName name;
	TInt instancesFound = 0;
	
	while( find.Next(name) == KErrNone )
		{
		
		RProcess process;
		
		const TInt err = process.Open( find );
		
		if (  KErrNone == err )
			{
			
			if ( process.ExitType() == EExitPending )
				{
				instancesFound++;
				
				process.Kill( KErrCancel );
				process.Close(); 
				
				INFO_PRINTF2( _L("Process %S found and killed"), &aProcessName );
				}
				
			process.Close(); 
			}
			
		}
		
	return instancesFound;
	}



void CSsStepBase::StripExtension( TDes& aFilename )
	{
	TInt dot = aFilename.Find( _L(".") );
	
	if( KErrNotFound != dot)
		{
		aFilename.SetLength( dot );
		}
	
	}



