#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream( const size_t capacity ) : capacity_( capacity ){
    DUMMY_CODE();
}

size_t ByteStream::write(const string &data) {
   size_t len = data.size();
    if (len > remaining_capacity())
    {
        len = remaining_capacity();
    }
    bytesPushed_+= len;
    for (size_t i = 0; i < len; ++i)
    {
      streamQ.push_back(data[i]);
    }
    return len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
 // Your code here.
    if(streamQ.empty()){
        return std::string();
    }
    string peekstr;
    for(size_t i = 0;i< len&& i < streamQ.size();++i){
        peekstr += streamQ[i];
    }
   
    return peekstr;

}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
      // Your code here.
  uint64_t bytesCnt = 0;
 
  while(!streamQ.empty() && bytesCnt < len){
    streamQ.pop_front();
    
    bytesCnt++;
    bytesPoped_++;
  }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    const auto ret = peek_output(len);
    pop_output(len);
    return ret;
}

void ByteStream::end_input() {
    pushClosed=true;
    
}

bool ByteStream::input_ended() const { 
    return pushClosed;
}

size_t ByteStream::buffer_size() const { 
    return streamQ.size();
}

bool ByteStream::buffer_empty() const { 
    return streamQ.empty();
}

bool ByteStream::eof() const { 
    return (buffer_empty() && input_ended());
}

size_t ByteStream::bytes_written() const { 
    return bytesPushed_;
}

size_t ByteStream::bytes_read() const {
    return bytesPoped_;
}

size_t ByteStream::remaining_capacity() const { 
    return capacity_ - streamQ.size();
}
