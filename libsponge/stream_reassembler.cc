#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity),_remaining_capacity(capacity) {}
bool StreamReassembler::merge_block(blockNode &b1,const blockNode &b2) {
    assert(b1.indexBegin < b2.indexBegin);

    if(b1.indexBegin + b1.streamLength < b2.indexBegin) {return 0;}
    else if (b1.indexBegin + b1.streamLength == b2.indexBegin)
    {
        // 正好连成一段
        b1.streamLength += b2.streamLength;
        b1.data +=b2.data;
        assert(b1.data.size() == b1.streamLength);
        assembler.erase(b2);
        assert(assembler.find(b2)==assembler.end());
     
    }else{
        // overlapped 
        uint64_t nextByteIndex = b1.indexBegin +  b1.streamLength;
        b1.data +=b2.data.substr(nextByteIndex-b2.indexBegin);
        b1.streamLength += (b2.streamLength - (nextByteIndex-b2.indexBegin) );
        assert(b1.data.size() == b1.streamLength);
        // remove overlapped (nextByteIndex - b2.indexBegin) bytes
        _unassembled_bytes -= (nextByteIndex - b2.indexBegin);
        _remaining_capacity += (nextByteIndex - b2.indexBegin);
        assert(_unassembled_bytes + _remaining_capacity == _capacity);

        assembler.erase(b2);
        assert(assembler.find(b2)==assembler.end());

    }
    return 1;    
}
//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // overload
    if(data.size() + index > _capacity ) return;
    //不确定 这里要不要return 
    //if(data.size() > _remaining_capacity) return ;
    if(eof){
        _output.end_input();
        return;     
    }
    blockNode newblock;
    //如果子串包含已经写入bytestream的部分 就把冗余删除
    assert(index>=0 && _nexindex>=0);
    if(data.size()+index <=_nexindex) {return ;}
    else if(index < _nexindex && data.size()+index > _nexindex ){
        newblock.indexBegin = _nexindex;
        // 删除冗余部分后 子串开始index
        uint64_t start = _nexindex - index;
        newblock.data = data.substr(start);
        newblock.streamLength = newblock.data.size();
    }else{
        assert(index >= _nexindex && index+data.size() <= _capacity );
        newblock.indexBegin = index;
        newblock.data = data;
        newblock.streamLength=data.size();
    }
    // insert newblock
    assembler.insert(newblock);
    _unassembled_bytes +=newblock.streamLength;
    _remaining_capacity -= newblock.streamLength;
    // 寻找相邻block merge 然后更新remaining capacity 和 unassembeld_bytes

    // 如果newblock的indexBegin 是bytestream 的_nexindex 则寻找后一个可以合并的block 然后写入
    if(newblock.indexBegin==_nexindex){
        // 一直寻找后面可以合并的block 直到不能合并为止
        bool gomerge = 1;
        do{
        set<blockNode>::iterator rightblock = assembler.lower_bound(newblock);
        if(rightblock !=assembler.end()) gomerge = merge_block(newblock,(*rightblock));
        }while(gomerge);
        _output.write(newblock.data);
        _nexindex = newblock.indexBegin + newblock.streamLength;
        _unassembled_bytes -= newblock.streamLength;
        _remaining_capacity += newblock.streamLength;
        assert(_output.bytes_written() == _nexindex);
    }else{
    // 如果newblock的indexBegin 不是bytestream 的_nexindex 则写入assembler
      assembler.insert(newblock);
      _remaining_capacity -= newblock.streamLength;
      _unassembled_bytes += newblock.streamLength;
    }
    


}

size_t StreamReassembler::unassembled_bytes() const { return {}; }

bool StreamReassembler::empty() const { return {}; }
