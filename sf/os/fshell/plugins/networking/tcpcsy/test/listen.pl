# listen.pl
# 
# Copyright (c) 2010 Accenture. All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# 
# Initial Contributors:
# Accenture - Initial contribution
#
use strict;
use IO::Socket;

my $server_port = 3030;

my $server = IO::Socket::INET->new(LocalPort => $server_port,
                                Type      => SOCK_STREAM,
                                Reuse     => 1,
                                Listen    => 10 )   # or SOMAXCONN
    or die "Couldn't be a tcp server on port $server_port : $@\n";

while (my $client = $server->accept()) {
	print "New client connected\n";
	while (my $line = <$client>) {
		print "Received: \"$line\"\n";
	}
	print "Client disconnected\n";
	close ($client);
}

close($server);


