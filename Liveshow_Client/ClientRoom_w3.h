#ifndef CLIENTROOM_W3_H
#define CLIENTROOM_W3_H

#include <QWidget>
#include"protocol.h"

#include"QHostAddress"
#include<QUdpSocket>

#include<QMovie>
#include"VideoSuface.h"
#include<QCamera>
#include<QBuffer>

#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>

#include"QVector"
#include"QLabel"
#include"Qtimer"
#include"QPoint"

namespace Ui {
class ClientRoom_w3;
}

struct AudioPackage
{
    char data[1024];
    int dataLen;
};

class ClientRoom_w3 : public QWidget
{
    Q_OBJECT

public:
    static ClientRoom_w3* getInstance();
    explicit ClientRoom_w3(QWidget *parent = 0);
    ~ClientRoom_w3();

    void joinRoom(const Protocol& dataPack);
    void createRoom(const Protocol& dataPack);
    void refresh_audienceList(Protocol& dataPack);
    void show_chatRecord(Protocol& dataPack);
    //void backToHall();
    void sendGiftFalse();
    void host_giftRefresh(Protocol& dataPack);
    void user_giftRefresh(Protocol& dataPack);
    void giftShow(Protocol& dataPack);
    void show_gift(QString msg);

    //视频音频传输
    void setAddress(QString address,int cameraPort,int audioPort);

    void udpCamera();       //视频部分

    void udpAudio();        //音频部分

    void show_barrage(QString msg);     //弹幕部分

signals:
    void backToHall_signal();
    void sendMsg_signal(QString,QString);
    void backToHall_signal(QString,QString);
    void sendGift_signal(QString,int);
private slots:
    void on_btn_backToHall_clicked();
    void on_le_msg_returnPressed();
    void on_btn_sendMsg_clicked();
    void on_btn_openCamera_clicked();
    void on_btn_closeCamero_clicked();
    void on_btn_fishBall_clicked();

    void readyreadSlot();   //视频部分
    void frameChangedSlot(QVideoFrame frame);

    void captureDataFromDevice();//音频部分
    void onReadyReadAudio();
    void on_btn_openVoice_clicked();
    void on_btn_closeVoice_clicked();

    void timeout_slot();  //弹幕部分
    void on_btn_openBarrage_clicked();
    void on_btn_closeBarrage_clicked();

    void on_btn_fim_clicked();
    void on_btn_car_clicked();
    void on_btn_plane_clicked();
    void on_btn_rocket_clicked();
    void on_btn_oneKeyReward_clicked();
    void timeout2_slot();
private:
    Ui::ClientRoom_w3 *ui;
    static ClientRoom_w3* instance;

    QHostAddress        groupAddress;       //组播ip
    int                 cameraPort;         //视频端口
    int                 audioPort;          //音频端口

    QUdpSocket*         writeSocket;        //视频部分
    QUdpSocket*         readSocket;
    VideoSuface*        vs;
    QCamera*            camera;

    QUdpSocket*         socketWdaudio;      //音频部分
    QUdpSocket*         socketRdaudio;
    QAudioInput*        AudiInput;
    QAudioOutput*       AudiOutput;
    QIODevice*          audioInputIoDevice;
    QIODevice*          audioOutputIoDevice;

    QLabel* label;                          //弹幕部分
    QVector<QLabel*> labelVect;
    QVector<QLabel*> labelVect2;
    QTimer* timer;
    QTimer* timer2;
};

#endif // CLIENTROOM_W3_H
