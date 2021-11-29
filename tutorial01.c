// tutorial01.c
//
// This tutorial was written by Stephen Dranger (dranger@gmail.com).
//
// Code based on a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1

// A small sample program that shows how to use libavformat and libavcodec to
// read video from a file.
//
// Use the Makefile to build all examples.
//
//      or gcc -Wall -ggdb tutorial01.c -D_GNU_SOURCE=1 -D_REENTRANT -I/usr/local/include -I/usr/include/SDL -c -o obj/tutorial01.o
//
// Run using
//
// tutorial01 myvideofile.mpg
//
// to write the first five frames from "myvideofile.mpg" to disk in PPM format.

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#include <stdio.h>

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;

  // Open file
  sprintf(szFilename, "frame%d.ppm", iFrame);
  pFile = fopen(szFilename, "wb");
  if(pFile == NULL)
    return;

  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);

  // Write pixel data
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);

  // Close file
  fclose(pFile);
}

int main(int argc, char *argv[]) {
  AVFormatContext *pFormatCtx = NULL;
  int             i, videoStream;
  AVCodecContext  *pCodecCtx = NULL;
  AVCodec         *pCodec = NULL;
  AVFrame         *pFrame = NULL;
  AVFrame         *pFrameRGB = NULL;
  AVPacket        packet;
  int             frameFinished;
  int             numBytes;
  uint8_t         *buffer = NULL;

  AVDictionary    *optionsDict = NULL;
  struct SwsContext      *sws_ctx = NULL;

  if(argc < 2) {
    printf("Please provide a movie file\n");
    return -1;
  }

  // av_register_all 只需要调用一次，他会注册所有可用的文件格式和编解码库，当文件被打开时他们将自动匹配相应的编解码库。
  av_register_all();

  // 从传入的第二个参数获得文件路径，这个函数会读取文件头信息，并把信息保存在 pFormatCtx 结构体当中。
  // 这个函数后面两个参数分别是： 指定文件格式、格式化选项，当我们设置为 NULL 或 0 时，libavformat 会自动完成这些工作。
  if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)!=0)
    return -1; // Couldn't open file

  // 要得到流信息
  if(avformat_find_stream_info(pFormatCtx, NULL)<0)
    return -1; // Couldn't find stream information

  // 这个函数填充了 pFormatCtx->streams 流信息， 可以通过 dump_format 把信息打印出来：
  av_dump_format(pFormatCtx, 0, argv[1], 0);

  // 获取第一个视频流
  videoStream = -1;
  for(i=0; i<pFormatCtx->nb_streams; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
      videoStream = i;
      break;
    }
  if(videoStream == -1)
    return -1; // Didn't find a video stream

  // pCodecCtx 包含了这个流在用的编解码的所有信息，但我们仍需要通过他获得特定的解码器然后打开他。
  pCodecCtx = pFormatCtx->streams[videoStream]->codec;

  // 为视频流获取特定的解码器。
  pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
  if(pCodec == NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1; // Codec not found
  }
  // Open codec
  if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
    return -1; // Could not open codec

  //　我们需要内存空间存储一帧数据
  pFrame = av_frame_alloc();

  // Allocate an AVFrame structure
  pFrameRGB = av_frame_alloc();
  if(pFrameRGB==NULL)
    return -1;

  // 即使分配了帧空间，我们仍然需要空间来存放转换时的 raw 数据，我们用 avpicture_get_size 来得到需要的空间，然后手动分配。
  numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
			      pCodecCtx->height);
  buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  sws_ctx = sws_getContext(
              pCodecCtx->width,
              pCodecCtx->height,
              pCodecCtx->pix_fmt,
              pCodecCtx->width,
              pCodecCtx->height,
              AV_PIX_FMT_RGB24,
              SWS_BILINEAR,
              NULL,
              NULL,
              NULL
            );

  // Assign appropriate parts of buffer to image planes in pFrameRGB
  // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
  // of AVPicture
  // 我们使用 avpicture_fill 来关联新分配的缓冲区的帧。 
  // AVPicture 结构体是 AVFrame 结构体的一个子集，开始的AVFrame 是和 AVPicture 相同的
  avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
		 pCodecCtx->width, pCodecCtx->height);

  // 通过包来读取整个视频流， 然后解码到帧当中，一但一帧完成了， 将转换并保存它
  i=0;
  while(av_read_frame(pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
    if(packet.stream_index == videoStream) {
      // Decode video frame
      avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
      // Did we get a video frame?
      if(frameFinished) {
	    // Convert the image from its native format to RGB
        sws_scale
        (
            sws_ctx,
            (uint8_t const * const *)pFrame->data,
            pFrame->linesize,
            0,
            pCodecCtx->height,
            pFrameRGB->data,
            pFrameRGB->linesize
        );

	      // Save the frame to disk
      	if(++i<=5){
            SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, i);
        }
        else{
          av_free_packet(&packet);
          break;
        }
      }
    }

      // Free the packet that was allocated by av_read_frame
      av_free_packet(&packet);
  }

  // Free the RGB image
  av_free(buffer);
  av_free(pFrameRGB);

  // Free the YUV frame
  av_free(pFrame);

  // Close the codec
  avcodec_close(pCodecCtx);

  // Close the video file
  avformat_close_input(&pFormatCtx);

  return 0;
}
