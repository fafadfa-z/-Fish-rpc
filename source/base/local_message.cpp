#include "base/local_message.h"
#include "base/log/logger.h"

thread_local int thread_tid = 0;

namespace Fish
{
    LocalMassage *LocalMassage::readConfig(const std::filesystem::path &path)
    {
        if (!entity_)
        {
            entity_ = new LocalMassage(path);
        }
        return entity_;
    }

    LocalMassage::LocalMassage(const std::filesystem::path &configFile)
        : FileRead(configFile), configFile_(configFile)
    {
        std::filesystem::directory_entry direEntry(configFile_);

        assert(direEntry.is_regular_file());

        std::string mes;
        while (readLine(mes))
        {
            if (mes == "")
                break;
            auto iter = std::find(mes.begin(), mes.end(), ':');

            confMap_[std::string(mes.begin(), iter)] = std::string(iter + 2, mes.end());
        }
    }

    std::string LocalMassage::configRead(const char *str)
    {
        auto iter = confMap_.find(str);

        if(iter == confMap_.end()) return std::string();

        return iter->second;
    }
    std::string LocalMassage::configRead(const std::string &str)
    {
        auto iter = confMap_.find(str);

        if(iter == confMap_.end()) return std::string();

        return iter->second;
    }

    void LocalMassage::prinfMes()
    {
        for (auto &[key, value] : confMap_)
        {
            std::cout << key << ":\t" << value << std::endl;
        }
    }

    LocalMassage::~LocalMassage()
    {
    }
}