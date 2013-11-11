// string_utils.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <e32std.h>
#include "job.h"

class ShStringify
	{
public:
	static const TDesC* JobStatus(CJob::TStatus aStatus);
	};


#endif // __STRING_UTILS_H__
