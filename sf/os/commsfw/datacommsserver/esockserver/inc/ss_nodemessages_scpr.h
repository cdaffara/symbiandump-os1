// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// THIS API IS INTERNAL TO NETWORKING AND IS SUBJECT TO CHANGE AND NOT FOR EXTERNAL USE
// 
//

/**
 @file
 @publishedPartner
 @released
*/

#ifndef SYMBIAN_NODEMESSAGES_SCPR_H
#define SYMBIAN_NODEMESSAGES_SCPR_H

#include <comms-infras/ss_nodemessages_rejoiningprovider.h>
#include <comms-infras/ss_nodemessages_subconn.h>

namespace ESock
{

class TCFScpr : public TCFDataClient, public TCFServiceProvider, public TCFRejoiningProvider, public TCFControlProvider, public TCFParameterProvider
	{
private:
protected:
public:
    enum { ERealmId = 0x10285F4D }; //UID allocated on 4/6/08 from KUidNodeMessageRealms
	};

} //namespace esock

#endif
//SYMBIAN_NODEMESSAGES_SCPR_H

