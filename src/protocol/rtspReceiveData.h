﻿
#ifndef RTSP_RTSPRECEIVEDATA_H
#define RTSP_RTSPRECEIVEDATA_H

#include "transportPacket.h"
#include "TcpSocket.h"
#include "NALReader.h"
#include "adtsReader.h"
#include "adtsHeader.h"

class RtspReceiveData {
private:

    /* byte 0 */
    /*数据源的个数（即源的个数），如果只有一个源那么此时的值为0*/
    uint8_t csrcLen{0};
    /*扩充位，如果设置为允许的话，固定头结构后面（即包的12个字节后面，有效负载的前面）紧跟着一个扩展头结构，该结构是已定义的一种格式*/
    uint8_t extension{0};

    /*如果P=1，则在该报文的尾部填充一个或多个额外的八位组，它们不是有效载荷的一部分  */
    uint8_t padding{0};
    /*版本号*/
    uint8_t version{2};

    /* byte 1 */
    /*用于说明RTP报文中有效载荷的类型，如GSM音频、JPEM图像等,在流媒体中大部分是用来区分音频流和视频流的，这样便于客户端进行解析*/
    uint8_t payloadType{0};
    /*不同的有效载荷有不同的含义，对于视频，标记一帧的结束；对于音频，标记会话的开始*/
    uint8_t marker{0};

    /* bytes 2,3 */
    /*用于标识发送者所发送的RTP报文的序列号，每发送一个报文，序列号增1。
     * 这个字段当下层的承载协议用UDP的时候，网络状况不好的时候可以用来检查丢包。
     * 同时出现网络抖动的情况可以用来对数据进行重新排序，序列号的初始值是随机的，同时音频包和视频包的sequence是分别记数的*/
    uint16_t seq{0};

    /* bytes 4-7 */
    /*必须使用90 kHz 时钟频率。时戳反映了该RTP报文的第一个八位组的采样时刻。接收者使用时戳来计算延迟和延迟抖动，并进行同步控制*/
    uint32_t timestamp{0};

    /* bytes 8-11 */
    /*用于标识同步信源。该标识符是随机选择的，参加同一视频会议的两个同步信源不能有相同的SSRC*/
    uint32_t ssrc{0};
private:
    SOCKET clientSocket{0};
    uint8_t videoChannel{0};
    uint8_t audioChannel{0};

    TransportPacket ts;
    uint8_t *buffer{nullptr};
    uint8_t *nalUintData{nullptr};
    uint32_t nalUintOffset{0};


    NALReader videoReader;
    NALPicture *picture{nullptr};
    AdtsReader audioReader;
    AdtsHeader adtsHeader;


    int &packetNumber;
public:
    explicit RtspReceiveData(int &transportStreamPacketNumber);

    int init(SOCKET socket, const std::string &path, uint8_t video, uint8_t audio);

    int writeVideoData(uint8_t *data, uint8_t size);

    int writeAudioData(uint8_t audioObjectType, uint8_t samplingFrequencyIndex, uint8_t channelConfiguration);

    int receiveData(const std::string &packet);

    ~RtspReceiveData();

private:
    int getRtpHeader(ReadStream &rs);

    int disposeRtpData(uint8_t *rtpBuffer, uint32_t rtpBufferSize, uint8_t channel, uint16_t length);
};


#endif //RTSP_RTSPRECEIVEDATA_H
