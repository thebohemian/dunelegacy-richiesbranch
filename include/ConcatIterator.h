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

#ifndef CONCATITERATOR_H
#define CONCATITERATOR_H

#include <list>

template <class T> class ConcatIterator {

private:
	typedef std::list<T> TList;
	typedef TList* TListPointer;
	typedef typename TList::iterator TList_Iter;
	typedef std::list< TListPointer> TListOfList;
	typedef typename TListOfList::iterator TListOfList_Iter;


	TList_Iter List_Iter;
	TListOfList_Iter ListOfList_Iter;
	TListOfList ListOfList;
public:
	ConcatIterator() { };
	~ConcatIterator() { };

	void addList(std::list<T>& _List) {
		if(_List.empty()) {
			// ignore this list
			return;
		}

		if(ListOfList.empty()) {
			ListOfList.push_back(&_List);
			ListOfList_Iter = ListOfList.begin();
			List_Iter = (*ListOfList_Iter)->begin();
		} else {
			ListOfList.push_back(&_List);
		}
	}

	bool IterationFinished() {
		if(ListOfList.empty()) {
			return true;
		}

		if(ListOfList_Iter == ListOfList.end()) {
			return true;
		}

		return false;
	}

	T operator* () {
		if(IterationFinished()) {
			fprintf(stderr,"ConcatIterator::operator*(): Cannot dereference because iteration is finished.\n");
			exit(EXIT_FAILURE);
		}

		return *List_Iter;
	}

	void operator++ () {
		if(IterationFinished()) {
			fprintf(stderr,"ConcatIterator::operator++(): Cannot increment because iteration is finished.\n");
			exit(EXIT_FAILURE);
		}

		++List_Iter;
		if(List_Iter == (*ListOfList_Iter)->end()) {
			// use next list
			++ListOfList_Iter;
			if(ListOfList_Iter == ListOfList.end()) {
				return;
			}

			List_Iter = (*ListOfList_Iter)->begin();
		}
	}
};

#endif // CONCATITERATOR_H
