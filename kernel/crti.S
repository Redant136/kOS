.section .init
.global _init
.type _init, @function
_init:
#if defined(__x64__)
	push %rbp
	mov %esp, %ebp
#elif defined(__ix86__)
	push %ebp
	mov %esp, %ebp
#endif

	/* gcc will nicely put the contents of crtbegin.o's .init section here. */

.section .fini
.global _fini
.type _fini, @function
_fini:
#if defined(__x64__)
	push %rbp
	mov %esp, %ebp
#elif defined(__ix86__)
	push %ebp
	mov %esp, %ebp
#endif

	/* gcc will nicely put the contents of crtbegin.o's .fini section here. */
