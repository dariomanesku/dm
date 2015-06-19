/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

enum
{
    Invalid = 0xffff,
};

HandleTy alloc()
{
    DM_CHECK(m_numHandles < max(), "handleAllocAlloc | %d, %d", m_numHandles, max());

    if (m_numHandles < max())
    {
        const HandleTy index = m_numHandles++;
        const HandleTy handle = m_dense[index];
        m_sparse[handle] = index;
        return handle;
    }

    return Invalid;
}

bool contains(HandleTy _handle)
{
    DM_CHECK(_handle < max(), "handleAllocContains | %d, %d", _handle, max());

    HandleTy index = m_sparse[_handle];

    return (index < m_numHandles && m_dense[index] == _handle);
}

void free(HandleTy _handle)
{
    DM_CHECK(m_numHandles > 0, "handleAllocFree | %d", m_numHandles);

    HandleTy index = m_sparse[_handle];

    if (index < m_numHandles && m_dense[index] == _handle)
    {
        --m_numHandles;
        HandleTy temp = m_dense[m_numHandles];
        m_dense[m_numHandles] = _handle;
        m_sparse[temp] = index;
        m_dense[index] = temp;
    }
}

const HandleTy* getHandles() const
{
    return m_dense;
}

HandleTy getHandleAt(HandleTy _idx) const
{
    DM_CHECK(_idx < m_numHandles, "handleAllocGetHandleAt | %d %d", _idx, m_numHandles);

    return m_dense[_idx];
}

void reset()
{
    m_numHandles = 0;
    for (HandleTy ii = 0, end = max(); ii < end; ++ii)
    {
        m_dense[ii] = ii;
    }
}

/* vim: set sw=4 ts=4 expandtab: */
