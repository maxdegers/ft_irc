include .config/srcs.mk
include .config/display.mk
include .config/colors.mk

OBJS	=	$(SRCS:%.cpp=${OBJ_D}%.o)
SRC_D	=	srcs/
OBJ_D	=	.objs/
NAME	=	ircserv
CC		=	c++
CFLAGS	=	-Wall -Wextra -Werror -std=c++98 -g3
DFLAGS	=	-MMD -MF $(@:.o=.d)

AUTHORS	=	mpitot, gberthol, mbrousse
DEBUT	=	20/01/2025
FIN		=	...

all		:	header .internal_separate1 ${NAME}

${OBJS}	:	${OBJ_D}%.o: ${SRC_D}%.cpp Makefile
	@$(call print_progress,$<)
	@${CC} ${CFLAGS} ${DFLAGS} ${HEAD} -c $< -o $@
	@$(call update_progress,$<)

-include $(OBJS:.o=.d)
${NAME}	:	${OBJ_D} ${OBJS}
	@$(call print_progress,$(NAME))
	@${CC} ${CFLAGS} ${OBJS} ${HEAD} -o ${NAME}
	@$(eval CHANGED=1)
	@$(call erase)
	@$(call done_and_dusted,$(NAME))

${OBJ_D}:
	@mkdir -p ${OBJ_D}
	@mkdir -p ${OBJ_D}/client
	@mkdir -p ${OBJ_D}/server

clean	:
	@echo "Cleaning $(WHITE)[$(RED)$(NAME)$(WHITE)]...$(DEFAULT)"
	@rm -rf ${OBJ_D}
	@echo "$(WHITE)[$(RED)$(OBJ_D)$(WHITE)] $(RED)deleted.$(DEFAULT)"

fclean	:
	@echo "F***ing-Cleaning $(WHITE)[$(RED)$(NAME)$(WHITE)]...$(DEFAULT)"
	@rm -rf ${OBJ_D}
	@echo "$(WHITE)[$(RED)$(OBJ_D)$(WHITE)] $(RED)deleted.$(DEFAULT)"
	@rm -f ${NAME}
	@echo "$(WHITE)[$(RED)$(NAME)$(WHITE)] $(RED)deleted.$(DEFAULT)"

header:
	@printf "  __ _  $(GREEN_B)  ___________  _____\n$(DEFAULT)"
	@printf " / _| | $(GREEN_B) |_   _| ___ \\/  __ \\\\\n$(DEFAULT)"
	@printf "| |_| |_$(GREEN_B)   | | | |_/ /| /  \\/\n$(DEFAULT)"
	@printf "|  _| __| $(GREEN_B) | | |    / | |\n$(DEFAULT)"
	@printf "| | | |_ $(GREEN_B) _| |_| |\\ \ | \\__/\\\\\n$(DEFAULT)"
	@printf "|_|  \\__|.$(GREEN_B)\___/\\_| \\_| \\____/\n$(DEFAULT)"
	@echo
	@printf "%b" "$(GREEN_B)Name:		$(WHITE)$(NAME)\n"
	@printf "%b" "$(GREEN_B)Authors:	$(WHITE)$(AUTHORS)\n"
	@printf "%b" "$(GREEN_B)Date: 		$(WHITE)$(DEBUT) $(WHITE_B)-> $(WHITE)$(FIN)\n$(DEFAULT)"
	@printf "%b" "$(GREEN_B)CC: 		$(WHITE)$(CC)\n$(DEFAULT)"
	@printf "%b" "$(GREEN_B)Flags: 		$(WHITE)$(CFLAGS)\n$(DEFAULT)"

leak: all .internal_separate3
	@echo "$(MAGENTA)Valgrind $(WHITE)~ $(YELLOW)Flags:$(DEFAULT)"
	@echo "   $(YELLOW)-$(DEFAULT)Show Leak Kinds"
	@echo "   $(YELLOW)-$(DEFAULT)Track FDs"
	@echo "   $(YELLOW)-$(DEFAULT)Show Mismatched Frees"
	@echo "   $(YELLOW)-$(DEFAULT)Read Variable Information"
	@echo "   $(YELLOW)-$(DEFAULT)Leak check"
	@$(call separator)
	@valgrind		--show-leak-kinds=all		\
					--track-fds=yes 			\
					--show-mismatched-frees=yes	\
					--read-var-info=yes			\
					--leak-check=full			\
					./$(NAME)

re		:	header .internal_separate1 fclean .internal_separate2 ${NAME}

.PHONY	:	all clean fclean re leak

.NOTPARALLEL all:
	@if [ $(CHANGED) -eq 0 ]; then \
		echo "$(YELLOW)Nothing to be done for $(WHITE)[$(CYAN)$(NAME)$(WHITE)].$(DEFAULT)"; \
	fi

.internal_announce	:
	@echo "$(YELLOW)Compiling $(WHITE)[$(CYAN)${NAME}$(WHITE)]...$(DEFAULT)"
