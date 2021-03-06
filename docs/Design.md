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
two named pipes (see the "Limitations" section for why we think this is
a bad idea).
The command-line tool uses one of the pipes, to submit *request messages* to
the daemon, and a separate pipe to receive responses from the daemon.

The choice to use two named pipes instead of one is made to make this
interaction asynchronous for the ease of implementation as it allows us to
receive the requests from one thread and respond to them in another thread.
But this asynchronous process may lead to unintended side effects specially
when using named pipes. If the daemon responds to multiple requests in
succession, the client may read and discard the succeeding messages.
As explained in the "Limitations" section, given more time, we will
likely reverse this decision.

Currently, "Request Messages" are serialized into string using a very simple
logic that separates the "message type" from its potentially empty "content"
with a comma. While this logic is sufficient for our current needs, in any
serious application, this serialization mechanism should evolve into using
an established protocol based on a well-defined schema.

### Daemon Architecture

Watchdog deamon is designed for simplicity and modularity. It uses a simple
thread pool to manage workers that each perform separate tasks, while having
limited interactions with each other to communicate the results of those
tasks.

As of v1.0 (which should be considered as v0.0.1), the daemon uses three
separate threads whose responsibilties can be summarized as follows:

* `named_pipe_reader`: handles requests received from the command-line tool.
  This thread listens to a named pipe and deserializes any recieved request
  into a `NamedPipeMessage` which is *pushed* into a `NamedPipeMessageQueue`
  queue.

* `inventory_manager`: performs requests received by the client and submits
  responses to a named pipe for consumption by the command-line tool. Since
  one possible request is to report collected filesystem events, this thread
  is one and only consumer of the filesystem event queue.

* `event_collector_lsof`: uses the `lsof` utility command line tool to collect
  information about the open files in directories under watch and generate
  `Event` instances representing file system events that are put into the
  filesystem event queue.

The thread `event_collector_lsof` is the only producer of Filesystem events
in the current implementation. But this general design allows for having
multiple "event collector" threads that use different mechanism to capture
useful information from different sources that may complement each other
and contribute to forming a better understanding of the events.

While using multiple "event collectors" makes sense, we can always except
overlap in the collected information which requires introduction of a new
aggregator module that keeps track of the captured data and eliminates
duplicate events that are previously reported by other event collectors.
Correctly implementing this feature requires more time and research.

## Limitations

### Collecting Filesystem Events

Currently, we are only using one source `lsof` to collect relevant information.
`lsof` is very powerful and is useful as *one* source of information but
lacks many essential features which make the case that it should not be
the *only* source of information. As an example, `lsof` is meant to list
open files; it is not the right tool to detect creation or deletion of files,
or renaming or moving them, which are the among the events of interest to us.

Moreover, the way we are using `lsof` to collect information is far from
desirable. To start, we are piping into `lsof` as a separate process which
is a general bad practice and makes `lsof` a potential attack vector.
In addition, since we are periodically polling `lsof`, there is always
a chance that we miss file accesses that start and terminate during our
polling interval.

We can address these issues, by improving our implementation of this event
collector, and by incorporating separate independent event collectors to
cross-reference our findings. In addition, using a separate complementary
approach of tracking processes' activities rather than filesystem objects,
we may have further improve our coverage of identifying events.
Suggestions on these topics are provided in the "Future Work" section below.

### Message Queues

Our implementation of event queue is very crude and prone to exploitation.
In the current design, the queue can be easily overflowed by a potential
attacker seeking to generate a large number of events. While imposing a
strict limit on the size of the queue may lead to the lose of information,
it is still possible to limit inflows to the queue by monitoring its inflow.
Another possible approach is to improve the implementation to better handle
queue overflows. While these mitigations may not prevent lose of information,
we believe they may help control it.

### Using Named Pipes

As we mentioned earlier, we think that using Named Pipes is error-prone and
inefficient. Named Pipes do not offer any means for correctly handling multiple
readers. If multiple instances of command-line tool are running at the
same time, the response issues for the request from one instance may be
read and consumed by another.

We believe the only solution to this issue is a complete replacement of this
mechanism with a less primitive mechanism such as using sockets. This way we
can distinguish between different clients and effectively serve their requests.

## Future Work

### Additional Event Collectors

To improve our success in capturing all file system events, we propose
implementing additional event collectors as separate independent sources
of information.

Two candidate options for collecting such information are `inotify` and
`auditctl` (if available):

* `inotify` may be missing process information but is widely-recognized as
  the primary API for capturing filesystem change events. Knowing these events
  can at least help us determine that we are missing important information
  from `lsof` and other sources.

* `auditctl` may not be available on all machines but it can provide valuable
  information about processes and the files they access.

We can also use separate different approaches to enhance our coverage:

* We can query the list of running processes and audit the list of files
  they are accessing. To catch missed events, we can periodically scan the
  directories under watch and maintain records of the existing files and some
  of their properties such as content hash and last modified date.

* We can place "canary" files in the directories under watch and attempt to
  identify and stop any other process that may access/modify them.

### Tracking Process Activities

We can improve our findings by maintaining records on activities of each
process, excluding processes on our approved lists and attempting to detect
if they form a pattern similar to milicious applications.

### Improved Resiliency

To fight against code-injection and other direct attacks, we can check the
integrity of our client processes before trusting their requests.

### Intrusive Protection

As a next step, by establishing patterns about activities of processes, it
is possible to compare them against patterns of known malicious softwares.
If we are running with root-level permissions, it is possible to take action
against such processes and intrusively fight them.

[Running.md]: ./Running.md