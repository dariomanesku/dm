/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include "dm.h"

/// Header includes.
#if (DM_INCL & DM_INCL_HEADER_INCLUDES)
    #include <stdio.h>
    #include <stdint.h>
#endif // (DM_INCL & DM_INCL_HEADER_INCLUDES)

/// Header body.
#if (DM_INCL & DM_INCL_HEADER_BODY)
#   if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#       undef DM_RW_H_HEADER_GUARD
#   endif // if (DM_INCL & DM_INCL_HEADER_BODY_OPT_REMOVE_HEADER_GUARD)
#   ifndef DM_RW_H_HEADER_GUARD
#   define DM_RW_H_HEADER_GUARD
namespace DM_NAMESPACE
{
    struct RwError
    {
        enum Enum
        {
            None,
            Open,
            Read,
            Write,
            Eof,
        };
    };

    struct RwType
    {
        enum Enum
        {
            Memory,
            FilePath,
        };
    };

    struct Rw
    {
        uint8_t m_error;
        uint8_t m_type;
        union
        {
            struct
            {
                const char* m_path;
                FILE* m_file;
            };

            struct
            {
                void* m_mem;
                size_t m_size;
                size_t m_offset;
            };
        };
    };

    struct Whence
    {
        enum Enum
        {
            Begin,
            Current,
            End,
        };
    };

    void     rwInit(Rw* _rw, const char* _path);
    void     rwInit(Rw* _rw, FILE* _file);
    void     rwInit(Rw* _rw, void* _mem, size_t _size);
    bool     rwFileOpen(Rw* _rw, const char* _mode = "rb");
    bool     rwFileOpened(const Rw* _rw);
    void     rwFileClose(Rw* _rw);
    uint8_t  rwGetError(Rw* _rw);
    void     rwClearError(Rw* _rw);

    typedef int64_t (*RwSeekFn)(Rw* _rw, int64_t _offset, Whence::Enum _whence);
    typedef size_t  (*RwReadFn)(Rw* _rw, void* _data, size_t _size);

    RwSeekFn rwSeekFnFor(const Rw* _rw);
    RwReadFn rwReadFnFor(const Rw* _rw);

    struct RwScopeFileClose
    {
        RwScopeFileClose(Rw* _rw, bool _condition = true)
        {
            m_rw = _rw;
            m_condition = _condition;
        }

        ~RwScopeFileClose()
        {
            if (m_condition)
            {
                rwFileClose(m_rw);
            }
        }
    private:
        Rw* m_rw;
        bool m_condition;
    };

} // namespace DM_NAMESPACE
#   endif // DM_RW_H_HEADER_GUARD
#endif // (DM_INCL & DM_INCL_HEADER_BODY)

/// Impl includes.
#if (DM_INCL & DM_INCL_IMPL_INCLUDES)
    #include <stdio.h>  // fopen
    #include <stdint.h>
    #include <string.h> // memcpy
#endif // (DM_INCL & DM_INCL_IMPL_INCLUDES)

/// Impl body.
#if (DM_INCL & DM_INCL_IMPL_BODY)
namespace DM_NAMESPACE
{
    void rwInit(Rw* _rw, void* _mem, size_t _size)
    {
        _rw->m_error = RwError::None;
        _rw->m_type  = RwType::Memory;
        _rw->m_mem   = _mem;
        _rw->m_size  = _size;
    }

    void rwInit(Rw* _rw, FILE* _file)
    {
        _rw->m_error = RwError::None;
        _rw->m_type  = RwType::FilePath;
        _rw->m_path  = NULL;
        _rw->m_file  = _file;
    }

    void rwInit(Rw* _rw, const char* _path)
    {
        _rw->m_error = RwError::None;
        _rw->m_type  = RwType::FilePath;
        _rw->m_path  = _path;
        _rw->m_file  = NULL;
    }

