#pragma once

#include <sdk-meta/cond.h>
#include <sdk-meta/iter.h>
#include <sdk-meta/opt.h>
#include <sdk-meta/traits.h>
#include <sdk-meta/types.h>

template <typename T>
struct LinkedTrait {
    T* _next;
    T* _prev;

    [[gnu::always_inline]] constexpr bool operator==(
        LinkedTrait const& other) const {
        return this == &other;
    }
};

template <typename T>
concept ILinked = Meta::Extends<T, LinkedTrait<T>>;

namespace _ {
template <typename T>
struct Node {
    T        _value;
    Node<T>* _next;
    Node<T>* _prev;
};
} // namespace _

template <typename T>
class List {
    struct Node {
        Node *_next, *_prev;
        T     _value;
    };
    using Inner = T;
    using Elem  = Cond<ILinked<T>, T, Node>;

    Elem* _head;
    Elem* _tail;
    usize _count;

    static constexpr Elem* wrap(T const& value) {
        if constexpr (ILinked<T>) {
            return &value;
        } else {
            return new Elem { value, nullptr, nullptr };
        }
    }

    static constexpr T const& unwrap(Elem const* elem) {
        if constexpr (ILinked<T>) {
            return *elem;
        } else {
            return elem->_value;
        }
    }

public:
    constexpr List() noexcept : _head(nullptr), _tail(nullptr), _count(0) { }

    template <Meta::CopyConstructible<T> U>
    List(List<U> const& other) : _head(nullptr),
                                 _tail(nullptr),
                                 _count(0) { }

    template <Meta::MoveConstructible<T> U>
    List(List<U>&& other) noexcept {
        clear();
        ::swap(_head, other._head);
        ::swap(_tail, other._tail);
        ::swap(_count, other._count);
    }

    ~List() { clear(); }

    Elem* head() const { return _head; }

    Elem* tail() const { return _tail; }

    // MARK: - Iterators

    // MARK: - Capacity

    [[gnu::always_inline]] bool isEmpty() const { return _count == 0; }

    [[gnu::always_inline]] usize size() const { return _count; }

    // MARK: - Modifiers

    [[clang::always_inline]] void clear() {
        while (_head != nullptr) {
            Elem* n = _head->_next;
            delete _head;
            _head = n;
        }

        _tail  = nullptr;
        _count = 0;
    }

    [[clang::always_inline]] void insert(usize index, T const& value) { }

    template <typename... Args>
    [[clang::always_inline]] void emplace(Args&&... args) {
        Elem* n = new Elem { T(::forward<Args>(args)...) };

        if (_head == NONE)
            _head = _tail = n;
        else {
            n->_prev     = _tail;
            _tail->_next = n;
            _tail        = n;
        }
        n->_next = nullptr;

        _count++;
    }

    template <typename... Args>
    [[clang::always_inline]] void emplaceAt(usize index, Args&&... args) { }

    [[clang::always_inline]] void append(T const& value) {
        auto* n = wrap(value);

        if (_head == NONE)
            _head = _tail = n;
        else {
            n->_prev     = _tail;
            _tail->_next = n;
            _tail        = n;
        }
        n->_next = nullptr;

        _count++;
    }

    [[clang::always_inline]] void remove(usize index) {
        if (index >= size()) {
            // Handle out-of-bounds index
        }

        Elem* curr = head();
        for (usize i = 0; i < index; ++i) {
            curr = next(curr);
        }

        if (curr->_prev != NONE)
            curr->_prev->_next = curr->_next;
        else
            _head = curr->_next;

        if (curr->_next != NONE)
            curr->_next->_prev = curr->_prev;
        else
            _tail = curr->_prev;

        delete curr;
        _count--;
    }

    [[clang::always_inline]] void remove(T const& value) {
        auto* curr = head();

        while (curr != NONE) {
            if (unwrap(curr) != value) {
                curr = curr->_next;
                continue;
            }

            if (curr->_prev != NONE)
                curr->_prev->_next = curr->_next;
            else
                _head = curr->_next;

            if (curr->_next != NONE)
                curr->_next->_prev = curr->_prev;
            else
                _tail = curr->_prev;

            delete curr;
            _count--;
            return;
        }
    }

    [[clang::always_inline]] Opt<T&> at(usize index) {
        if (index >= size()) {
            return NONE;
        }

        Elem* curr = head();
        for (usize i = 0; i < index; ++i) {
            curr = next(curr);
        }
        return unwrap(curr);
    }

    [[clang::always_inline]] void operator+=(T const& value) { }

    [[clang::always_inline]] void operator-=(T const& value) { }

    [[clang::always_inline]] T& operator[](usize index) {
        if (index >= size()) {
            panic("List::operator[]: index out of bounds.");
        }

        Elem* curr = head();
        for (usize i = 0; i < index; ++i) {
            curr = curr->_next;
        }
        return unwrap(curr);
    }

    [[clang::always_inline]] T const& operator[](usize index) const {
        if (index >= size()) {
            panic("List::operator[]: index out of bounds.");
        }

        Elem* curr = head();
        for (usize i = 0; i < index; ++i) {
            curr = curr->_next;
        }
        return unwrap(curr);
    }

    struct It {
        List const& list;
        Elem*       curr;

        constexpr It(List const& lst) : list(lst), curr(lst.head()) { }
    };
};
