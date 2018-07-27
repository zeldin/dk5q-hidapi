#ifndef DK5Q_H_
#define DK5Q_H_ 1

#include <stdint.h>
#include <stdbool.h>

struct dk5q_context;
typedef struct dk5q_context *dk5q_handle;

extern dk5q_handle dk5q_open(int instance);
extern void dk5q_close(dk5q_handle);
extern int dk5q_set_key_channel(dk5q_handle handle, uint8_t key, uint8_t channel, uint8_t value, bool delayed);
extern int dk5q_set_key_rgb(dk5q_handle handle, uint8_t key, uint8_t r, uint8_t g, uint8_t b, bool delayed);
extern int dk5q_apply(dk5q_handle handle);

#endif /* DK5Q_H_ */
