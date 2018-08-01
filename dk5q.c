#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <hidapi.h>

#include "dk5q.h"

struct dk5q_context {
  hid_device *device;
  uint8_t seq;
};

static const char channelgroups[256] =
  "000000000000001111222222"
  "000000000011111112222222"
  "000000000011111122222222"
  "000000000111111111122222"
  "000000000011111122222222"
  "000000000011111112222222"
  "000000000001111112222222"
  "000000000000000000000002"
  "000000000000000000000002"
  ;

static int dk5q_exchange_reports(dk5q_handle handle, const uint8_t report[64])
{
  uint8_t buf[65];
  uint8_t seq = handle->seq++;
  buf[0] = 0; /* Report ID */
  memcpy(buf+1, report, 64);
  buf[3] = seq;
  int res = hid_send_feature_report(handle->device, buf, 65);
  if (res < 0)
    return res;
  res = hid_get_feature_report(handle->device, buf, 65);
  if (res < 0)
    return res;

  /* Check acknowledge */
  if (buf[2] == 0x14 && buf[3] == seq)
    return 0;

  return -1;
}

static int dk5q_init_device(dk5q_handle handle)
{
  static const uint8_t report[64] = {
    0, 0x13, 0x0, 0x4d, 0x43, 0x49, 0x51, 0x46, 0x49, 0x46, 0x45, 0x44,
    0x4c, 0x48, 0x39, 0x46, 0x34, 0x41, 0x45, 0x43, 0x58, 0x39, 0x31, 0x36,
    0x50, 0x42, 0x44, 0x35, 0x50, 0x33, 0x41, 0x33, 0x30, 0x37, 0x38,
  };
  return dk5q_exchange_reports(handle, report);
}

int dk5q_apply(dk5q_handle handle)
{
  static const uint8_t report[64] = {
    0, 0x2d, 0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  };
  return dk5q_exchange_reports(handle, report);
}

int dk5q_set_key_channel_transition(dk5q_handle handle, uint8_t key, uint8_t channel,
                                    uint8_t from_value, uint8_t to_value, uint8_t steps,
                                    uint8_t delay, uint8_t hold1, uint8_t hold2,
                                    bool repeat, bool deferred)
{
  uint8_t report[64] = {
    0, 0x28, 0, channel, 1, key, 2,
    0, 0, 0, 0, delay, 0, 0, 0, 0, 0,
    0, 0, 0, 0, delay, 0, 0, 0, 0, 0,
    0, 0, 0, 0, (repeat? 0x18:0), (deferred? 0x40 : 0)|(repeat? 0x10:0),
  };
  if (to_value < from_value) {
    report[13] = from_value;
    report[23] = to_value;
    report[25] = hold1;
    report[15] = hold2;
    report[31] |= 2;
  } else {
    report[23] = from_value;
    report[13] = to_value;
    report[15] = hold1;
    report[25] = hold2;
    report[31] |= 1;
  }
  if (steps) {
    report[9] = report[19] = steps = (report[13] - report[23]) / steps;
  }
  if (steps) {
    steps = (report[13] - report[23]) % steps;
  }
  report[7] = report[13] - steps;
  report[17] = report[23] + steps;

  return dk5q_exchange_reports(handle, report);
}

int dk5q_set_key_channel(dk5q_handle handle, uint8_t key, uint8_t channel, uint8_t value, bool deferred)
{
  uint8_t report[64] = {
    0, 0x28, 0, channel, 1, key, 2,
    0, 0, 0, 0, 0, 0, value, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, (deferred? 0x40 : 0),
  };

  return dk5q_exchange_reports(handle, report);
}

int dk5q_set_key_rgb(dk5q_handle handle, uint8_t key, uint8_t r, uint8_t g, uint8_t b, bool deferred)
{
  int base = channelgroups[key]&3;
  int res = dk5q_set_key_channel(handle, key, base, r, deferred);
  if (res >= 0) {
    res = dk5q_set_key_channel(handle, key, (base+1)%3, g, deferred);
    if (res >= 0) {
      res = dk5q_set_key_channel(handle, key, (base+2)%3, b, deferred);
    }
  }
  return res;
}

