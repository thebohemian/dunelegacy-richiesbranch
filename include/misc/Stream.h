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

#ifndef STREAM_H
#define STREAM_H


#include <SDL.h>
#include <string>
#include <list>
#include <vector>
#include <set>
#include <exception>

class Stream
{
public:
	Stream() { ; };
	virtual ~Stream() { ; };

	/**
        This method flushes all caches and finally writes out all cached output.
	*/
	virtual void flush() = 0;

	// write operations

	virtual void writeString(const std::string& str) = 0;

	virtual void writeUint8(Uint8 x) = 0;
	virtual void writeUint16(Uint16 x) = 0;
	virtual void writeUint32(Uint32 x) = 0;
	virtual void writeUint64(Uint64 x) = 0;
	virtual void writeBool(bool x) = 0;
	virtual void writeDouble(double x) = 0;

	// read operations

	/**
		readString reads in a strings from the stream.
        \return the read string
	*/
	virtual std::string readString() = 0;

	virtual Uint8 readUint8() = 0;
	virtual Uint16 readUint16() = 0;
	virtual Uint32 readUint32() = 0;
	virtual Uint64 readUint64() = 0;
	virtual bool readBool() = 0;
	virtual double readDouble() = 0;


    /**
        Writes out a Sint8 value.
        \param x    the value to write out
	*/
	void writeSint8(Sint8 x) {
        Uint8 tmp = *((Uint8*) &x);
        writeUint8(tmp);
	}

    /**
        Writes out a Sint16 value.
        \param x    the value to write out
	*/
	void writeSint16(Sint16 x) {
        Uint16 tmp = *((Uint16*) &x);
        writeUint16(tmp);
	}

	/**
        Writes out a Sint32 value.
        \param x    the value to write out
	*/
	void writeSint32(Sint32 x) {
        Uint32 tmp = *((Uint32*) &x);
        writeUint32(tmp);
	}

	/**
        Writes out a Sint64 value.
        \param x    the value to write out
	*/
	void writeSint64(Sint64 x) {
        Uint64 tmp = *((Uint64*) &x);
        writeUint64(tmp);
	}

	/**
        Reads in a Sint8 value.
        \return the read value
	*/
	Sint8 readSint8() {
	    Uint8 tmp = readUint8();
        return *((Sint8*) &tmp);
	}

	/**
        Reads in a Sint16 value.
        \return the read value
	*/
	Sint16 readSint16() {
	    Uint16 tmp = readUint16();
        return *((Sint16*) &tmp);
	}

	/**
        Reads in a Sint32 value.
        \return the read value
	*/
	Sint32 readSint32() {
	    Uint32 tmp = readUint32();
        return *((Sint32*) &tmp);
	}

	/**
        Reads in a Sint64 value.
        \return the read value
	*/
	Sint64 readSint64() {
	    Uint64 tmp = readUint64();
        return *((Sint64*) &tmp);
	}

	/**
		Writes out a complete list of Uint32
		\param	List	the list to write
	*/
	void writeUint32List(const std::list<Uint32>& List) {
		writeUint32(List.size());
		std::list<Uint32>::const_iterator iter;
		for(iter=List.begin(); iter != List.end(); ++iter) {
			writeUint32(*iter);
		}
	}

	/**
		Reads a list of Uint32 written by writeUint32List().
		\return	the read list
	*/
	std::list<Uint32> readUint32List() {
		std::list<Uint32> List;
		Uint32 size = readUint32();
		for(unsigned int i=0; i < size; i++) {
			List.push_back(readUint32());
		}
		return List;
	}

	/**
		Writes out a complete vector of Uint32
		\param	vec	the vector to write
	*/
	void writeUint32Vector(const std::vector<Uint32>& vec) {
		writeUint32(vec.size());
		std::vector<Uint32>::const_iterator iter;
		for(iter=vec.begin(); iter != vec.end(); ++iter) {
			writeUint32(*iter);
		}
	}

	/**
		Reads a vector of Uint32 written by writeUint32Vector().
		\return	the read vector
	*/
	std::vector<Uint32> readUint32Vector() {
		std::vector<Uint32> vec;
		Uint32 size = readUint32();
		for(unsigned int i=0; i < size; i++) {
			vec.push_back(readUint32());
		}
		return vec;
	}

    /**
		Writes out a complete set of Uint32
		\param	Set	the set to write
	*/
	void writeUint32Set(const std::set<Uint32>& Set) {
		writeUint32(Set.size());
		std::set<Uint32>::const_iterator iter;
		for(iter=Set.begin(); iter != Set.end(); ++iter) {
			writeUint32(*iter);
		}
	}

    /**
		Reads a set of Uint32 written by writeUint32Set().
		\return	the read set
	*/
	std::set<Uint32> readUint32Set() {
		std::set<Uint32> retSet;
		Uint32 size = readUint32();
		for(unsigned int i=0; i < size; i++) {
			retSet.insert(readUint32());
		}
		return retSet;
	}

	class exception : public std::exception {
	public:
		exception() throw () { ; };
		virtual ~exception() throw () { ; };
	};

	class eof : public Stream::exception {
	public:
		eof(const std::string& str) throw () { this->str = str; };
		virtual ~eof() throw () { ; };

		virtual const char* what() const throw () { return str.c_str(); };

	private:
		std::string str;
	};

	class error : public Stream::exception {
	public:
		error(const std::string& str) throw () { this->str = str; };
		virtual ~error() throw () { ; };

		virtual const char* what() const throw () { return str.c_str(); };

	private:
		std::string str;
	};
};

#endif // STREAM_H
