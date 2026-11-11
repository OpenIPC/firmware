/* SPDX-License-Identifier: MIT */
/* Minimal ARC600 RPMsg remote: split virtqueues, NS and one echo endpoint.
 * No RTOS, stack, interrupt enable, DMA or audio peripheral access.
 * All shared data accesses bypass D-cache; SYNC orders publication.
 * Linux stops the core by holding it in reset.
 * Doorbell to the ARM: bit 0 for the vrings, bit 1 for a fatal error, kept
 * set until the reset.
 */
        .cpu arc600
        .equ SHARED_START, 0xa3fc0000
        .equ SHARED_END,   0xa4000000
        .equ RING_BYTES,   8192
        .equ RING_AVAIL,   512
        .equ RING_USED,    4096
        .equ ENDPOINT,     1024
        .equ PMU_TO_ARM,   0xf00001f0
        .equ PMU_FROM_ARM, 0xf00001e4
        .equ DB_VRING,     1
        .equ DB_CRASH,     2

        .section .vectors,"ax",@progbits
        .balign 8
        .global arc_vectors
arc_vectors:
        .org 0
        j arc_start
        .set slot, 1
        .rept 31
        .org slot * 8
        j arc_trap
        .set slot, slot + 1
        .endr
        .org 0x100

        .section .text,"ax",@progbits
        .balign 4
        .global arc_start
arc_start:
        mov r25, firmware_diagnostics
        mov r26, resource_table
        mov r0, 1
        st.di r0, [r25, 0]       /* BOOT */
        mov r0, 0
        st.di r0, [r25, 4]       /* Error code. */
        st.di r0, [r25, 8]       /* Remote TX completions. */
        st.di r0, [r25, 12]      /* Remote RX completions. */
        st.di r0, [r25, 24]      /* ARM kicks observed. */
        mov r20, 0              /* Queue0 consumed avail index. */
        mov r21, 0              /* Queue1 consumed avail index. */
        mov r22, 0              /* Queue0 produced used index. */
        mov r23, 0              /* Queue1 produced used index. */
        sync
        mov r0, 2
        st.di r0, [r25, 0]       /* WAIT_DRIVER_OK */
.Lwait_driver:
        ldb.di r0, [r26, 44]
        and r1, r0, 0xc0
        brne r1, 0, .Lwait_driver
        and r0, r0, 4
        breq r0, 0, .Lwait_driver
        sync
        mov r13, 7
        ld.di r0, [r26, 36]      /* Negotiated gfeatures: NS only. */
        cmp r0, 1
        bne .Lfatal
        mov r13, 1
        ld.di r0, [r26, 52]      /* Queue0 alignment. */
        cmp r0, 4096
        bne .Lfatal
        ld.di r0, [r26, 72]
        cmp r0, 4096
        bne .Lfatal
        ld.di r0, [r26, 56]
        cmp r0, 32
        bne .Lfatal
        ld.di r0, [r26, 76]
        cmp r0, 32
        bne .Lfatal
        ld.di r16, [r26, 48]     /* Queue0: remote TX, Linux rvq. */
        ld.di r17, [r26, 68]     /* Queue1: remote RX, Linux svq. */
        mov r13, 2
        and r0, r16, 4095
        cmp r0, 0
        bne .Lfatal
        and r0, r17, 4095
        cmp r0, 0
        bne .Lfatal
        cmp r16, SHARED_START
        blo .Lfatal
        cmp r17, SHARED_START
        blo .Lfatal
        cmp r16, SHARED_END - RING_BYTES
        bhi .Lfatal
        cmp r17, SHARED_END - RING_BYTES
        bhi .Lfatal
        add r0, r16, RING_BYTES
        cmp r0, r17
        bls .Lrings_disjoint
        add r0, r17, RING_BYTES
        cmp r0, r16
        bhi .Lfatal
.Lrings_disjoint:
        /* Fresh virtio rings must have no device-produced used entries. */
        mov r13, 6
        add r0, r16, RING_USED
        ldw.di r0, [r0, 2]
        cmp r0, 0
        bne .Lfatal
        add r0, r17, RING_USED
        ldw.di r0, [r0, 2]
        cmp r0, 0
        bne .Lfatal
        ld.di r0, [r26, 60]
        st.di r0, [r25, 16]      /* Linux-assigned queue0 notifyid. */
        ld.di r0, [r26, 80]
        st.di r0, [r25, 20]      /* Linux-assigned queue1 notifyid. */
        mov r0, 3
        st.di r0, [r25, 0]       /* WAIT_NS_BUFFER */

.Lannounce:
        bl .Lcheck_status
        bl .Lack_kick
        mov r0, r16
        mov r1, r20
        mov r2, 2               /* VRING_DESC_F_WRITE. */
        bl .Lget_descriptor
        breq r6, 0, .Lannounce
        mov r13, 4
        cmp r5, 56
        blo .Lfatal
        mov r14, r3             /* Descriptor id across byte copy. */
        mov r7, ns_message
        mov r8, r4
        mov r9, 56
        bl .Lcheck_status
