#ifndef PROTOCOL_H
#define PROTOCOL_H

#include"QJsonObject"
#include"QJsonDocument"

class Protocol{
public:
    enum Type{
        none,

        rgst,               //账户注册
        login,              //账户登录
        ClientExit,         //账户退出
        ClientDelete,       //账户注销
        showUserInfo,       //显示信息
        chargeMoney,        //账户充值

        RoomCreate,         //创建房间
        JoinRoom,           //加入房间
        refreshRoomList,    //房间列表
        refreshAudienceList,//游客列表
        closeRoom,          //关闭房间
        sendMsg,            //用户群聊
        backToHall,         //退出直播
        AudienceBackToHall, //退出直播
        sendGift            //送出礼物
    };

    Protocol(Type type = none):type(type){}

    void setType(Type type){this->type = type;}
    QJsonValueRef operator[](const QString& key){return m_body[key];}   //set值时返回值必须是引用或指针，否则操作的只是副本，值无法改变

    Type getType()const{return type;}
    QJsonValue operator[](const QString& key)const{return m_body[key];}

    //打包
    QByteArray pack(){
        //数据包 = head + body(可变长数据部分)
        QByteArray body = QJsonDocument(m_body).toJson();

        QByteArray head(8,0);
        //head的前四个字节为可变长数据部分的大小
        *(int*)head.data() = body.size();
        //head的后四个字节为数据包的类型(固定数据部分的大小)
        *(Type*)(head.data()+4) = type; //??

        return head+body;
    }
    //拆包
    int unpack(const QByteArray& dataPack){
        if(dataPack.isEmpty()){return 0;}

        //可边长数据部分的长度
        int len = *(int*)dataPack.data();
        if(len<=0){return 0;}

        //取出类型
        this->type = *(Type*)(dataPack.data()+4);

        //取出可边长数据部分
        QByteArray body = dataPack.mid(8,len);
        //保存到json类里
        m_body = QJsonDocument::fromJson(body).object();

        return 8+len;
    }

private:
   Type type;
   QJsonObject m_body;
};

#endif // PROTOCOL_H
