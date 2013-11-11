// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalTechnology
*/

#ifndef SYMBIAN_DEN_APIEXTENSIONREGISTER_H
#define SYMBIAN_DEN_APIEXTENSIONREGISTER_H

#include <elements/nm_common.h>

namespace Den
{

class TApiExtIdentification;

/**
	@internalTechnology

	Utility class
*/
class TApiExtRegister
	{
public:
	~TApiExtRegister();

	void RegisterOpenInterfaceLC(const TApiExtIdentification& aExtItfId);
	void DeregisterClosedInterface(const TApiExtIdentification& aExtItfId);
	TBool IsOpened(const TApiExtIdentification& aExtItfId);

	//For interface enumeration
	TInt InterfaceCount()
		{
		return iApiExtList.Count();
		}
	const TApiExtIdentification& operator[](TInt aInd);

private:
	static void CleanupInterfaceRegistration(TAny* aSelf);
	void CleanupInterfaceDeregistrationPushL(const TApiExtIdentification& aExtItfId);
	TApiExtIdentification* iClenupInfoExtItfId; //Used for cleanup only

private:
	RArray<TApiExtIdentification> iApiExtList;
	};

} // namespace Den

#endif // SYMBIAN_DEN_APIEXTENSIONREGISTER_H

