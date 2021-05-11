/*
 * @author: shenke
 * @date: 2021/4/14
 * @project: ZTE-DB-2021
 * @desp:
 */

#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define JUDGE
// #define TEST
// #define FIRST_STAGE

#ifdef TEST
#ifdef __linux
#define OUTPUT_FILE "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/output.txt"
#define LOG_FILE "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/log.txt"
#else
#define OUTPUT_FILE "D:/GitHub Repository/ZTE-DB-2021/preliminary/output.txt"
#define LOG_FILE "D:/GitHub Repository/ZTE-DB-2021/preliminary/log.txt"
#endif
#endif


class Handler {
public:
    Handler(std::string datasetA, std::string datasetB);

    void Init();

    void Input();

    void Handle();

    void Output();

    void Release();

private:

    void Handle(const uint32_t &valueId1, const uint32_t &valueId2, const uint32_t &valueId3);

    uint32_t TakeId(const char *begin, const uint16_t &size);

    static uint32_t HashCode(const char *begin, const uint16_t &size);

    static inline uint32_t HashCode(const uint64_t &num);

    static bool IsEqual(const char *value1, const char *value2);

    inline void AddLocationA(const uint32_t &id, const uint32_t &loc);

    inline void AddLocationB(const uint32_t &id, const uint32_t &loc);

    static inline bool IsExist(const uint32_t &loc);

    static inline bool IsExistA(const uint32_t &loc);

    static inline bool IsExistB(const uint32_t &loc);

    static inline uint64_t GetId(const uint32_t &valueIdx1, const uint32_t &valueIdx2, const uint32_t &valueIdx3);

    inline char *GetLocPtr(const uint32_t &loc);

    void GetValue(const uint32_t &loc, char value[]);

    static void GetValue(const char *begin, char value[]);

    void PrintLog(const std::string &log);

    // const static uint64_t MAX_ID = 2097143u;    // 小于 2^21 - 1 (2097151) 的最大素数
    const static uint32_t MAX_ID = 0x1fffffu;    // 2097151

    // TODO: 开 40w 过不了，后续开大点或者改成 vector
    const static uint32_t MAX_COUNT_A = 800000u;
    const static uint32_t MAX_COUNT_B = 800000u;

    std::string datasetA, datasetB;
    char *bufferA{}, *bufferB{};
    uint32_t sizeA{0u}, sizeB{0u};

    // std::bitset<MAX_ID + 1u> overflowFlag;  // 1: 该 id 已冲突, 0: 该 id 未使用或者未冲突
    uint32_t *location = new uint32_t[MAX_ID + 1u];

    uint32_t countB{0u};
    uint32_t resultCount{0u};
    uint64_t *datasetBId = new uint64_t[MAX_COUNT_B];
    uint64_t *resultId = new uint64_t[MAX_COUNT_A];

#ifdef TEST
    std::ofstream LOG;
    std::chrono::system_clock::time_point start;
#endif
};


Handler::Handler(std::string datasetA, std::string datasetB) : datasetA(std::move(datasetA)), datasetB(std::move(datasetB)) {
}

void Handler::Init() {
#ifdef TEST
    // 清空 log.txt 和 output.txt
    struct stat output{};
    if (stat(OUTPUT_FILE, &output) == 0) {
        std::fstream file(OUTPUT_FILE, std::ios::out);
    }

    struct stat log{};
    if (stat(LOG_FILE, &log) == 0) {
        std::fstream file(LOG_FILE, std::ios::out);
    }

    LOG.open(LOG_FILE, std::ios::app);
    freopen(OUTPUT_FILE, "w", stdout);

    start = std::chrono::system_clock::now();
#endif

    int fd1 = open(datasetA.c_str(), O_RDONLY);
    sizeA = lseek(fd1, 0, SEEK_END);
    bufferA = (char *) mmap(nullptr, sizeA, PROT_READ, MAP_PRIVATE, fd1, 0);
    close(fd1);

    int fd2 = open(datasetB.c_str(), O_RDONLY);
    sizeB = lseek(fd2, 0, SEEK_END);
    bufferB = (char *) mmap(nullptr, sizeB, PROT_READ, MAP_PRIVATE, fd2, 0);
    close(fd2);
}

