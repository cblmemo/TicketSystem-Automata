//
// Created by Rainy Memory on 2021/1/28.
//

#ifndef RAINYMEMORY_MEMORYPOOL_H
#define RAINYMEMORY_MEMORYPOOL_H

#include <string>
#include <fstream>

using std::string;
using std::fstream;
using std::ios;

namespace RainyMemory {
    template<class T, class extraMessage>
    class MemoryPool {
    private:
        int writePoint;
        const string filename;
        fstream fin, fout;
    
    public:
        explicit MemoryPool(const string &_filename) : filename(_filename) {
            fin.open(filename, ios::in);
            if (fin.fail()) {
                fin.clear();
                fin.close();
                fout.open(filename, ios::out | ios::binary);
                fout.close();
                writePoint = -1;
                extraMessage temp;
                fout.open(filename, ios::in | ios::out | ios::binary);
                fout.seekp(0);
                fout.write(reinterpret_cast<const char *>(&temp), sizeof(extraMessage));
                fout.seekp(sizeof(extraMessage));
                fout.write(reinterpret_cast<const char *>(&writePoint), sizeof(int));
                fout.close();
            }
            else {
                fin.seekg(sizeof(extraMessage));
                fin.read(reinterpret_cast<char *>(&writePoint), sizeof(int));
                fin.close();
            }
        }
        
        ~MemoryPool() = default;
        
        void clear() {
            fout.open(filename, ios::out | ios::binary);
            fout.close();
            writePoint = -1;
            extraMessage temp;
            fout.open(filename, ios::in | ios::out | ios::binary);
            fout.seekp(0);
            fout.write(reinterpret_cast<const char *>(&temp), sizeof(extraMessage));
            fout.seekp(sizeof(extraMessage));
            fout.write(reinterpret_cast<const char *>(&writePoint), sizeof(int));
            fout.close();
        }
        
        int write(const T &o) {
            fin.open(filename, ios::in | ios::binary);
            fout.open(filename, ios::in | ios::out | ios::binary);
            int offset;
            if (writePoint < 0) {
                fout.seekp(0, ios::end);
                offset = fout.tellp();
            }
            else {
                offset = writePoint;
                fin.seekg(writePoint);
                fin.read(reinterpret_cast<char *>(&writePoint), sizeof(int));
                fout.seekp(sizeof(extraMessage));
                fout.write(reinterpret_cast<const char *>(&writePoint), sizeof(int));
                fout.seekp(offset);
            }
            fout.write(reinterpret_cast<const char *>(&o), sizeof(T));
            fin.close();
            fout.close();
            return offset;
        }
        
        T read(int offset) {
            fin.open(filename, ios::in | ios::binary);
            T temp;
            fin.seekg(offset);
            fin.read(reinterpret_cast<char *>(&temp), sizeof(T));
            fin.close();
            return temp;
        }
        
        void update(const T &o, int offset) {
            fout.open(filename, ios::in | ios::out | ios::binary);
            fout.seekp(offset);
            fout.write(reinterpret_cast<const char *>(&o), sizeof(T));
            fout.close();
        }
        
        void erase(int offset) {
            fout.open(filename, ios::in | ios::out | ios::binary);
            fout.seekp(offset);
            fout.write(reinterpret_cast<const char *>(&writePoint), sizeof(int));
            writePoint = offset;
            fout.seekp(sizeof(extraMessage));
            fout.write(reinterpret_cast<const char *>(&writePoint), sizeof(int));
            fout.close();
        }
        
        extraMessage readExtraMessage() {
            fin.open(filename, ios::in | ios::binary);
            extraMessage temp;
            fin.seekg(0);
            fin.read(reinterpret_cast<char *>(&temp), sizeof(extraMessage));
            fin.close();
            return temp;
        }
        
        void updateExtraMessage(const extraMessage &o) {
            fout.open(filename, ios::in | ios::out | ios::binary);
            fout.seekp(0);
            fout.write(reinterpret_cast<const char *>(&o), sizeof(extraMessage));
            fout.close();
        }
        
        int tellWritePoint() {
            if (writePoint >= 0)return writePoint;
            else {
                fout.open(filename, ios::in | ios::out | ios::binary);
                fout.seekp(0, ios::end);
                int tempWritePoint = fout.tellp();
                fout.close();
                return tempWritePoint;
            }
        }
    };
}

#endif //RAINYMEMORY_MEMORYPOOL_H
