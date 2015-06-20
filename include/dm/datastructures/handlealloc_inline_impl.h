/*
 * Copyright 2014-2015 Dario Manesku. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

static HandleType invalid()
{
    return TyInfo<HandleType>::Max();
}

HandleType alloc()
{
    DM_CHECK(m_numHandles < max(), "handleAllocAlloc | %d, %d", m_numHandles, max());

    if (m_numHandles < max())
    {
        const HandleType index = m_numHandles++;
        const HandleType handle = m_handles[index];
        m_indices[handle] = index;
        return handle;
    }

    return invalid();
}

bool contains(HandleType _handle)
{
    DM_CHECK(_handle < max(), "handleAllocContains | %d, %d", _handle, max());

    HandleType index = m_indices[_handle];

    return (index < m_numHandles && m_handles[index] == _handle);
}

void free(HandleType _handle)
{
    DM_CHECK(m_numHandles > 0, "handleAllocFree | %d", m_numHandles);

    HandleType index = m_indices[_handle];

    if (index < m_numHandles && m_handles[index] == _handle)
    {
        --m_numHandles;
        HandleType temp = m_handles[m_numHandles];
        m_handles[m_numHandles] = _handle;
        m_indices[temp] = index;
        m_handles[index] = temp;
    }
}

const HandleType* getHandles() const
{
    return m_handles;
}

HandleType getHandleAt(HandleType _idx) const
{
    DM_CHECK(_idx < m_numHandles, "handleAllocGetHandleAt | %d %d", _idx, m_numHandles);

    return m_handles[_idx];
}

HandleType getIdxOf(HandleType _handle) const
{
    DM_CHECK(_handle < max(), "handleAllocGetIdxOf | %d %d", _handle, max());

    return m_indices[_handle];
}

void reset()
{
    m_numHandles = 0;
    for (HandleType ii = 0, end = max(); ii < end; ++ii)
    {
        m_handles[ii] = ii;
    }
}

/* vim: set sw=4 ts=4 expandtab: */
