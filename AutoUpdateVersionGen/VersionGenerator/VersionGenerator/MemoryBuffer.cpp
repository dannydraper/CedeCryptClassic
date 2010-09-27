// A memory buffer class. Written by Danny Draper (c) 2007. Provides safe memory buffer storage.

#include "MemoryBuffer.h"

MemoryBuffer::MemoryBuffer ()
{
	m_bCleared = true;
	m_bSizeset = false;
	m_iBuffersize = 0;
	m_iAppendpointer = 0;
}

MemoryBuffer::~MemoryBuffer ()
{
}



void MemoryBuffer::SetSize (unsigned long iSize)
{
	if (m_bSizeset == false) {
		m_bSizeset = true;
		m_bCleared = false;
		m_buffer = (BYTE *) calloc (iSize, 1);
		m_iBuffersize = iSize;
	} else {
		m_buffer = (BYTE *) realloc (m_buffer, iSize);
		m_iBuffersize = iSize;
		m_iAppendpointer = 0;
		m_bCleared = false;
		ZeroMemory (m_buffer, iSize);
	}
}

unsigned long MemoryBuffer::GetSize ()
{
	return m_iBuffersize;
}

BYTE MemoryBuffer::GetByte (unsigned long lIndex)
{
	if (m_bSizeset == true) {
		if (m_iAppendpointer > 0) {
			if (lIndex >=0 && lIndex < m_iAppendpointer) {
				return m_buffer[lIndex];
			} else {
				return 0;
			}
		} else {
			if (lIndex >=0 && lIndex < m_iBuffersize) {
				return m_buffer[lIndex];
			} else {
				return 0;
			}
		}
	} else {
		return 0;
	}
}

bool MemoryBuffer::Write (void *pItem, unsigned long iOffset, unsigned long itemSize)
{
	if (m_bSizeset == true) { // Make sure memory is allocated
		if (iOffset >=0 && iOffset < m_iBuffersize) { // Make sure the offset is within bounds.
			if (itemSize <= (m_iBuffersize-iOffset)) { // Make sure the item size will fit within the space available
				memcpy (m_buffer+iOffset, pItem, itemSize);
				return true;
			} else {
				return false;
			}
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool MemoryBuffer::Append (void *pItem, unsigned long itemSize)
{
	if (m_bSizeset == true) { // Make sure memory is allocated
		if (itemSize <= (m_iBuffersize-m_iAppendpointer)) {
			memcpy (m_buffer+m_iAppendpointer, pItem, itemSize);
			m_iAppendpointer+=itemSize;
			return true;
		} else {
			return false;
		}
	} else {
		return false;	
	}
}

bool MemoryBuffer::SaveToFile (LPCSTR lpszFilename)
{
	FILE *stream;

	if (m_bSizeset == true) {
		if (m_iAppendpointer > 0) {
			if ((stream = fopen (lpszFilename, "w+b")) == NULL) {
				_fcloseall ();
				return false;
			} else {
				fwrite (m_buffer, m_iAppendpointer, 1, stream);
				_fcloseall ();
				return true;
			}			
		} else {
			if ((stream = fopen (lpszFilename, "w+b")) == NULL) {
				_fcloseall ();
				return false;
			} else {
				fwrite (m_buffer, m_iBuffersize, 1, stream);
				_fcloseall ();
				return true;
			}

		}
	} else {
		return false;
	}
}

bool MemoryBuffer::ReadFromFile (LPCSTR lpszFilename)
{
	FILE *stream2;
	int fh = 0;
	int filelen = 0;

	// Open a file
	if( _sopen_s(&fh, lpszFilename, _O_BINARY | _O_RDONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE ) == 0 )
	{
		filelen = _filelength( fh );
		_close( fh );

		if( (stream2 = fopen( lpszFilename, "r+b" )) == NULL ) {
			_fcloseall( );
			return false;
		} else {
			// Allocate the memory we need
			SetSize (filelen);

			fread( m_buffer, sizeof(BYTE), filelen, stream2);
			m_iAppendpointer = filelen;
			m_iBuffersize = filelen;

			_fcloseall( );
			return true;
		}
	    // All other files are closed:
	    _fcloseall( );
	} else {
		return false;
	}
}

bool MemoryBuffer::SwapBytes (unsigned long iIndex1, unsigned long iIndex2)
{
	BYTE bValue1;
	BYTE bValue2;

	if (m_bSizeset == true) {
		if (m_iAppendpointer > 0) {
			if (iIndex1 >= 0 && iIndex1 <=m_iAppendpointer) {
				if (iIndex2 >= 0 && iIndex2 <=m_iAppendpointer) {
					if (iIndex2 != iIndex1) {
						bValue1 = m_buffer[iIndex1];
						bValue2 = m_buffer[iIndex2];
						m_buffer[iIndex1] = bValue2;
						m_buffer[iIndex2] = bValue1;
						return true;
					} else {
						return false;
					}
				} else {
					return false;
				}
			} else {
				return false;
			}
		} else {
			if (iIndex1 >= 0 && iIndex1 <=m_iBuffersize) {
				if (iIndex2 >= 0 && iIndex2 <=m_iBuffersize) {
					if (iIndex2 != iIndex1) {
						bValue1 = m_buffer[iIndex1];
						bValue2 = m_buffer[iIndex2];
						m_buffer[iIndex1] = bValue2;
						m_buffer[iIndex2] = bValue1;
						return true;
					} else {
						return false;
					}
				} else {
					return false;
				}
			} else {
				return false;
			}
		}
	} else {
		return false;
	}
}

unsigned long MemoryBuffer::GetAppendPointer ()
{
	return m_iAppendpointer;
}

void *MemoryBuffer::GetBuffer ()
{
	return (void *) m_buffer;
}

bool MemoryBuffer::IsSizeset ()
{
	return m_bSizeset;
}

void MemoryBuffer::Clear ()
{
	if (m_bCleared == false) {
		m_bCleared = true;
		m_bSizeset = false;
		m_iBuffersize = 0;
		m_iAppendpointer = 0;
		// Free the buffer
		free (m_buffer);	
	}
}