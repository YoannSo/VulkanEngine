#pragma once
namespace engine
{
    class DirtyFlag
    {
    public:
        void markDirty()
        {
            m_dirty = true;
        }
        bool consume()
        {
            bool wasDirty = m_dirty;
            m_dirty = false;
            return wasDirty;
        }

    private:
        bool m_dirty{false};
    };
} // namespace engine

