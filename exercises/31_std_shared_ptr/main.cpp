#include "../exercise.h"
#include <memory>

// READ: `std::shared_ptr` <https://zh.cppreference.com/w/cpp/memory/shared_ptr>
// READ: `std::weak_ptr` <https://zh.cppreference.com/w/cpp/memory/weak_ptr>

// TODO: 将下列 `?` 替换为正确的值
int main(int argc, char **argv) {
    auto shared = std::make_shared<int>(10);
    std::shared_ptr<int> ptrs[]{shared, shared, shared};

    std::weak_ptr<int> observer = shared;
    ASSERT(observer.use_count() == 4, "");

    ptrs[0].reset();
    ASSERT(observer.use_count() == 3, "");

    ptrs[1] = nullptr;
    ASSERT(observer.use_count() == 2, "");

    ptrs[2] = std::make_shared<int>(*shared);
    ASSERT(observer.use_count() == 1, "");

    ptrs[0] = shared;
    ptrs[1] = shared;
    ptrs[2] = std::move(shared); // 释放 shared，将所有信息转移给 ptrs[2]
    // std::cout << ptrs[0].use_count() << " " << ptrs[2].use_count() << std::endl;
    ASSERT(observer.use_count() == 3, "");

    std::ignore = std::move(ptrs[0]);  // 释放 ptrs[0] 的相关信息
    ptrs[1] = std::move(ptrs[1]);  // 转移 ptrs[1] 到 ptrs[1]
    ptrs[1] = std::move(ptrs[2]);  // 转移 ptrs[2] 到 ptrs[2]
    ASSERT(observer.use_count() == 2, "");

    shared = observer.lock();  // shared 重新获取原来的信息
    ASSERT(observer.use_count() == 3, "");

    shared = nullptr;  // 释放 shared
    for (auto &ptr : ptrs) ptr = nullptr;
    ASSERT(observer.use_count() == 0, "");

    shared = observer.lock();
    ASSERT(observer.use_count() == 0, "");

    return 0;
}
