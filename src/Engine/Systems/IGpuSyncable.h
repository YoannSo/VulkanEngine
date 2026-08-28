#pragma once
namespace engine
{
    class IGpuSyncable
    {
    public:
        virtual ~IGpuSyncable() = default;
        virtual void syncBuffer() = 0; // no-op en interne si rien n'a changé
    };
} // namespace engine

