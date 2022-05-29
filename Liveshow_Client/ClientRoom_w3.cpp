#include "ClientRoom_w3.h"
#include "ui_ClientRoom_w3.h"
#include<QCameraInfo>
#include"QMessageBox"

ClientRoom_w3* ClientRoom_w3::instance = 0;

ClientRoom_w3* ClientRoom_w3::getInstance(){
    if(instance==0){
        instance = new ClientRoom_w3;
    }
    return instance;
}
ClientRoom_w3::ClientRoom_w3(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientRoom_w3)
{
    ui->setupUi(this);

    writeSocket = new QUdpSocket(this);          //主播摄像头
    readSocket = new QUdpSocket(this);           //观众摄像头
    udpCamera();                                 //UDP视频协议

    socketWdaudio = new QUdpSocket(this);        //主播音频
    socketRdaudio = new QUdpSocket(this);        //观众音频
    udpAudio();                                  //UDP音频协议

    timer = new QTimer(this);
    timer2 = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout_slot()));
    connect(timer2,SIGNAL(timeout()),this,SLOT(timeout2_slot()));
}
ClientRoom_w3::~ClientRoom_w3()
{
    delete ui;
}

///视频部分
//UDP视频协议
void ClientRoom_w3::udpCamera()
{
    //摄像头代码
    QCameraInfo info = QCameraInfo::defaultCamera();
    camera = new QCamera(info, this);
    vs = new VideoSuface(this);
    camera->setViewfinder(vs);

    connect(vs, SIGNAL(frameChanged(QVideoFrame)),
            this, SLOT(frameChangedSlot(QVideoFrame)));
    connect(this->readSocket, SIGNAL(readyRead()),
            this, SLOT(readyreadSlot()));
}
//将视频放到主播界面上，将视频发给同组游客
void ClientRoom_w3::frameChangedSlot(QVideoFrame frame)
{
    //QVideoFrame---->QImage
    frame.map(QAbstractVideoBuffer::ReadOnly);
#if 0

        QImage(uchar *data,     //图片字节数组的首地址
           int width,           //图片的宽度
           int height,          //图片高度
           int bytesPerLine,    //图片每行的字节数
           Format format,       //图片处理格式
           );

#endif
    QImage img(
        frame.bits(),
        frame.width(),
        frame.height(),
        frame.bytesPerLine(),
        QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat())
    );


