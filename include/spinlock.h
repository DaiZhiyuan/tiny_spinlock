#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#include <stdint.h>

#define TICKET_SHIFT	16

typedef struct {
	uint16_t owner;
	uint16_t next;
} __attribute__((aligned(4))) arm64_spinlock_t;

static inline void arm64_spin_lock(arm64_spinlock_t *lock)
{
	unsigned int tmp;
	arm64_spinlock_t lockval, newval;

	asm volatile(
	/* Atomically increment the next ticket. */
#if !defined(USE_LSE)
	/* LL/SC */
"	prfm	pstl1strm, %3\n"
"1:	ldaxr	%w0, %3\n"
"	add	%w1, %w0, %w5\n"
"	stxr	%w2, %w1, %3\n"
"	cbnz	%w2, 1b\n"
#else
	/* LSE atomics */
"	mov	%w2, %w5\n"
"	ldadda	%w2, %w0, %3\n"
"	nop	\n"
"	nop	\n"
"	nop	\n"
#endif
	/* Did we get the lock? */
"	eor	%w1, %w0, %w0, ror #16\n"
"	cbz	%w1, 3f\n"
	/*
	 * No: spin on the owner. Send a local event to avoid missing an
	 * unlock before the exclusive load.
	 */
"	sevl\n"
"2:	wfe\n"
"	ldaxrh	%w2, %4\n"
"	eor	%w1, %w2, %w0, lsr #16\n"
"	cbnz	%w1, 2b\n"
	/* We got the lock. Critical section starts here. */
"3:"
	: "=&r" (lockval), "=&r" (newval), "=&r" (tmp), "+Q" (*lock)
	: "Q" (lock->owner), "I" (1 << TICKET_SHIFT)
	: "memory");
}

static inline void arm64_spin_unlock(arm64_spinlock_t *lock)
{
	unsigned long tmp;

	asm volatile(
#if !defined(USE_LSE)
	/* LL/SC */
	"	ldrh	%w1, %0\n"
	"	add	%w1, %w1, #1\n"
	"	stlrh	%w1, %0\n"
#else
	/* LSE atomics */
	"	mov	%w1, #1\n"
	"	staddlh	%w1, %0\n"
	"	nop \n"
#endif
	: "=Q" (lock->owner), "=&r" (tmp)
	:
	: "memory");
}

#endif
