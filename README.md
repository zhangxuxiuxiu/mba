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
		4> all events can be sent to HeadOffice then dispatched to their corresponding 
	recipients or sent to the Office where the event produces;
		5> if a message can't be processed by current office, it will be delivered to 
	its parent until the HeadOffice can't process, then an exception will throw;
		6> by default, a message will be delivered in realtime, but also it could be 
	delayed through invoking $AriseAfter(duration) and $AriseAt(time_point)

future works:
	1> support for timed message which could decay to a normal message with a default 
	now-timestamp( this feature needs a concurrent-priority-queue or a sorted vector);
		progressing:
			a)default timed message is a normal message;
			b)insertion in sorted vector is O(n) time complexity while priority-queue
		is O(log(n)), so priority-queue is preferred.
			c)message will be delivered in $duration or at $time_point
		DONE!!!

	2> a LocalOffic or AsyncOffice can be initialized with a message type list
	and only process event types specified in its declared type list, or compilation 
	error occurs;
	3> if any message specified in message type list is not bound, compilation error 
	occures in the Office constructor
	[### seems 2>&3> are unnecessary b' current solution will throw exception if a 
	message is not bound with any recipient ###]

	4> provide a performance-improved and interface-friendly sync-queue/concurrent-queue;
		to support timed message, a concurrent-priority-queue is needed.
		performance: refer to famous library
		interface-friendly: bool push(T&&) && bool pop(T&);		

	5> support thread pool in AsyncOffice; 
	6> support future in ProxyOffices; (hard and important)
	7> support state management in each office(some events can only be dealt once, some should 
	be dealt after some pre-events and so on)

thoughts:
	1>	should regional office set its sub-office's poster's source as itself automatically 
	through binding?  
			if yes, the cmf would form a layered message flow, sub-office can only post 
		messages to its parent office until no parent can process it;
			if no, all regional offices can only post messages to HeadOffice through 
		constructing themself with HeadOffice::GetPoster() as constructor parameter manually.
			when yes, uses have no sense of Poster and messages could be delivered with 
		locality considered which brings performance gain to whole system through lowering 
		HeadOffice's burden compared with NO.
			when no, setting poster manually could increase flexibility of regional offices 
		while inceasing users' knowlege burden and user class constraints(constructor must 
		take Poster as parameter).
			based on considerations above, YES is the current solution.




		






