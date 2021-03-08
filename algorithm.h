//
// Created by Rainy Memory on 2021/3/6.
//

#ifndef BPLUSTREE_ALGORITHM_H
#define BPLUSTREE_ALGORITHM_H

namespace RainyMemory {
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
            if (!(val < *first) && !(*first < val))return first;
        }
        return last;
    }
    
    template<class T>
    const T &min(const T &o1, const T &o2) {
        return o1 < o2 ? o1 : o2;
    }
    
    template<class T>
    const T &max(const T &o1, const T &o2) {
        return !(o1 < o2) ? o1 : o2;
    }
    
}

#endif //BPLUSTREE_ALGORITHM_H
