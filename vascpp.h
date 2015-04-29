#pragma once

#include "vas.h"
class VASCpp {
	public:
		VASCpp (int n);
		~VASCpp ();

		bool insert (VAS_KEY * key, VAS_VALUE data);
		bool replace (VAS_KEY * key, VAS_VALUE data);
		bool remove (VAS_KEY * key);
		VAS_VALUE get (VAS_KEY * key);
	
	private:
		VAS * vas;
};
