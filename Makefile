# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                                  +**+   +*  *    #
#                                                     +:+ +:+         +:+      #
#    By: kcouchma <kcouchma@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/10/21 21:38:16 by kiri              #+#    #+#              #
#    Updated: 2024/07/17 12:31:48 by aboyreau          +#-.-*  +         *     #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
CXXFLAGS += -Wall -Wextra -Werror -std=c++98 -I include

SRC =	main		\
		Server		\
		Client		\
		ClientParser\
		Channel		\
		PhoneBook	\
		InputErrors	\
		Messageable

SRCS = $(addprefix srcs/, $(addsuffix .cpp,$(SRC)))
OBJS = $(addprefix objs/, $(addsuffix .o,$(SRC)))

all: $(NAME)

$(NAME): $(LIBS) $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $(NAME) $(LDLIBS)

objs/%.o: srcs/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all
