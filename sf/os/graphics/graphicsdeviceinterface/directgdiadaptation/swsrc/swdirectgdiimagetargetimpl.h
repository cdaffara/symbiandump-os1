// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SWDIRECTGDIIMAGETARGETIMPL_H_
#define SWDIRECTGDIIMAGETARGETIMPL_H_

/**
@file
@internalComponent
*/

#include "directgdiimageref.h"
#include <e32def.h>

class CSwDirectGdiDriverImpl;
class CFbsDrawDevice;

/**
A concrete software implementation of an image target represented by RSgImage.
*/
NONSHARABLE_CLASS(CSwDirectGdiImageTargetImpl) : public CDirectGdiImageRef
{
public:
	static TInt New(CSwDirectGdiImageTargetImpl*& aImage, CSwDirectGdiDriverImpl& aDriver, CFbsDrawDevice* aDrawDevice, const RSgImage& aSgImage);
	virtual ~CSwDirectGdiImageTargetImpl();
	CFbsDrawDevice* DrawDevice() const;
	
private:
	CSwDirectGdiImageTargetImpl(CSwDirectGdiDriverImpl& aDriver, CFbsDrawDevice*);
	TInt Construct(const RSgImage& aSgImage);
	
private:
	CSwDirectGdiDriverImpl& iDriver;
	CFbsDrawDevice* iDrawDevice;
};

#endif /*SWDIRECTGDIIMAGETARGETIMPL_H_*/
