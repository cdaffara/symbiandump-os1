// license.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __LICENSE_H__
#define __LICENSE_H__

class TLicense
	{
public:
	enum TLicenseStatus
		{
		EValid,
		EInvalid
		};
public:
	static TLicenseStatus CheckL(RIoReadHandle& aReadHandle, RIoWriteHandle& aWriteHandle);
	};

#endif // __LICENSE_H__
