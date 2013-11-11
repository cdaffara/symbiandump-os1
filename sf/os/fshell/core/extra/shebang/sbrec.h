// sbrec.h
// 
// Copyright (c) 2007 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __SBREC_H__
#define __SBREC_H__

#include <apmrec.h>

NONSHARABLE_CLASS(CShebangRecognizer) : public CApaDataRecognizerType
	{
public: // From CApaDataRecognizerType.
	CShebangRecognizer();
	TUint PreferredBufSize();
	TDataType SupportedDataTypeL(TInt aIndex) const;
	// For ECOM.
	static CApaDataRecognizerType* CreateRecognizerL();
private: // From CApaDataRecognizerType.
	void DoRecognizeL(const TDesC& aName, const TDesC8& aBuffer);
	};

#endif
