# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    srcs.mk                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mbrousse <mbrousse@student.42lyon.fr>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/10/21 15:46:19 by mpitot            #+#    #+#              #
#    Updated: 2024/10/30 16:20:14 by mbrousse         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Sources ******************************************************************** #

SRCS			=	$(MAIN)			\
					$(SERV)			\
					$(CLIENT)		\
					$(LOG)			\
					$(CHAN)			\

SRC_MAIN		=	main.cpp
SRC_SERV		=	Server.cpp
SRC_CLIENT		=	Client.cpp
SRC_LOG			=	Log.cpp
SRC_CHAN		=	Channel.cpp

MAIN			=	$(addprefix $(DIR_MAIN), $(SRC_MAIN))
SERV			=	$(addprefix $(DIR_SERV), $(SRC_SERV))
CLIENT			=	$(addprefix $(DIR_CLIENT), $(SRC_CLIENT))
LOG				=	$(addprefix $(DIR_LOG), $(SRC_LOG))
CHAN				=	$(addprefix $(DIR_CHAN), $(SRC_CHAN))

# Directories **************************************************************** #
SRC_D			=	srcs/
DIR_MAIN		=
DIR_SERV		=	server/
DIR_CLIENT		=	client/
DIR_LOG			=	logs/
DIR_CHAN		=	channels/

# Headers ******************************************************************** #
HEAD			=	-Iincludes				\
					-I$(SRC_D)$(DIR_SERV)	\
					-I$(SRC_D)$(DIR_CLIENT)	\
					-I$(SRC_D)$(DIR_LOG)	\
					-I$(SRC_D)$(DIR_CHAN)	\

