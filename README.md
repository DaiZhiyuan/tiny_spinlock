# tiny_spinlock

spinlock implementation(From [Linux](https://elixir.bootlin.com/linux/v4.14.336/source/arch/arm64/include/asm/spinlock.h#L228)])

ARM64 implements spinlocks using a ticket lock mechanism:
- Each CPU attempting to acquire the lock receives a unique "ticket"
  (similar to a queue number), ensuring FIFO (First In, First Out) order.
- The currently held ticket is stored in `lock->owner`.
- A requesting CPU increments `lock->next` and waits until its ticket
matches `owner`, indicating that it has obtained the lock.

The implementation logic of arm64_spin_lock():
1. Read `lock->next` (to get a ticket).
2. Atomically increment `lock->next`, ensuring the ticket is unique.
3. If the current ticket matches `lock->owner`, the lock is acquired, and the CPU enters the critical section.
4. If the ticket does not match, the CPU waits in a loop (using `wfe` for low-power waiting) until `owner` updates.

arm64_spin_unlock() atomically releases the lock, allowing the next waiting CPU to acquire it and enter the critical section.
1. `lock->owner` holds the current ticket owner.
2. `lock->next` indicates the next waiting ticket.
3. Releasing the lock increments `owner`, allowing the next CPU with the adjacent ticket to enter the critical section.

## LL/SC（Load-Link/Store-Conditional）

```
● make
● make check
 96c:   885ffc62        ldaxr   w2, [x3]
 974:   88057c64        stxr    w5, w4, [x3]
 98c:   485ffc05        ldaxrh  w5, [x0]
program use load/store exclusives instruction

● ./bin/spinlock_test
Expected Value: 2560000
Real Value: 2560000
```

## LSE（Large System Extension）

```
● make -f Makefile.lse
● make -f Makefile.lse check
 96c:   b8a50062        ldadda  w5, w2, [x3]
program use LSE instruction

● ./bin/spinlock_test
Expected Value: 2560000
Real Value: 2560000
```

