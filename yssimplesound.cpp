#include <algorithm>

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "yssimplesound.h"

YsSoundPlayer *YsSoundPlayer::currentPlayer=nullptr;

YsSoundPlayer::YsSoundPlayer()
{
	api=CreateAPISpecificData();
	playerStatePtr.reset(new STATE);
	*playerStatePtr=STATE_UNINITIALIZED;
}
YsSoundPlayer::~YsSoundPlayer()
{
	End();
	if(GetCurrentPlayer()==this)
	{
		NullifyCurrentPlayer();
	}
	DeleteAPISpecificData(api);
}

void YsSoundPlayer::MakeCurrent(void)
{
	currentPlayer=this;
}

/* static */ void YsSoundPlayer::NullifyCurrentPlayer(void)
{
	currentPlayer=nullptr;
}
/* static */ YsSoundPlayer *YsSoundPlayer::GetCurrentPlayer(void)
{
	return currentPlayer;
}



////////////////////////////////////////////////////////////



YsSoundPlayer::SoundData::MemInStream::MemInStream(long long int len,const unsigned char dat[])
{
	this->length=len;
	this->pointer=0;
	this->dat=dat;
}
long long int YsSoundPlayer::SoundData::MemInStream::Fetch(unsigned char buf[],long long int len)
{
	long long int nCopy=0;
	if(pointer+len<length)
	{
		nCopy=len;
	}
	else
	{
		nCopy=length-pointer;
	}
	for(long long int i=0; i<nCopy; ++i)
	{
		buf[i]=dat[pointer];
		++pointer;
	}
	return nCopy;
}

long long int YsSoundPlayer::SoundData::MemInStream::Skip(long long int len)
{
	long long int nSkip=0;
	if(pointer+len<length)
	{
		nSkip=len;
	}
	else
	{
		nSkip=length-pointer;
	}
	pointer+=nSkip;
	return nSkip;
}

YsSoundPlayer::SoundData::FileInStream::FileInStream(FILE *fp)
{
	this->fp=fp;
}
long long int YsSoundPlayer::SoundData::FileInStream::Skip(long long int len)
{
	size_t totalSkipped=0;

	const long long skipBufSize=1024*1024;
	char *skipbuf=new char [skipBufSize];
	while(0<len)
	{
		auto toSkip=len;
		if(skipBufSize<toSkip)
		{
			toSkip=skipBufSize;
		}
		auto skipped=fread(skipbuf,1,toSkip,fp);
		totalSkipped+=skipped;
		len-=skipped;
		if(0==skipped)
		{
			break;
		}
	}
	delete [] skipbuf;
	return totalSkipped;
}
long long int YsSoundPlayer::SoundData::FileInStream::Fetch(unsigned char buf[],long long int len)
{
	return fread(buf,1,len,fp);
}

////////////////////////////////////////////////////////////

void YsSoundPlayer::Start(void)
{
	if(YSOK==StartAPISpecific())
	{
		*playerStatePtr=STATE_STARTED;
	}
}
void YsSoundPlayer::End(void)
{
	EndAPISpecific();
	*playerStatePtr=STATE_ENDED;
}

void YsSoundPlayer::PreparePlay(SoundData &dat)
{
	if(nullptr==dat.playerStatePtr)
	{
		dat.playerStatePtr=this->playerStatePtr;
	}
	else if(dat.playerStatePtr!=this->playerStatePtr)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  YsSoundPlayer::SoundData can be associated with only one player.\n");
		return;
	}
	dat.PreparePlay(*this);
	dat.prepared=true;
}

void YsSoundPlayer::PlayOneShot(SoundData &dat)
{
	if(true!=dat.IsPrepared(*this))
	{
		PreparePlay(dat);
	}
	if(YSOK==PlayOneShotAPISpecific(dat))
	{
	}
}
void YsSoundPlayer::PlayBackground(SoundData &dat)
{
	if(true!=dat.IsPrepared(*this))
	{
		PreparePlay(dat);
	}
	if(YSOK==PlayBackgroundAPISpecific(dat))
	{
	}
}

YSRESULT YsSoundPlayer::StartStreaming(Stream &streamPlayer,StreamingOption opt)
{
	streamPlayer.requestedRingBufferLengthMillisec=opt.ringBufferLengthMillisec;
	return StartStreamingAPISpecific(streamPlayer,opt);
}

YSRESULT YsSoundPlayer::StartStreaming(Stream &streamPlayer)
{
	StreamingOption opt;
	return StartStreaming(streamPlayer,opt);
}

void YsSoundPlayer::StopStreaming(Stream &streamPlayer)
{
	StopStreamingAPISpecific(streamPlayer);
}

YSBOOL YsSoundPlayer::StreamPlayerReadyToAcceptNextNumSample(const Stream &streamPlayer,unsigned int numSamples) const
{
	return StreamPlayerReadyToAcceptNextNumSampleAPISpecific(streamPlayer,numSamples);
}

YSBOOL YsSoundPlayer::StreamPlayerReadyToAcceptNextSegment(const Stream &streamPlayer,const SoundData &dat) const
{
	// Currently not supporting 8 bit samples.
	if(16!=dat.BitPerSample())
	{
		return YSFALSE;
	}

	uint32_t millisec=dat.GetNumSamplePerChannel()*1000/dat.PlayBackRate();
	if(streamPlayer.requestedRingBufferLengthMillisec<millisec*2)
	{
		// This output is added because one of my MMLPlayer sample was trying to send 1000 millisec at a time.
		// It was fine in Linux and macOSX since the implementation was using double-buffering.
		// It caused some trouble in Windows since the implementation was really using a ring-buffer.
		printf("Number of Samples given to the stream player (%u) must be less than or equal\n",millisec);
		printf("to the half of the requested ring-buffer size (%u)\n",streamPlayer.requestedRingBufferLengthMillisec);
		return YSFALSE;
	}

	return StreamPlayerReadyToAcceptNextNumSample(streamPlayer,dat.GetNumSamplePerChannel());
}

YSRESULT YsSoundPlayer::AddNextStreamingSegment(Stream &streamPlayer,const SoundData &dat)
{
	// Currently not supporting 8 bit samples.
	if(16!=dat.BitPerSample())
	{
		return YSERR;
	}
	return AddNextStreamingSegmentAPISpecific(streamPlayer,dat);
}

void YsSoundPlayer::Stop(SoundData &dat)
{
	StopAPISpecific(dat);
}
void YsSoundPlayer::Pause(SoundData &dat)
{
	PauseAPISpecific(dat);
}
void YsSoundPlayer::Resume(SoundData &dat)
{
	ResumeAPISpecific(dat);
}

void YsSoundPlayer::KeepPlaying(void)
{
	KeepPlayingAPISpecific();
}

YSBOOL YsSoundPlayer::IsPlaying(const SoundData &dat) const
{
	return IsPlayingAPISpecific(dat);
}

double YsSoundPlayer::GetCurrentPosition(const SoundData &dat) const
{
	return GetCurrentPositionAPISpecific(dat);
}

void YsSoundPlayer::SetVolume(SoundData &dat,float vol)
{
	dat.playBackVolumeLeft=vol;
	dat.playBackVolumeRight=vol;
	SetVolumeAPISpecific(dat,vol,vol);
}

void YsSoundPlayer::SetVolumeLR(SoundData &dat,float leftVol,float rightVol)
{
	dat.playBackVolumeLeft=leftVol;
	dat.playBackVolumeRight=rightVol;
	SetVolumeAPISpecific(dat,leftVol,rightVol);
}

////////////////////////////////////////////////////////////



YsSoundPlayer::SoundData::SoundData()
{
	api=CreateAPISpecificData();
	Initialize();
}

YsSoundPlayer::SoundData::~SoundData()
{
	CleanUp();
	DeleteAPISpecificData(api);
}

void YsSoundPlayer::SoundData::CopyFrom(const SoundData &incoming)
{
	if(this!=&incoming)
	{
		CleanUp();

		prepared=false;

		playerStatePtr=incoming.playerStatePtr;

		lastModifiedChannel=incoming.lastModifiedChannel;
		stereo=incoming.stereo;
		bit=incoming.bit;
		rate=incoming.rate;
		sizeInBytes=incoming.sizeInBytes;

		isSigned=incoming.isSigned;
		dat=incoming.dat;
		playBackVolumeLeft=incoming.playBackVolumeLeft;
		playBackVolumeRight=incoming.playBackVolumeRight;

		// Do not copy -> APISpecificDataPerSoundData *api;
	}
}

