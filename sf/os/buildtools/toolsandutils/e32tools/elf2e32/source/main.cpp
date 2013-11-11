// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the main function for elf2e32 tool
// @internalComponent
// @released
// 
//

//
#include "elf2e32.h"

/**
This function creates an instance of Elf2E32 and calls Execute() of Elf2E32 to generate
the appropriate target.
@internalComponent
@released

@param aArgc
 The number of command line arguments passed into the program
@param aArgv
 The listing of all the arguments


@return EXIT_SUCCESS if the generation of the target is successful, else EXIT_FAILURE

*/
int main(int argc, char** argv) 
{
  Elf2E32 aElf2E32(argc, argv);
  return aElf2E32.Execute();
}