//    图片太大会可能导致发送失败，所以将图片设置的尽可能小一点，但会失真
    img = img.scaled(400,320);     //像素480*320

    //旋转图片180
    QMatrix matrix;
    matrix.rotate(180);
    img = img.transformed(matrix);

    //QImage--->QPixmap
    QPixmap pixMap = QPixmap::fromImage(img);
    //让图片适应label的大小
    pixMap = pixMap.scaled(ui->label_view->size());
    //将图片显示到label上
    ui->label_view->setPixmap(pixMap);

    qDebug() << img.size();
    //将图片信息以广播组的形式发送出去
    QByteArray byte;
    QBuffer buffer(&byte);           //缓存
    img.save(&buffer, "JPEG");

    qDebug() << byte.size();

    //将图片信息写入到制定端口,数据太大会导致发送失败返回-1，具体可参考帮助文档QUdpSocket::writeDatagram
    //注意writeDatagram的地址和端口要和接收端相同（端口不宜过小，以免和其他冲突
    //qDebug()<< writeSocket->writeDatagram(byte,groupAddress,cameraPort);
    qDebug()<< writeSocket->writeDatagram(byte,byte.size(),groupAddress,cameraPort);
    //if(writeSocket->writeDatagram(byte,groupAddress,cameraPort)==-1){return;}
    //qDebug() << "write....";
}
//游客接收到视频
void ClientRoom_w3::readyreadSlot()
{
    //qDebug() << "............";
    quint64 size = readSocket->pendingDatagramSize();
    //直接读取异步监听时接收到的数据报
    QByteArray buff;
    // 字节矩阵，大小用qint64描述，适合操作1大量数据2字符串数组3二进制01数据，节省内存，更加安全
    //一般用const char*初始化
    // 默认采用深拷贝，可指定为浅拷贝
    buff.resize(size);
    //为缓冲区规定一个maxsize（没必要用那么大，刚好装下图像就够了）
    readSocket->readDatagram(buff.data(),buff.size());
    // 数据报只支持一维操作
    QBuffer buffer(&buff);

    //提供用Qbytearray读写内存中IO缓存区的接口，若无传参内部默认自动创建一个Qbytearray；

    //对IO缓存区读写操作等效于像IO设备读写数据

    //操作QBuffer像在操作文件（其实原理都差不多，各自都是内存中一块特殊区域嘛）
    QImageReader reader(&buffer,"JPEG");
    //可读入磁盘文件、设备文件中的图像、以及其他图像数据如pixmap和image，相比较更加专业。
    //buffer属于设备文件一类，
    //专业地读取设备文件的图像数据。
    QImage image = reader.read();
    //read()方法用来读取设备图像，也可读取视频，读取成功返回QImage*，否则返回NULL
    //格式转换
    QPixmap pix = QPixmap::fromImage(image);
    ui->label_view->setPixmap(pix.scaled(ui->label_view->size()));
}
//开启摄像头
void ClientRoom_w3::on_btn_openCamera_clicked()
{
    if(ui->le_hostName->text() == ui->le_currentName->text())  //主播间不显示主播名
    {
        //udpCamera();
        ui->label_9->hide();
        ui->label_11->hide();
        camera->start();
    }else{
        //绑定本地端口 游客绑定该组ip和port
        ui->label_9->hide();
        ui->label_11->hide();
        readSocket->bind(QHostAddress::AnyIPv4,
                         cameraPort,
                         QUdpSocket::ShareAddress|
                         QUdpSocket::ReuseAddressHint);
        readSocket->joinMulticastGroup(groupAddress);
        //connect(this->readSocket, SIGNAL(readyRead()),
                //this, SLOT(readyreadSlot()));
    }
}
//关闭摄像头
void ClientRoom_w3::on_btn_closeCamero_clicked()
{
    if(ui->le_hostName->text() == ui->le_currentName->text())  //主播间不显示主播名
    {
        ui->label_9->show();
        camera->stop();                     //主播可以停止录制
        //camera->destroyed();

        if(!camera->isMetaDataAvailable()){
            ui->label_view->clear();            //主播可以关闭页面
            //ui->label_view->clear();            //??清两遍
        }
    }else{
        ui->label_9->show();
        this->readSocket->leaveMulticastGroup(groupAddress);
        this->readSocket->abort();
        if(!camera->isMetaDataAvailable()){
            ui->label_view->clear();            //游客可以关闭页面
            //ui->label_view->clear();
        }
    }
}

