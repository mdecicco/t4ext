#pragma once
#include <types.h>

namespace t4ext {
    template <typename T>
    struct ListNode {
        ListNode<T>* next;
        ListNode<T>* last;
        T value;
    };
};