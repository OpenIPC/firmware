#ifndef __HI_MEM_H__
#define __HI_MEM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

void *memcpy1(void *dest, const void *src, size_t n);
void *memcpy_back(void *dest, const void *src, size_t n);
void *memset1(void *s, int c, size_t n);
int memcmp1(const void *s1, const void *s2, size_t n);
char *strcpy1(char *dest, const char *src);
int strcmp1(const char *s1, const char *s2);
size_t strlen1(const char *s);

#define memcpy memcpy_back
#define memset memset1
#define memcmp memcmp1
#define strcpy strcpy1
#define strcmp strcmp1
#define strlen strlen1

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HI_MEM_H__ */

