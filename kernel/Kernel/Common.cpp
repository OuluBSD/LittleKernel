#include "Kernel.h"

extern "C" {

void* memcpy(void* dest, const void* src, uint32 len) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (uint32 i = 0; i < len; i++) {
        d[i] = s[i];
    }
    return dest;
}

void* memset(void* dest, char val, uint32 len) {
    char* ptr = (char*)dest;
    for (uint32 i = 0; i < len; i++) {
        ptr[i] = val;
    }
    return dest;
}

uint16* memsetw(uint16* dest, uint16 val, uint32 count) {
    for (uint32 i = 0; i < count; i++) {
        dest[i] = val;
    }
    return dest;
}

int strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

char* strcpy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

char* strcat(char* dest, const char* src) {
    int dest_len = strlen(dest);
    int i = 0;
    while (src[i] != '\0') {
        dest[dest_len + i] = src[i];
        i++;
    }
    dest[dest_len + i] = '\0';
    return dest;
}

int strcmp(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

char* strncpy(char* dest, const char* src, uint32 count) {
    uint32 i;
    for (i = 0; i < count && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    for (; i < count; ++i) {
        dest[i] = '\0';
    }
    return dest;
}

}

void Spinlock::Acquire() {
    while (__sync_val_compare_and_swap(&lock, 0, 1) == 1) {
        // Wait until the lock is available
        while (lock == 1) {
            // Add a pause instruction to be nice to hyperthreading CPUs
            __asm__ volatile("pause");
        }
    }
}

void Spinlock::Release() {
    __sync_lock_release(&lock);
}

bool Spinlock::TryAcquire() {
    return __sync_val_compare_and_swap(&lock, 0, 1) == 0;
}