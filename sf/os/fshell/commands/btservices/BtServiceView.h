// BtServiceView.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef BTSERVICEVIEW_H_
#define BTSERVICEVIEW_H_

const TInt KMaxDescriptorLength = 256;

//this class can be used as Interface to receive progress information 
//from CBtServicesEng
//
class CBtServiceView
	{
public:
	CBtServiceView();
	virtual ~CBtServiceView();
	
	virtual void PrintTextMsg(TInt aVerboseLevel, const TDesC& aMsg);
	virtual void AsyncCompleted();
	
	};

#endif
