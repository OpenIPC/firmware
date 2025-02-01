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

int main(int argc, char **argv) {
	unsigned char drone_publickey[crypto_box_PUBLICKEYBYTES];
	unsigned char drone_secretkey[crypto_box_SECRETKEYBYTES];
	unsigned char gs_publickey[crypto_box_PUBLICKEYBYTES];
	unsigned char gs_secretkey[crypto_box_SECRETKEYBYTES];
	FILE *fp;

	if (argc != 2) {
		printf("Passphrase is missing\n");
		return 1;
	}

	if (sodium_init() < 0) {
		printf("Libsodium init failed\n");
		return 1;
	}

	const char *seed = argv[1];
	printf("Using passphrase: %s\n", seed);

	if (crypto_box_seed_keypair(drone_publickey, drone_secretkey, seed) != 0 ||
			crypto_box_seed_keypair(gs_publickey, gs_secretkey, seed) != 0) {
		printf("Unable to generate keys\n");
		return 1;
	}

	const char *drone = "/etc/drone.key";
	if ((fp = fopen(drone, "w")) == NULL) {
		printf("Unable to save: %s\n", drone);
		return 1;
	}

	fwrite(drone_secretkey, crypto_box_SECRETKEYBYTES, 1, fp);
	fwrite(gs_publickey, crypto_box_PUBLICKEYBYTES, 1, fp);
	fclose(fp);

	printf("Drone keypair saved: %s\n", drone);

	const char *station = "/tmp/gs.key";
	if ((fp = fopen(station, "w")) == NULL) {
		printf("Unable to save: %s\n", station);
		return 1;
	}

	fwrite(gs_secretkey, crypto_box_SECRETKEYBYTES, 1, fp);
	fwrite(drone_publickey, crypto_box_PUBLICKEYBYTES, 1, fp);
	fclose(fp);

	printf("Station keypair saved: %s\n", station);

	return 0;
}
