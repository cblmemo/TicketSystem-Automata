//
// Created by Rainy Memory on 2021/5/21.
//

#ifndef CODE_MEMORYPOOL_H
#define CODE_MEMORYPOOL_H

#include <iostream>

using std::string;

namespace RainyMemory {
    template<class T, class extraMessage = int>
    class MemoryPool {
        /*
         * class MemoryPool
         * --------------------------------------------------------
         * A class implements memory pool to storage data.
         * This class offer single type data's storage, update and
         * delete in file.
         * ALso, this class support deleted data's space reclamation,
         * and an extra (not same type of stored data) message storage.
         *
         */
    
    private:
        int writePoint;
        const string filename;
        FILE *file;
    
    public:
        explicit MemoryPool(const string &_filename, const extraMessage &ex = extraMessage {}) : filename(_filename) {
            file = fopen(filename.c_str(), "rb");
            if (file == NULL) {
                file = fopen(filename.c_str(), "wb+");
                fclose(file);
                writePoint = -1;
                extraMessage temp(ex);
                file = fopen(filename.c_str(), "rb+");
                fseek(file, 0, SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&temp), sizeof(extraMessage), 1, file);
                fseek(file, sizeof(extraMessage), SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
                fclose(file);
            }
            else {
                fseek(file, sizeof(extraMessage), SEEK_SET);
                fread(reinterpret_cast<char *>(&writePoint), sizeof(int), 1, file);
                fclose(file);
            }
        }
        
        ~MemoryPool() = default;
        
        T read(int offset) {
            file = fopen(filename.c_str(), "rb");
            T temp;
            fseek(file, offset, SEEK_SET);
            fread(reinterpret_cast<char *>(&temp), sizeof(T), 1, file);
            fclose(file);
            return temp;
        }
        
        int write(const T &o) {
            int offset;
            file = fopen(filename.c_str(), "rb+");
            if (writePoint < 0) {
                fseek(file, 0, SEEK_END);
                offset = ftell(file);
            }
            else {
                offset = writePoint;
                fseek(file, writePoint, SEEK_SET);
                fread(reinterpret_cast<char *>(&writePoint), sizeof(int), 1, file);
                fseek(file, sizeof(extraMessage), SEEK_SET);
                fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
                fseek(file, offset, SEEK_SET);
            }
            fwrite(reinterpret_cast<const char *>(&o), sizeof(T), 1, file);
            fclose(file);
            return offset;
        }
        
        void update(const T &o, int offset) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, offset, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&o), sizeof(T), 1, file);
            fclose(file);
        }
        
        void erase(int offset) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, offset, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            writePoint = offset;
            fseek(file, sizeof(extraMessage), SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            fclose(file);
        }
        
        void clear(extraMessage ex = extraMessage {}) {
            file = fopen(filename.c_str(), "wb+");
            fclose(file);
            writePoint = -1;
            extraMessage temp(ex);
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&temp), sizeof(extraMessage), 1, file);
            fseek(file, sizeof(extraMessage), SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&writePoint), sizeof(int), 1, file);
            fclose(file);
        }
        
        extraMessage readExtraMessage() {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            extraMessage temp;
            fread(reinterpret_cast<char *>(&temp), sizeof(extraMessage), 1, file);
            fclose(file);
            return temp;
        }
        
        void updateExtraMessage(const extraMessage &o) {
            file = fopen(filename.c_str(), "rb+");
            fseek(file, 0, SEEK_SET);
            fwrite(reinterpret_cast<const char *>(&o), sizeof(extraMessage), 1, file);
            fclose(file);
        }
        
        int tellWritePoint() {
            if (writePoint >= 0)return writePoint;
            else {
                file = fopen(filename.c_str(), "rb+");
                fseek(file, 0, SEEK_END);
                int tempWritePoint = ftell(file);
                fclose(file);
                return tempWritePoint;
            }
        }
    };
}

#endif //CODE_MEMORYPOOL_H