int dk5q_set_key_r(dk5q_handle handle, uint8_t key, uint8_t r, bool deferred)
{
  int base = channelgroups[key]&3;
  return dk5q_set_key_channel(handle, key, base, r, deferred);
}

int dk5q_set_key_g(dk5q_handle handle, uint8_t key, uint8_t g, bool deferred)
{
  int base = channelgroups[key]&3;
  return dk5q_set_key_channel(handle, key, (base+1)%3, g, deferred);
}

int dk5q_set_key_b(dk5q_handle handle, uint8_t key, uint8_t b, bool deferred)
{
  int base = channelgroups[key]&3;
  return dk5q_set_key_channel(handle, key, (base+2)%3, b, deferred);
}

int dk5q_set_key_rgb_transition(dk5q_handle handle, uint8_t key,
                                uint8_t from_r, uint8_t from_g, uint8_t from_b,
                                uint8_t to_r, uint8_t to_g, uint8_t to_b,
                                uint8_t steps, uint8_t delay, uint8_t hold,
                                bool repeat, bool deferred)
{
  int base = channelgroups[key]&3;
  int res = dk5q_set_key_channel_transition(handle, key, base, from_r, to_r,
                                            steps, delay, hold, hold, repeat, deferred);
  if (res >= 0) {
    res = dk5q_set_key_channel_transition(handle, key, (base+1)%3, from_g, to_g,
                                            steps, delay, hold, hold, repeat, deferred);
    if (res >= 0) {
      res = dk5q_set_key_channel_transition(handle, key, (base+2)%3, from_b, to_b,
                                            steps, delay, hold, hold, repeat, deferred);
    }
  }
  return res;
}

int dk5q_fade_key_rgb(dk5q_handle handle, uint8_t key,
                      uint8_t from_r, uint8_t from_g, uint8_t from_b,
                      uint8_t to_r, uint8_t to_g, uint8_t to_b,
                      bool deferred)
{
  return dk5q_set_key_rgb_transition(handle, key, from_r, from_g, from_b,
                                     to_r, to_g, to_b, 12, 5, 0, false, deferred);
}

int dk5q_blink_key_rgb(dk5q_handle handle, uint8_t key,
                       uint8_t r, uint8_t g, uint8_t b, bool deferred)
{
  return dk5q_set_key_rgb_transition(handle, key, 0, 0, 0, r, g, b,
                                     3, 1, 39, true, deferred);
}

int dk5q_breathe_key_rgb(dk5q_handle handle, uint8_t key,
                         uint8_t r, uint8_t g, uint8_t b, bool deferred)
{
  return dk5q_set_key_rgb_transition(handle, key, 0, 0, 0, r, g, b,
                                     5, 3, 39, true, deferred);
}

int dk5q_color_cycle_key(dk5q_handle handle, uint8_t key, bool deferred)
{
  int base = channelgroups[key]&3;
  int res = dk5q_set_key_channel_transition(handle, key, base, 0x00, 0xff,
                                            7, 3, 50, 36, true, deferred);
  if (res >= 0) {
    res = dk5q_set_key_channel_transition(handle, key, (base+1)%3, 0xff, 0x00,
                                          7, 3, 36, 36, true, deferred);
    if (res >= 0) {
      res = dk5q_set_key_channel_transition(handle, key, (base+2)%3, 0x00, 0xff,
                                            7, 3, 36, 72, true, deferred);
    }
  }
  return res;
}

dk5q_handle dk5q_open(int instance)
{
  struct hid_device_info *devs, *cur_dev;
  hid_device *device = NULL;
  dk5q_handle handle = NULL;

  for (cur_dev = devs = hid_enumerate(0x24f0, 0x2020);
       cur_dev;
       cur_dev = cur_dev->next) {
    if (cur_dev->interface_number == 2) {
      if (instance > 0)
	--instance;
      else {
	device = hid_open_path(cur_dev->path);
	break;
      }
    }
  }
  hid_free_enumeration(devs);
  if (device) {
    handle = calloc(1, sizeof(struct dk5q_context));
    if (handle) {
      handle->device = device;
      handle->seq = 1;
      dk5q_init_device(handle);
    } else
      hid_close(device);
  }
  return handle;
}

void dk5q_close(dk5q_handle handle)
{
  if(handle) {
    hid_close(handle->device);
    free(handle);
  }
}
