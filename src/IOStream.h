#pragma once
#define QT_NO_FOREACH 
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

#include <future>
#include <thread>
#include <chrono>
#include <mutex>
#include <functional>



#include "GLpainter.h"
#include "Zstream.h"



class OutputStream
{
    OutputStream():m_isRunning(false){}//singelton
    std::ofstream m_fileStream;
    std::future<void> m_thrd;
    bool m_isRunning;
    
    mutable std::mutex m_mtx;
   


    static void privRun(OutputStream* sP)
    {
        Zstream& zs = Zstream::i();
        
        std::array<uint16_t, 2> wh = { uint16_t(zs.getWidth()),uint16_t(zs.getHeight()) };
        char buffer[1024] = { 0 };
        float* buffer_f = reinterpret_cast<float*>(buffer);
        memcpy(buffer, ".3ds", 4);
        uint32_t flags = zs.getFlags();
        buffer_f[1] = *reinterpret_cast<const float*>(&flags);
        buffer_f[2] = zs.getZ2mm();

        buffer_f[3] = *reinterpret_cast<const float*>(&wh);
        QMatrix3x3 k = zs.getK().transposed();
        memcpy(buffer_f + 4, k.data(), 9 * sizeof(float));
        buffer_f[13] = zs.getFPS();

        sP->m_fileStream.write(buffer, 1024);
        int dataSz = wh[0] * wh[1];

        char frameHeader[256] = { 0 };
        std::vector<uint16_t> buffvec(dataSz * 2);
        
        uint32_t waitTimeUS = uint32_t(1e6 / zs.getFPS());
        while (true)
        {
            zs.copyZdata(buffvec.data());
            zs.copyCdata(buffvec.data() + dataSz);
            
            sP->m_fileStream.write(frameHeader, 256);
            sP->m_fileStream.write(reinterpret_cast<char*>(buffvec.data()), buffvec.size() * sizeof(uint16_t));
            std::this_thread::sleep_for(std::chrono::microseconds(waitTimeUS));
            {
                std::lock_guard<std::mutex> grd(sP->m_mtx);
                if (!sP->m_isRunning)
                    break;

            }
            
        }
        sP->m_fileStream.close();
        

    }
public:
    static OutputStream& i()
    {
        static OutputStream    instance; // Guaranteed to be destroyed.

        return instance;
    }

  

    bool start(const std::string& fn)
    {
        m_fileStream.open(fn, std::ios::binary);
        if (!m_fileStream.is_open())
            return false;
        
        
        
        m_isRunning = true;
        m_thrd = std::async(std::launch::async, OutputStream::privRun, this);
        
        return true;


    }
    void stop()
    {
        {
            
            m_isRunning = false;
        }
        
        m_thrd.wait();


    }
    bool isOpen() const { std::lock_guard<std::mutex> grd(m_mtx); return m_isRunning; }

   
};



class InputStream
{
    class BaseStream
    {
        std::mutex m_mtx;
    protected:
        bool m_isRunning;
        bool m_isPaused;

        template<class T, class K>
        static void privInnerRunStreamCopy(const BaseStream* sP,const T* zdata, const K* colData)
        {
            Zstream::i().setBuffer(zdata, colData);
          
            
            while (sP->m_isPaused && sP->m_isRunning)
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
        
        }
    public:
        BaseStream() :m_isRunning(false), m_isPaused(false){};

        void start() { std::lock_guard<std::mutex> grd(m_mtx); m_isRunning = true; m_isPaused = false; }
        void stop() { std::lock_guard<std::mutex> grd(m_mtx); m_isRunning = false; }
        void pause() { std::lock_guard<std::mutex> grd(m_mtx); m_isPaused = true; }
        bool set(const void* inputData) { return false; }
        bool isPaused()const { return m_isPaused; }
    };
    
    BaseStream* m_stream;
    std::future<void> m_thrd;

    InputStream() {};//Singleton

public:

    class FileStream : public BaseStream
    {

        std::ifstream m_fileStream;
        

    public:
        FileStream() :BaseStream()
        {

        }

