/*
 * @author: shenke
 * @date: 2021/4/14
 * @project: ZTE-DB-2021
 * @desp:
 */

#include <bits/stdc++.h>
#include <sys/stat.h>

#define TEST
// #define FIRST_STAGE


#ifdef TEST
#define OUTPUT_FILE "D:/GitHub Repository/ZTE-DB-2021/preliminary/output.txt"
// #define OUTPUT_FILE "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/output.txt"

#define LOG_FILE "D:/GitHub Repository/ZTE-DB-2021/preliminary/log.txt"
// #define LOG_FILE "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/log.txt"
#endif


class Handler {
public:
    void Init();

    void Input(const std::string &datasetB);

    void Handle(const std::string &datasetA);

    void Output();

    void Release();

private:

    void Handle(const std::string &value1, const std::string &value2, const std::string &value3);

    inline uint64_t GetId(const uint32_t &valueIdx1, const uint32_t &valueIdx2, const uint32_t &valueIdx3);

    void PrintLog(const std::string &log);

    std::unordered_map<std::string, uint32_t> map;
    std::vector<std::string> values;

    std::unordered_set<uint64_t> datasetBIds;
    std::unordered_set<uint64_t> resultIds;

#ifdef TEST
    std::ofstream LOG;
    std::chrono::system_clock::time_point start;
#endif
};


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
}

void Handler::Input(const std::string &datasetB) {
    FILE *fp;
    char buffer[1u << 13u];
    char valueBuffer[1u << 12u];
    uint32_t valueIdx1, valueIdx2, valueIdx3;

    fp = fopen(datasetB.c_str(), "r");
    if (fp == nullptr) {
        PrintLog("Fail to open file: " + datasetB);
        exit(-1);
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        uint16_t left = 0u, right;
        while (buffer[left] != '\"') ++left;
        right = ++left;
        while (buffer[right] != '\"') ++right;
        memcpy(valueBuffer, buffer + left, right - left);
        valueBuffer[right - left] = '\0';
        std::string value1(valueBuffer);
        if (map.find(value1) == map.end()) {
            valueIdx1 = (uint32_t) map.size();
            map[value1] = valueIdx1;
            values.emplace_back(value1);
        } else {
            valueIdx1 = map[value1];
        }

        right += 3u;
        left = right;
        while (buffer[right] != '\"') ++right;
        memcpy(valueBuffer, buffer + left, right - left);
        valueBuffer[right - left] = '\0';
        std::string value2(valueBuffer);
        if (map.find(value2) == map.end()) {
            valueIdx2 = (uint32_t) map.size();
            map[value2] = valueIdx2;
            values.emplace_back(value2);
        } else {
            valueIdx2 = map[value2];
        }

        right += 3u;
        left = right;
        while (buffer[right] != '\"') ++right;
        memcpy(valueBuffer, buffer + left, right - left);
        valueBuffer[right - left] = '\0';
        std::string value3(valueBuffer);
        if (map.find(value3) == map.end()) {
            valueIdx3 = (uint32_t) map.size();
            map[value3] = valueIdx3;
            values.emplace_back(value3);
        } else {
            valueIdx3 = map[value3];
        }

        datasetBIds.insert(GetId(valueIdx1, valueIdx2, valueIdx3));
    }

    fclose(fp);
}

