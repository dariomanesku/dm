/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef DM_NG_BITARRAY_HEADER_GUARD
#define DM_NG_BITARRAY_HEADER_GUARD

#include <stdint.h>
#include <dm/check.h>
#include <dm/ng/allocator.h>
#include <dm/ng/bitops.h>

namespace dm { namespace ng {

template <typename BitArrayStorageTy>
struct BitArrayImpl : BitArrayStorageTy
{
    /// Expected interface:
    ///
    ///     template <typename Ty>
    ///     struct BitArrayStorageTemplate
    ///     {
    ///         typedef Ty ElementType;
    ///         bool isResizable();
    ///         bool resize(uint32_t _max);
    ///         Ty* elements();
    ///         uint32_t max();
    ///     };
    using BitArrayStorageTy::bits;
    using BitArrayStorageTy::numSlots;
    using BitArrayStorageTy::max;

    BitArrayImpl() : BitArrayStorageTy()
    {
    }

    static uint64_t markFirstUnsetBit(uint64_t _v)
    {
        const uint64_t val = _v;      // ..010111
        const uint64_t neg = ~val;    // ..101000
        const uint64_t add = val+1;   // ..011000
        const uint64_t bit = neg&add; // ..001000

        return bit;
    }

    void set(uint32_t _bit)
    {
        DM_CHECK(_bit < max(), "BitArray::set() | %d, %d", _bit, max());

        const uint32_t bucket = _bit>>6;
        const uint64_t bit    = UINT64_C(1)<<(_bit&63);
        bits()[bucket] |= bit;
    }

    void unset(uint32_t _bit)
    {
        DM_CHECK(_bit < max(), "BitArray::unset() | %d, %d", _bit, max());

        const uint32_t bucket = _bit>>6;
        const uint64_t bit    = UINT64_C(1)<<(_bit&63);
        bits()[bucket] &= ~bit;
    }

    void toggle(uint32_t _bit)
    {
        DM_CHECK(_bit < max(), "BitArray::toggle() | %d, %d", _bit, max());

        const uint32_t bucket = _bit>>6;
        const uint64_t bit    = UINT64_C(1)<<(_bit&63);
        bits()[bucket] ^= bit;
    }

    bool isSet(uint32_t _bit)
    {
        DM_CHECK(_bit < max(), "BitArray::isSet() | %d, %d", _bit, max());

        const uint32_t bucket = _bit>>6;
        const uint64_t bit    = UINT64_C(1)<<(_bit&63);
        return (0 != (bits()[bucket] & bit));
    }

    private:
    inline uint32_t setRightmostBit(uint32_t _slot)
    {
        const uint64_t rightmost = bits()[_slot]+1;
        bits()[_slot] |= rightmost;

        const uint32_t pos = uint32_t(cnttz_u64(rightmost));
        const uint32_t idx = (_slot<<6)+pos;
        const uint32_t maxIdx = this->max();
        return idx < maxIdx ? idx : maxIdx;
    }
    public:

    uint32_t setFirst()
    {
        for (uint32_t slot = 0, end = numSlots(); slot < end; ++slot)
        {
            const bool hasUnsetBits = (bits()[slot] != UINT64_MAX);
            if (hasUnsetBits)
            {
                return setRightmostBit(slot);
            }
        }

        return max();
    }

    uint32_t setAny()
    {
        const uint32_t begin = m_last;
        const uint32_t count = numSlots();

        for (uint32_t slot = begin, end = numSlots(); slot < end; ++slot)
        {
            const bool hasUnsetBits = (bits()[slot] != UINT64_MAX);
            if (hasUnsetBits)
            {
                return setRightmostBit(slot);
            }
            else
            {
                m_last = slot+1;
            }
        }

        m_last = (m_last >= count) ? 0 : m_last;

        for (uint32_t slot = 0, end = begin; slot < end; ++slot)
        {
            const bool hasUnsetBits = (bits()[slot] != UINT64_MAX);
            if (hasUnsetBits)
            {
                return setRightmostBit(slot);
            }
            else
            {
                m_last = slot+1;
            }
        }

        return max();
    }

    /// Returns max() if none set.
    uint32_t getFirstSetBit()
    {
        for (uint32_t ii = 0, end = numSlots(); ii < end; ++ii)
        {
            const bool hasSetBits = (0 != bits()[ii]);
            if (hasSetBits)
            {
                const uint32_t pos = uint32_t(cnttz_u64(bits()[ii]));
                return (ii<<6)+pos;
            }
        }

        return max();
    }

    uint32_t getFirstUnsetBit()
    {
        for (uint32_t ii = 0, end = numSlots(); ii < end; ++ii)
        {
            const bool hasUnsetBits = (bits()[ii] != UINT64_MAX);
            if (hasUnsetBits)
            {
                const uint64_t sel = markFirstUnsetBit(bits()[ii]);
                const uint32_t pos = uint32_t(cnttz_u64(sel));
                return (ii<<6)+pos;
            }
        }

        return max();
    }