        bool set(const void* inputData)
        {
            std::string str = *reinterpret_cast<const  std::string*>(inputData);
            m_fileStream = std::ifstream(str, std::ios::binary);
            char buffer[1024] ;
            m_fileStream.read(buffer, 1024);

            if (memcmp(buffer, ".3ds", 4) != 0)
                return false;
            float* buffer_f = reinterpret_cast<float*>(buffer);
            float z2mm = buffer_f[2];
            uint32_t flags = *reinterpret_cast<uint32_t*>(buffer_f + 1);
            std::array<uint16_t, 2> wh = *reinterpret_cast<std::array<uint16_t, 2>*>(buffer_f + 3);
            float* k = buffer_f + 4;
            float fps = buffer_f[13];
            Zstream::i().set(wh, k, z2mm,fps, flags);

            return true;
        }
        static void run(FileStream* sP)
        {
            
            Zstream& zs = Zstream::i();
            int n = zs.getHeight() *zs.getWidth();;
            std::vector<unsigned short> dataBuff(n * 2);
            std::ifstream& s = sP->m_fileStream;
            std::streampos streamStart = s.tellg();
            uint32_t waitTimeUS = uint32_t(1e6 / zs.getFPS());
            while (sP->m_isRunning) // Application still alive?
            {
                std::streamsize num = s.gcount();
                if (num < n * 4)
                {
                    s.clear();
                    s.seekg(streamStart);
                }
                uint8_t frameHeder[256];
                s.read(reinterpret_cast<char*>(frameHeder),256);
                s.read(reinterpret_cast<char*>(&dataBuff[0]), n * 4);
                std::this_thread::sleep_for(std::chrono::microseconds(waitTimeUS));


                privInnerRunStreamCopy(sP,dataBuff.data(), dataBuff.data() + n);
               
            }


        }

        
    };


    class RealSenseStream : public BaseStream
    {

        rs2::pipeline m_pipe;
    public:

        RealSenseStream() :BaseStream()
        {
        }
        
        bool set(const void* v = nullptr)
        {
            //using namespace std::chrono_literals;
            try
            {
                rs2::config cfg;
                cfg.enable_stream(RS2_STREAM_DEPTH);
                cfg.enable_stream(RS2_STREAM_INFRARED);

                // Start streaming with default recommended configuration
                rs2::pipeline_profile selection = m_pipe.start(cfg);
                rs2::video_stream_profile depth_stream = selection.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>();
                rs2_intrinsics i = depth_stream.get_intrinsics();


                std::array<float, 9> k = { i.fx,0,i.ppx,0,i.fy,i.ppy,0,0,1 };
                float fps = depth_stream.fps();
                float z2mm = 1;
                if (i.fx == 0)
                {
                    k = { 320,0,320,0,240,240,0,0,1 };
                    z2mm = 8;
                    fps = 30;
                }
                std::array<uint16_t, 2> wh = { uint16_t(depth_stream.width()), uint16_t(depth_stream.height()) };
                Zstream::i().set(wh, k.data(), z2mm, fps, 1);
            }
            catch (const std::exception& e)
            {
                return false;
            }
            return true;
        }

        static void run(const RealSenseStream* sP)
        {
            Zstream& zs = Zstream::i();
            const RealSenseStream& s = *sP;
            GLpainter::i().setStatus("Starting RS camera...");
            s.m_pipe.wait_for_frames(10000); // Wait for next set of frames from the camera
            GLpainter::i().setStatus("streaming", false);
            int n = zs.getHeight() *zs.getWidth();
            while (s.m_isRunning) // Application still alive?
            {
                rs2::frameset data = s.m_pipe.wait_for_frames(10000); // Wait for next set of frames from the camera

                rs2::frame depth = data.get_depth_frame(); // Find and colorize the depth data

                rs2::frame color = data.first_or_default(RS2_STREAM_INFRARED);          // Find the color data
                privInnerRunStreamCopy(sP,static_cast<const uint16_t*>(depth.get_data()), static_cast<const uint8_t*>(color.get_data()));

            }




        }
    };

    bool isLoaded()const { return m_stream != nullptr; }
    BaseStream* get()const { return m_stream; }


    ~InputStream()
    {
        stop();
    }

  


    static InputStream& i()
    {
        static InputStream    instance; // Guaranteed to be destroyed.

        return instance;
    }

    template<class Stream>
    bool start()
    {
        if (m_stream == nullptr)
            return false;
        Stream* strm = reinterpret_cast<Stream*>(m_stream);
        strm->start();
        m_thrd = std::async(std::launch::async, Stream::run, strm);
        return true;
        

    }
    
    template<class Stream>
    bool load(void* inputdata=nullptr)
    {
        if (isLoaded())
            stop();

        Stream*  strm = new Stream;
        bool ok = strm->set(inputdata);
        if (!ok)
            return false;

        m_stream = dynamic_cast<BaseStream*>(strm);
        return true;
    }
    template<class Stream>
    Stream* getStream() { return m_stream; }

    void stop()
    {
        if (m_stream == nullptr)
            return;
        m_stream->stop();
        m_thrd.wait();
        delete m_stream;
        m_stream = nullptr;
    }



};


