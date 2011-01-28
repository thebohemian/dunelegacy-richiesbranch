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

#include <misc/FileStream.h>

#include <string.h>
#include <SDL_endian.h>

FileStream::FileStream()
{
	fp = NULL;
}

FileStream::~FileStream()
{
	close();
}

bool FileStream::open(const char* filename, const char* mode)
{
	if(fp != NULL) {
		fclose(fp);
	}

	if( (fp = fopen(filename,mode)) == NULL) {
		return false;
	} else {
		return true;
	}
}

bool FileStream::open(std::string filename, const char* mode)
{
	return open(filename.c_str(), mode);
}

void FileStream::close()
{
	if(fp != NULL) {
		fclose(fp);
		fp = NULL;
	}
}

void FileStream::flush() {
    if(fp != NULL) {
        fflush(fp);
    }
}

void FileStream::writeString(const std::string& str)
{
	writeUint32(str.length());

    if(!str.empty()) {
        if(fwrite(str.c_str(),str.length(),1,fp) != 1) {
            throw Stream::error("FileStream::writeString(): An I/O-Error occurred!");
        }
    }
}

void FileStream::writeUint8(Uint8 x)
{
	if(fwrite(&x,sizeof(Uint8),1,fp) != 1) {
		throw Stream::error("FileStream::writeUint8(): An I/O-Error occurred!");
	}
}

void FileStream::writeUint16(Uint16 x)
{
	x = SDL_SwapLE16(x);

	if(fwrite(&x,sizeof(Uint16),1,fp) != 1) {
		throw Stream::error("FileStream::writeUint16(): An I/O-Error occurred!");
	}
}

void FileStream::writeUint32(Uint32 x)
{
	x = SDL_SwapLE32(x);

	if(fwrite(&x,sizeof(Uint32),1,fp) != 1) {
		throw Stream::error("FileStream::writeUint32(): An I/O-Error occurred!");
	}
}

void FileStream::writeUint64(Uint64 x)
{
	x = SDL_SwapLE64(x);
	if(fwrite(&x,sizeof(Uint64),1,fp) != 1) {
		throw Stream::error("FileStream::writeUint64(): An I/O-Error occurred!");
	}
}

void FileStream::writeBool(bool x)
{
	writeUint8(x == true ? 1 : 0);
}

void FileStream::writeDouble(double x)
{
	if(sizeof(double) != sizeof(Uint64)) {
		throw Stream::error("FileStream::writeDouble(): sizeof(double) != sizeof(Uint64). Cannot save doubles on such systems.");
	}
	Uint64 tmp;
	memcpy(&tmp,&x,sizeof(Uint64)); // workaround for a strange optimization in gcc 4.1
	writeUint64(tmp);
}

std::string FileStream::readString()
{
	Uint32 length;

	length = readUint32();

    if(length == 0) {
        return "";
    } else {
        std::string str;

        str.resize(length);

        if(fread(&str[0],length,1,fp) != 1) {
            if(feof(fp) != 0) {
                throw Stream::eof("FileStream::readString(): End-of-File reached!");
            } else {
                throw Stream::error("FileStream::readString(): An I/O-Error occurred!");
            }
        }

        return str;
    }
}

Uint8 FileStream::readUint8()
{
	Uint8 tmp;
	if(fread(&tmp,sizeof(Uint8),1,fp) != 1) {
		if(feof(fp) != 0) {
			throw Stream::eof("FileStream::readUint8(): End-of-File reached!");
		} else {
			throw Stream::error("FileStream::readUint8(): An I/O-Error occurred!");
		}
	}

	return tmp;
}

Uint16 FileStream::readUint16()
{
	Uint16 tmp;
	if(fread(&tmp,sizeof(Uint16),1,fp) != 1) {
		if(feof(fp) != 0) {
			throw Stream::eof("FileStream::readUint16(): End-of-File reached!");
		} else {
			throw Stream::error("FileStream::readUint16(): An I/O-Error occurred!");
		}
	}

	return SDL_SwapLE16(tmp);
}

Uint32 FileStream::readUint32()
{
	Uint32 tmp;
	if(fread(&tmp,sizeof(Uint32),1,fp) != 1) {
		if(feof(fp) != 0) {
			throw Stream::eof("FileStream::readUint32(): End-of-File reached!");
		} else {
			throw Stream::error("FileStream::readUint32(): An I/O-Error occurred!");
		}
	}

	return SDL_SwapLE32(tmp);
}

Uint64 FileStream::readUint64()
{
	Uint64 tmp;
	if(fread(&tmp,sizeof(Uint64),1,fp) != 1) {
		if(feof(fp) != 0) {
			throw Stream::eof("FileStream::readUint64(): End-of-File reached!");
		} else {
			throw Stream::error("FileStream::readUint64(): An I/O-Error occurred!");
		}
	}
	return SDL_SwapLE64(tmp);
}

bool FileStream::readBool()
{
	return (readUint8() == 1 ? true : false);
}

double FileStream::readDouble()
{
	if(sizeof(double) != sizeof(Uint64)) {
		throw Stream::error("FileStream::readDouble(): sizeof(double) != sizeof(Uint64). Cannot read doubles on such systems.");
	}

	Uint64 tmp = readUint64();
	double tmp2;
	memcpy(&tmp2,&tmp,sizeof(Uint64)); // workaround for a strange optimization in gcc 4.1
	return tmp2;
}