    bool rwFileOpen(Rw* _rw, const char* _mode)
    {
        if (RwType::FilePath == _rw->m_type
        &&  NULL == _rw->m_file)
        {
            FILE* file = fopen(_rw->m_path, _mode);
            if (NULL != file)
            {
                _rw->m_error = 0;
                _rw->m_file  = file;

                return true;
            }
            else
            {
                _rw->m_error = RwError::Open; // Error opening file.

                return false;
            }
        }

        return false;
    }

    bool rwFileOpened(const Rw* _rw)
    {
        return (NULL != _rw->m_file);
    }

    void rwFileClose(Rw* _rw)
    {
        if (NULL != _rw->m_file)
        {
            int result = fclose(_rw->m_file);
            if (0 == result)
            {
                _rw->m_file = NULL;
            }
            else
            {
                _rw->m_error = RwError::Eof; // Error closing file.
            }
        }
    }

    uint8_t rwGetError(Rw* _rw)
    {
        return _rw->m_error;
    }

    void rwClearError(Rw* _rw)
    {
        _rw->m_error = 0;
    }

    #if CMFT_COMPILER_MSVC
    #   ifndef fseeko64
    #      define fseeko64 _fseeki64
    #   endif // fseeko64
    #   ifndef ftello64
    #      define ftello64 _fseeki64
    #   endif // ftello64
    #elif CMFT_PLATFORM_APPLE
    #   ifndef fseeko64
    #      define fseeko64 fseeko
    #   endif // fseeko64
    #   ifndef ftello64
    #      define ftello64 ftello
    #   endif // ftello64
    #endif // CMFT_COMPILER

    int64_t rwSeekFile(Rw* _rw, int64_t _offset = 0, Whence::Enum _whence = Whence::Current)
    {
        fseeko64(_rw->m_file, _offset, _whence);
        return ftello64(_rw->m_file);
    }

    int64_t rwSeekMem(Rw* _rw, int64_t _offset = 0, Whence::Enum _whence = Whence::Current)
    {
        int64_t offset;
        if (Whence::Begin == _whence)
        {
            offset = _offset;
        }
        else if (Whence::Current == _whence)
        {
            offset = int64_t(_rw->m_offset) + _offset;
        }
        else /*.if (Whence::End == _whence).*/
        {
            offset = int64_t(_rw->m_size) - _offset;
        }
        offset = offset > 0 ? offset : 0;
        offset = offset < int64_t(_rw->m_size) ? offset : int64_t(_rw->m_size);

        _rw->m_offset = offset;

        return offset;
    }

    RwSeekFn rwSeekFnFor(const Rw* _rw)
    {
        if (RwType::Memory == _rw->m_type)
        {
            return rwSeekMem;
        }
        else
        {
            return rwSeekFile;
        }
    }

    size_t rwReadFile(Rw* _src, void* _data, size_t _size)
    {
        const size_t size = fread(_data, 1, _size, _src->m_file);
        if (size != _size)
        {
            if (0 != feof(_src->m_file))
            {
                _src->m_error = RwError::Eof;
            }
            else if (0 != ferror(_src->m_file))
            {
                _src->m_error = RwError::Read;
            }
        }

        return size;
    }

    size_t rwReadMem(Rw* _rw, void* _data, size_t _size)
    {
        const size_t remainder  = _rw->m_size - _rw->m_offset;
        const size_t sizeToRead = _size < remainder ? _size : remainder;
        if (_size != sizeToRead)
        {
            _rw->m_error = RwError::Read; // Size truncated.
        }
        _rw->m_offset += sizeToRead;

        memcpy(_data, _rw->m_mem, sizeToRead);

        return sizeToRead;
    }

    RwReadFn rwReadFnFor(const Rw* _rw)
    {
        if (RwType::Memory == _rw->m_type)
        {
            return rwReadMem;
        }
        else
        {
            return rwReadFile;
        }
    }
} // namespace DM_NAMESPACE
#endif // (DM_INCL & DM_INCL_IMPL_BODY)

/* vim: set sw=4 ts=4 expandtab: */
