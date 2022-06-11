#include "rpc/protocol.h"

#include<chrono>
#include"string.h"
#include<iostream>

namespace Fish
{
    Protocol::Protocol()
    {
        msg_._msg.magic_ = dfMagic;
        msg_._msg.totalLength_ = dfHeadLength;  
        msg_._msg.headLength_ = dfHeadLength;
        msg_._msg.version_ = dfVersion;
        msg_._msg.sFormat_ = 1; 
    }
    

    std::string Protocol::gettime()
    {
        using namespace std::chrono;

        auto temp = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

        std::string result;

        result.resize(sizeof(temp));

        ::memcpy(&(result[0]), &temp, sizeof(temp));

        return result;
    }

    //函数返回-1，说明create失败，
    //函数返回其他负数，说明数据短于head，只读取了magic，返回magic索引位置的复数
    //函数返回0，说明没有读到magic
    //函数返回>0， 说明正常读取
    int Protocol::create(std::string_view sView)
    {
        if(sView.size() < dfHeadLength) return -1;
        //Protocol::ptr protl = std::make_shared<Protocol>();       

        size_t idx;             //表示查找位置
        size_t readIdx = 0;     //表示已经读取的位置
        //使用状态机处理
        switch (handleState)
        {
        case RPCSTATE::FREE:
            //找magic
            idx = sView.find(static_cast<char>(dfMagic));
            if(idx == std::string_view::npos)   return -1;

            sView = sView.substr(idx);
            readIdx += idx;           
            
            std::copy(sView.data(),sView.data() + msg_._msg.totalLength_,msg_.msgStr);

            if(sView.size() < msg_._msg.totalLength_)
            {
                handleState = RPCSTATE::LACK;
                return -1*idx;
            }                
            else
            {
                handleState = RPCSTATE::WAITCONTENT;
                sView = sView.substr(msg_._msg.totalLength_);
                readIdx += msg_._msg.totalLength_;
            } 

        case RPCSTATE::WAITCONTENT:
           //找到contentLength存储的位置
    
            msg_._msg.contentLength_ = *((uint32_t*)(msg_.msgStr + msg_._msg.totalLength_-4));

    
            //仍然缺失content内容
            if(sView.size() + content_.size() < msg_._msg.contentLength_)   
            {
                content_ += sView;
                handleState =  RPCSTATE::LACK;
            }
            else
            {
                std::string_view thisContent;
                thisContent = sView.substr(0, msg_._msg.contentLength_ - content_.size());
               
                content_ += thisContent;
                handleState = RPCSTATE::READY;
                readIdx += (content_.size());
                return readIdx;
            }       
            case RPCSTATE::READY:    
                break;
            case RPCSTATE::LACK:
            break;
            case RPCSTATE::WORKING:
                break;
        }       
        return readIdx;
    }

    void Protocol::setContent(const std::string& content)
    {
        msg_._msg.contentLength_ += content.size();
        content_ = content;
    }

    void Protocol::setLength(uint32_t length)
    {   msg_._msg.totalLength_ += length - msg_._msg.headLength_; 
        if(msg_._msg.totalLength_ < 0) 
            msg_._msg.totalLength_ = 0;
        msg_._msg.headLength_ = length; 
    }

    // void Protocol::editAppend(std::string_view& sview)
    // {
    //     msg_._msg.totalLength_ += sview.size();
    //     std::copy(sview.begin(),sview.end(),msg_._msg.extend_);
    // }

    void Protocol::setHeadMsg(MsgType type,uint32_t id)
    {
        msg_._msg.msgType_ = type;
        msg_._msg.msgId_ = id; 
    }
    void Protocol::printInfo()
    {
        std::cout << "magic_ = " << (int)msg_._msg.magic_ << "    totalLength_ = "
        << (int)msg_._msg.totalLength_ << "    headLength_ = " << (int)msg_._msg.headLength_
        << "    version_ = " << (int)msg_._msg.version_ << "    msgType_ = "
        << (int)msg_._msg.msgType_ << "    sFormat_ = " << (int)msg_._msg.sFormat_
        << "    msgId_ = " << (int)msg_._msg.msgId_ << "    contentLength_ = " << (int)msg_._msg.contentLength_ << std::endl;
    }


    std::string Protocol::result()
    {
        std::string result;

        result.resize(msg_._msg.totalLength_);

        std::copy(msg_.msgStr, msg_.msgStr + msg_._msg.totalLength_, &result[0]);

        result += content_;

        return result;
    }

}