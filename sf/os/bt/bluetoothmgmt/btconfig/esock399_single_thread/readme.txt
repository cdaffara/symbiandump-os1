// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
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

Until the pan profile has been updated to run correctly with the pan agent in
the control thread and the bnep packet driver in the data thread ESock will
need to be run in single thread mode. To use the post-399 version of ESock in
single thread mode (data and control planes running in the same thread) you
need to:

1) Remove the esock_mip.cmi and esock_dip.cmi files from the 
\epoc32\release\{TARGET}\{RELEASE}\z\private\101F7988 directory.

2) Copy the esock_ip.cmi and esock_bt.cmi files from this directory into the
\epoc32\release\{TARGET}\{RELEASE}\z\private\101F7988 directory, replacing the
existing esock_bt.cmi file.

