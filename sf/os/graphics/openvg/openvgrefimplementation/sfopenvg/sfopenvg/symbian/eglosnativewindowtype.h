/*
* Copyright (c) 2009 Symbian Foundation Ltd
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Symbian Foundation Ltd - initial contribution.
* 
* Contributors:
*
* Description:
* Declaration of native window type structure
*/

#include <e32std.h>
#include <w32std.h>

#ifndef EGLOSNATVIEWINDOWTYPE_H
#define EGLOSNATVIEWINDOWTYPE_H

struct TNativeWindowType
	{
public:
	TNativeWindowType() :
	iSize(0,0),
	iBitmap(NULL)
	{};
	TSize				iSize; //to keep
	CFbsBitmap* 		iBitmap; //to keep
	};

#endif //EGLOSCALLBACK_H
