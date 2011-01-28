/*
 *  This file is part of Dune Legacy.
 *
 *  Dune Legacy is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Dune Legacy is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Dune Legacy.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef METHODPOINTER_H
#define METHODPOINTER_H

#include "CallbackTarget.h"
#include <stdlib.h>

/*
	With VC++ 6.0 you should use /vmg as compile option to let this weird member function pointers work.
*/

typedef CallbackTarget* PCallbackTarget;
typedef void (CallbackTarget::*WidgetCallbackWithNoParameter) (void);
typedef void (CallbackTarget::*WidgetCallbackWithIntParameter) (int Data);
typedef void (CallbackTarget::*WidgetCallbackWithVoidStarParameter) (void* Data);

/// A class for saving a pointer to a method of an object.
class MethodPointer {
public:
	/// default constructor
	MethodPointer() {
		Type = Method_Nothing;
		pObject = NULL;
		Data.Data_Int = 0;
	};

	/**
		Constructor that creates a pointer to an method with no parameter.
		\param	pObject	Pointer to the object
		\param	pMethod	Pointer to a method with no parameter
	*/
	MethodPointer(PCallbackTarget pObject,WidgetCallbackWithNoParameter pMethod) {
		Type = Method_NoParameter;
		this->pObject = pObject;
		PMethod.PMethod_NoParameter = pMethod;
		Data.Data_Int = 0;
	};

	/**
		Constructor that creates a pointer to an method with one int parameter. This int parameter
		is passed, when this method is called.
		\param	pObject	Pointer to the object
		\param	pMethod	Pointer to a method with one int parameter
		\param	iData	an int that is passed to this method if it is called
	*/
	MethodPointer(PCallbackTarget pObject,WidgetCallbackWithIntParameter pMethod, int iData) {
		Type = Method_IntParameter;
		this->pObject = pObject;
		PMethod.PMethod_IntParameter = pMethod;
		this->Data.Data_Int = iData;
	};

	/**
		Constructor that creates a pointer to an method with one void* parameter. This void* parameter
		is passed, when this method is called.
		\param	pObject	Pointer to the object
		\param	pMethod	Pointer to a method with one void* parameter
		\param	pData	an void* that is passed to this method if it is called
	*/
	MethodPointer(PCallbackTarget pObject,WidgetCallbackWithVoidStarParameter pMethod, void* pData) {
		Type = Method_VoidStarParameter;
		this->pObject = pObject;
		PMethod.PMethod_VoidStarParameter = pMethod;
		this->Data.Data_VoidStar = pData;
	};

	/// destructor
	virtual ~MethodPointer() { ; };

	/**
		Calls the method this object is pointing to.
	*/
	void operator() () const {
		if(pObject == NULL) {
			return;
		}

		switch(Type) {
			case Method_NoParameter: {
				(pObject->*(PMethod.PMethod_NoParameter))();
			} break;

			case Method_IntParameter: {
				(pObject->*(PMethod.PMethod_IntParameter))(Data.Data_Int);
			} break;

			case Method_VoidStarParameter: {
				(pObject->*(PMethod.PMethod_VoidStarParameter))(Data.Data_VoidStar);
			} break;

			default: {
				return;
			} break;
		}
	};

	operator bool() const {
	    return (pObject != NULL);
	}

private:
	enum {
		Method_Nothing,				///< Method points to nothing
		Method_NoParameter,			///< Method has no parameter
		Method_IntParameter,		///< Method has an int parameter
		Method_VoidStarParameter	///< Method has a void* parameter
	} Type;

	union {
		WidgetCallbackWithNoParameter		PMethod_NoParameter;
		WidgetCallbackWithIntParameter		PMethod_IntParameter;
		WidgetCallbackWithVoidStarParameter	PMethod_VoidStarParameter;
	} PMethod;					///< Pointer to the method called

	union {
		int		Data_Int;
		void*	Data_VoidStar;
	} Data;						///< Data that should be passed to the called method

	PCallbackTarget pObject;	///< Pointer to the object
};

#endif //METHODPOINTER_H
