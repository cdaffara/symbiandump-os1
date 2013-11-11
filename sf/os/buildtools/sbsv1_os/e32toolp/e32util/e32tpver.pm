# Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
# Returns the version number for E32TOOLP - update for each release
# 
#

package E32tpver;
require Exporter;
@ISA=qw(Exporter);
@EXPORT=qw(
	E32tpver
);


sub E32tpver () {
	my $Version=678; 
}

1;
