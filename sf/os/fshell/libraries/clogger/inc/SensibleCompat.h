// SensibleCompat.h
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

#ifndef SENSIBLECOMPAT_H
#define SENSIBLECOMPAT_H

#ifdef __HIDE_IPC_V1__

// Differences in client server stuff
#define RMessage RMessage2
#define CServerBase CServer2
#define CSessionBase CSession2
#define SLOT(msg, n) n
#define IPC(name, a,b,c,d) TIpcArgs name(a,b,c,d);

#else

#define CServerBase CServer
#define CSessionBase CSharableSession
#define SLOT(msg, n) msg.Ptr ## n ()
#define IPC(name, a,b,c,d) TInt name[4] = {(TInt)a,(TInt)b,(TInt)c,(TInt)d}

#endif

#ifndef EKA2

// Some things EKA1 platforms didn't have
#define TRAP_IGNORE(x) { TRAPD(dontCare, x); }

#endif

#endif /* SENSIBLECOMPAT_H */
