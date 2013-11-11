// string_utils.cpp
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

#include "string_utils.h"

#define CASE_RETURN_LIT(XXX) case XXX: { _LIT(_KLit, #XXX); return &_KLit; }
#define CASE_RETURN_LIT2(XXX, YYY) case XXX: { _LIT(_KLit, YYY); return &_KLit; }
#define DEFAULT_RETURN_LIT(XXX) default: { _LIT(_KLit, XXX); return &_KLit; }

const TDesC* ShStringify::JobStatus(CJob::TStatus aStatus)
	{
	switch (aStatus)
		{
		CASE_RETURN_LIT2(CJob::EPending, "Pending");
		CASE_RETURN_LIT2(CJob::ERunning, "Running");
		CASE_RETURN_LIT2(CJob::EStopped, "Stopped");
		CASE_RETURN_LIT2(CJob::EComplete, "Complete");
		DEFAULT_RETURN_LIT("*** STATUS UNKNOWN ***");
		}
	}
