// wsp_def.h
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __WSP_DEF_H__
#define __WSP_DEF_H__

#define WIN32_HANDLE  void*

const TUint KWspMajorVersionNumber = 0;
const TUint KWspMinorVersionNumber = 1;
const TUint KWspBuildVersionNumber = 1;

TInt MapWinSockError(TInt aWinSockError);

#endif // __WSP_DEF_H__
