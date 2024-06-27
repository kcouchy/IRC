# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/21 21:38:16 by atu               #+#    #+#              #
#    Updated: 2024/06/27 15:56:04 by aboyreau         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
CXXFLAGS += -Wall -Wextra -Werror -std=c++98

SRC =	main		\
		Server		\
		Client		\
		Channel		\
		PhoneBook	\
		InputErrors	\
		Messageable

SRCS = $(addsuffix .cpp,$(SRC))
OBJS = $(addsuffix .o,$(SRC))

all: $(NAME)

$(NAME): $(LIBS) $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(NAME) $(LDLIBS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