///音频部分
//UDP音频协议
void ClientRoom_w3::udpAudio()
{
    // 设置音频文件格式;
    QAudioFormat format;
    // 设置采样频率;
    format.setSampleRate(8000);
    // 设置通道数;
    format.setChannelCount(1);
    // 设置每次采样得到的样本数据位值;
    format.setSampleSize(8);
    // 设置编码方法;
    format.setCodec("audio/pcm");
    // 设置采样字节存储顺序;
    format.setByteOrder(QAudioFormat::LittleEndian);
    // 设置采样类型;
    format.setSampleType(QAudioFormat::UnSignedInt);
    //按照上述的声音输入输出设备和声音的参数信息创建QAudioInput和QAudioOutput对象
    this->AudiInput = new QAudioInput(format);
    this->AudiOutput = new QAudioOutput(format);
    audioOutputIoDevice = AudiOutput->start();          //默认情况，扬声器处于启动状态
}
//将音频在主播端播放，将音频发给同组游客
void ClientRoom_w3::captureDataFromDevice()
{
    AudioPackage pack;
    memset(&pack,0,sizeof(AudioPackage));
    pack.dataLen = audioInputIoDevice->read(pack.data,1024);
    socketWdaudio->writeDatagram((char*)&pack,               //发给游客
                                 sizeof(AudioPackage),
                                 groupAddress,
                                 audioPort); //???
    /*
    把得到的原始音频数据写入缓存，然后调用audioOutputDevice()的
    write函数把缓存的数据写入声音输出设备即可听到声音。
    */
    audioOutputIoDevice->write(pack.data,pack.dataLen);     //自已的扬声器发声
}
//游客收到音频
void ClientRoom_w3::onReadyReadAudio()
{
//原始音频数据采用的G.711编码方式进行编码，就可以获得压缩后的音频数据，即可进行网络传输
    AudioPackage pack;
    memset(&pack,0,sizeof(AudioPackage));
    socketRdaudio->readDatagram((char*)&pack,sizeof(AudioPackage));
    audioOutputIoDevice->write(pack.data,pack.dataLen);
}
//开启麦克风
void ClientRoom_w3::on_btn_openVoice_clicked()
{
    if(ui->le_hostName->text() == ui->le_currentName->text()){  //主播间不显示主播名
        audioInputIoDevice = AudiInput->start();            //主播麦克风启动收音
        //AudiOutput->stop();
        /*在captureDataFromDevice()槽函数中调用
          audioInputIODevice的read函数得到原始的音频数据。
        */
        connect(audioInputIoDevice,SIGNAL(readyRead()),
                this,SLOT(captureDataFromDevice()));
    }else{
        AudiInput->stop();                                  //游客窗口被禁言
        socketWdaudio->setSocketOption(QAbstractSocket::MulticastTtlOption,1);//??设置套接字属性
        //绑定本地端口
        socketRdaudio->bind (QHostAddress::AnyIPv4,
                             audioPort,
                             QUdpSocket::ReuseAddressHint|
                             QUdpSocket::ShareAddress);//绑定广播地址端口
        socketRdaudio->joinMulticastGroup (groupAddress);  //添加到组播，绑定到读套接字上
        connect(socketRdaudio, SIGNAL(readyRead()),
                this, SLOT(onReadyReadAudio()));
    }
}
//关闭麦克风
void ClientRoom_w3::on_btn_closeVoice_clicked()
{
    if(ui->le_hostName->text() == ui->le_currentName->text()){  //主播间不显示主播名
        AudiInput->stop();                                      //主播麦克风被禁止收音
    }else{
        audioOutputIoDevice->destroyed();
        socketRdaudio->leaveMulticastGroup(groupAddress);
        socketRdaudio->abort();
    }
}