.Lns_copy:
        ldb.di r0, [r7]
        stb.di r0, [r8]
        add r7, r7, 1
        add r8, r8, 1
        sub r9, r9, 1
        brne r9, 0, .Lns_copy
        mov r0, r16
        mov r1, r22
        mov r2, r14
        mov r3, 56
        bl .Lpublish_used
        mov r22, r1
        add r20, r20, 1
        and r20, r20, 65535
        st.di r20, [r25, 8]
        mov r0, 4
        st.di r0, [r25, 0]       /* ECHO_RUNNING */

.Lpoll:
        bl .Lcheck_status
        bl .Lack_kick
        mov r0, r17
        mov r1, r21
        mov r2, 0               /* Host TX buffers are device-readable. */
        bl .Lget_descriptor
        breq r6, 0, .Lpoll
        mov r14, r3             /* RX descriptor id. */
        mov r15, r4             /* RX packet address. */
        mov r13, 5
        ldw.di r18, [r15, 12]    /* Payload size. */
        add r19, r18, 16
        cmp r19, r5
        bhi .Lfatal
        ldw.di r0, [r15, 14]     /* RPMsg flags are reserved/zero. */
        cmp r0, 0
        bne .Lfatal
        ld.di r0, [r15, 4]
        cmp r0, ENDPOINT
        bne .Lconsume_rx         /* Unknown endpoint: bounded drop. */

.Lwait_tx:
        bl .Lcheck_status
        bl .Lack_kick
        mov r0, r16
        mov r1, r20
        mov r2, 2
        bl .Lget_descriptor
        breq r6, 0, .Lwait_tx
        mov r13, 4
        cmp r19, r5
        bhi .Lfatal
        /* Reject TX capacity overlapping the still-owned RX packet. */
        add r6, r15, r19
        cmp r4, r6
        bhs .Lbuffers_disjoint
        add r6, r4, r5
        cmp r15, r6
        blo .Lfatal
.Lbuffers_disjoint:
        mov r7, r4              /* TX packet address. */
        mov r8, r3              /* TX descriptor id. */
        bl .Lcheck_status
        mov r0, ENDPOINT
        st.di r0, [r7, 0]
        ld.di r0, [r15, 0]
        st.di r0, [r7, 4]       /* Echo to the sender endpoint. */
        mov r0, 0
        st.di r0, [r7, 8]
        st.di r18, [r7, 12]      /* u16 len + zero u16 flags. */
        add r10, r15, 16
        add r11, r7, 16
        mov r12, r18
        breq r12, 0, .Lecho_copied
.Lecho_copy:
        ldb.di r0, [r10]
        stb.di r0, [r11]
        add r10, r10, 1
        add r11, r11, 1
        sub r12, r12, 1
        brne r12, 0, .Lecho_copy
.Lecho_copied:
        mov r0, r16
        mov r1, r22
        mov r2, r8
        mov r3, r19
        bl .Lpublish_used
        mov r22, r1
        add r20, r20, 1
        and r20, r20, 65535
        st.di r20, [r25, 8]
.Lconsume_rx:
        mov r0, r17
        mov r1, r23
        mov r2, r14
        mov r3, 0               /* Device read, no bytes written to RX. */
        bl .Lpublish_used
        mov r23, r1
        add r21, r21, 1
        and r21, r21, 65535
        st.di r21, [r25, 12]
        b .Lpoll

/* Check status without touching shared queues; preserve all work registers. */
.Lcheck_status:
        ldb.di r27, [r26, 44]
        and r27, r27, 0xc4
        cmp r27, 4
        bne .Lwithdrawn
        sync
        j [blink]

/* Polling is deliberate: no ARC IRQ handler or global IRQ enable is needed. */
.Lack_kick:
        mov r27, PMU_FROM_ARM
        ld.di r28, [r27]
        breq r28, 0, .Lack_return
        mov r28, 0
        st.di r28, [r27]
        sync
        ld.di r28, [r25, 24]
        add r28, r28, 1
        st.di r28, [r25, 24]
.Lack_return:
        j [blink]

/* r0=ring, r1=last avail, r2=required flags; r3=id,r4=buffer,r5=len,
 * r6=0 if empty else1. Clobber r7..r13 only; no nested helper calls.
 */
