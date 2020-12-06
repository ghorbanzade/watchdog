# Design Document

This document provides a brief summary of what Watchdog is trying to achieve,
a high-level overview of its current design and the reasons behind those
design decisions, and a long list of features and functionalities that are
required but missing in current implementation of this software. A brief
list of features that can be improved or redesigned in this software, given
more time, is provided in the last section.

## Objective

Watchdog attempts to monitor all filesystem events and detect file changes
that occur in a given set of directories, while identifying the processes
that make those changes and providing some basic information about them.
Such information is useful in detecting potential security breaches and
fighting malicious software such as ransomewares that primarily attack
filesystem objects.

## Current Design

The goal of capturing process information has had the most impact on how
Watchdog is designed, since it implies that watchdog should be a security software, not a utility file change monitoring application.

To conform to the requirements of a potential security software, Watchdog
is designed to leave room for future features that can make it protective
of other processes and resilient against attacks by other processes.
While these ideals have shaped the design of Watchdog, they are missing
from the current implementation since Watchdog is developed in very short
time.

### High-Level Architecture

This software has two main components: a `watchdogd` daemon that is meant
to be run as a `systemctl` service with root-level permissions, and a
`watchdog` command-line tool that helps us interact with the service, by
registering directories to watch and reading filesystem events captured by
the service.
Given that the command-line tool interacts with the service, ideally, it
should require root-level permissions too. But we leave it to the user
installing the software to install these tools with system permissions.
So both tools can be run as regular users, as standalone executables, at
the moment.

While the daemon is meant to have a long lifetime, the command line tool,
exits as soon as it receives a response for its first interaction with the
daemon. These responses are usually simple acknowledgements by the daemon
that the request has been accepted but dependending on the request, they
may include content that the tool should report to the user.

### Communication Mechanism

The list of supported requests is provided below:

* Add: adds one or more directories to the watch-list
* Clear: removes all directories from the watch-list
* List: reports list of all directories on the watch-list
* Monitor: reports captured file system events triggered from directories
  on the watchlist or their sub-directories. This report includes information
  about the processes initiating the events.

The user of the command-line tool can initiate these requests by providing
the appropriate command-line option (as documented in [Running.md]). The
tool interprets the provided command-line arguments and passes the request
to the daemon.

Currently, the two watchdog components interact with each other through
two named pipes (see next section for why we think this is a bad idea).
The command-line tool uses one of the pipes, to submit *request messages* to
the daemon, and a separate pipe to receive responses from the daemon.

The choice to use two named pipes instead of one is made to make this
interaction asynchronous for the ease of implementation as it allows us to
receive the requests from one thread and respond to them in another thread.
But this asynchronous process may lead to unintended side effects specially
when using named pipes. If the daemon responds to multiple requests in
succession, the client may read and discard the succeeding messages.
As explained in the next section, given more time, we will likely reverse
this decision.

Currently, "Request Messages" are serialized into string using a very simple
logic that separates the "message type" from its potentially empty "content"
with a comma. While this logic is sufficient for our current needs, in any
serious application, this serialization mechanism should evolve into using
an established protocol based on a well-defined schema.

### Daemon Architecture

### Collecting Filesystem Events

## Limitations

## Future Work

[Running.md]: ./Running.md