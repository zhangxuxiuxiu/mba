### this framework is referred from the atm example of << c++ concurrency in action>>
### with a lot of design decisions changed, like optimizing the time complexity O(N) 
### of the dispatchering messages to O(1), composableHandler could support a more   
### clean way to dispatch than the dispatcher in atm.

###########################  cmf  ############################# 
				concurrent message flow architecture

	this project aims to construct a loosely coupled architecture in which 
different components communicate with each other through messages via 
an uniform message sender and each component is ususally a message-driven
thread.

	messages are handled by handlers, there three kinds of handlers:
		1> WrappedHandler is used to process a specific type of message. 
	It's a template handler with a message type as its template parameter.
		2> SyncHandler is a composable handler to deal with messages 
	synchrounously. It can act as a subhandler or just as a whole 
	system in current thread.
		3> AsyncHandler is a composable handler to deal with messages
	asynchrounously. It should act as a subhandler to deal with I/O 
	bound messages so that it will yield in its own thread. 

	besides above handlers, a few key features are as follows:
		1> a handler processes multiple event types;
		2> a event type could be processed by multiple handlers;
		3> a specific event gets dispatched to its handler in constant time;
		4> all events will be sent to system handler then dispatched to their
			corresponding handlers;
		### syntactic constraints ###
		5> a handler must be initialized with a event type list;
		6> a handler can only process event types specified in its declared type
			list, or compilation error occurs;
		### available user components ###
		7> headquarter acts as the main thread to hang the whole application;
		8> regional office acts as regional general agent to receive and deliver 
			messages synchronously or asynchronously;