.Lget_descriptor:
        ldb.di r27, [r26, 44]
        and r27, r27, 0xc4
        cmp r27, 4
        bne .Lwithdrawn
        add r7, r0, RING_AVAIL
        ldw.di r8, [r7, 2]
        sub r8, r8, r1
        and r8, r8, 65535
        mov r6, 0
        breq r8, 0, .Ldescriptor_return
        mov r13, 6
        cmp r8, 32
        bhi .Lfatal
        sync
        and r8, r1, 31
        asl r8, r8, 1
        add r7, r7, r8
        ldw.di r3, [r7, 4]
        mov r13, 3
        cmp r3, 32
        bhs .Lfatal
        asl r7, r3, 4
        add r7, r7, r0
        ldw.di r8, [r7, 12]
        cmp r8, r2              /* Reject NEXT/INDIRECT/unknown bits. */
        bne .Lfatal
        ld.di r8, [r7, 4]       /* Descriptor address high word. */
        cmp r8, 0
        bne .Lfatal
        ld.di r4, [r7, 0]
        ld.di r5, [r7, 8]
        mov r13, 4
        cmp r5, 16
        blo .Lfatal
        cmp r5, 512
        bhi .Lfatal
        and r8, r4, 3
        cmp r8, 0
        bne .Lfatal
        cmp r4, SHARED_START
        blo .Lfatal
        cmp r4, SHARED_END
        bhs .Lfatal
        add r9, r4, r5
        cmp r9, SHARED_END
        bhi .Lfatal
        /* Payload ranges must not alias either ring allocation. */
        add r8, r16, RING_BYTES
        cmp r4, r8
        bhs .Lbuffer_not_txring
        cmp r9, r16
        bhi .Lfatal
.Lbuffer_not_txring:
        add r8, r17, RING_BYTES
        cmp r4, r8
        bhs .Lbuffer_not_rxring
        cmp r9, r17
        bhi .Lfatal
.Lbuffer_not_rxring:
        mov r6, 1
.Ldescriptor_return:
        j [blink]

/* r0=ring,r1=used index,r2=id,r3=length; returns incremented u16 r1.
 * Clobbers r4..r6,r27/r28. SYNC orders packet and used element before idx.
 * avail.NO_INTERRUPT is advisory; respect it without EVENT_IDX negotiation.
 */
.Lpublish_used:
        ldb.di r27, [r26, 44]
        and r27, r27, 0xc4
        cmp r27, 4
        bne .Lwithdrawn
        add r4, r0, RING_USED
        and r5, r1, 31
        asl r5, r5, 3
        add r5, r5, r4
        st.di r2, [r5, 4]
        st.di r3, [r5, 8]
        sync
        add r1, r1, 1
        and r1, r1, 65535
        stw.di r1, [r4, 2]
        sync
        add r5, r0, RING_AVAIL
        ldw.di r6, [r5]
        and r6, r6, 1
        brne r6, 0, .Lpublish_return
        mov r27, PMU_TO_ARM
        ld.di r28, [r27]
        or r28, r28, DB_VRING
        st.di r28, [r27]
        sync
.Lpublish_return:
        j [blink]

.Lwithdrawn:
        sync
        mov r0, 5
        st.di r0, [r25, 0]       /* DRIVER_OK withdrawn: wait for reset. */
.Lwithdrawn_wait:
        b .Lwithdrawn_wait
.Lfatal:
        mov r0, 0x80
        b .Lreport_fault
        .global arc_trap
arc_trap:
        mov r25, firmware_diagnostics
        mov r0, 0x81
        mov r13, 0x100
.Lreport_fault:
        st.di r13, [r25, 4]      /* Error code. */
        st.di r0, [r25, 0]       /* FATAL or TRAP. */
        sync
        /* The ARM clears its bits with a read-modify-write that can drop
         * one set meanwhile, so set the crash bit again whenever it is clear.
         */
        mov r27, PMU_TO_ARM
.Lfault_wait:
        ld.di r28, [r27]
        and r0, r28, DB_CRASH
        brne r0, 0, .Lfault_wait
        or r28, r28, DB_CRASH
        st.di r28, [r27]
        sync
        b .Lfault_wait

        .section .rodata,"a",@progbits
        .balign 4
ns_message:
        .long ENDPOINT, 53, 0
        .short 40, 0
        .asciz "rpmsg-arc-echo"
        .zero 17
        .long ENDPOINT, 0        /* RPMSG_NS_CREATE */

        .section .resource_table,"aw",@progbits
        .balign 4
        .global resource_table
resource_table:
        .long 1, 1, 0, 0        /* Version, entries, reserved[2]. */
        .long 20                /* Offset to RSC_VDEV header. */
        .long 3                 /* RSC_VDEV */
        .long 7, 0, 1, 0, 0     /* RPMsg id, notifyid, NS, gfeatures, config. */
        .byte 0, 2, 0, 0        /* Status, two vrings, reserved. */
        .long 0xffffffff, 4096, 32, 0xffffffff, 0
        .long 0xffffffff, 4096, 32, 0xffffffff, 0

        .section .diagnostics,"aw",@progbits
        .balign 4
        .global firmware_diagnostics
firmware_diagnostics:
        .zero 64
