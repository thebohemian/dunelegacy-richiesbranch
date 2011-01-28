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

#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include "Stream.h"
#include <stdlib.h>
#include <string>

class MemoryStream : public Stream
{
public:
	MemoryStream()
	 : bReadOnly(true), currentPos(0), bufferSize(0), pBuffer(NULL) {
        ;
    }

	~MemoryStream() {
        if(bReadOnly == false) {
            free(pBuffer);
        }
	}

	void open(const char* data, int length) {
        if(bReadOnly == false) {
            free(pBuffer);
        }

	    bReadOnly = true;
        currentPos = 0;
        bufferSize = length;
        pBuffer = const_cast<char*>(data);
	}

	void open() {
        if(bReadOnly == false) {
            free(pBuffer);
        }

        bReadOnly = false;
        currentPos = 0;
        bufferSize = 4;
        pBuffer = (char*) malloc(4);
        if(pBuffer == NULL) {
            throw Stream::error("MemoryStream::open(): malloc failed!");
        }
	}

    const char* getData() const {
        return pBuffer;
    }

    int getDataLength() const {
        return (bReadOnly == true) ? bufferSize : currentPos;
    }

	void flush() {
        ;
	}



	// write operations

	void writeString(const std::string& str) {
	    ensureBufferSize(currentPos + str.length() + sizeof(Uint32));

        writeUint32(str.length());

        if(!str.empty()) {
            memcpy(pBuffer + currentPos, str.c_str(), str.length());
            currentPos += str.length();
        }
    }


	void writeUint8(Uint8 x) {
        ensureBufferSize(currentPos + sizeof(Uint8));
        *((Uint8*) (pBuffer + currentPos)) = x;
        currentPos += sizeof(Uint8);
	}

	void writeUint16(Uint16 x) {
        ensureBufferSize(currentPos + sizeof(Uint16));
        *((Uint16*) (pBuffer + currentPos)) = x;
        currentPos += sizeof(Uint16);
	}

	void writeUint32(Uint32 x) {
        ensureBufferSize(currentPos + sizeof(Uint32));
        *((Uint32*) (pBuffer + currentPos)) = x;
        currentPos += sizeof(Uint32);
	}

	void writeUint64(Uint64 x) {
        ensureBufferSize(currentPos + sizeof(Uint64));
        *((Uint64*) (pBuffer + currentPos)) = x;
        currentPos += sizeof(Uint64);
	}

	void writeBool(bool x) {
        writeUint8(x == true ? 1 : 0);
	}

	void writeDouble(double x) {
        if(sizeof(double) != sizeof(Uint64)) {
            throw Stream::error("MemoryStream::writeDouble(): sizeof(double) != sizeof(Uint64). Cannot save doubles on such systems.");
        }
        Uint64 tmp;
        memcpy(&tmp,&x,sizeof(Uint64)); // workaround for a strange optimization in gcc 4.1
        writeUint64(tmp);
    }

	// read operations

	std::string readString() {
	    Uint32 length = readUint32();
	    std::string resultString(pBuffer + currentPos, length);
	    currentPos += length;
	    return resultString;
	}

	Uint8 readUint8() {
	    if(currentPos >= bufferSize) {
            throw Stream::eof("MemoryStream::readUint8(): End-of-File reached!");
	    }

        Uint8 tmp = *((Uint8*) (pBuffer + currentPos));
        currentPos += sizeof(Uint8);
        return tmp;
	}

	Uint16 readUint16() {
	    if(currentPos >= bufferSize) {
            throw Stream::eof("MemoryStream::readUint16(): End-of-File reached!");
	    }

        Uint16 tmp = *((Uint16*) (pBuffer + currentPos));
        currentPos += sizeof(Uint16);
        return tmp;
	}

	Uint32 readUint32() {
	    if(currentPos >= bufferSize) {
            throw Stream::eof("MemoryStream::readUint32(): End-of-File reached!");
	    }

        Uint32 tmp = *((Uint32*) (pBuffer + currentPos));
        currentPos += sizeof(Uint32);
        return tmp;
	}

	Uint64 readUint64() {
	    if(currentPos >= bufferSize) {
            throw Stream::eof("MemoryStream::readUint64(): End-of-File reached!");
	    }

        Uint64 tmp = *((Uint64*) (pBuffer + currentPos));
        currentPos += sizeof(Uint64);
        return tmp;
	}

	bool readBool() {
        return (readUint8() == 1 ? true : false);
    }

	double readDouble() {
        if(sizeof(double) != sizeof(Uint64)) {
            throw Stream::error("MemoryStream::readDouble(): sizeof(double) != sizeof(Uint64). Cannot read doubles on such systems.");
        }

        Uint64 tmp = readUint64();
        double tmp2;
        memcpy(&tmp2,&tmp,sizeof(Uint64)); // workaround for a strange optimization in gcc 4.1
        return tmp2;
    }

	void ensureBufferSize(int minBufferSize) {
        if(minBufferSize < bufferSize) {
            return;
        }

        int newBufferSize = ((bufferSize * 3) / 2);
        if(newBufferSize < minBufferSize) {
            newBufferSize = minBufferSize;
        }

        char* pNewBuffer = (char*) realloc(pBuffer, newBufferSize);
        if(pNewBuffer == NULL) {
            throw Stream::error("MemoryStream::ensureBufferSize(): realloc failed!");
        } else {
            pBuffer = pNewBuffer;
            bufferSize = newBufferSize;
        }
	}

private:
    bool    bReadOnly;
    int     currentPos;
    int     bufferSize;
	char*   pBuffer;
};

#endif // MEMORYSTREAM_H