void Handler::Handle(const std::string &datasetA) {
    FILE *fp;
    char buffer[1u << 13u];
    char valueBuffer[1u << 12u];

    fp = fopen(datasetA.c_str(), "r");
    if (fp == nullptr) {
        PrintLog("Fail to open file: " + datasetA);
        exit(-1);
    }

    while (fgets(buffer, sizeof(buffer), fp)) {
        uint16_t left = 0u, right;
        while (buffer[left] != '\"') ++left;
        right = ++left;
        while (buffer[right] != '\"') ++right;

        memcpy(valueBuffer, buffer + left, right - left);
        valueBuffer[right - left] = '\0';
        std::string value1(valueBuffer);

        right += 3u;
        left = right;
        while (buffer[right] != '\"') ++right;
        memcpy(valueBuffer, buffer + left, right - left);
        valueBuffer[right - left] = '\0';
        std::string value2(valueBuffer);

        right += 3u;
        left = right;
        while (buffer[right] != '\"') ++right;
        memcpy(valueBuffer, buffer + left, right - left);
        valueBuffer[right - left] = '\0';
        std::string value3(valueBuffer);

        Handle(value1, value2, value3);
    }

    fclose(fp);
}

void Handler::Output() {
    for (const uint64_t &id : resultIds) {
        printf("\"%s\",\"%s\",\"%s\"\n",
               values[(id >> 42u) & 0x1fffffu].c_str(), values[(id >> 21u) & 0x1fffffu].c_str(), values[id & 0x1fffffu].c_str());
    }
}

void Handler::Release() {
#ifdef TEST
    std::chrono::duration<double, std::milli> duration = std::chrono::system_clock::now() - start;
    PrintLog("Elapsed time: " + std::to_string(duration.count()) + "ms");

    fclose(stdout);
    LOG.close();
#endif
}

void Handler::Handle(const std::string &value1, const std::string &value2, const std::string &value3) {
    bool exist1 = map.find(value1) != map.end();
    bool exist2 = map.find(value2) != map.end();
    bool exist3 = map.find(value3) != map.end();

    if (exist1 && exist2 && exist3) {
        uint64_t id = GetId(map[value1], map[value2], map[value3]);
        if (datasetBIds.find(id) != datasetBIds.end()) {
            return;
        }
        resultIds.insert(id);
    } else {
        uint32_t valueIdx1, valueIdx2, valueIdx3;

        if (!exist1) {
            valueIdx1 = map.size();
            map[value1] = map.size();
            values.emplace_back(value1);
        } else {
            valueIdx1 = map[value1];
        }

        if (!exist2) {
            valueIdx2 = map.size();
            map[value2] = map.size();
            values.emplace_back(value2);
        } else {
            valueIdx2 = map[value2];
        }

        if (!exist3) {
            valueIdx3 = map.size();
            map[value3] = map.size();
            values.emplace_back(value3);
        } else {
            valueIdx3 = map[value3];
        }

        resultIds.insert(GetId(valueIdx1, valueIdx2, valueIdx3));
    }
}

inline uint64_t Handler::GetId(const uint32_t &valueIdx1, const uint32_t &valueIdx2, const uint32_t &valueIdx3) {
    return (((uint64_t) valueIdx1) << 42u) + (((uint64_t) valueIdx2) << 21u) + (uint64_t) valueIdx3;
}

void Handler::PrintLog(const std::string &log) {
#ifdef TEST
    LOG << log << std::endl;
#endif
}


int main() {
#ifdef TEST
    // std::string datasetA = "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/data/input1.csv";
    // std::string datasetB = "/home/shenke/Develop/Repository/ZTE-DB-2021/preliminary/data/input2.csv";
    std::string datasetA = R"(D:\GitHub Repository\ZTE-DB-2021\preliminary\data\input1.csv)";
    std::string datasetB = R"(D:\GitHub Repository\ZTE-DB-2021\preliminary\data\input2.csv)";
#else
#ifdef FIRST_STAGE
    std::string datasetA = "/home/web/ztedatabase/1/input1.csv";
    std::string datasetB = "/home/web/ztedatabase/1/input2.csv";
#else
    std::string datasetA = "/home/web/ztedatabase/2/input1.csv";
    std::string datasetB = "/home/web/ztedatabase/2/input2.csv";
#endif
#endif

    Handler handler;
    handler.Init();
    handler.Input(datasetB);
    handler.Handle(datasetA);
    handler.Output();
    handler.Release();

    exit(0);
}
