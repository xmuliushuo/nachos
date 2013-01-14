#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

class BoundedBuffer {
   public:
     // create a bounded buffer with a limit of 'maxsize' bytes
     BoundedBuffer(int maxsize);
     
     // read 'size' bytes from the bounded buffer, storing into 'data'.
     // ('size' may be greater than 'maxsize')
     void Read(void *data, int size);
     
     // write 'size' bytes from 'data' into the bounded buffer.
     // ('size' may be greater than 'maxsize')
     void Write(void *data, int size);
   private:
     void *m_buffer;
     Lock *m_lock;
     int m_bufferSize;
     int m_count;
     int m_nextin;
     int m_nextout;
     Condition *m_bufferEmpty;
     Condition *m_bufferFull;
};

#endif
