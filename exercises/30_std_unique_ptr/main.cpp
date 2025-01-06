#include "../exercise.h"
#include <cstring>
#include <memory>
#include <string>
#include <vector>

// READ: `std::unique_ptr` <https://zh.cppreference.com/w/cpp/memory/unique_ptr>

std::vector<std::string> RECORDS;

class Resource {
    std::string _records;

public:
    void record(char record) {
        _records.push_back(record);
    }

    ~Resource() {
        RECORDS.push_back(_records);
    }
};

using Unique = std::unique_ptr<Resource>;
Unique reset(Unique ptr) {
    // 如果非空，记录一个 r
    if (ptr) ptr->record('r');
    // 如果非空，释放 ptr
    // 返回一个 空的unique_ptr
    return std::make_unique<Resource>();
}
Unique drop(Unique ptr) {
    // 记录一个 d
    if (ptr) ptr->record('d');
    // 释放 ptr
    // 返回一个 空指针
    return nullptr;
}
Unique forward(Unique ptr) {
    // 记录一个 f
    if (ptr) ptr->record('f');
    // 不释放，继续返回 ptr
    return ptr;
}

int main(int argc, char **argv) {
    std::vector<std::string> problems[3];

    drop(forward(reset(nullptr)));
    problems[0] = std::move(RECORDS);
    // 1. reset 创建空的 ptr1
    // 2. forward 向 ptr1 中追加 f
    // 3. forward 向 ptr1 中追加 f
    // 4. reset 向 ptr1 中追加 r，释放 ptr1(ffr)，返回 ptr2
    // 5. drop 向 ptr2 中追加 d，释放 ptr2(d)，返回 nullptr
    // 6. forward 接收 nullptr，不做任何操作，返回 nullptr
    // 结果：如果临时量是先创建的后释放，结果就是 dffr
    forward(drop(reset(forward(forward(reset(nullptr))))));
    problems[1] = std::move(RECORDS);

    // 1. reset 创建空的 ptr1
    // 2. reset 向 ptr1 中追加 r，释放 ptr1(r)，返回 ptr2
    // 3. drop 向 ptr2 中追加 d，释放 ptr2(d)，返回 nullptr
    // 4. reset 返回 ptr3
    // 5. drop 向 ptr3 中追加 d，释放 ptr3(d)，返回 nullptr
    // 6. drop 不做任何操作，返回 nullptr
    // 结果：如果临时量是先创建的后释放，结果就是 ddr
    drop(drop(reset(drop(reset(reset(nullptr))))));
    problems[2] = std::move(RECORDS);

    // ---- 不要修改以上代码 ----

    std::vector<const char *> answers[]{
        {"fd"},
        // TODO: 分析 problems[1] 中资源的生命周期，将记录填入 `std::vector`
        // NOTICE: 此题结果依赖对象析构逻辑，平台相关，提交时以 CI 实际运行平台为准
        {"d", "ffr"},
        {"d", "d", "r"},
    };

    // ---- 不要修改以下代码 ----

    for (auto i = 0; i < 3; ++i) {
        ASSERT(problems[i].size() == answers[i].size(), "wrong size");
        for (auto j = 0; j < problems[i].size(); ++j) {
            // std::cout << problems[i][j];
            ASSERT(std::strcmp(problems[i][j].c_str(), answers[i][j]) == 0, "wrong location");
        }
    }

    return 0;
}
