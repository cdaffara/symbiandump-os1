/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CustomInterface Builder
*
*/

#ifndef __CUSTOMINTERFACEBUILDER_H
#define __CUSTOMINTERFACEBUILDER_H

#include <e32std.h>
#include <mmf/common/mmfcontroller.h>
#include <MCustomInterface.h>

class CAdaptationCustomInterfaceBuilder;
class CMMFDevSound;
class CMMFObjectContainer;

class CCustomInterfaceBuilder : public CMMFObject
	{
public:
	IMPORT_C static CCustomInterfaceBuilder* NewL(CMMFObjectContainer& aObjectContainer,
	                                              CMMFDevSound&        aDevSound);
	virtual ~CCustomInterfaceBuilder();

	virtual void HandleRequest(TMMFMessage& aMessage);

    IMPORT_C static CCustomInterfaceBuilder* NewL(CMMFObjectContainer& aObjectContainer,
	                                              MCustomInterface& aCustomInterface);

    IMPORT_C void SetSecureCustomInterfaces( const TBool aSecure );

private:
	CCustomInterfaceBuilder(CMMFObjectContainer& aObjectContainer,
	                        CMMFDevSound&        aDevSound);
	CCustomInterfaceBuilder(CMMFObjectContainer& aObjectContainer,
	                        MCustomInterface& aCustomInterface);
	void ConstructL();
	void        DoHandleRequestL(TMMFMessage& aMessage);
	void        DoBuildL(TMMFMessage& aMessage);
	void        DoRemoveL(TMMFMessage& aMessage);
	TAny*       GetCustomInterfaceL(TUid aInterfaceId);

	//CMMFDevSound&                      iDevSound;
    CMMFDevSound*                      iDevSound;
	MCustomInterface*                  iCustomInterface;
	CMMFObjectContainer&               iObjectContainer;
	CAdaptationCustomInterfaceBuilder* iAdaptationBuilder;
	TBool                              iSecureCustomInterfaces;
	};

#endif
