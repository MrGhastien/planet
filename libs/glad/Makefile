
libglad.a: src/glad.o
	@echo -e "	\e[35mAR	$(notdir $@)\e[0m"
	@$(AR) rc $@ $^

%.o: %.c include/glad/glad.h include/KHR/khrplatform.h
	@echo -e "	\e[36mCC	$(notdir $@)\e[0m"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<
