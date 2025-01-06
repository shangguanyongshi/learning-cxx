#include "../exercise.h"
#include <cstring>

// READ: 类模板 <https://zh.cppreference.com/w/cpp/language/class_template>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;

    Tensor4D(unsigned int const shape_[4], T const *data_) {
        unsigned int size = 1;
        for (size_t i = 0; i < 4; ++i) {
            size *= shape_[i];
            shape[i] = shape_[i];
        }
        // TODO: 填入正确的 shape 并计算 size
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T));
    }
    ~Tensor4D() {
        delete[] data;
    }

    // 为了保持简单，禁止复制和移动
    Tensor4D(Tensor4D const &) = delete;
    Tensor4D(Tensor4D &&) noexcept = delete;

    // 这个加法需要支持“单向广播”。
    // 具体来说，`others` 可以具有与 `this` 不同的形状，形状不同的维度长度必须为 1。
    // `others` 长度为 1 但 `this` 长度不为 1 的维度将发生广播计算。
    // 例如，`this` 形状为 `[1, 2, 3, 4]`，`others` 形状为 `[1, 2, 1, 4]`，
    // 则 `this` 与 `others` 相加时，3 个形状为 `[1, 2, 1, 4]` 的子张量各自与 `others` 对应项相加。
    Tensor4D &operator+=(Tensor4D const &others) {
        // TODO: 实现单向广播的加法
        
        // 保存四维下标中第 i 维的值应该偏移的基数
        unsigned int offset_base_this[4] = {0, 0, 0, 1};
        unsigned int offset_base_other[4] = {0, 0, 0, 1};
        for (int i = 2; i >= 0; --i) {
            offset_base_this[i] = offset_base_this[i + 1] * shape[i + 1];
            offset_base_other[i] = offset_base_other[i + 1] * others.shape[i + 1];
            // std::cout << i << ": " << offset_base_this[i] << ", " << offset_base_other[i] << std::endl;
        }

        // 遍历比较 this 和 others 的 shape，并记录 others 中与 this 不同且为 1 的下标
        for (int i = 0; i < 4; ++i) {
            // std::cout << shape[i] << " " << others.shape[i] << std::endl;
            // 如果 others 中对应下标和 this 的下标不同，其偏移量设置为 0
            if (shape[i] != others.shape[i] && others.shape[i] == 1) {
                offset_base_other[i] = 0;
            }
        }
        std::cout << std::endl;
        // 遍历索引中的每一个元素，累加 others 中对应位置的值
        for (unsigned int a = 0; a < shape[0]; ++a) {
            for (unsigned int b = 0; b < shape[1]; ++b) {
                for (unsigned int c = 0; c < shape[2]; ++c) {
                    for (unsigned int d = 0; d < shape[3]; ++d) {
                        // 根据偏移量计算当前位置的元素在哪
                        int pos_this = a * offset_base_this[0] +
                                       b * offset_base_this[1] +
                                       c * offset_base_this[2] + 
                                       d * offset_base_this[3];
                        int pos_other = a * offset_base_other[0] +
                                        b * offset_base_other[1] +
                                        c * offset_base_other[2] + 
                                        d * offset_base_other[3];
                        // std::cout << a << "," << b << "," << c << "," << d << ": " << pos_this << " " << pos_other << std::endl;
                        data[pos_this] += others.data[pos_other];
                    }
                }
            }
        }

        return *this;
    }
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D(shape, data);
        auto t1 = Tensor4D(shape, data);
        t0 += t1;
        for (auto i = 0u; i < sizeof(data) / sizeof(*data); ++i) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }
    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D(s0, d0);
        auto t1 = Tensor4D(s1, d1);
        t0 += t1;
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}
