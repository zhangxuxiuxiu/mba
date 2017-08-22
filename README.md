### this framework is referred from the atm example of << c++ concurrency in action>>
### with a lot of design decisions changed, like optimizing the time complexity O(N) 
### of the dispatchering messages to O(1), composableHandler could support a more   
### clean way to dispatch than the dispatcher in atm.

###########################  mba  ############################# 
				message based architecture

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
