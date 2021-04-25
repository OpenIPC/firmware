#ifndef UTILS_H
#define UTILS_H

const char *get_json_strval(const cJSON *json, const char *key,
                            const char *def_val);
int get_json_intval(const cJSON *json, const char *key,
                            int def_val);

#endif /* UTILS_H */
