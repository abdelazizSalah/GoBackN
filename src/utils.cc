#include "utils.h"
#include <random>

int Mod (int n, int m) {
    while(n < 0){
        n += m ;
    }   
    return n % m ; 
}

int getNodeId(const char* name){
    return name[4] - '0';
}

bool _between(SeqNum a, SeqNum b, SeqNum c)
{
    return ((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a));
}

/* Return true if a <= b <= c circularly; false otherwise. */
bool between(SeqNum a,SeqNum b, SeqNum c){
    return _between(a,b,c+1); 
}

ByteStream toByteStream(std::string str){
    ByteStream bytes;
    for(char c: str)
        bytes.push_back(Byte(c));
    return bytes;
}

std::string toString(ByteStream bytes){
    std::string str;
    for(Byte byte: bytes)
        str += (char)byte.to_ulong();
    return str;
}

int uniform(int a, int b){
    std::random_device rd;
    std::mt19937 genertor(rd());
    std::uniform_int_distribution<int> uniform_dis(a, b);

    return uniform_dis(genertor);
}

float uniform_real(float a, float b){
    std::random_device rd;
    std::mt19937 genertor(rd());
    std::uniform_real_distribution<float> uniform_dis(a, b);

    return uniform_dis(genertor);
}

std::string addRandomError(std::string payload, int& modified_bit){
    ByteStream bytes = toByteStream(payload);

    // choose a uniformally random bit
    int idx = uniform(0, bytes.size()-1);
    int bitPos = uniform(0, 7);
    bytes[idx] ^= 1<<bitPos; // flip the chosen bit

    modified_bit = idx*8 + bitPos;

    return toString(bytes);
}