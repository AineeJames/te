#include "keystring.h"
#include "raylib.h"
#include <ctype.h>
#include <string.h>

typedef struct {
  int key;
  const char *str;
} KeyMap;

#define KEYDEF(key, str) {key, str},
static const KeyMap key_map[] = {
#include "keycodes.def"
};
#undef KEYDEF

static int str_eq_nocase(const char *a, const char *b) {
  while (*a && *b) {
    if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
      return 0;
    a++;
    b++;
  }
  return *a == *b;
}

int string_to_keycode(const char *str) {
  if (!str || !*str)
    return KEY_NULL;

  for (size_t i = 0; i < sizeof(key_map) / sizeof(key_map[0]); i++) {
    if (str_eq_nocase(key_map[i].str, str))
      return key_map[i].key;
  }

  return KEY_NULL;
}

const char *keycode_to_string(int key) {
  for (size_t i = 0; i < sizeof(key_map) / sizeof(key_map[0]); i++) {
    if (key_map[i].key == key)
      return key_map[i].str;
  }

  return NULL;
}
