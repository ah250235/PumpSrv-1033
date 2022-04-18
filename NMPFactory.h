#pragma once

#include "NmpDetalis.h"

//
//	CNMPFactory
//
//  Need More Parameter Detail object Factory
//  Use a Lookup table to find the proper NMPLookup_Entry entry
//  when found, use lamda function to create an instance of the detail instance
//
class CNMPFactory {

protected:

	typedef struct {
		NMP_TYPE NMPType;
		char*    PATH;
		short	OLAState;

		// implement lamda function to create the proper detail object
		CNMPDetails* (*func_ptr_)(void);
	} NMPLookup_Entry;

	static const NMPLookup_Entry build_Lookup_[];
	static CNMPFactory inst_;

private:
	// allow only one instance Singleton
	CNMPFactory() { ; }

public:
	CNMPDetails* Create(NMP_TYPE NMPType, char *BasePath);

	// Singleton Instance
	static CNMPFactory& Inst() {
		return inst_;
	}
};