void YsSoundPlayer::SoundData::MoveFrom(SoundData &incoming)
{
	if(this!=&incoming)
	{
		CleanUp();

		prepared=false;

		playerStatePtr=incoming.playerStatePtr;

		lastModifiedChannel=incoming.lastModifiedChannel;
		stereo=incoming.stereo;
		bit=incoming.bit;
		rate=incoming.rate;
		sizeInBytes=incoming.sizeInBytes;

		isSigned=incoming.isSigned;
		std::swap(dat,incoming.dat);
		playBackVolumeLeft=incoming.playBackVolumeLeft;
		playBackVolumeRight=incoming.playBackVolumeRight;

		incoming.CleanUp();

		// Do not copy -> APISpecificDataPerSoundData *api;
	}
}

void YsSoundPlayer::SoundData::Initialize(void)
{
	CleanUp();
}

void YsSoundPlayer::SoundData::CleanUp(void)
{
	CleanUpAPISpecific();

	dat.clear();

	prepared=false;

	lastModifiedChannel=0;
	stereo=YSFALSE;
	bit=16;
	rate=44100;
	sizeInBytes=0;
	isSigned=YSTRUE;
	playBackVolumeLeft=1.0;
	playBackVolumeRight=1.0;
}

unsigned int YsSoundPlayer::SoundData::NTimeStep(void) const
{
	return SizeInByte()/BytePerTimeStep();
}

YSBOOL YsSoundPlayer::SoundData::Stereo(void) const
{
	return stereo;
}

unsigned int YsSoundPlayer::SoundData::BytePerTimeStep(void) const
{
	unsigned int nChannel=(YSTRUE==stereo ? 2 : 1);
	return nChannel*BytePerSample();
}

unsigned int YsSoundPlayer::SoundData::BitPerSample(void) const
{
	return bit;
}

unsigned int YsSoundPlayer::SoundData::BytePerSample(void) const
{
	return bit/8;
}

long long YsSoundPlayer::SoundData::SecToNumSample(double sec) const
{
	return SecToNumSample(sec,PlayBackRate());
}
/* static */ long long YsSoundPlayer::SoundData::SecToNumSample(double sec,unsigned int playBackRate)
{
	return (long long)(sec*(double)playBackRate);
}
double YsSoundPlayer::SoundData::NumSampleToSec(long long numSample) const
{
	return NumSampleToSec(numSample,PlayBackRate());
}
/* static */ double YsSoundPlayer::SoundData::NumSampleToSec(long long numSample,unsigned int playBackRate)
{
	return (double)numSample/(double)playBackRate;
}

unsigned int YsSoundPlayer::SoundData::PlayBackRate(void) const
{
	return rate;
}

unsigned int YsSoundPlayer::SoundData::SizeInByte(void) const
{
	return sizeInBytes;
}

YSBOOL YsSoundPlayer::SoundData::IsSigned(void) const
{
	return isSigned;
}

const unsigned char *YsSoundPlayer::SoundData::DataPointer(void) const
{
	return dat.data();
}

const unsigned char *YsSoundPlayer::SoundData::DataPointerAtTimeStep(unsigned int ts) const
{
	return dat.data()+ts*BytePerTimeStep();
}

static unsigned GetUnsigned(const unsigned char buf[])
{
	return buf[0]+buf[1]*0x100+buf[2]*0x10000+buf[3]*0x1000000;
}

static unsigned GetUnsignedShort(const unsigned char buf[])
{
	return buf[0]+buf[1]*0x100;
}



YSRESULT YsSoundPlayer::SoundData::CreateFromSigned16bitStereo(unsigned int samplingRate,std::vector <unsigned char> &wave)
{
	CleanUp();

	stereo=YSTRUE;
	bit=16;
	rate=samplingRate;
	sizeInBytes=wave.size();
	isSigned=YSTRUE;
	playBackVolumeLeft=1.0;
	playBackVolumeRight=1.0;

	dat.swap(wave);

	return YSOK;
}



