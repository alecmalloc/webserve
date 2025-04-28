#!/usr/bin/perl
use strict;
use warnings;

sub divide {
    my ($a, $b) = @_;
    return $a / $b;
}

#this will errors bec a division by zero
my $result = divide(10, 0);

print "Result is: $result\n";