void Handler::Input() {
    uint32_t idx = 0u, loc;
    uint32_t valueId1, valueId2, valueId3;

    while (idx < sizeB) {
        while (bufferB[idx] != '\"') ++idx;

        loc = ++idx;
        while (bufferB[idx] != '\"') ++idx;
        valueId1 = TakeId(&bufferB[loc], idx - loc);
        AddLocationB(valueId1, loc);

        loc = (idx += 3u);
        while (bufferB[idx] != '\"') ++idx;
        valueId2 = TakeId(&bufferB[loc], idx - loc);
        AddLocationB(valueId2, loc);

        loc = (idx += 3u);
        while (bufferB[idx] != '\"') ++idx;
        valueId3 = TakeId(&bufferB[loc], idx - loc);
        AddLocationB(valueId3, loc);

        while (idx < sizeB && bufferB[idx] != '\n') ++idx;
        ++idx;

        datasetBId[countB++] = GetId(valueId1, valueId2, valueId3);
    }

    // 排序 datasetBId，方便后面二分查找
    std::sort(datasetBId, datasetBId + countB);
}

void Handler::Handle() {
    uint32_t idx = 0u, loc;
    uint32_t valueId1, valueId2, valueId3;

    while (idx < sizeA) {
        while (bufferA[idx] != '\"') ++idx;

        loc = ++idx;
        while (bufferA[idx] != '\"') ++idx;
        valueId1 = TakeId(&bufferA[loc], idx - loc);
        AddLocationA(valueId1, loc);

        loc = (idx += 3u);
        while (bufferA[idx] != '\"') ++idx;
        valueId2 = TakeId(&bufferA[loc], idx - loc);
        AddLocationA(valueId2, loc);

        loc = (idx += 3u);
        while (bufferA[idx] != '\"') ++idx;
        valueId3 = TakeId(&bufferA[loc], idx - loc);
        AddLocationA(valueId3, loc);

        while (idx < sizeA && bufferA[idx] != '\n') ++idx;
        ++idx;

        Handle(valueId1, valueId2, valueId3);
    }
}

void Handler::Output() {
    // 排序，方便去重
    std::sort(resultId, resultId + resultCount);

    uint64_t id;
    uint32_t idx = 0u;
    char valueBuffer[3][1u << 10u];

    while (idx < resultCount) {
        id = resultId[idx++];
        GetValue(location[(id >> 42u) & 0x1fffffu], valueBuffer[0]);
        GetValue(location[(id >> 21u) & 0x1fffffu], valueBuffer[1]);
        GetValue(location[id & 0x1fffffu], valueBuffer[2]);

        printf("\"%s\",\"%s\",\"%s\"\n", valueBuffer[0], valueBuffer[1], valueBuffer[2]);

        while (idx < resultCount && id == resultId[idx]) ++idx;
    }

}

void Handler::Release() {
#ifdef TEST
    std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - start;
    PrintLog("Elapsed time: " + std::to_string(duration.count()) + "ms");

    fclose(stdout);
    LOG.close();
#endif

    munmap(bufferA, sizeA);
    munmap(bufferB, sizeB);
}

void Handler::Handle(const uint32_t &valueId1, const uint32_t &valueId2, const uint32_t &valueId3) {
    uint64_t id = GetId(valueId1, valueId2, valueId3);
    if (IsExistB(location[valueId1]) && IsExistB(location[valueId2]) && IsExistB(location[valueId3]) && std::binary_search(datasetBId, datasetBId + countB, id)) {
        return;
    }
    resultId[resultCount++] = id;
}

uint32_t Handler::TakeId(const char *begin, const uint16_t &size) {
    // TODO: 解决 hash 冲突
    uint32_t code = HashCode(begin, size);
    while (IsExist(location[code]) && !IsEqual(begin, GetLocPtr(location[code]))) {
        code = (code + 1) & 0x1fffffu;
    }
    return code;
}

