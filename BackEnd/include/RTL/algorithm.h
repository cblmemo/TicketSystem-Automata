//
// Created by Rainy Memory on 2021/3/6.
//

#ifndef TICKETSYSTEM_AUTOMATA_ALGORITHM_H
#define TICKETSYSTEM_AUTOMATA_ALGORITHM_H

/*
 * algorithm.h
 * --------------------------------------------------------
 * A header implements [sort] using heuristic algorithm,
 * [lower_bound], [upper_bound] and [find] for BPlusTree.
 *
 */

#include "vector.h"

namespace RainyMemory {
    namespace inner_vector {
        enum threshold {
            THRESHOLD = 16
        };
        
        inline int _lg(int n) {
            int k;
            for (k = 0; n > 1; n >>= 1)k++;
            return k;
        }
        
        template<class T>
        inline void _swap(T &o1, T &o2) {
            T temp(o1);
            o1 = o2, o2 = temp;
        }
        
        template<class T>
        void _make_heap(RainyMemory::vector<T> &vec, int low, int high, bool (*compare)(const T &, const T &)) {
            for (int i = low; i <= high; i++) {
                int index = i - low + 1;
                while ((index >> 1) >= 1 && compare(vec[(index >> 1) + low - 1], vec[index + low - 1])) _swap(vec[index + low - 1], vec[(index >> 1) + low - 1]), index >>= 1;
            }
        }
        
        template<class T>
        void _pop_heap(RainyMemory::vector<T> &vec, int low, int high, bool (*compare)(const T &, const T &)) {
            _swap(vec[low], vec[high]), high--;
            int now = 1, child;
            while ((now << 1) + low - 1 <= high) {
                child = (now << 1 | 1) + low - 1 <= high && compare(vec[(now << 1) + low - 1], vec[(now << 1 | 1) + low - 1]) ? now << 1 | 1 : now << 1;
                if (compare(vec[now + low - 1], vec[child + low - 1])) _swap(vec[now + low - 1], vec[child + low - 1]), now = child;
                else break;
            }
        }
        
        template<class T>
        void _sort_heap(RainyMemory::vector<T> &vec, int low, int high, bool (*compare)(const T &, const T &)) {
            for (int i = high; i > low; i--) {
                _pop_heap(vec, low, i, compare);
            }
        }
        
        template<class T>
        int _quick_sort_partition(RainyMemory::vector<T> &vec, int low, int high, bool (*compare)(const T &, const T &)) {
            T temp = vec[low];
            while (low < high) {
                while (low < high && !(compare(vec[high], temp))) high--;
                if (low < high) vec[low] = vec[high], low++;
                while (low < high && (compare(vec[low], temp) || (!compare(vec[low], temp) && !compare(temp, vec[low])))) low++;
                if (low < high) vec[high] = vec[low], high--;
            }
            vec[low] = temp;
            return low;
        }
        
        template<class T>
        void insertion_sort(RainyMemory::vector<T> &vec, int low, int high, bool (*compare)(const T &, const T &)) {
            T temp;
            int j;
            for (int i = low + 1; i <= high; i++) {
                temp = vec[i];
                j = i - 1;
                while (j >= low && compare(temp, vec[j])) vec[j + 1] = vec[j], j--;
                vec[j + 1] = temp;
            }
        }
        
        template<class T>
        void quick_sort(RainyMemory::vector<T> &vec, int low, int high, bool (*compare)(const T &, const T &)) {
            if (low >= high) return;
            int mid = _quick_sort_partition(vec, low, high, compare);
            quick_sort(vec, low, mid - 1, compare);
            quick_sort(vec, mid + 1, high, compare);
        }
        
        template<class T>
        void heap_sort(RainyMemory::vector<T> &vec, int low, int high, bool (*compare)(const T &, const T &)) {
            _make_heap(vec, low, high, compare);
            _sort_heap(vec, low, high, compare);
        }
        
        template<class T>
        void intro_sort(RainyMemory::vector<T> &vec, int low, int high, bool (*compare)(const T &, const T &), int depth_limitation) {
            if (depth_limitation == 0) heap_sort(vec, low, high, compare);
            else if (high - low < THRESHOLD) insertion_sort(vec, low, high, compare);
            else {
                int mid = _quick_sort_partition(vec, low, high, compare);
                intro_sort(vec, low, mid - 1, compare, depth_limitation - 1);
                intro_sort(vec, mid + 1, high, compare, depth_limitation - 1);
            }
        }
        
        template<class iter>
        void iter_swap(iter a, iter b) {
            using std::swap;
            swap(*a, *b);
        }
    }
    
    template<class T>
    void sortVector(RainyMemory::vector<T> &vec, bool (*compare)(const T &, const T &) = [](const T &o1, const T &o2) -> bool { return o1 < o2; }) {
        inner_vector::intro_sort(vec, 0, vec.size() - 1, compare, inner_vector::_lg(vec.size() - 1) * 2);
    }
    
    template<class ptr, class T>
    ptr lower_bound(ptr first, ptr last, const T &val) {
        ptr now;
        int count = last - first, step;
        while (count > 0) {
            step = count >> 1;
            now = first + step;
            if (*now < val) {
                first = ++now;
                count -= step + 1;
            }
            else count = step;
        }
        return first;
    }
    
    template<class ptr, class T>
    ptr upper_bound(ptr first, ptr last, const T &val) {
        ptr now;
        int count = last - first, step;
        while (count > 0) {
            step = count >> 1;
            now = first + step;
            if (!(val < *now)) {
                first = ++now;
                count -= step + 1;
            }
            else count = step;
        }
        return first;
    }
    
    template<class ptr, class T>
    ptr find(ptr first, ptr last, const T &val) {
        int len = last - first;
        for (int i = 0; i < len; i++, first++) {
            if (!(val < *first) && !(*first < val)) return first;
        }
        return last;
    }
}

#endif //TICKETSYSTEM_AUTOMATA_ALGORITHM_H
