/* SPDX-License-Identifier: GPL-2.0-or-later */
/* Copyright (C) 2026 Difan Zhang */
/*
 * Reader for the YAML written by tuning/bin2yaml.py: "day:" and "night:"
 * sections, two-space module keys and four-space "name: value" or
 * "name: [v, v, ...]" fields.
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "t23_ipa.h"

struct ipa_field {
	char module[32];
	char name[64];
	int64_t *values;
	unsigned int count;
};

static int ipa_field_cmp(const void *a, const void *b)
{
	const struct ipa_field *fa = a;
	const struct ipa_field *fb = b;
	int ret = strcmp(fa->module, fb->module);

	if (ret)
		return ret;
	return strcmp(fa->name, fb->name);
}

static int add_field(struct ipa_tuning_set *set, const char *module,
		     const char *name, const char *text)
{
	struct ipa_field *f;
	const char *p = text;
	unsigned int capacity = 16;
	char *end;

	if (set->count == set->capacity) {
		unsigned int n = set->capacity ? set->capacity * 2 : 256;
		void *grown = realloc(set->fields, n * sizeof(*set->fields));

		if (!grown)
			return -ENOMEM;
		set->fields = grown;
		set->capacity = n;
	}
	f = &set->fields[set->count];
	memset(f, 0, sizeof(*f));
	snprintf(f->module, sizeof(f->module), "%s", module);
	snprintf(f->name, sizeof(f->name), "%s", name);
	f->values = malloc(capacity * sizeof(*f->values));
	if (!f->values)
		return -ENOMEM;
	if (*p == '[')
		p++;
	for (;;) {
		while (*p == ' ' || *p == ',')
			p++;
		if (!*p || *p == ']' || *p == '\n')
			break;
		if (f->count == capacity) {
			int64_t *grown;

			capacity *= 2;
			grown = realloc(f->values, capacity * sizeof(*grown));
			if (!grown)
				return -ENOMEM;
			f->values = grown;
		}
		f->values[f->count++] = strtoll(p, &end, 0);
		if (end == p)
			return -EINVAL;
		p = end;
	}
	set->count++;
	return 0;
}

int ipa_tuning_load(struct ipa_tuning *t, const char *path)
{
	struct ipa_tuning_set *set = NULL;
	char module[32] = "", *line = NULL;
	size_t size = 0;
	unsigned int lineno = 0;
	int ret = 0;
	FILE *f;

	memset(t, 0, sizeof(*t));
	f = fopen(path, "r");
	if (!f)
		return -errno;
	while (getline(&line, &size, f) > 0) {
		char *colon, *key;
		size_t indent = strspn(line, " ");

		lineno++;
		key = line + indent;
		if (*key == '#' || *key == '\n' || !*key)
			continue;
		colon = strchr(key, ':');
		if (!colon) {
			ret = -EINVAL;
			break;
		}
		*colon = '\0';
		if (indent == 0) {
			set = !strcmp(key, "day") ? &t->set[0] :
			      !strcmp(key, "night") ? &t->set[1] : NULL;
		} else if (indent == 2) {
			snprintf(module, sizeof(module), "%s", key);
		} else if (indent == 4 && set && strcmp(module, "unmapped")) {
			ret = add_field(set, module, key, colon + 1 + strspn(colon + 1, " "));
			if (ret)
				break;
		}
	}
	free(line);
	fclose(f);
	if (!ret && (!t->set[0].count || t->set[1].count != t->set[0].count))
		ret = -EINVAL;
	if (ret) {
		ipa_err("%s:%u: invalid tuning file\n", path, lineno);
		ipa_tuning_free(t);
	} else {
		unsigned int s;

		for (s = 0; s < 2; s++)
			qsort(t->set[s].fields, t->set[s].count,
			      sizeof(struct ipa_field), ipa_field_cmp);
	}
	return ret;
}

void ipa_tuning_free(struct ipa_tuning *t)
{
	unsigned int s, i;

	for (s = 0; s < 2; s++) {
		for (i = 0; i < t->set[s].count; i++)
			free(t->set[s].fields[i].values);
		free(t->set[s].fields);
	}
	memset(t, 0, sizeof(*t));
}

const int64_t *ipa_tuning_get(const struct ipa_tuning *t, const char *module,
			      const char *name, unsigned int count)
{
	const struct ipa_tuning_set *set = &t->set[t->night];
	struct ipa_field key, *found;

	snprintf(key.module, sizeof(key.module), "%s", module);
	snprintf(key.name, sizeof(key.name), "%s", name);

	found = bsearch(&key, set->fields, set->count,
			sizeof(struct ipa_field), ipa_field_cmp);
	if (!found) {
		ipa_err("tuning %s.%s: missing\n", module, name);
		return NULL;
	}
	if (count && found->count != count) {
		ipa_err("tuning %s.%s: %u values, expected %u\n",
			module, name, found->count, count);
		return NULL;
	}
	return found->values;
}

/* 9 points over log2(total gain) in Q16, rounded half up. */
int64_t ipa_intp9(const int64_t *v, uint32_t gain_log2_q16)
{
	uint32_t index = gain_log2_q16 >> 16, frac = gain_log2_q16 & 0xffff;
	int64_t a, b, d, t;

	if (index > 7)
		return v[8];
	a = v[index];
	b = v[index + 1];
	if (a == b)
		return b;
	d = a < b ? b - a : a - b;
	t = ((d * frac) >> 16) + (((d * frac) & 0x8000) >> 15);
	return a < b ? a + t : a - t;
}
