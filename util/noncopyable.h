#pragma once

namespace cmf 
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

