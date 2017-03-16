# this is a simple perl script to test the log parsing functionality
# of the RavenClawLogParser perl module

use RavenClawLogParser;

# hash to hold the log information
my %log;

ParseLog(shift, \%log);

foreach $key (keys %log) {
  if(($key =~ /input/) || 
     ($key =~ /error/)) {
    print "$key:$log{$key}\n";
  }
}

print scalar(keys %log);
