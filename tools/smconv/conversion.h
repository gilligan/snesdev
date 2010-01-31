#ifndef CONVERSION_H
#define CONVERSION_H

#include "inputdata.h"

namespace Conversion {

	class Pattern {
	};

	class Sample {
	};
	
	class Instrument {
	};

	class Module {
		
	};
	
	// converted data handler
	class Bank {
		
	public:
		Bank( SoundbankData * );
		void AddModule( const char *filename );


	};
};

#endif