uint32_t Handler::HashCode(const char *begin, const uint16_t &size) {
    uint64_t code = 0u;
    for (uint16_t i = 0u; i < size; ++i, ++begin) {
        code = code * 31 + *begin;
    }
    return HashCode(code);
}

inline uint32_t Handler::HashCode(const uint64_t &num) {
    return (uint32_t)((num ^ (num >> 16u)) & 0x1fffffu);
}

bool Handler::IsEqual(const char *value1, const char *value2) {
    while (*value1 != '\"' && *value2 != '\"' && *value1 == *value2) {
        ++value1;
        ++value2;
    }
    return *value1 == '\"' && *value2 == '\"';
}

inline void Handler::AddLocationA(const uint32_t &id, const uint32_t &loc) {
    // 最高位设置 0，表示该字符串位于数据集 a 中，剩余位表示该字符串在数据集 a 中的起始位置
    if (!IsExist(location[id])) {
        location[id] = (loc & 0x7fffffffu);
    }
}

inline void Handler::AddLocationB(const uint32_t &id, const uint32_t &loc) {
    // 最高位设置 1，表示该字符串位于数据集 b 中，剩余位表示该字符串在数据集 b 中的起始位置
    if (!IsExist(location[id])) {
        location[id] = (loc | 0x80000000);
    }
}

inline bool Handler::IsExist(const uint32_t &loc) {
    // 除最高位外，其余位是否不为 0
    return (loc & 0x7fffffffu) != 0u;
}

inline bool Handler::IsExistA(const uint32_t &loc) {
    // 最高位是否为 0
    return (loc & 0x80000000u) == 0u;
}

inline bool Handler::IsExistB(const uint32_t &loc) {
    // 最高位是否为 1
    return (loc & 0x80000000u) != 0u;
}

inline uint64_t Handler::GetId(const uint32_t &valueIdx1, const uint32_t &valueIdx2, const uint32_t &valueIdx3) {
    return (((uint64_t) valueIdx1) << 42u) + (((uint64_t) valueIdx2) << 21u) + (uint64_t) valueIdx3;
}

inline char *Handler::GetLocPtr(const uint32_t &loc) {
    char *buffer = IsExistA(loc) ? bufferA : bufferB;
    return &buffer[loc & 0x7fffffffu];
}

void Handler::GetValue(const uint32_t &loc, char *value) {
    // 将字符串写入 value 数组中
    GetValue(GetLocPtr(loc), value);
}

void Handler::GetValue(const char *begin, char *value) {
    // 将字符串写入 value 数组中
    const char *end = begin;
    uint16_t len = 0u;
    while (*end != '\"') {
        ++end;
        ++len;
    }
    memcpy(value, begin, len);
    value[len] = '\0';
}

void Handler::PrintLog(const std::string &log) {
#ifdef TEST
    LOG << log << std::endl;
#endif
}


int main(int argc, char *argv[]) {

#ifdef JUDGE
    std::string datasetA = argv[1];
    std::string datasetB = argv[2];
#else
#ifdef TEST
#ifdef __linux
    std::string datasetA = "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/data/test/input1.csv";
    std::string datasetB = "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/data/test/input2.csv";
#else
    std::string datasetA = R"(D:\GitHub Repository\ZTE-DB-2021\preliminary\data\input1.csv)";
    std::string datasetB = R"(D:\GitHub Repository\ZTE-DB-2021\preliminary\data\input2.csv)";
#endif
#else
#ifdef FIRST_STAGE
    std::string datasetA = "/home/web/ztedatabase/1/input1.csv";
    std::string datasetB = "/home/web/ztedatabase/1/input2.csv";
#else
    std::string datasetA = "/home/web/ztedatabase/2/input1.csv";
    std::string datasetB = "/home/web/ztedatabase/2/input2.csv";
#endif
#endif
#endif

    Handler handler(datasetA, datasetB);
    handler.Init();
    handler.Input();
    handler.Handle();
    handler.Output();
    handler.Release();

    exit(0);
}