///其他
//退出直播
void ClientRoom_w3::on_btn_backToHall_clicked()
{
    on_btn_closeCamero_clicked();
    on_btn_closeVoice_clicked();
    if(!camera->isMetaDataAvailable()){
        QString hostAccount = ui->le_hostName->text();
        QString userAccount = ui->le_currentName->text();
        emit backToHall_signal(hostAccount,userAccount);
    }
}
/*void ClientRoom_w3::backToHall(){
//!??? 第一次直播不能开两次
//    if(ui->le_hostName->text() == ui->le_currentName->text())  //主播间不显示主播名
//    {
        camera->stop();                     //主播可以停止录制
        ui->label_view->clear();            //主播可以关闭页面
        ui->label_view->clear();            //??清两遍
//    }else{
        this->readSocket->leaveMulticastGroup(groupAddress);
        this->readSocket->abort();
        ui->label_view->clear();            //游客可以关闭页面
        ui->label_view->clear();
//    }
}*/
//拿到组播ip,port
void ClientRoom_w3::setAddress(QString address,int cameraPort,int audioPort)
{
    this->groupAddress.setAddress(address);
    this->cameraPort = cameraPort;
    this->audioPort = audioPort;
}
//创建房间
void ClientRoom_w3::createRoom(const Protocol& dataPack){
    ui->listWidget->clear();
    ui->le_msg->setFocus();
    QString _address = dataPack["address"].toString();
    int _cameraPort = dataPack["cameraPort"].toInt();
    int _audioPort = dataPack["audioPort"].toInt();
    this->setAddress(_address,_cameraPort,_audioPort);

    QString _userAccount = dataPack["userAccount"].toString();
    QString _roomName = dataPack["roomName"].toString();
    double _money = dataPack["money"].toDouble();
    int _score = dataPack["score"].toInt();
    QString title = QString("***********%1直播间(主播窗口)***********").arg(_roomName);

    this->setWindowTitle(title);
    ui->le_hostName->setText(_userAccount);
    ui->le_roomName->setText(_roomName);
    ui->le_currentName->setText(_userAccount);
    ui->le_currentMoney->setText(QString::number(_money,'f',2));
    ui->le_currentScore->setText(QString::number(_score));
}
//加入房间
void ClientRoom_w3::joinRoom(const Protocol& dataPack){
    ui->le_msg->setFocus();
    QString _address = dataPack["address"].toString();
    int _cameraPort = dataPack["cameraPort"].toInt();
    int _audioPort = dataPack["audioPort"].toInt();
    this->setAddress(_address,_cameraPort,_audioPort);

    QString _hostAccount = dataPack["hostAccount"].toString();
    QString _roomName = dataPack["roomName"].toString();
    QString _userAccount = dataPack["userAccount"].toString();
    double _money = dataPack["money"].toDouble();
    int _score = dataPack["score"].toInt();
    QString title = QString("***********%1直播间(游客窗口)***********").arg(_roomName);

    this->setWindowTitle(title);
    ui->le_hostName->setText(_hostAccount);
    ui->le_roomName->setText(_roomName);
    ui->le_currentName->setText(_userAccount);
    ui->le_currentMoney->setText(QString::number(_money,'f',2));
    ui->le_currentScore->setText(QString::number(_score));
}
//刷新游客列表
void ClientRoom_w3::refresh_audienceList(Protocol& dataPack){   //未响应？ 解:此处不能为常量引用
    ui->listWidget->clear();                                    //因为dataPack为左值时系统会认为是在改变它的值
    int count = 0;
    QString userAccount;
    while(dataPack[QString::number(count)].isNull()==false){
        userAccount = dataPack[QString::number(count)].toString();
        ui->listWidget->insertItem(count,userAccount);
        count++;
    }
}
//群聊
void ClientRoom_w3::on_le_msg_returnPressed()
{
    QString msg = ui->le_msg->text();
    if(msg.isEmpty()){
        QMessageBox::warning(this,"提示","发送信息为空");
        return;
    }
    QString roomName = ui->le_roomName->text();
    ui->le_msg->clear();
    emit sendMsg_signal(msg,roomName);
}
void ClientRoom_w3::on_btn_sendMsg_clicked()
{
    on_le_msg_returnPressed();
}
//聊天记录
void ClientRoom_w3::show_chatRecord(Protocol& dataPack){
    QString msg = dataPack["msg"].toString();
    ui->tb_chatRecord->append(msg);
    show_barrage(msg);
}
//显示弹幕
void ClientRoom_w3::show_barrage(QString msg){
    label = new QLabel(msg,ui->label_textBarrage);
    labelVect.push_back(label);

    //设置漂屏标签样式
    label->setStyleSheet("color:blue");
    label->setFont(QFont("Timers",22,QFont::Bold));

    //起始位置
    int labelHeight = qrand()%(ui->label_textBarrage->height()-22);//纵坐标为随机值
    label->move(0,labelHeight);
    label->adjustSize();    //设置label大小自适应字体长度
    timer->start(15);
}
//显示礼物
void ClientRoom_w3::show_gift(QString msg){
    label = new QLabel(msg,ui->label_giftView);
    labelVect2.push_back(label);

    //设置漂屏标签样式
    label->setStyleSheet("color:red");     //???图片效果
    label->setFont(QFont("Timers",15,QFont::Bold));

    //起始位置
    label->move(0,341);
    label->adjustSize();    //设置label大小自适应字体长度
    timer2->start(15);
}
//定时器事件
void ClientRoom_w3::timeout_slot(){
     QVector<QLabel*>::iterator it;
     for(it=labelVect.begin();it!=labelVect.end();it++){
        const QPoint& p = (*it)->pos();
        if(p.x()<(ui->label_textBarrage->width())){
            (*it)->move(p.x()+1,p.y());
            (*it)->show();
        }else{
            (*it)->hide();
        }
     }
}
//定时器事件2
void ClientRoom_w3::timeout2_slot(){
    QVector<QLabel*>::iterator it;
     for(it=labelVect2.begin();it!=labelVect2.end();it++){
        const QPoint& p = (*it)->pos();
        if(p.y()>0){
            (*it)->move(p.x(),p.y()-1);
            (*it)->show();
        }else{
            (*it)->hide();
        }
     }
}
//开启弹幕
void ClientRoom_w3::on_btn_openBarrage_clicked()
{
    ui->label_textBarrage->show();
}
//关闭弹幕
void ClientRoom_w3::on_btn_closeBarrage_clicked()
{
    ui->label_textBarrage->hide();
}
//送礼失败
void ClientRoom_w3::sendGiftFalse(){
    QMessageBox::critical(this,"送礼","余额不足，送礼失败");
}
//送鱼丸
void ClientRoom_w3::on_btn_fishBall_clicked()
{
    if(ui->le_hostName->text()==ui->le_currentName->text()){
        QMessageBox::warning(this,"提示","主播不能给自己送礼物");
        return;
    }
    QString _hostName = ui->le_hostName->text();
    int _value = 1;
    emit sendGift_signal(_hostName,_value);
}
//送鱼翅
void ClientRoom_w3::on_btn_fim_clicked()
{
    if(ui->le_hostName->text()==ui->le_currentName->text()){
        QMessageBox::warning(this,"提示","主播不能给自己送礼物");
        return;
    }
    QString _hostName = ui->le_hostName->text();
    int _value = 2;
    emit sendGift_signal(_hostName,_value);
}
//送跑车
void ClientRoom_w3::on_btn_car_clicked()
{
    if(ui->le_hostName->text()==ui->le_currentName->text()){
        QMessageBox::warning(this,"提示","主播不能给自己送礼物");
        return;
    }
    QString _hostName = ui->le_hostName->text();
    int _value = 10;
    emit sendGift_signal(_hostName,_value);
}
//送飞机
void ClientRoom_w3::on_btn_plane_clicked()
{
    if(ui->le_hostName->text()==ui->le_currentName->text()){
        QMessageBox::warning(this,"提示","主播不能给自己送礼物");
        return;
    }
    QString _hostName = ui->le_hostName->text();
    int _value = 50;
    emit sendGift_signal(_hostName,_value);
}
//送火箭
void ClientRoom_w3::on_btn_rocket_clicked()
{
    if(ui->le_hostName->text()==ui->le_currentName->text()){
        QMessageBox::warning(this,"提示","主播不能给自己送礼物");
        return;
    }
    QString _hostName = ui->le_hostName->text();
    int _value = 100;
    emit sendGift_signal(_hostName,_value);
}
//一键打赏
void ClientRoom_w3::on_btn_oneKeyReward_clicked()
{
    if(ui->le_hostName->text()==ui->le_currentName->text()){
        QMessageBox::warning(this,"提示","主播不能给自己送礼物");
        return;
    }
    QString _hostName = ui->le_hostName->text();
    int _value = 500;
    emit sendGift_signal(_hostName,_value);
}
//送礼后主播信息刷新
void ClientRoom_w3::host_giftRefresh(Protocol& dataPack){
    int _score = dataPack["score"].toInt();
    _score += ui->le_currentScore->text().toInt();
    ui->le_currentScore->setText(QString::number(_score));
}
//送礼后游客信息刷新
void ClientRoom_w3::user_giftRefresh(Protocol& dataPack){
    double _money = dataPack["money"].toDouble();
    int _score = dataPack["score"].toInt();
    ui->le_currentMoney->setText(QString::number(_money,'f',2));
    ui->le_currentScore->setText(QString::number(_score));
}
//礼物显示
void ClientRoom_w3::giftShow(Protocol& dataPack){
    int _value = dataPack["value"].toInt();
    QString _userName = dataPack["userName"].toString();
    QString gift;
    if(_value==1){
        gift = "鱼丸 × 1";
    }else if(_value==2){
        gift = "鱼翅 × 1";
    }else if(_value==10){
        gift = "跑车 × 1";
    }else if(_value==50){
        gift = "飞机 × 1";
    }else if(_value==100){
        gift = "火箭 × 1";
    }else if(_value==500){
        gift = "火箭 × 5";
    }
    QString msg = QString("%1送出了%2").arg(_userName).arg(gift);
    show_gift(msg);
}




