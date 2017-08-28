### this framework is referred from the atm example of << c++ concurrency in action>>
### with a lot of design decisions changed, like optimizing the time complexity O(N) 
### of the dispatchering messages to O(1), composableHandler could support a more   
### clean way to dispatch than the dispatcher in atm.

###########################  cmf  ############################# 
				concurrent message flow architecture

	this project aims to construct a loosely coupled architecture in which 
different components communicate with each other through messages via 
an uniform message poster.

	messages are always wrapped as WrappedMessage and received by recipients, 
there are four kinds of recipients:
		1> WrappedRecipient is used to process a specific type of message. 
	It's a template recipient with a message type as its template parameter.
		2> LocalOffice is a composable recipient to deal with a list of message 
	types synchronously. It usually act as a sub-recipient of a big recipient 
	like AsyncOffic, HeadOffice or event another LocalOffice;
		3> AsyncOffice is a composable recipient to deal with messages
	asynchronously. It usually act as a sub-recipient to deal with I/O 
	bound messages so that it will yield in its own thread. 
		4> HeadOffice act as the main thread to hang the whole application,
	it is responsible to dispatch and negociate messages between sub-recipients.
	of course, it can receive message by itself as well.

	besides above recipients, a few key features are as follows:
		1> a recipient can process one or multiple event types;
		2> a event type could be processed by multiple recipients;
		3> a specific event gets dispatched to its recipient in constant time;
		4> all events can be sent to HeadOffice then dispatched to their
			corresponding recipients or sent to the Office where the event produces;
		### syntactic constraints ###
		5> a LocalOffic or AsyncOffice can be initialized with a event type list
		and only process event types specified in its declared type list, or compilation 
		error occurs;

future works:
	1> provide a performance-improved and interface-friendly sync-queue/concurrent-queue;
	2> support for timed message which could decay to a normal message with a default 
		now-timestamp( this feature needs a concurrent-priority-queue or a sorted vector);
	3> support thread pool in AsyncOffice 
	4> support future in ProxyOffices
