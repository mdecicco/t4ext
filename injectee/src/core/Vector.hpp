#pragma once
#include <types.h>

namespace t4ext {
    template <typename T>
    class Vector {
        public:
            u32 size() {
                return m_last - m_first;
            }

            u32 capacity() {
                return m_end - m_first;
            }

            T& operator[](u32 idx) {
                return m_first[idx];
            }

            T& last() {
                return m_last[-1];
            }

        protected:
            T* m_first;
            T* m_last;
            T* m_end;
    };
};