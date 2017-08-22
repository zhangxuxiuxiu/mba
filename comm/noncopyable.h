#pragma once

namespace comm 
{
	class Noncopyable
	{
		protected:
			Noncopyable(){}

		private:
			Noncopyable(const Noncopyable&);
			Noncopyable& operator =(const Noncopyable&);
	};

}

