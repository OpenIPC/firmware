#include "cjson/cJSON.h"
#include "utils.h"

const char *get_json_strval(const cJSON *json, const char *key,
                            const char *def_val) {
  const cJSON *jval = cJSON_GetObjectItemCaseSensitive(json, key);
  if (cJSON_IsString(jval) && (jval->valuestring != NULL)) {
    return jval->valuestring;
  } else {
    return def_val;
  }
}

int get_json_intval(const cJSON *json, const char *key,
                            int def_val) {
  const cJSON *jval = cJSON_GetObjectItemCaseSensitive(json, key);
  if (cJSON_IsNumber(jval) && (jval->valueint)) {
    return jval->valueint;
  } else {
    return def_val;
  }
}
