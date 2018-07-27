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

int dk5q_set_key_channel(dk5q_handle handle, uint8_t key, uint8_t channel, uint8_t value)
{
  uint8_t report[64] = {
    0, 0x28, 0, channel, 1, key, 2,
    0, 0, 0, 0, 0, 0, value, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 0x40,
  };

  return dk5q_exchange_reports(handle, report);
}

int dk5q_set_key_rgb(dk5q_handle handle, uint8_t key, uint8_t r, uint8_t g, uint8_t b)
{
  int base = channelgroups[key]&3;
  int res = dk5q_set_key_channel(handle, key, base, r);
  if (res >= 0) {
    res = dk5q_set_key_channel(handle, key, (base+1)%3, g);
    if (res >= 0) {
      res = dk5q_set_key_channel(handle, key, (base+2)%3, b);
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
