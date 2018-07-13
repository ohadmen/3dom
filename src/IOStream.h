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
    OutputStream() {}//singelton
    std::ofstream m_fileStream;
    std::chrono::time_point<std::chrono::system_clock> m_timer;
public:
    static OutputStream& i()
    {
        static OutputStream    instance; // Guaranteed to be destroyed.

        return instance;
    }

    bool open(const std::string& fn)
    {
        m_fileStream.open(fn, std::ios::binary);

    }
    bool isOpen() const { return m_fileStream.is_open(); }
    bool set(const std::array<uint16_t, 2>& wh, float* k, float z2mm, uint32_t flags)
    {
        if (!m_fileStream.is_open())
            return false;
        char buffer[1024];
        float* buffer_f = reinterpret_cast<float*>(buffer);
        memcpy(buffer, ".3ds", 4);
        m_fileStream.write(buffer, 1024);
        buffer_f[1] = *reinterpret_cast<const float*>(&flags);
        buffer_f[2] = z2mm;

        buffer_f[3] = *reinterpret_cast<const float*>(&wh);
        memcpy(buffer + 4, k, 9);
        m_timer = std::chrono::system_clock::from_time_t(0);
        return true;

    }
    template<class T, class K>
    bool  write(const T* zdata, const K* colData, int n)
    {
        if (!m_fileStream.is_open())
            return false;
        uint16_t timeElapsed;//msec
        if (m_timer == std::chrono::system_clock::from_time_t(0))
            timeElapsed = 0;
        else
        {
            timeElapsed = uint16_t((std::chrono::system_clock::now() - m_timer).count() * 1000);
            m_timer = std::chrono::system_clock::now();
        }

        m_fileStream << timeElapsed;
        m_fileStream.write(reinterpret_cast<const char*>(zdata), n*sizeof(T));
        m_fileStream.write(reinterpret_cast<const char*>(colData), n * sizeof(T));
        return true;
    }
    void close()
    {
        m_fileStream.close();
    }
};



class InputStream
{
    class BaseStream
    {
        std::mutex m_mtx;
    protected:
        bool m_isRunning;
        bool m_isPaused;

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
            char buffer[1024];
            m_fileStream.read(buffer, 1024);

            if (memcmp(buffer, ".3ds", 4) != 0)
                return false;
            float* buffer_f = reinterpret_cast<float*>(buffer);
            float z2mm = buffer_f[2];
            uint32_t flags = *reinterpret_cast<uint32_t*>(buffer_f + 1);
            std::array<uint16_t, 2> wh = *reinterpret_cast<std::array<uint16_t, 2>*>(buffer_f + 3);
            float* k = buffer_f + 4;
            
            Zstream::i().set(wh, k, z2mm, flags);
            if(OutputStream::i().isOpen())
                OutputStream::i().set(wh, k, z2mm, flags);
            return true;
        }
        static void run(FileStream* sP)
        {
            
            Zstream& zs = Zstream::i();
            int n = zs.getHeight() *zs.getWidth();;
            std::vector<unsigned short> dataBuff(n * 2);
            std::ifstream& s = sP->m_fileStream;
            std::streampos streamStart = s.tellg();
            while (sP->m_isRunning) // Application still alive?
            {
                std::streamsize num = s.gcount();
                if (num < n * 4)
                {
                    s.clear();
                    s.seekg(streamStart);
                }
                uint16_t waitTime;
                s.read(reinterpret_cast<char*>(&waitTime), 2);
                s.read(reinterpret_cast<char*>(&dataBuff[0]), n * 4);
                zs.setBuffer(dataBuff.data(), dataBuff.data() + n);
                if (OutputStream::i().isOpen())
                    OutputStream::i().write(dataBuff.data(), dataBuff.data() + n, n);

                std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
                while(sP->m_isPaused)
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));
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

            rs2::config cfg;
            cfg.enable_stream(RS2_STREAM_DEPTH);
            cfg.enable_stream(RS2_STREAM_INFRARED);

            // Start streaming with default recommended configuration
            rs2::pipeline_profile selection = m_pipe.start(cfg);
            rs2::video_stream_profile depth_stream = selection.get_stream(RS2_STREAM_DEPTH).as<rs2::video_stream_profile>();
            rs2_intrinsics i = depth_stream.get_intrinsics();


            std::array<float, 9> k = { i.fx,0,i.ppx,0,i.fy,i.ppy,0,0,1 };
            float z2mm = 1;
            if (i.fx == 0)
            {
                k = { 320,0,320,0,240,240,0,0,1 };
                z2mm = 8;
            }
            std::array<uint16_t, 2> wh = { uint16_t(depth_stream.width()), uint16_t(depth_stream.height()) };
            Zstream::i().set(wh, k.data(), z2mm, 1);
            return true;
        }

        static void run(const RealSenseStream* sP)
        {

            const RealSenseStream& s = *sP;
            GLpainter::i().setStatus("Starting RS camera...");
            s.m_pipe.wait_for_frames(10000); // Wait for next set of frames from the camera
            GLpainter::i().setStatus("streaming", false);
            while (s.m_isRunning) // Application still alive?
            {
                rs2::frameset data = s.m_pipe.wait_for_frames(10000); // Wait for next set of frames from the camera

                rs2::frame depth = data.get_depth_frame(); // Find and colorize the depth data

                rs2::frame color = data.first_or_default(RS2_STREAM_INFRARED);          // Find the color data
                Zstream::i().setBuffer(static_cast<const uint16_t*>(depth.get_data()), static_cast<const uint8_t*>(color.get_data()));
                while (sP->m_isPaused)
                    std::this_thread::sleep_for(std::chrono::milliseconds(20));

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


