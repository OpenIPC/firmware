/* ptz — KC110 pan/tilt/IR controller via PL061 /dev/mem (no sysfs overhead).
 *
 * Pin map (OpenIPC numbering on Hi3518EV200 PL061 chips 0-8).
 * Derived from vendor kernel disasm of gpio_set_value @ 0xc01b854c:
 *   bank = vendor_gpio / 10, pin = vendor_gpio % 10
 *   PL061_phys = 0x20140000 + (bank << 16) + (1 << (pin + 2))
 *
 *   pan   A=8/1  B=7/4  C=7/3  D=7/2     (vendor 81 / 74 / 73 / 72)
 *   tilt  A=0/0  B=0/1  C=0/2  D=4/4     (vendor  0 /  1 /  2 / 44)
 *   IR-LED  bank 7 pin 7                  (vendor 77, active-high)
 *   IR-cut  bank 8 pin 0                  (vendor 80, edge-triggers solenoid)
 *
 * State persisted in /etc/ptz_state across invocations so phase continuity
 * across CLI calls keeps the motor smooth.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#define PL061_BASE  0x20140000u
#define PL061_STRIDE 0x10000u
#define GPIODIR_OFF 0x400u
#define STATE_PATH  "/etc/ptz_state"

#define PAN_STEPS_MAX  1934
#define PAN_DEG        170
#define TILT_STEPS_MAX 387
#define TILT_DEG       68

struct pin { uint8_t bank, pin; };

static const struct pin PAN[4]  = { {8,1}, {7,4}, {7,3}, {7,2} };  /* A B C D */
static const struct pin TILT[4] = { {0,0}, {0,1}, {0,2}, {4,4} };
static const struct pin IRLED   = {7,7};
static const struct pin IRCUT   = {8,0};

/* Full-step (2-phase-on) sequence: AB, BC, CD, DA. */
static const uint8_t SEQ[4][4] = {
    {1,1,0,0}, {0,1,1,0}, {0,0,1,1}, {1,0,0,1},
};

static void *bank_map[9];

static void *map_bank(uint8_t bank) {
    if (bank_map[bank]) return bank_map[bank];
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) { perror("/dev/mem"); exit(1); }
    off_t phys = PL061_BASE + (off_t)bank * PL061_STRIDE;
    void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, phys);
    close(fd);
    if (p == MAP_FAILED) { fprintf(stderr,"mmap bank %u: %s\n",bank,strerror(errno)); exit(1); }
    bank_map[bank] = p;
    return p;
}

static void pin_set_output(struct pin p) {
    volatile uint32_t *dir = (uint32_t *)((uint8_t *)map_bank(p.bank) + GPIODIR_OFF);
    *dir |= (1u << p.pin);
}

static void pin_write(struct pin p, int v) {
    /* PL061 GPIODATA: address = base + (mask << 2). Only bits in mask change. */
    uint8_t *base = (uint8_t *)map_bank(p.bank);
    uint32_t mask = 1u << p.pin;
    volatile uint32_t *reg = (uint32_t *)(base + (mask << 2));
    *reg = v ? mask : 0;
}

static void msleep(long ms) {
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000000L };
    nanosleep(&ts, NULL);
}

/* State -------------------------------------------------------------------- */

struct state {
    int pan_pos;     /* 0 .. PAN_STEPS_MAX */
    int tilt_pos;    /* 0 .. TILT_STEPS_MAX */
    int pan_phase;   /* 0..3 — last SEQ index applied */
    int tilt_phase;
    int ircut;       /* 0 or 1 */
    int irled;       /* 0 or 1 */
};

static void state_load(struct state *s) {
    memset(s, 0, sizeof(*s));
    FILE *f = fopen(STATE_PATH, "r");
    if (!f) return;
    fscanf(f, "pan_pos=%d tilt_pos=%d pan_phase=%d tilt_phase=%d ircut=%d irled=%d",
           &s->pan_pos, &s->tilt_pos, &s->pan_phase, &s->tilt_phase,
           &s->ircut, &s->irled);
    fclose(f);
}

static void state_save(const struct state *s) {
    FILE *f = fopen(STATE_PATH, "w");
    if (!f) { perror(STATE_PATH); return; }
    fprintf(f, "pan_pos=%d tilt_pos=%d pan_phase=%d tilt_phase=%d ircut=%d irled=%d\n",
            s->pan_pos, s->tilt_pos, s->pan_phase, s->tilt_phase, s->ircut, s->irled);
    fclose(f);
}

/* Motor drive -------------------------------------------------------------- */

static void apply_phase(const struct pin coils[4], int phase) {
    const uint8_t *row = SEQ[phase & 3];
    for (int i = 0; i < 4; i++) pin_write(coils[i], row[i]);
}

static void park(const struct pin coils[4]) {
    for (int i = 0; i < 4; i++) pin_write(coils[i], 0);
}

static void prep_motor(const struct pin coils[4]) {
    for (int i = 0; i < 4; i++) pin_set_output(coils[i]);
}

/* Drive `delta` steps. Positive = forward sequence direction. Returns new phase. */
static int drive(const struct pin coils[4], int start_phase, int delta, long phase_ms) {
    int phase = start_phase;
    int dir = (delta > 0) ? 1 : -1;
    int n = delta * dir;
    while (n--) {
        phase = (phase + dir + 4) & 3;
        apply_phase(coils, phase);
        msleep(phase_ms);
    }
    return phase;
}

/* Commands ----------------------------------------------------------------- */

