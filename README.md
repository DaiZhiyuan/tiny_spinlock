# tiny_spinlock

spinlock implementation(From [Linux](https://elixir.bootlin.com/linux/v4.14.336/source/arch/arm64/include/asm/spinlock.h#L228)])

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