YSRESULT YsSoundPlayer::SoundData::LoadWav(const char fn[])
{
	FILE *fp;
	printf("Loading %s\n",fn);
	fp=fopen(fn,"rb");
	if(fp!=NULL)
	{
		auto res=LoadWav(fp);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::LoadWav(FILE *fp)
{
	if(nullptr!=fp)
	{
		FileInStream inStream(fp);
		return LoadWav(inStream);
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::LoadWavFromMemory(long long int length,const unsigned char incoming[])
{
	MemInStream inStream(length,incoming);
	return LoadWav(inStream);
}

YSRESULT YsSoundPlayer::SoundData::LoadWav(BinaryInStream &inStream)
{
	CleanUp();

	unsigned char buf[256];
	unsigned int l;
	unsigned int fSize,hdrSize,dataSize;

	// Wave Header
	unsigned short wFormatTag,nChannels;
	unsigned nSamplesPerSec,nAvgBytesPerSec;
	unsigned short nBlockAlign,wBitsPerSample,cbSize;


	if(inStream.Fetch(buf,4)!=4)
	{
		printf("Error in reading RIFF.\n");
		return YSERR;
	}
	if(strncmp((char *)buf,"RIFF",4)!=0)
	{
		printf("Warning: RIFF not found.\n");
	}


	if(inStream.Fetch(buf,4)!=4)
	{
		printf("Error in reading file size.\n");
		return YSERR;
	}
	fSize=GetUnsigned(buf);
	printf("File Size=%d\n",fSize+8);
	// Wait, is it fSize+12?  A new theory tells that "fmt " immediately following "WAVE"
	// is a chunk???

	if(inStream.Fetch(buf,4)!=4)
	{
		printf("Error in reading WAVE signature.\n");
		return YSERR;
	}
	if(strncmp((char *)buf,"WAVE",4)!=0)
	{
		printf("Warning: WAVE signature not found\n");
	}


	bool fmtSet=false,dataSet=false;
	for(;;)
	{
		if(inStream.Fetch(buf,4)!=4)
		{
			if(true!=fmtSet)
			{
				printf("Error WAVE format not set before the end of file.\n");
				return YSERR;
			}
			if(true!=dataSet)
			{
				printf("Error WAVE data not set before the end of file.\n");
				return YSERR;
			}
			break;
		}

		buf[0]=toupper(buf[0]);
		buf[1]=toupper(buf[1]);
		buf[2]=toupper(buf[2]);
		buf[3]=toupper(buf[3]);

		if('F'==buf[0] && 'M'==buf[1] && 'T'==buf[2])
		{
			if(true==fmtSet)
			{
				printf("Error: Multiple fmt tags.\n");
				return YSERR;
			}
			if(inStream.Fetch(buf,4)!=4)
			{
				printf("Error in reading header size.\n");
				return YSERR;
			}
			hdrSize=GetUnsigned(buf);
			printf("Header Size=%d\n",hdrSize);

			//    WORD  wFormatTag;
			//    WORD  nChannels;
			//    DWORD nSamplesPerSec;
			//    DWORD nAvgBytesPerSec;
			//    WORD  nBlockAlign;
			//    WORD  wBitsPerSample;
			//    WORD  cbSize;
			if(inStream.Fetch(buf,hdrSize)!=hdrSize)
			{
				printf("Error in reading header.\n");
				return YSERR;
			}
			wFormatTag=GetUnsignedShort(buf);
			nChannels=GetUnsignedShort(buf+2);
			nSamplesPerSec=GetUnsigned(buf+4);
			nAvgBytesPerSec=GetUnsigned(buf+8);
			nBlockAlign=GetUnsignedShort(buf+12);
			wBitsPerSample=(hdrSize>=16 ? GetUnsignedShort(buf+14) : 0);
			cbSize=(hdrSize>=18 ? GetUnsignedShort(buf+16) : 0);

			printf("wFormatTag=%d\n",wFormatTag);
			printf("nChannels=%d\n",nChannels);
			printf("nSamplesPerSec=%d\n",nSamplesPerSec);
			printf("nAvgBytesPerSec=%d\n",nAvgBytesPerSec);
			printf("nBlockAlign=%d\n",nBlockAlign);
			printf("wBitsPerSample=%d\n",wBitsPerSample);
			printf("cbSize=%d\n",cbSize);

			fmtSet=true;
		}
		else if(buf[0]=='D' && buf[1]=='A' && buf[2]=='T' && buf[3]=='A')
		{
			if(true==dataSet)
			{
				printf("Error: Multiple DATA tags appeared.\n");
				return YSERR;
			}

			if(inStream.Fetch(buf,4)!=4)
			{
				printf("Error in reading data size.\n");
				return YSERR;
			}
			dataSize=GetUnsigned(buf);
			printf("Data Size=%d (0x%x)\n",dataSize,dataSize);

			dat.resize(dataSize);
			if((l=inStream.Fetch(dat.data(),dataSize))!=dataSize)
			{
				printf("Warning: File ended before reading all data.\n");
				printf("  %d (0x%x) bytes have been read\n",l,l);
			}

			dataSet=true;
		}
		else
		{
			printf("Skipping %c%c%c%c (Unknown Block)\n",buf[0],buf[1],buf[2],buf[3]);
			if(inStream.Fetch(buf,4)!=4)
			{
				printf("Error while skipping unknown block.\n");
				return YSERR;
			}

			l=GetUnsigned(buf);
			if(inStream.Skip(l)!=l)
			{
				printf("Error while skipping unknown block.\n");
				return YSERR;
			}
		}
	}

	this->stereo=(nChannels==2 ? YSTRUE : YSFALSE);
	this->bit=wBitsPerSample;
	this->sizeInBytes=dataSize;
	this->rate=nSamplesPerSec;

	if(wBitsPerSample==8)
	{
		isSigned=YSFALSE;
	}
	else
	{
		isSigned=YSTRUE;
	}

	return YSOK;
}


YSRESULT YsSoundPlayer::SoundData::ConvertTo16Bit(void)
{
	if(bit==16)
	{
		return YSOK;
	}
	else if(bit==8)
	{
		prepared=false;
		if(sizeInBytes>0 && 0<dat.size()) // ? Why did I write 0<dat.size()?  2020/04/02
		{
			std::vector <unsigned char> newDat;
			newDat.resize(sizeInBytes*2);
			for(int i=0; i<sizeInBytes; i++)
			{
				newDat[i*2]  =dat[i];
				newDat[i*2+1]=dat[i];
			}
			std::swap(dat,newDat);

			sizeInBytes*=2;
			bit=16;
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::ConvertTo8Bit(void)
{
	if(bit==8)
	{
		return YSOK;
	}
	else if(bit==16)
	{
		prepared=false;
		std::vector <unsigned char> newDat;
		newDat.resize(sizeInBytes/2);
		for(int i=0; i<sizeInBytes; i+=2)
		{
			newDat[i/2]=dat[i];
		}
		std::swap(dat,newDat);
		bit=8;
		sizeInBytes/=2;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::ConvertToStereo(void)
{
	if(stereo==YSTRUE)
	{
		return YSOK;
	}
	else
	{
		if(bit==8)
		{
			std::vector <unsigned char> newDat;
			newDat.resize(sizeInBytes*2);
			for(int i=0; i<sizeInBytes; i++)
			{
				newDat[i*2  ]=dat[i];
				newDat[i*2+1]=dat[i];
			}
			std::swap(dat,newDat);
			stereo=YSTRUE;
			sizeInBytes*=2;
			return YSOK;
		}
		else if(bit==16)
		{
			std::vector <unsigned char> newDat;
			newDat.resize(sizeInBytes*2);
			for(int i=0; i<sizeInBytes; i+=2)
			{
				newDat[i*2  ]=dat[i];
				newDat[i*2+1]=dat[i+1];
				newDat[i*2+2]=dat[i];
				newDat[i*2+3]=dat[i+1];
			}
			std::swap(dat,newDat);
			stereo=YSTRUE;
			sizeInBytes*=2;
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::Resample(int newRate)
{
	if(rate!=newRate)
	{
		prepared=false;

		const size_t nChannel=(YSTRUE==stereo ? 2 : 1);
		const size_t bytePerSample=bit/8;
		const size_t bytePerTimeStep=nChannel*bytePerSample;
		const size_t curNTimeStep=sizeInBytes/bytePerTimeStep;

		const size_t newNTimeStep=curNTimeStep*newRate/rate;
		const size_t newSize=newNTimeStep*bytePerTimeStep;

		std::vector <unsigned char> newDat;
		newDat.resize(newSize);
		{
			for(size_t ts=0; ts<newNTimeStep; ts++)
			{
				double oldTimeStepD=(double)curNTimeStep*(double)ts/(double)newNTimeStep;
				size_t oldTimeStep=(size_t)oldTimeStepD;
				double param=fmod(oldTimeStepD,1.0);
				unsigned char *newTimeStepPtr=newDat.data()+ts*bytePerTimeStep;

				for(size_t ch=0; ch<nChannel; ++ch)
				{
					if(curNTimeStep-1<=oldTimeStep)
					{
						const int value=GetSignedValueRaw(ch,curNTimeStep-1);
						SetSignedValueRaw(newTimeStepPtr+bytePerSample*ch,value);
					}
					else if(0==oldTimeStep || curNTimeStep-2<=oldTimeStep)
					{
						const double value[2]=
							{
								(double)GetSignedValueRaw(ch,oldTimeStep),
								(double)GetSignedValueRaw(ch,oldTimeStep+1)
							};
						const int newValue=(int)(value[0]*(1.0-param)+value[1]*param);
						SetSignedValueRaw(newTimeStepPtr+bytePerSample*ch,newValue);
					}
					else
					{
						const double v[4]=
							{
								(double)GetSignedValueRaw(ch,oldTimeStep-1),  // At x=-1.0
								(double)GetSignedValueRaw(ch,oldTimeStep),    // At x= 0.0
								(double)GetSignedValueRaw(ch,oldTimeStep+1),  // At x= 1.0
								(double)GetSignedValueRaw(ch,oldTimeStep+2)   // At x= 2.0
							};

						// Cubic interpolation.  Linear didn't work well.
						// axxx+bxx+cx+d=e
						// x=-1  -> -a+b-c+d=v0   (A)
						// x= 0  ->        d=v1   (B)
						// x= 1  ->  a+b+c+d=v2   (C)
						// x= 2  -> 8a+4b+2c+d=v3 (D)
						//
						// (B) =>  d=v1;
						// (A)+(C) => 2b+2d=v0+v2  => b=(v0+v2-2d)/2
						//
						// (D)-2*(B) =>  6a+2b-d=v3-2*v2
						//           =>  a=(v3-2*v2-2b+d)/6

						const double d=v[1];
						const double b=(v[0]+v[2]-2.0*d)/2.0;
						const double a=(v[3]-2.0*v[2]-2.0*b+d)/6.0;
						const double c=v[2]-a-b-d;

						double newValue=a*param*param*param
							+b*param*param
							+c*param
							+d;
						SetSignedValueRaw(newTimeStepPtr+bytePerSample*ch,(int)newValue);
					}
				}
			}
		}

		rate=newRate;
		std::swap(dat,newDat);
		sizeInBytes=newSize;
	}
	return YSOK;
}

YSRESULT YsSoundPlayer::SoundData::ConvertToMono(void)
{
	if(YSTRUE==stereo)
	{
		prepared=false;

		const size_t bytePerSample=bit/8;
		const size_t bytePerTimeStep=2*bytePerSample;
		const size_t nTimeStep=sizeInBytes/bytePerTimeStep;

		const size_t newSize=nTimeStep*bytePerSample;

		std::vector <unsigned char> newDat;
		newDat.resize(newSize);
		{
			for(size_t ts=0; ts<nTimeStep; ts++)
			{
				const int newValue=(GetSignedValueRaw(0,ts)+GetSignedValueRaw(1,ts))/2;
				unsigned char *const newTimeStepPtr=newDat.data()+ts*bytePerSample;
				SetSignedValueRaw(newTimeStepPtr,newValue);
			}

			std::swap(dat,newDat);
			sizeInBytes=newSize;
			stereo=YSFALSE;

			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsSoundPlayer::SoundData::ConvertToSigned(void)
{
	if(isSigned==YSTRUE)
	{
		return YSOK;
	}
	else
	{
		prepared=false;
		if(bit==8)
		{
			for(int i=0; i<sizeInBytes; i++)
			{
				dat[i]-=128;
			}
		}
		else if(bit==16)
		{
			for(int i=0; i<sizeInBytes-1; i+=2)
			{
				int d;
				d=dat[i]+dat[i+1]*256;
				d-=32768;
				dat[i]=d&255;
				dat[i+1]=(d>>8)&255;
			}
		}
		isSigned=YSTRUE;
	}
	return YSOK;
}

YSRESULT YsSoundPlayer::SoundData::ConvertToUnsigned(void)
{
	if(isSigned!=YSTRUE)
	{
		return YSOK;
	}
	else
	{
		prepared=false;
		if(bit==8)
		{
			for(int i=0; i<sizeInBytes; i++)
			{
				dat[i]+=128;
			}
		}
		else if(bit==16)
		{
			for(int i=0; i<sizeInBytes-1; i+=2)
			{
				int d=dat[i]+dat[i+1]*256;
				if(d>=32768)
				{
					d-=65536;
				}
				d+=32768;
				dat[i]=d&255;
				dat[i+1]=(d>>8)&255;
			}
		}
		isSigned=YSFALSE;
	}
	return YSOK;
}

YSRESULT YsSoundPlayer::SoundData::DeleteChannel(int channel)
{
	if(YSTRUE==stereo)
	{
		prepared=false;

		const size_t bytePerSample=bit/8;
		const size_t bytePerTimeStep=2*bytePerSample;
		const size_t nTimeStep=sizeInBytes/bytePerTimeStep;

		const size_t newSize=nTimeStep*bytePerSample;

		std::vector <unsigned char> newDat;
		newDat.resize(newSize);
		{
			for(size_t ts=0; ts<nTimeStep; ts++)
			{
				const int newValue=GetSignedValueRaw(1-channel,ts);
				unsigned char *const newTimeStepPtr=newDat.data()+ts*bytePerSample;
				SetSignedValueRaw(newTimeStepPtr,newValue);
			}

			std::swap(dat,newDat);
			sizeInBytes=newSize;
			stereo=YSFALSE;

			if(channel<=lastModifiedChannel)
			{
				lastModifiedChannel--;
				if(0>lastModifiedChannel)
				{
					lastModifiedChannel=0;
				}
			}

			return YSOK;
		}
	}
	return YSERR;
}

//int main(int ac,char *av[])
//{
//	YsWavFile test;
//	test.LoadWav(av[1]);
//	return 0;
//}

int YsSoundPlayer::SoundData::GetNumChannel(void) const
{
	return (YSTRUE==stereo ? 2 : 1);
}

int YsSoundPlayer::SoundData::GetLastModifiedChannel(void) const
{
	return lastModifiedChannel;
}

int YsSoundPlayer::SoundData::GetNumSample(void) const
{
	return (sizeInBytes*8/bit);
}

int YsSoundPlayer::SoundData::GetNumSamplePerChannel(void) const
{
	return GetNumSample()/GetNumChannel();
}

size_t YsSoundPlayer::SoundData::GetUnitSize(void) const
{
	return BytePerSample()*GetNumChannel();
}

size_t YsSoundPlayer::SoundData::GetSamplePosition(int atIndex) const
{
	return atIndex*GetNumChannel()*(bit/8);
}

int YsSoundPlayer::SoundData::GetSignedValueRaw(int channel,int atTimeStep) const
{
	const size_t sampleIdx=GetSamplePosition(atTimeStep);
	const size_t unitSize=GetUnitSize();

	if(sampleIdx+unitSize<=sizeInBytes && 0<=channel && channel<GetNumChannel())
	{
		int rawSignedValue=0;
		size_t offset=sampleIdx+channel*BytePerSample();
		switch(BitPerSample())
		{
		case 8:
			if(YSTRUE==isSigned)
			{
				rawSignedValue=dat[offset];
				if(128<=rawSignedValue)
				{
					rawSignedValue-=256;
				}
			}
			else
			{
				rawSignedValue=dat[offset]-128;
			}
			break;
		case 16:
			// Assume little endian
			rawSignedValue=dat[offset]+256*dat[offset+1];
			if(YSTRUE==isSigned)
			{
				if(32768<=rawSignedValue)
				{
					rawSignedValue-=65536;
				}
			}
			else
			{
				rawSignedValue-=32768;
		    }
			break;
		}
		return rawSignedValue;
	}
	return 0;
}

void YsSoundPlayer::SoundData::SetSignedValue16(int channel,int atTimeStep,int rawSignedValue)
{
	const size_t sampleIdx=GetSamplePosition(atTimeStep);
	const size_t unitSize=GetUnitSize();

	if(sampleIdx+unitSize<=sizeInBytes && 0<=channel && channel<GetNumChannel())
	{
		prepared=false;
		lastModifiedChannel=channel;
		size_t offset=sampleIdx+channel*BytePerSample();
		switch(BitPerSample())
		{
		case 8:
			if(YSTRUE==isSigned)
			{
				rawSignedValue>>=8;
				rawSignedValue&=255;
				dat[offset]=rawSignedValue;
			}
			else
			{
				rawSignedValue>>=8;
				rawSignedValue+=128;
				rawSignedValue&=255;
				dat[offset]=rawSignedValue;
			}
			break;
		case 16:
			// Assume little endian
			if(YSTRUE==isSigned)
			{
				dat[offset  ]=(rawSignedValue&255);
				dat[offset+1]=((rawSignedValue>>8)&255);
			}
			else
			{
				rawSignedValue+=32768;
				dat[offset  ]=(rawSignedValue&255);
				dat[offset+1]=((rawSignedValue>>8)&255);
		    }
			break;
		}
	}
}

int YsSoundPlayer::SoundData::GetSignedValue16(int channel,int atTimeStep) const
{
	if(16==bit)
	{
		return GetSignedValueRaw(channel,atTimeStep);
	}
	else if(8==bit)
	{
		auto value=GetSignedValueRaw(channel,atTimeStep);
		value=value*32767/127;
		return value;
	}
	return 0; // ?
}

void YsSoundPlayer::SoundData::ResizeByNumSample(long long int nSample)
{
	prepared=false;

	long long int newDataSize=nSample*GetNumChannel()*BytePerSample();
	std::vector <unsigned char> newDat;
	newDat.resize(newDataSize);

	for(long long int i=0; i<newDataSize && i<sizeInBytes; ++i)
	{
		newDat[i]=dat[i];
	}
	for(long long int i=sizeInBytes; i<newDataSize; ++i)
	{
		newDat[i]=0;
	}

	std::swap(dat,newDat);
	sizeInBytes=newDataSize;
}

void YsSoundPlayer::SoundData::SetSignedValueRaw(unsigned char *savePtr,int rawSignedValue)
{
	prepared=false;
	switch(bit)
	{
	case 8:
		if(rawSignedValue<-128)
		{
			rawSignedValue=-128;
		}
		else if(127<rawSignedValue)
		{
			rawSignedValue=127;
		}
		if(YSTRUE==isSigned)
		{
			if(0>rawSignedValue)
			{
				rawSignedValue+=256;
			}
			*savePtr=(unsigned char)rawSignedValue;
		}
		else
		{
			rawSignedValue+=128;
			*savePtr=(unsigned char)rawSignedValue;
		}
		break;
	case 16:
		if(-32768>rawSignedValue)
		{
			rawSignedValue=-32768;
		}
		else if(32767<rawSignedValue)
		{
			rawSignedValue=32767;
		}

		if(YSTRUE==isSigned)
		{
			if(0>rawSignedValue)
			{
				rawSignedValue+=65536;
			}
		}
		else
		{
			rawSignedValue+=32768;
		}

		// Assume little endian (.WAV is supposed to use little endian).
		savePtr[0]=(rawSignedValue&255);
		savePtr[1]=((rawSignedValue>>8)&255);
		break;
	}
}



std::vector <unsigned char> YsSoundPlayer::SoundData::MakeWavByteData(void) const
{
	std::vector <unsigned char> byteData;
	byteData.push_back('R');
	byteData.push_back('I');
	byteData.push_back('F');
	byteData.push_back('F');

	AddUnsignedInt(byteData,0);

	byteData.push_back('W');
	byteData.push_back('A');
	byteData.push_back('V');
	byteData.push_back('E');
	byteData.push_back('f');
	byteData.push_back('m');
	byteData.push_back('t');
	byteData.push_back(' ');

	const int nChannels=GetNumChannel();
	const int nBlockAlign=nChannels*BitPerSample()/8;
	const int nAvgBytesPerSec=PlayBackRate()*nBlockAlign;

	AddUnsignedInt(byteData,16);
	AddUnsignedShort(byteData,1); // wFormatTag=1
	AddUnsignedShort(byteData,(unsigned short)GetNumChannel());
	AddUnsignedInt(byteData,PlayBackRate());  // nSamplesPerSec
	AddUnsignedInt(byteData,nAvgBytesPerSec);
	AddUnsignedShort(byteData,(unsigned short)nBlockAlign);
	AddUnsignedShort(byteData,(unsigned short)BitPerSample()); // wBitsPerSample

	byteData.push_back('d');
	byteData.push_back('a');
	byteData.push_back('t');
	byteData.push_back('a');

	AddUnsignedInt(byteData,SizeInByte());

	for(int i=0; i<SizeInByte(); ++i)
	{
		byteData.push_back(dat[i]);
	}

	auto totalSize=byteData.size()-8;
	byteData[4]=totalSize&255;
	byteData[5]=(totalSize>>8)&255;
	byteData[6]=(totalSize>>16)&255;
	byteData[7]=(totalSize>>24)&255;

	return byteData;
}

/* static */ void YsSoundPlayer::SoundData::AddUnsignedInt(std::vector <unsigned char> &byteData,unsigned int dat)
{
	unsigned char buf[4];
	buf[0]=dat&255;
	buf[1]=(dat>>8)&255;
	buf[2]=(dat>>16)&255;
	buf[3]=(dat>>24)&255;
	byteData.push_back(buf[0]);
	byteData.push_back(buf[1]);
	byteData.push_back(buf[2]);
	byteData.push_back(buf[3]);
}

/* static */ void YsSoundPlayer::SoundData::AddUnsignedShort(std::vector <unsigned char> &byteData,unsigned short dat)
{
	unsigned char buf[2];
	buf[0]=dat&255;
	buf[1]=(dat>>8)&255;
	byteData.push_back(buf[0]);
	byteData.push_back(buf[1]);
}




YsSoundPlayer::Stream::Stream()
{
	api=CreateAPISpecificData();
}
YsSoundPlayer::Stream::~Stream()
{
	DeleteAPISpecificData(api);
	api=nullptr;
}


#define ALSA_USE_INTERVAL_TIMER

#include <algorithm>
#include <stdio.h>
#include <vector>
#include <iostream>
#include "yssimplesound.h"


#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>



// By uncommenting the following, KeepPlaying is unnecessary if the process has only one thread.
// #define ALSA_USE_INTERVAL_TIMER


/*
2015/10/21  About a broken promise of ALSA asynchronous playback.

I am writing this memo so that I won't waste any more time to explore possibility of eliminating KeepPlaying function by ALSA's asynchronous-playback feature.

After numerous frustrating attempts, I conclude that there is no such thing called "asynchronous playback" in ALSA.

There is a working example pcm.c, which you can download from:
    http://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2pcm_8c-example.html

In this example, asnychronous playback looks to be working.  You can add:
    printf("*\n");

at the beginning of async_callback function to confirm the function is called if you start the program with "-m async" options.

It appears to be playing audio asynchronously, but it is not.

Try commenting out sleep(1) in the async_loop function.  The call-back function async_callback won't be called unless you keep your main thread idle.

If I let my main thread do something useful, the playback stops.  
It is not even asynchronous.  Yes, async_callback is playing audio, but, asynchronous with what?  Nothing.

The term "asynchronous playback" implies that I can let the main thread do something useful while ALSA is playing an audio data concurrently.
However, in ALSA, you need to stop your main thread while ALSA's so-called asynchronous-playback is playing an audio data.
It is absolutely useless.

Better use polling-based playback.  At least that way you can let your main thread do something useful, while keeping the audio played background.




2023/10/17
ALSA is the most difficult and the worst sound API I have ever used.  Especially, the API requires constant
polling, which is very problematic.  This requirement substantially restricts the program structure.
And many programmers are having difficulties (search StackOverflow).

As a result, among macOS, Windows, and Linux versions of this YsSimpleSound library, only Linux version
required constant call to KeepPlaying function.

I let my students use this library for an assignment.  It was a disaster.  So many students didn't bother
calling KeepPlaying in the main loop because it was unnecessary in Windows and macOS.

But, that was before.

Finally!  Interval Timer solved the problem.  One hiccup of the interval timer was that the signal
SIGALRM could be sent to any thread.  If I don't do anything if the thread was not the audio thread,
it at least prevented crash, but could not feed the next piece of wave to ALSA in time.

The solution was to force sending SIGALRM to the audio thread to pthread_signal if another thread
ended up receiving te signal.

Still there was a possibility of reentrance, which was prevented by not set up interval timer.
Instead, just set up a one-time timer, and then set a new timer in the timer handler.

The last issue was ALSA seems to freeze randomly if I call one of snd_pcm_drop, snd_pcm_prepare, or snd_pcm_wait 
while the PCM state is PREPARED.  I was doing it when PCM state was not RUNNING.  But, once I figured,
simply not calling them when the state is PREPARED solved the problem.

*/



class YsSoundPlayer::APISpecificData
{
public:
	class PlayingSound
	{
	public:
		SoundData *dat;
		unsigned int ptr;
		YSBOOL loop;
		YSBOOL stop;

		void Make(SoundData &dat,YSBOOL loop);
	};

	class PlayingStream
	{
	public:
		Stream *dat=nullptr;
		unsigned int ptr=0;

		void Make(Stream &dat);
	};

	std::vector <PlayingSound> playing;
	std::vector <PlayingStream> playingStream;

	snd_pcm_t *handle=nullptr;
	snd_async_handler_t *asyncHandler=nullptr;
	snd_pcm_hw_params_t *hwParam=nullptr;
	snd_pcm_sw_params_t *swParam=nullptr;

	unsigned int nChannel=2,rate=44100;
	snd_pcm_uframes_t pcmBufSize=0;
	snd_pcm_uframes_t nPeriod=0;

	const unsigned int bytePerSample=2;
	unsigned int bytePerTimeStep=4;
	unsigned int bufSizeInNStep=0;
	unsigned int bufSizeInByte=0;
	unsigned char *writeBuf=nullptr;

	static std::vector <YsSoundPlayer::APISpecificData *> activePlayers;

	APISpecificData();
	~APISpecificData();
	void CleanUp(void);
	YSRESULT Start(void);
	YSRESULT End(void);

	static void KeepPlayingCallBack(snd_async_handler_t *handler);

	void KeepPlaying(void);

	void DiscardEnded(void);

	/* writePtr, wavPtr is in number of samples, not number of bytes.
	   Returns true if loop==YSFALSE and the buffer moved all the way to the end.
	   writePtr is updated.

	   writePtr is pointer to the write buffer, not the input SoundData.

	   However, actual number of steps written to the hardware (or the driver's buffer) depends on the
	   subsequent ALSA function.  Therefore, actually updating the pointer should take place after doing so.
	*/
	bool PopulateWriteBuffer(unsigned int &writePtr,unsigned int wavPtr,const SoundData *wavFile,YSBOOL loop,int nThSound);

	void PrintState(int errCode);

	double GetCurrentPosition(const SoundData &dat) const;

	static void TimerInterrupt(int signum);
	static void UserInterrupt(int signum);
	static void StartTimer(void);
	static void EndTimer(void);
	static void MaskTimer(void);
	static void UnmaskTimer(void);
	class TimerMaskGuard;
};

std::vector <YsSoundPlayer::APISpecificData *> YsSoundPlayer::APISpecificData::activePlayers;

class YsSoundPlayer::Stream::APISpecificData
{
public:
	SoundData playing,standBy;
};

void YsSoundPlayer::APISpecificData::PlayingSound::Make(SoundData &dat,YSBOOL loop)
{
	this->dat=&dat;
	this->ptr=0;
	this->loop=loop;
	this->stop=YSFALSE;
}

void YsSoundPlayer::APISpecificData::PlayingStream::Make(Stream &dat)
{
	this->dat=&dat;
	this->ptr=0;
}

class YsSoundPlayer::SoundData::APISpecificDataPerSoundData
{
public:
	APISpecificDataPerSoundData();
	~APISpecificDataPerSoundData();
	void CleanUp(void);
};


#ifdef ALSA_USE_INTERVAL_TIMER

// Another attempt to un-necessitate KeepPlaying function. >>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <pthread.h>
#include <thread>
#include <mutex>

// https://stackoverflow.com/questions/21091000/how-to-get-thread-id-of-a-pthread-in-linux-c-program
static pthread_t audioThread;
static bool timerRunning=false;
static struct itimerval prevTimer;
static void (*prevSignalHandler)(int);
static int maskLevel=0;

void YsSoundPlayer::APISpecificData::TimerInterrupt(int signum)
{
	auto thisThread=pthread_self();
	if(thisThread!=audioThread)
	{
		// This sigprocmask is supposed to disable signal sent to other thread.
		// https://devarea.com/linux-handling-signals-in-a-multithreaded-application/
		// But, doesn't work.
		//static sigset_t sigset;
		//sigemptyset(&sigset);
		//sigaddset(&sigset,SIGALRM);
		//sigprocmask(SIG_BLOCK,&sigset,NULL);

		// This handler needs to be invoked in the audioThread, but if I
		// do it, somehow the program hangs in player->KeepPlaying()
		// It looks to crash when nothing is playing.
		pthread_kill(audioThread,SIGALRM);

		return;
	}

	if(0==maskLevel)
	{
		// Just in case.  If another threads intecepts the signal, and shot SIGALRM by pthread_kill,
		// there might be a lag until the audio thread receives the signal.
		// If the signal comes in while the timer signal is masked, do not call KeepPlaying, 
		// but instead schedule next timer.
		for(auto player : activePlayers)
		{
			player->KeepPlaying();
		}
	}

	struct itimerval timer,prevTimer;
	timer.it_value.tv_sec=0;
	timer.it_value.tv_usec=2000; // 2ms timer.
	timer.it_interval.tv_sec=0;
	timer.it_interval.tv_usec=0;
	setitimer(ITIMER_REAL,&timer,&prevTimer);
}
void YsSoundPlayer::APISpecificData::StartTimer(void)
{
	if(true!=timerRunning)
	{
		audioThread=pthread_self();

		struct itimerval timer;
		timer.it_value.tv_sec=0;
		timer.it_value.tv_usec=2000; // 2ms timer.
		timer.it_interval.tv_sec=0;
		timer.it_interval.tv_usec=0;
		// Do not repeat.  Next timer will be set in the handler.
		// This way prevents re-entrance problem.
		prevSignalHandler=signal(SIGALRM,TimerInterrupt);
		setitimer(ITIMER_REAL,&timer,&prevTimer);
		timerRunning=true;
	}
}
void YsSoundPlayer::APISpecificData::EndTimer(void)
{
	if(true==timerRunning)
	{
		struct itimerval timer;
		setitimer(ITIMER_REAL,&prevTimer,&timer);
		signal(SIGALRM,SIG_IGN);
		timerRunning=false;
		std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Give 5 chances to flush pending signals.
		signal(SIGALRM,prevSignalHandler);
	}
}
void YsSoundPlayer::APISpecificData::MaskTimer(void)
{
	if(true==timerRunning)
	{
		if(0==maskLevel)
		{
			sigset_t sigset;
			sigemptyset(&sigset);
			sigaddset(&sigset,SIGALRM);
			sigprocmask(SIG_BLOCK,&sigset,NULL);
		}
		++maskLevel;
	}
}
void YsSoundPlayer::APISpecificData::UnmaskTimer(void)
{
	if(true==timerRunning)
	{
		if(0<maskLevel)
		{
			--maskLevel;
			if(0==maskLevel)
			{
				sigset_t sigset;
				sigemptyset(&sigset);
				sigaddset(&sigset,SIGALRM);
				sigprocmask(SIG_UNBLOCK,&sigset,NULL);
			}
		}
	}
}
class YsSoundPlayer::APISpecificData::TimerMaskGuard
{
public:
	TimerMaskGuard()
	{
		MaskTimer();
	}
	~TimerMaskGuard()
	{
		UnmaskTimer();
	}
};
// Another attempt to un-necessitate KeepPlaying function. <<
#else
void YsSoundPlayer::APISpecificData::TimerInterrupt(int){}
void YsSoundPlayer::APISpecificData::UserInterrupt(int){}
void YsSoundPlayer::APISpecificData::StartTimer(void){}
void YsSoundPlayer::APISpecificData::EndTimer(void){}
void YsSoundPlayer::APISpecificData::MaskTimer(void){}
void YsSoundPlayer::APISpecificData::UnmaskTimer(void){}
class YsSoundPlayer::APISpecificData::TimerMaskGuard
{
public:
};
#endif


////////////////////////////////////////////////////////////



YsSoundPlayer::APISpecificData::APISpecificData() :
	bytePerSample(2) // 2 bytes for 16-bit mono stream per time step
{
	for(auto &p : playing)
	{
		p.dat=nullptr;
		p.ptr=0;
	}

	handle=nullptr;
	hwParam=nullptr;
	swParam=nullptr;
	asyncHandler=nullptr;
	nChannel=0;
	rate=0;
	nPeriod=0;
	pcmBufSize=0;
	writeBuf=nullptr;

	CleanUp();
}
YsSoundPlayer::APISpecificData::~APISpecificData()
{
	End();
	CleanUp();
}

void YsSoundPlayer::APISpecificData::CleanUp(void)
{
}

YSRESULT YsSoundPlayer::APISpecificData::Start(void)
{
	int res=snd_pcm_open(&handle,"default",SND_PCM_STREAM_PLAYBACK,SND_PCM_NONBLOCK);
	if(0>res)
	{
		printf("Cannot open PCM device.\n");
		handle=nullptr;
		return YSERR;
	}

	snd_pcm_hw_params_alloca(&hwParam);
	snd_pcm_hw_params_any(handle,hwParam);
	snd_pcm_hw_params_set_access(handle,hwParam,SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format(handle,hwParam,SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_channels(handle,hwParam,2);

	// ? Why did I make it 8000 before ?
	// Apparently 8000Hz has some problems in play back.
	// 
	unsigned int rateRequest=44100;
	int dir;  // What's dir?
	snd_pcm_hw_params_set_rate_near(handle,hwParam,&rateRequest,&dir);

	// Make buffer size small, otherwise it's going to give me too much latency.
	// Wanted 40ms, but didn't work.
	// 2020/01/12
	//   After numerous attempts, period size of 1/25 seconds (40ms) and buffer size double of it (80ms)
	//   Works well without giving buffer underrun.
	//   It's Linux after all.  Don't expect quality audio from Linux.
	snd_pcm_uframes_t periodSizeRequest;
	periodSizeRequest=rateRequest/25;
	snd_pcm_hw_params_set_period_size(handle,hwParam,periodSizeRequest,0);

	snd_pcm_uframes_t bufferSizeRequest;
	bufferSizeRequest=periodSizeRequest*2;
	snd_pcm_hw_params_set_buffer_size_near(handle,hwParam,&bufferSizeRequest);

	if(0>snd_pcm_hw_params(handle,hwParam))
	{
		printf("Cannot set hardward parameters.\n");
		return YSERR;
	}

	snd_pcm_hw_params_get_channels(hwParam,&nChannel);
	snd_pcm_hw_params_get_rate(hwParam,&rate,&dir);
	snd_pcm_hw_params_get_period_size(hwParam,&nPeriod,&dir);
	snd_pcm_hw_params_get_buffer_size(hwParam,&pcmBufSize);
	printf("%d channels, %d Hz, %d periods, %d frames buffer.\n",
		   nChannel,rate,(int)nPeriod,(int)pcmBufSize);

	bytePerTimeStep=bytePerSample*nChannel;
	bufSizeInNStep=nPeriod*4;
	bufSizeInByte=nChannel*bufSizeInNStep*bytePerTimeStep;
	if(nullptr!=writeBuf)
	{
		delete [] writeBuf;
	}
	writeBuf=new unsigned char [bufSizeInByte];
	for(int i=0; i<bufSizeInByte; ++i)
	{
		writeBuf[i]=0;
	}

	// snd_pcm_sw_params_alloca(&swParam);
	// snd_async_add_pcm_handler(&asyncHandler,handle,KeepPlayingCallBack,this);


	snd_pcm_prepare(handle);
	if(0==snd_pcm_wait(handle,1000))
	{
		printf("snd_pcm_wait timed out.\n");
		printf("There may be no sound.\n");
	}
	snd_pcm_start(handle);

	printf("YsSoundPlayer started.\n");


	snd_pcm_drop(handle);
	snd_pcm_prepare(handle);
	snd_pcm_wait(handle,1);


	auto found=std::find(activePlayers.begin(),activePlayers.end(),this);
	if(activePlayers.end()==found)
	{
		activePlayers.push_back(this);
	}

	StartTimer();

	return YSOK;
}
YSRESULT YsSoundPlayer::APISpecificData::End(void)
{
	CleanUp();

	{
		TimerMaskGuard tmg;

		if(nullptr!=hwParam)
		{
			// This gives an error => snd_pcm_hw_params_free(hwParam);
			hwParam=nullptr;
		}
		if(nullptr!=swParam)
		{
			// This gives an error => snd_pcm_sw_params_free(swParam);
			swParam=nullptr;
		}
		if(nullptr!=handle)
		{
			snd_pcm_close(handle);
			handle=nullptr;
		}
		if(nullptr!=writeBuf)
		{
			delete [] writeBuf;
			writeBuf=nullptr;
		}

		auto found=std::find(activePlayers.begin(),activePlayers.end(),this);
		if(activePlayers.end()!=found)
		{
			activePlayers.erase(found);
		}
	}
	if(0==activePlayers.size())
	{
		EndTimer();
	}

	return YSOK;
}

/* static */ void YsSoundPlayer::APISpecificData::KeepPlayingCallBack(snd_async_handler_t *handler)
{
	printf("*\n");
}

void YsSoundPlayer::APISpecificData::KeepPlaying(void)
{
	if(nullptr!=handle)
	{
		snd_pcm_state_t pcmState=snd_pcm_state(handle);

		snd_pcm_status_t *pcmStatus;
		snd_pcm_status_alloca(&pcmStatus);
		snd_pcm_status(handle,pcmStatus);

		if(SND_PCM_STATE_RUNNING!=pcmState && SND_PCM_STATE_PREPARED!=pcmState)
		{
			// 2023/10/19
			// Call to the following three functions while the PCM state is PREPARED might freeze.
			// If it is PREPARED, probably I don't have to do any of the three.
			snd_pcm_drop(handle);
			snd_pcm_prepare(handle);
			snd_pcm_wait(handle,1);
		}

		const int nAvail=(unsigned int)snd_pcm_avail(handle);

		if(nPeriod<=nAvail)
		{
			unsigned int writeBufFilledInNStep=0;
			int nThSound=0;
			for(auto &p : playing)
			{
				if(nullptr!=p.dat)
				{
					unsigned int writePtr=0;
					PopulateWriteBuffer(writePtr,p.ptr,p.dat,p.loop,nThSound);
					writeBufFilledInNStep=std::max(writePtr,writeBufFilledInNStep);
					++nThSound;
				}
			}
			for(auto &p : playingStream)
			{
				// p.ptr is pointer in number of samples (not in number of bytes).
				if(nullptr!=p.dat)
				{
					unsigned int writePtr=0;
					YSBOOL loop=YSFALSE;
					if(0!=p.dat->api->playing.NTimeStep())
					{
						bool firstBufferDone=PopulateWriteBuffer(writePtr,p.ptr,&p.dat->api->playing,loop,nThSound);
						if(true==firstBufferDone)
						{
							// PopulateWriteBuffer returning true means the buffer is gone to the end.
							if(0!=p.dat->api->standBy.NTimeStep())
							{
								PopulateWriteBuffer(writePtr,0,&p.dat->api->standBy,loop,nThSound);
							}
						}
						++nThSound;
						writeBufFilledInNStep=std::max(writePtr,writeBufFilledInNStep);
					}
				}
			}

			if(0<nThSound)
			{
				int nWritten=(0<writeBufFilledInNStep ? snd_pcm_writei(handle,writeBuf,writeBufFilledInNStep) : 0);
				if(nWritten==-EAGAIN)
				{
				}
				else if(-EPIPE==nWritten || -EBADFD==nWritten)
				{
					snd_pcm_prepare(handle);
					snd_pcm_wait(handle,1);
					printf("ALSA: Recover from underrun\n");
				}
				else if(0>nWritten)
				{
					PrintState(-nWritten);
				}
				else if(0<nWritten)
				{
					int outPlaybackRate=this->rate;
					for(auto &p : playing)
					{
						if(nullptr!=p.dat)
						{
							if(YSTRUE!=p.loop)
							{
								p.ptr+=nWritten*p.dat->PlayBackRate()/outPlaybackRate;
								if(p.dat->NTimeStep()<=p.ptr)
								{
									p.dat=NULL;
									p.ptr=0;
								}
							}
							else
							{
								p.ptr+=nWritten*p.dat->PlayBackRate()/outPlaybackRate;
								while(p.ptr>=p.dat->NTimeStep())
								{
									p.ptr-=p.dat->NTimeStep();
								}
							}
						}
					}
					for(auto &p : playingStream)
					{
						if(nullptr!=p.dat)
						{
							p.ptr+=nWritten*p.dat->api->playing.PlayBackRate()/outPlaybackRate;
							if(p.dat->api->playing.NTimeStep()<=p.ptr)
							{
								p.ptr-=p.dat->api->playing.NTimeStep();
								p.dat->api->playing.MoveFrom(p.dat->api->standBy);
								p.dat->api->standBy.CleanUp();
								if(p.dat->api->playing.NTimeStep()<=p.ptr)
								{
									p.dat->api->playing.CleanUp();
									p.ptr=0;
								}
							}
							if(0==p.dat->api->playing.NTimeStep())
							{
								p.ptr=0;
							}
						}
					}
				}
			}
		}
		else
		{
			snd_pcm_state_t state=snd_pcm_state(handle);
			if(SND_PCM_STATE_RUNNING!=state)
			{
				PrintState(0);
				snd_pcm_prepare(handle);
				snd_pcm_wait(handle,1);
				printf("ALSA: Recover from state\n");
			}
		}

		DiscardEnded();
	}
}

void YsSoundPlayer::APISpecificData::DiscardEnded(void)
{
	for(long long int i=playing.size()-1; 0<=i; --i)
	{
		if(nullptr==playing[i].dat)
		{
			playing[i]=playing.back();
			playing.resize(playing.size()-1);
		}
	}

	// Question: Do I have to snd_pcm_drop here or not?
	// if(0==playing.size())
	// {
	// 	snd_pcm_drop(handle);
	// }
}

bool YsSoundPlayer::APISpecificData::PopulateWriteBuffer(unsigned int &writePtr,unsigned int wavPtr,const SoundData *wavFile,YSBOOL loop,int nThSound)
{
	TimerMaskGuard tmg;

	bool notLoopAndAllTheWayToEnd=false;

	int64_t numSamplesIn=wavFile->GetNumSamplePerChannel();
	int64_t numSamplesOut=bufSizeInNStep;

	int numChannelsIn=wavFile->GetNumChannel();
	int numChannelsOut=this->nChannel;

	int playbackRate=this->rate;

	int balance=0;
	int inChannel1=numChannelsIn-1;
	while(writePtr<bufSizeInNStep && wavPtr<numSamplesIn)
	{
		short chOut[2];
		chOut[0]=wavFile->GetSignedValue16(0,wavPtr);
		chOut[1]=wavFile->GetSignedValue16(inChannel1,wavPtr);

		for(int outCh=0; outCh<numChannelsOut; ++outCh)
		{
			if(0==nThSound)
			{
				writeBuf[writePtr*bytePerTimeStep+outCh*2  ]=chOut[outCh]&255;
				writeBuf[writePtr*bytePerTimeStep+outCh*2+1]=(chOut[outCh]>>8)&255;
			}
			else
			{
				int c=  (int)writeBuf[writePtr*bytePerTimeStep+outCh*2]
				     +(((int)writeBuf[writePtr*bytePerTimeStep+outCh*2+1])<<8);
				c=(c&0x7fff)-(c&0x8000);
				auto v=chOut[outCh]+c;
				if(32767<v)
				{
					v=32767;
				}
				if(v<-32768)
				{
					v=-32768;
				}
				writeBuf[writePtr*bytePerTimeStep+outCh*2  ]=v&0xff;
				writeBuf[writePtr*bytePerTimeStep+outCh*2+1]=((v>>8)&0xff);
			}
			++writePtr;

			balance-=wavFile->PlayBackRate();
			while(balance<0)
			{
				balance+=playbackRate;
				++wavPtr;
			}

			if(wavFile->NTimeStep()<=wavPtr)
			{
				if(YSTRUE!=loop)
				{
					notLoopAndAllTheWayToEnd=true;
				}
				else
				{
					wavPtr=0;
				}
			}
		}
	}
	if(0==nThSound)
	{
		for(auto ptr=writePtr; ptr<bufSizeInNStep; ++ptr)
		{
			for(int outCh=0; outCh<numChannelsOut; ++outCh)
			{
				writeBuf[ptr*bytePerTimeStep+outCh*2  ]=0;
				writeBuf[ptr*bytePerTimeStep+outCh*2+1]=0;
			}
		}
	}

	return notLoopAndAllTheWayToEnd;
}

void YsSoundPlayer::APISpecificData::PrintState(int errCode)
{
	TimerMaskGuard tmg;

	if(0==errCode)
	{
	}
	else if(EBADFD==errCode)
	{
		printf("EBADFD\n");
	}
	else if(EPIPE==errCode)
	{
		printf("EPIPE\n");
	}
	else if(ESTRPIPE==errCode)
	{
		printf("ESTRPIPE\n");
	}
	else if(EAGAIN==errCode)
	{
		printf("EAGAIN\n");
	}
	else
	{
		printf("Unknown error.\n");
	}

	snd_pcm_state_t state=snd_pcm_state(handle);
	switch(state)
	{
	case SND_PCM_STATE_OPEN:
		printf("SND_PCM_STATE_OPEN\n");
		break;
	case SND_PCM_STATE_SETUP:
		printf("SND_PCM_STATE_SETUP\n");
		break;
	case SND_PCM_STATE_PREPARED:
		printf("SND_PCM_STATE_PREPARED\n");
		break;
	case SND_PCM_STATE_RUNNING:
		printf("SND_PCM_STATE_RUNNING\n");
		break;
	case SND_PCM_STATE_XRUN:
		printf("SND_PCM_STATE_XRUN\n");
		break;
	case SND_PCM_STATE_DRAINING:
		printf("SND_PCM_STATE_DRAINING\n");
		break;
	case SND_PCM_STATE_PAUSED:
		printf("SND_PCM_STATE_PAUSED\n");
		break;
	case SND_PCM_STATE_SUSPENDED:
		printf("SND_PCM_STATE_SUSPENDED\n");
		break;
	case SND_PCM_STATE_DISCONNECTED:
		printf("SND_PCM_STATE_DISCONNECTED\n");
		break;
	}
}

double YsSoundPlayer::APISpecificData::GetCurrentPosition(const SoundData &dat) const
{
	TimerMaskGuard tmg;

 	for(auto &p : playing)
	{
		if(&dat==p.dat)
		{
			return (double)p.ptr/(double)dat.PlayBackRate();
		}
	}
	return 0.0;
}

////////////////////////////////////////////////////////////

YsSoundPlayer::APISpecificData *YsSoundPlayer::CreateAPISpecificData(void)
{
	return new APISpecificData;
}
void YsSoundPlayer::DeleteAPISpecificData(APISpecificData *ptr)
{
	delete ptr;
}

YSRESULT YsSoundPlayer::StartAPISpecific(void)
{
	return api->Start();
}
YSRESULT YsSoundPlayer::EndAPISpecific(void)
{
	return api->End();
}

void YsSoundPlayer::SetVolumeAPISpecific(SoundData &dat,float leftVol,float rightVol)
{
}

void YsSoundPlayer::KeepPlayingAPISpecific(void)
{
#ifndef ALSA_USE_INTERVAL_TIMER
	APISpecificData::TimerMaskGuard tmg;
	api->KeepPlaying();
#endif
}

YSRESULT YsSoundPlayer::PlayOneShotAPISpecific(SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;

	for(auto &p : api->playing)
	{
		if(p.dat==&dat)
		{
			return YSOK;
		}
	}

	YsSoundPlayer::APISpecificData::PlayingSound p;
	p.Make(dat,YSFALSE);
	api->playing.push_back(p);

	return YSOK;
}
YSRESULT YsSoundPlayer::PlayBackgroundAPISpecific(SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;

	for(auto &p : api->playing)
	{
		if(p.dat==&dat)
		{
			return YSOK;
		}
	}

	YsSoundPlayer::APISpecificData::PlayingSound p;
	p.Make(dat,YSTRUE);
	api->playing.push_back(p);

	return YSOK;
}

YSBOOL YsSoundPlayer::IsPlayingAPISpecific(const SoundData &dat) const
{
	APISpecificData::TimerMaskGuard tmg;

	for(auto &p : api->playing)
	{
		if(&dat==p.dat)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

double YsSoundPlayer::GetCurrentPositionAPISpecific(const SoundData &dat) const
{
	APISpecificData::TimerMaskGuard tmg;
	return api->GetCurrentPosition(dat);
}

void YsSoundPlayer::StopAPISpecific(SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;

	YSBOOL stopped=YSFALSE;

	for(auto &p : api->playing)
	{
		if(&dat==p.dat)
		{
			p.ptr=0;
			p.dat=nullptr;
			stopped=YSTRUE;
		}
	}

	if(YSTRUE==stopped)
	{
		api->DiscardEnded();
		snd_pcm_drop(api->handle);
	}
}

void YsSoundPlayer::PauseAPISpecific(SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;
}

void YsSoundPlayer::ResumeAPISpecific(SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;
}

////////////////////////////////////////////////////////////

YsSoundPlayer::SoundData::APISpecificDataPerSoundData::APISpecificDataPerSoundData()
{
	APISpecificData::TimerMaskGuard tmg;
	CleanUp();
}
YsSoundPlayer::SoundData::APISpecificDataPerSoundData::~APISpecificDataPerSoundData()
{
	APISpecificData::TimerMaskGuard tmg;
	CleanUp();
}
void YsSoundPlayer::SoundData::APISpecificDataPerSoundData::CleanUp(void)
{
}

////////////////////////////////////////////////////////////

YsSoundPlayer::SoundData::APISpecificDataPerSoundData *YsSoundPlayer::SoundData::CreateAPISpecificData(void)
{
	return new APISpecificDataPerSoundData;
}
void YsSoundPlayer::SoundData::DeleteAPISpecificData(APISpecificDataPerSoundData *ptr)
{
	delete ptr;
}

bool YsSoundPlayer::SoundData::IsPrepared(YsSoundPlayer &)
{
	return prepared;
}

YSRESULT YsSoundPlayer::SoundData::PreparePlay(YsSoundPlayer &player)
{
	APISpecificData::TimerMaskGuard tmg;
	Resample(player.api->rate);
	ConvertToMono();
	ConvertTo16Bit();
	ConvertToSigned();
	return YSOK;
}

void YsSoundPlayer::SoundData::CleanUpAPISpecific(void)
{
	APISpecificData::TimerMaskGuard tmg;
	api->CleanUp();
}

////////////////////////////////////////////////////////////

YsSoundPlayer::Stream::APISpecificData *YsSoundPlayer::Stream::CreateAPISpecificData(void)
{
	APISpecificData *api=new APISpecificData;
	return api;
}
void YsSoundPlayer::Stream::DeleteAPISpecificData(APISpecificData *api)
{
	delete api;
}

YSRESULT YsSoundPlayer::StartStreamingAPISpecific(Stream &stream,StreamingOption)
{
	APISpecificData::TimerMaskGuard tmg;
	for(auto playingStream : this->api->playingStream)
	{
		if(playingStream.dat==&stream)
		{
			return YSOK; // Already playing
		}
	}

	APISpecificData::PlayingStream newStream;
	newStream.Make(stream);
	this->api->playingStream.push_back(newStream);

	return YSOK;
}
void YsSoundPlayer::StopStreamingAPISpecific(Stream &stream)
{
	APISpecificData::TimerMaskGuard tmg;
	for(int i=0; i<this->api->playingStream.size(); ++i)
	{
		if(this->api->playingStream[i].dat==&stream)
		{
			this->api->playingStream[i]=this->api->playingStream.back();
			this->api->playingStream.pop_back();
		}
	}
}
YSBOOL YsSoundPlayer::StreamPlayerReadyToAcceptNextNumSampleAPISpecific(const Stream &stream,unsigned int) const
{
	APISpecificData::TimerMaskGuard tmg;
	if(0==stream.api->standBy.NTimeStep())
	{
		return YSTRUE;
	}
	return YSFALSE;
}
YSRESULT YsSoundPlayer::AddNextStreamingSegmentAPISpecific(Stream &stream,const SoundData &dat)
{
	APISpecificData::TimerMaskGuard tmg;
	if(0==stream.api->playing.NTimeStep())
	{
		stream.api->playing.CopyFrom(dat);
		return YSOK;
	}
	if(0==stream.api->standBy.NTimeStep())
	{
		stream.api->standBy.CopyFrom(dat);
		return YSOK;
	}
	return YSERR;
}