static int clamp(int v, int lo, int hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static int cmd_pan(struct state *s, int delta, long phase_ms) {
    int target = clamp(s->pan_pos + delta, 0, PAN_STEPS_MAX);
    int real = target - s->pan_pos;
    prep_motor(PAN);
    s->pan_phase = drive(PAN, s->pan_phase, real, phase_ms);
    park(PAN);
    s->pan_pos = target;
    return real;
}

static int cmd_tilt(struct state *s, int delta, long phase_ms) {
    int target = clamp(s->tilt_pos + delta, 0, TILT_STEPS_MAX);
    int real = target - s->tilt_pos;
    prep_motor(TILT);
    s->tilt_phase = drive(TILT, s->tilt_phase, real, phase_ms);
    park(TILT);
    s->tilt_pos = target;
    return real;
}

/* Home: drive backwards beyond max range, hit mechanical stop, mark as 0. */
static void cmd_home(struct state *s, long phase_ms) {
    prep_motor(PAN);
    drive(PAN, s->pan_phase, -(PAN_STEPS_MAX + 200), phase_ms);
    park(PAN);
    s->pan_phase = 0;
    s->pan_pos = 0;

    prep_motor(TILT);
    drive(TILT, s->tilt_phase, -(TILT_STEPS_MAX + 50), phase_ms);
    park(TILT);
    s->tilt_phase = 0;
    s->tilt_pos = 0;
}

static void cmd_irled(struct state *s, int on) {
    pin_set_output(IRLED);
    pin_write(IRLED, on ? 1 : 0);
    s->irled = on ? 1 : 0;
}

/* IR-cut: edge-triggered solenoid. Pulse the master GPIO to flip filter state. */
static void cmd_daynight(struct state *s) {
    pin_set_output(IRCUT);
    int new_state = !s->ircut;
    pin_write(IRCUT, new_state);
    /* hold long enough for solenoid to actuate */
    msleep(80);
    /* leave the pin in the new state; vendor pattern is to keep the bit set */
    s->ircut = new_state;
}

static void cmd_status(const struct state *s) {
    printf("pan_pos=%d/%d   tilt_pos=%d/%d   ircut=%d   irled=%d\n",
           s->pan_pos, PAN_STEPS_MAX, s->tilt_pos, TILT_STEPS_MAX,
           s->ircut, s->irled);
}

/* Argument parsing --------------------------------------------------------- */

static int parse_int(const char *s) {
    char *end; long v = strtol(s, &end, 10);
    if (*end) { fprintf(stderr,"bad integer: %s\n",s); exit(2); }
    return (int)v;
}

static void usage(void) {
    fprintf(stderr,
        "usage:\n"
        "  ptz pan <+N|-N|abs N> [phase_ms]\n"
        "  ptz tilt <+N|-N|abs N> [phase_ms]\n"
        "  ptz home [phase_ms]\n"
        "  ptz ir on|off\n"
        "  ptz daynight\n"
        "  ptz status\n"
        "  ptz raw set <bank> <pin> <0|1>     (debug)\n"
        "default phase_ms = 3\n");
    exit(2);
}

int main(int argc, char **argv) {
    if (argc < 2) usage();
    struct state s; state_load(&s);

    if (!strcmp(argv[1], "pan")) {
        if (argc < 3) usage();
        long pms = (argc >= 4 && strcmp(argv[2],"abs")) ? parse_int(argv[3]) : 3;
        int delta;
        if (!strcmp(argv[2], "abs")) {
            if (argc < 4) usage();
            delta = parse_int(argv[3]) - s.pan_pos;
            pms = (argc >= 5) ? parse_int(argv[4]) : 3;
        } else delta = parse_int(argv[2]);
        int real = cmd_pan(&s, delta, pms);
        printf("pan: %d steps (requested %d, clamped) → pos=%d\n", real, delta, s.pan_pos);
    }
    else if (!strcmp(argv[1], "tilt")) {
        if (argc < 3) usage();
        long pms = (argc >= 4 && strcmp(argv[2],"abs")) ? parse_int(argv[3]) : 3;
        int delta;
        if (!strcmp(argv[2], "abs")) {
            if (argc < 4) usage();
            delta = parse_int(argv[3]) - s.tilt_pos;
            pms = (argc >= 5) ? parse_int(argv[4]) : 3;
        } else delta = parse_int(argv[2]);
        int real = cmd_tilt(&s, delta, pms);
        printf("tilt: %d steps (requested %d, clamped) → pos=%d\n", real, delta, s.tilt_pos);
    }
    else if (!strcmp(argv[1], "home")) {
        long pms = (argc >= 3) ? parse_int(argv[2]) : 4;
        cmd_home(&s, pms);
        puts("home: reset to (0,0)");
    }
    else if (!strcmp(argv[1], "ir")) {
        if (argc < 3) usage();
        cmd_irled(&s, !strcmp(argv[2], "on"));
        printf("ir: %s\n", s.irled ? "on" : "off");
    }
    else if (!strcmp(argv[1], "daynight")) {
        cmd_daynight(&s);
        printf("daynight: ircut=%d\n", s.ircut);
    }
    else if (!strcmp(argv[1], "status")) {
        cmd_status(&s);
    }
    else if (!strcmp(argv[1], "raw") && argc >= 6 && !strcmp(argv[2],"set")) {
        struct pin p = { (uint8_t)parse_int(argv[3]), (uint8_t)parse_int(argv[4]) };
        pin_set_output(p);
        pin_write(p, parse_int(argv[5]));
        printf("raw: bank=%u pin=%u val=%s\n", p.bank, p.pin, argv[5]);
    }
    else usage();

    state_save(&s);
    return 0;
}
