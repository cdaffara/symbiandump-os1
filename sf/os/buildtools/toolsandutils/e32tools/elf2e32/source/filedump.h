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
// FileDump Class for elf2e32 tool
// @internalComponent
// @released
// 
//

#ifndef __FILEDUMP_H__
#define __FILEDUMP_H__

#include "usecasebase.h"

/**
class for dumping e32image file or dumping asm file
@internalComponent
@released
*/
class FileDump : public UseCaseBase
{

	public:
		FileDump(ParameterListInterface* aParameterListInterface);
		~FileDump();
		int Execute();
	private:
		int DumpE32Image(const char * fileName);
		int GenerateAsmFile(const char* afileName);//DumpAsm
};

#endif

