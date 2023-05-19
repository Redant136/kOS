/* x86 crtn.s */
.section .init
	/* gcc will nicely put the contents of crtend.o's .init section here. */
#if defined(__x64__)
	pop %rbp
#elif defined(__ix86__)
	pop %ebp
#endif

	ret

.section .fini
	/* gcc will nicely put the contents of crtend.o's .fini section here. */
#if defined(__x64__)
	pop %rbp
#elif defined(__ix86__)
	pop %ebp
#endif
	ret
