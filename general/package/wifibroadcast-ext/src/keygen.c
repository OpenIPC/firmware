// Copyright (C) 2017 - 2024 Vasily Evseenko <svpcom@p2ptech.org>

/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 3.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <fcntl.h>
#include <sodium.h>
#include <string.h>

int main(int argc, char **argv) {
	unsigned char publickey[crypto_box_PUBLICKEYBYTES];
	unsigned char secretkey[crypto_box_SECRETKEYBYTES];
	FILE *fp;

	if (argc != 2) {
		printf("Passphrase is missing\n");
		return 1;
	}

	if (sodium_init() < 0) {
		printf("Libsodium init failed\n");
		return 1;
	}

	char seed[32];
	strncpy(seed, argv[1], sizeof(seed));
	printf("Using passphrase: %s\n", seed);

	if (crypto_box_seed_keypair(publickey, secretkey, seed) != 0) {
		printf("Unable to generate key\n");
		return 1;
	}

	const char *key = "/etc/drone.key";
	if ((fp = fopen(key, "w")) == NULL) {
		printf("Unable to save: %s\n", key);
		return 1;
	}

	fwrite(secretkey, crypto_box_SECRETKEYBYTES, 1, fp);
	fwrite(publickey, crypto_box_PUBLICKEYBYTES, 1, fp);
	fclose(fp);

	printf("Key saved: %s\n", key);

	return 0;
}