    /// Returns max() if none set.
    uint32_t getLastSetBit()
    {
        for (uint32_t ii = numSlots(); ii--; )
        {
            const bool hasSetBits = (0 != bits()[ii]);
            if (hasSetBits)
            {
                const bool allSet = (UINT64_MAX == bits()[ii]);
                if (allSet)
                {
                    return (ii+1)<<6;
                }
                else
                {
                    const uint64_t sel = markFirstUnsetBit(bits()[ii]);
                    const uint64_t leading = cntlz_u64(sel);
                    const uint32_t pos = 63-uint32_t(leading);
                    return ((ii)<<6)+pos;
                }
            }
        }

        return 0;
    }

    uint32_t getLastUnsetBit()
    {
        for (uint32_t ii = numSlots(); ii--; )
        {
            const bool hasSetBits = (0 != bits()[ii]);
            if (hasSetBits)
            {
                const uint64_t leading = cntlz_u64(bits()[ii]);
                const uint32_t pos = 63-uint32_t(leading);
                return (ii<<6)+pos;
            }
        }

        return max();
    }

    uint32_t doCount()
    {
        uint64_t count = 0;
        for (uint32_t ii = numSlots(); ii--; )
        {
            const uint64_t curr = bits()[ii];
            count += cntbits_u64(curr);
        }

        return uint32_t(count);
    }

    void reset() // Notice: Has to be called on initialization.
    {
        m_last = 0;
        memset(bits(), 0, numSlots()*sizeof(uint64_t));
    }

private:
    uint32_t m_last;
};

template <uint32_t MaxT>
struct BitArrayStorageT
{
    uint64_t* bits()
    {
        return m_bits;
    }

    uint32_t numSlots()
    {
        return NumSlots;
    }

    uint32_t max()
    {
        return MaxT;
    }

    enum { NumSlots = ((MaxT-1)>>6)+1 };
    uint64_t m_bits[MaxT];
};

struct BitArrayStorageExt
{
    static inline uint32_t numSlotsFor(uint32_t _max)
    {
        return ((_max-1)>>6) + 1;
    }

    static inline uint32_t sizeFor(uint32_t _max)
    {
        return numSlotsFor(_max)*sizeof(uint64_t);
    }

    BitArrayStorageExt()
    {
        m_bits = NULL;
        m_numSlots = 0;
        m_max = 0;
    }

    uint8_t* init(uint32_t _max, uint8_t* _mem)
    {
        m_bits = (uint64_t*)_mem;
        m_numSlots = numSlotsFor(_max);
        m_max = _max;

        return ((uint8_t*)_mem + sizeFor(_max));
    }

    uint64_t* bits()
    {
        return m_bits;
    }

    uint32_t numSlots()
    {
        return m_numSlots;
    }

    uint32_t max()
    {
        return m_max;
    }

    uint64_t* m_bits;
    uint32_t m_numSlots;
    uint32_t m_max;
};

struct BitArrayStorage
{
    static inline uint32_t numSlotsFor(uint32_t _max)
    {
        return ((_max-1)>>6) + 1;
    }

    static inline uint32_t sizeFor(uint32_t _max)
    {
        return numSlotsFor(_max)*sizeof(uint64_t);
    }

    BitArrayStorage()
    {
        m_bits = NULL;
        m_numSlots = 0;
        m_max = 0;
    }

    void init(uint32_t _max, ReallocFn _reallocFn = &::realloc)
    {
        void* mem = dm_alloc(sizeFor(_max), _reallocFn);

        m_bits = (uint64_t*)mem;
        m_numSlots = numSlotsFor(_max);
        m_max = _max;

        m_reallocFn = _reallocFn;
    }

    void destroy()
    {
        if (NULL != m_bits)
        {
            dm_free(m_bits, m_reallocFn);
            m_bits = NULL;
        }
    }

    uint64_t* bits()
    {
        return m_bits;
    }

    uint32_t numSlots()
    {
        return m_numSlots;
    }

    uint32_t max()
    {
        return m_max;
    }

    uint64_t* m_bits;
    uint32_t m_numSlots;
    uint32_t m_max;
    ReallocFn m_reallocFn;
};

template <uint32_t MaxTy>
struct BitArrayT : BitArrayImpl<BitArrayStorageT<MaxTy> >
{
    typedef BitArrayImpl<BitArrayStorageT<MaxTy> > Base;

    BitArrayT()
    {
        Base::reset();
    }
};

struct BitArrayExt : BitArrayImpl<BitArrayStorageExt>
{
    typedef BitArrayImpl<BitArrayStorageExt> Base;

    uint8_t* init(uint32_t _max, uint8_t* _mem)
    {
        uint8_t* ptr = Base::init(_max, _mem);
        Base::reset();

        return ptr;
    }
};

struct BitArray : BitArrayImpl<BitArrayStorage>
{
    typedef BitArrayImpl<BitArrayStorage> Base;

    void init(uint32_t _max, ReallocFn _reallocFn = &::realloc)
    {
        Base::init(_max, _reallocFn);
        Base::reset();
    }
};

struct BitArrayH : BitArrayExt
{
    ReallocFn m_reallocFn;
};

} //namespace ng
} //namespace dm

#endif // DM_NG_BITARRAY_HEADER_GUARD

/* vim: set sw=4 ts=4 expandtab: */
