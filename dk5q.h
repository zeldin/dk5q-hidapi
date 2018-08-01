#ifndef DK5Q_H_
#define DK5Q_H_ 1

#include <stdint.h>
#include <stdbool.h>

struct dk5q_context;
typedef struct dk5q_context *dk5q_handle;

extern dk5q_handle dk5q_open(int instance);
extern void dk5q_close(dk5q_handle);
extern int dk5q_set_key_channel_transition(dk5q_handle handle, uint8_t key, uint8_t channel, uint8_t from_value, uint8_t to_value, uint8_t steps, uint8_t delay, uint8_t hold1, uint8_t hold2, bool repeat, bool deferred);
extern int dk5q_set_key_channel(dk5q_handle handle, uint8_t key, uint8_t channel, uint8_t value, bool deferred);
extern int dk5q_set_key_rgb(dk5q_handle handle, uint8_t key, uint8_t r, uint8_t g, uint8_t b, bool deferred);
extern int dk5q_set_key_r(dk5q_handle handle, uint8_t key, uint8_t r, bool deferred);
extern int dk5q_set_key_g(dk5q_handle handle, uint8_t key, uint8_t g, bool deferred);
extern int dk5q_set_key_b(dk5q_handle handle, uint8_t key, uint8_t b, bool deferred);
extern int dk5q_set_key_rgb_transition(dk5q_handle handle, uint8_t key, uint8_t from_r, uint8_t from_g, uint8_t from_b, uint8_t to_r, uint8_t to_g, uint8_t to_b, uint8_t steps, uint8_t delay, uint8_t hold, bool repeat, bool deferred);
extern int dk5q_fade_key_rgb(dk5q_handle handle, uint8_t key, uint8_t from_r, uint8_t from_g, uint8_t from_b, uint8_t to_r, uint8_t to_g, uint8_t to_b, bool deferred);
extern int dk5q_blink_key_rgb(dk5q_handle handle, uint8_t key, uint8_t r, uint8_t g, uint8_t b, bool deferred);
extern int dk5q_breathe_key_rgb(dk5q_handle handle, uint8_t key, uint8_t r, uint8_t g, uint8_t b, bool deferred);
extern int dk5q_color_cycle_key(dk5q_handle handle, uint8_t key, bool deferred);
extern int dk5q_apply(dk5q_handle handle);

#endif /* DK5Q_H_ */
